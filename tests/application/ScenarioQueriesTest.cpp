#include <gtest/gtest.h>
#include "application/ScenarioQueries.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "domain/Munition.h"
#include "domain/TacticalGraphic.h"

using namespace application;

// ── Helpers ──────────────────────────────────────────────────────────────────

static Entity makeAirEntity(const QString& name, int force = 1) {
  Entity e;
  e.name = name;
  e.forceIdentifier = force;
  e.domain = QStringLiteral("Air");
  e.destroyed = false;
  e.altitude = 5000;
  return e;
}

static Entity makeSurfaceEntity(const QString& name, int force = 1) {
  Entity e;
  e.name = name;
  e.forceIdentifier = force;
  e.domain = QStringLiteral("Surface");
  e.destroyed = false;
  e.altitude = 0;
  return e;
}

static SensorContact makeContact(const QString& target, double rangeMeters, bool detected = true) {
  SensorContact c;
  c.targetEntityName = target;
  c.detected = detected;
  c.rangeMeters = rangeMeters;
  c.bearingDegrees = 0.0;
  return c;
}

// ── detectedMissileTargetsInRange ────────────────────────────────────────────

TEST(ScenarioQueriesTest, MissileTargets_NullStateReturnsEmpty) {
  Entity launcher = makeAirEntity(QStringLiteral("F1"));
  auto result = detectedMissileTargetsInRange(nullptr, launcher);
  EXPECT_TRUE(result.isEmpty());
}

TEST(ScenarioQueriesTest, MissileTargets_ReturnsDetectedAirEnemyInRange) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity target = makeAirEntity(QStringLiteral("E1"), 2);
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("E1"), 50000.0));
  state->addEntity(launcher);
  state->addEntity(target);

  auto result = detectedMissileTargetsInRange(state.get(), launcher);
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result.first().entity->name, QStringLiteral("E1"));
}

TEST(ScenarioQueriesTest, MissileTargets_IgnoresFriendly) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity friendly = makeAirEntity(QStringLiteral("F2"), 1);
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("F2"), 50000.0));
  state->addEntity(launcher);
  state->addEntity(friendly);

  auto result = detectedMissileTargetsInRange(state.get(), launcher);
  EXPECT_TRUE(result.isEmpty());
}

TEST(ScenarioQueriesTest, MissileTargets_IgnoresSurface) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity surfaceTarget = makeSurfaceEntity(QStringLiteral("S1"), 2);
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("S1"), 50000.0));
  state->addEntity(launcher);
  state->addEntity(surfaceTarget);

  auto result = detectedMissileTargetsInRange(state.get(), launcher);
  EXPECT_TRUE(result.isEmpty());
}

TEST(ScenarioQueriesTest, MissileTargets_IgnoresOutOfRange) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity target = makeAirEntity(QStringLiteral("E1"), 2);
  // beyond max range
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("E1"), 999999.0));
  state->addEntity(launcher);
  state->addEntity(target);

  auto result = detectedMissileTargetsInRange(state.get(), launcher);
  EXPECT_TRUE(result.isEmpty());
}

TEST(ScenarioQueriesTest, MissileTargets_IgnoresDestroyed) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity target = makeAirEntity(QStringLiteral("E1"), 2);
  target.destroyed = true;
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("E1"), 50000.0));
  state->addEntity(launcher);
  state->addEntity(target);

  auto result = detectedMissileTargetsInRange(state.get(), launcher);
  EXPECT_TRUE(result.isEmpty());
}

// ── activeMissileInFlightForTarget ───────────────────────────────────────────

TEST(ScenarioQueriesTest, ActiveMissile_ReturnsFalseWhenNoMunitions) {
  auto state = std::make_unique<ScenarioState>();
  EXPECT_FALSE(activeMissileInFlightForTarget(state.get(), QStringLiteral("F1"), QStringLiteral("E1")));
}

