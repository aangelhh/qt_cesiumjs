#include "infrastructure/sensors/SharedLibrarySensorModel.h"

#include "application/SensorDetectionModel.h"
#include "application/sensors/NativeSensorModel.h"
#include "domain/EntityIdentity.h"

#include <algorithm>

namespace infrastructure::sensors {

SharedLibrarySensorModel::SharedLibrarySensorModel(
    QString configuredModelId,
    QString libraryPath)
    : _modelId(std::move(configuredModelId)),
      _libraryPath(std::move(libraryPath)),
      _library(_libraryPath) {
  if (_modelId.trimmed().isEmpty() || _libraryPath.trimmed().isEmpty()) {
    _errorString = QStringLiteral("Provider id or library path is empty");
    return;
  }
  if (!_library.load()) {
    _errorString = _library.errorString();
    return;
  }

  const auto abiVersion = reinterpret_cast<QttestSensorPluginAbiVersionFn>(
      _library.resolve("qttest_sensor_model_abi_version"));
  const auto pluginModelId = reinterpret_cast<QttestSensorPluginModelIdFn>(
      _library.resolve("qttest_sensor_model_id"));
  _evaluate = reinterpret_cast<QttestSensorPluginEvaluateFn>(
      _library.resolve("qttest_sensor_model_evaluate"));
  if (!abiVersion || !pluginModelId || !_evaluate) {
    _errorString = QStringLiteral("Required qttest sensor plugin symbols are missing");
    _library.unload();
    _evaluate = nullptr;
    return;
  }
  if (abiVersion() != QTTEST_SENSOR_MODEL_PLUGIN_ABI_VERSION) {
    _errorString = QStringLiteral("Unsupported sensor plugin ABI version %1")
        .arg(abiVersion());
    _library.unload();
    _evaluate = nullptr;
    return;
  }
  if (!pluginModelId() || QString::fromUtf8(pluginModelId()).trimmed().isEmpty()) {
    _errorString = QStringLiteral("Sensor plugin returned an empty model id");
    _library.unload();
    _evaluate = nullptr;
  }
}

QString SharedLibrarySensorModel::modelId() const {
  return _modelId.trimmed().toLower();
}

application::sensors::SensorEvaluationResult SharedLibrarySensorModel::evaluate(
    const application::sensors::SensorEvaluationContext& context) const {
  if (!_evaluate) {
    application::sensors::NativeSensorModel fallback;
    return fallback.evaluate(context);
  }

  QttestSensorEvaluationInputV1 input{};
  input.structSize = sizeof(input);
  input.scenarioSeed = context.scenarioSeed;
  input.evaluationIndex = context.evaluationIndex;
  input.simulationTimeSeconds = context.simulationTimeSeconds;
  input.rangeMeters = context.rangeMeters;
  input.configuredProbability = context.sensor.probabilityOfDetection;
  input.targetSignature = application::SensorDetectionModel::targetSignature(
      context.sensor,
      context.target);
  input.observerLatitudeDegrees = context.observer.latitude;
  input.observerLongitudeDegrees = context.observer.longitude;
  input.observerAltitudeMeters = context.observer.altitude;
  input.observerHeadingDegrees = context.observer.headingDegrees;
  input.observerSpeedKnots = context.observer.speedKnots;
  input.targetLatitudeDegrees = context.target.latitude;
  input.targetLongitudeDegrees = context.target.longitude;
  input.targetAltitudeMeters = context.target.altitude;
  input.targetHeadingDegrees = context.target.headingDegrees;
  input.targetSpeedKnots = context.target.speedKnots;
  input.sensorMinRangeMeters = context.sensor.minRangeMeters;
  input.sensorMaxRangeMeters = context.sensor.maxRangeMeters;

  QttestSensorEvaluationOutputV1 output{};
  output.structSize = sizeof(output);
  if (_evaluate(&input, &output) != 0 || output.status != 0) {
    application::sensors::NativeSensorModel fallback;
    return fallback.evaluate(context);
  }

  application::sensors::SensorEvaluationResult result;
  result.probability = std::clamp(output.probability, 0.0, 1.0);
  result.sample = application::SensorDetectionModel::deterministicSample(
      context.scenarioSeed,
      domain::entityKey(context.observer),
      context.sensor.id.trimmed().isEmpty()
          ? context.sensor.name
          : context.sensor.id,
      domain::entityKey(context.target),
      context.evaluationIndex);
  result.detected = result.sample < result.probability;
  result.effectiveModelId = this->modelId();
  return result;
}

bool SharedLibrarySensorModel::isAvailable() const {
  return _library.isLoaded() && _evaluate;
}

QString SharedLibrarySensorModel::errorString() const {
  return _errorString;
}

} // namespace infrastructure::sensors
