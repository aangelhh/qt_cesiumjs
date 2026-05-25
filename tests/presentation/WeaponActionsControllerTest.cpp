#include <gtest/gtest.h>
#include "application/ScenarioState.h"
#include "presentation/WeaponActionsController.h"

using namespace presentation;

// ── Test fixture ──────────────────────────────────────────────────────────────

namespace {

struct WeaponActionsFixture {
  ScenarioState state;
  QString       lastLog;
  QString       lastStatus;
  bool          synced     = false;
  bool          simRunning = true;
  QString       selectedName;

  WeaponActionsController* makeController(
      WeaponActionsController::PickItemFn pickFn = [](const QStringList&) { return QString{}; }) {
    return new WeaponActionsController(
        &state,
        [this]() { return selectedName; },
        [this](const QString& name) -> const Entity* {
          const auto& entities = state.entities();
          for (const Entity& e : entities) {
            if (e.name == name) return &e;
          }
          return nullptr;
        },
        [this]() { return simRunning; },
        [this](const QString& msg) { lastLog = msg; },
        [this](const QString& msg) { lastStatus = msg; },
        [this]() { synced = true; },
        std::move(pickFn));
  }

  Entity addAirFighter(const QString& name, int missiles = 2, int bombs = 2) {
    Entity e;
    e.name = name;
    e.domain = QStringLiteral("Air");
    e.category = QStringLiteral("Fighter");
    e.forceIdentifier = 1;
    WeaponInventoryItem m; m.weaponType = QStringLiteral("Missile"); m.quantity = missiles;
    WeaponInventoryItem b; b.weaponType = QStringLiteral("Bomb"); b.quantity = bombs;
    e.weapons = {m, b};
    e.destroyed = false;
    state.addEntity(e);
    return e;
  }
};

} // namespace

// ── addMissileToSelected ─────────────────────────────────────────────────────

TEST(WeaponActionsController, AddMissileNoSelectionIsNoop) {
  WeaponActionsFixture f;
  f.selectedName = QStringLiteral("");
  auto* ctrl = f.makeController();
  ctrl->addMissileToSelected();
  EXPECT_TRUE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(WeaponActionsController, AddMissileSuccess) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 1, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  auto* ctrl = f.makeController();
  ctrl->addMissileToSelected();
  EXPECT_TRUE(f.synced);
  EXPECT_TRUE(f.lastLog.contains(QStringLiteral("Missile added")));
  delete ctrl;
}

// ── addBombToSelected ─────────────────────────────────────────────────────────

TEST(WeaponActionsController, AddBombNoSelectionIsNoop) {
  WeaponActionsFixture f;
  f.selectedName = QStringLiteral("");
  auto* ctrl = f.makeController();
  ctrl->addBombToSelected();
  EXPECT_TRUE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(WeaponActionsController, AddBombSuccess) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 0, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  auto* ctrl = f.makeController();
  ctrl->addBombToSelected();
  EXPECT_TRUE(f.synced);
  EXPECT_TRUE(f.lastLog.contains(QStringLiteral("Bomb added")));
  delete ctrl;
}

// ── launchMissileFromSelected ─────────────────────────────────────────────────

TEST(WeaponActionsController, LaunchMissileNoAmmoSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 0, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  auto* ctrl = f.makeController();
  ctrl->launchMissileFromSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("No hay misiles")));
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(WeaponActionsController, LaunchMissileSimNotRunningSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 2, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  f.simRunning = false;
  auto* ctrl = f.makeController();
  ctrl->launchMissileFromSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("simulacion")));
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

// ── releaseBombFromSelected ───────────────────────────────────────────────────

TEST(WeaponActionsController, ReleaseBombNoAmmoSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 0, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  auto* ctrl = f.makeController();
  ctrl->releaseBombFromSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("No hay bombas")));
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(WeaponActionsController, ReleaseBombSimNotRunningSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 0, 2);
  f.selectedName = QStringLiteral("Eagle-1");
  f.simRunning = false;
  auto* ctrl = f.makeController();
  ctrl->releaseBombFromSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("simulacion")));
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

// ── launchMissileAtSelected ───────────────────────────────────────────────────

TEST(WeaponActionsController, LaunchMissileAtNoAmmoSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 0, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  auto* ctrl = f.makeController();
  ctrl->launchMissileAtSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("No hay misiles")));
  delete ctrl;
}

TEST(WeaponActionsController, LaunchMissileAtSimNotRunningSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 2, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  f.simRunning = false;
  auto* ctrl = f.makeController();
  ctrl->launchMissileAtSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("simulacion")));
  delete ctrl;
}

TEST(WeaponActionsController, LaunchMissileAtNoTargetsSetsStatus) {
  WeaponActionsFixture f;
  f.addAirFighter(QStringLiteral("Eagle-1"), 2, 0);
  f.selectedName = QStringLiteral("Eagle-1");
  f.simRunning = true;
  // No enemies → no detected targets
  auto* ctrl = f.makeController();
  ctrl->launchMissileAtSelected();
  EXPECT_TRUE(f.lastStatus.contains(QStringLiteral("No detected")));
  delete ctrl;
}
