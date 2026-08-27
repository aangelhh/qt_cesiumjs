#include <gtest/gtest.h>

#include "application/SensorDetectionModel.h"
#include "application/SensorEngine.h"

namespace {

Entity makeObserver() {
  Entity entity;
  entity.entityId = QStringLiteral("observer-id");
  entity.name = QStringLiteral("Observer");
  entity.forceIdentifier = 1;
  entity.domain = QStringLiteral("Air");
  entity.latitude = 40.0;
  entity.longitude = 0.0;
  entity.altitude = 5000;
  return entity;
}

Entity makeTarget() {
  Entity entity;
  entity.entityId = QStringLiteral("target-id");
  entity.name = QStringLiteral("Target");
  entity.forceIdentifier = 2;
  entity.domain = QStringLiteral("Air");
  entity.latitude = 40.0;
  entity.longitude = 0.1;
  entity.altitude = 5000;
  return entity;
}

SensorDefinition makeProbabilisticRadar() {
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-1");
  sensor.name = QStringLiteral("Primary Radar");
  sensor.sensorType = QStringLiteral("radar");
  sensor.sensorSubType = QStringLiteral("airborneRadar");
  sensor.maxRangeMeters = 100000.0;
  sensor.azimuthWidthDegrees = 360.0;
  sensor.elevationWidthDegrees = 180.0;
  sensor.updatePeriodSeconds = 1.0;
  sensor.probabilityOfDetection = 0.5;
  sensor.trackHoldSeconds = 1.5;
  return sensor;
}

qint64 findDetectionThenTwoMisses(
    const Entity& observer,
    const SensorDefinition& sensor,
    const Entity& target,
    quint32 seed) {
  for (qint64 scanIndex = 0; scanIndex < 100000; ++scanIndex) {
    const auto first = application::SensorDetectionModel::evaluate(
        seed, observer, sensor, target, 10000.0, scanIndex);
    const auto second = application::SensorDetectionModel::evaluate(
        seed, observer, sensor, target, 10000.0, scanIndex + 1);
    const auto third = application::SensorDetectionModel::evaluate(
        seed, observer, sensor, target, 10000.0, scanIndex + 2);
    if (first.detected && !second.detected && !third.detected) {
      return scanIndex;
    }
  }
  return -1;
}

} // namespace

TEST(SensorDetectionModel, ProbabilityFallsWithRangeAndTargetSignature) {
  SensorDefinition sensor = makeProbabilisticRadar();
  sensor.probabilityOfDetection = 0.8;
  Entity target = makeTarget();

  const double nearProbability =
      application::SensorDetectionModel::detectionProbability(
          sensor, target, 1000.0);
  const double farProbability =
      application::SensorDetectionModel::detectionProbability(
          sensor, target, 90000.0);

  target.radarSignature = 0.25;
  const double lowSignatureProbability =
      application::SensorDetectionModel::detectionProbability(
          sensor, target, 1000.0);

  EXPECT_GT(nearProbability, farProbability);
  EXPECT_GT(nearProbability, lowSignatureProbability);
  EXPECT_GE(lowSignatureProbability, 0.0);
  EXPECT_LE(nearProbability, 1.0);
}

TEST(SensorDetectionModel, SamplingIsStableForSameScenarioInputs) {
  const double first = application::SensorDetectionModel::deterministicSample(
      1234U,
      QStringLiteral("observer"),
      QStringLiteral("radar"),
      QStringLiteral("target"),
      17);
  const double repeated = application::SensorDetectionModel::deterministicSample(
      1234U,
      QStringLiteral("observer"),
      QStringLiteral("radar"),
      QStringLiteral("target"),
      17);
  const double differentSeed = application::SensorDetectionModel::deterministicSample(
      4321U,
      QStringLiteral("observer"),
      QStringLiteral("radar"),
      QStringLiteral("target"),
      17);

  EXPECT_DOUBLE_EQ(first, repeated);
  EXPECT_NE(first, differentSeed);
  EXPECT_GE(first, 0.0);
  EXPECT_LT(first, 1.0);
}

