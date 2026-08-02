#include <gtest/gtest.h>

#include "application/JsbsimSetpointController.h"

using namespace application;

TEST(JsbsimSetpointController, CommandsBankAcrossHeadingWraparound) {
  JsbsimControlState state;
  state.headingDegrees = 350.0;
  state.trueAirspeedKnots = 320.0;
  state.altitudeMeters = 3000.0;

  JsbsimControlSetpoint setpoint;
  setpoint.headingDegrees = 10.0;
  setpoint.altitudeMeters = 3000.0;
  setpoint.speedKnots = 320.0;

  const JsbsimControlOutput output = computeJsbsimSetpointControl(
      state,
      setpoint,
      QStringLiteral("fighter-generic"));

  EXPECT_GT(output.aileronNormalized, 0.0);
  EXPECT_DOUBLE_EQ(output.rudderNormalized, 0.0);
}

TEST(JsbsimSetpointController, OpposesExistingBankWhenHeadingIsCaptured) {
  JsbsimControlState state;
  state.headingDegrees = 90.0;
  state.bankDegrees = 20.0;
  state.trueAirspeedKnots = 320.0;
  state.altitudeMeters = 3000.0;

  JsbsimControlSetpoint setpoint;
  setpoint.headingDegrees = 90.0;
  setpoint.altitudeMeters = 3000.0;
  setpoint.speedKnots = 320.0;

  const JsbsimControlOutput output = computeJsbsimSetpointControl(
      state,
      setpoint,
      QStringLiteral("fighter-generic"));

  EXPECT_LT(output.aileronNormalized, 0.0);
}

TEST(JsbsimSetpointController, UsesBoundedPitchCommandForAltitudeChanges) {
  JsbsimControlState state;
  state.headingDegrees = 90.0;
  state.trueAirspeedKnots = 320.0;
  state.altitudeMeters = 3000.0;

  JsbsimControlSetpoint climb;
  climb.headingDegrees = 90.0;
  climb.altitudeMeters = 10000.0;
  climb.speedKnots = 320.0;
  const JsbsimControlOutput climbOutput = computeJsbsimSetpointControl(
      state,
      climb,
      QStringLiteral("fighter-generic"));

  JsbsimControlSetpoint descend = climb;
  descend.altitudeMeters = 500.0;
  const JsbsimControlOutput descendOutput = computeJsbsimSetpointControl(
      state,
      descend,
      QStringLiteral("fighter-generic"));

  EXPECT_LT(climbOutput.elevatorNormalized, 0.0);
  EXPECT_GT(descendOutput.elevatorNormalized, 0.0);
  EXPECT_LE(qAbs(climbOutput.elevatorNormalized), 1.0);
  EXPECT_LE(qAbs(descendOutput.elevatorNormalized), 1.0);
}

TEST(JsbsimSetpointController, EnforcesSafeFighterMinimumSpeed) {
  JsbsimControlState state;
  state.trueAirspeedKnots = 250.0;

  JsbsimControlSetpoint setpoint;
  setpoint.speedKnots = 0.0;

  const JsbsimControlOutput output = computeJsbsimSetpointControl(
      state,
      setpoint,
      QStringLiteral("fighter-generic"));

  EXPECT_GT(output.throttleNormalized, 0.0);
  EXPECT_LT(output.throttleNormalized, 0.5);
}

TEST(JsbsimSetpointController, RateLimitsAbruptControlChanges) {
  JsbsimControlOutput previous;
  JsbsimControlOutput requested;
  requested.aileronNormalized = 1.0;
  requested.elevatorNormalized = -1.0;
  requested.rudderNormalized = 1.0;
  requested.throttleNormalized = 1.0;

  const JsbsimControlOutput limited = rateLimitJsbsimControl(
      previous,
      requested,
      0.1,
      QStringLiteral("fighter-generic"));

  EXPECT_NEAR(limited.aileronNormalized, 0.08, 1e-9);
  EXPECT_NEAR(limited.elevatorNormalized, -0.045, 1e-9);
  EXPECT_NEAR(limited.rudderNormalized, 0.08, 1e-9);
  EXPECT_NEAR(limited.throttleNormalized, 0.53, 1e-9);
}
