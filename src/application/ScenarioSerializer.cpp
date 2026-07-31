#include "application/ScenarioSerializer.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>

// ── Local normalization helpers ───────────────────────────────────────────────

namespace {

QStringList supportedBehaviorModes() {
  return {
      QStringLiteral("Manual"),
      QStringLiteral("Aggressive"),
      QStringLiteral("Defensive"),
      QStringLiteral("Patrol"),
  };
}

QString normalizedBehaviorMode(const QString& behaviorMode) {
  const QString trimmed = behaviorMode.trimmed();
  for (const QString& mode : supportedBehaviorModes()) {
    if (mode.compare(trimmed, Qt::CaseInsensitive) == 0) {
      return mode;
    }
  }
  return QStringLiteral("Manual");
}

bool entityIsGroundDomain(const Entity& entity) {
  return entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
}

void normalizeGroundEntity(Entity& entity) {
  if (!entityIsGroundDomain(entity)) {
    return;
  }
  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
  entity.flightDynamicsEnabled = false;
  entity.flightDynamicsMode = QStringLiteral("kinematic");
  entity.jsbsimAircraftModel.clear();
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 0.0;
  entity.currentTask = EntityTask{};
}

// ── Weapon ────────────────────────────────────────────────────────────────────

QJsonObject toJson(const WeaponInventoryItem& weapon) {
  return {
      {QStringLiteral("weaponType"), weapon.weaponType},
      {QStringLiteral("quantity"), weapon.quantity},
  };
}

WeaponInventoryItem weaponInventoryItemFromJson(const QJsonObject& object) {
  WeaponInventoryItem item;
  item.weaponType = object.value(QStringLiteral("weaponType")).toString();
  item.quantity = object.value(QStringLiteral("quantity")).toInt(0);
  return item;
}

// ── Sensor ────────────────────────────────────────────────────────────────────

QJsonObject toJson(const SensorDefinition& sensor) {
  return {
      {QStringLiteral("id"), sensor.id},
      {QStringLiteral("name"), sensor.name},
      {QStringLiteral("sensorType"), sensor.sensorType},
      {QStringLiteral("sensorSubType"), sensor.sensorSubType},
      {QStringLiteral("enabled"), sensor.enabled},
      {QStringLiteral("emitting"), sensor.emitting},
      {QStringLiteral("maxRangeMeters"), sensor.maxRangeMeters},
      {QStringLiteral("minRangeMeters"), sensor.minRangeMeters},
      {QStringLiteral("azimuthCenterDegrees"), sensor.azimuthCenterDegrees},
      {QStringLiteral("azimuthWidthDegrees"), sensor.azimuthWidthDegrees},
      {QStringLiteral("elevationCenterDegrees"), sensor.elevationCenterDegrees},
      {QStringLiteral("elevationWidthDegrees"), sensor.elevationWidthDegrees},
      {QStringLiteral("updatePeriodSeconds"), sensor.updatePeriodSeconds},
      {QStringLiteral("maxTracks"), sensor.maxTracks},
      {QStringLiteral("iffCapable"), sensor.iffCapable},
      {QStringLiteral("canDetectAir"), sensor.canDetectAir},
      {QStringLiteral("canDetectGround"), sensor.canDetectGround},
      {QStringLiteral("canDetectSurface"), sensor.canDetectSurface},
      {QStringLiteral("terrainMaskingEnabled"), sensor.terrainMaskingEnabled},
      {QStringLiteral("probabilityOfDetection"), sensor.probabilityOfDetection},
  };
}

SensorDefinition sensorFromJson(const QJsonObject& object) {
  SensorDefinition sensor;
  sensor.id = object.value(QStringLiteral("id")).toString();
  sensor.name = object.value(QStringLiteral("name")).toString();
  sensor.sensorType = object.value(QStringLiteral("sensorType")).toString(QStringLiteral("radar"));
  sensor.sensorSubType =
      object.value(QStringLiteral("sensorSubType")).toString(QStringLiteral("generic"));
  sensor.enabled = object.value(QStringLiteral("enabled")).toBool(true);
  sensor.emitting = object.value(QStringLiteral("emitting")).toBool(true);
  sensor.maxRangeMeters = object.value(QStringLiteral("maxRangeMeters")).toDouble(250000.0);
  sensor.minRangeMeters = object.value(QStringLiteral("minRangeMeters")).toDouble(0.0);
  sensor.azimuthCenterDegrees = object.value(QStringLiteral("azimuthCenterDegrees")).toDouble(0.0);
  sensor.azimuthWidthDegrees = object.value(QStringLiteral("azimuthWidthDegrees")).toDouble(360.0);
  sensor.elevationCenterDegrees = object.value(QStringLiteral("elevationCenterDegrees")).toDouble(0.0);
  sensor.elevationWidthDegrees = object.value(QStringLiteral("elevationWidthDegrees")).toDouble(90.0);
  sensor.updatePeriodSeconds = object.value(QStringLiteral("updatePeriodSeconds")).toDouble(5.0);
  sensor.maxTracks = object.value(QStringLiteral("maxTracks")).toInt(32);
  sensor.iffCapable = object.value(QStringLiteral("iffCapable")).toBool(true);
  sensor.canDetectAir = object.value(QStringLiteral("canDetectAir")).toBool(true);
  sensor.canDetectGround = object.value(QStringLiteral("canDetectGround")).toBool(false);
  sensor.canDetectSurface = object.value(QStringLiteral("canDetectSurface")).toBool(false);
  sensor.terrainMaskingEnabled = object.value(QStringLiteral("terrainMaskingEnabled")).toBool(false);
  sensor.probabilityOfDetection = object.value(QStringLiteral("probabilityOfDetection")).toDouble(1.0);
  return sensor;
}

// ── SensorContact ─────────────────────────────────────────────────────────────

QJsonObject toJson(const SensorContact& contact) {
  return {
      {QStringLiteral("sensorId"), contact.sensorId},
      {QStringLiteral("sensorType"), contact.sensorType},
      {QStringLiteral("sensorSubType"), contact.sensorSubType},
      {QStringLiteral("targetEntityName"), contact.targetEntityName},
      {QStringLiteral("rangeMeters"), contact.rangeMeters},
      {QStringLiteral("bearingDegrees"), contact.bearingDegrees},
      {QStringLiteral("lineOfSight"), contact.lineOfSight},
      {QStringLiteral("detected"), contact.detected},
  };
}

SensorContact sensorContactFromJson(const QJsonObject& object) {
  SensorContact contact;
  contact.sensorId = object.value(QStringLiteral("sensorId")).toString();
  contact.sensorType = object.value(QStringLiteral("sensorType")).toString();
  contact.sensorSubType = object.value(QStringLiteral("sensorSubType")).toString();
  contact.targetEntityName = object.value(QStringLiteral("targetEntityName")).toString();
  contact.rangeMeters = object.value(QStringLiteral("rangeMeters")).toDouble(0.0);
  contact.bearingDegrees = object.value(QStringLiteral("bearingDegrees")).toDouble(0.0);
  contact.lineOfSight = object.value(QStringLiteral("lineOfSight")).toBool(true);
  contact.detected = object.value(QStringLiteral("detected")).toBool(false);
  return contact;
}

// ── EntityTask ────────────────────────────────────────────────────────────────

QJsonObject toJson(const EntityTask& task) {
  return {
      {QStringLiteral("taskType"), task.taskType},
      {QStringLiteral("enabled"), task.enabled},
      {QStringLiteral("status"), task.status},
      {QStringLiteral("targetHeadingDegrees"), task.targetHeadingDegrees},
      {QStringLiteral("targetAltitudeMeters"), task.targetAltitudeMeters},
      {QStringLiteral("targetSpeedKnots"), task.targetSpeedKnots},
      {QStringLiteral("targetLatitude"), task.targetLatitude},
      {QStringLiteral("targetLongitude"), task.targetLongitude},
      {QStringLiteral("targetEntityName"), task.targetEntityName},
      {QStringLiteral("targetWaypointName"), task.targetWaypointName},
      {QStringLiteral("targetRouteName"), task.targetRouteName},
      {QStringLiteral("targetAreaName"), task.targetAreaName},
      {QStringLiteral("targetAreaRadiusMeters"), task.targetAreaRadiusMeters},
      {QStringLiteral("followDistanceMeters"), task.followDistanceMeters},
      {QStringLiteral("arrivalToleranceMeters"), task.arrivalToleranceMeters},
      {QStringLiteral("durationSeconds"), task.durationSeconds},
      {QStringLiteral("elapsedSeconds"), task.elapsedSeconds},
      {QStringLiteral("routeCurrentWaypointIndex"), task.routeCurrentWaypointIndex},
      {QStringLiteral("routeTotalWaypoints"), task.routeTotalWaypoints},
      {QStringLiteral("interceptDistanceMeters"), task.interceptDistanceMeters},
      {QStringLiteral("altitudeToleranceMeters"), task.altitudeToleranceMeters},
      {QStringLiteral("timeoutSeconds"), task.timeoutSeconds},
      {QStringLiteral("maxEngagementTimeSeconds"), task.maxEngagementTimeSeconds},
      {QStringLiteral("shotCooldownSeconds"), task.shotCooldownSeconds},
      {QStringLiteral("racetrackLegLengthMeters"), task.racetrackLegLengthMeters},
      {QStringLiteral("weaponType"), task.weaponType},
      {QStringLiteral("targetDomain"), task.targetDomain},
      {QStringLiteral("enemyOnly"), task.enemyOnly},
      {QStringLiteral("damageThresholdPercent"), task.damageThresholdPercent},
      {QStringLiteral("rangeMeters"), task.rangeMeters},
  };
}

EntityTask taskFromJson(const QJsonObject& object) {
  EntityTask task;
  task.taskType = object.value(QStringLiteral("taskType")).toString();
  if (task.taskType == QStringLiteral("InterceptEntity2D") ||
      task.taskType == QStringLiteral("InterceptEntity3D")) {
    task.taskType = QStringLiteral("InterceptEntity");
  }
  task.enabled = object.value(QStringLiteral("enabled")).toBool(false);
  task.status = object.value(QStringLiteral("status")).toString(QStringLiteral("Idle"));
  task.targetHeadingDegrees = object.value(QStringLiteral("targetHeadingDegrees")).toDouble(0.0);
  task.targetAltitudeMeters = object.value(QStringLiteral("targetAltitudeMeters")).toInt(0);
  task.targetSpeedKnots = object.value(QStringLiteral("targetSpeedKnots")).toDouble(0.0);
  task.targetLatitude = object.value(QStringLiteral("targetLatitude")).toDouble(0.0);
  task.targetLongitude = object.value(QStringLiteral("targetLongitude")).toDouble(0.0);
  task.targetEntityName = object.value(QStringLiteral("targetEntityName")).toString();
  task.targetWaypointName = object.value(QStringLiteral("targetWaypointName")).toString();
  task.targetRouteName = object.value(QStringLiteral("targetRouteName")).toString();
  task.targetAreaName = object.value(QStringLiteral("targetAreaName")).toString();
  task.targetAreaRadiusMeters = object.value(QStringLiteral("targetAreaRadiusMeters")).toDouble(0.0);
  task.followDistanceMeters = object.value(QStringLiteral("followDistanceMeters")).toDouble(1000.0);
  task.arrivalToleranceMeters = object.value(QStringLiteral("arrivalToleranceMeters")).toDouble(100.0);
  task.durationSeconds = object.value(QStringLiteral("durationSeconds")).toDouble(0.0);
  task.elapsedSeconds = object.value(QStringLiteral("elapsedSeconds")).toDouble(0.0);
  task.routeCurrentWaypointIndex = object.value(QStringLiteral("routeCurrentWaypointIndex")).toInt(0);
  task.routeTotalWaypoints = object.value(QStringLiteral("routeTotalWaypoints")).toInt(0);
  task.interceptDistanceMeters = object.value(QStringLiteral("interceptDistanceMeters")).toDouble(500.0);
  task.altitudeToleranceMeters = object.value(QStringLiteral("altitudeToleranceMeters")).toDouble(100.0);
  task.timeoutSeconds = object.value(QStringLiteral("timeoutSeconds")).toDouble(120.0);
  task.maxEngagementTimeSeconds =
      object.value(QStringLiteral("maxEngagementTimeSeconds")).toDouble(120.0);
  task.shotCooldownSeconds =
      object.value(QStringLiteral("shotCooldownSeconds")).toDouble(8.0);
  task.racetrackLegLengthMeters =
      object.value(QStringLiteral("racetrackLegLengthMeters")).toDouble(10000.0);
  task.weaponType = object.value(QStringLiteral("weaponType")).toString();
  task.targetDomain = object.value(QStringLiteral("targetDomain")).toString();
  task.enemyOnly = object.value(QStringLiteral("enemyOnly")).toBool(true);
  task.damageThresholdPercent =
      object.value(QStringLiteral("damageThresholdPercent")).toDouble(50.0);
  task.rangeMeters = object.value(QStringLiteral("rangeMeters")).toDouble(1000.0);
  return task;
}

// ── Entity ────────────────────────────────────────────────────────────────────

QJsonObject toJson(const Entity& entity) {
  QJsonArray sensors;
  for (const SensorDefinition& sensor : entity.sensors) {
    sensors.append(toJson(sensor));
  }
  QJsonArray contacts;
  for (const SensorContact& contact : entity.sensorContacts) {
    contacts.append(toJson(contact));
  }
  QJsonArray weapons;
  for (const WeaponInventoryItem& weapon : entity.weapons) {
    weapons.append(toJson(weapon));
  }
  return {
      {QStringLiteral("name"), entity.name},
      {QStringLiteral("type"), entity.type},
      {QStringLiteral("domain"), entity.domain},
      {QStringLiteral("category"), entity.category},
      {QStringLiteral("entityTypeCode"), entity.entityTypeCode},
      {QStringLiteral("callsign"), entity.callsign},
      {QStringLiteral("forceIdentifier"), entity.forceIdentifier},
      {QStringLiteral("latitude"), entity.latitude},
      {QStringLiteral("longitude"), entity.longitude},
      {QStringLiteral("groundHeight"), entity.groundHeight},
      {QStringLiteral("altitude"), entity.altitude},
      {QStringLiteral("modelName"), entity.modelName},
      {QStringLiteral("modelUri"), entity.modelUri},
      {QStringLiteral("entityKind"), entity.entityKind},
      {QStringLiteral("entityDomain"), entity.entityDomain},
      {QStringLiteral("entityCountry"), entity.entityCountry},
      {QStringLiteral("entityCategory"), entity.entityCategory},
      {QStringLiteral("entitySubcategory"), entity.entitySubcategory},
      {QStringLiteral("entitySpecific"), entity.entitySpecific},
      {QStringLiteral("entityExtra"), entity.entityExtra},
      {QStringLiteral("headingDegrees"), entity.headingDegrees},
      {QStringLiteral("flightDynamicsEnabled"), entity.flightDynamicsEnabled},
      {QStringLiteral("flightDynamicsMode"), entity.flightDynamicsMode},
      {QStringLiteral("jsbsimAircraftModel"), entity.jsbsimAircraftModel},
      {QStringLiteral("systemsDisplayProfileId"), entity.systemsDisplayProfileId},
      {QStringLiteral("engineCount"), entity.engineCount},
      {QStringLiteral("speedKnots"), entity.speedKnots},
      {QStringLiteral("verticalSpeedMetersPerSecond"), entity.verticalSpeedMetersPerSecond},
      {QStringLiteral("destroyed"), entity.destroyed},
      {QStringLiteral("damagePercent"), entity.damagePercent},
      {QStringLiteral("behaviorMode"), normalizedBehaviorMode(entity.behaviorMode)},
      {QStringLiteral("currentTask"), toJson(entity.currentTask)},
      {QStringLiteral("weapons"), weapons},
      {QStringLiteral("sensors"), sensors},
      {QStringLiteral("sensorContacts"), contacts},
  };
}

Entity entityFromJson(const QJsonObject& object) {
  Entity entity;
  entity.name = object.value(QStringLiteral("name")).toString();
  entity.type = object.value(QStringLiteral("type")).toString(QStringLiteral("Entity"));
  entity.domain = object.value(QStringLiteral("domain")).toString(QStringLiteral("Air"));
  entity.category = object.value(QStringLiteral("category")).toString(QStringLiteral("Fighter"));
  entity.entityTypeCode = object.value(QStringLiteral("entityTypeCode")).toString();
  entity.callsign = object.value(QStringLiteral("callsign")).toString();
  entity.forceIdentifier = object.value(QStringLiteral("forceIdentifier")).toInt(1);
  entity.latitude = object.value(QStringLiteral("latitude")).toDouble(0.0);
  entity.longitude = object.value(QStringLiteral("longitude")).toDouble(0.0);
  entity.groundHeight = object.value(QStringLiteral("groundHeight")).toDouble(0.0);
  entity.altitude = object.value(QStringLiteral("altitude")).toInt(0);
  entity.modelName = object.value(QStringLiteral("modelName")).toString();
  entity.modelUri = object.value(QStringLiteral("modelUri")).toString();
  entity.entityKind = object.value(QStringLiteral("entityKind")).toInt(0);
  entity.entityDomain = object.value(QStringLiteral("entityDomain")).toInt(0);
  entity.entityCountry = object.value(QStringLiteral("entityCountry")).toInt(0);
  entity.entityCategory = object.value(QStringLiteral("entityCategory")).toInt(0);
  entity.entitySubcategory = object.value(QStringLiteral("entitySubcategory")).toInt(0);
  entity.entitySpecific = object.value(QStringLiteral("entitySpecific")).toInt(0);
  entity.entityExtra = object.value(QStringLiteral("entityExtra")).toInt(0);
  entity.headingDegrees = object.value(QStringLiteral("headingDegrees")).toDouble(0.0);
  entity.flightDynamicsEnabled = object.value(QStringLiteral("flightDynamicsEnabled")).toBool(false);
  entity.flightDynamicsMode = object.value(QStringLiteral("flightDynamicsMode")).toString(QStringLiteral("kinematic"));
  entity.jsbsimAircraftModel = object.value(QStringLiteral("jsbsimAircraftModel")).toString();
  entity.systemsDisplayProfileId =
      object.value(QStringLiteral("systemsDisplayProfileId")).toString();
  entity.engineCount = qMax(
      0,
      object.value(QStringLiteral("engineCount")).toInt(0));
  entity.speedKnots = object.value(QStringLiteral("speedKnots")).toDouble(0.0);
  entity.verticalSpeedMetersPerSecond = object.value(QStringLiteral("verticalSpeedMetersPerSecond")).toDouble(0.0);
  entity.destroyed = object.value(QStringLiteral("destroyed")).toBool(false);
  entity.damagePercent = qBound(
      0.0,
      object.value(QStringLiteral("damagePercent")).toDouble(0.0),
      100.0);
  entity.behaviorMode =
      normalizedBehaviorMode(
          object.value(QStringLiteral("behaviorMode")).toString(QStringLiteral("Manual")));
  entity.currentTask = taskFromJson(object.value(QStringLiteral("currentTask")).toObject());

  const QJsonArray weapons = object.value(QStringLiteral("weapons")).toArray();
  for (const QJsonValue& value : weapons) {
    entity.weapons.push_back(weaponInventoryItemFromJson(value.toObject()));
  }
  const QJsonArray sensors = object.value(QStringLiteral("sensors")).toArray();
  for (const QJsonValue& value : sensors) {
    entity.sensors.push_back(sensorFromJson(value.toObject()));
  }
  const QJsonArray contacts = object.value(QStringLiteral("sensorContacts")).toArray();
  for (const QJsonValue& value : contacts) {
    entity.sensorContacts.push_back(sensorContactFromJson(value.toObject()));
  }

  normalizeGroundEntity(entity);
  if (entity.entityTypeCode.isEmpty()) {
    entity.refreshEntityTypeCode();
  }
  return entity;
}

// ── Waypoint ──────────────────────────────────────────────────────────────────

QJsonObject toJson(const Waypoint& waypoint) {
  QJsonObject object{
      {QStringLiteral("name"), waypoint.name},
      {QStringLiteral("latitude"), waypoint.latitude},
      {QStringLiteral("longitude"), waypoint.longitude},
  };
  if (waypoint.altitudeMetersSet || waypoint.altitudeMeters != 0.0) {
    object.insert(QStringLiteral("altitudeMeters"), waypoint.altitudeMeters);
  }
  return object;
}

Waypoint waypointFromJson(const QJsonObject& object) {
  Waypoint waypoint;
  waypoint.name = object.value(QStringLiteral("name")).toString();
  waypoint.latitude = object.value(QStringLiteral("latitude")).toDouble(0.0);
  waypoint.longitude = object.value(QStringLiteral("longitude")).toDouble(0.0);
  waypoint.altitudeMetersSet = object.contains(QStringLiteral("altitudeMeters"));
  waypoint.altitudeMeters = object.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
  return waypoint;
}

// ── RouteGraphic ──────────────────────────────────────────────────────────────

QJsonObject toJson(const RouteGraphic& route) {
  QJsonArray points;
  for (const RoutePoint& point : route.points) {
    QJsonObject pointObject{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
    };
    if (point.altitudeMetersSet || point.altitudeMeters != 0.0) {
      pointObject.insert(QStringLiteral("altitudeMeters"), point.altitudeMeters);
    }
    points.append(pointObject);
  }
  return {
      {QStringLiteral("name"), route.name},
      {QStringLiteral("points"), points},
  };
}

RouteGraphic routeFromJson(const QJsonObject& object) {
  RouteGraphic route;
  route.name = object.value(QStringLiteral("name")).toString();
  const QJsonArray points = object.value(QStringLiteral("points")).toArray();
  for (const QJsonValue& value : points) {
    const QJsonObject pointObject = value.toObject();
    RoutePoint point;
    point.latitude = pointObject.value(QStringLiteral("latitude")).toDouble(0.0);
    point.longitude = pointObject.value(QStringLiteral("longitude")).toDouble(0.0);
    point.altitudeMetersSet = pointObject.contains(QStringLiteral("altitudeMeters"));
    point.altitudeMeters = pointObject.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
    route.points.push_back(point);
  }
  return route;
}

// ── AreaDefinition ────────────────────────────────────────────────────────────

QJsonObject toJson(const AreaDefinition& area) {
  QJsonArray points;
  for (const RoutePoint& point : area.points) {
    QJsonObject pointObject{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
    };
    if (point.altitudeMetersSet || point.altitudeMeters != 0.0) {
      pointObject.insert(QStringLiteral("altitudeMeters"), point.altitudeMeters);
    }
    points.append(pointObject);
  }
  return {
      {QStringLiteral("id"), area.id},
      {QStringLiteral("name"), area.name},
      {QStringLiteral("areaType"), area.areaType},
      {QStringLiteral("forceIdentifier"), area.forceIdentifier},
      {QStringLiteral("centerLatitude"), area.centerLatitude},
      {QStringLiteral("centerLongitude"), area.centerLongitude},
      {QStringLiteral("centerAltitudeMeters"), area.centerAltitudeMeters},
      {QStringLiteral("radiusMeters"), area.radiusMeters},
      {QStringLiteral("semiMajorAxisMeters"), area.semiMajorAxisMeters},
      {QStringLiteral("semiMinorAxisMeters"), area.semiMinorAxisMeters},
      {QStringLiteral("rotationDegrees"), area.rotationDegrees},
      {QStringLiteral("points"), points},
      {QStringLiteral("minAltitudeMeters"), area.minAltitudeMeters},
      {QStringLiteral("maxAltitudeMeters"), area.maxAltitudeMeters},
      {QStringLiteral("notes"), area.notes},
  };
}

AreaDefinition areaFromJson(const QJsonObject& object) {
  AreaDefinition area;
  area.id = object.value(QStringLiteral("id")).toString();
  area.name = object.value(QStringLiteral("name")).toString();
  area.areaType = object.value(QStringLiteral("areaType")).toString(QStringLiteral("Circle"));
  area.forceIdentifier = object.value(QStringLiteral("forceIdentifier")).toInt(0);
  area.centerLatitude = object.value(QStringLiteral("centerLatitude")).toDouble(0.0);
  area.centerLongitude = object.value(QStringLiteral("centerLongitude")).toDouble(0.0);
  area.centerAltitudeMeters = object.value(QStringLiteral("centerAltitudeMeters")).toDouble(0.0);
  area.radiusMeters = object.value(QStringLiteral("radiusMeters")).toDouble(1000.0);
  area.semiMajorAxisMeters = object.value(QStringLiteral("semiMajorAxisMeters")).toDouble(1000.0);
  area.semiMinorAxisMeters = object.value(QStringLiteral("semiMinorAxisMeters")).toDouble(600.0);
  area.rotationDegrees = object.value(QStringLiteral("rotationDegrees")).toDouble(0.0);
  const QJsonArray points = object.value(QStringLiteral("points")).toArray();
  for (const QJsonValue& value : points) {
    const QJsonObject pointObject = value.toObject();
    RoutePoint point;
    point.latitude = pointObject.value(QStringLiteral("latitude")).toDouble(0.0);
    point.longitude = pointObject.value(QStringLiteral("longitude")).toDouble(0.0);
    point.altitudeMetersSet = pointObject.contains(QStringLiteral("altitudeMeters"));
    point.altitudeMeters = pointObject.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
    area.points.push_back(point);
  }
  area.minAltitudeMeters = object.value(QStringLiteral("minAltitudeMeters")).toDouble(0.0);
  area.maxAltitudeMeters = object.value(QStringLiteral("maxAltitudeMeters")).toDouble(0.0);
  area.notes = object.value(QStringLiteral("notes")).toString();
  return area;
}

} // namespace

