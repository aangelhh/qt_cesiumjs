#include <gtest/gtest.h>

#include "application/dynamics/JSBSimDynamicsModel.h"

#include <cmath>
#include <vector>

namespace {

using application::dynamics::DynamicsControlSetpoint;
using application::dynamics::DynamicsModelConfiguration;
using application::dynamics::DynamicsState;
using application::dynamics::DynamicsStepContext;
using application::dynamics::JSBSimDynamicsModel;

DynamicsState makeCessnaState() {
  return DynamicsState{
      40.0,   // latitudeDegrees
      -3.0,   // longitudeDegrees
      1500.0, // altitudeMeters
      90.0,   // headingDegrees
      0.0,    // pitchDegrees
      0.0,    // rollDegrees
      110.0,  // speedKnots
      0.0,    // verticalSpeedMetersPerSecond
      100.0,  // fuelRemainingKilograms
      -1.0,   // fuelCapacityKilograms (output only, ignored as input)
  };
}

DynamicsState makeFighterState() {
  return DynamicsState{
      40.0, -3.0, 3000.0, 90.0, 0.0, 0.0, 320.0, 0.0, 2000.0, -1.0,
  };
}

constexpr double kDeterministicDeltaSeconds = 1.0 / 60.0;

DynamicsStepContext deterministicContextForTick(int tick) {
  DynamicsControlSetpoint setpoint{
      true, true, 90.0, 3000.0, 320.0, QStringLiteral("fighter-generic")};
  if (tick >= 80 && tick < 160) {
    setpoint.targetHeadingDegrees = 125.0;
    setpoint.targetAltitudeMeters = 3600.0;
    setpoint.targetSpeedKnots = 380.0;
  } else if (tick >= 160) {
    setpoint.targetHeadingDegrees = 70.0;
    setpoint.targetAltitudeMeters = 2800.0;
    setpoint.targetSpeedKnots = 300.0;
  }

  DynamicsStepContext context;
  context.simulationTimeSeconds =
      static_cast<double>(tick + 1) * kDeterministicDeltaSeconds;
  context.deltaTimeSeconds = kDeterministicDeltaSeconds;
  context.controlSetpoint = setpoint;
  return context;
}

void expectEquivalentDynamicsState(
    const DynamicsState& actual,
    const DynamicsState& expected) {
  // These tolerances allow harmless floating-point noise but catch any
  // simulation-visible divergence in a replay on the same build/platform.
  constexpr double kPositionToleranceDegrees = 1.0e-10;
  constexpr double kScalarTolerance = 1.0e-8;
  EXPECT_NEAR(
      actual.latitudeDegrees,
      expected.latitudeDegrees,
      kPositionToleranceDegrees);
  EXPECT_NEAR(
      actual.longitudeDegrees,
      expected.longitudeDegrees,
      kPositionToleranceDegrees);
  EXPECT_NEAR(actual.altitudeMeters, expected.altitudeMeters, kScalarTolerance);
  EXPECT_NEAR(actual.headingDegrees, expected.headingDegrees, kScalarTolerance);
  EXPECT_NEAR(actual.pitchDegrees, expected.pitchDegrees, kScalarTolerance);
  EXPECT_NEAR(actual.rollDegrees, expected.rollDegrees, kScalarTolerance);
  EXPECT_NEAR(actual.speedKnots, expected.speedKnots, kScalarTolerance);
  EXPECT_NEAR(
      actual.verticalSpeedMetersPerSecond,
      expected.verticalSpeedMetersPerSecond,
      kScalarTolerance);
  EXPECT_NEAR(
      actual.fuelRemainingKilograms,
      expected.fuelRemainingKilograms,
      kScalarTolerance);
  EXPECT_NEAR(
      actual.fuelCapacityKilograms,
      expected.fuelCapacityKilograms,
      kScalarTolerance);
}

} // namespace

TEST(JSBSimDynamicsModel, RequiresConfigurationAndInitialization) {
  JSBSimDynamicsModel model;
  EXPECT_EQ(model.backendId(), QStringLiteral("jsbsim"));
  EXPECT_FALSE(model.isInitialized());
  EXPECT_FALSE(model.initialize(makeCessnaState()));

  const DynamicsModelConfiguration configuration{
      QStringLiteral("c172x"), QStringLiteral("TestPlatform"), false, 200.0};
  ASSERT_TRUE(model.configure(configuration));
  ASSERT_TRUE(model.initialize(makeCessnaState()));
  EXPECT_TRUE(model.isInitialized());
  EXPECT_EQ(model.loadedModelName(), QStringLiteral("c172x"));
}

