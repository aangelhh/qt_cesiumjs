#include <gtest/gtest.h>

#include "application/dynamics/KinematicDynamicsModel.h"

#include <limits>

namespace {

application::dynamics::DynamicsState makeAirState() {
  return {
      40.0,
      -3.0,
      3000.0,
      90.0,
      4.0,
      8.0,
      300.0,
      10.0,
  };
}

} // namespace

TEST(KinematicDynamicsModel, RequiresConfigurationAndInitialization) {
  application::dynamics::KinematicDynamicsModel model;

  EXPECT_EQ(model.backendId(), QStringLiteral("kinematic"));
  EXPECT_FALSE(model.isInitialized());
  EXPECT_FALSE(model.initialize(makeAirState()));

  ASSERT_TRUE(model.configure({QStringLiteral("kinematic"), {}, false}));
  ASSERT_TRUE(model.initialize(makeAirState()));
  EXPECT_TRUE(model.isInitialized());
}

TEST(KinematicDynamicsModel, AdvancesHorizontalAndVerticalState) {
  application::dynamics::KinematicDynamicsModel model;
  ASSERT_TRUE(model.configure({QStringLiteral("kinematic"), {}, false}));
  ASSERT_TRUE(model.initialize(makeAirState()));

  const auto result = model.step({120.0, 2.0});

  ASSERT_TRUE(result.advanced) << result.errorMessage.toStdString();
  const auto state = model.state();
  EXPECT_NEAR(state.latitudeDegrees, 40.0, 1e-5);
  EXPECT_GT(state.longitudeDegrees, -3.0);
  EXPECT_DOUBLE_EQ(state.altitudeMeters, 3020.0);
  EXPECT_DOUBLE_EQ(state.headingDegrees, 90.0);
  EXPECT_DOUBLE_EQ(state.speedKnots, 300.0);
}

TEST(KinematicDynamicsModel, GroundConfigurationPreservesAltitudeAndLevelsAttitude) {
  application::dynamics::KinematicDynamicsModel model;
  auto state = makeAirState();
  state.altitudeMeters = 725.0;
  ASSERT_TRUE(model.configure({QStringLiteral("kinematic"), {}, true}));
  ASSERT_TRUE(model.initialize(state));

  ASSERT_TRUE(model.step({10.0, 5.0}));

  const auto next = model.state();
  EXPECT_DOUBLE_EQ(next.altitudeMeters, 725.0);
  EXPECT_DOUBLE_EQ(next.verticalSpeedMetersPerSecond, 0.0);
  EXPECT_DOUBLE_EQ(next.pitchDegrees, 0.0);
  EXPECT_DOUBLE_EQ(next.rollDegrees, 0.0);
  EXPECT_NE(next.longitudeDegrees, state.longitudeDegrees);
}

TEST(KinematicDynamicsModel, ResetAndShutdownControlLifecycle) {
  application::dynamics::KinematicDynamicsModel model;
  ASSERT_TRUE(model.configure({QStringLiteral("kinematic"), {}, false}));
  ASSERT_TRUE(model.initialize(makeAirState()));
  ASSERT_TRUE(model.step({1.0, 1.0}));

  auto resetState = makeAirState();
  resetState.latitudeDegrees = 50.0;
  resetState.speedKnots = 0.0;
  ASSERT_TRUE(model.reset(resetState));
  EXPECT_DOUBLE_EQ(model.state().latitudeDegrees, 50.0);

  model.shutdown();
  EXPECT_FALSE(model.isInitialized());
  const auto result = model.step({2.0, 1.0});
  EXPECT_FALSE(result.advanced);
}

TEST(KinematicDynamicsModel, RejectsInvalidStateAndTimeStep) {
  application::dynamics::KinematicDynamicsModel model;
  ASSERT_TRUE(model.configure({QStringLiteral("kinematic"), {}, false}));

  auto invalidState = makeAirState();
  invalidState.latitudeDegrees = std::numeric_limits<double>::quiet_NaN();
  EXPECT_FALSE(model.initialize(invalidState));

  ASSERT_TRUE(model.initialize(makeAirState()));
  EXPECT_FALSE(model.step({0.0, 0.0}));
  EXPECT_FALSE(model.step({0.0, -1.0}));
}
