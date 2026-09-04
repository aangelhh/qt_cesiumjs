#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"
#include "geospatial/GeographicLibGeospatialService.h"

#include <cmath>
#include <cstring>

namespace {

void appendUnsigned32(tactical::hla::ByteBuffer& output, std::uint32_t value) {
  output.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

tactical::hla::ByteBuffer float32(float value) {
  std::uint32_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  tactical::hla::ByteBuffer output;
  appendUnsigned32(output, bits);
  return output;
}

void appendFloat64(tactical::hla::ByteBuffer& output, double value) {
  std::uint64_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  for (int shift = 56; shift >= 0; shift -= 8) {
    output.push_back(static_cast<std::uint8_t>((bits >> shift) & 0xffU));
  }
}

tactical::hla::ByteBuffer worldLocation(
    double latitude,
    double longitude,
    double altitude) {
  const geospatial::GeographicLibGeospatialService service;
  const geospatial::EcefCoordinate ecef = service.geodeticToEcef(
      {latitude, longitude, altitude});
  tactical::hla::ByteBuffer output;
  appendFloat64(output, ecef.xMeters);
  appendFloat64(output, ecef.yMeters);
  appendFloat64(output, ecef.zMeters);
  return output;
}

tactical::hla::ByteBuffer encodedString(const std::string& value) {
  tactical::hla::ByteBuffer output(value.begin(), value.end());
  output.push_back(0);
  return output;
}

tactical::hla::ByteBuffer objectIdentifierArray(
    const std::vector<std::string>& values) {
  tactical::hla::ByteBuffer output;
  appendUnsigned32(output, static_cast<std::uint32_t>(values.size()));
  for (const std::string& value : values) {
    output.insert(output.end(), value.begin(), value.end());
    output.push_back(0);
  }
  return output;
}

} // namespace

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

TEST(HlaInboundAdapter, ConvertsRemoteMunitionLifecycleWithoutCreatingEntity) {
  tactical::hla::HlaInboundAdapter adapter;
  tactical::hla::RprEntityState source;
  source.name = "remote-missile";
  source.domain = "Air";
  source.entityKind = 2;
  source.entityDomain = 2;
  source.category = 1;
  source.latitudeDegrees = 40.5;
  source.longitudeDegrees = -3.5;
  source.altitudeMeters = 4200.0;
  source.headingDegrees = 135.0;
  source.pitchDegrees = -4.0;
  source.speedKnots = 950.0;
  const auto attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      source, 2, 3, 40);

  adapter.onObjectDiscovered({
      40,
      "HLAobjectRoot.BaseEntity.PhysicalEntity.Munition.NETN_Munition",
      "remote-missile-01"});
  adapter.onObjectReflected({40, attributes, {}});

  EXPECT_TRUE(adapter.takeEntityChanges().empty());
  auto changes = adapter.takeMunitionChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_FALSE(changes.front().removed);
  EXPECT_EQ(changes.front().state.stableId, "hla:remote-missile-01");
  EXPECT_EQ(changes.front().state.entityKind, 2);
  EXPECT_EQ(changes.front().state.category, 1);
  EXPECT_NEAR(changes.front().state.latitudeDegrees, 40.5, 1.0e-6);
  EXPECT_NEAR(changes.front().state.longitudeDegrees, -3.5, 1.0e-6);
  EXPECT_NEAR(changes.front().state.altitudeMeters, 4200.0, 0.01);
  EXPECT_NEAR(changes.front().state.headingDegrees, 135.0, 0.01);
  EXPECT_NEAR(changes.front().state.pitchDegrees, -4.0, 0.01);
  EXPECT_NEAR(changes.front().state.speedKnots, 950.0, 0.01);
  EXPECT_TRUE(adapter.takeMunitionChanges().empty());

