#include "application/SensorEngine.h"

#include "application/sensors/SensorModelRegistry.h"
#include "domain/EntityIdentity.h"
#include "domain/GeoMath.h"

#include <QElapsedTimer>
#include <QtMath>

#include <algorithm>
#include <cmath>

namespace {

double normalizeDegrees180(double degrees) {
  while (degrees < -180.0) {
    degrees += 360.0;
  }
  while (degrees > 180.0) {
    degrees -= 360.0;
  }
  return degrees;
}

double distanceMeters(const Entity& source, const Entity& target) {
  return domain::slantDistanceMeters(
      source.latitude,
      source.longitude,
      source.altitude,
      target.latitude,
      target.longitude,
      target.altitude);
}

double horizontalDistanceMeters(const Entity& source, const Entity& target) {
  return domain::distanceMeters(
      source.latitude, source.longitude, target.latitude, target.longitude);
}

double bearingDegrees(const Entity& source, const Entity& target) {
  return domain::bearingDegrees(
      source.latitude, source.longitude, target.latitude, target.longitude);
}

bool isCombatObserver(const Entity& entity) {
  return entity.forceIdentifier == 1 || entity.forceIdentifier == 2;
}

bool isEnemyTarget(const Entity& source, const Entity& target) {
  if (source.forceIdentifier == 1) {
    return target.forceIdentifier == 2;
  }
  if (source.forceIdentifier == 2) {
    return target.forceIdentifier == 1;
  }
  return false;
}

bool targetInsideHorizontalBeam(
    const Entity& source,
    const SensorDefinition& sensor,
    const Entity& target) {
  if (sensor.azimuthWidthDegrees >= 360.0) {
    return true;
  }

  const double targetBearing = bearingDegrees(source, target);
  const double beamCenter = source.headingDegrees + sensor.azimuthCenterDegrees;
  const double deltaDegrees =
      qAbs(normalizeDegrees180(targetBearing - beamCenter));
  return deltaDegrees <= sensor.azimuthWidthDegrees / 2.0;
}

bool targetInsideVerticalBeam(
    const Entity& source,
    const SensorDefinition& sensor,
    const Entity& target) {
  if (sensor.elevationWidthDegrees <= 0.0) {
    // Zero or negative width means the vertical beam is effectively closed.
    return false;
  }
  if (sensor.elevationWidthDegrees >= 180.0) {
    // The elevation angle is relative to the local horizon, so 180 deg covers all.
    return true;
  }

  const double horizontalRange = horizontalDistanceMeters(source, target);
  const double altitudeDelta =
      static_cast<double>(target.altitude - source.altitude);
  // Entity has no pitch/roll, so elevation is interpreted against the local horizon.
  const double targetElevationDegrees =
      qRadiansToDegrees(qAtan2(altitudeDelta, horizontalRange));
  const double deltaDegrees =
      qAbs(normalizeDegrees180(
          targetElevationDegrees - sensor.elevationCenterDegrees));
  return deltaDegrees <= sensor.elevationWidthDegrees / 2.0;
}

const SensorContact* findPreviousContact(
    const SensorContacts& contacts,
    const SensorDefinition& sensor,
    const Entity& target) {
  const QString targetId = domain::entityKey(target);
  for (const SensorContact& contact : contacts) {
    if (contact.sensorId != sensor.id) {
      continue;
    }
    if (!contact.targetEntityId.trimmed().isEmpty()) {
      if (contact.targetEntityId == targetId) {
        return &contact;
      }
      continue;
    }
    if (contact.targetEntityName.compare(target.name, Qt::CaseInsensitive) == 0) {
      return &contact;
    }
  }
  return nullptr;
}

qint64 evaluationIndex(
    const SensorDefinition& sensor,
    double simulationTimeSeconds) {
  const double periodSeconds = std::max(0.001, sensor.updatePeriodSeconds);
  return static_cast<qint64>(
      std::floor(std::max(0.0, simulationTimeSeconds) / periodSeconds));
}

SensorContact makeContact(
    const SensorDefinition& sensor,
    const QString& effectiveModelProviderId,
    const Entity& target,
    double range,
    double bearing) {
  SensorContact contact;
  contact.sensorId = sensor.id;
  contact.sensorModelProviderId = effectiveModelProviderId;
  contact.sensorType = sensor.sensorType;
  contact.sensorSubType = sensor.sensorSubType;
  contact.targetEntityId = domain::entityKey(target);
  contact.targetEntityName = target.name;
  contact.rangeMeters = range;
  contact.bearingDegrees = bearing;
  contact.lineOfSight = true;
  return contact;
}

QString normalizedProviderId(const QString& providerId) {
  const QString normalized = providerId.trimmed().toLower();
  return normalized.isEmpty() ? QStringLiteral("native") : normalized;
}

SensorEvaluationDiagnostics makeEvaluationDiagnostics(
    const SensorDefinition& sensor,
    const application::sensors::ISensorModel& resolvedModel,
    const application::sensors::SensorEvaluationResult& evaluation) {
  SensorEvaluationDiagnostics diagnostics;
  diagnostics.requestedModelProviderId =
      normalizedProviderId(sensor.modelProviderId);
  diagnostics.effectiveModelProviderId = normalizedProviderId(
      evaluation.effectiveModelId.trimmed().isEmpty()
          ? resolvedModel.modelId()
          : evaluation.effectiveModelId);
  diagnostics.providerVersion = evaluation.providerVersion;
  diagnostics.providerCapabilities = evaluation.providerCapabilities.isEmpty()
      ? resolvedModel.capabilities()
      : evaluation.providerCapabilities;
  diagnostics.fallbackUsed = evaluation.fallbackUsed ||
      diagnostics.requestedModelProviderId !=
          diagnostics.effectiveModelProviderId;
  diagnostics.fallbackReason = evaluation.fallbackReason;
  if (diagnostics.fallbackUsed && diagnostics.fallbackReason.trimmed().isEmpty()) {
    diagnostics.fallbackReason = QStringLiteral(
        "Requested provider was unavailable; using %1")
        .arg(diagnostics.effectiveModelProviderId);
  }
  diagnostics.detectionProbability = evaluation.probability;
  diagnostics.deterministicSample = evaluation.sample;
  diagnostics.targetSignature = evaluation.targetSignature;
  diagnostics.signalToNoiseRatio = evaluation.signalToNoiseRatio;
  diagnostics.signalToNoiseRatioDecibels =
      evaluation.signalToNoiseRatioDecibels;
  diagnostics.rangeLossDecibels = evaluation.rangeLossDecibels;
  diagnostics.echoRatio = evaluation.echoRatio;
  diagnostics.receivedPowerWatts = evaluation.receivedPowerWatts;
  diagnostics.noisePowerWatts = evaluation.noisePowerWatts;
  diagnostics.evaluationDurationMilliseconds =
      evaluation.evaluationDurationMilliseconds;
  return diagnostics;
}

SensorRuntimeStatus& runtimeStatusFor(
    Entity& source,
    const SensorDefinition& sensor) {
  for (SensorRuntimeStatus& status : source.sensorRuntimeStatuses) {
    if (status.sensorId == sensor.id) {
      return status;
    }
  }
  SensorRuntimeStatus status;
  status.sensorId = sensor.id;
  source.sensorRuntimeStatuses.push_back(status);
  return source.sensorRuntimeStatuses.last();
}

void recordEvaluation(
    Entity& source,
    const SensorDefinition& sensor,
    const Entity& target,
    double simulationTimeSeconds,
    qint64 scanIndex,
    bool detected,
    const SensorEvaluationDiagnostics& diagnostics) {
  SensorRuntimeStatus& status = runtimeStatusFor(source, sensor);
  status.lastTargetEntityId = domain::entityKey(target);
  status.lastTargetEntityName = target.name;
  status.lastEvaluationSimulationSeconds =
      std::max(0.0, simulationTimeSeconds);
  status.lastEvaluationIndex = scanIndex;
  ++status.evaluationCount;
  if (detected) {
    ++status.detectionCount;
  }
  status.evaluation = diagnostics;
}

} // namespace

