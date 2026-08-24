#pragma once

#include "application/dynamics/IDynamicsModel.h"

#include <QString>

#include <cmath>

namespace application::dynamics {

struct DynamicsBackendBudgetPolicy {
  double maxStepMilliseconds = 8.0;
  int consecutiveOverrunLimit = 3;
  bool enforceFallback = true;
};

struct DynamicsBackendHealth {
  double lastStepMilliseconds = 0.0;
  int consecutiveBudgetOverruns = 0;
  bool fallbackLatched = false;
  QString fallbackReason;
};

inline bool dynamicsStateIsValid(
    const DynamicsState& state,
    QString* reason = nullptr) {
  const bool finite =
      std::isfinite(state.latitudeDegrees) &&
      std::isfinite(state.longitudeDegrees) &&
      std::isfinite(state.altitudeMeters) &&
      std::isfinite(state.headingDegrees) &&
      std::isfinite(state.pitchDegrees) &&
      std::isfinite(state.rollDegrees) &&
      std::isfinite(state.speedKnots) &&
      std::isfinite(state.verticalSpeedMetersPerSecond) &&
      std::isfinite(state.fuelRemainingKilograms) &&
      std::isfinite(state.fuelCapacityKilograms);
  if (!finite) {
    if (reason) {
      *reason = QStringLiteral("Dynamics state contains a non-finite value");
    }
    return false;
  }
  if (state.latitudeDegrees < -90.0 || state.latitudeDegrees > 90.0 ||
      state.longitudeDegrees < -180.0 || state.longitudeDegrees > 180.0) {
    if (reason) {
      *reason = QStringLiteral("Dynamics state contains invalid WGS84 coordinates");
    }
    return false;
  }
  if (state.speedKnots < 0.0) {
    if (reason) {
      *reason = QStringLiteral("Dynamics state contains negative speed");
    }
    return false;
  }
  if (state.fuelRemainingKilograms >= 0.0 &&
      state.fuelCapacityKilograms >= 0.0 &&
      state.fuelRemainingKilograms > state.fuelCapacityKilograms + 1.0e-6) {
    if (reason) {
      *reason = QStringLiteral("Dynamics state fuel exceeds configured capacity");
    }
    return false;
  }
  return true;
}

inline void latchDynamicsBackendFallback(
    DynamicsBackendHealth& health,
    const QString& reason) {
  health.fallbackLatched = true;
  health.fallbackReason = reason.trimmed().isEmpty()
      ? QStringLiteral("Dynamics backend failed")
      : reason.trimmed();
}

inline bool recordDynamicsStepDuration(
    DynamicsBackendHealth& health,
    double elapsedMilliseconds,
    const DynamicsBackendBudgetPolicy& policy = {}) {
  health.lastStepMilliseconds =
      std::isfinite(elapsedMilliseconds) && elapsedMilliseconds >= 0.0
          ? elapsedMilliseconds
          : 0.0;
  if (health.fallbackLatched) {
    return false;
  }

  const double budgetMilliseconds =
      std::isfinite(policy.maxStepMilliseconds) &&
              policy.maxStepMilliseconds > 0.0
          ? policy.maxStepMilliseconds
          : DynamicsBackendBudgetPolicy{}.maxStepMilliseconds;
  const int overrunLimit = policy.consecutiveOverrunLimit > 0
      ? policy.consecutiveOverrunLimit
      : DynamicsBackendBudgetPolicy{}.consecutiveOverrunLimit;

  if (health.lastStepMilliseconds <= budgetMilliseconds) {
    health.consecutiveBudgetOverruns = 0;
    return true;
  }

  if (!policy.enforceFallback) {
    // Wall-clock load must not alter simulation state during deterministic
    // replay or fast-time execution.
    health.consecutiveBudgetOverruns = 0;
    return true;
  }

  ++health.consecutiveBudgetOverruns;
  if (health.consecutiveBudgetOverruns < overrunLimit) {
    return true;
  }

  latchDynamicsBackendFallback(
      health,
      QStringLiteral("Step budget exceeded: %1 ms > %2 ms for %3 consecutive ticks")
          .arg(health.lastStepMilliseconds, 0, 'f', 2)
          .arg(budgetMilliseconds, 0, 'f', 2)
          .arg(overrunLimit));
  return false;
}

} // namespace application::dynamics
