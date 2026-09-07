#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaBackendFactory.h"
#include "infrastructure/interoperability/hla/OpenRtiCompatibilityBackend.h"

namespace {

class CapturingOpenRtiBackend final : public tactical::hla::IHlaBackend {
public:
  std::string id() const override { return "openrti1516e"; }
  std::string version() const override { return "test"; }
  std::vector<std::string> capabilities() const override { return {}; }

  tactical::hla::Result connect(const std::string& localSettings) override {
    connectedWith = localSettings;
    currentState = tactical::hla::BackendState::Connected;
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result createFederation(
      const std::string&,
      const std::vector<std::string>&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result joinFederation(
      const std::string&,
      const std::string&,
      const std::string&,
      const std::vector<std::string>&) override {
    currentState = tactical::hla::BackendState::Joined;
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result publishObjectClass(
      const std::string&,
      const std::vector<std::string>&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result subscribeObjectClass(
      const std::string&,
      const std::vector<std::string>&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result registerObjectInstance(
      const std::string&,
      const std::string&,
      tactical::hla::ObjectInstanceId& instanceId) override {
    instanceId = 1;
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result updateObjectAttributes(
      tactical::hla::ObjectInstanceId,
      const std::vector<tactical::hla::NamedValue>&,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result updateObjectAttributesAtTime(
      tactical::hla::ObjectInstanceId,
      const std::vector<tactical::hla::NamedValue>&,
      double,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result deleteObjectInstance(
      tactical::hla::ObjectInstanceId,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result deleteObjectInstanceAtTime(
      tactical::hla::ObjectInstanceId,
      double,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result publishInteractionClass(
      const std::string&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result subscribeInteractionClass(
      const std::string&,
      const std::vector<std::string>&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result sendInteraction(
      const std::string&,
      const std::vector<tactical::hla::NamedValue>&,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result sendInteractionAtTime(
      const std::string&,
      const std::vector<tactical::hla::NamedValue>&,
      double,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result registerSynchronizationPoint(
      const std::string&,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result achieveSynchronizationPoint(
      const std::string&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result enableTimeRegulation(double) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result enableTimeConstrained() override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result requestTimeAdvance(double) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result requestAttributeOwnershipAcquisition(
      tactical::hla::ObjectInstanceId,
      const std::vector<std::string>&,
      const tactical::hla::ByteBuffer&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result unconditionalAttributeOwnershipDivestiture(
      tactical::hla::ObjectInstanceId,
      const std::vector<std::string>&) override {
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result poll(double) override {
    return tactical::hla::Result::ok();
  }
  void setEventSink(tactical::hla::IHlaEventSink*) override {}
  tactical::hla::Result resign() override {
    currentState = tactical::hla::BackendState::Connected;
    return tactical::hla::Result::ok();
  }
  tactical::hla::Result disconnect() override {
    currentState = tactical::hla::BackendState::Disconnected;
    return tactical::hla::Result::ok();
  }
  tactical::hla::BackendState state() const override { return currentState; }
  std::string lastError() const override { return {}; }

  std::string connectedWith;
  tactical::hla::BackendState currentState =
      tactical::hla::BackendState::Disconnected;
};

} // namespace

TEST(HlaBackendFactory, CreatesBuiltInMockBackend) {
  tactical::hla::BackendConfiguration configuration;
  configuration.adapter = tactical::hla::BackendAdapter::Mock;
  configuration.expectedBackendId = "mock";

  auto result = tactical::hla::HlaBackendFactory::create(configuration);

  ASSERT_TRUE(result) << result.error;
  EXPECT_EQ(result.backend->id(), "mock");
}

TEST(HlaBackendFactory, RejectsUnexpectedBackendIdentity) {
  tactical::hla::BackendConfiguration configuration;
  configuration.adapter = tactical::hla::BackendAdapter::Mock;
  configuration.expectedBackendId = "openrti";

  auto result = tactical::hla::HlaBackendFactory::create(configuration);

  EXPECT_FALSE(result);
  EXPECT_NE(result.error.find("id mismatch"), std::string::npos);
}

TEST(HlaBackendFactory, LoadsConfiguredSharedLibrary) {
#ifdef QTTEST_HLA_MOCK_PLUGIN_PATH
  tactical::hla::BackendConfiguration configuration;
  configuration.adapter = tactical::hla::BackendAdapter::SharedLibrary;
  configuration.expectedBackendId = "mock-plugin";
  configuration.libraryPath = QTTEST_HLA_MOCK_PLUGIN_PATH;

  auto result = tactical::hla::HlaBackendFactory::create(configuration);

  ASSERT_TRUE(result) << result.error;
  EXPECT_EQ(result.backend->id(), "mock-plugin");
#else
  GTEST_SKIP() << "Mock HLA plugin path is unavailable";
#endif
}

TEST(HlaBackendFactory, OpenRtiAcceptsPitchCrcAddressSyntax) {
  auto backend = std::make_unique<CapturingOpenRtiBackend>();
  CapturingOpenRtiBackend* backendView = backend.get();
  tactical::hla::OpenRtiCompatibilityBackend compatibilityBackend(
      std::move(backend));

  const tactical::hla::Result result =
      compatibilityBackend.connect("crcAddress=192.0.2.10:14321");

  ASSERT_TRUE(result.success) << result.message;
  EXPECT_EQ(backendView->connectedWith, "rti://192.0.2.10:14321");
}
