#include <gtest/gtest.h>

#include "application/SystemsTelemetry.h"

TEST(SystemsTelemetry, ResolvesSingleAndTwinEngineProfilesFromPlatform) {
  Entity f16;
  f16.domain = QStringLiteral("Air");
  f16.category = QStringLiteral("Fighter");
  f16.modelName = QStringLiteral("F-16C");
  EXPECT_EQ(application::engineCountForEntity(f16), 1);
  EXPECT_EQ(
      application::systemsDisplayProfileForEntity(f16),
      QStringLiteral("air-turbine-1-engine"));

  Entity typhoon = f16;
  typhoon.modelName = QStringLiteral("Typhoon FGR4");
  EXPECT_EQ(application::engineCountForEntity(typhoon), 2);
  EXPECT_EQ(
      application::systemsDisplayProfileForEntity(typhoon),
      QStringLiteral("air-turbine-2-engine"));
}

TEST(SystemsTelemetry, ExplicitPlatformAssignmentOverridesInference) {
  Entity entity;
  entity.domain = QStringLiteral("Air");
  entity.modelName = QStringLiteral("Custom Test Platform");
  entity.systemsDisplayProfileId = QStringLiteral("test-four-engine");
  entity.engineCount = 4;

  EXPECT_EQ(application::engineCountForEntity(entity), 4);
  EXPECT_EQ(
      application::systemsDisplayProfileForEntity(entity),
      QStringLiteral("test-four-engine"));
}

TEST(SystemsTelemetry, KinematicEstimateIsDynamicAndClearlyMarked) {
  Entity entity;
  entity.name = QStringLiteral("Viper 1");
  entity.domain = QStringLiteral("Air");
  entity.modelName = QStringLiteral("F-16C");
  entity.speedKnots = 450.0;

  const auto snapshot =
      application::makeEstimatedSystemsTelemetrySnapshot(entity, 900.0);
  ASSERT_EQ(snapshot.engines.size(), 1);
  EXPECT_EQ(snapshot.dataSource, QStringLiteral("Kinematic estimate"));
  EXPECT_TRUE(snapshot.engines.front().estimated);
  EXPECT_EQ(snapshot.engines.front().state, QStringLiteral("RUNNING"));
  EXPECT_DOUBLE_EQ(snapshot.engines.front().n1Percent, 67.5);
}

TEST(SystemsTelemetry, ReportsFuelLevelFlowAndEndurance) {
  Entity entity;
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.modelName = QStringLiteral("F-16C");
  entity.speedKnots = 450.0;
  entity.fuelCapacityKilograms = 3200.0;
  entity.fuelRemainingKilograms = 1600.0;

  const auto snapshot =
      application::makeEstimatedSystemsTelemetrySnapshot(entity, 900.0);

  EXPECT_TRUE(snapshot.fuelAvailable);
  EXPECT_DOUBLE_EQ(snapshot.fuelCapacityKilograms, 3200.0);
  EXPECT_DOUBLE_EQ(snapshot.fuelRemainingKilograms, 1600.0);
  EXPECT_DOUBLE_EQ(snapshot.fuelPercent, 50.0);
  EXPECT_GT(snapshot.totalFuelFlowKilogramsPerHour, 0.0);
  EXPECT_TRUE(snapshot.enduranceAvailable);
  EXPECT_GT(snapshot.estimatedEnduranceSeconds, 0.0);
}

TEST(SystemsTelemetry, KinematicConsumptionDecreasesFuelWithoutGoingNegative) {
  Entity entity;
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.speedKnots = 450.0;
  entity.fuelCapacityKilograms = 100.0;
  entity.fuelRemainingKilograms = 10.0;

  application::consumeEstimatedFuel(entity, 60.0, 900.0);
  EXPECT_LT(entity.fuelRemainingKilograms, 10.0);
  EXPECT_GE(entity.fuelRemainingKilograms, 0.0);

  application::consumeEstimatedFuel(entity, 36000.0, 900.0);
  EXPECT_DOUBLE_EQ(entity.fuelRemainingKilograms, 0.0);
}
