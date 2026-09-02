#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaRuntime.h"
#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <memory>
#include <vector>

namespace {

tactical::hla::SessionConfiguration validConfiguration() {
  tactical::hla::SessionConfiguration configuration;
  configuration.localSettingsDesignator = "crcAddress=localhost";
  configuration.federationName = "TacticalEnvironment";
  configuration.federateName = "qttest-01";
  configuration.federateType = "qttest";
  configuration.fomModules = {"RPR_FOM.xml", "NETN-SE.xml"};
  return configuration;
}

} // namespace

TEST(HlaRuntime, ExecutesLifecycleInOrder) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));

  EXPECT_TRUE(runtime.start(validConfiguration()).success);
  EXPECT_EQ(runtime.state(), tactical::hla::BackendState::Joined);
  EXPECT_TRUE(runtime.poll(0.01).success);
  EXPECT_TRUE(runtime.stop().success);
  EXPECT_EQ(runtime.state(), tactical::hla::BackendState::Disconnected);

  EXPECT_EQ(
      backendView->operations(),
      (std::vector<tactical::hla::MockHlaBackend::Operation>{
          tactical::hla::MockHlaBackend::Operation::Connect,
          tactical::hla::MockHlaBackend::Operation::CreateFederation,
          tactical::hla::MockHlaBackend::Operation::JoinFederation,
          tactical::hla::MockHlaBackend::Operation::Poll,
          tactical::hla::MockHlaBackend::Operation::Resign,
          tactical::hla::MockHlaBackend::Operation::Disconnect}));
}

TEST(HlaRuntime, RejectsInvalidConfigurationBeforeConnecting) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  auto configuration = validConfiguration();
  configuration.federationName.clear();

  const tactical::hla::Result result = runtime.start(configuration);

  EXPECT_FALSE(result.success);
  EXPECT_FALSE(result.message.empty());
  EXPECT_TRUE(backendView->operations().empty());
  EXPECT_EQ(runtime.state(), tactical::hla::BackendState::Disconnected);
}

TEST(HlaRuntime, RollsBackConnectionWhenFederationCreationFails) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  backend->failNext(
      tactical::hla::MockHlaBackend::Operation::CreateFederation,
      "FOM rejected");
  tactical::hla::HlaRuntime runtime(std::move(backend));

  const tactical::hla::Result result = runtime.start(validConfiguration());

  EXPECT_FALSE(result.success);
  EXPECT_EQ(result.message, "FOM rejected");
  EXPECT_EQ(runtime.state(), tactical::hla::BackendState::Disconnected);
  EXPECT_EQ(
      backendView->operations(),
      (std::vector<tactical::hla::MockHlaBackend::Operation>{
          tactical::hla::MockHlaBackend::Operation::Connect,
          tactical::hla::MockHlaBackend::Operation::CreateFederation,
          tactical::hla::MockHlaBackend::Operation::Disconnect}));
}

TEST(HlaRuntime, CanJoinAnExistingFederationWithoutCreatingIt) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  auto configuration = validConfiguration();
  configuration.createFederationIfMissing = false;

  EXPECT_TRUE(runtime.start(configuration).success);
  EXPECT_EQ(
      backendView->operations(),
      (std::vector<tactical::hla::MockHlaBackend::Operation>{
          tactical::hla::MockHlaBackend::Operation::Connect,
          tactical::hla::MockHlaBackend::Operation::JoinFederation}));
}