TEST(ScenarioQueriesTest, ActiveMissile_ReturnsFalseForNullState) {
  EXPECT_FALSE(activeMissileInFlightForTarget(nullptr, QStringLiteral("F1"), QStringLiteral("E1")));
}

// ── entityAltitudeMeters ──────────────────────────────────────────────────────

TEST(ScenarioQueriesTest, AltitudeMeters_Returns0ForMissingEntity) {
  auto state = std::make_unique<ScenarioState>();
  EXPECT_EQ(entityAltitudeMeters(state.get(), QStringLiteral("Unknown")), 0);
}

TEST(ScenarioQueriesTest, AltitudeMeters_ReturnsEntityAltitude) {
  auto state = std::make_unique<ScenarioState>();
  Entity entity = makeAirEntity(QStringLiteral("F1"));
  entity.altitude = 8000;
  state->addEntity(entity);
  EXPECT_EQ(entityAltitudeMeters(state.get(), QStringLiteral("F1")), 8000);
}

// ── validBombReleaseTargets ───────────────────────────────────────────────────

TEST(ScenarioQueriesTest, BombTargets_ExcludesFriendly) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity friendly = makeSurfaceEntity(QStringLiteral("F2"), 1);
  Entity enemy = makeSurfaceEntity(QStringLiteral("E1"), 2);
  state->addEntity(launcher);
  state->addEntity(friendly);
  state->addEntity(enemy);

  auto targets = validBombReleaseTargets(state.get(), launcher);
  ASSERT_EQ(targets.size(), 1);
  EXPECT_EQ(targets.first()->name, QStringLiteral("E1"));
}

TEST(ScenarioQueriesTest, BombTargets_ExcludesAirDomain) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity enemyAir = makeAirEntity(QStringLiteral("E1"), 2);
  Entity enemySurface = makeSurfaceEntity(QStringLiteral("E2"), 2);
  state->addEntity(launcher);
  state->addEntity(enemyAir);
  state->addEntity(enemySurface);

  auto targets = validBombReleaseTargets(state.get(), launcher);
  ASSERT_EQ(targets.size(), 1);
  EXPECT_EQ(targets.first()->name, QStringLiteral("E2"));
}

TEST(ScenarioQueriesTest, BombTargets_ExcludesDestroyed) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity enemy = makeSurfaceEntity(QStringLiteral("E1"), 2);
  enemy.destroyed = true;
  state->addEntity(launcher);
  state->addEntity(enemy);

  auto targets = validBombReleaseTargets(state.get(), launcher);
  EXPECT_TRUE(targets.isEmpty());
}

// ── bestDetectedSurfaceBombTarget ─────────────────────────────────────────────

TEST(ScenarioQueriesTest, BestBombTarget_ReturnsNullWhenNoContacts) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  state->addEntity(launcher);
  EXPECT_EQ(bestDetectedSurfaceBombTarget(state.get(), launcher), nullptr);
}

TEST(ScenarioQueriesTest, BestBombTarget_ReturnsNearestDetectedSurface) {
  auto state = std::make_unique<ScenarioState>();
  Entity launcher = makeAirEntity(QStringLiteral("F1"), 1);
  Entity near = makeSurfaceEntity(QStringLiteral("E1"), 2);
  Entity far = makeSurfaceEntity(QStringLiteral("E2"), 2);
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("E2"), 80000.0));
  launcher.sensorContacts.push_back(makeContact(QStringLiteral("E1"), 30000.0));
  state->addEntity(launcher);
  state->addEntity(near);
  state->addEntity(far);

  const Entity* result = bestDetectedSurfaceBombTarget(state.get(), launcher);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->name, QStringLiteral("E1"));
}

// ── availableWaypointNames ────────────────────────────────────────────────────

TEST(ScenarioQueriesTest, WaypointNames_ReturnsEmptyWhenNoWaypoints) {
  auto state = std::make_unique<ScenarioState>();
  EXPECT_TRUE(availableWaypointNames(state.get()).isEmpty());
}

