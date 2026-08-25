#include "infrastructure/Ros2TelemetryContract.h"

#include <QRegularExpression>

namespace infrastructure {

Ros2KinematicsTelemetryMessage makeRos2TelemetryMessage(
    const application::KinematicsTelemetrySnapshot& snapshot) {
  Ros2KinematicsTelemetryMessage message;
  message.schemaVersion = QStringLiteral("qttest.kinematics.v1");
  message.entityId = snapshot.entityId.trimmed().isEmpty()
      ? snapshot.entityName
      : snapshot.entityId;
  message.entityName = snapshot.entityName;
  message.domain = snapshot.domain;
  message.category = snapshot.category;
  message.forceIdentifier = snapshot.forceIdentifier;
  message.taskType = snapshot.taskType;
  message.taskStatus = snapshot.taskStatus;
  message.dynamicsModel = snapshot.dynamicsModel;
  message.dynamicsFallbackReason = snapshot.dynamicsFallbackReason;
  message.simulationTimeSeconds = snapshot.simulationTimeSeconds;
  message.deltaTimeSeconds = snapshot.deltaTimeSeconds;
  message.latitudeDegrees = snapshot.latitudeDegrees;
  message.longitudeDegrees = snapshot.longitudeDegrees;
  message.altitudeMeters = snapshot.altitudeMeters;
  message.headingDegrees = snapshot.headingDegrees;
  message.pitchDegrees = snapshot.pitchDegrees;
  message.rollDegrees = snapshot.rollDegrees;
  message.speedKnots = snapshot.speedKnots;
  message.verticalSpeedMetersPerSecond =
      snapshot.verticalSpeedMetersPerSecond;
  message.targetHeadingDegrees = snapshot.targetHeadingDegrees;
  message.targetAltitudeMeters = snapshot.targetAltitudeMeters;
  message.targetSpeedKnots = snapshot.targetSpeedKnots;
  message.headingErrorDegrees = snapshot.headingErrorDegrees;
  message.altitudeErrorMeters = snapshot.altitudeErrorMeters;
  message.speedErrorKnots = snapshot.speedErrorKnots;
  message.maxHeadingRateDegreesPerSecond =
      snapshot.maxHeadingRateDegreesPerSecond;
  message.maxAccelerationKnotsPerSecond =
      snapshot.maxAccelerationKnotsPerSecond;
  message.maxClimbRateMetersPerSecond =
      snapshot.maxClimbRateMetersPerSecond;
  message.maxSpeedKnots = snapshot.maxSpeedKnots;
  message.maxAltitudeMeters = snapshot.maxAltitudeMeters;
  message.dynamicsStepDurationMilliseconds =
      snapshot.dynamicsStepDurationMilliseconds;
  message.taskEnabled = snapshot.taskEnabled;
  message.destroyed = snapshot.destroyed;
  message.fuelCapacityKilograms = snapshot.systems.fuelCapacityKilograms;
  message.fuelRemainingKilograms = snapshot.systems.fuelRemainingKilograms;
  message.fuelPercent = snapshot.systems.fuelPercent;
  message.totalFuelFlowKilogramsPerHour =
      snapshot.systems.totalFuelFlowKilogramsPerHour;
  message.estimatedEnduranceSeconds =
      snapshot.systems.estimatedEnduranceSeconds;
  message.engines.reserve(snapshot.systems.engines.size());
  for (const application::EngineTelemetry& engine : snapshot.systems.engines) {
    Ros2EngineTelemetryMessage engineMessage;
    engineMessage.engineId = engine.engineId;
    engineMessage.state = engine.state;
    engineMessage.n1Percent = engine.n1Percent;
    engineMessage.n2Percent = engine.n2Percent;
    engineMessage.exhaustTemperatureCelsius =
        engine.exhaustTemperatureCelsius;
    engineMessage.fuelFlowKilogramsPerHour =
        engine.fuelFlowKilogramsPerHour;
    engineMessage.thrustKilonewtons = engine.thrustKilonewtons;
    engineMessage.available = engine.available;
    engineMessage.estimated = engine.estimated;
    message.engines.push_back(engineMessage);
  }
  return message;
}

QString ros2TopicComponent(const QString& value) {
  QString component = value.trimmed().toLower();
  component.replace(
      QRegularExpression(QStringLiteral("[^a-z0-9_]+")),
      QStringLiteral("_"));
  component.replace(
      QRegularExpression(QStringLiteral("_+")),
      QStringLiteral("_"));
  component.remove(QRegularExpression(QStringLiteral("^_+|_+$")));
  if (component.isEmpty()) {
    component = QStringLiteral("unknown");
  }
  if (component.front().isDigit()) {
    component.prepend(QStringLiteral("entity_"));
  }
  return component;
}

QString ros2KinematicsTopic(
    const QString& topicPrefix,
    const QString& entityId) {
  QString prefix = topicPrefix.trimmed();
  if (prefix.isEmpty()) {
    prefix = QStringLiteral("/qttest/entities");
  }
  if (!prefix.startsWith(QLatin1Char('/'))) {
    prefix.prepend(QLatin1Char('/'));
  }
  while (prefix.endsWith(QLatin1Char('/'))) {
    prefix.chop(1);
  }
  return QStringLiteral("%1/%2/kinematics")
      .arg(prefix, ros2TopicComponent(entityId));
}

} // namespace infrastructure
