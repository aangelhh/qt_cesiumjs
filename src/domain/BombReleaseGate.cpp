#include "BombReleaseGate.h"
#include "GeoMath.h"
#include "../domain/Entity.h"
#include <cmath>
#include <algorithm>

namespace domain {

BombReleaseGateEvaluation evaluateBombReleaseGate(
    const Entity& launcher,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters) {
  BombReleaseGateEvaluation evaluation;

  const double relativeAltitudeMeters = std::max(
      0.0,
      static_cast<double>(launcher.altitude) - targetAltitudeMeters);
  const double horizontalSpeedMps = std::max(
      0.0,
      launcher.speedKnots * kKnotsToMetersPerSecond *
          std::cos(launcher.pitchDegrees * 3.14159265358979323846 / 180.0));
  evaluation.relativeAltitudeMeters = relativeAltitudeMeters;
  evaluation.horizontalSpeedMetersPerSecond = horizontalSpeedMps;

  if (relativeAltitudeMeters <= 0.0 || horizontalSpeedMps <= 1.0) {
    return evaluation;
  }

  const double verticalSpeedMps = launcher.verticalSpeedMetersPerSecond;
  const double discriminant =
      std::pow(verticalSpeedMps, 2.0) +
      2.0 * kBombReleaseGravityMps2 * relativeAltitudeMeters;
  if (discriminant < 0.0) {
    return evaluation;
  }

  const double timeToImpactSeconds =
      (verticalSpeedMps + std::sqrt(discriminant)) / kBombReleaseGravityMps2;
  if (timeToImpactSeconds <= 0.0) {
    return evaluation;
  }

  const double releaseDistanceMeters = horizontalSpeedMps * timeToImpactSeconds;
  const double distToTarget = distanceMeters(
      launcher.latitude, launcher.longitude,
      targetLatitude, targetLongitude);
  const double desiredHeading = bearingDegrees(
      launcher.latitude, launcher.longitude,
      targetLatitude, targetLongitude);
  const double headingErrorDegrees = std::abs(
      shortestSignedAngle(launcher.headingDegrees, desiredHeading));
  const double distanceErrorMeters = distToTarget - releaseDistanceMeters;

  evaluation.valid = true;
  evaluation.targetAhead = headingErrorDegrees <= 90.0;
  evaluation.withinHeadingCone =
      headingErrorDegrees <= kBombReleaseHeadingConeDegrees;
  evaluation.withinReleaseWindow =
      std::abs(distanceErrorMeters) <= kBombReleaseDistanceToleranceMeters;
  evaluation.timeToImpactSeconds = timeToImpactSeconds;
  evaluation.releaseDistanceMeters = releaseDistanceMeters;
  evaluation.distanceToTargetMeters = distToTarget;
  evaluation.desiredHeadingDegrees = desiredHeading;
  evaluation.headingErrorDegrees = headingErrorDegrees;
  evaluation.distanceErrorMeters = distanceErrorMeters;
  evaluation.secondsToReleaseWindow =
      evaluation.targetAhead && evaluation.withinHeadingCone
      ? distanceErrorMeters / horizontalSpeedMps
      : 0.0;
  return evaluation;
}

int weaponQuantity(const Entity& entity, const QString& weaponType) {
  for (const WeaponInventoryItem& item : entity.weapons) {
    if (item.weaponType.compare(weaponType, Qt::CaseInsensitive) == 0) {
      return item.quantity;
    }
  }
  return 0;
}

} // namespace domain
