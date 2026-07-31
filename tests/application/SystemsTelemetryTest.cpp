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
