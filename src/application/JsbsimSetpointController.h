#pragma once

#include <QString>

namespace application {

struct JsbsimControlState {
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double bankDegrees = 0.0;
  double altitudeMeters = 0.0;
  double trueAirspeedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
};

struct JsbsimControlSetpoint {
  double headingDegrees = 0.0;
  double altitudeMeters = 0.0;
  double speedKnots = 0.0;
};

struct JsbsimControlOutput {
  double aileronNormalized = 0.0;
  double elevatorNormalized = 0.0;
  double rudderNormalized = 0.0;
  double throttleNormalized = 0.5;
};

JsbsimControlOutput computeJsbsimSetpointControl(
    const JsbsimControlState& state,
    const JsbsimControlSetpoint& setpoint,
    const QString& profileId);

JsbsimControlOutput rateLimitJsbsimControl(
    const JsbsimControlOutput& previous,
    const JsbsimControlOutput& requested,
    double deltaSeconds,
    const QString& profileId);

} // namespace application
