#include "application/KinematicsTelemetry.h"

#include "application/MovementIntent.h"
#include "application/FlightDynamicsEngine.h"
#include "domain/GeoMath.h"

#include <cmath>

namespace application {
namespace {

constexpr double kDefaultPublicationPeriodSeconds = 0.1;

QString normalizedDynamicsModel(const Entity& entity) {
  const QString model = entity.flightDynamicsMode.trimmed();
  return model.isEmpty() ? QStringLiteral("kinematic") : model;
}

} // namespace

KinematicsTelemetrySnapshot makeKinematicsTelemetrySnapshot(
    const Entity& entity,
    double simulationTimeSeconds,
    double deltaTimeSeconds) {
  const MovementControllerLimits limits =
      movementControllerLimitsForEntity(entity);

  KinematicsTelemetrySnapshot snapshot;
  snapshot.entityName = entity.name;
  snapshot.domain = entity.domain;
  snapshot.category = entity.category;
  snapshot.forceIdentifier = entity.forceIdentifier;
  snapshot.simulationTimeSeconds = simulationTimeSeconds;
  snapshot.deltaTimeSeconds = deltaTimeSeconds;
  snapshot.latitudeDegrees = entity.latitude;
  snapshot.longitudeDegrees = entity.longitude;
  snapshot.altitudeMeters = static_cast<double>(entity.altitude);
  snapshot.headingDegrees = entity.headingDegrees;
  snapshot.pitchDegrees = entity.pitchDegrees;
  snapshot.rollDegrees = entity.rollDegrees;
  snapshot.speedKnots = entity.speedKnots;
  snapshot.verticalSpeedMetersPerSecond =
      entity.verticalSpeedMetersPerSecond;
  snapshot.targetHeadingDegrees = entity.currentTask.targetHeadingDegrees;
  snapshot.targetAltitudeMeters =
      static_cast<double>(entity.currentTask.targetAltitudeMeters);
  snapshot.targetSpeedKnots = entity.currentTask.targetSpeedKnots;
  snapshot.headingErrorDegrees = domain::shortestSignedAngle(
      entity.headingDegrees,
      entity.currentTask.targetHeadingDegrees);
  snapshot.altitudeErrorMeters =
      snapshot.targetAltitudeMeters - snapshot.altitudeMeters;
  snapshot.speedErrorKnots = snapshot.targetSpeedKnots - snapshot.speedKnots;
  snapshot.maxHeadingRateDegreesPerSecond =
      limits.headingRateDegreesPerSecond;
  snapshot.maxAccelerationKnotsPerSecond =
      limits.accelerationKnotsPerSecond;
  snapshot.maxClimbRateMetersPerSecond =
      limits.climbRateMetersPerSecond;
  snapshot.maxSpeedKnots = limits.maxSpeedKnots;
  snapshot.maxAltitudeMeters = limits.maxAltitudeMeters;
  snapshot.taskType = entity.currentTask.taskType;
  snapshot.taskStatus = entity.currentTask.status;
  snapshot.dynamicsModel = normalizedDynamicsModel(entity);
  snapshot.systems = FlightDynamicsEngine::systemsTelemetryForEntity(
      entity,
      limits.maxSpeedKnots);
  snapshot.taskEnabled = entity.currentTask.enabled;
  snapshot.destroyed = entity.destroyed;
  return snapshot;
}

KinematicsTelemetryPublisher::KinematicsTelemetryPublisher(
    double publicationPeriodSeconds)
    : _publicationPeriodSeconds(
          std::isfinite(publicationPeriodSeconds) &&
                  publicationPeriodSeconds > 0.0
              ? publicationPeriodSeconds
              : kDefaultPublicationPeriodSeconds) {}

QVector<KinematicsTelemetrySnapshot> KinematicsTelemetryPublisher::advance(
    const QVector<Entity>& entities,
    double simulationTimeSeconds,
    double deltaTimeSeconds) {
  if (!std::isfinite(deltaTimeSeconds) || deltaTimeSeconds <= 0.0) {
    return {};
  }

  _elapsedSeconds += deltaTimeSeconds;
  if (_elapsedSeconds + 1.0e-12 < _publicationPeriodSeconds) {
    return {};
  }
  _elapsedSeconds = std::fmod(_elapsedSeconds, _publicationPeriodSeconds);

  QVector<KinematicsTelemetrySnapshot> snapshots;
  snapshots.reserve(entities.size());
  for (const Entity& entity : entities) {
    snapshots.push_back(makeKinematicsTelemetrySnapshot(
        entity,
        simulationTimeSeconds,
        deltaTimeSeconds));
  }
  return snapshots;
}

void KinematicsTelemetryPublisher::reset() {
  _elapsedSeconds = 0.0;
}

double KinematicsTelemetryPublisher::publicationPeriodSeconds() const {
  return _publicationPeriodSeconds;
}

} // namespace application
