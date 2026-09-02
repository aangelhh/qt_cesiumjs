#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaSimulationControlPublisher.h"
#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <algorithm>
#include <memory>

TEST(HlaSimulationControlPublisher, PublishesStartPauseAndStopUsingRprClasses) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "control-test";
  configuration.federateName = "control-01";
  configuration.federateType = "qttest";
  ASSERT_TRUE(runtime.start(configuration).success);
  tactical::hla::HlaSimulationControlPublisher publisher(runtime);

  ASSERT_TRUE(publisher.publish(
      tactical::hla::RemoteSimulationControl::StartResume, 12.0).success);
  ASSERT_TRUE(publisher.publish(
      tactical::hla::RemoteSimulationControl::Pause, 13.0).success);
  ASSERT_TRUE(publisher.publish(
      tactical::hla::RemoteSimulationControl::Stop, 14.0).success);

  EXPECT_EQ(std::count(
      backendView->sentInteractionClasses().begin(),
      backendView->sentInteractionClasses().end(),
      "HLAinteractionRoot.StartResume"), 1);
  EXPECT_EQ(std::count(
      backendView->sentInteractionClasses().begin(),
      backendView->sentInteractionClasses().end(),
      "HLAinteractionRoot.StopFreeze"), 2);
  EXPECT_EQ(std::count(
      backendView->operations().begin(), backendView->operations().end(),
      tactical::hla::MockHlaBackend::Operation::PublishInteractionClass), 2);
}
