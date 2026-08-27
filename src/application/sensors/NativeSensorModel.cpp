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
  return result;
}

} // namespace application::sensors
