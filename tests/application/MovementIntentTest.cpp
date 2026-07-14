#include <gtest/gtest.h>

#include "application/MovementIntent.h"

using application::MovementControllerLimits;
using application::MovementIntent;
using application::applyMovementIntent;
using application::movementControllerLimitsForEntity;
using application::movementIntentFromTask;

namespace {

Entity makeAirEntity() {
  Entity entity;
  entity.name = QStringLiteral("Aircraft");
  entity.domain = QStringLiteral("Air");
  entity.altitude = 3000;
  entity.headingDegrees = 350.0;
  entity.speedKnots = 100.0;
  return entity;
}

} // namespace

TEST(MovementIntent, AppliesShortestHeadingStepAcrossNorth) {
  Entity entity = makeAirEntity();
  MovementControllerLimits limits;
  limits.headingRateDegreesPerSecond = 5.0;

  MovementIntent intent;
  intent.targetHeadingDegrees = 10.0;
  intent.targetAltitudeMeters = 3000.0;
  intent.targetSpeedKnots = 100.0;

  applyMovementIntent(entity, intent, 1.0, limits);

  EXPECT_DOUBLE_EQ(entity.headingDegrees, 355.0);
}

TEST(MovementIntent, AppliesSpeedAndVerticalIntentWithinLimits) {
  Entity entity = makeAirEntity();
  MovementControllerLimits limits;
  limits.accelerationKnotsPerSecond = 8.0;
  limits.climbRateMetersPerSecond = 20.0;

  MovementIntent intent;
  intent.targetHeadingDegrees = 350.0;
  intent.targetAltitudeMeters = 5000.0;
  intent.targetSpeedKnots = 200.0;

  applyMovementIntent(entity, intent, 2.0, limits);

  EXPECT_DOUBLE_EQ(entity.speedKnots, 116.0);
  EXPECT_DOUBLE_EQ(entity.verticalSpeedMetersPerSecond, 20.0);
}

TEST(MovementIntent, AppliesDescentIntentWithoutChangingSpeedPastTarget) {
  Entity entity = makeAirEntity();
  entity.speedKnots = 210.0;

  MovementIntent intent;
  intent.targetHeadingDegrees = 350.0;
  intent.targetAltitudeMeters = 2500.0;
  intent.targetSpeedKnots = 200.0;

  applyMovementIntent(entity, intent, 2.0);

  EXPECT_DOUBLE_EQ(entity.speedKnots, 200.0);
  EXPECT_DOUBLE_EQ(entity.verticalSpeedMetersPerSecond, -20.0);
}

TEST(MovementIntent, CopiesEntityTaskSetpoints) {
  EntityTask task;
  task.targetHeadingDegrees = 270.0;
  task.targetAltitudeMeters = 7500;
  task.targetSpeedKnots = 320.0;

  const MovementIntent intent = movementIntentFromTask(task);

  EXPECT_DOUBLE_EQ(intent.targetHeadingDegrees, 270.0);
  EXPECT_DOUBLE_EQ(intent.targetAltitudeMeters, 7500.0);
  EXPECT_DOUBLE_EQ(intent.targetSpeedKnots, 320.0);
}

TEST(MovementIntent, FighterProfileAllowsSharperMovementThanHeavyAircraft) {
  Entity fighter = makeAirEntity();
  fighter.category = QStringLiteral("Fighter");
  Entity tanker = makeAirEntity();
  tanker.category = QStringLiteral("Tanker");

  const MovementControllerLimits fighterLimits =
      movementControllerLimitsForEntity(fighter);
  const MovementControllerLimits tankerLimits =
      movementControllerLimitsForEntity(tanker);

  EXPECT_GT(fighterLimits.headingRateDegreesPerSecond,
            tankerLimits.headingRateDegreesPerSecond);
  EXPECT_GT(fighterLimits.accelerationKnotsPerSecond,
            tankerLimits.accelerationKnotsPerSecond);
  EXPECT_GT(fighterLimits.climbRateMetersPerSecond,
            tankerLimits.climbRateMetersPerSecond);
  EXPECT_GT(fighterLimits.maxSpeedKnots,
            tankerLimits.maxSpeedKnots);
  EXPECT_GT(fighterLimits.maxAltitudeMeters,
            tankerLimits.maxAltitudeMeters);
}

TEST(MovementIntent, GroundProfileDoesNotCommandVerticalMovement) {
  Entity ground = makeAirEntity();
  ground.domain = QStringLiteral("Ground");
  ground.category = QStringLiteral("Vehicle");

  const MovementControllerLimits limits = movementControllerLimitsForEntity(ground);

  EXPECT_DOUBLE_EQ(limits.climbRateMetersPerSecond, 0.0);
}

TEST(MovementIntent, FighterProfileClampsImpossibleAltitudeAndSpeed) {
  Entity fighter = makeAirEntity();
  fighter.category = QStringLiteral("Fighter");
  fighter.altitude = 15990;
  fighter.speedKnots = 890.0;
  const MovementControllerLimits limits = movementControllerLimitsForEntity(fighter);

  MovementIntent intent;
  intent.targetHeadingDegrees = 350.0;
  intent.targetAltitudeMeters = 25000.0;
  intent.targetSpeedKnots = 1200.0;

  applyMovementIntent(fighter, intent, 1.0, limits);

  EXPECT_DOUBLE_EQ(fighter.speedKnots, 900.0);
  EXPECT_DOUBLE_EQ(fighter.verticalSpeedMetersPerSecond, 0.0);
}

TEST(MovementIntent, FighterProfileUsesOperationalClimbRate) {
  Entity fighter = makeAirEntity();
  fighter.category = QStringLiteral("Fighter");
  fighter.altitude = 3000;
  const MovementControllerLimits limits = movementControllerLimitsForEntity(fighter);

  MovementIntent intent;
  intent.targetHeadingDegrees = 350.0;
  intent.targetAltitudeMeters = 6000.0;
  intent.targetSpeedKnots = 400.0;

  applyMovementIntent(fighter, intent, 1.0, limits);

  EXPECT_DOUBLE_EQ(fighter.verticalSpeedMetersPerSecond, 35.0);
}
