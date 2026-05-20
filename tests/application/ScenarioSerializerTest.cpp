#include <gtest/gtest.h>
#include "application/ScenarioSerializer.h"
#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"

#include <QDir>
#include <QTemporaryFile>

using namespace application;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Entity makeSimpleEntity(const QString& name, int force = 1) {
  Entity e;
  e.name = name;
  e.forceIdentifier = force;
  e.domain = QStringLiteral("Air");
  e.category = QStringLiteral("Fighter");
  e.latitude = 35.0;
  e.longitude = 50.0;
  e.altitude = 8000;
  e.headingDegrees = 270.0;
  e.speedKnots = 450.0;
  e.damagePercent = 10.0;
  e.behaviorMode = QStringLiteral("Aggressive");
  WeaponInventoryItem missile;
  missile.weaponType = QStringLiteral("Missile");
  missile.quantity = 2;
  e.weapons.push_back(missile);
  return e;
}

static QString tempFilePath() {
  return QDir::tempPath() + QStringLiteral("/qt_scenario_test.json");
}

// ── saveScenario / loadScenario round-trip ────────────────────────────────────

TEST(ScenarioSerializer, RoundTripEntity) {
  ScenarioSnapshot snapshot;
  snapshot.entities.push_back(makeSimpleEntity(QStringLiteral("Eagle-1"), 1));
  snapshot.entities.push_back(makeSimpleEntity(QStringLiteral("Red-1"), 2));

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));

  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.entities.size(), 2);
  EXPECT_EQ(loaded.entities.at(0).name, QStringLiteral("Eagle-1"));
  EXPECT_EQ(loaded.entities.at(1).name, QStringLiteral("Red-1"));
  EXPECT_EQ(loaded.entities.at(0).forceIdentifier, 1);
  EXPECT_EQ(loaded.entities.at(1).forceIdentifier, 2);
}

TEST(ScenarioSerializer, RoundTripEntityFields) {
  ScenarioSnapshot snapshot;
  Entity e = makeSimpleEntity(QStringLiteral("E"));
  e.latitude = 48.8566;
  e.longitude = 2.3522;
  e.altitude = 12000;
  e.headingDegrees = 90.0;
  e.damagePercent = 25.0;
  snapshot.entities.push_back(e);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_FALSE(loaded.entities.isEmpty());
  const Entity& le = loaded.entities.at(0);
  EXPECT_NEAR(le.latitude, 48.8566, 0.0001);
  EXPECT_NEAR(le.longitude, 2.3522, 0.0001);
  EXPECT_EQ(le.altitude, 12000);
  EXPECT_NEAR(le.damagePercent, 25.0, 0.001);
  EXPECT_EQ(le.behaviorMode, QStringLiteral("Aggressive"));
}

TEST(ScenarioSerializer, RuntimeStateResetOnLoad) {
  ScenarioSnapshot snapshot;
  Entity e = makeSimpleEntity(QStringLiteral("E"));
  e.speedKnots = 500.0;
  e.verticalSpeedMetersPerSecond = -30.0;
  e.flightDynamicsEnabled = true;
  e.currentTask.taskType = QStringLiteral("MoveToLocation");
  e.currentTask.enabled = true;
  snapshot.entities.push_back(e);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_FALSE(loaded.entities.isEmpty());
  const Entity& le = loaded.entities.at(0);
  EXPECT_NEAR(le.speedKnots, 0.0, 0.001);
  EXPECT_NEAR(le.verticalSpeedMetersPerSecond, 0.0, 0.001);
  EXPECT_FALSE(le.flightDynamicsEnabled);
  EXPECT_TRUE(le.currentTask.taskType.isEmpty());
  EXPECT_FALSE(le.currentTask.enabled);
}

TEST(ScenarioSerializer, RoundTripWeapons) {
  ScenarioSnapshot snapshot;
  Entity e = makeSimpleEntity(QStringLiteral("E"));
  WeaponInventoryItem bomb;
  bomb.weaponType = QStringLiteral("Bomb");
  bomb.quantity = 4;
  e.weapons.push_back(bomb);
  snapshot.entities.push_back(e);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_FALSE(loaded.entities.isEmpty());
  EXPECT_EQ(loaded.entities.at(0).weapons.size(), 2);
}

// ── Waypoints ─────────────────────────────────────────────────────────────────

TEST(ScenarioSerializer, RoundTripWaypoints) {
  ScenarioSnapshot snapshot;
  Waypoint wp;
  wp.name = QStringLiteral("Alpha");
  wp.latitude = 10.0;
  wp.longitude = 20.0;
  wp.altitudeMeters = 5000.0;
  snapshot.waypoints.push_back(wp);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.waypoints.size(), 1);
  EXPECT_EQ(loaded.waypoints.at(0).name, QStringLiteral("Alpha"));
  EXPECT_NEAR(loaded.waypoints.at(0).latitude, 10.0, 0.0001);
  EXPECT_NEAR(loaded.waypoints.at(0).altitudeMeters, 5000.0, 0.1);
}

// ── Routes ────────────────────────────────────────────────────────────────────

TEST(ScenarioSerializer, RoundTripRoutes) {
  ScenarioSnapshot snapshot;
  RouteGraphic route;
  route.name = QStringLiteral("Route-Alpha");
  RoutePoint p;
  p.latitude = 5.0; p.longitude = 10.0; p.altitudeMeters = 3000.0;
  route.points.push_back(p);
  snapshot.routes.push_back(route);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.routes.size(), 1);
  EXPECT_EQ(loaded.routes.at(0).name, QStringLiteral("Route-Alpha"));
  ASSERT_EQ(loaded.routes.at(0).points.size(), 1);
  EXPECT_NEAR(loaded.routes.at(0).points.at(0).latitude, 5.0, 0.0001);
}

// ── Areas ─────────────────────────────────────────────────────────────────────

TEST(ScenarioSerializer, RoundTripAreas) {
  ScenarioSnapshot snapshot;
  AreaDefinition area;
  area.id = QStringLiteral("a1");
  area.name = QStringLiteral("Zone-1");
  area.areaType = QStringLiteral("Circle");
  area.radiusMeters = 15000.0;
  snapshot.areas.push_back(area);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.areas.size(), 1);
  EXPECT_EQ(loaded.areas.at(0).name, QStringLiteral("Zone-1"));
  EXPECT_NEAR(loaded.areas.at(0).radiusMeters, 15000.0, 0.1);
}

// ── Error cases ───────────────────────────────────────────────────────────────

TEST(ScenarioSerializer, LoadFromNonexistentFileReturnsEmpty) {
  const ScenarioSnapshot result = loadScenario(QStringLiteral("/nonexistent/path/x.json"));
  EXPECT_TRUE(result.entities.isEmpty());
  EXPECT_TRUE(result.waypoints.isEmpty());
}

TEST(ScenarioSerializer, SaveToUnwritablePathReturnsFalse) {
  ScenarioSnapshot snapshot;
  // Try to write to a directory path (not a file)
  const bool ok = saveScenario(QStringLiteral("/"), snapshot);
  EXPECT_FALSE(ok);
}