TEST(ScenarioQueriesTest, WaypointNames_ReturnsNames) {
  auto state = std::make_unique<ScenarioState>();
  Waypoint wp;
  wp.name = QStringLiteral("WP1");
  wp.latitude = 40.0;
  wp.longitude = -3.0;
  wp.altitudeMeters = 0.0;
  state->addWaypoint(wp);
  const QStringList names = availableWaypointNames(state.get());
  ASSERT_EQ(names.size(), 1);
  EXPECT_EQ(names.first(), QStringLiteral("WP1"));
}

TEST(ScenarioQueriesTest, WaypointNames_DeduplicatesNames) {
  auto state = std::make_unique<ScenarioState>();
  Waypoint wp1;
  wp1.name = QStringLiteral("WP1");
  wp1.latitude = 40.0; wp1.longitude = -3.0;
  Waypoint wp2;
  wp2.name = QStringLiteral("WP1");
  wp2.latitude = 41.0; wp2.longitude = -4.0;
  state->addWaypoint(wp1);
  state->addWaypoint(wp2);
  EXPECT_EQ(availableWaypointNames(state.get()).size(), 1);
}

TEST(ScenarioQueriesTest, WaypointNames_SkipsEmptyNames) {
  auto state = std::make_unique<ScenarioState>();
  Waypoint wp;
  wp.name = QStringLiteral("   ");
  wp.latitude = 40.0; wp.longitude = -3.0;
  state->addWaypoint(wp);
  EXPECT_TRUE(availableWaypointNames(state.get()).isEmpty());
}

// ── availableRouteNames ───────────────────────────────────────────────────────

TEST(ScenarioQueriesTest, RouteNames_ReturnsRouteNames) {
  auto state = std::make_unique<ScenarioState>();
  RouteGraphic route;
  route.name = QStringLiteral("Route1");
  RoutePoint pt; pt.latitude = 40.0; pt.longitude = -3.0; pt.altitudeMeters = 0.0;
  route.points.push_back(pt);
  state->addRoute(route);
  const QStringList names = availableRouteNames(state.get());
  ASSERT_EQ(names.size(), 1);
  EXPECT_EQ(names.first(), QStringLiteral("Route1"));
}

TEST(ScenarioQueriesTest, RouteNames_RequirePointsFiltersEmpty) {
  auto state = std::make_unique<ScenarioState>();
  RouteGraphic route;
  route.name = QStringLiteral("Empty");
  state->addRoute(route);
  EXPECT_EQ(availableRouteNames(state.get(), false).size(), 1);
  EXPECT_EQ(availableRouteNames(state.get(), true).size(), 0);
}

// ── availableAreaNames ────────────────────────────────────────────────────────

TEST(ScenarioQueriesTest, AreaNames_ReturnsAreaName) {
  auto state = std::make_unique<ScenarioState>();
  AreaDefinition area;
  area.id = QStringLiteral("a1");
  area.name = QStringLiteral("Area1");
  area.areaType = QStringLiteral("Circle");
  state->addArea(area);
  const QStringList names = availableAreaNames(state.get());
  ASSERT_EQ(names.size(), 1);
  EXPECT_EQ(names.first(), QStringLiteral("Area1"));
}

TEST(ScenarioQueriesTest, AreaNames_FallsBackToIdWhenNameEmpty) {
  auto state = std::make_unique<ScenarioState>();
  AreaDefinition area;
  area.id = QStringLiteral("zone-alpha");
  area.name = QStringLiteral("");
  area.areaType = QStringLiteral("Circle");
  state->addArea(area);
  const QStringList names = availableAreaNames(state.get());
  ASSERT_EQ(names.size(), 1);
  EXPECT_EQ(names.first(), QStringLiteral("zone-alpha"));
}

TEST(ScenarioQueriesTest, AreaNames_ReturnsNullStateEmpty) {
  EXPECT_TRUE(availableAreaNames(nullptr).isEmpty());
}
