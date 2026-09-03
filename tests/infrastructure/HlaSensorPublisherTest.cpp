#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaSensorPublisher.h"
#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <algorithm>
#include <memory>

TEST(HlaSensorPublisher, PublishesEmitterAndActiveRadarBeamLifecycle) {
  auto backend = std::make_unique<tactical::hla::MockHlaBackend>();
  tactical::hla::MockHlaBackend* backendView = backend.get();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "sensor-test";
  configuration.federateName = "sensor-01";
  configuration.federateType = "qttest";
  ASSERT_TRUE(runtime.start(configuration).success);
  tactical::hla::HlaSensorPublisher publisher(runtime);

  tactical::hla::RprSensorState sensor;
  sensor.entityStableId = "fighter-01";
  sensor.sensorId = "radar-01";
  sensor.enabled = true;
  sensor.emitting = true;
  sensor.frequencyHertz = 10.0e9;
  sensor.bandwidthHertz = 2.0e6;
  sensor.peakPowerWatts = 5000.0;

  ASSERT_TRUE(publisher.synchronize({sensor}).success);
  EXPECT_EQ(publisher.registeredSensorCount(), 1U);
  EXPECT_NE(std::find(
      backendView->publishedObjectClasses().begin(),
      backendView->publishedObjectClasses().end(),
      "HLAobjectRoot.EmbeddedSystem.EmitterSystem"),
      backendView->publishedObjectClasses().end());
  EXPECT_NE(std::find(
      backendView->publishedObjectClasses().begin(),
      backendView->publishedObjectClasses().end(),
      "HLAobjectRoot.EmitterBeam.RadarBeam"),
      backendView->publishedObjectClasses().end());

  sensor.emitting = false;
  ASSERT_TRUE(publisher.synchronize({sensor}).success);
  EXPECT_EQ(publisher.registeredSensorCount(), 1U);
  ASSERT_TRUE(publisher.synchronize({}).success);
  EXPECT_EQ(publisher.registeredSensorCount(), 0U);
  EXPECT_EQ(std::count(
      backendView->operations().begin(), backendView->operations().end(),
      tactical::hla::MockHlaBackend::Operation::DeleteObjectInstance), 2);
}
