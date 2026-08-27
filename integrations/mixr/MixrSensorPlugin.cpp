#include "infrastructure/sensors/SensorModelPluginApi.h"

#include "mixr/models/RfEmission.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace {

constexpr double kDefaultReferenceRangeMeters = 100000.0;
constexpr double kHalfProbabilityScale = 0.6931471805599453;
constexpr double kBoltzmannConstant = 1.380649e-23;
constexpr double kReferenceTemperatureKelvin = 290.0;

#ifndef QTTEST_MIXR_PROVIDER_VERSION
#define QTTEST_MIXR_PROVIDER_VERSION "unknown"
#endif

bool hasRangeExtension(const QttestSensorEvaluationInputV1& input) {
  return input.structSize >=
      offsetof(QttestSensorEvaluationInputV1, sensorMaxRangeMeters) +
          sizeof(input.sensorMaxRangeMeters);
}

bool hasRadarProfileExtension(const QttestSensorEvaluationInputV1& input) {
  return input.structSize >=
      offsetof(QttestSensorEvaluationInputV1, radarRcsScaleSquareMeters) +
          sizeof(input.radarRcsScaleSquareMeters);
}

double finiteNonNegative(double value) {
  return std::isfinite(value) ? std::max(0.0, value) : 0.0;
}

struct MixrEvaluationMetrics {
  double echoRatio = 0.0;
  double rangeLossDecibels = 0.0;
  double receivedPowerWatts = 0.0;
  double noisePowerWatts = 0.0;
  double signalToNoiseRatio = 0.0;
  double profilePerformanceScale = 1.0;
};

double linearDecibels(double decibels) {
  return std::pow(10.0, decibels / 10.0);
}

double profilePerformance(
    double peakPowerWatts,
    double dutyCycle,
    double antennaGainDecibels,
    unsigned int pulses,
    double bandwidthHertz,
    double receiverNoiseDecibels,
    double systemLossDecibels) {
  const double gain = linearDecibels(antennaGainDecibels);
  const double noiseFactor = linearDecibels(receiverNoiseDecibels);
  const double loss = linearDecibels(systemLossDecibels);
  return finiteNonNegative(peakPowerWatts) *
      std::clamp(dutyCycle, 0.0, 1.0) * gain * gain *
      static_cast<double>(std::max(1U, pulses)) /
      (std::max(1.0, bandwidthHertz) * noiseFactor * loss);
}

MixrEvaluationMetrics mixrMetrics(
    double rangeMeters,
    double referenceRangeMeters,
    double radarCrossSectionSquareMeters,
    const QttestSensorEvaluationInputV1& input) {
  const bool hasProfile = hasRadarProfileExtension(input);
  const double peakPowerWatts = hasProfile
      ? finiteNonNegative(input.radarPeakPowerWatts)
      : 25000.0;
  const double dutyCycle = hasProfile
      ? std::clamp(input.radarDutyCycle, 0.0, 1.0)
      : 0.1;
  const double bandwidthHertz = hasProfile
      ? std::max(1.0, input.radarBandwidthHertz)
      : 1.0e6;
  const double receiverNoiseDecibels = hasProfile
      ? finiteNonNegative(input.radarReceiverNoiseDecibels)
      : 3.0;
  const double frequencyHertz = hasProfile
      ? std::max(1.0, input.radarFrequencyHertz)
      : 10.0e9;
  const double antennaGainDecibels = hasProfile
      ? input.radarAntennaGainDecibels
      : 35.0;
  const unsigned int pulses = hasProfile
      ? std::max(1U, input.radarNumberPulses)
      : 16U;
  const double systemLossDecibels = hasProfile
      ? finiteNonNegative(input.radarSystemLossDecibels)
      : 6.0;
  const double rcsScale = hasProfile
      ? std::max(0.01, input.radarRcsScaleSquareMeters)
      : 1.0;

  mixr::models::RfEmission observedEmission;
  observedEmission.setRange(std::max(1.0, rangeMeters));
  observedEmission.setRCS(
      finiteNonNegative(radarCrossSectionSquareMeters) * rcsScale);
  observedEmission.setPower(peakPowerWatts);
  observedEmission.setFrequency(frequencyHertz);
  observedEmission.setBandwidth(bandwidthHertz);
  observedEmission.setPulses(pulses);
  observedEmission.setGain(linearDecibels(antennaGainDecibels));
  observedEmission.setTransmitLoss(linearDecibels(systemLossDecibels));

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
  const double gain = observedEmission.getGain();
  const double systemLoss = std::max(1.0, observedEmission.getTransmitLoss());
  metrics.receivedPowerWatts =
      peakPowerWatts * dutyCycle * gain * gain * returnedEcho *
      static_cast<double>(pulses) / systemLoss;
  metrics.noisePowerWatts =
      kBoltzmannConstant * kReferenceTemperatureKelvin * bandwidthHertz *
      linearDecibels(receiverNoiseDecibels);
  if (metrics.noisePowerWatts > 0.0) {
    metrics.signalToNoiseRatio =
        metrics.receivedPowerWatts / metrics.noisePowerWatts;
  }
  const double defaultPerformance = profilePerformance(
      25000.0, 0.1, 35.0, 16U, 1.0e6, 3.0, 6.0);
  const double selectedPerformance = profilePerformance(
      peakPowerWatts,
      dutyCycle,
      antennaGainDecibels,
      pulses,
      bandwidthHertz,
      receiverNoiseDecibels,
      systemLossDecibels);
  metrics.profilePerformanceScale = defaultPerformance > 0.0
      ? selectedPerformance / defaultPerformance
      : 1.0;
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
      input->targetSignature,
      *input);
  const double radarResponse =
      1.0 - std::exp(
          -kHalfProbabilityScale * metrics.echoRatio *
          metrics.profilePerformanceScale);

  output->status = 0;
  output->probability = std::clamp(
      finiteNonNegative(input->configuredProbability) * radarResponse,
      0.0,
      1.0);
  if (output->structSize >= sizeof(QttestSensorEvaluationOutputV1)) {
    output->diagnosticsMask =
        QTTEST_SENSOR_DIAGNOSTIC_SNR |
        QTTEST_SENSOR_DIAGNOSTIC_SNR_DB |
        QTTEST_SENSOR_DIAGNOSTIC_RANGE_LOSS_DB |
        QTTEST_SENSOR_DIAGNOSTIC_ECHO_RATIO |
        QTTEST_SENSOR_DIAGNOSTIC_RECEIVED_POWER_W |
        QTTEST_SENSOR_DIAGNOSTIC_NOISE_POWER_W;
    output->signalToNoiseRatio = metrics.signalToNoiseRatio;
    output->signalToNoiseRatioDecibels = metrics.signalToNoiseRatio > 0.0
        ? 10.0 * std::log10(metrics.signalToNoiseRatio)
        : -std::numeric_limits<double>::infinity();
    output->rangeLossDecibels = metrics.rangeLossDecibels;
    output->echoRatio = metrics.echoRatio;
    output->receivedPowerWatts = metrics.receivedPowerWatts;
    output->noisePowerWatts = metrics.noisePowerWatts;
  }
  return 0;
}
