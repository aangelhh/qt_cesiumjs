#include <gtest/gtest.h>

#include "application/dynamics/JSBSimDynamicsModel.h"

#include <cmath>

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
