#include <gtest/gtest.h>

#include "application/SensorEngine.h"

#include <QSet>

namespace {

Entity makeEntity(
    const QString& name,
    int forceIdentifier,
    const QString& domain,
    double longitude) {
  Entity entity;
  entity.name = name;
  entity.forceIdentifier = forceIdentifier;
  entity.domain = domain;
  entity.latitude = 40.0;
  entity.longitude = longitude;
  entity.altitude = domain == QStringLiteral("Air") ? 5000 : 0;
  return entity;
}

SensorDefinition makeRadar() {
  SensorDefinition radar;
  radar.id = QStringLiteral("radar-1");
  radar.sensorType = QStringLiteral("radar");
  radar.sensorSubType = QStringLiteral("airborneRadar");
  radar.maxRangeMeters = 100000.0;
  radar.azimuthWidthDegrees = 360.0;
  radar.elevationWidthDegrees = 180.0;
  return radar;
}

SensorDefinition makeGroundRadar() {
  SensorDefinition radar = makeRadar();
  radar.id = QStringLiteral("ground-radar-1");
  radar.sensorSubType = QStringLiteral("groundRadar");
  radar.azimuthWidthDegrees = 360.0;
  return radar;
}

} // namespace

TEST(SensorEngine, ContactCarriesProducingSensorTypeAndSubType) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Observer"), 1, QStringLiteral("Air"), 0.0);
  observer.sensors.push_back(makeRadar());
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("Target"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  ASSERT_EQ(entities.at(0).sensorContacts.size(), 1);
  const SensorContact& contact = entities.at(0).sensorContacts.front();
  EXPECT_EQ(contact.sensorId, QStringLiteral("radar-1"));
  EXPECT_EQ(contact.sensorModelProviderId, QStringLiteral("native"));
  EXPECT_EQ(contact.sensorType, QStringLiteral("radar"));
  EXPECT_EQ(contact.sensorSubType, QStringLiteral("airborneRadar"));
  EXPECT_EQ(contact.targetEntityName, QStringLiteral("Target"));
  EXPECT_EQ(
      contact.evaluation.requestedModelProviderId,
      QStringLiteral("native"));
  EXPECT_EQ(
      contact.evaluation.effectiveModelProviderId,
      QStringLiteral("native"));
  EXPECT_EQ(contact.evaluation.providerVersion, QStringLiteral("native-v1"));
  EXPECT_FALSE(contact.evaluation.fallbackUsed);
  EXPECT_DOUBLE_EQ(contact.evaluation.detectionProbability, 1.0);
  EXPECT_GE(contact.evaluation.evaluationDurationMilliseconds, 0.0);
  ASSERT_EQ(entities.at(0).sensorRuntimeStatuses.size(), 1);
  EXPECT_EQ(entities.at(0).sensorRuntimeStatuses.front().evaluationCount, 1U);
  EXPECT_EQ(entities.at(0).sensorRuntimeStatuses.front().detectionCount, 1U);
}

TEST(SensorEngine, DuplicateTargetNamesProduceContactsWithDistinctIds) {
  QVector<Entity> entities;
  Entity observer = makeEntity(
      QStringLiteral("Observer"), 1, QStringLiteral("Air"), 0.0);
  observer.sensors.push_back(makeRadar());
  Entity first = makeEntity(
      QStringLiteral("Bandit"), 2, QStringLiteral("Air"), 0.1);
  Entity second = makeEntity(
      QStringLiteral("Bandit"), 2, QStringLiteral("Air"), 0.2);
  const QString firstId = first.entityId;
  const QString secondId = second.entityId;
  entities = {observer, first, second};

  SensorEngine::updateEntityContacts(entities);

  ASSERT_EQ(entities.at(0).sensorContacts.size(), 2);
  QSet<QString> targetIds;
  for (const SensorContact& contact : entities.at(0).sensorContacts) {
    EXPECT_EQ(contact.targetEntityName, QStringLiteral("Bandit"));
    targetIds.insert(contact.targetEntityId);
  }
  EXPECT_TRUE(targetIds.contains(firstId));
  EXPECT_TRUE(targetIds.contains(secondId));
}

