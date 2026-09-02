#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaEntityPublisher.h"
#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <algorithm>
#include <memory>

namespace {

tactical::hla::SessionConfiguration configuration() {
  tactical::hla::SessionConfiguration value;
  value.federationName = "publisher-test";
  value.federateName = "publisher-01";
  value.federateType = "qttest";
  return value;
}

std::size_t operationCount(
    const tactical::hla::MockHlaBackend& backend,
    tactical::hla::MockHlaBackend::Operation operation) {
  return static_cast<std::size_t>(std::count(
      backend.operations().begin(), backend.operations().end(), operation));
}

tactical::hla::RprEntityState entity(
    std::string id,
    std::string displayName) {
  tactical::hla::RprEntityState value;
  value.stableId = std::move(id);
  value.name = std::move(displayName);
  value.domain = "Air";
  value.latitudeDegrees = 40.0;
  value.longitudeDegrees = -4.0;
  value.altitudeMeters = 3000.0;
  return value;
}

} // namespace

TEST(HlaEntityPublisher, RegistersDuplicateDisplayNamesByStableIdAndUpdatesThem) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  ASSERT_TRUE(runtime.start(configuration()).success);
  tactical::hla::HlaEntityPublisher publisher(runtime);

  ASSERT_TRUE(publisher.synchronize({
      entity("entity-a", "mirage2000"),
      entity("entity-b", "mirage2000")}).success);
  EXPECT_EQ(publisher.registeredObjectCount(), 2U);
  EXPECT_EQ(operationCount(
      *backendView,
      tactical::hla::MockHlaBackend::Operation::PublishObjectClass), 1U);
  EXPECT_EQ(operationCount(
      *backendView,
      tactical::hla::MockHlaBackend::Operation::RegisterObjectInstance), 2U);
  EXPECT_EQ(operationCount(
      *backendView,
      tactical::hla::MockHlaBackend::Operation::UpdateObjectAttributes), 2U);

  ASSERT_TRUE(publisher.synchronize({entity("entity-b", "mirage2000")}).success);
  EXPECT_EQ(publisher.registeredObjectCount(), 1U);
  EXPECT_EQ(operationCount(
      *backendView,
      tactical::hla::MockHlaBackend::Operation::DeleteObjectInstance), 1U);
}

TEST(HlaEntityPublisher, RejectsEntityWithoutStableId) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  ASSERT_TRUE(runtime.start(configuration()).success);
  tactical::hla::HlaEntityPublisher publisher(runtime);

  tactical::hla::RprEntityState invalid;
  invalid.name = "no-id";
  const tactical::hla::Result result = publisher.synchronize({invalid});

  EXPECT_FALSE(result.success);
  EXPECT_NE(result.message.find("stable id"), std::string::npos);
}