TEST(SensorDetectionModel, SamplesFollowConfiguredProbabilityWithinTolerance) {
  SensorDefinition sensor = makeProbabilisticRadar();
  sensor.probabilityOfDetection = 0.4;
  Entity observer = makeObserver();
  Entity target = makeTarget();

  int detections = 0;
  constexpr int kTrials = 1000;
  for (quint32 seed = 1; seed <= kTrials; ++seed) {
    const auto evaluation = application::SensorDetectionModel::evaluate(
        seed, observer, sensor, target, 0.0, 0);
    detections += evaluation.detected ? 1 : 0;
  }

  EXPECT_GT(detections, 340);
  EXPECT_LT(detections, 460);
}

TEST(SensorDetectionModel, ContactCoastsThenExpiresAfterConsecutiveMisses) {
  Entity observer = makeObserver();
  const SensorDefinition sensor = makeProbabilisticRadar();
  observer.sensors.push_back(sensor);
  const Entity target = makeTarget();
  constexpr quint32 seed = 7788U;
  const qint64 firstScan =
      findDetectionThenTwoMisses(observer, sensor, target, seed);
  ASSERT_GE(firstScan, 0);
  const double startTime = static_cast<double>(firstScan);

  QVector<Entity> entities{observer, target};
  SensorEngine::updateEntityContacts(entities, startTime, seed);
  ASSERT_EQ(entities.front().sensorContacts.size(), 1);
  EXPECT_EQ(
      entities.front().sensorContacts.front().trackState,
      QStringLiteral("Detected"));
  const double initialConfidence =
      entities.front().sensorContacts.front().confidence;

  SensorEngine::updateEntityContacts(entities, startTime + 0.5, seed);
  ASSERT_EQ(entities.front().sensorContacts.size(), 1);
  EXPECT_EQ(
      entities.front().sensorContacts.front().trackState,
      QStringLiteral("Detected"));
  EXPECT_DOUBLE_EQ(
      entities.front().sensorContacts.front().confidence,
      initialConfidence);

  SensorEngine::updateEntityContacts(entities, startTime + 1.0, seed);
  ASSERT_EQ(entities.front().sensorContacts.size(), 1);
  EXPECT_EQ(
      entities.front().sensorContacts.front().trackState,
      QStringLiteral("Coasting"));
  EXPECT_LT(
      entities.front().sensorContacts.front().confidence,
      initialConfidence);
  EXPECT_EQ(
      entities.front().sensorContacts.front().missedDetectionCount,
      1);

  SensorEngine::updateEntityContacts(entities, startTime + 2.0, seed);
  EXPECT_TRUE(entities.front().sensorContacts.isEmpty());
}

TEST(SensorDetectionModel, SameSeedProducesSameContactSequence) {
  Entity observer = makeObserver();
  observer.sensors.push_back(makeProbabilisticRadar());
  const Entity target = makeTarget();
  QVector<Entity> firstRun{observer, target};
  QVector<Entity> secondRun{observer, target};

  for (int second = 0; second < 20; ++second) {
    SensorEngine::updateEntityContacts(firstRun, second, 7788U);
    SensorEngine::updateEntityContacts(secondRun, second, 7788U);
    ASSERT_EQ(
        firstRun.front().sensorContacts.size(),
        secondRun.front().sensorContacts.size());
    if (firstRun.front().sensorContacts.isEmpty()) {
      continue;
    }
    const SensorContact& first = firstRun.front().sensorContacts.front();
    const SensorContact& secondContact = secondRun.front().sensorContacts.front();
    EXPECT_EQ(first.trackState, secondContact.trackState);
    EXPECT_DOUBLE_EQ(first.confidence, secondContact.confidence);
    EXPECT_EQ(first.missedDetectionCount, secondContact.missedDetectionCount);
  }
}
