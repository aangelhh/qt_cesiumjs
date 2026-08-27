#include <gtest/gtest.h>

#include "application/SensorDetectionModel.h"
#include "application/SensorEngine.h"
#include "application/ScenarioState.h"
#include "application/sensors/NativeSensorModel.h"
#include "application/sensors/SensorModelRegistry.h"

#include <memory>

namespace {

class FixedSensorModel final : public application::sensors::ISensorModel {
public:
  FixedSensorModel(QString id, bool detected)
      : _id(std::move(id)), _detected(detected) {}

  QString modelId() const override {
    return _id;
  }

  application::sensors::SensorEvaluationResult evaluate(
      const application::sensors::SensorEvaluationContext& context) const override {
    ++evaluationCount;
    lastSensorId = context.sensor.id;
    application::sensors::SensorEvaluationResult result;
    result.probability = _detected ? 1.0 : 0.0;
    result.sample = _detected ? 0.0 : 1.0;
    result.detected = _detected;
    result.effectiveModelId = _id;
    result.providerVersion = QStringLiteral("fixed-test-v1");
    result.targetSignature = 1.25;
    result.signalToNoiseRatio = 4.0;
    result.signalToNoiseRatioDecibels = 6.0206;
    return result;
  }

  mutable int evaluationCount = 0;
  mutable QString lastSensorId;

private:
  QString _id;
  bool _detected = false;
};

Entity makeEntity(
    const QString& id,
    int forceIdentifier,
    double longitude) {
  Entity entity;
  entity.entityId = id;
  entity.name = id;
  entity.forceIdentifier = forceIdentifier;
  entity.domain = QStringLiteral("Air");
  entity.latitude = 40.0;
  entity.longitude = longitude;
  entity.altitude = 5000;
  return entity;
}

SensorDefinition makeRadar(const QString& providerId) {
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-primary");
  sensor.name = QStringLiteral("Primary Radar");
  sensor.modelProviderId = providerId;
  sensor.sensorType = QStringLiteral("radar");
  sensor.sensorSubType = QStringLiteral("airborneRadar");
  sensor.maxRangeMeters = 100000.0;
  sensor.azimuthWidthDegrees = 360.0;
  sensor.elevationWidthDegrees = 180.0;
  return sensor;
}

} // namespace

TEST(SensorModelRegistry, ContainsNativeProviderByDefault) {
  const application::sensors::SensorModelRegistry registry;

  EXPECT_EQ(
      registry.resolve(QStringLiteral("native")).modelId(),
      QStringLiteral("native"));
  EXPECT_TRUE(registry.modelIds().contains(QStringLiteral("native")));
}

TEST(SensorModelRegistry, ResolvesRegisteredProviderCaseInsensitively) {
  application::sensors::SensorModelRegistry registry;
  auto provider =
      std::make_shared<FixedSensorModel>(QStringLiteral("Stone-Soup"), true);

  ASSERT_TRUE(registry.registerModel(provider));
  EXPECT_EQ(
      &registry.resolve(QStringLiteral(" stone-soup ")),
      provider.get());
}

TEST(SensorModelRegistry, UnknownProviderFallsBackToNative) {
  const application::sensors::SensorModelRegistry registry;

  EXPECT_EQ(
      registry.resolve(QStringLiteral("not-installed")).modelId(),
      QStringLiteral("native"));
}

TEST(SensorModelRegistry, NativeProviderPreservesDetectionEvaluation) {
  const application::sensors::NativeSensorModel native;
  const Entity observer = makeEntity(QStringLiteral("observer"), 1, 0.0);
  const Entity target = makeEntity(QStringLiteral("target"), 2, 0.1);
  const SensorDefinition sensor = makeRadar(QStringLiteral("native"));
  const auto expected = application::SensorDetectionModel::evaluate(
      42U, observer, sensor, target, 10000.0, 7);
  const auto actual = native.evaluate(
      {42U, 7.0, 7, 10000.0, observer, sensor, target});

  EXPECT_DOUBLE_EQ(actual.probability, expected.probability);
  EXPECT_DOUBLE_EQ(actual.sample, expected.sample);
  EXPECT_EQ(actual.detected, expected.detected);
}

TEST(SensorModelRegistry, SensorEngineUsesProviderSelectedBySensor) {
  application::sensors::SensorModelRegistry registry;
  auto externalProvider =
      std::make_shared<FixedSensorModel>(QStringLiteral("mixr"), false);
  ASSERT_TRUE(registry.registerModel(externalProvider));

  Entity observer = makeEntity(QStringLiteral("observer"), 1, 0.0);
  observer.sensors.push_back(makeRadar(QStringLiteral("mixr")));
  QVector<Entity> entities{
      observer,
      makeEntity(QStringLiteral("target"), 2, 0.1)};

  SensorEngine::updateEntityContacts(entities, 0.0, 42U, &registry);

  EXPECT_EQ(externalProvider->evaluationCount, 1);
  EXPECT_EQ(externalProvider->lastSensorId, QStringLiteral("radar-primary"));
  EXPECT_TRUE(entities.front().sensorContacts.isEmpty());
  ASSERT_EQ(entities.front().sensorRuntimeStatuses.size(), 1);
  const SensorRuntimeStatus& runtime =
      entities.front().sensorRuntimeStatuses.front();
  EXPECT_EQ(runtime.evaluation.effectiveModelProviderId, QStringLiteral("mixr"));
  EXPECT_EQ(runtime.evaluation.providerVersion, QStringLiteral("fixed-test-v1"));
  EXPECT_DOUBLE_EQ(runtime.evaluation.signalToNoiseRatio, 4.0);
  EXPECT_EQ(runtime.evaluationCount, 1U);
  EXPECT_EQ(runtime.detectionCount, 0U);
}

TEST(SensorModelRegistry, UnknownProviderRecordsExplicitNativeFallback) {
  application::sensors::SensorModelRegistry registry;
  Entity observer = makeEntity(QStringLiteral("observer"), 1, 0.0);
  observer.sensors.push_back(makeRadar(QStringLiteral("not-installed")));
  QVector<Entity> entities{
      observer,
      makeEntity(QStringLiteral("target"), 2, 0.1)};

  SensorEngine::updateEntityContacts(entities, 0.0, 42U, &registry);

  ASSERT_EQ(entities.front().sensorRuntimeStatuses.size(), 1);
  const SensorEvaluationDiagnostics& diagnostics =
      entities.front().sensorRuntimeStatuses.front().evaluation;
  EXPECT_EQ(
      diagnostics.requestedModelProviderId,
      QStringLiteral("not-installed"));
  EXPECT_EQ(
      diagnostics.effectiveModelProviderId,
      QStringLiteral("native"));
  EXPECT_TRUE(diagnostics.fallbackUsed);
  EXPECT_FALSE(diagnostics.fallbackReason.isEmpty());
}

TEST(SensorModelRegistry, ScenarioRuntimeAcceptsAdditionalProviders) {
  ScenarioState state;
  auto provider =
      std::make_shared<FixedSensorModel>(QStringLiteral("stone-soup"), true);

  ASSERT_TRUE(state.registerSensorModel(provider));
  EXPECT_TRUE(state.sensorModelIds().contains(QStringLiteral("native")));
  EXPECT_TRUE(state.sensorModelIds().contains(QStringLiteral("stone-soup")));
}