// ── Public API ────────────────────────────────────────────────────────────────

namespace application {

bool saveScenario(const QString& filePath, const ScenarioSnapshot& snapshot) {
  QJsonArray entities;
  for (const Entity& entity : snapshot.entities) {
    entities.append(toJson(entity));
  }
  QJsonArray waypoints;
  for (const Waypoint& waypoint : snapshot.waypoints) {
    waypoints.append(toJson(waypoint));
  }
  QJsonArray routes;
  for (const RouteGraphic& route : snapshot.routes) {
    routes.append(toJson(route));
  }
  QJsonArray areas;
  for (const AreaDefinition& area : snapshot.areas) {
    areas.append(toJson(area));
  }

  QFile file(filePath);
  const QFileInfo info(file);
  QDir().mkpath(info.absolutePath());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  const QJsonDocument document(QJsonObject{
      {QStringLiteral("entities"), entities},
      {QStringLiteral("waypoints"), waypoints},
      {QStringLiteral("routes"), routes},
      {QStringLiteral("areas"), areas},
  });
  file.write(document.toJson(QJsonDocument::Indented));
  return true;
}

ScenarioSnapshot loadScenario(const QString& filePath) {
  ScenarioSnapshot snapshot;

  QFile file(filePath);
  if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return snapshot;
  }

  const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
  if (!document.isObject()) {
    return snapshot;
  }

  const QJsonObject root = document.object();

  for (const QJsonValue& value : root.value(QStringLiteral("entities")).toArray()) {
    Entity entity = entityFromJson(value.toObject());
    // Reset runtime state on load (matches ScenarioState::load() contract).
    entity.currentTask = EntityTask{};
    entity.flightDynamicsEnabled = false;
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    snapshot.entities.push_back(entity);
  }

  for (const QJsonValue& value : root.value(QStringLiteral("waypoints")).toArray()) {
    snapshot.waypoints.push_back(waypointFromJson(value.toObject()));
  }
  for (const QJsonValue& value : root.value(QStringLiteral("routes")).toArray()) {
    snapshot.routes.push_back(routeFromJson(value.toObject()));
  }
  for (const QJsonValue& value : root.value(QStringLiteral("areas")).toArray()) {
    snapshot.areas.push_back(areaFromJson(value.toObject()));
  }

  return snapshot;
}

} // namespace application
