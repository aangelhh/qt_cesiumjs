#include "TrackSummaryBuilder.h"
#include "../domain/CombatRules.h"
#include "../domain/Entity.h"
#include "../domain/GeoMath.h"
#include "../domain/Munition.h"

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

} // namespace presentation