TEST(JSBSimDynamicsModel, StepAdvancesStateAndFuelStaysBounded) {
  JSBSimDynamicsModel model;
  // Uses the turbine "f16" model rather than the piston "c172x": JSBSim's
  // piston-engine "set-running" override does not reliably force a running
  // state (a pre-existing JSBSim/aircraft-model characteristic, not
  // something this extraction changes), so fuel flow for c172x can't be
  // relied on in a short test. f16 is the model the existing
  // JsbsimFlightControlSmoke regression test already exercises for exactly
  // this reason.
  const DynamicsModelConfiguration configuration{
      QStringLiteral("f16"), QStringLiteral("TestFighter"), false, 3000.0};
  ASSERT_TRUE(model.configure(configuration));
  ASSERT_TRUE(model.initialize(makeFighterState()));

  const DynamicsState afterInit = model.state();
  ASSERT_GT(afterInit.fuelCapacityKilograms, 0.0);
  ASSERT_GT(afterInit.fuelRemainingKilograms, 0.0);

  DynamicsStepContext context;
  context.simulationTimeSeconds = 0.0;
  context.deltaTimeSeconds = 1.0 / 60.0;
  context.controlSetpoint = DynamicsControlSetpoint{
      true, true, 90.0, 3000.0, 320.0, QStringLiteral("fighter-generic")};

  for (int tick = 0; tick < 180; ++tick) {
    context.simulationTimeSeconds += context.deltaTimeSeconds;
    const auto result = model.step(context);
    ASSERT_TRUE(result) << result.errorMessage.toStdString();
  }

  const DynamicsState finalState = model.state();
  EXPECT_TRUE(std::isfinite(finalState.latitudeDegrees));
  EXPECT_TRUE(std::isfinite(finalState.longitudeDegrees));
  EXPECT_GE(finalState.fuelRemainingKilograms, 0.0);
  EXPECT_LE(finalState.fuelRemainingKilograms, finalState.fuelCapacityKilograms);
  EXPECT_LT(finalState.fuelRemainingKilograms, afterInit.fuelRemainingKilograms);
}

TEST(JSBSimDynamicsModel, StepFailsWhenNotInitialized) {
  JSBSimDynamicsModel model;
  DynamicsStepContext context;
  context.deltaTimeSeconds = 1.0;
  const auto result = model.step(context);
  EXPECT_FALSE(result);
  EXPECT_FALSE(result.errorMessage.isEmpty());
}

TEST(JSBSimDynamicsModel, ResetReappliesInitialConditionsWithoutModelNameChange) {
  JSBSimDynamicsModel model;
  const DynamicsModelConfiguration configuration{
      QStringLiteral("c172x"), QStringLiteral("TestPlatform"), false, 200.0};
  ASSERT_TRUE(model.configure(configuration));
  ASSERT_TRUE(model.initialize(makeCessnaState()));

  DynamicsStepContext context;
  context.deltaTimeSeconds = 1.0;
  context.controlSetpoint = DynamicsControlSetpoint{
      false, false, 0.0, 0.0, 0.0, QString()};
  for (int tick = 0; tick < 3; ++tick) {
    ASSERT_TRUE(model.step(context));
  }

  DynamicsState resetState = makeCessnaState();
  resetState.latitudeDegrees = 41.5;
  resetState.longitudeDegrees = 2.1;
  ASSERT_TRUE(model.reset(resetState));
  EXPECT_TRUE(model.isInitialized());
  EXPECT_EQ(model.loadedModelName(), QStringLiteral("c172x"));

  const DynamicsState afterReset = model.state();
  EXPECT_NEAR(afterReset.latitudeDegrees, 41.5, 0.01);
  EXPECT_NEAR(afterReset.longitudeDegrees, 2.1, 0.01);
}

TEST(JSBSimDynamicsModel, ApplyExternalFuelOverrideUpdatesTanks) {
  JSBSimDynamicsModel model;
  // Request a capacity well above any real aircraft's physical tank size, so
  // the model clamps down to the true physical capacity and this test stays
  // correct regardless of that model-specific value.
  const DynamicsModelConfiguration configuration{
      QStringLiteral("c172x"), QStringLiteral("TestPlatform"), false, 5000.0};

  EXPECT_FALSE(model.applyExternalFuelOverride(50.0));

  ASSERT_TRUE(model.configure(configuration));
  ASSERT_TRUE(model.initialize(makeCessnaState()));

  const double capacity = model.state().fuelCapacityKilograms;
  ASSERT_GT(capacity, 0.0);
  const double target = capacity * 0.5;

  ASSERT_TRUE(model.applyExternalFuelOverride(target));
  const DynamicsState afterOverride = model.state();
  EXPECT_NEAR(afterOverride.fuelRemainingKilograms, target, 1.0);
  EXPECT_LE(afterOverride.fuelRemainingKilograms, afterOverride.fuelCapacityKilograms);
}

