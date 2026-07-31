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
  e.systemsDisplayProfileId = QStringLiteral("air-turbine-2-engine");
  e.engineCount = 2;
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
  EXPECT_EQ(
      le.systemsDisplayProfileId,
      QStringLiteral("air-turbine-2-engine"));
  EXPECT_EQ(le.engineCount, 2);
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

TEST(ScenarioSerializer, GroundEntityLoadPreservesConfiguredAltitude) {
  ScenarioSnapshot snapshot;
  Entity ground = makeSimpleEntity(QStringLiteral("Ground-1"));
  ground.domain = QStringLiteral("Ground");
  ground.category = QStringLiteral("Vehicle");
  ground.altitude = 350;
  ground.pitchDegrees = 12.0;
  ground.rollDegrees = -8.0;
  ground.speedKnots = 25.0;
  ground.currentTask.enabled = true;
  ground.currentTask.taskType = QStringLiteral("MoveToLocation");
  snapshot.entities.push_back(ground);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_FALSE(loaded.entities.isEmpty());
  const Entity& loadedGround = loaded.entities.at(0);
  EXPECT_EQ(loadedGround.domain, QStringLiteral("Ground"));
  EXPECT_EQ(loadedGround.altitude, 350);
  EXPECT_DOUBLE_EQ(loadedGround.pitchDegrees, 0.0);
  EXPECT_DOUBLE_EQ(loadedGround.rollDegrees, 0.0);
  EXPECT_DOUBLE_EQ(loadedGround.speedKnots, 0.0);
  EXPECT_TRUE(loadedGround.currentTask.taskType.isEmpty());
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

TEST(ScenarioSerializer, RoundTripSensorTypeMetadata) {
  ScenarioSnapshot snapshot;
  Entity entity = makeSimpleEntity(QStringLiteral("SensorPlatform"));
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-primary");
  sensor.sensorType = QStringLiteral("radar");
  sensor.sensorSubType = QStringLiteral("airborneRadar");
  entity.sensors.push_back(sensor);

  SensorContact contact;
  contact.sensorId = sensor.id;
  contact.sensorType = sensor.sensorType;
  contact.sensorSubType = sensor.sensorSubType;
  contact.targetEntityName = QStringLiteral("Target");
  contact.detected = true;
  entity.sensorContacts.push_back(contact);
  snapshot.entities.push_back(entity);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.entities.size(), 1);
  ASSERT_EQ(loaded.entities.front().sensors.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().sensors.front().sensorSubType,
      QStringLiteral("airborneRadar"));
  ASSERT_EQ(loaded.entities.front().sensorContacts.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().sensorType,
      QStringLiteral("radar"));
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().sensorSubType,
      QStringLiteral("airborneRadar"));
}

TEST(ScenarioSerializer, LegacySensorWithoutSubTypeLoadsAsGeneric) {
  QTemporaryFile file;
  ASSERT_TRUE(file.open());
  file.write(R"({
    "waypoints": [],
    "routes": [],
    "areas": [],
    "entities": [{
      "name": "LegacyRadar",
      "sensors": [{"id": "legacy", "sensorType": "radar"}]
    }]
  })");
  file.close();

  const ScenarioSnapshot loaded = loadScenario(file.fileName());

  ASSERT_EQ(loaded.entities.size(), 1);
  ASSERT_EQ(loaded.entities.front().sensors.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().sensors.front().sensorSubType,
      QStringLiteral("generic"));
}

// ── Waypoints ─────────────────────────────────────────────────────────────────

TEST(ScenarioSerializer, RoundTripWaypoints) {
  ScenarioSnapshot snapshot;
  Waypoint wp;
  wp.name = QStringLiteral("Alpha");
  wp.latitude = 10.0;
  wp.longitude = 20.0;
  wp.altitudeMeters = 5000.0;
  wp.altitudeMetersSet = true;
  snapshot.waypoints.push_back(wp);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.waypoints.size(), 1);
  EXPECT_EQ(loaded.waypoints.at(0).name, QStringLiteral("Alpha"));
  EXPECT_NEAR(loaded.waypoints.at(0).latitude, 10.0, 0.0001);
  EXPECT_NEAR(loaded.waypoints.at(0).altitudeMeters, 5000.0, 0.1);
  EXPECT_TRUE(loaded.waypoints.at(0).altitudeMetersSet);
}

TEST(ScenarioSerializer, LegacyWaypointWithoutAltitudeKeepsAltitudeUnset) {
  QTemporaryFile file;
  ASSERT_TRUE(file.open());
  file.write(R"({
    "waypoints": [
      {"name": "Legacy", "latitude": 10.0, "longitude": 20.0}
    ],
    "routes": [],
    "areas": [],
    "entities": []
  })");
  file.close();

  const ScenarioSnapshot loaded = loadScenario(file.fileName());

  ASSERT_EQ(loaded.waypoints.size(), 1);
  EXPECT_EQ(loaded.waypoints.at(0).name, QStringLiteral("Legacy"));
  EXPECT_DOUBLE_EQ(loaded.waypoints.at(0).altitudeMeters, 0.0);
  EXPECT_FALSE(loaded.waypoints.at(0).altitudeMetersSet);
}

// ── Routes ────────────────────────────────────────────────────────────────────

TEST(ScenarioSerializer, RoundTripRoutes) {
  ScenarioSnapshot snapshot;
  RouteGraphic route;
  route.name = QStringLiteral("Route-Alpha");
  RoutePoint p;
  p.latitude = 5.0; p.longitude = 10.0; p.altitudeMeters = 3000.0;
  p.altitudeMetersSet = true;
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
  EXPECT_NEAR(loaded.routes.at(0).points.at(0).altitudeMeters, 3000.0, 0.1);
  EXPECT_TRUE(loaded.routes.at(0).points.at(0).altitudeMetersSet);
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
