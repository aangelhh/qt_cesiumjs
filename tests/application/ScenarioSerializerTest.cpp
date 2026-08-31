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

TEST(ScenarioSerializer, RoundTripPreservesDistinctIdsForDuplicateDisplayNames) {
  ScenarioSnapshot snapshot;
  Entity first = makeSimpleEntity(QStringLiteral("mirage2000"), 1);
  Entity second = makeSimpleEntity(QStringLiteral("mirage2000"), 2);
  ASSERT_NE(first.entityId, second.entityId);
  snapshot.entities = {first, second};

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.entities.size(), 2);
  EXPECT_EQ(loaded.entities.at(0).name, loaded.entities.at(1).name);
  EXPECT_EQ(loaded.entities.at(0).entityId, first.entityId);
  EXPECT_EQ(loaded.entities.at(1).entityId, second.entityId);
  EXPECT_NE(loaded.entities.at(0).entityId, loaded.entities.at(1).entityId);
}

TEST(ScenarioSerializer, LegacyDuplicateNamesReceiveDistinctStableIds) {
  QTemporaryFile file;
  ASSERT_TRUE(file.open());
  file.write(R"({
    "waypoints": [], "routes": [], "areas": [],
    "entities": [{"name": "legacy"}, {"name": "legacy"}]
  })");
  file.close();

  const ScenarioSnapshot loaded = loadScenario(file.fileName());

  ASSERT_EQ(loaded.entities.size(), 2);
  EXPECT_FALSE(loaded.entities.at(0).entityId.isEmpty());
  EXPECT_FALSE(loaded.entities.at(1).entityId.isEmpty());
  EXPECT_NE(loaded.entities.at(0).entityId, loaded.entities.at(1).entityId);
}

TEST(ScenarioSerializer, RoundTripEntityFields) {
  ScenarioSnapshot snapshot;
  Entity e = makeSimpleEntity(QStringLiteral("E"));
  e.latitude = 48.8566;
  e.longitude = 2.3522;
  e.altitude = 12000;
  e.headingDegrees = 90.0;
  e.damagePercent = 25.0;
  e.dynamicsModelCompatibility = QStringLiteral("approximation");
  e.systemsDisplayProfileId = QStringLiteral("air-turbine-2-engine");
  e.controlProfileId = QStringLiteral("fighter-generic");
  e.cesiumModelAxes = QStringLiteral("x-forward-y-up");
  e.engineCount = 2;
  e.fuelCapacityKilograms = 6400.0;
  e.fuelRemainingKilograms = 2750.0;
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
  EXPECT_EQ(le.dynamicsModelCompatibility, QStringLiteral("approximation"));
  EXPECT_EQ(
      le.systemsDisplayProfileId,
      QStringLiteral("air-turbine-2-engine"));
  EXPECT_EQ(le.controlProfileId, QStringLiteral("fighter-generic"));
  EXPECT_EQ(le.cesiumModelAxes, QStringLiteral("x-forward-y-up"));
  EXPECT_EQ(le.engineCount, 2);
  EXPECT_DOUBLE_EQ(le.fuelCapacityKilograms, 6400.0);
  EXPECT_DOUBLE_EQ(le.fuelRemainingKilograms, 2750.0);
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
  ground.cesiumModelAxes = QStringLiteral("x-forward-y-up");
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
  EXPECT_EQ(
      loadedGround.cesiumModelAxes,
      QStringLiteral("x-forward-y-up"));
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
  snapshot.sensorRandomSeed = 424242U;
  Entity entity = makeSimpleEntity(QStringLiteral("SensorPlatform"));
  entity.radarSignature = 0.35;
  entity.thermalSignature = 1.4;
  entity.visualSignature = 0.8;
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-primary");
  sensor.modelProviderId = QStringLiteral("mixr");
  sensor.sensorType = QStringLiteral("radar");
  sensor.sensorSubType = QStringLiteral("airborneRadar");
  sensor.probabilityOfDetection = 0.72;
  sensor.trackHoldSeconds = 8.0;
  sensor.radarProfile.profileId = QStringLiteral("custom");
  sensor.radarProfile.peakPowerWatts = 42000.0;
  sensor.radarProfile.dutyCycle = 0.15;
  sensor.radarProfile.frequencyHertz = 9.2e9;
  sensor.radarProfile.probabilityFalseAlarm = 2.0e-7;
  entity.sensors.push_back(sensor);

  SensorContact contact;
  contact.sensorId = sensor.id;
  contact.sensorModelProviderId = QStringLiteral("mixr");
  contact.sensorType = sensor.sensorType;
  contact.sensorSubType = sensor.sensorSubType;
  contact.targetEntityName = QStringLiteral("Target");
  contact.detected = true;
  contact.confidence = 0.64;
  contact.lastSeenSimulationSeconds = 12.5;
  contact.trackState = QStringLiteral("Coasting");
  contact.lastEvaluationIndex = 7;
  contact.missedDetectionCount = 1;
  entity.sensorContacts.push_back(contact);
  snapshot.entities.push_back(entity);

  const QString path = tempFilePath();
  ASSERT_TRUE(saveScenario(path, snapshot));
  const ScenarioSnapshot loaded = loadScenario(path);
  QFile::remove(path);

  ASSERT_EQ(loaded.entities.size(), 1);
  EXPECT_EQ(loaded.sensorRandomSeed, 424242U);
  EXPECT_DOUBLE_EQ(loaded.entities.front().radarSignature, 0.35);
  EXPECT_DOUBLE_EQ(loaded.entities.front().thermalSignature, 1.4);
  EXPECT_DOUBLE_EQ(loaded.entities.front().visualSignature, 0.8);
  ASSERT_EQ(loaded.entities.front().sensors.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().sensors.front().modelProviderId,
      QStringLiteral("mixr"));
  EXPECT_EQ(
      loaded.entities.front().sensors.front().sensorSubType,
      QStringLiteral("airborneRadar"));
  EXPECT_DOUBLE_EQ(
      loaded.entities.front().sensors.front().probabilityOfDetection,
      0.72);
  EXPECT_DOUBLE_EQ(
      loaded.entities.front().sensors.front().trackHoldSeconds,
      8.0);
  const RadarProfile& loadedProfile =
      loaded.entities.front().sensors.front().radarProfile;
  EXPECT_EQ(loadedProfile.profileId, QStringLiteral("custom"));
  EXPECT_DOUBLE_EQ(loadedProfile.peakPowerWatts, 42000.0);
  EXPECT_DOUBLE_EQ(loadedProfile.dutyCycle, 0.15);
  EXPECT_DOUBLE_EQ(loadedProfile.frequencyHertz, 9.2e9);
  EXPECT_DOUBLE_EQ(loadedProfile.probabilityFalseAlarm, 2.0e-7);
  ASSERT_EQ(loaded.entities.front().sensorContacts.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().sensorModelProviderId,
      QStringLiteral("mixr"));
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().sensorType,
      QStringLiteral("radar"));
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().sensorSubType,
      QStringLiteral("airborneRadar"));
  EXPECT_DOUBLE_EQ(
      loaded.entities.front().sensorContacts.front().confidence,
      0.64);
  EXPECT_DOUBLE_EQ(
      loaded.entities.front().sensorContacts.front().lastSeenSimulationSeconds,
      12.5);
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().trackState,
      QStringLiteral("Coasting"));
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().lastEvaluationIndex,
      7);
  EXPECT_EQ(
      loaded.entities.front().sensorContacts.front().missedDetectionCount,
      1);
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
  EXPECT_EQ(
      loaded.entities.front().sensors.front().radarProfile.profileId,
      QStringLiteral("generic"));
  EXPECT_DOUBLE_EQ(
      loaded.entities.front().sensors.front().radarProfile.peakPowerWatts,
      25000.0);
}

