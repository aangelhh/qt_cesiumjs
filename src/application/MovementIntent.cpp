#include "application/MovementIntent.h"

#include "domain/GeoMath.h"

#include <QtGlobal>

namespace application {
namespace {

double stepToward(double currentValue, double targetValue, double maxStep) {
  const double safeMaxStep = qMax(0.0, maxStep);
  const double delta = targetValue - currentValue;
  if (qAbs(delta) <= safeMaxStep) {
    return targetValue;
  }
  return currentValue + (delta > 0.0 ? safeMaxStep : -safeMaxStep);
}

} // namespace

MovementIntent movementIntentFromTask(const EntityTask& task) {
  MovementIntent intent;
  intent.targetHeadingDegrees = task.targetHeadingDegrees;
  intent.targetAltitudeMeters = static_cast<double>(task.targetAltitudeMeters);
  intent.targetSpeedKnots = task.targetSpeedKnots;
  return intent;
}

void applyMovementIntent(
    Entity& entity,
    const MovementIntent& intent,
    double deltaSeconds,
    const MovementControllerLimits& limits) {
  if (deltaSeconds <= 0.0 || intent.mode == MovementIntentMode::None) {
    return;
  }

  const double headingDelta = domain::shortestSignedAngle(
      entity.headingDegrees,
      intent.targetHeadingDegrees);
  entity.headingDegrees = domain::normalizeDegrees360(
      entity.headingDegrees +
      stepToward(
          0.0,
          headingDelta,
          limits.headingRateDegreesPerSecond * deltaSeconds));

  entity.speedKnots = stepToward(
      entity.speedKnots,
      qMax(0.0, intent.targetSpeedKnots),
      limits.accelerationKnotsPerSecond * deltaSeconds);

  const double altitudeDelta =
      intent.targetAltitudeMeters - static_cast<double>(entity.altitude);
  entity.verticalSpeedMetersPerSecond = stepToward(
      0.0,
      altitudeDelta,
      limits.climbRateMetersPerSecond);
}

void stopMovementIntent(Entity& entity) {
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 0.0;
}

} // namespace application
