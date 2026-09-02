#include "application/HlaCombatDemoScenario.h"

namespace application {
namespace {

SensorDefinition airborneRadar(const QString& id, const QString& name) {
  SensorDefinition radar;
  radar.id = id;
  radar.name = name;
  radar.sensorType = QStringLiteral("radar");
  radar.sensorSubType = QStringLiteral("airborne-radar");
  radar.enabled = true;
  radar.emitting = true;
  radar.maxRangeMeters = 120000.0;
  radar.azimuthWidthDegrees = 120.0;
  radar.elevationWidthDegrees = 60.0;
  radar.updatePeriodSeconds = 0.2;
  radar.probabilityOfDetection = 1.0;
  radar.radarProfile.peakPowerWatts = 100000.0;
  return radar;
}

Entity fighter(
    const QString& id,
    const QString& name,
    int forceIdentifier,
    double latitude,
    double longitude,
    const QString& modelUri) {
  Entity entity;
  entity.entityId = id;
  entity.name = name;
  entity.callsign = name;
  entity.type = QStringLiteral("Fighter");
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.forceIdentifier = forceIdentifier;
  entity.latitude = latitude;
  entity.longitude = longitude;
  entity.altitude = 4500;
  entity.headingDegrees = 90.0;
  entity.speedKnots = 300.0;
  entity.flightDynamicsEnabled = true;
  entity.flightDynamicsMode = QStringLiteral("kinematic");
  entity.modelName = name;
  entity.modelUri = modelUri;
  entity.cesiumModelAxes = QStringLiteral("x-forward-y-up");
  entity.entityKind = 1;
  entity.entityDomain = 2;
  entity.entityCountry = 225;
  entity.entityCategory = 1;
  entity.entitySubcategory = 1;
  entity.radarSignature = 10.0;
  entity.fuelCapacityKilograms = 5000.0;
  entity.fuelRemainingKilograms = 4500.0;
  entity.refreshEntityTypeCode();
  return entity;
}

} // namespace

HlaCombatDemoScenario makeHlaCombatDemoScenario() {
  HlaCombatDemoScenario demo;
  demo.friendly = fighter(
      QStringLiteral("hla-demo-friendly"),
      QStringLiteral("Blue Mirage 2000"),
      1,
      40.40,
      -3.78,
      QStringLiteral("/models/Air/Fighter/dassault_mirage_2000.glb"));
  demo.friendly.type = QStringLiteral("Dassault Mirage 2000");
  demo.friendly.modelName = QStringLiteral("Mirage 2000");
  demo.opposing = fighter(
      QStringLiteral("hla-demo-opposing"),
      QStringLiteral("Red Bandit"),
      2,
      40.41,
      -3.70,
      QStringLiteral("/models/Air/Fighter/f-16_fighting_falcon.glb"));

  demo.friendly.speedKnots = 440.0;
  // Keep enough rounds for the graphical demo to tolerate missed shots while
  // still exercising the sustained Attack Until Destroyed lifecycle.
  demo.friendly.weapons.push_back({QStringLiteral("Missile"), 16});
  demo.friendly.sensors.push_back(airborneRadar(
      QStringLiteral("blue-air-radar"), QStringLiteral("Blue Air Radar")));
  demo.opposing.speedKnots = 240.0;
  demo.opposing.sensors.push_back(airborneRadar(
      QStringLiteral("red-air-radar"), QStringLiteral("Red Air Radar")));

  demo.opposingMovementTask.taskType =
      QStringLiteral("FlyHeadingAltitudeSpeed");
  demo.opposingMovementTask.enabled = true;
  demo.opposingMovementTask.status = QStringLiteral("Running");
  demo.opposingMovementTask.targetHeadingDegrees = 90.0;
  demo.opposingMovementTask.targetAltitudeMeters = demo.opposing.altitude;
  demo.opposingMovementTask.targetSpeedKnots = demo.opposing.speedKnots;

  EntityTask follow;
  follow.taskType = QStringLiteral("FollowEntity");
  follow.enabled = true;
  follow.status = QStringLiteral("NotStarted");
  follow.targetEntityId = demo.opposing.entityId;
  follow.targetEntityName = demo.opposing.name;
  follow.followDistanceMeters = 3000.0;
  follow.arrivalToleranceMeters = 500.0;
  follow.targetSpeedKnots = demo.friendly.speedKnots;
  demo.friendlyPlanTasks.push_back(follow);

  EntityTask attack;
  attack.taskType = QStringLiteral("AttackUntilDestroyed");
  attack.enabled = true;
  attack.status = QStringLiteral("NotStarted");
  attack.targetEntityId = demo.opposing.entityId;
  attack.targetEntityName = demo.opposing.name;
  attack.weaponType = QStringLiteral("Missile");
  attack.maxEngagementTimeSeconds = 240.0;
  attack.timeoutSeconds = 240.0;
  attack.shotCooldownSeconds = 5.0;
  demo.friendlyPlanTasks.push_back(attack);

  return demo;
}

} // namespace application
