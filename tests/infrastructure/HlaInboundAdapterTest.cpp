#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"

TEST(HlaInboundAdapter, ConvertsRemotePlatformLifecycleToEntityChanges) {
  tactical::hla::HlaInboundAdapter adapter;
  tactical::hla::RprEntityState source;
  source.name = "remote-fighter";
  source.domain = "Air";
  source.latitudeDegrees = 41.0;
  source.longitudeDegrees = -3.0;
  source.altitudeMeters = 5000.0;
  source.speedKnots = 420.0;
  const auto attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      source, 2, 3, 4);

  adapter.onObjectDiscovered({
      12,
      "HLAobjectRoot.BaseEntity.PhysicalEntity.Platform.Aircraft",
      "remote-fighter-01"});
  adapter.onObjectReflected({12, attributes, {}});
  auto changes = adapter.takeEntityChanges();

  ASSERT_EQ(changes.size(), 1U);
  EXPECT_FALSE(changes.front().removed);
  EXPECT_EQ(changes.front().state.stableId, "hla:remote-fighter-01");
  EXPECT_EQ(changes.front().state.name, "remote-figh");
  EXPECT_EQ(changes.front().state.domain, "Air");
  EXPECT_NEAR(changes.front().state.altitudeMeters, 5000.0, 0.1);
  EXPECT_TRUE(adapter.takeEntityChanges().empty());

  adapter.onObjectRemoved({12, {}});
  changes = adapter.takeEntityChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_TRUE(changes.front().removed);
  EXPECT_EQ(changes.front().state.stableId, "hla:remote-fighter-01");
}

TEST(HlaInboundAdapter, ConvertsRprSimulationControlInteractions) {
  tactical::hla::HlaInboundAdapter adapter;
  adapter.onInteractionReceived({"HLAinteractionRoot.StartResume", {}, {}});
  adapter.onInteractionReceived({
      "HLAinteractionRoot.StopFreeze", {{"Reason", {0}}}, {}});
  adapter.onInteractionReceived({
      "HLAinteractionRoot.StopFreeze", {{"Reason", {2}}}, {}});

  const auto controls = adapter.takeSimulationControls();
  ASSERT_EQ(controls.size(), 3U);
  EXPECT_EQ(controls[0], tactical::hla::RemoteSimulationControl::StartResume);
  EXPECT_EQ(controls[1], tactical::hla::RemoteSimulationControl::Pause);
  EXPECT_EQ(controls[2], tactical::hla::RemoteSimulationControl::Stop);
  EXPECT_TRUE(adapter.takeSimulationControls().empty());
}
