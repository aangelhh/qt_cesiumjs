#include "application/sensors/NativeSensorModel.h"

#include "application/SensorDetectionModel.h"

namespace application::sensors {

QString NativeSensorModel::staticModelId() {
  return QStringLiteral("native");
}

QString NativeSensorModel::modelId() const {
  return staticModelId();
}

SensorEvaluationResult NativeSensorModel::evaluate(
    const SensorEvaluationContext& context) const {
  SensorEvaluationResult result = application::SensorDetectionModel::evaluate(
      context.scenarioSeed,
      context.observer,
      context.sensor,
      context.target,
      context.rangeMeters,
      context.evaluationIndex);
  result.effectiveModelId = staticModelId();
  result.providerVersion = QStringLiteral("native-v1");
  result.targetSignature = application::SensorDetectionModel::targetSignature(
      context.sensor,
      context.target);
  if (sensor::typeFromString(context.sensor.sensorType) == SensorType::Radar) {
    const application::RadarSignalMetrics metrics =
        application::SensorDetectionModel::radarSignalMetrics(
            context.sensor,
            context.target,
            context.rangeMeters);
    result.signalToNoiseRatio = metrics.signalToNoiseRatio;
    result.signalToNoiseRatioDecibels =
        metrics.signalToNoiseRatioDecibels;
    result.receivedPowerWatts = metrics.receivedPowerWatts;
    result.noisePowerWatts = metrics.noisePowerWatts;
  }
  return result;
}

} // namespace application::sensors
