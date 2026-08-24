#include <gtest/gtest.h>

#include "application/dynamics/DynamicsBackendHealth.h"

#include <limits>

namespace {

using application::dynamics::DynamicsBackendBudgetPolicy;
using application::dynamics::DynamicsBackendHealth;
using application::dynamics::DynamicsState;

DynamicsState validState() {
  return {
      40.0, -3.0, 3000.0, 90.0, 2.0, 5.0, 320.0, 1.0, 1000.0, 2000.0,
  };
}

} // namespace

TEST(DynamicsBackendHealth, AcceptsCanonicalStateAndRejectsInvalidOutput) {
  QString reason;
  EXPECT_TRUE(application::dynamics::dynamicsStateIsValid(validState(), &reason));
  EXPECT_TRUE(reason.isEmpty());

  auto invalid = validState();
  invalid.rollDegrees = std::numeric_limits<double>::quiet_NaN();
  EXPECT_FALSE(application::dynamics::dynamicsStateIsValid(invalid, &reason));
  EXPECT_FALSE(reason.isEmpty());

  invalid = validState();
  invalid.latitudeDegrees = 91.0;
  EXPECT_FALSE(application::dynamics::dynamicsStateIsValid(invalid, &reason));

  invalid = validState();
  invalid.fuelRemainingKilograms = 2500.0;
  EXPECT_FALSE(application::dynamics::dynamicsStateIsValid(invalid, &reason));
}

TEST(DynamicsBackendHealth, LatchesAfterConsecutiveBudgetOverruns) {
  DynamicsBackendHealth health;
  const DynamicsBackendBudgetPolicy policy{/*maxStepMilliseconds=*/5.0,
                                           /*consecutiveOverrunLimit=*/3};

  EXPECT_TRUE(application::dynamics::recordDynamicsStepDuration(
      health, 6.0, policy));
  EXPECT_EQ(health.consecutiveBudgetOverruns, 1);
  EXPECT_TRUE(application::dynamics::recordDynamicsStepDuration(
      health, 7.0, policy));
  EXPECT_EQ(health.consecutiveBudgetOverruns, 2);
  EXPECT_FALSE(application::dynamics::recordDynamicsStepDuration(
      health, 8.0, policy));
  EXPECT_TRUE(health.fallbackLatched);
  EXPECT_TRUE(health.fallbackReason.contains(QStringLiteral("budget exceeded")));
}

TEST(DynamicsBackendHealth, HealthyStepBreaksOverrunSequence) {
  DynamicsBackendHealth health;
  const DynamicsBackendBudgetPolicy policy{/*maxStepMilliseconds=*/5.0,
                                           /*consecutiveOverrunLimit=*/2};

  EXPECT_TRUE(application::dynamics::recordDynamicsStepDuration(
      health, 6.0, policy));
  EXPECT_TRUE(application::dynamics::recordDynamicsStepDuration(
      health, 4.0, policy));
  EXPECT_EQ(health.consecutiveBudgetOverruns, 0);
  EXPECT_TRUE(application::dynamics::recordDynamicsStepDuration(
      health, 6.0, policy));
  EXPECT_FALSE(health.fallbackLatched);
}

TEST(DynamicsBackendHealth, ObserveOnlyBudgetCannotAlterDeterministicExecution) {
  DynamicsBackendHealth health;
  const DynamicsBackendBudgetPolicy policy{/*maxStepMilliseconds=*/1.0,
                                           /*consecutiveOverrunLimit=*/1,
                                           /*enforceFallback=*/false};

  EXPECT_TRUE(application::dynamics::recordDynamicsStepDuration(
      health, 50.0, policy));
  EXPECT_DOUBLE_EQ(health.lastStepMilliseconds, 50.0);
  EXPECT_EQ(health.consecutiveBudgetOverruns, 0);
  EXPECT_FALSE(health.fallbackLatched);
  EXPECT_TRUE(health.fallbackReason.isEmpty());
}

TEST(DynamicsBackendHealth, ExplicitFailureRemainsLatched) {
  DynamicsBackendHealth health;
  application::dynamics::latchDynamicsBackendFallback(
      health,
      QStringLiteral("Model initialization failed"));

  EXPECT_TRUE(health.fallbackLatched);
  EXPECT_EQ(
      health.fallbackReason,
      QStringLiteral("Model initialization failed"));
  EXPECT_FALSE(application::dynamics::recordDynamicsStepDuration(
      health, 1.0));
}