TEST(SensorEngine, RejectsTargetDomainsDisabledBySensorConfiguration) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Observer"), 1, QStringLiteral("Air"), 0.0);
  SensorDefinition radar = makeRadar();
  radar.sensorSubType = QStringLiteral("generic");
  radar.canDetectAir = false;
  radar.canDetectGround = true;
  observer.sensors.push_back(radar);
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("AirTarget"), 2, QStringLiteral("Air"), 0.1));
  entities.push_back(makeEntity(QStringLiteral("GroundTarget"), 2, QStringLiteral("Ground"), 0.2));

  SensorEngine::updateEntityContacts(entities);

  ASSERT_EQ(entities.at(0).sensorContacts.size(), 1);
  EXPECT_EQ(
      entities.at(0).sensorContacts.front().targetEntityName,
      QStringLiteral("GroundTarget"));
}

TEST(SensorEngine, AirborneRadarUsesObserverHeadingAndConfiguredFieldOfView) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Observer"), 1, QStringLiteral("Air"), 0.0);
  observer.headingDegrees = 90.0;
  SensorDefinition radar = makeRadar();
  radar.azimuthWidthDegrees = 60.0;
  observer.sensors.push_back(radar);
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("EastTarget"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);
  ASSERT_EQ(entities.at(0).sensorContacts.size(), 1);

  entities[0].headingDegrees = 270.0;
  SensorEngine::updateEntityContacts(entities);
  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}

TEST(SensorEngine, AirborneRadarRejectsTargetsOutsideMaximumRange) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Observer"), 1, QStringLiteral("Air"), 0.0);
  SensorDefinition radar = makeRadar();
  radar.maxRangeMeters = 5000.0;
  observer.sensors.push_back(radar);
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("FarTarget"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}

TEST(SensorEngine, AirborneRadarDoesNotOperateFromGroundPlatform) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Observer"), 1, QStringLiteral("Ground"), 0.0);
  observer.sensors.push_back(makeRadar());
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("AirTarget"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}

TEST(SensorEngine, AirborneRadarMvpIgnoresNonAirTargets) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Observer"), 1, QStringLiteral("Air"), 0.0);
  SensorDefinition radar = makeRadar();
  radar.canDetectGround = true;
  observer.sensors.push_back(radar);
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("GroundTarget"), 2, QStringLiteral("Ground"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}

TEST(SensorEngine, GroundRadarWithFullCoverageDetectsAirRegardlessOfHeading) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("GroundRadar"), 1, QStringLiteral("Ground"), 0.0);
  observer.headingDegrees = 270.0;
  observer.sensors.push_back(makeGroundRadar());
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("AirTarget"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  ASSERT_EQ(entities.at(0).sensorContacts.size(), 1);
  EXPECT_EQ(
      entities.at(0).sensorContacts.front().sensorSubType,
      QStringLiteral("groundRadar"));
  EXPECT_EQ(
      entities.at(0).sensorContacts.front().targetEntityName,
      QStringLiteral("AirTarget"));
}

TEST(SensorEngine, GroundRadarSectorUsesConfiguredHeadingAndFieldOfView) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("GroundRadar"), 1, QStringLiteral("Ground"), 0.0);
  observer.headingDegrees = 90.0;
  SensorDefinition radar = makeGroundRadar();
  radar.azimuthWidthDegrees = 60.0;
  observer.sensors.push_back(radar);
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("AirTarget"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);
  ASSERT_EQ(entities.at(0).sensorContacts.size(), 1);

  entities[0].headingDegrees = 270.0;
  SensorEngine::updateEntityContacts(entities);
  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}

TEST(SensorEngine, GroundRadarDoesNotOperateFromAirPlatform) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("Aircraft"), 1, QStringLiteral("Air"), 0.0);
  observer.sensors.push_back(makeGroundRadar());
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("AirTarget"), 2, QStringLiteral("Air"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}

TEST(SensorEngine, GroundRadarMvpIgnoresNonAirTargets) {
  QVector<Entity> entities;
  Entity observer = makeEntity(QStringLiteral("GroundRadar"), 1, QStringLiteral("Ground"), 0.0);
  SensorDefinition radar = makeGroundRadar();
  radar.canDetectGround = true;
  observer.sensors.push_back(radar);
  entities.push_back(observer);
  entities.push_back(makeEntity(QStringLiteral("GroundTarget"), 2, QStringLiteral("Ground"), 0.1));

  SensorEngine::updateEntityContacts(entities);

  EXPECT_TRUE(entities.at(0).sensorContacts.isEmpty());
}
