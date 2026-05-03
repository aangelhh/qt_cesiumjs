#include "application/ScenarioState.h"

#include "application/FlightDynamicsEngine.h"
#include "application/SensorEngine.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QtMath>

namespace {

constexpr double kEarthRadiusMeters = 6371000.0;
constexpr double kKnotsToMetersPerSecond = 0.514444;
constexpr double kDefaultMissileBoostMetersPerSecond = 250.0;
constexpr double kDefaultMissileTtlSeconds = 12.0;
constexpr double kDefaultMissileDamagePercent = 25.0;
constexpr double kDefaultMissileMaxRangeMeters = 60000.0;
constexpr double kDefaultMissileHitRadiusMeters = 120.0;
constexpr double kDefaultMissileTurnRateDegreesPerSecond = 45.0;
constexpr double kDefaultMissilePitchRateDegreesPerSecond = 30.0;
constexpr double kDefaultMissileMaxPitchDegrees = 60.0;
constexpr double kGravityMetersPerSecondSquared = 9.81;
constexpr double kDefaultBombTtlSeconds = 45.0;
constexpr double kDefaultBombHitRadiusMeters = 120.0;
constexpr double kDefaultBombBlastRadiusMeters = 200.0;
constexpr double kDefaultBombBaseDamage = 100.0;
constexpr double kDefaultBombForwardOffsetMeters = 25.0;
constexpr double kDefaultBombDownOffsetMeters = 5.0;
constexpr double kMinimumMunitionAltitudeMeters = 1.0;
constexpr double kLaunchFlashTtlSeconds = 0.25;
constexpr double kImpactFlashTtlSeconds = 0.45;
constexpr double kBombSmokeTrailIntervalSeconds = 0.25;
constexpr double kBombSmokeTrailTtlSeconds = 1.0;
constexpr double kBombImpactFlashTtlSeconds = 0.35;
constexpr double kBombSmokeTtlSeconds = 3.0;
constexpr double kBehaviorAutoMissileCooldownSeconds = 12.0;

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

QString defaultMissileModelUri() {
  const QString path =
      QDir(projectRoot()).absoluteFilePath(QStringLiteral("models/missile/missile.glb"));
  return QUrl::fromLocalFile(path).toString();
}

QString defaultBombModelUri() {
  const QString path =
      QDir(projectRoot()).absoluteFilePath(QStringLiteral("models/bomb/bomb.glb"));
  return QUrl::fromLocalFile(path).toString();
}

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

bool entityCanCarryMissiles(const Entity& entity) {
  return !entity.destroyed &&
         entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0 &&
         entity.category.compare(QStringLiteral("Fighter"), Qt::CaseInsensitive) == 0;
}

bool entityCanCarryBombs(const Entity& entity) {
  return entityCanCarryMissiles(entity);
}

WeaponInventoryItem* findWeaponInventoryItem(
    QVector<WeaponInventoryItem>& weapons,
    const QString& weaponType) {
  for (WeaponInventoryItem& item : weapons) {
    if (item.weaponType.compare(weaponType, Qt::CaseInsensitive) == 0) {
      return &item;
    }
  }
  return nullptr;
}

int weaponQuantity(
    const Entity& entity,
    const QString& weaponType) {
  for (const WeaponInventoryItem& item : entity.weapons) {
    if (item.weaponType.compare(weaponType, Qt::CaseInsensitive) == 0) {
      return item.quantity;
    }
  }
  return 0;
}

bool entityIsGroundDomain(const Entity& entity) {
  return entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
}

void normalizeGroundEntity(Entity& entity) {
  if (!entityIsGroundDomain(entity)) {
    return;
  }

  entity.altitude = 0;
  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
  entity.flightDynamicsEnabled = false;
  entity.flightDynamicsMode = QStringLiteral("kinematic");
  entity.jsbsimAircraftModel.clear();
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 0.0;
  entity.currentTask = EntityTask{};
}

const Entity* findEntityByName(
    const QVector<Entity>& entities,
    const QString& entityName) {
  for (const Entity& entity : entities) {
    if (entity.name == entityName) {
      return &entity;
    }
  }
  return nullptr;
}

bool entityIsValidMissileTarget(
    const Entity& launcher,
    const Entity& target) {
  return !target.destroyed &&
         target.name != launcher.name &&
         target.forceIdentifier != launcher.forceIdentifier &&
         target.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0;
}

double detectedTargetRangeMeters(
    const Entity& launcher,
    const QString& targetName) {
  const QString trimmedTargetName = targetName.trimmed();
  if (trimmedTargetName.isEmpty()) {
    return -1.0;
  }

  double closestRangeMeters = -1.0;
  for (const SensorContact& contact : launcher.sensorContacts) {
    if (!contact.detected ||
        contact.targetEntityName.trimmed().compare(
            trimmedTargetName,
            Qt::CaseInsensitive) != 0) {
      continue;
    }

    if (closestRangeMeters < 0.0 || contact.rangeMeters < closestRangeMeters) {
      closestRangeMeters = contact.rangeMeters;
    }
  }

  return closestRangeMeters;
}

bool entityIsValidBehaviorTarget(
    const Entity& observer,
    const Entity& target) {
  return !target.destroyed &&
         target.name != observer.name &&
         target.forceIdentifier != observer.forceIdentifier;
}

QString selectBestBehaviorTargetName(
    const Entity& observer,
    const QVector<Entity>& entities) {
  QString selectedTargetName;
  double selectedRangeMeters = -1.0;

  for (const SensorContact& contact : observer.sensorContacts) {
    if (!contact.detected || contact.targetEntityName.trimmed().isEmpty()) {
      continue;
    }

    const Entity* target =
        findEntityByName(entities, contact.targetEntityName.trimmed());
    if (!target || !entityIsValidBehaviorTarget(observer, *target)) {
      continue;
    }

    if (selectedRangeMeters < 0.0 || contact.rangeMeters < selectedRangeMeters) {
      selectedRangeMeters = contact.rangeMeters;
      selectedTargetName = target->name;
    }
  }

  return selectedTargetName;
}

bool entityPassesAutoMissileQuickValidation(
    const Entity& launcher,
    const Entity& target) {
  return !launcher.destroyed &&
         !target.destroyed &&
         !launcher.name.trimmed().isEmpty() &&
         !target.name.trimmed().isEmpty() &&
         launcher.name != target.name &&
         launcher.forceIdentifier != target.forceIdentifier &&
         weaponQuantity(launcher, QStringLiteral("Missile")) > 0;
}

double normalizeDegrees360(double degrees) {
  while (degrees < 0.0) {
    degrees += 360.0;
  }
  while (degrees >= 360.0) {
    degrees -= 360.0;
  }
  return degrees;
}

double shortestSignedAngle(double currentDegrees, double targetDegrees) {
  double delta =
      normalizeDegrees360(targetDegrees) - normalizeDegrees360(currentDegrees);
  while (delta > 180.0) {
    delta -= 360.0;
  }
  while (delta < -180.0) {
    delta += 360.0;
  }
  return delta;
}

double clampStep(double currentValue, double targetValue, double maxStep) {
  if (maxStep <= 0.0) {
    return currentValue;
  }
  if (targetValue > currentValue) {
    return qMin(targetValue, currentValue + maxStep);
  }
  return qMax(targetValue, currentValue - maxStep);
}

double bearingDegrees(
    double latitude1,
    double longitude1,
    double latitude2,
    double longitude2) {
  const double lat1 = qDegreesToRadians(latitude1);
  const double lon1 = qDegreesToRadians(longitude1);
  const double lat2 = qDegreesToRadians(latitude2);
  const double lon2 = qDegreesToRadians(longitude2);
  const double deltaLon = lon2 - lon1;

  const double y = qSin(deltaLon) * qCos(lat2);
  const double x = qCos(lat1) * qSin(lat2) -
                   qSin(lat1) * qCos(lat2) * qCos(deltaLon);
  return normalizeDegrees360(qRadiansToDegrees(qAtan2(y, x)));
}

QPair<double, double> destinationPoint(
    double latitude,
    double longitude,
    double bearingDegreesValue,
    double distanceMetersValue) {
  const double angularDistance = distanceMetersValue / kEarthRadiusMeters;
  const double bearing = qDegreesToRadians(bearingDegreesValue);
  const double lat1 = qDegreesToRadians(latitude);
  const double lon1 = qDegreesToRadians(longitude);

  const double sinLat1 = qSin(lat1);
  const double cosLat1 = qCos(lat1);
  const double sinAngular = qSin(angularDistance);
  const double cosAngular = qCos(angularDistance);

  const double lat2 = qAsin(
      sinLat1 * cosAngular +
      cosLat1 * sinAngular * qCos(bearing));
  const double lon2 = lon1 + qAtan2(
      qSin(bearing) * sinAngular * cosLat1,
      cosAngular - sinLat1 * qSin(lat2));

  return {qRadiansToDegrees(lat2), qRadiansToDegrees(lon2)};
}

double distanceMeters(
    double latitude1,
    double longitude1,
    double latitude2,
    double longitude2) {
  const double lat1 = qDegreesToRadians(latitude1);
  const double lon1 = qDegreesToRadians(longitude1);
  const double lat2 = qDegreesToRadians(latitude2);
  const double lon2 = qDegreesToRadians(longitude2);
  const double deltaLat = lat2 - lat1;
  const double deltaLon = lon2 - lon1;
  const double a = qPow(qSin(deltaLat / 2.0), 2.0) +
                   qCos(lat1) * qCos(lat2) * qPow(qSin(deltaLon / 2.0), 2.0);
  const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
  return kEarthRadiusMeters * c;
}

ActiveMunition makeMissileMunition(const Entity& entity, int serial) {
  ActiveMunition munition;
  munition.id = QStringLiteral("%1-missile-%2")
                    .arg(entity.name)
                    .arg(serial);
  munition.launcherEntityName = entity.name;
  munition.forceIdentifier = entity.forceIdentifier;
  munition.munitionType = QStringLiteral("Missile");
  munition.modelUri = defaultMissileModelUri();
  munition.latitude = entity.latitude;
  munition.longitude = entity.longitude;
  munition.altitudeMeters = static_cast<double>(entity.altitude);
  munition.headingDegrees = normalizeDegrees360(entity.headingDegrees);
  munition.pitchDegrees = entity.pitchDegrees;
  munition.rollDegrees = 0.0;
  munition.speedMetersPerSecond =
      qMax(0.0, entity.speedKnots * kKnotsToMetersPerSecond) +
      kDefaultMissileBoostMetersPerSecond;
  munition.verticalSpeedMetersPerSecond = 0.0;
  munition.ttlSeconds = kDefaultMissileTtlSeconds;
  munition.hitRadiusMeters = kDefaultMissileHitRadiusMeters;
  return munition;
}

ActiveMunition makeBombMunition(const Entity& entity, int serial) {
  ActiveMunition munition;
  munition.id = QStringLiteral("%1-bomb-%2")
                    .arg(entity.name)
                    .arg(serial);
  munition.launcherEntityName = entity.name;
  munition.forceIdentifier = entity.forceIdentifier;
  munition.munitionType = QStringLiteral("Bomb");
  munition.modelUri = defaultBombModelUri();
  munition.status = QStringLiteral("Falling");
  munition.headingDegrees = normalizeDegrees360(entity.headingDegrees);
  munition.pitchDegrees = entity.pitchDegrees;
  munition.rollDegrees = 0.0;
  const double pitchRadians = qDegreesToRadians(entity.pitchDegrees);
  const double baseSpeedMetersPerSecond =
      qMax(0.0, entity.speedKnots * kKnotsToMetersPerSecond);
  munition.speedMetersPerSecond =
      qMax(0.0, baseSpeedMetersPerSecond * qCos(pitchRadians));
  munition.verticalSpeedMetersPerSecond = entity.verticalSpeedMetersPerSecond;
  munition.ttlSeconds = kDefaultBombTtlSeconds;
  munition.hitRadiusMeters = kDefaultBombHitRadiusMeters;
  munition.blastRadiusMeters = kDefaultBombBlastRadiusMeters;
  munition.baseDamage = kDefaultBombBaseDamage;

  const auto [offsetLatitude, offsetLongitude] = destinationPoint(
      entity.latitude,
      entity.longitude,
      munition.headingDegrees,
      kDefaultBombForwardOffsetMeters);
  munition.latitude = offsetLatitude;
  munition.longitude = offsetLongitude;
  munition.altitudeMeters = qMax(
      kMinimumMunitionAltitudeMeters,
      static_cast<double>(entity.altitude) - kDefaultBombDownOffsetMeters);
  return munition;
}

bool munitionIsBomb(const ActiveMunition& munition) {
  return munition.munitionType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0;
}

TransientEffect makeTransientEffect(
    const QString& id,
    const QString& effectType,
    int forceIdentifier,
    double latitude,
    double longitude,
    double altitudeMeters,
    double ttlSeconds) {
  TransientEffect effect;
  effect.id = id;
  effect.effectType = effectType;
  effect.forceIdentifier = forceIdentifier;
  effect.latitude = latitude;
  effect.longitude = longitude;
  effect.altitudeMeters = altitudeMeters;
  effect.ttlSeconds = ttlSeconds;
  return effect;
}

void appendBombSmokeTrailEffect(
    QVector<TransientEffect>& effects,
    const ActiveMunition& munition,
    int trailIndex) {
  effects.push_back(makeTransientEffect(
      QStringLiteral("%1-bombtrail-%2").arg(munition.id).arg(trailIndex),
      QStringLiteral("BombSmokeTrail"),
      munition.forceIdentifier,
      munition.latitude,
      munition.longitude,
      munition.altitudeMeters,
      kBombSmokeTrailTtlSeconds));
}

void appendBombImpactEffects(
    QVector<TransientEffect>& effects,
    const ActiveMunition& munition) {
  effects.push_back(makeTransientEffect(
      munition.id + QStringLiteral("-bombimpactflash"),
      QStringLiteral("BombImpactFlash"),
      munition.forceIdentifier,
      munition.latitude,
      munition.longitude,
      munition.altitudeMeters,
      kBombImpactFlashTtlSeconds));
  effects.push_back(makeTransientEffect(
      munition.id + QStringLiteral("-bombsmoke"),
      QStringLiteral("BombSmoke"),
      munition.forceIdentifier,
      munition.latitude,
      munition.longitude,
      munition.altitudeMeters,
      kBombSmokeTtlSeconds));
}

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

QJsonObject toJson(const SensorDefinition& sensor) {
  return {
      {QStringLiteral("id"), sensor.id},
      {QStringLiteral("name"), sensor.name},
      {QStringLiteral("sensorType"), sensor.sensorType},
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
  };
}

EntityTask taskFromJson(const QJsonObject& object) {
  EntityTask task;
  task.taskType = object.value(QStringLiteral("taskType")).toString();
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
  return task;
}

QJsonObject toJson(const Waypoint& waypoint) {
  return {
      {QStringLiteral("name"), waypoint.name},
      {QStringLiteral("latitude"), waypoint.latitude},
      {QStringLiteral("longitude"), waypoint.longitude},
      {QStringLiteral("altitudeMeters"), waypoint.altitudeMeters},
  };
}

Waypoint waypointFromJson(const QJsonObject& object) {
  Waypoint waypoint;
  waypoint.name = object.value(QStringLiteral("name")).toString();
  waypoint.latitude = object.value(QStringLiteral("latitude")).toDouble(0.0);
  waypoint.longitude = object.value(QStringLiteral("longitude")).toDouble(0.0);
  waypoint.altitudeMeters = object.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
  return waypoint;
}

QJsonObject toJson(const RouteGraphic& route) {
  QJsonArray points;
  for (const RoutePoint& point : route.points) {
    points.append(QJsonObject{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
        {QStringLiteral("altitudeMeters"), point.altitudeMeters},
    });
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
    point.altitudeMeters = pointObject.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
    route.points.push_back(point);
  }
  return route;
}

QJsonObject toJson(const AreaDefinition& area) {
  QJsonArray points;
  for (const RoutePoint& point : area.points) {
    points.append(QJsonObject{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
        {QStringLiteral("altitudeMeters"), point.altitudeMeters},
    });
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
    point.altitudeMeters = pointObject.value(QStringLiteral("altitudeMeters")).toDouble(0.0);
    area.points.push_back(point);
  }
  area.minAltitudeMeters = object.value(QStringLiteral("minAltitudeMeters")).toDouble(0.0);
  area.maxAltitudeMeters = object.value(QStringLiteral("maxAltitudeMeters")).toDouble(0.0);
  area.notes = object.value(QStringLiteral("notes")).toString();
  return area;
}

QJsonObject toJson(const SensorContact& contact) {
  return {
      {QStringLiteral("sensorId"), contact.sensorId},
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
  contact.targetEntityName = object.value(QStringLiteral("targetEntityName")).toString();
  contact.rangeMeters = object.value(QStringLiteral("rangeMeters")).toDouble(0.0);
  contact.bearingDegrees = object.value(QStringLiteral("bearingDegrees")).toDouble(0.0);
  contact.lineOfSight = object.value(QStringLiteral("lineOfSight")).toBool(true);
  contact.detected = object.value(QStringLiteral("detected")).toBool(false);
  return contact;
}

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
  entity.speedKnots = object.value(QStringLiteral("speedKnots")).toDouble(0.0);
  entity.verticalSpeedMetersPerSecond = object.value(QStringLiteral("verticalSpeedMetersPerSecond")).toDouble(0.0);
  entity.destroyed = object.value(QStringLiteral("destroyed")).toBool(false);
  entity.damagePercent = qBound(
      0.0,
      object.value(QStringLiteral("damagePercent")).toDouble(0.0),
      100.0);
  entity.behaviorMode =
      normalizedBehaviorMode(object.value(QStringLiteral("behaviorMode")).toString(QStringLiteral("Manual")));
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

} // namespace

ScenarioState::ScenarioState() {
  this->reset();
}

void ScenarioState::addEntity(const Entity& entity) {
  Entity newEntity = entity; // Create a mutable copy
  normalizeGroundEntity(newEntity);
  newEntity.currentTask = EntityTask{}; // CRITICAL: Ensure new entity starts with clean task state
  _entities.push_back(newEntity);
  _taskStacks[newEntity.name] = domain::TaskStack(); // CRITICAL: Initialize empty stack for new entity
  this->refreshSensors();
  this->save();
}

const QVector<Entity>& ScenarioState::entities() const {
  return _entities;
}

const QVector<ActiveMunition>& ScenarioState::activeMunitions() const {
  return _activeMunitions;
}

const QVector<TransientEffect>& ScenarioState::transientEffects() const {
  return _transientEffects;
}

double ScenarioState::missileMaxRangeMeters() {
  return kDefaultMissileMaxRangeMeters;
}

bool ScenarioState::removeEntity(const QString& entityName) {
  for (qsizetype index = 0; index < _entities.size(); ++index) {
    if (_entities.at(index).name == entityName) {
      _entities.removeAt(index);
      _taskStacks.erase(entityName); // Clean up stack for removed entity
      _behaviorMissileCooldownSeconds.erase(entityName);
      this->refreshSensors();
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addWaypoint(const Waypoint& waypoint) {
  for (Waypoint& existing : _waypoints) {
    if (existing.name == waypoint.name) {
      existing = waypoint;
      this->save();
      return;
    }
  }
  _waypoints.push_back(waypoint);
  this->save();
}

const QVector<Waypoint>& ScenarioState::waypoints() const {
  return _waypoints;
}

bool ScenarioState::removeWaypoint(const QString& waypointName) {
  for (qsizetype index = 0; index < _waypoints.size(); ++index) {
    if (_waypoints.at(index).name == waypointName) {
      _waypoints.removeAt(index);
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addRoute(const RouteGraphic& route) {
  for (RouteGraphic& existing : _routes) {
    if (existing.name == route.name) {
      existing = route;
      this->save();
      return;
    }
  }
  _routes.push_back(route);
  this->save();
}

const QVector<RouteGraphic>& ScenarioState::routes() const {
  return _routes;
}

bool ScenarioState::removeRoute(const QString& routeName) {
  for (qsizetype index = 0; index < _routes.size(); ++index) {
    if (_routes.at(index).name == routeName) {
      _routes.removeAt(index);
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addArea(const AreaDefinition& area) {
  for (AreaDefinition& existing : _areas) {
    if (existing.id == area.id || existing.name == area.name) {
      existing = area;
      this->save();
      return;
    }
  }
  _areas.push_back(area);
  this->save();
}

const QVector<AreaDefinition>& ScenarioState::areas() const {
  return _areas;
}

bool ScenarioState::removeArea(const QString& areaName) {
  for (qsizetype index = 0; index < _areas.size(); ++index) {
    if (_areas.at(index).name == areaName || _areas.at(index).id == areaName) {
      _areas.removeAt(index);
      for (Entity& entity : _entities) {
        if ((entity.currentTask.taskType == QStringLiteral("PatrolArea") ||
             entity.currentTask.taskType == QStringLiteral("OrbitArea")) &&
            entity.currentTask.targetAreaName == areaName) {
          entity.currentTask = EntityTask{};
          entity.currentTask.status = QStringLiteral("Idle");
        }
      }
      this->save();
      return true;
    }
  }
  return false;
}

bool ScenarioState::assignTask(const QString& entityName, const EntityTask& task) {
  for (Entity& entity : _entities) {
    if (entity.name == entityName) {
      if (entity.destroyed) {
        return false;
      }
      entity.currentTask = task;
      if (entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") &&
          !entity.currentTask.targetWaypointName.trimmed().isEmpty()) {
        for (const Waypoint& waypoint : _waypoints) {
          if (waypoint.name == entity.currentTask.targetWaypointName) {
            entity.currentTask.targetLatitude = waypoint.latitude;
            entity.currentTask.targetLongitude = waypoint.longitude;
            entity.currentTask.targetAltitudeMeters =
                static_cast<int>(qRound(waypoint.altitudeMeters));
            break;
          }
        }
      }
      if (entity.currentTask.taskType == QStringLiteral("MoveAlongRoute") &&
          !entity.currentTask.targetRouteName.trimmed().isEmpty()) {
        for (const RouteGraphic& route : _routes) {
          if (route.name != entity.currentTask.targetRouteName || route.points.isEmpty()) {
            continue;
          }
          const RoutePoint& point = route.points.last();
          entity.currentTask.targetLatitude = point.latitude;
          entity.currentTask.targetLongitude = point.longitude;
          entity.currentTask.targetAltitudeMeters =
              static_cast<int>(qRound(point.altitudeMeters));
          break;
        }
      }
      if ((entity.currentTask.taskType == QStringLiteral("PatrolArea") ||
           entity.currentTask.taskType == QStringLiteral("OrbitArea")) &&
          !entity.currentTask.targetAreaName.trimmed().isEmpty()) {
        for (const AreaDefinition& area : _areas) {
          if (area.name != entity.currentTask.targetAreaName &&
              area.id != entity.currentTask.targetAreaName) {
            continue;
          }
          entity.currentTask.targetLatitude = area.centerLatitude;
          entity.currentTask.targetLongitude = area.centerLongitude;
          entity.currentTask.targetAltitudeMeters =
              static_cast<int>(qRound(area.centerAltitudeMeters));
          double radiusMeters = area.radiusMeters;
          if (radiusMeters <= 0.0) {
            if (area.areaType == QStringLiteral("Ellipse")) {
              radiusMeters = qMax(area.semiMinorAxisMeters, 100.0);
            } else if (!area.points.isEmpty()) {
              radiusMeters = 250.0;
            } else {
              radiusMeters = 500.0;
            }
          }
          entity.currentTask.targetAreaRadiusMeters = radiusMeters;
          break;
        }
      }
      if (task.enabled) {
        entity.flightDynamicsEnabled = true;
        if (entity.flightDynamicsMode.trimmed().isEmpty()) {
          entity.flightDynamicsMode = QStringLiteral("kinematic");
        }
        if (((entity.currentTask.taskType == QStringLiteral("MoveToLocation")) ||
             (entity.currentTask.taskType == QStringLiteral("MoveToWaypoint")) ||
             (entity.currentTask.taskType == QStringLiteral("MoveAlongRoute")) ||
             (entity.currentTask.taskType == QStringLiteral("PatrolArea")) ||
             (entity.currentTask.taskType == QStringLiteral("OrbitArea"))) &&
            entity.currentTask.targetSpeedKnots <= 0.0) {
          entity.currentTask.targetSpeedKnots =
              entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
              ? 220.0
              : 12.0;
        }
        if (entity.currentTask.taskType == QStringLiteral("FollowEntity") &&
            entity.currentTask.targetSpeedKnots <= 0.0) {
          entity.currentTask.targetSpeedKnots =
              entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
              ? 220.0
              : 12.0;
        }
      }
      this->save();
      return true;
    }
  }
  return false;
}

bool ScenarioState::clearTask(const QString& entityName) {
  EntityTask clearedTask;
  clearedTask.status = QStringLiteral("Idle");
  return this->assignTask(entityName, clearedTask);
}

bool ScenarioState::setEntityDestroyed(const QString& entityName, bool destroyed) {
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }

    if (entity.destroyed == destroyed) {
      return true;
    }

    entity.destroyed = destroyed;
    entity.damagePercent = destroyed
        ? 100.0
        : qMin(entity.damagePercent, 99.0);
    entity.currentTask = EntityTask{};
    entity.currentTask.status = destroyed
        ? QStringLiteral("Destroyed")
        : QStringLiteral("Idle");
    if (destroyed) {
      entity.behaviorTargetEntityName.clear();
      _behaviorMissileCooldownSeconds.erase(entity.name);
    }
    entity.flightDynamicsEnabled = false;
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    entity.sensorContacts.clear();

    if (domain::TaskStack* stack = this->getTaskStack(entityName)) {
      while (!stack->isEmpty()) {
        stack->pop();
      }
    }

    this->refreshSensors();
    this->save();
    return true;
  }
  return false;
}

bool ScenarioState::setEntityBehaviorMode(
    const QString& entityName,
    const QString& behaviorMode) {
  const QString normalizedMode = normalizedBehaviorMode(behaviorMode);
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }

    if (entity.behaviorMode == normalizedMode) {
      return true;
    }

    entity.behaviorMode = normalizedMode;
    if (normalizedMode == QStringLiteral("Manual")) {
      entity.behaviorTargetEntityName.clear();
    }
    _pendingEventLogMessages.push_back(
        QStringLiteral("%1 behavior mode set to %2")
            .arg(entity.name, entity.behaviorMode));
    this->save();
    return true;
  }
  return false;
}

void ScenarioState::applyDamageWithSource(
    const QString& targetName,
    double damageAmount,
    const QString& sourceLabel) {
  const QString trimmedTargetName = targetName.trimmed();
  const double clampedDamageAmount = qMax(0.0, damageAmount);
  const QString trimmedSourceLabel = sourceLabel.trimmed().isEmpty()
      ? QStringLiteral("Unknown")
      : sourceLabel.trimmed();
  if (trimmedTargetName.isEmpty() || clampedDamageAmount <= 0.0) {
    return;
  }

  for (Entity& entity : _entities) {
    if (entity.name != trimmedTargetName || entity.destroyed) {
      continue;
    }

    entity.damagePercent = qBound(
        0.0,
        entity.damagePercent + clampedDamageAmount,
        100.0);

    if (entity.damagePercent >= 100.0) {
      entity.damagePercent = 100.0;
      _pendingEventLogMessages.push_back(
          QStringLiteral("%1 hit %2: Destroyed")
              .arg(trimmedSourceLabel, entity.name));
      this->setEntityDestroyed(entity.name, true);
      return;
    }

    _pendingEventLogMessages.push_back(
        QStringLiteral("%1 hit %2: %3 (%4%)")
            .arg(trimmedSourceLabel, entity.name)
            .arg(entity.damageStateLabel())
            .arg(qRound(entity.damagePercent)));
    this->save();
    return;
  }
}

void ScenarioState::applyMissileDamage(
    const QString& targetName,
    double damageAmount) {
  this->applyDamageWithSource(
      targetName,
      damageAmount,
      QStringLiteral("Missile"));
}

void ScenarioState::applyBombBlastDamage(const ActiveMunition& munition) {
  if (!munitionIsBomb(munition) ||
      munition.blastRadiusMeters <= 0.0 ||
      munition.baseDamage <= 0.0) {
    return;
  }

  struct BlastDamageHit {
    QString targetName;
    double damageAmount = 0.0;
  };

  QVector<BlastDamageHit> hits;
  for (const Entity& entity : _entities) {
    if (entity.destroyed || entity.name == munition.launcherEntityName) {
      continue;
    }

    const double horizontalDistanceMeters = distanceMeters(
        munition.latitude,
        munition.longitude,
        entity.latitude,
        entity.longitude);
    const double verticalSeparationMeters = qAbs(
        munition.altitudeMeters - static_cast<double>(entity.altitude));
    const double slantRangeMeters = qSqrt(
        qPow(horizontalDistanceMeters, 2.0) +
        qPow(verticalSeparationMeters, 2.0));
    if (slantRangeMeters >= munition.blastRadiusMeters) {
      continue;
    }

    const double damageAmount = munition.baseDamage * (
        1.0 - slantRangeMeters / munition.blastRadiusMeters);
    if (damageAmount <= 0.0) {
      continue;
    }

    hits.push_back(BlastDamageHit{entity.name, damageAmount});
  }

  for (const BlastDamageHit& hit : hits) {
    this->applyDamageWithSource(
        hit.targetName,
        hit.damageAmount,
        QStringLiteral("Bomb"));
  }
}

bool ScenarioState::addMissileToEntity(const QString& entityName, int quantity) {
  if (quantity <= 0) {
    return false;
  }

  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryMissiles(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Missile"));
    if (!item) {
      entity.weapons.push_back(
          WeaponInventoryItem{QStringLiteral("Missile"), quantity});
    } else {
      item->quantity += quantity;
    }

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::addBombToEntity(const QString& entityName, int quantity) {
  if (quantity <= 0) {
    return false;
  }

  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryBombs(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Bomb"));
    if (!item) {
      entity.weapons.push_back(
          WeaponInventoryItem{QStringLiteral("Bomb"), quantity});
    } else {
      item->quantity += quantity;
    }

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::launchMissile(const QString& entityName) {
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryMissiles(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Missile"));
    if (!item || item->quantity <= 0) {
      return false;
    }

    --item->quantity;

    ActiveMunition munition = makeMissileMunition(entity, _nextMunitionSerial++);
    _activeMunitions.push_back(munition);
    _transientEffects.push_back(makeTransientEffect(
        munition.id + QStringLiteral("-launch"),
        QStringLiteral("LaunchFlash"),
        entity.forceIdentifier,
        munition.latitude,
        munition.longitude,
        munition.altitudeMeters,
        kLaunchFlashTtlSeconds));

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::releaseBomb(const QString& entityName) {
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryBombs(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Bomb"));
    if (!item || item->quantity <= 0) {
      return false;
    }

    --item->quantity;

    ActiveMunition munition = makeBombMunition(entity, _nextMunitionSerial++);
    _activeMunitions.push_back(munition);

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::launchMissileAt(
    const QString& launcherName,
    const QString& targetName) {
  const QString trimmedLauncherName = launcherName.trimmed();
  const QString trimmedTargetName = targetName.trimmed();
  if (trimmedLauncherName.isEmpty() || trimmedTargetName.isEmpty()) {
    return false;
  }

  const Entity* validatedTarget = nullptr;
  for (const Entity& entity : _entities) {
    if (entity.name != trimmedTargetName) {
      continue;
    }
    validatedTarget = &entity;
    break;
  }
  if (!validatedTarget) {
    return false;
  }

  for (Entity& launcher : _entities) {
    if (launcher.name != trimmedLauncherName) {
      continue;
    }
    if (!entityCanCarryMissiles(launcher) ||
        !entityIsValidMissileTarget(launcher, *validatedTarget)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(launcher.weapons, QStringLiteral("Missile"));
    if (!item || item->quantity <= 0) {
      return false;
    }

    const double targetRangeMeters =
        detectedTargetRangeMeters(launcher, validatedTarget->name);
    if (targetRangeMeters < 0.0 ||
        targetRangeMeters > kDefaultMissileMaxRangeMeters) {
      return false;
    }

    --item->quantity;

    ActiveMunition munition = makeMissileMunition(launcher, _nextMunitionSerial++);
    munition.targetEntityName = validatedTarget->name;
    munition.guidanceActive = true;
    munition.status = QStringLiteral("Tracking");
    _activeMunitions.push_back(munition);
    _transientEffects.push_back(makeTransientEffect(
        munition.id + QStringLiteral("-launch"),
        QStringLiteral("LaunchFlash"),
        launcher.forceIdentifier,
        munition.latitude,
        munition.longitude,
        munition.altitudeMeters,
        kLaunchFlashTtlSeconds));

    this->save();
    return true;
  }

  return false;
}

QStringList ScenarioState::takePendingEventLogMessages() {
  const QStringList messages = _pendingEventLogMessages;
  _pendingEventLogMessages.clear();
  return messages;
}

domain::TaskStack* ScenarioState::getTaskStack(const QString& entityName) {
  return &_taskStacks[entityName];
}

void ScenarioState::refreshSensors() {
  SensorEngine::updateEntityContacts(_entities);
}

void ScenarioState::advanceActiveMunitions(double deltaSeconds) {
  if (deltaSeconds <= 0.0 || _activeMunitions.isEmpty()) {
    return;
  }

  const double maxTurnStepDegrees =
      kDefaultMissileTurnRateDegreesPerSecond * deltaSeconds;
  const double maxPitchStepDegrees =
      kDefaultMissilePitchRateDegreesPerSecond * deltaSeconds;

  for (ActiveMunition& munition : _activeMunitions) {
    if (!munition.active) {
      continue;
    }

    const double previousAgeSeconds = munition.ageSeconds;
    const bool isBomb = munitionIsBomb(munition);
    const bool hasGuidedTarget =
        !isBomb && !munition.targetEntityName.trimmed().isEmpty();
    const Entity* trackedTarget = nullptr;
    if (hasGuidedTarget && munition.guidanceActive) {
      trackedTarget = findEntityByName(_entities, munition.targetEntityName);
      if (trackedTarget && !trackedTarget->destroyed) {
        const double desiredHeadingDegrees = bearingDegrees(
            munition.latitude,
            munition.longitude,
            trackedTarget->latitude,
            trackedTarget->longitude);
        const double headingDeltaDegrees = shortestSignedAngle(
            munition.headingDegrees,
            desiredHeadingDegrees);
        munition.headingDegrees = normalizeDegrees360(
            munition.headingDegrees +
            clampStep(0.0, headingDeltaDegrees, maxTurnStepDegrees));

        const double horizontalDistanceToTargetMeters = distanceMeters(
            munition.latitude,
            munition.longitude,
            trackedTarget->latitude,
            trackedTarget->longitude);
        const double altitudeDeltaMeters =
            static_cast<double>(trackedTarget->altitude) - munition.altitudeMeters;
        const double desiredPitchDegrees = qBound(
            -kDefaultMissileMaxPitchDegrees,
            qRadiansToDegrees(qAtan2(
                altitudeDeltaMeters,
                qMax(1.0, horizontalDistanceToTargetMeters))),
            kDefaultMissileMaxPitchDegrees);
        munition.pitchDegrees = qBound(
            -kDefaultMissileMaxPitchDegrees,
            clampStep(
                munition.pitchDegrees,
                desiredPitchDegrees,
                maxPitchStepDegrees),
            kDefaultMissileMaxPitchDegrees);
        munition.status = QStringLiteral("Tracking");
      } else {
        munition.guidanceActive = false;
        munition.status = QStringLiteral("Lost Target");
        trackedTarget = nullptr;
      }
    }

    double horizontalDistanceMeters = 0.0;
    double verticalDistanceMeters = 0.0;
    if (isBomb) {
      munition.verticalSpeedMetersPerSecond -=
          kGravityMetersPerSecondSquared * deltaSeconds;
      horizontalDistanceMeters =
          qMax(0.0, munition.speedMetersPerSecond * deltaSeconds);
      verticalDistanceMeters =
          munition.verticalSpeedMetersPerSecond * deltaSeconds;
      munition.pitchDegrees = qRadiansToDegrees(qAtan2(
          munition.verticalSpeedMetersPerSecond,
          qMax(1.0, munition.speedMetersPerSecond)));
      munition.status = QStringLiteral("Falling");
    } else {
      const double pitchRadians = qDegreesToRadians(munition.pitchDegrees);
      const double totalDistanceMeters = munition.speedMetersPerSecond * deltaSeconds;
      horizontalDistanceMeters =
          qMax(0.0, totalDistanceMeters * qCos(pitchRadians));
      verticalDistanceMeters = totalDistanceMeters * qSin(pitchRadians);
    }

    const auto [nextLatitude, nextLongitude] = destinationPoint(
        munition.latitude,
        munition.longitude,
        munition.headingDegrees,
        horizontalDistanceMeters);

    munition.latitude = nextLatitude;
    munition.longitude = nextLongitude;
    munition.altitudeMeters += verticalDistanceMeters;
    munition.ageSeconds += deltaSeconds;

    if (isBomb) {
      const int previousTrailIndex = qFloor(previousAgeSeconds / kBombSmokeTrailIntervalSeconds);
      const int currentTrailIndex = qFloor(munition.ageSeconds / kBombSmokeTrailIntervalSeconds);
      for (int trailIndex = previousTrailIndex + 1; trailIndex <= currentTrailIndex; ++trailIndex) {
        appendBombSmokeTrailEffect(_transientEffects, munition, trailIndex);
      }
    }

    if (hasGuidedTarget) {
      if (munition.guidanceActive && trackedTarget && !trackedTarget->destroyed) {
        const double horizontalDistanceToTargetMeters = distanceMeters(
            munition.latitude,
            munition.longitude,
            trackedTarget->latitude,
            trackedTarget->longitude);
        const double verticalSeparationMeters = qAbs(
            munition.altitudeMeters - static_cast<double>(trackedTarget->altitude));
        const double slantRangeMeters = qSqrt(
            qPow(horizontalDistanceToTargetMeters, 2.0) +
            qPow(verticalSeparationMeters, 2.0));
        if (slantRangeMeters <= munition.hitRadiusMeters) {
          this->applyMissileDamage(
              trackedTarget->name,
              kDefaultMissileDamagePercent);
          if (isBomb) {
            appendBombImpactEffects(_transientEffects, munition);
          } else {
            _transientEffects.push_back(makeTransientEffect(
                munition.id + QStringLiteral("-impact"),
                QStringLiteral("ImpactFlash"),
                munition.forceIdentifier,
                munition.latitude,
                munition.longitude,
                munition.altitudeMeters,
                kImpactFlashTtlSeconds));
          }
          munition.active = false;
          continue;
        }
      }
    } else {
      QString impactedEntityName;
      double bestImpactRangeMeters = munition.hitRadiusMeters;
      for (const Entity& entity : _entities) {
        if (entity.destroyed ||
            entity.name == munition.launcherEntityName ||
            entity.forceIdentifier == munition.forceIdentifier) {
          continue;
        }

        const double horizontalDistanceMeters = distanceMeters(
            munition.latitude,
            munition.longitude,
            entity.latitude,
            entity.longitude);
        const double verticalSeparationMeters = qAbs(
            munition.altitudeMeters - static_cast<double>(entity.altitude));
        const double slantRangeMeters = qSqrt(
            qPow(horizontalDistanceMeters, 2.0) +
            qPow(verticalSeparationMeters, 2.0));
        if (slantRangeMeters <= bestImpactRangeMeters) {
          bestImpactRangeMeters = slantRangeMeters;
          impactedEntityName = entity.name;
        }
      }

      if (!impactedEntityName.isEmpty()) {
        if (isBomb) {
          this->applyBombBlastDamage(munition);
          appendBombImpactEffects(_transientEffects, munition);
        } else {
          this->applyMissileDamage(
              impactedEntityName,
              kDefaultMissileDamagePercent);
          _transientEffects.push_back(makeTransientEffect(
              munition.id + QStringLiteral("-impact"),
              QStringLiteral("ImpactFlash"),
              munition.forceIdentifier,
              munition.latitude,
              munition.longitude,
              munition.altitudeMeters,
              kImpactFlashTtlSeconds));
        }
        munition.active = false;
        continue;
      }
    }

    if (munition.altitudeMeters <= kMinimumMunitionAltitudeMeters) {
      if (isBomb) {
        this->applyBombBlastDamage(munition);
        appendBombImpactEffects(_transientEffects, munition);
      }
      munition.active = false;
      continue;
    }

    if (munition.ageSeconds >= munition.ttlSeconds) {
      munition.active = false;
    }
  }

  for (qsizetype index = _activeMunitions.size() - 1; index >= 0; --index) {
    if (_activeMunitions.at(index).active) {
      continue;
    }
    _activeMunitions.removeAt(index);
    if (index == 0) {
      break;
    }
  }
}

void ScenarioState::advanceTransientEffects(double deltaSeconds) {
  if (deltaSeconds <= 0.0 || _transientEffects.isEmpty()) {
    return;
  }

  for (TransientEffect& effect : _transientEffects) {
    if (!effect.active) {
      continue;
    }
    effect.ageSeconds += deltaSeconds;
    if (effect.ageSeconds >= effect.ttlSeconds) {
      effect.active = false;
    }
  }

  for (qsizetype index = _transientEffects.size() - 1; index >= 0; --index) {
    if (_transientEffects.at(index).active) {
      continue;
    }
    _transientEffects.removeAt(index);
    if (index == 0) {
      break;
    }
  }
}

void ScenarioState::advanceBehaviors(double deltaSeconds) {
  if (deltaSeconds > 0.0) {
    for (auto it = _behaviorMissileCooldownSeconds.begin();
         it != _behaviorMissileCooldownSeconds.end();) {
      it->second = qMax(0.0, it->second - deltaSeconds);
      if (it->second <= 0.0) {
        it = _behaviorMissileCooldownSeconds.erase(it);
        continue;
      }
      ++it;
    }
  }

  for (Entity& entity : _entities) {
    if (entity.destroyed) {
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    const QString behaviorMode = normalizedBehaviorMode(entity.behaviorMode);
    if (behaviorMode == QStringLiteral("Manual")) {
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    if (entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    const QString selectedTargetName =
        selectBestBehaviorTargetName(entity, _entities);
    if (entity.behaviorTargetEntityName != selectedTargetName) {
      const QString previousTargetName = entity.behaviorTargetEntityName;
      entity.behaviorTargetEntityName = selectedTargetName;
      if (!selectedTargetName.isEmpty()) {
        _pendingEventLogMessages.push_back(
            QStringLiteral("%1 behavior target selected: %2")
                .arg(entity.name, selectedTargetName));
      } else if (!previousTargetName.isEmpty()) {
        _pendingEventLogMessages.push_back(
            QStringLiteral("%1 behavior target cleared")
                .arg(entity.name));
      }
    }

    if (behaviorMode != QStringLiteral("Aggressive") ||
        entity.behaviorTargetEntityName.trimmed().isEmpty()) {
      continue;
    }

    const auto cooldownIt = _behaviorMissileCooldownSeconds.find(entity.name);
    if (cooldownIt != _behaviorMissileCooldownSeconds.end() &&
        cooldownIt->second > 0.0) {
      continue;
    }

    const Entity* target =
        findEntityByName(_entities, entity.behaviorTargetEntityName.trimmed());
    if (!target || !entityPassesAutoMissileQuickValidation(entity, *target)) {
      continue;
    }

    const QString launcherName = entity.name;
    const QString targetName = target->name;
    if (!this->launchMissileAt(launcherName, targetName)) {
      continue;
    }

    _behaviorMissileCooldownSeconds[launcherName] =
        kBehaviorAutoMissileCooldownSeconds;
    _pendingEventLogMessages.push_back(
        QStringLiteral("%1 auto-launched missile at %2")
            .arg(launcherName, targetName));
  }
}

void ScenarioState::advanceSimulation(double deltaSeconds) {
  FlightDynamicsEngine::advanceEntities(_entities, _taskStacks, deltaSeconds);
  this->advanceBehaviors(deltaSeconds);
  this->advanceActiveMunitions(deltaSeconds);
  this->advanceTransientEffects(deltaSeconds);
  this->refreshSensors();
}

void ScenarioState::stopMission() {
  for (Entity& entity : _entities) {
    entity.currentTask = EntityTask{};
    entity.currentTask.status = QStringLiteral("Stopped");
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    entity.sensorContacts.clear();
    entity.behaviorTargetEntityName.clear();
  }
  _activeMunitions.clear();
  _transientEffects.clear();
  _pendingEventLogMessages.clear();
  _taskStacks.clear();
  _behaviorMissileCooldownSeconds.clear();
  this->refreshSensors();
  this->save();
}

bool ScenarioState::save() const {
  QJsonArray entities;
  for (const Entity& entity : _entities) {
    entities.append(toJson(entity));
  }

  QJsonArray waypoints;
  for (const Waypoint& waypoint : _waypoints) {
    waypoints.append(toJson(waypoint));
  }

  QJsonArray routes;
  for (const RouteGraphic& route : _routes) {
    routes.append(toJson(route));
  }

  QJsonArray areas;
  for (const AreaDefinition& area : _areas) {
    areas.append(toJson(area));
  }

  QFile file(this->storagePath());
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

bool ScenarioState::load() {
  _entities.clear();
  _activeMunitions.clear();
  _transientEffects.clear();
  _pendingEventLogMessages.clear();
  _waypoints.clear();
  _routes.clear();
  _areas.clear();
  _taskStacks.clear(); // Clear all stacks before loading new scenario
  _behaviorMissileCooldownSeconds.clear();
  _nextMunitionSerial = 1;

  QFile file(this->storagePath());
  if (!file.exists()) {
    return true;
  }
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
  if (!document.isObject()) {
    return false;
  }

  const QJsonArray entities = document.object().value(QStringLiteral("entities")).toArray();
  for (const QJsonValue& value : entities) {
    Entity entity = entityFromJson(value.toObject()); // Deserialize first
    entity.currentTask = EntityTask{}; // CRITICAL: Reset currentTask to clean slate for loaded entity
    entity.flightDynamicsEnabled = false;
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    _taskStacks[entity.name] = domain::TaskStack(); // CRITICAL: Initialize empty stack for loaded entity
    _entities.push_back(entity); // Then push the cleaned entity
  }

  const QJsonArray waypoints = document.object().value(QStringLiteral("waypoints")).toArray();
  for (const QJsonValue& value : waypoints) {
    _waypoints.push_back(waypointFromJson(value.toObject()));
  }

  const QJsonArray routes = document.object().value(QStringLiteral("routes")).toArray();
  for (const QJsonValue& value : routes) {
    _routes.push_back(routeFromJson(value.toObject()));
  }

  const QJsonArray areas = document.object().value(QStringLiteral("areas")).toArray();
  for (const QJsonValue& value : areas) {
    _areas.push_back(areaFromJson(value.toObject()));
  }

  this->refreshSensors();
  return true;
}

void ScenarioState::reset() {
  _entities.clear();
  _activeMunitions.clear();
  _transientEffects.clear();
  _pendingEventLogMessages.clear();
  _waypoints.clear();
  _routes.clear();
  _areas.clear();
  _taskStacks.clear(); // Clear all stacks before reset
  _behaviorMissileCooldownSeconds.clear();
  _nextMunitionSerial = 1;
  this->save();
}

QString ScenarioState::storagePath() const {
  return QDir(projectRoot()).absoluteFilePath(QStringLiteral("Data/scenario_state.json"));
}
