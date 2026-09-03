#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"
#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <algorithm>
#include <memory>

TEST(HlaWarfarePublisher, SendsEachWeaponFireExactlyOnce) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "warfare-test";
  configuration.federateName = "warfare-01";
  configuration.federateType = "qttest";
  ASSERT_TRUE(runtime.start(configuration).success);
  tactical::hla::HlaWarfarePublisher publisher(runtime);

  tactical::hla::RprWeaponFireState munition;
  munition.stableId = "missile-01";
  munition.munitionType = "Missile";
  munition.latitudeDegrees = 40.0;
  munition.longitudeDegrees = -4.0;
  munition.altitudeMeters = 3000.0;
  munition.speedMetersPerSecond = 500.0;

  ASSERT_TRUE(publisher.synchronize({munition}).success);
  ASSERT_TRUE(publisher.synchronize({munition}).success);

  EXPECT_EQ(publisher.sentWeaponFireCount(), 1U);
  EXPECT_EQ(std::count(
      backendView->operations().begin(),
      backendView->operations().end(),
      tactical::hla::MockHlaBackend::Operation::PublishInteractionClass), 1);
  EXPECT_EQ(std::count(
      backendView->operations().begin(),
      backendView->operations().end(),
      tactical::hla::MockHlaBackend::Operation::SendInteraction), 1);
}

TEST(HlaWarfarePublisher, SendsEachMunitionDetonationExactlyOnce) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "detonation-test";
  configuration.federateName = "warfare-01";
  configuration.federateType = "qttest";
  ASSERT_TRUE(runtime.start(configuration).success);
  tactical::hla::HlaWarfarePublisher publisher(runtime);

  tactical::hla::RprMunitionDetonationState detonation;
  detonation.effectId = "missile-01-impact";
  detonation.munitionStableId = "missile-01";
  detonation.munitionType = "Missile";
  detonation.latitudeDegrees = 40.0;
  detonation.longitudeDegrees = -4.0;
  detonation.altitudeMeters = 1200.0;

  ASSERT_TRUE(publisher.synchronizeDetonations({detonation}).success);
  ASSERT_TRUE(publisher.synchronizeDetonations({detonation}).success);

  EXPECT_EQ(publisher.sentDetonationCount(), 1U);
  EXPECT_EQ(std::count(
      backendView->sentInteractionClasses().begin(),
      backendView->sentInteractionClasses().end(),
      "HLAinteractionRoot.MunitionDetonation"), 1);
}