void SensorEngine::updateEntityContacts(
    QVector<Entity>& entities,
    double simulationTimeSeconds,
    quint32 scenarioSeed,
    const application::sensors::SensorModelRegistry* modelRegistry) {
  const application::sensors::SensorModelRegistry& models = modelRegistry
      ? *modelRegistry
      : application::sensors::SensorModelRegistry::defaultRegistry();

  for (int sourceIndex = 0; sourceIndex < entities.size(); ++sourceIndex) {
    Entity& source = entities[sourceIndex];
    const SensorContacts previousContacts = source.sensorContacts;
    source.sensorContacts.clear();

    if (source.destroyed) {
      source.sensorRuntimeStatuses.clear();
      continue;
    }
    if (!isCombatObserver(source)) {
      continue;
    }

    for (const SensorDefinition& sensor : source.sensors) {
      if (!sensor.enabled || !sensor.emitting || sensor.maxRangeMeters <= 0.0 ||
          sensor.maxTracks <= 0) {
        continue;
      }
      if (!sensor::canOperateFromDomain(sensor, source.domain)) {
        continue;
      }

      int tracksAdded = 0;
      const qint64 scanIndex = evaluationIndex(sensor, simulationTimeSeconds);
      for (int targetIndex = 0; targetIndex < entities.size(); ++targetIndex) {
        if (sourceIndex == targetIndex) {
          continue;
        }

        const Entity& target = entities[targetIndex];
        if (target.destroyed) {
          continue;
        }
        if (!isEnemyTarget(source, target)) {
          continue;
        }
        if (!sensor::supportsTargetDomain(sensor, target.domain)) {
          continue;
        }

        const double range = distanceMeters(source, target);
        if (range < sensor.minRangeMeters || range > sensor.maxRangeMeters) {
          continue;
        }

        if (!targetInsideHorizontalBeam(source, sensor, target)) {
          continue;
        }

        if (!targetInsideVerticalBeam(source, sensor, target)) {
          continue;
        }

        const double bearing = bearingDegrees(source, target);
        const SensorContact* previous =
            findPreviousContact(previousContacts, sensor, target);

        if (previous && previous->lastEvaluationIndex == scanIndex) {
          SensorContact contact = *previous;
          contact.rangeMeters = range;
          contact.bearingDegrees = bearing;
          source.sensorContacts.push_back(contact);
          ++tracksAdded;
          if (tracksAdded >= sensor.maxTracks) {
            break;
          }
          continue;
        }

        const application::sensors::ISensorModel& sensorModel =
            models.resolve(sensor.modelProviderId);
        QElapsedTimer evaluationTimer;
        evaluationTimer.start();
        application::sensors::SensorEvaluationResult evaluation =
            sensorModel.evaluate({
                scenarioSeed,
                simulationTimeSeconds,
                scanIndex,
                range,
                source,
                sensor,
                target});
        evaluation.evaluationDurationMilliseconds =
            static_cast<double>(evaluationTimer.nsecsElapsed()) / 1000000.0;
        const SensorEvaluationDiagnostics diagnostics =
            makeEvaluationDiagnostics(sensor, sensorModel, evaluation);
        recordEvaluation(
            source,
            sensor,
            target,
            simulationTimeSeconds,
            scanIndex,
            evaluation.detected,
            diagnostics);

        if (evaluation.detected) {
          SensorContact contact = makeContact(
              sensor,
              evaluation.effectiveModelId.trimmed().isEmpty()
                  ? sensorModel.modelId()
                  : evaluation.effectiveModelId,
              target,
              range,
              bearing);
          contact.detected = true;
          contact.confidence = evaluation.probability;
          contact.lastSeenSimulationSeconds =
              std::max(0.0, simulationTimeSeconds);
          contact.trackState = QStringLiteral("Detected");
          contact.lastEvaluationIndex = scanIndex;
          contact.evaluation = diagnostics;
          source.sensorContacts.push_back(contact);
        } else if (previous) {
          const double secondsSinceLastSeen = std::max(
              0.0,
              simulationTimeSeconds - previous->lastSeenSimulationSeconds);
          if (secondsSinceLastSeen > std::max(0.0, sensor.trackHoldSeconds)) {
            continue;
          }

          SensorContact contact = *previous;
          contact.rangeMeters = range;
          contact.bearingDegrees = bearing;
          contact.detected = true;
          contact.confidence = std::clamp(
              previous->confidence * 0.5,
              0.0,
              1.0);
          contact.trackState = QStringLiteral("Coasting");
          contact.lastEvaluationIndex = scanIndex;
          contact.missedDetectionCount = previous->missedDetectionCount + 1;
          contact.evaluation = diagnostics;
          source.sensorContacts.push_back(contact);
        } else {
          continue;
        }

        ++tracksAdded;
        if (tracksAdded >= sensor.maxTracks) {
          break;
        }
      }
    }
  }
}
