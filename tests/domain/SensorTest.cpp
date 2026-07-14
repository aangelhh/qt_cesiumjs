#include <gtest/gtest.h>

#include "domain/Sensor.h"

TEST(SensorTypes, ParsesSupportedTypesCaseInsensitively) {
  EXPECT_EQ(sensor::typeFromString(QStringLiteral("Radar")), SensorType::Radar);
  EXPECT_EQ(sensor::typeFromString(QStringLiteral("infrared")), SensorType::Infrared);
  EXPECT_EQ(sensor::typeFromString(QStringLiteral("IR")), SensorType::Infrared);
  EXPECT_EQ(sensor::typeFromString(QStringLiteral("optical")), SensorType::Visual);
  EXPECT_EQ(sensor::typeFromString(QStringLiteral("unsupported")), SensorType::Unknown);
}

TEST(SensorTypes, ParsesSupportedSubTypesWithLegacySeparators) {
  EXPECT_EQ(
      sensor::subTypeFromString(QStringLiteral("AirborneRadar")),
      SensorSubType::AirborneRadar);
  EXPECT_EQ(
      sensor::subTypeFromString(QStringLiteral("ground_radar")),
      SensorSubType::GroundRadar);
  EXPECT_EQ(
      sensor::subTypeFromString(QStringLiteral("Naval Radar")),
      SensorSubType::NavalRadar);
  EXPECT_EQ(sensor::subTypeFromString(QString()), SensorSubType::Generic);
}

TEST(SensorTypes, AppliesConfiguredTargetDomains) {
  SensorDefinition definition;
  definition.canDetectAir = true;
  definition.canDetectGround = false;
  definition.canDetectSurface = true;

  EXPECT_TRUE(sensor::canDetectDomain(definition, QStringLiteral("Air")));
  EXPECT_FALSE(sensor::canDetectDomain(definition, QStringLiteral("Ground")));
  EXPECT_TRUE(sensor::canDetectDomain(definition, QStringLiteral("Surface")));
  EXPECT_FALSE(sensor::canDetectDomain(definition, QStringLiteral("Space")));
}

TEST(SensorTypes, AirborneRadarOnlyOperatesFromAirPlatform) {
  SensorDefinition definition;
  definition.sensorSubType = QStringLiteral("airborneRadar");

  EXPECT_TRUE(sensor::canOperateFromDomain(definition, QStringLiteral("Air")));
  EXPECT_FALSE(sensor::canOperateFromDomain(definition, QStringLiteral("Ground")));
  EXPECT_FALSE(sensor::canOperateFromDomain(definition, QStringLiteral("Surface")));
}

TEST(SensorTypes, AirborneRadarMvpOnlySupportsAirTargets) {
  SensorDefinition definition;
  definition.sensorSubType = QStringLiteral("airborneRadar");
  definition.canDetectAir = true;
  definition.canDetectGround = true;
  definition.canDetectSurface = true;

  EXPECT_TRUE(sensor::supportsTargetDomain(definition, QStringLiteral("Air")));
  EXPECT_FALSE(sensor::supportsTargetDomain(definition, QStringLiteral("Ground")));
  EXPECT_FALSE(sensor::supportsTargetDomain(definition, QStringLiteral("Surface")));
}

TEST(SensorTypes, GroundRadarOnlyOperatesFromGroundPlatform) {
  SensorDefinition definition;
  definition.sensorSubType = QStringLiteral("groundRadar");

  EXPECT_FALSE(sensor::canOperateFromDomain(definition, QStringLiteral("Air")));
  EXPECT_TRUE(sensor::canOperateFromDomain(definition, QStringLiteral("Ground")));
  EXPECT_FALSE(sensor::canOperateFromDomain(definition, QStringLiteral("Surface")));
}

TEST(SensorTypes, GroundRadarMvpOnlySupportsAirTargets) {
  SensorDefinition definition;
  definition.sensorSubType = QStringLiteral("groundRadar");
  definition.canDetectAir = true;
  definition.canDetectGround = true;
  definition.canDetectSurface = true;

  EXPECT_TRUE(sensor::supportsTargetDomain(definition, QStringLiteral("Air")));
  EXPECT_FALSE(sensor::supportsTargetDomain(definition, QStringLiteral("Ground")));
  EXPECT_FALSE(sensor::supportsTargetDomain(definition, QStringLiteral("Surface")));
}
