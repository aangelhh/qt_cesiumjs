#include "application/SensorDetectionModel.h"

#include "domain/EntityIdentity.h"

#include <QByteArray>
#include <QtMath>

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

quint64 fnv1a64(const QByteArray& bytes) {
  quint64 hash = 14695981039346656037ULL;
  for (const char byte : bytes) {
    hash ^= static_cast<quint8>(byte);
    hash *= 1099511628211ULL;
  }
  return hash;
}

double normalizedRange(const SensorDefinition& sensor, double rangeMeters) {
  const double span = sensor.maxRangeMeters - sensor.minRangeMeters;
  if (span <= 0.0) {
    return 1.0;
  }
  return std::clamp(
      (rangeMeters - sensor.minRangeMeters) / span,
      0.0,
      1.0);
}

} // namespace

namespace application {

double SensorDetectionModel::targetSignature(
    const SensorDefinition& sensor,
    const Entity& target) {
  switch (sensor::typeFromString(sensor.sensorType)) {
    case SensorType::Radar:
      return std::max(0.0, target.radarSignature);
    case SensorType::Infrared:
      return std::max(0.0, target.thermalSignature);
    case SensorType::Visual:
      return std::max(0.0, target.visualSignature);
    case SensorType::Unknown:
      return 1.0;
  }
  return 1.0;
}

double SensorDetectionModel::detectionProbability(
    const SensorDefinition& sensor,
    const Entity& target,
    double rangeMeters) {
  const double baseProbability =
      std::clamp(sensor.probabilityOfDetection, 0.0, 1.0);
  const double signatureFactor =
      std::clamp(targetSignature(sensor, target), 0.0, 2.0);

  // Default legacy sensors and targets remain deterministic after passing the
  // geometric gates. Configuring either probability or signature opts in to
  // the probabilistic model.
  if (baseProbability >= 1.0 && qFuzzyCompare(signatureFactor, 1.0)) {
    return 1.0;
  }

  const double rangeRatio = normalizedRange(sensor, rangeMeters);
  const double distanceFactor = 1.0 - 0.75 * rangeRatio * rangeRatio;
  return std::clamp(
      baseProbability * distanceFactor * signatureFactor,
      0.0,
      1.0);
}

RadarSignalMetrics SensorDetectionModel::radarSignalMetrics(
    const SensorDefinition& sensor,
    const Entity& target,
    double rangeMeters) {
  constexpr double kLightSpeedMetersPerSecond = 299792458.0;
  constexpr double kBoltzmannConstant = 1.380649e-23;
  constexpr double kReferenceTemperatureKelvin = 290.0;
  constexpr double kPi = 3.14159265358979323846;

  const RadarProfile& profile = sensor.radarProfile;
  const double frequencyHertz = std::max(1.0, profile.frequencyHertz);
  const double bandwidthHertz = std::max(1.0, profile.bandwidthHertz);
  const double range = std::max(1.0, rangeMeters);
  const double wavelength = kLightSpeedMetersPerSecond / frequencyHertz;
  const double gain = std::pow(10.0, profile.antennaGainDecibels / 10.0);
  const double noiseFactor =
      std::pow(10.0, std::max(0.0, profile.receiverNoiseDecibels) / 10.0);
  const double loss =
      std::pow(10.0, std::max(0.0, profile.systemLossDecibels) / 10.0);
  const double rcs = targetSignature(sensor, target) *
      std::max(0.01, profile.rcsScaleSquareMeters);
  const double numerator =
      std::max(0.0, profile.peakPowerWatts) *
      std::clamp(profile.dutyCycle, 0.0, 1.0) * gain * gain *
      wavelength * wavelength * std::max(0.0, rcs) *
      static_cast<double>(std::max(1, profile.numberPulses));
  const double denominator =
      std::pow(4.0 * kPi, 3.0) * std::pow(range, 4.0) * loss;

  RadarSignalMetrics metrics;
  metrics.receivedPowerWatts = denominator > 0.0
      ? numerator / denominator
      : 0.0;
  metrics.noisePowerWatts =
      kBoltzmannConstant * kReferenceTemperatureKelvin * bandwidthHertz *
      noiseFactor;
  if (metrics.noisePowerWatts > 0.0) {
    metrics.signalToNoiseRatio =
        metrics.receivedPowerWatts / metrics.noisePowerWatts;
  }
  metrics.signalToNoiseRatioDecibels = metrics.signalToNoiseRatio > 0.0
      ? 10.0 * std::log10(metrics.signalToNoiseRatio)
      : -std::numeric_limits<double>::infinity();
  return metrics;
}

double SensorDetectionModel::deterministicSample(
    quint32 scenarioSeed,
    const QString& observerEntityId,
    const QString& sensorId,
    const QString& targetEntityId,
    qint64 evaluationIndex) {
  QByteArray key;
  key.reserve(160);
  key.append(QByteArray::number(scenarioSeed));
  key.append('|');
  key.append(observerEntityId.toUtf8());
  key.append('|');
  key.append(sensorId.toUtf8());
  key.append('|');
  key.append(targetEntityId.toUtf8());
  key.append('|');
  key.append(QByteArray::number(evaluationIndex));

  const quint64 hash = fnv1a64(key);
  constexpr double kTwoTo53 = 9007199254740992.0;
  return static_cast<double>(hash >> 11) / kTwoTo53;
}

sensors::SensorEvaluationResult SensorDetectionModel::evaluate(
    quint32 scenarioSeed,
    const Entity& observer,
    const SensorDefinition& sensor,
    const Entity& target,
    double rangeMeters,
    qint64 evaluationIndex) {
  sensors::SensorEvaluationResult evaluation;
  evaluation.probability = detectionProbability(sensor, target, rangeMeters);
  evaluation.sample = deterministicSample(
      scenarioSeed,
      domain::entityKey(observer),
      sensor.id.trimmed().isEmpty() ? sensor.name : sensor.id,
      domain::entityKey(target),
      evaluationIndex);
  evaluation.detected = evaluation.sample < evaluation.probability;
  return evaluation;
}

} // namespace application
