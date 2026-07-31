#pragma once

#include "domain/Entity.h"
#include "application/SystemsTelemetry.h"

#include <QString>
#include <QVector>

namespace application {

// Read-only-by-contract value snapshot. It never retains references to runtime state.
struct KinematicsTelemetrySnapshot {
  QString entityName;
  QString domain;
  QString category;
  int forceIdentifier = 0;

  // Simulation clock values, in seconds.
  double simulationTimeSeconds = 0.0;
  double deltaTimeSeconds = 0.0;

  // Actual WGS84 position and kinematic state.
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;
  double speedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;

  // Current movement setpoints and signed tracking errors.
  double targetHeadingDegrees = 0.0;
  double targetAltitudeMeters = 0.0;
  double targetSpeedKnots = 0.0;
  double headingErrorDegrees = 0.0;
  double altitudeErrorMeters = 0.0;
  double speedErrorKnots = 0.0;

  // Active controller envelope, using the units named by each field.
  double maxHeadingRateDegreesPerSecond = 0.0;
  double maxAccelerationKnotsPerSecond = 0.0;
  double maxClimbRateMetersPerSecond = 0.0;
  double maxSpeedKnots = 0.0;
  double maxAltitudeMeters = 0.0;

  QString taskType;
  QString taskStatus;
  QString dynamicsModel;
  SystemsTelemetrySnapshot systems;
  bool taskEnabled = false;
  bool destroyed = false;
};

KinematicsTelemetrySnapshot makeKinematicsTelemetrySnapshot(
    const Entity& entity,
    double simulationTimeSeconds,
    double deltaTimeSeconds);

class KinematicsTelemetryPublisher {
public:
  explicit KinematicsTelemetryPublisher(double publicationPeriodSeconds = 0.1);

  QVector<KinematicsTelemetrySnapshot> advance(
      const QVector<Entity>& entities,
      double simulationTimeSeconds,
      double deltaTimeSeconds);

  void reset();
  double publicationPeriodSeconds() const;

private:
  double _publicationPeriodSeconds;
  double _elapsedSeconds = 0.0;
};

} // namespace application
