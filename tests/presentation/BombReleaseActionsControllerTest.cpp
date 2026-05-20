#include <gtest/gtest.h>
#include "application/ScenarioState.h"
#include "application/ScenarioQueries.h"
#include "presentation/BombReleaseController.h"
#include "presentation/BombReleaseActionsController.h"
#include "presentation/TrackSummaryBuilder.h"

using namespace presentation;

namespace {

struct Fixture {
  ScenarioState          state;
  BombReleaseController* bombCtrl = nullptr;
  QString   lastStatus;
  QString   lastLog;
  bool      synced      = false;
  bool      isRunning   = true;
  bool      isPickPending = false;
  bool      coordPickStarted = false;
  QString   selectedName;
  QString   pickItemReturn;
  bool      pickItemOk  = true;

  BombReleaseController* makeBombCtrl() {
    return new BombReleaseController(
        &state,
        [this](const QString& m) { lastLog = m; },
        [this](const QString& m) { lastStatus = m; },
        [this]() { synced = true; },
        [](const QString&, bool) {},
        [](const QString&) {},
        nullptr);
  }

  BombReleaseActionsController* makeController(BombReleaseController* bc) {
    return new BombReleaseActionsController(
        bc,
        [this]() { return selectedName; },
        [this](const QString& name) -> const Entity* {
          for (const Entity& e : state.entities()) {
            if (e.name == name) return &e;
          }
          return nullptr;
        },
        [this]() { return isRunning; },
        [this]() { return isPickPending; },
        [this]() { coordPickStarted = true; },
        [this](const Entity& launcher) {
          return application::validBombReleaseTargets(&state, launcher);
        },
        [](const Entity& e) { return presentation::bombTargetDisplayLabel(e); },
        [this](const QString&, const QString&,
               const QStringList&, bool& ok) -> QString {
          ok = pickItemOk;
          return pickItemReturn;
        },
        [this](const QString& m) { lastStatus = m; },
        [this](const QString& m) { lastLog = m; },
        [this]() { synced = true; });
  }

  Entity addBomber(const QString& name, int bombs = 2) {
    Entity e;
    e.name   = name;
    e.domain = QStringLiteral("Air");
    e.category = QStringLiteral("Bomber");
    e.forceIdentifier = 1;
    e.latitude  = 40.0;
    e.longitude = -3.0;
    e.altitude  = 5000;
    WeaponInventoryItem w; w.weaponType = QStringLiteral("Bomb"); w.quantity = bombs;
    e.weapons = {w};
    state.addEntity(e);
    return e;
  }

  Entity addGroundTarget(const QString& name) {
    Entity e;
    e.name   = name;
    e.domain = QStringLiteral("Surface");
    e.category = QStringLiteral("Armored");
    e.forceIdentifier = 2; // opposing — valid target for forceId=1 launcher
    e.latitude  = 40.1;
    e.longitude = -3.1;
    e.altitude  = 0;
    state.addEntity(e);
    return e;
  }
};

} // namespace

// ── releaseBombAtSurfaceEntity ───────────────────────────────────────────────

TEST(BombReleaseActionsController, releaseQueuesWithValidTarget) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber1"));
  f.addGroundTarget(QStringLiteral("Target1"));
  f.selectedName   = QStringLiteral("Bomber1");
  f.pickItemReturn = QStringLiteral("Target1 (Opposing / Surface)");
  f.pickItemOk     = true;
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtSurfaceEntity();

  // pending bomb release should be queued
  EXPECT_TRUE(bc->pendingRelease().pending);
  EXPECT_EQ(bc->pendingRelease().launcherEntityName, QStringLiteral("Bomber1"));
  delete ctrl;
  delete bc;
}

TEST(BombReleaseActionsController, releaseAbortedWhenNoBombs) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber2"), 0); // 0 bombs
  f.selectedName = QStringLiteral("Bomber2");
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtSurfaceEntity();

  EXPECT_FALSE(bc->pendingRelease().pending);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
  delete bc;
}

TEST(BombReleaseActionsController, releaseAbortedWhenSimNotRunning) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber3"));
  f.selectedName = QStringLiteral("Bomber3");
  f.isRunning    = false;
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtSurfaceEntity();

  EXPECT_FALSE(bc->pendingRelease().pending);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
  delete bc;
}

TEST(BombReleaseActionsController, releaseAbortedWhenNoTargets) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber4")); // no opposing targets added
  f.selectedName = QStringLiteral("Bomber4");
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtSurfaceEntity();

  EXPECT_FALSE(bc->pendingRelease().pending);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
  delete bc;
}

TEST(BombReleaseActionsController, releaseAbortedWhenUserCancelsDialog) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber5"));
  f.addGroundTarget(QStringLiteral("Target5"));
  f.selectedName = QStringLiteral("Bomber5");
  f.pickItemOk   = false;
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtSurfaceEntity();

  EXPECT_FALSE(bc->pendingRelease().pending);
  delete ctrl;
  delete bc;
}

// ── releaseBombAtCustomCoordinates ───────────────────────────────────────────

TEST(BombReleaseActionsController, customCoordsStartsPickMode) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber6"));
  f.selectedName = QStringLiteral("Bomber6");
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtCustomCoordinates();

  EXPECT_TRUE(bc->isPickingMode());
  EXPECT_TRUE(f.coordPickStarted);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
  delete bc;
}

TEST(BombReleaseActionsController, customCoordsAbortedWhenPickAlreadyPending) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber7"));
  f.selectedName  = QStringLiteral("Bomber7");
  f.isPickPending = true;
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->releaseBombAtCustomCoordinates();

  EXPECT_FALSE(bc->isPickingMode());
  EXPECT_FALSE(f.coordPickStarted);
  delete ctrl;
  delete bc;
}

// ── cancelPendingBombRelease ─────────────────────────────────────────────────

TEST(BombReleaseActionsController, cancelClearsRelease) {
  Fixture f;
  f.addBomber(QStringLiteral("Bomber8"));
  f.addGroundTarget(QStringLiteral("Target8"));
  auto* bc = f.makeBombCtrl();
  bc->queue(QStringLiteral("Bomber8"), 40.0, -3.0, 0.0,
            QStringLiteral("Target8"), QStringLiteral("Surface Entity"),
            QStringLiteral("Target8"), false, false);
  ASSERT_TRUE(bc->pendingRelease().pending);

  auto* ctrl = f.makeController(bc);
  f.selectedName = QStringLiteral("Bomber8");

  ctrl->cancelPendingBombRelease();

  EXPECT_FALSE(bc->pendingRelease().pending);
  EXPECT_FALSE(f.lastLog.isEmpty());
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
  delete bc;
}

TEST(BombReleaseActionsController, cancelWithNoPendingReportsStatus) {
  Fixture f;
  auto* bc   = f.makeBombCtrl();
  auto* ctrl = f.makeController(bc);

  ctrl->cancelPendingBombRelease();

  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
  delete bc;
}
