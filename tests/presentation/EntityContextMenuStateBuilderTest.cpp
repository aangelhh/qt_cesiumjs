#include <gtest/gtest.h>
#include "presentation/EntityContextMenuStateBuilder.h"
#include "domain/Entity.h"
#include <QVariantMap>

using namespace presentation;

namespace {

Entity makeAirFighter() {
  Entity e;
  e.name         = "Eagle1";
  e.domain       = "Air";
  e.category     = "Fighter";
  e.behaviorMode = "Aggressive";
  e.destroyed    = false;
  e.weapons      = {{QStringLiteral("Missile"), 3}, {QStringLiteral("Bomb"), 2}};
  return e;
}

PendingBombRelease pendingFor(const QString& name) {
  PendingBombRelease r;
  r.pending              = true;
  r.launcherEntityName   = name;
  return r;
}

} // namespace

// ── entityBehaviorModeOptions ──────────────────────────────────────────────

TEST(EntityBehaviorModeOptions, ReturnsAllFourModes) {
  const QStringList opts = entityBehaviorModeOptions();
  ASSERT_EQ(opts.size(), 4);
  EXPECT_TRUE(opts.contains(QStringLiteral("Manual")));
  EXPECT_TRUE(opts.contains(QStringLiteral("Aggressive")));
  EXPECT_TRUE(opts.contains(QStringLiteral("Defensive")));
  EXPECT_TRUE(opts.contains(QStringLiteral("Patrol")));
}

TEST(EntityBehaviorModeOptions, ManualIsFirst) {
  EXPECT_EQ(entityBehaviorModeOptions().at(0), QStringLiteral("Manual"));
}

// ── buildEntityContextMenuState — null entity ─────────────────────────────

TEST(BuildEntityContextMenuState, NullEntityGivesZeroedCounts) {
  auto s = buildEntityContextMenuState(nullptr, nullptr, {}, false, false, {});
  EXPECT_FALSE(s.canUseWeapons);
  EXPECT_EQ(s.missileCount, 0);
  EXPECT_EQ(s.bombCount, 0);
  EXPECT_EQ(s.detectedMissileTargetCount, 0);
  EXPECT_FALSE(s.bombReleasePendingForThisEntity);
  EXPECT_TRUE(s.entityName.isEmpty());
}

TEST(BuildEntityContextMenuState, NullEntityDefaultsBehaviorModeToManual) {
  auto s = buildEntityContextMenuState(nullptr, nullptr, {}, false, false, {});
  EXPECT_EQ(s.currentBehaviorMode, QStringLiteral("Manual"));
}

// ── weapon counts ─────────────────────────────────────────────────────────

TEST(BuildEntityContextMenuState, PopulatesWeaponCounts) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_TRUE(s.canUseWeapons);
  EXPECT_EQ(s.missileCount, 3);
  EXPECT_EQ(s.bombCount, 2);
}

TEST(BuildEntityContextMenuState, NonFighterCannotUseWeapons) {
  Entity e = makeAirFighter();
  e.category = "Transport";
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_FALSE(s.canUseWeapons);
}

TEST(BuildEntityContextMenuState, DestroyedEntityCannotUseWeapons) {
  Entity e = makeAirFighter();
  e.destroyed = true;
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_FALSE(s.canUseWeapons);
}

// ── behavior mode ─────────────────────────────────────────────────────────

TEST(BuildEntityContextMenuState, UsesEntityBehaviorMode) {
  Entity e = makeAirFighter();
  e.behaviorMode = "Patrol";
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_EQ(s.currentBehaviorMode, QStringLiteral("Patrol"));
}

TEST(BuildEntityContextMenuState, EmptyBehaviorModeFallsBackToManual) {
  Entity e = makeAirFighter();
  e.behaviorMode = "";
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_EQ(s.currentBehaviorMode, QStringLiteral("Manual"));
}

TEST(BuildEntityContextMenuState, WhitespaceOnlyBehaviorModeFallsBackToManual) {
  Entity e = makeAirFighter();
  e.behaviorMode = "   ";
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_EQ(s.currentBehaviorMode, QStringLiteral("Manual"));
}

TEST(BuildEntityContextMenuState, BehaviorModeOptionListAlwaysPopulated) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_EQ(s.behaviorModeOptions.size(), 4);
}

// ── bomb release ──────────────────────────────────────────────────────────

TEST(BuildEntityContextMenuState, BombReleasePendingForMatchingEntity) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, pendingFor("Eagle1"), false, false, {});
  EXPECT_TRUE(s.bombReleasePendingForThisEntity);
}

TEST(BuildEntityContextMenuState, BombReleasePendingCaseInsensitiveMatch) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, pendingFor("eagle1"), false, false, {});
  EXPECT_TRUE(s.bombReleasePendingForThisEntity);
}

TEST(BuildEntityContextMenuState, BombReleaseNotPendingForDifferentEntity) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, pendingFor("Viper2"), false, false, {});
  EXPECT_FALSE(s.bombReleasePendingForThisEntity);
}

TEST(BuildEntityContextMenuState, BombReleaseNotActiveWhenPendingFalse) {
  Entity e = makeAirFighter();
  PendingBombRelease r;
  r.pending            = false;
  r.launcherEntityName = "Eagle1";
  auto s = buildEntityContextMenuState(&e, nullptr, r, false, false, {});
  EXPECT_FALSE(s.bombReleasePendingForThisEntity);
}

// ── flags ─────────────────────────────────────────────────────────────────

TEST(BuildEntityContextMenuState, ForwardsBoolFlags) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, {}, true, true, {});
  EXPECT_TRUE(s.simulationRunning);
  EXPECT_TRUE(s.entityDestroyed);
}

TEST(BuildEntityContextMenuState, ReadsSummaryMapFlags) {
  Entity e = makeAirFighter();
  QVariantMap m;
  m[QStringLiteral("hidden")]               = true;
  m[QStringLiteral("radarCoverageVisible")] = true;
  m[QStringLiteral("trackHistoryVisible")]  = false;
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, m);
  EXPECT_TRUE(s.hidden);
  EXPECT_TRUE(s.radarCoverageVisible);
  EXPECT_FALSE(s.trackHistoryVisible);
}

TEST(BuildEntityContextMenuState, DefaultSummaryFlagsAreFalse) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_FALSE(s.hidden);
  EXPECT_FALSE(s.radarCoverageVisible);
  EXPECT_FALSE(s.trackHistoryVisible);
}

TEST(BuildEntityContextMenuState, EntityNameCopied) {
  Entity e = makeAirFighter();
  auto s = buildEntityContextMenuState(&e, nullptr, {}, false, false, {});
  EXPECT_EQ(s.entityName, QStringLiteral("Eagle1"));
}