  adapter.onObjectRemoved({40, {}});
  changes = adapter.takeMunitionChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_TRUE(changes.front().removed);
  EXPECT_EQ(changes.front().state.stableId, "hla:remote-missile-01");
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

TEST(HlaInboundAdapter, QueuesTimeManagementCallbacksInOrder) {
  tactical::hla::HlaInboundAdapter adapter;
  adapter.onTimeRegulationEnabled(0.0);
  adapter.onTimeConstrainedEnabled(0.0);
  adapter.onTimeAdvanceGranted(0.033);

  const auto events = adapter.takeTimeManagementEvents();
  ASSERT_EQ(events.size(), 3U);
  EXPECT_EQ(
      events[0].kind,
      tactical::hla::RemoteTimeManagementEventKind::RegulationEnabled);
  EXPECT_EQ(
      events[1].kind,
      tactical::hla::RemoteTimeManagementEventKind::ConstrainedEnabled);
  EXPECT_EQ(
      events[2].kind,
      tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted);
  EXPECT_DOUBLE_EQ(events[2].logicalTimeSeconds, 0.033);
  EXPECT_TRUE(adapter.takeTimeManagementEvents().empty());
}

TEST(HlaInboundAdapter, ConvertsEmitterAndRadarBeamWithoutCreatingEntity) {
  tactical::hla::HlaInboundAdapter adapter;
  adapter.onObjectDiscovered({
      20,
      "HLAobjectRoot.EmbeddedSystem.EmitterSystem",
      "qttest.sensor-remote-radar"});
  adapter.onObjectReflected({
      20,
      {{"HostObjectIdentifier", encodedString("qttest.remote-aircraft")}},
      {}});
  adapter.onObjectDiscovered({
      21,
      "HLAobjectRoot.EmitterBeam.RadarBeam",
      "qttest.radar-beam-remote-radar"});
  adapter.onObjectReflected({
      21,
      {{"EmitterSystemIdentifier",
        encodedString("qttest.sensor-remote-radar")},
       {"BeamAzimuthCenter", float32(0.1F)},
       {"BeamAzimuthSweep", float32(1.0471976F)},
       {"BeamElevationCenter", float32(0.0F)},
       {"BeamElevationSweep", float32(0.5235988F)},
       {"EmissionFrequency", float32(10.0e9F)},
       {"FrequencyRange", float32(1.0e6F)},
       {"EffectiveRadiatedPower", float32(80.0F)},
       {"HighDensityTrack", {1}},
       {"TrackObjectIdentifiers", objectIdentifierArray(
            {"qttest.target-01", "qttest.target-02"})}},
      {}});

  EXPECT_TRUE(adapter.takeEntityChanges().empty());
  const auto changes = adapter.takeSensorChanges();
  ASSERT_EQ(changes.size(), 2U);
  EXPECT_FALSE(changes[0].emitting);
  EXPECT_TRUE(changes[1].emitting);
  EXPECT_EQ(changes[1].hostEntityId, "hla:qttest.remote-aircraft");
  EXPECT_NEAR(changes[1].azimuthWidthDegrees, 120.0, 0.01);
  EXPECT_NEAR(changes[1].elevationWidthDegrees, 60.0, 0.01);
  EXPECT_TRUE(changes[1].hasTracks);
  ASSERT_EQ(changes[1].trackedObjectInstanceNames.size(), 2U);
  EXPECT_EQ(
      changes[1].trackedObjectInstanceNames[0], "qttest.target-01");
  EXPECT_EQ(
      changes[1].trackedObjectInstanceNames[1], "qttest.target-02");

  adapter.onObjectRemoved({21, {}});
  const auto removalChanges = adapter.takeSensorChanges();
  ASSERT_EQ(removalChanges.size(), 1U);
  EXPECT_FALSE(removalChanges.front().removed);
  EXPECT_FALSE(removalChanges.front().emitting);
}

TEST(HlaInboundAdapter, DecodesAndDeduplicatesRemoteWarfareInteractions) {
  tactical::hla::HlaInboundAdapter adapter;
  const tactical::hla::RemoteInteraction fire = {
      "HLAinteractionRoot.WeaponFire",
      {{"EventIdentifier", {0, 1, 0, 2, 0, 3}},
       {"FiringLocation", worldLocation(40.4, -3.7, 4500.0)},
       {"FiringObjectIdentifier", encodedString("qttest.fighter-01")},
       {"MunitionObjectIdentifier", encodedString("qttest.munition-01")},
       {"TargetObjectIdentifier", encodedString("qttest.target-01")},
       {"MunitionType", {2, 2, 0, 0, 1, 0, 0, 0}}},
      {}};
  adapter.onInteractionReceived(fire);
  adapter.onInteractionReceived(fire);
  adapter.onInteractionReceived({
      "HLAinteractionRoot.MunitionDetonation",
      {{"EventIdentifier", {0, 1, 0, 2, 0, 3}},
       {"DetonationLocation", worldLocation(40.5, -3.6, 4200.0)},
       {"FiringObjectIdentifier", encodedString("qttest.fighter-01")},
       {"MunitionObjectIdentifier", encodedString("qttest.munition-01")},
       {"TargetObjectIdentifier", encodedString("qttest.target-01")},
       {"MunitionType", {2, 2, 0, 0, 2, 0, 0, 0}}},
      {}});

  const auto events = adapter.takeWarfareEvents();
  ASSERT_EQ(events.size(), 2U);
  EXPECT_EQ(
      events[0].kind,
      tactical::hla::RemoteWarfareEventKind::WeaponFire);
  EXPECT_EQ(events[0].munitionType, "Missile");
  EXPECT_EQ(events[0].firingObjectInstanceName, "qttest.fighter-01");
  EXPECT_EQ(events[0].munitionObjectInstanceName, "qttest.munition-01");
  EXPECT_EQ(events[0].targetObjectInstanceName, "qttest.target-01");
  EXPECT_NEAR(events[0].latitudeDegrees, 40.4, 1.0e-6);
  EXPECT_NEAR(events[0].longitudeDegrees, -3.7, 1.0e-6);
  EXPECT_NEAR(events[0].altitudeMeters, 4500.0, 0.01);
  EXPECT_EQ(
      events[1].kind,
      tactical::hla::RemoteWarfareEventKind::MunitionDetonation);
  EXPECT_EQ(events[1].munitionType, "Bomb");
  EXPECT_EQ(events[1].firingObjectInstanceName, "qttest.fighter-01");
  EXPECT_EQ(events[1].munitionObjectInstanceName, "qttest.munition-01");
  EXPECT_EQ(events[1].targetObjectInstanceName, "qttest.target-01");
  EXPECT_TRUE(adapter.takeWarfareEvents().empty());

  adapter.onInteractionReceived({
      "HLAinteractionRoot.WeaponFire",
      {{"FiringLocation", worldLocation(40.4, -3.7, 4500.0)}},
      {}});
  EXPECT_TRUE(adapter.takeWarfareEvents().empty());
}
