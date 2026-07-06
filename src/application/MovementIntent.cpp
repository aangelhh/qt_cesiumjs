#include "application/MovementIntent.h"

#include "domain/GeoMath.h"

#include <QString>
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

MovementControllerLimits movementControllerLimitsForEntity(const Entity& entity) {
  const bool isGround =
      entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
  if (isGround) {
    return {18.0, 10.0, 0.0, 70.0, static_cast<double>(qMax(0, entity.altitude)), 1.0};
  }

  const QString category = entity.category.trimmed().toLower();
  if (category.contains(QStringLiteral("fighter")) ||
      category.contains(QStringLiteral("interceptor"))) {
    return {6.0, 18.0, 35.0, 900.0, 16000.0, 50.0};
  }
  if (category.contains(QStringLiteral("bomber")) ||
      category.contains(QStringLiteral("transport")) ||
      category.contains(QStringLiteral("tanker")) ||
      category.contains(QStringLiteral("awacs"))) {
    return {2.5, 6.0, 12.0, 520.0, 11000.0, 50.0};
  }

  return {4.0, 10.0, 20.0, 650.0, 12000.0, 50.0};
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
      qBound(0.0, intent.targetSpeedKnots, limits.maxSpeedKnots),
      limits.accelerationKnotsPerSecond * deltaSeconds);

  const double targetAltitudeMeters = qBound(
      0.0,
      intent.targetAltitudeMeters,
      limits.maxAltitudeMeters);
  const double altitudeDelta =
      targetAltitudeMeters - static_cast<double>(entity.altitude);
  entity.verticalSpeedMetersPerSecond =
      qAbs(altitudeDelta) <= limits.altitudeCaptureToleranceMeters
          ? 0.0
          : stepToward(0.0, altitudeDelta, limits.climbRateMetersPerSecond);
}

void stopMovementIntent(Entity& entity) {
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 0.0;
}

} // namespace application
