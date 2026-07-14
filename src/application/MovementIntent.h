#pragma once

#include "domain/Entity.h"

namespace application {

enum class MovementIntentMode {
  None,
  DirectSetpoint,
  SpatialTarget,
  Pursuit,
  Hold
};

struct MovementControllerLimits {
  double headingRateDegreesPerSecond = 3.0;
  double accelerationKnotsPerSecond = 8.0;
  double climbRateMetersPerSecond = 20.0;
  double maxSpeedKnots = 650.0;
  double maxAltitudeMeters = 12000.0;
  double altitudeCaptureToleranceMeters = 25.0;
};

struct MovementIntent {
  double targetHeadingDegrees = 0.0;
  double targetAltitudeMeters = 0.0;
  double targetSpeedKnots = 0.0;
  MovementIntentMode mode = MovementIntentMode::DirectSetpoint;
};

MovementIntent movementIntentFromTask(const EntityTask& task);

MovementControllerLimits movementControllerLimitsForEntity(const Entity& entity);

void applyMovementIntent(
    Entity& entity,
    const MovementIntent& intent,
    double deltaSeconds,
    const MovementControllerLimits& limits = {});

void stopMovementIntent(Entity& entity);

} // namespace application
