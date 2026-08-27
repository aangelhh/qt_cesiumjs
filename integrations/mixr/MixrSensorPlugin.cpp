#include "infrastructure/sensors/SensorModelPluginApi.h"

#include "mixr/models/RfEmission.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace {

constexpr double kDefaultReferenceRangeMeters = 100000.0;
constexpr double kHalfProbabilityScale = 0.6931471805599453;

#ifndef QTTEST_MIXR_PROVIDER_VERSION
#define QTTEST_MIXR_PROVIDER_VERSION "unknown"
#endif

bool hasRangeExtension(const QttestSensorEvaluationInputV1& input) {
  return input.structSize >=
      offsetof(QttestSensorEvaluationInputV1, sensorMaxRangeMeters) +
          sizeof(input.sensorMaxRangeMeters);
}

double finiteNonNegative(double value) {
  return std::isfinite(value) ? std::max(0.0, value) : 0.0;
}

struct MixrEvaluationMetrics {
  double echoRatio = 0.0;
  double rangeLossDecibels = 0.0;
};

MixrEvaluationMetrics mixrMetrics(
    double rangeMeters,
    double referenceRangeMeters,
    double radarCrossSectionSquareMeters) {
  mixr::models::RfEmission observedEmission;
  observedEmission.setRange(std::max(1.0, rangeMeters));
  observedEmission.setRCS(finiteNonNegative(radarCrossSectionSquareMeters));

  mixr::models::RfEmission referenceEmission;
  referenceEmission.setRange(std::max(1.0, referenceRangeMeters));

  // MIXR's monostatic radar equation applies its one-way spherical range loss
  // once during transmission and once to the target return (IRadar.cpp).
  const double observedLoss = observedEmission.getRangeLoss();
  const double referenceLoss = referenceEmission.getRangeLoss();
  const double returnedEcho =
      observedEmission.getRCS() * observedLoss * observedLoss;
  const double referenceEcho = referenceLoss * referenceLoss;
  MixrEvaluationMetrics metrics;
  metrics.echoRatio =
      referenceEcho > 0.0 ? returnedEcho / referenceEcho : 0.0;
  metrics.rangeLossDecibels = observedLoss > 0.0
      ? -10.0 * std::log10(observedLoss)
      : 0.0;
  return metrics;
}

} // namespace

extern "C" std::uint32_t qttest_sensor_model_abi_version() {
  return QTTEST_SENSOR_MODEL_PLUGIN_ABI_VERSION;
}

extern "C" const char* qttest_sensor_model_id() {
  return "mixr";
}

extern "C" const char* qttest_sensor_model_version() {
  return QTTEST_MIXR_PROVIDER_VERSION;
}

extern "C" int qttest_sensor_model_evaluate(
    const QttestSensorEvaluationInputV1* input,
    QttestSensorEvaluationOutputV1* output) {
  if (!input || !output ||
      input->structSize < offsetof(QttestSensorEvaluationInputV1, targetSpeedKnots) +
              sizeof(input->targetSpeedKnots) ||
      output->structSize <
          offsetof(QttestSensorEvaluationOutputV1, probability) +
              sizeof(output->probability)) {
    return 1;
  }

  const double referenceRangeMeters =
      hasRangeExtension(*input) && std::isfinite(input->sensorMaxRangeMeters) &&
              input->sensorMaxRangeMeters > 1.0
          ? input->sensorMaxRangeMeters
          : kDefaultReferenceRangeMeters;
  const MixrEvaluationMetrics metrics = mixrMetrics(
      finiteNonNegative(input->rangeMeters),
      referenceRangeMeters,
      input->targetSignature);
  const double radarResponse =
      1.0 - std::exp(-kHalfProbabilityScale * metrics.echoRatio);

  output->status = 0;
  output->probability = std::clamp(
      finiteNonNegative(input->configuredProbability) * radarResponse,
      0.0,
      1.0);
  if (output->structSize >= sizeof(QttestSensorEvaluationOutputV1)) {
    output->diagnosticsMask =
        QTTEST_SENSOR_DIAGNOSTIC_RANGE_LOSS_DB |
        QTTEST_SENSOR_DIAGNOSTIC_ECHO_RATIO;
    output->rangeLossDecibels = metrics.rangeLossDecibels;
    output->echoRatio = metrics.echoRatio;
  }
  return 0;
}
