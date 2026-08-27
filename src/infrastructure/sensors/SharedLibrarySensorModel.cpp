#include "infrastructure/sensors/SharedLibrarySensorModel.h"

#include "application/SensorDetectionModel.h"
#include "application/sensors/NativeSensorModel.h"
#include "domain/EntityIdentity.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

application::sensors::SensorEvaluationResult nativeFallback(
    const application::sensors::SensorEvaluationContext& context,
    const QString& reason) {
  application::sensors::NativeSensorModel fallback;
  application::sensors::SensorEvaluationResult result = fallback.evaluate(context);
  result.fallbackUsed = true;
  result.fallbackReason = reason;
  return result;
}

} // namespace

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
  const auto pluginVersion = reinterpret_cast<QttestSensorPluginVersionFn>(
      _library.resolve("qttest_sensor_model_version"));
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
    return;
  }
  _providerVersion = pluginVersion && pluginVersion()
      ? QString::fromUtf8(pluginVersion()).trimmed()
      : QStringLiteral("ABI v1");
}

QString SharedLibrarySensorModel::modelId() const {
  return _modelId.trimmed().toLower();
}

application::sensors::SensorEvaluationResult SharedLibrarySensorModel::evaluate(
    const application::sensors::SensorEvaluationContext& context) const {
  if (!_evaluate) {
    return nativeFallback(
        context,
        _errorString.trimmed().isEmpty()
            ? QStringLiteral("Shared-library provider is unavailable")
            : _errorString);
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
  const RadarProfile& profile = context.sensor.radarProfile;
  input.radarPeakPowerWatts = profile.peakPowerWatts;
  input.radarDutyCycle = profile.dutyCycle;
  input.radarBandwidthHertz = profile.bandwidthHertz;
  input.radarReceiverNoiseDecibels = profile.receiverNoiseDecibels;
  input.radarFrequencyHertz = profile.frequencyHertz;
  input.radarAntennaGainDecibels = profile.antennaGainDecibels;
  input.radarBeamWidthDegrees = profile.beamWidthDegrees;
  input.radarNumberPulses = static_cast<uint32_t>(profile.numberPulses);
  input.radarSystemLossDecibels = profile.systemLossDecibels;
  input.radarProbabilityFalseAlarm = profile.probabilityFalseAlarm;
  input.radarRcsScaleSquareMeters = profile.rcsScaleSquareMeters;

  QttestSensorEvaluationOutputV1 output{};
  output.structSize = sizeof(output);
  output.signalToNoiseRatio = std::numeric_limits<double>::quiet_NaN();
  output.signalToNoiseRatioDecibels =
      std::numeric_limits<double>::quiet_NaN();
  output.rangeLossDecibels = std::numeric_limits<double>::quiet_NaN();
  output.echoRatio = std::numeric_limits<double>::quiet_NaN();
  output.receivedPowerWatts = std::numeric_limits<double>::quiet_NaN();
  output.noisePowerWatts = std::numeric_limits<double>::quiet_NaN();
  if (_evaluate(&input, &output) != 0 || output.status != 0) {
    return nativeFallback(
        context,
        QStringLiteral("Shared-library provider evaluation failed"));
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
  result.providerVersion = _providerVersion;
  result.targetSignature = input.targetSignature;
  if (output.diagnosticsMask & QTTEST_SENSOR_DIAGNOSTIC_SNR) {
    result.signalToNoiseRatio = output.signalToNoiseRatio;
  }
  if (output.diagnosticsMask & QTTEST_SENSOR_DIAGNOSTIC_SNR_DB) {
    result.signalToNoiseRatioDecibels = output.signalToNoiseRatioDecibels;
  }
  if (output.diagnosticsMask & QTTEST_SENSOR_DIAGNOSTIC_RANGE_LOSS_DB) {
    result.rangeLossDecibels = output.rangeLossDecibels;
  }
  if (output.diagnosticsMask & QTTEST_SENSOR_DIAGNOSTIC_ECHO_RATIO) {
    result.echoRatio = output.echoRatio;
  }
  if (output.diagnosticsMask & QTTEST_SENSOR_DIAGNOSTIC_RECEIVED_POWER_W) {
    result.receivedPowerWatts = output.receivedPowerWatts;
  }
  if (output.diagnosticsMask & QTTEST_SENSOR_DIAGNOSTIC_NOISE_POWER_W) {
    result.noisePowerWatts = output.noisePowerWatts;
  }
  return result;
}

bool SharedLibrarySensorModel::isAvailable() const {
  return _library.isLoaded() && _evaluate;
}

QString SharedLibrarySensorModel::errorString() const {
  return _errorString;
}

} // namespace infrastructure::sensors