TEST(JSBSimDynamicsModel, EngineTelemetryReportsRunningEngineForFighterProfile) {
  JSBSimDynamicsModel model;
  const DynamicsModelConfiguration configuration{
      QStringLiteral("f16"), QStringLiteral("TestFighter"), false, 3000.0};
  ASSERT_TRUE(model.configure(configuration));
  ASSERT_TRUE(model.initialize(makeFighterState()));

  DynamicsStepContext context;
  context.deltaTimeSeconds = 1.0 / 60.0;
  context.controlSetpoint = DynamicsControlSetpoint{
      true, true, 90.0, 3000.0, 320.0, QStringLiteral("fighter-generic")};
  for (int tick = 0; tick < 180; ++tick) {
    context.simulationTimeSeconds += context.deltaTimeSeconds;
    ASSERT_TRUE(model.step(context));
  }

  const auto engines = model.engineTelemetry(/*entityDestroyed=*/false);
  ASSERT_EQ(engines.size(), 1);
  const application::EngineTelemetry& engine = engines.front();
  EXPECT_EQ(engine.state, QStringLiteral("RUNNING"));
  EXPECT_TRUE(engine.n1Available);
  EXPECT_TRUE(engine.n2Available);
  EXPECT_TRUE(engine.fuelFlowAvailable);
  EXPECT_TRUE(engine.thrustAvailable);
  EXPECT_GT(engine.n1Percent, 0.0);
  EXPECT_GT(engine.n2Percent, 0.0);

  const auto destroyedEngines = model.engineTelemetry(/*entityDestroyed=*/true);
  ASSERT_EQ(destroyedEngines.size(), 1);
  EXPECT_EQ(destroyedEngines.front().state, QStringLiteral("FAILED"));
}

TEST(JSBSimDynamicsModel, IdenticalInstancesRemainEquivalentAtEveryTick) {
  const DynamicsModelConfiguration configuration{
      QStringLiteral("f16"), QStringLiteral("DeterministicFighter"), false, 3000.0};
  JSBSimDynamicsModel first;
  JSBSimDynamicsModel second;
  ASSERT_TRUE(first.configure(configuration));
  ASSERT_TRUE(second.configure(configuration));
  ASSERT_TRUE(first.initialize(makeFighterState()));
  ASSERT_TRUE(second.initialize(makeFighterState()));

  for (int tick = 0; tick < 240; ++tick) {
    SCOPED_TRACE(::testing::Message() << "tick=" << tick);
    const DynamicsStepContext context = deterministicContextForTick(tick);
    const auto firstResult = first.step(context);
    const auto secondResult = second.step(context);
    ASSERT_TRUE(firstResult) << firstResult.errorMessage.toStdString();
    ASSERT_TRUE(secondResult) << secondResult.errorMessage.toStdString();
    expectEquivalentDynamicsState(first.state(), second.state());
  }
}

TEST(JSBSimDynamicsModel, ResetAndReplayReproduceTheRecordedTrajectory) {
  const DynamicsModelConfiguration configuration{
      QStringLiteral("f16"), QStringLiteral("ReplayFighter"), false, 3000.0};
  JSBSimDynamicsModel model;
  ASSERT_TRUE(model.configure(configuration));
  ASSERT_TRUE(model.initialize(makeFighterState()));

  std::vector<DynamicsState> recordedStates;
  recordedStates.reserve(240);
  for (int tick = 0; tick < 240; ++tick) {
    const auto result = model.step(deterministicContextForTick(tick));
    ASSERT_TRUE(result) << result.errorMessage.toStdString();
    recordedStates.push_back(model.state());
  }

  ASSERT_TRUE(model.reset(makeFighterState()));
  for (int tick = 0; tick < 240; ++tick) {
    SCOPED_TRACE(::testing::Message() << "replay tick=" << tick);
    const auto result = model.step(deterministicContextForTick(tick));
    ASSERT_TRUE(result) << result.errorMessage.toStdString();
    expectEquivalentDynamicsState(model.state(), recordedStates.at(tick));
  }
}
