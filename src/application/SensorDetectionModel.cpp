#include "application/SensorDetectionModel.h"

#include "domain/EntityIdentity.h"

#include <QByteArray>
#include <QtMath>

#include <algorithm>
#include <cmath>

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
