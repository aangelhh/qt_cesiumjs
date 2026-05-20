#include <gtest/gtest.h>
#include "application/ScenarioState.h"
#include "presentation/EntityStateActionsController.h"
#include "presentation/EntityVisualStateManager.h"

using namespace presentation;

// ── Fixture ───────────────────────────────────────────────────────────────────

namespace {

struct Fixture {
  ScenarioState state;
  EntityVisualStateManager visualState{QString{}};
  QString   lastLog;
  QString   lastStatus;
  bool      synced       = false;
  bool      dialogClosed = false;
  QString   selectedName;
  bool      isDestroyed  = false;
  bool      isOperable   = true;
  bool      flyTargetsOk = true;
  double    flyHeading   = 90.0;
  int       flyAlt       = 5000;
  double    flySpeed     = 300.0;
  double    askDoubleRet = 0.0;
  bool      askDoubleOk  = true;

  EntityStateActionsController* makeController() {
    return new EntityStateActionsController(
        &state,
        &visualState,
        [this]() { return selectedName; },
        [this]() { return isDestroyed; },
        [this]() { return isOperable; },
        [this](const QString&, const EntityTask&, bool) -> bool {
          synced = true;
          return true;
        },
        [this](double& h, int& a, double& s) -> bool {
          h = flyHeading; a = flyAlt; s = flySpeed;
          return flyTargetsOk;
        },
        [this](const QString&, const QString&,
               double, double, double, bool& ok) -> double {
          ok = askDoubleOk;
          return askDoubleRet;
        },
        [this]() { dialogClosed = true; },
        [this](const QString& m) { lastLog = m; },
        [this](const QString& m) { lastStatus = m; },
        [this]() { synced = true; });
  }

  Entity addEntity(const QString& name) {
    Entity e;
    e.name   = name;
    e.domain = QStringLiteral("Air");
    e.category = QStringLiteral("Fighter");
    e.forceIdentifier = 1;
    state.addEntity(e);
    return e;
  }
};

} // namespace

// ── setSelectedDestroyed ─────────────────────────────────────────────────────

