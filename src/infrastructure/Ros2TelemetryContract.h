#pragma once

#include "application/KinematicsTelemetry.h"

#include <QString>
#include <QVector>

namespace infrastructure {

struct Ros2EngineTelemetryMessage {
  QString engineId;
  QString state;
  double n1Percent = 0.0;
  double n2Percent = 0.0;
  double exhaustTemperatureCelsius = 0.0;
  double fuelFlowKilogramsPerHour = 0.0;
  double thrustKilonewtons = 0.0;
  bool available = false;
  bool estimated = false;
};

struct Ros2KinematicsTelemetryMessage {
  QString schemaVersion;
  QString entityId;
  QString entityName;
  QString domain;
  QString category;
  int forceIdentifier = 0;
  QString taskType;
  QString taskStatus;
  QString dynamicsModel;
  QString dynamicsFallbackReason;

  double simulationTimeSeconds = 0.0;
  double deltaTimeSeconds = 0.0;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;
  double speedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
  double targetHeadingDegrees = 0.0;
  double targetAltitudeMeters = 0.0;
  double targetSpeedKnots = 0.0;
  double headingErrorDegrees = 0.0;
  double altitudeErrorMeters = 0.0;
  double speedErrorKnots = 0.0;
  double maxHeadingRateDegreesPerSecond = 0.0;
  double maxAccelerationKnotsPerSecond = 0.0;
  double maxClimbRateMetersPerSecond = 0.0;
  double maxSpeedKnots = 0.0;
  double maxAltitudeMeters = 0.0;
  double dynamicsStepDurationMilliseconds = 0.0;
  bool taskEnabled = false;
  bool destroyed = false;
  double fuelCapacityKilograms = 0.0;
  double fuelRemainingKilograms = 0.0;
  double fuelPercent = 0.0;
  double totalFuelFlowKilogramsPerHour = 0.0;
  double estimatedEnduranceSeconds = 0.0;
  QVector<Ros2EngineTelemetryMessage> engines;
};

Ros2KinematicsTelemetryMessage makeRos2TelemetryMessage(
    const application::KinematicsTelemetrySnapshot& snapshot);
QString ros2TopicComponent(const QString& value);
QString ros2KinematicsTopic(
    const QString& topicPrefix,
    const QString& entityId);

} // namespace infrastructure
