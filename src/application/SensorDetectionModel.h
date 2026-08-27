#pragma once

#include "application/sensors/ISensorModel.h"
#include "domain/Entity.h"

#include <QtGlobal>

namespace application {

struct RadarSignalMetrics {
  double receivedPowerWatts = 0.0;
  double noisePowerWatts = 0.0;
  double signalToNoiseRatio = 0.0;
  double signalToNoiseRatioDecibels = 0.0;
};

class SensorDetectionModel {
public:
  static double targetSignature(
      const SensorDefinition& sensor,
      const Entity& target);

  static double detectionProbability(
      const SensorDefinition& sensor,
      const Entity& target,
      double rangeMeters);

  static RadarSignalMetrics radarSignalMetrics(
      const SensorDefinition& sensor,
      const Entity& target,
      double rangeMeters);

  static double deterministicSample(
      quint32 scenarioSeed,
      const QString& observerEntityId,
      const QString& sensorId,
      const QString& targetEntityId,
      qint64 evaluationIndex);

  static sensors::SensorEvaluationResult evaluate(
      quint32 scenarioSeed,
      const Entity& observer,
      const SensorDefinition& sensor,
      const Entity& target,
      double rangeMeters,
      qint64 evaluationIndex);
};

} // namespace application