TEST(EntityStateActionsController, destroySelectedEntity) {
  Fixture f;
  f.addEntity(QStringLiteral("Alpha"));
  f.selectedName = QStringLiteral("Alpha");
  auto* ctrl = f.makeController();

  ctrl->destroySelected();

  const Entity* e = nullptr;
  for (const Entity& ent : f.state.entities()) {
    if (ent.name == QStringLiteral("Alpha")) { e = &ent; break; }
  }
  ASSERT_NE(e, nullptr);
  EXPECT_TRUE(e->destroyed);
  EXPECT_TRUE(f.synced);
  EXPECT_FALSE(f.lastLog.isEmpty());
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(EntityStateActionsController, restoreSelectedEntity) {
  Fixture f;
  f.addEntity(QStringLiteral("Bravo"));
  f.selectedName = QStringLiteral("Bravo");
  // destroy first
  f.state.setEntityDestroyed(QStringLiteral("Bravo"), true);
  auto* ctrl = f.makeController();

  ctrl->restoreSelected();

  const Entity* e = nullptr;
  for (const Entity& ent : f.state.entities()) {
    if (ent.name == QStringLiteral("Bravo")) { e = &ent; break; }
  }
  ASSERT_NE(e, nullptr);
  EXPECT_FALSE(e->destroyed);
  delete ctrl;
}

TEST(EntityStateActionsController, destroyWithNoSelectionDoesNothing) {
  Fixture f;
  f.selectedName = QString{};  // nothing selected
  auto* ctrl = f.makeController();
  ctrl->destroySelected();
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(EntityStateActionsController, destroyClosesTaskDialog) {
  Fixture f;
  f.addEntity(QStringLiteral("Charlie"));
  f.selectedName = QStringLiteral("Charlie");
  auto* ctrl = f.makeController();
  ctrl->destroySelected();
  EXPECT_TRUE(f.dialogClosed);
  delete ctrl;
}

// ── setSelectedBehaviorMode ──────────────────────────────────────────────────

TEST(EntityStateActionsController, setBehaviorModeUpdatesEntity) {
  Fixture f;
  f.addEntity(QStringLiteral("Delta"));
  f.selectedName = QStringLiteral("Delta");
  f.isDestroyed  = false;
  auto* ctrl = f.makeController();

  ctrl->setSelectedBehaviorMode(QStringLiteral("Autonomous"));

  EXPECT_TRUE(f.synced);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(EntityStateActionsController, setBehaviorModeSkipsDestroyedEntity) {
  Fixture f;
  f.addEntity(QStringLiteral("Echo"));
  f.selectedName = QStringLiteral("Echo");
  f.isDestroyed  = true;
  auto* ctrl = f.makeController();
  ctrl->setSelectedBehaviorMode(QStringLiteral("Autonomous"));
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

// ── setSelectedHidden ────────────────────────────────────────────────────────

TEST(EntityStateActionsController, hideEntitySetsVisualFlag) {
  Fixture f;
  f.addEntity(QStringLiteral("Fox"));
  f.selectedName = QStringLiteral("Fox");
  auto* ctrl = f.makeController();

  ctrl->setSelectedHidden(true);

  EXPECT_TRUE(f.visualState.stateFor(
      QStringLiteral("Fox")).hidden);
  EXPECT_TRUE(f.synced);
  delete ctrl;
}

TEST(EntityStateActionsController, showEntityClearsVisualFlag) {
  Fixture f;
  f.addEntity(QStringLiteral("Golf"));
  f.selectedName = QStringLiteral("Golf");
  f.visualState.setFlag(QStringLiteral("Golf"),
                        EntityVisualStateManager::Flag::Hidden, true);
  auto* ctrl = f.makeController();

  ctrl->setSelectedHidden(false);

  EXPECT_FALSE(f.visualState.stateFor(
      QStringLiteral("Golf")).hidden);
  delete ctrl;
}

// ── setSelectedRadarCoverageVisible ──────────────────────────────────────────

TEST(EntityStateActionsController, enableRadarCoverage) {
  Fixture f;
  f.addEntity(QStringLiteral("Hotel"));
  f.selectedName = QStringLiteral("Hotel");
  auto* ctrl = f.makeController();

  ctrl->setSelectedRadarCoverageVisible(true);

  EXPECT_TRUE(f.visualState.stateFor(
      QStringLiteral("Hotel")).radarCoverageVisible);
  EXPECT_TRUE(f.synced);
  delete ctrl;
}

// ── setSelectedTrackHistoryVisible ───────────────────────────────────────────

TEST(EntityStateActionsController, enableTrackHistory) {
  Fixture f;
  f.addEntity(QStringLiteral("India"));
  f.selectedName = QStringLiteral("India");
  auto* ctrl = f.makeController();

  ctrl->setSelectedTrackHistoryVisible(true);

  EXPECT_TRUE(f.visualState.stateFor(
      QStringLiteral("India")).trackHistoryVisible);
  EXPECT_TRUE(f.synced);
  delete ctrl;
}

// ── setSelectedHeading / Altitude / Speed ────────────────────────────────────

TEST(EntityStateActionsController, setHeadingAppliesTask) {
  Fixture f;
  f.selectedName  = QStringLiteral("Juliet");
  f.askDoubleRet  = 180.0;
  f.askDoubleOk   = true;
  f.flyTargetsOk  = true;
  auto* ctrl = f.makeController();

  ctrl->setSelectedHeading();

  EXPECT_TRUE(f.synced);
  delete ctrl;
}

TEST(EntityStateActionsController, setHeadingCancelledByUser) {
  Fixture f;
  f.selectedName = QStringLiteral("Kilo");
  f.askDoubleOk  = false;
  auto* ctrl = f.makeController();
  ctrl->setSelectedHeading();
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(EntityStateActionsController, setAltitudeAppliesTask) {
  Fixture f;
  f.selectedName = QStringLiteral("Lima");
  f.askDoubleRet = 8000.0;
  f.askDoubleOk  = true;
  auto* ctrl = f.makeController();

  ctrl->setSelectedAltitude();

  EXPECT_TRUE(f.synced);
  delete ctrl;
}

TEST(EntityStateActionsController, setSpeedAppliesTask) {
  Fixture f;
  f.selectedName = QStringLiteral("Mike");
  f.askDoubleRet = 450.0;
  f.askDoubleOk  = true;
  auto* ctrl = f.makeController();

  ctrl->setSelectedSpeed();

  EXPECT_TRUE(f.synced);
  delete ctrl;
}

TEST(EntityStateActionsController, setHeadingNoFlyTargetsDoesNothing) {
  Fixture f;
  f.selectedName  = QStringLiteral("November");
  f.flyTargetsOk  = false;
  auto* ctrl = f.makeController();
  ctrl->setSelectedHeading();
  EXPECT_FALSE(f.synced);
  delete ctrl;
}