TEST(ScenarioSerializer, LegacyF16InfersCesiumModelAxes) {
  QTemporaryFile file;
  ASSERT_TRUE(file.open());
  file.write(R"({
    "waypoints": [],
    "routes": [],
    "areas": [],
    "entities": [{
      "name": "LegacyF16",
      "modelUri": "file:///models/Air/Fighter/f-16_fighting_falcon.glb"
    }]
  })");
  file.close();

  const ScenarioSnapshot loaded = loadScenario(file.fileName());

  ASSERT_EQ(loaded.entities.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().cesiumModelAxes,
      QStringLiteral("x-forward-y-up"));
}

TEST(ScenarioSerializer, LegacyMirageInfersCesiumModelAxes) {
  QTemporaryFile file;
  ASSERT_TRUE(file.open());
  file.write(R"({
    "waypoints": [],
    "routes": [],
    "areas": [],
    "entities": [{
      "name": "LegacyMirage",
      "modelUri": "file:///models/Air/Fighter/dassault_mirage_2000.glb"
    }]
  })");
  file.close();

  const ScenarioSnapshot loaded = loadScenario(file.fileName());

  ASSERT_EQ(loaded.entities.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().cesiumModelAxes,
      QStringLiteral("x-forward-y-up"));
}

TEST(ScenarioSerializer, LegacyA10InfersZForwardCesiumModelAxes) {
  QTemporaryFile file;
  ASSERT_TRUE(file.open());
  file.write(R"({
    "waypoints": [],
    "routes": [],
    "areas": [],
    "entities": [{
      "name": "LegacyA10",
      "modelUri": "file:///models/Air/Fighter/a-10_thunderbolt_ii.glb"
    }]
  })");
  file.close();

  const ScenarioSnapshot loaded = loadScenario(file.fileName());

  ASSERT_EQ(loaded.entities.size(), 1);
  EXPECT_EQ(
      loaded.entities.front().cesiumModelAxes,
      QStringLiteral("z-forward-y-up"));
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
