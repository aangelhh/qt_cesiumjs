#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"
#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <algorithm>
#include <memory>

namespace {

const tactical::hla::NamedValue* findParameter(
    const std::vector<tactical::hla::NamedValue>& parameters,
    const std::string& name) {
  const auto iterator = std::find_if(
      parameters.cbegin(), parameters.cend(), [&name](const auto& parameter) {
        return parameter.name == name;
      });
  return iterator == parameters.cend() ? nullptr : &*iterator;
}

std::string decodeObjectIdentifier(const tactical::hla::ByteBuffer& value) {
  const auto terminator = std::find(value.cbegin(), value.cend(), 0);
  return std::string(value.cbegin(), terminator);
}

} // namespace

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
  munition.firingObjectInstanceName = "qttest.fighter-01";
  munition.targetObjectInstanceName = "qttest.target-01";
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
  ASSERT_EQ(backendView->sentInteractions().size(), 1U);
  const auto& parameters = backendView->sentInteractions().front().parameters;
  const auto* firing = findParameter(parameters, "FiringObjectIdentifier");
  const auto* target = findParameter(parameters, "TargetObjectIdentifier");
  ASSERT_NE(firing, nullptr);
  ASSERT_NE(target, nullptr);
  EXPECT_EQ(decodeObjectIdentifier(firing->value), "qttest.fighter-01");
  EXPECT_EQ(decodeObjectIdentifier(target->value), "qttest.target-01");
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

  tactical::hla::RprWeaponFireState fire;
  fire.stableId = "missile-01";
  fire.munitionType = "Missile";
  fire.firingObjectInstanceName = "qttest.fighter-01";
  fire.targetObjectInstanceName = "qttest.target-01";
  ASSERT_TRUE(publisher.synchronize({fire}).success);

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
  ASSERT_EQ(backendView->sentInteractions().size(), 2U);
  const auto& fireParameters =
      backendView->sentInteractions().at(0).parameters;
  const auto& detonationParameters =
      backendView->sentInteractions().at(1).parameters;
  const auto* fireEvent = findParameter(fireParameters, "EventIdentifier");
  const auto* detonationEvent =
      findParameter(detonationParameters, "EventIdentifier");
  ASSERT_NE(fireEvent, nullptr);
  ASSERT_NE(detonationEvent, nullptr);
  EXPECT_EQ(fireEvent->value, detonationEvent->value);
  const auto* detonationFiring =
      findParameter(detonationParameters, "FiringObjectIdentifier");
  const auto* detonationTarget =
      findParameter(detonationParameters, "TargetObjectIdentifier");
  ASSERT_NE(detonationFiring, nullptr);
  ASSERT_NE(detonationTarget, nullptr);
  EXPECT_EQ(
      decodeObjectIdentifier(detonationFiring->value),
      "qttest.fighter-01");
  EXPECT_EQ(
      decodeObjectIdentifier(detonationTarget->value),
      "qttest.target-01");
}
