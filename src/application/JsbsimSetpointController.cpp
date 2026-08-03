#include "application/JsbsimSetpointController.h"

#include <QtGlobal>
#include <QtMath>

#include <cmath>

namespace application {
namespace {

struct ControlProfile {
  double maximumBankDegrees = 20.0;
  double maximumFlightPathDegrees = 7.0;
  double maximumVerticalSpeedMetersPerSecond = 25.0;
  double minimumAirspeedKnots = 110.0;
  double headingToBankGain = 0.25;
  double bankToAileronGain = 0.012;
  double pitchToElevatorGain = -0.018;
  double speedToThrottleGain = 0.003;
  double trimPitchDegrees = 2.0;
  double lateralCommandRatePerSecond = 0.8;
  double elevatorCommandRatePerSecond = 0.45;
  double throttleCommandRatePerSecond = 0.3;
};

ControlProfile profileFor(const QString& profileId) {
  ControlProfile profile;
  if (profileId.trimmed().compare(
          QStringLiteral("fighter-generic"),
          Qt::CaseInsensitive) == 0) {
    profile.maximumBankDegrees = 30.0;
    profile.maximumFlightPathDegrees = 10.0;
    profile.maximumVerticalSpeedMetersPerSecond = 40.0;
    profile.minimumAirspeedKnots = 180.0;
    profile.headingToBankGain = 0.3;
    profile.bankToAileronGain = 0.014;
    profile.pitchToElevatorGain = -0.02;
  }
  return profile;
}

double normalizeDegrees360(double angleDegrees) {
  double normalized = std::fmod(angleDegrees, 360.0);
  if (normalized < 0.0) {
    normalized += 360.0;
  }
  return normalized;
}

double shortestSignedAngle(double fromDegrees, double toDegrees) {
  double delta = normalizeDegrees360(toDegrees) -
      normalizeDegrees360(fromDegrees);
  if (delta > 180.0) {
    delta -= 360.0;
  } else if (delta < -180.0) {
    delta += 360.0;
  }
  return delta;
}

double normalizedSurface(double value) {
  return qBound(-1.0, value, 1.0);
}

double normalizedThrottle(double value) {
  return qBound(0.0, value, 1.0);
}

double approach(double current, double target, double maximumDelta) {
  return current + qBound(-maximumDelta, target - current, maximumDelta);
}

} // namespace

JsbsimControlOutput computeJsbsimSetpointControl(
    const JsbsimControlState& state,
    const JsbsimControlSetpoint& setpoint,
    const QString& profileId) {
  const ControlProfile profile = profileFor(profileId);
  JsbsimControlOutput output;

  const double headingErrorDegrees = shortestSignedAngle(
      state.headingDegrees,
      setpoint.headingDegrees);
  const double targetBankDegrees = qBound(
      -profile.maximumBankDegrees,
      headingErrorDegrees * profile.headingToBankGain,
      profile.maximumBankDegrees);
  const double bankErrorDegrees = targetBankDegrees - state.bankDegrees;
  output.aileronNormalized = normalizedSurface(
      bankErrorDegrees * profile.bankToAileronGain);

  const double altitudeErrorMeters =
      setpoint.altitudeMeters - state.altitudeMeters;
  const double desiredVerticalSpeed = qBound(
      -profile.maximumVerticalSpeedMetersPerSecond,
      altitudeErrorMeters * 0.04,
      profile.maximumVerticalSpeedMetersPerSecond);
  const double airspeedMetersPerSecond = qMax(
      profile.minimumAirspeedKnots,
      state.trueAirspeedKnots) * 0.514444;
  const double flightPathRatio = qBound(
      -0.95,
      desiredVerticalSpeed / airspeedMetersPerSecond,
      0.95);
  const double targetFlightPathDegrees = qBound(
      -profile.maximumFlightPathDegrees,
      qRadiansToDegrees(std::asin(flightPathRatio)),
      profile.maximumFlightPathDegrees);
  const double targetPitchDegrees =
      profile.trimPitchDegrees + targetFlightPathDegrees;
  const double pitchErrorDegrees = targetPitchDegrees - state.pitchDegrees;
  output.elevatorNormalized = normalizedSurface(
      pitchErrorDegrees * profile.pitchToElevatorGain);

  // Coordinated turns are produced with bank. Direct rudder from heading error
  // caused yaw oscillation on fighter models.
  output.rudderNormalized = 0.0;

  const double safeTargetSpeedKnots = qMax(
      profile.minimumAirspeedKnots,
      setpoint.speedKnots);
  const double speedErrorKnots =
      safeTargetSpeedKnots - state.trueAirspeedKnots;
  output.throttleNormalized = normalizedThrottle(
      0.5 + speedErrorKnots * profile.speedToThrottleGain);
  return output;
}

JsbsimControlOutput rateLimitJsbsimControl(
    const JsbsimControlOutput& previous,
    const JsbsimControlOutput& requested,
    double deltaSeconds,
    const QString& profileId) {
  if (deltaSeconds <= 0.0) {
    return previous;
  }
  const ControlProfile profile = profileFor(profileId);
  JsbsimControlOutput limited;
  limited.aileronNormalized = approach(
      previous.aileronNormalized,
      requested.aileronNormalized,
      profile.lateralCommandRatePerSecond * deltaSeconds);
  limited.elevatorNormalized = approach(
      previous.elevatorNormalized,
      requested.elevatorNormalized,
      profile.elevatorCommandRatePerSecond * deltaSeconds);
  limited.rudderNormalized = approach(
      previous.rudderNormalized,
      requested.rudderNormalized,
      profile.lateralCommandRatePerSecond * deltaSeconds);
  limited.throttleNormalized = approach(
      previous.throttleNormalized,
      requested.throttleNormalized,
      profile.throttleCommandRatePerSecond * deltaSeconds);
  return limited;
}

} // namespace application
