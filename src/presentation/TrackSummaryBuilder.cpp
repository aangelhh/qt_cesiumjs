#include "TrackSummaryBuilder.h"
#include "../domain/CombatRules.h"
#include "../domain/Entity.h"
#include "../domain/GeoMath.h"
#include "../domain/Munition.h"
#include "../domain/TacticalGraphic.h"
#include "EntityVisualStateManager.h"

#include <QVariantList>
#include <algorithm>

namespace presentation {

QVariantMap makeTrackSummary(
    const QString& name,
    const QString& type,
    const QString& team,
    const QString& altitudeText,
    const QString& positionText,
    const QString& status,
    double latitude,
    double longitude) {
  return {
      {QStringLiteral("name"), name},
      {QStringLiteral("type"), type},
      {QStringLiteral("team"), team},
      {QStringLiteral("altitude"), altitudeText},
      {QStringLiteral("position"), positionText},
      {QStringLiteral("status"), status},
      {QStringLiteral("latitude"), latitude},
      {QStringLiteral("longitude"), longitude},
      {QStringLiteral("callsign"), QString()},
      {QStringLiteral("domain"), QString()},
      {QStringLiteral("category"), QString()},
      {QStringLiteral("forceIdentifier"), 0},
      {QStringLiteral("entityKind"), 0},
      {QStringLiteral("entityDomain"), 0},
      {QStringLiteral("entityCountry"), 0},
      {QStringLiteral("entityCategory"), 0},
      {QStringLiteral("entitySubcategory"), 0},
      {QStringLiteral("entitySpecific"), 0},
      {QStringLiteral("entityExtra"), 0},
      {QStringLiteral("entityTypeCode"), QString()},
      {QStringLiteral("modelName"), QString()},
      {QStringLiteral("modelUri"), QString()},
      {QStringLiteral("munitionType"), QString()},
      {QStringLiteral("effectType"), QString()},
      {QStringLiteral("headingDegrees"), 0.0},
      {QStringLiteral("pitchDegrees"), 0.0},
      {QStringLiteral("rollDegrees"), 0.0},
      {QStringLiteral("modelScale"), 1.0},
      {QStringLiteral("pointSize"), 11},
      {QStringLiteral("labelVisible"), true},
      {QStringLiteral("flightDynamicsEnabled"), false},
      {QStringLiteral("flightDynamicsMode"), QStringLiteral("kinematic")},
      {QStringLiteral("jsbsimAircraftModel"), QString()},
      {QStringLiteral("speedKnots"), 0.0},
      {QStringLiteral("verticalSpeedMetersPerSecond"), 0.0},
      {QStringLiteral("taskType"), QString()},
      {QStringLiteral("taskEnabled"), false},
      {QStringLiteral("taskStatus"), QStringLiteral("Idle")},
      {QStringLiteral("taskTargetHeadingDegrees"), 0.0},
      {QStringLiteral("taskTargetAltitudeMeters"), 0},
      {QStringLiteral("taskTargetSpeedKnots"), 0.0},
      {QStringLiteral("taskTargetLatitude"), 0.0},
      {QStringLiteral("taskTargetLongitude"), 0.0},
      {QStringLiteral("taskTargetEntityName"), QString()},
      {QStringLiteral("taskTargetWaypointName"), QString()},
      {QStringLiteral("taskTargetRouteName"), QString()},
      {QStringLiteral("taskFollowDistanceMeters"), 1000.0},
      {QStringLiteral("taskArrivalToleranceMeters"), 100.0},
      {QStringLiteral("taskDurationSeconds"), 0.0},
      {QStringLiteral("taskElapsedSeconds"), 0.0},
      {QStringLiteral("taskInterceptDistanceMeters"), 500.0},
      {QStringLiteral("taskAltitudeToleranceMeters"), 100.0},
      {QStringLiteral("taskTimeoutSeconds"), 120.0},
      {QStringLiteral("destroyed"), false},
      {QStringLiteral("damagePercent"), 0.0},
      {QStringLiteral("damageState"), QStringLiteral("Intact")},
      {QStringLiteral("hidden"), false},
      {QStringLiteral("radarCoverageVisible"), false},
      {QStringLiteral("trackHistoryVisible"), false},
  };
}

QVariantMap makeMunitionTrackSummary(const ActiveMunition& munition) {
  const double altitudeMeters = std::max(0.0, munition.altitudeMeters);
  const double speedKnots = munition.speedMetersPerSecond / 0.514444;  // kKnotsToMetersPerSecond
  const QString munitionType = munition.munitionType.trimmed();
  const bool isBomb =
      munitionType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0;

  QVariantMap summary = makeTrackSummary(
      munition.id,
      QStringLiteral("Munition"),
      domain::forceIdentifierLabel(munition.forceIdentifier),
      QStringLiteral("%1 m").arg(altitudeMeters, 0, 'f', 0),
      domain::formatPosition(munition.latitude, munition.longitude),
      munition.status.trimmed().isEmpty() ? QStringLiteral("Flying") : munition.status,
      munition.latitude,
      munition.longitude);
  summary.insert(QStringLiteral("category"),
                 isBomb ? QStringLiteral("Bomb") : QStringLiteral("Missile"));
  summary.insert(QStringLiteral("munitionType"), munition.munitionType);
  summary.insert(QStringLiteral("forceIdentifier"), munition.forceIdentifier);
  summary.insert(QStringLiteral("modelName"),
                 isBomb ? QStringLiteral("Bomb") : QStringLiteral("Missile"));
  summary.insert(QStringLiteral("modelUri"), munition.modelUri);
  summary.insert(QStringLiteral("headingDegrees"), munition.headingDegrees);
  summary.insert(QStringLiteral("pitchDegrees"), munition.pitchDegrees);
  summary.insert(QStringLiteral("rollDegrees"), munition.rollDegrees);
  summary.insert(QStringLiteral("speedKnots"), speedKnots);
  summary.insert(QStringLiteral("modelScale"), isBomb ? 0.5 : 0.35);
  summary.insert(QStringLiteral("labelVisible"), false);
  return summary;
}

QVariantMap makeTransientEffectTrackSummary(const TransientEffect& effect) {
  const double altitudeMeters = std::max(0.0, effect.altitudeMeters);
  const QString effectType = effect.effectType.trimmed().toCaseFolded();
  int pointSize = 10;
  if (effectType == QStringLiteral("impactflash")) {
    pointSize = 18;
  } else if (effectType == QStringLiteral("bombsmoketrail")) {
    pointSize = 8;
  } else if (effectType == QStringLiteral("bombimpactflash")) {
    pointSize = 16;
  } else if (effectType == QStringLiteral("bombsmoke")) {
    pointSize = 20;
  }
  QVariantMap summary = makeTrackSummary(
      effect.id,
      QStringLiteral("Effect"),
      domain::forceIdentifierLabel(effect.forceIdentifier),
      QStringLiteral("%1 m").arg(altitudeMeters, 0, 'f', 0),
      domain::formatPosition(effect.latitude, effect.longitude),
      effect.effectType,
      effect.latitude,
      effect.longitude);
  summary.insert(QStringLiteral("category"), QStringLiteral("Effect"));
  summary.insert(QStringLiteral("effectType"), effect.effectType);
  summary.insert(QStringLiteral("forceIdentifier"), effect.forceIdentifier);
  summary.insert(QStringLiteral("labelVisible"), false);
  summary.insert(QStringLiteral("pointSize"), pointSize);
  return summary;
}

QVariantMap makePendingBombTargetTrackSummary(
    const QString& targetLabel,
    double latitude,
    double longitude,
    double targetAltitudeMeters,
    const QString& teamLabel,
    const QString& releaseStateLabel,
    double distanceMetersToTarget) {
  const QString distanceText = distanceMetersToTarget >= 0.0
      ? QStringLiteral("%1 km").arg(distanceMetersToTarget / 1000.0, 0, 'f', 1)
      : QString();
  const QString armedLabel = releaseStateLabel.trimmed().isEmpty()
      ? QStringLiteral("Armed")
      : releaseStateLabel;
  QVariantMap summary = makeTrackSummary(
      QStringLiteral("Bomb Target"),
      QStringLiteral("PendingBombTarget"),
      teamLabel.trimmed().isEmpty() ? QStringLiteral("Friendly") : teamLabel,
      QStringLiteral("%1 m").arg(std::max(0.0, targetAltitudeMeters), 0, 'f', 0),
      domain::formatPosition(latitude, longitude),
      distanceText.trimmed().isEmpty()
          ? armedLabel
          : QStringLiteral("%1 | %2").arg(armedLabel, distanceText),
      latitude,
      longitude);
  summary.insert(QStringLiteral("category"), QStringLiteral("PendingBombTarget"));
  summary.insert(QStringLiteral("pointSize"), 16);
  summary.insert(QStringLiteral("labelVisible"), true);
  summary.insert(QStringLiteral("pendingBombReleaseState"), releaseStateLabel);
  summary.insert(QStringLiteral("pendingBombTargetLabel"), targetLabel);
  summary.insert(QStringLiteral("pendingBombTargetDistanceMeters"), distanceMetersToTarget);
  return summary;
}

QVariantMap makePendingBombTargetLineTrackSummary(
    const Entity& launcher,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters,
    const QString& teamLabel,
    const QString& releaseStateLabel) {
  QVariantMap summary = makeTrackSummary(
      QStringLiteral("Bomb Target Line"),
      QStringLiteral("PendingBombTargetLine"),
      teamLabel.trimmed().isEmpty() ? QStringLiteral("Friendly") : teamLabel,
      QStringLiteral("%1 m").arg(std::max(0, launcher.altitude)),
      domain::formatPosition(launcher.latitude, launcher.longitude),
      releaseStateLabel.trimmed().isEmpty() ? QStringLiteral("Armed") : releaseStateLabel,
      launcher.latitude,
      launcher.longitude);
  summary.insert(QStringLiteral("category"), QStringLiteral("PendingBombTargetLine"));
  summary.insert(QStringLiteral("labelVisible"), false);
  summary.insert(QStringLiteral("pendingBombReleaseState"), releaseStateLabel);
  QVariantList routePoints;
  routePoints.push_back(QVariantMap{
      {QStringLiteral("longitude"), launcher.longitude},
      {QStringLiteral("latitude"), launcher.latitude},
      {QStringLiteral("altitudeMeters"), static_cast<double>(launcher.altitude)},
  });
  routePoints.push_back(QVariantMap{
      {QStringLiteral("longitude"), targetLongitude},
      {QStringLiteral("latitude"), targetLatitude},
      {QStringLiteral("altitudeMeters"), targetAltitudeMeters},
  });
  summary.insert(QStringLiteral("routePoints"), routePoints);
  return summary;
}

QString missileTargetDisplayLabel(const Entity& entity, double rangeMeters) {
  return QStringLiteral("%1 (%2 / %3, %4 km)")
      .arg(
          entity.name,
          domain::forceIdentifierLabel(entity.forceIdentifier),
          entity.category,
          QString::number(rangeMeters / 1000.0, 'f', 1));
}

QString bombTargetDisplayLabel(const Entity& entity) {
  return QStringLiteral("%1 (%2 / %3)")
      .arg(entity.name,
           domain::forceIdentifierLabel(entity.forceIdentifier),
           entity.domain);
}

QVariantMap makeEntityTrackSummary(
    const Entity& entity,
    const EntityVisualState& visualState) {

  const QString damageState = entity.damageStateLabel();

  QVariantMap summary = makeTrackSummary(
      entity.name,
      entity.type,
      domain::forceIdentifierLabel(entity.forceIdentifier),
      QStringLiteral("%1 m").arg(entity.altitude),
      domain::formatPosition(entity.latitude, entity.longitude),
      damageState,
      entity.latitude,
      entity.longitude);

  summary.insert(QStringLiteral("domain"),              entity.domain);
  summary.insert(QStringLiteral("category"),            entity.category);
  summary.insert(QStringLiteral("callsign"),            entity.callsign);
  summary.insert(QStringLiteral("forceIdentifier"),     entity.forceIdentifier);
  summary.insert(QStringLiteral("entityKind"),          entity.entityKind);
  summary.insert(QStringLiteral("entityDomain"),        entity.entityDomain);
  summary.insert(QStringLiteral("entityCountry"),       entity.entityCountry);
  summary.insert(QStringLiteral("entityCategory"),      entity.entityCategory);
  summary.insert(QStringLiteral("entitySubcategory"),   entity.entitySubcategory);
  summary.insert(QStringLiteral("entitySpecific"),      entity.entitySpecific);
  summary.insert(QStringLiteral("entityExtra"),         entity.entityExtra);
  summary.insert(QStringLiteral("entityTypeCode"),      entity.entityTypeCode);
  summary.insert(QStringLiteral("modelName"),           entity.modelName);
  summary.insert(QStringLiteral("modelUri"),            entity.modelUri);
  summary.insert(QStringLiteral("headingDegrees"),      entity.headingDegrees);
  summary.insert(QStringLiteral("pitchDegrees"),        entity.pitchDegrees);
  summary.insert(QStringLiteral("rollDegrees"),         entity.rollDegrees);
  summary.insert(QStringLiteral("flightDynamicsEnabled"),  entity.flightDynamicsEnabled);
  summary.insert(QStringLiteral("flightDynamicsMode"),     entity.flightDynamicsMode);
  summary.insert(QStringLiteral("jsbsimAircraftModel"),    entity.jsbsimAircraftModel);
  summary.insert(QStringLiteral("speedKnots"),          entity.speedKnots);
  summary.insert(QStringLiteral("verticalSpeedMetersPerSecond"), entity.verticalSpeedMetersPerSecond);
  summary.insert(QStringLiteral("destroyed"),           entity.destroyed);
  summary.insert(QStringLiteral("damagePercent"),       entity.damagePercent);
  summary.insert(QStringLiteral("damageState"),         damageState);
  summary.insert(QStringLiteral("behaviorMode"),
      entity.behaviorMode.trimmed().isEmpty()
          ? QStringLiteral("Manual")
          : entity.behaviorMode);
  summary.insert(QStringLiteral("behaviorTargetEntityName"), entity.behaviorTargetEntityName);
  summary.insert(QStringLiteral("hidden"),               visualState.hidden);
  summary.insert(QStringLiteral("radarCoverageVisible"), visualState.radarCoverageVisible);
  summary.insert(QStringLiteral("trackHistoryVisible"),  visualState.trackHistoryVisible);
  summary.insert(QStringLiteral("taskType"),             entity.currentTask.taskType);
  summary.insert(QStringLiteral("taskEnabled"),          entity.currentTask.enabled);
  summary.insert(QStringLiteral("taskStatus"),           entity.currentTask.status);
  summary.insert(QStringLiteral("taskTargetHeadingDegrees"),    entity.currentTask.targetHeadingDegrees);
  summary.insert(QStringLiteral("taskTargetAltitudeMeters"),    entity.currentTask.targetAltitudeMeters);
  summary.insert(QStringLiteral("taskTargetSpeedKnots"),        entity.currentTask.targetSpeedKnots);
  summary.insert(QStringLiteral("taskTargetLatitude"),          entity.currentTask.targetLatitude);
  summary.insert(QStringLiteral("taskTargetLongitude"),         entity.currentTask.targetLongitude);
  summary.insert(QStringLiteral("taskTargetEntityName"),        entity.currentTask.targetEntityName);
  summary.insert(QStringLiteral("taskTargetWaypointName"),      entity.currentTask.targetWaypointName);
  summary.insert(QStringLiteral("taskTargetRouteName"),         entity.currentTask.targetRouteName);
  summary.insert(QStringLiteral("taskTargetAreaName"),          entity.currentTask.targetAreaName);
  summary.insert(QStringLiteral("taskTargetAreaRadiusMeters"),  entity.currentTask.targetAreaRadiusMeters);
  summary.insert(QStringLiteral("taskFollowDistanceMeters"),    entity.currentTask.followDistanceMeters);
  summary.insert(QStringLiteral("taskArrivalToleranceMeters"),  entity.currentTask.arrivalToleranceMeters);
  summary.insert(QStringLiteral("taskDurationSeconds"),         entity.currentTask.durationSeconds);
  summary.insert(QStringLiteral("taskElapsedSeconds"),          entity.currentTask.elapsedSeconds);
  summary.insert(QStringLiteral("taskRouteCurrentWaypointIndex"), entity.currentTask.routeCurrentWaypointIndex);
  summary.insert(QStringLiteral("taskRouteTotalWaypoints"),       entity.currentTask.routeTotalWaypoints);
  summary.insert(QStringLiteral("taskInterceptDistanceMeters"), entity.currentTask.interceptDistanceMeters);
  summary.insert(QStringLiteral("taskAltitudeToleranceMeters"), entity.currentTask.altitudeToleranceMeters);
  summary.insert(QStringLiteral("taskTimeoutSeconds"),          entity.currentTask.timeoutSeconds);
  summary.insert(QStringLiteral("taskMaxEngagementTimeSeconds"), entity.currentTask.maxEngagementTimeSeconds);
  summary.insert(QStringLiteral("taskShotCooldownSeconds"),     entity.currentTask.shotCooldownSeconds);
  summary.insert(QStringLiteral("taskRacetrackLegLengthMeters"), entity.currentTask.racetrackLegLengthMeters);
  summary.insert(QStringLiteral("taskWeaponType"),              entity.currentTask.weaponType);
  summary.insert(QStringLiteral("taskTargetDomain"),            entity.currentTask.targetDomain);
  summary.insert(QStringLiteral("taskEnemyOnly"),               entity.currentTask.enemyOnly);
  summary.insert(QStringLiteral("taskDamageThresholdPercent"),  entity.currentTask.damageThresholdPercent);
  summary.insert(QStringLiteral("taskRangeMeters"),             entity.currentTask.rangeMeters);
  summary.insert(QStringLiteral("sensorCount"),    entity.sensors.size());
  summary.insert(QStringLiteral("contactCount"),   entity.sensorContacts.size());

  QVariantList sensors;
  for (const SensorDefinition& sensor : entity.sensors) {
    sensors.push_back(QVariantMap{
        {QStringLiteral("id"),                    sensor.id},
        {QStringLiteral("name"),                  sensor.name},
        {QStringLiteral("sensorType"),            sensor.sensorType},
        {QStringLiteral("enabled"),               sensor.enabled},
        {QStringLiteral("emitting"),              sensor.emitting},
        {QStringLiteral("maxRangeMeters"),         sensor.maxRangeMeters},
        {QStringLiteral("azimuthCenterDegrees"),   sensor.azimuthCenterDegrees},
        {QStringLiteral("azimuthWidthDegrees"),    sensor.azimuthWidthDegrees},
        {QStringLiteral("elevationCenterDegrees"), sensor.elevationCenterDegrees},
        {QStringLiteral("elevationWidthDegrees"),  sensor.elevationWidthDegrees},
        {QStringLiteral("maxTracks"),             sensor.maxTracks},
    });
  }
  summary.insert(QStringLiteral("sensors"), sensors);

  QVariantList contacts;
  for (const SensorContact& contact : entity.sensorContacts) {
    contacts.push_back(QVariantMap{
        {QStringLiteral("sensorId"),         contact.sensorId},
        {QStringLiteral("targetEntityName"), contact.targetEntityName},
        {QStringLiteral("rangeMeters"),      contact.rangeMeters},
        {QStringLiteral("bearingDegrees"),   contact.bearingDegrees},
        {QStringLiteral("lineOfSight"),      contact.lineOfSight},
        {QStringLiteral("detected"),         contact.detected},
    });
  }
  summary.insert(QStringLiteral("sensorContacts"), contacts);

  return summary;
}

QString normalizeEntityCategory(const QString& category) {
  static const QStringList kKnownCategories{
      QStringLiteral("Fighter"),
      QStringLiteral("Bomber"),
      QStringLiteral("Helicopter"),
      QStringLiteral("Transport"),
      QStringLiteral("Tank"),
      QStringLiteral("Truck"),
      QStringLiteral("ArmoredVehicle"),
      QStringLiteral("Armored Vehicle"),
      QStringLiteral("Radar"),
      QStringLiteral("SAMLauncher"),
      QStringLiteral("SAM Launcher"),
  };
  const QString trimmed = category.trimmed();
  return kKnownCategories.contains(trimmed) ? trimmed : QStringLiteral("Other");
}

QVariantMap makeWaypointTrackSummary(const Waypoint& waypoint) {
  QVariantMap summary = makeTrackSummary(
      waypoint.name,
      QStringLiteral("Waypoint"),
      QStringLiteral("Graphic"),
      QStringLiteral("%1 m").arg(waypoint.altitudeMeters, 0, 'f', 0),
      domain::formatPosition(waypoint.latitude, waypoint.longitude),
      QStringLiteral("Ready"),
      waypoint.latitude,
      waypoint.longitude);
  summary.insert(QStringLiteral("type"), QStringLiteral("Waypoint"));
  summary.insert(QStringLiteral("altitudeMeters"), waypoint.altitudeMeters);
  summary.insert(QStringLiteral("altitudeMetersSet"), waypoint.altitudeMetersSet);
  return summary;
}

QVariantMap makeRouteTrackSummary(const RouteGraphic& route) {
  QVariantList points;
  for (const RoutePoint& point : route.points) {
    points.push_back(QVariantMap{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
        {QStringLiteral("altitudeMeters"), point.altitudeMeters},
        {QStringLiteral("altitudeMetersSet"), point.altitudeMetersSet},
    });
  }
  const RoutePoint firstPoint = route.points.isEmpty() ? RoutePoint{} : route.points.first();
  QVariantMap summary = makeTrackSummary(
      route.name,
      QStringLiteral("Route"),
      QStringLiteral("Graphic"),
      QStringLiteral("-"),
      route.points.isEmpty()
          ? QStringLiteral("-")
          : domain::formatPosition(firstPoint.latitude, firstPoint.longitude),
      QStringLiteral("%1 points").arg(route.points.size()),
      firstPoint.latitude,
      firstPoint.longitude);
  summary.insert(QStringLiteral("type"), QStringLiteral("Route"));
  summary.insert(QStringLiteral("routePoints"), points);
  return summary;
}

QVariantMap makeAreaTrackSummary(const AreaDefinition& area) {
  QVariantMap summary = makeTrackSummary(
      area.name,
      QStringLiteral("Area"),
      QStringLiteral("Graphic"),
      QStringLiteral("%1 m").arg(area.centerAltitudeMeters, 0, 'f', 0),
      domain::formatPosition(area.centerLatitude, area.centerLongitude),
      QStringLiteral("%1").arg(area.areaType),
      area.centerLatitude,
      area.centerLongitude);
  summary.insert(QStringLiteral("type"), QStringLiteral("Area"));
  summary.insert(QStringLiteral("areaType"), area.areaType);
  summary.insert(QStringLiteral("radiusMeters"), area.radiusMeters);
  summary.insert(QStringLiteral("semiMajorAxisMeters"), area.semiMajorAxisMeters);
  summary.insert(QStringLiteral("semiMinorAxisMeters"), area.semiMinorAxisMeters);
  summary.insert(QStringLiteral("rotationDegrees"), area.rotationDegrees);
  QVariantList areaPoints;
  for (const RoutePoint& point : area.points) {
    areaPoints.push_back(QVariantMap{
        {QStringLiteral("latitude"), point.latitude},
        {QStringLiteral("longitude"), point.longitude},
        {QStringLiteral("altitudeMeters"), point.altitudeMeters},
        {QStringLiteral("altitudeMetersSet"), point.altitudeMetersSet},
    });
  }
  summary.insert(QStringLiteral("areaPoints"), areaPoints);
  summary.insert(QStringLiteral("minAltitudeMeters"), area.minAltitudeMeters);
  summary.insert(QStringLiteral("maxAltitudeMeters"), area.maxAltitudeMeters);
  return summary;
}

} // namespace presentation
