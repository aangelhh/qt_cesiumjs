#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <algorithm>
#include <limits>

namespace {

const tactical::hla::NamedValue* findAttribute(
    const std::vector<tactical::hla::NamedValue>& attributes,
    const std::string& name) {
  const auto iterator = std::find_if(
      attributes.begin(), attributes.end(), [&](const auto& attribute) {
        return attribute.name == name;
      });
  return iterator == attributes.end() ? nullptr : &*iterator;
}

} // namespace

TEST(RprFomEncoding, SelectsRprPlatformClassFromDomain) {
  EXPECT_EQ(
      tactical::hla::RprFomEncoding::objectClassName("Air"),
      "HLAobjectRoot.BaseEntity.PhysicalEntity.Platform.Aircraft");
  EXPECT_EQ(
      tactical::hla::RprFomEncoding::objectClassName("Ground"),
      "HLAobjectRoot.BaseEntity.PhysicalEntity.Platform.GroundVehicle");
  EXPECT_EQ(
      tactical::hla::RprFomEncoding::objectClassName("Surface"),
      "HLAobjectRoot.BaseEntity.PhysicalEntity.Platform.SurfaceVessel");
}

TEST(RprFomEncoding, UsesStableIdForObjectInstanceName) {
  EXPECT_EQ(
      tactical::hla::RprFomEncoding::objectInstanceName(
          "e6ee4edb:{duplicate}"),
      "qttest.e6ee4edb__duplicate_");
}

TEST(RprFomEncoding, EncodesCoreRprAttributesWithStandardSizes) {
  tactical::hla::RprEntityState entity;
  entity.stableId = "entity-01";
  entity.name = "mirage2000-long-name";
  entity.domain = "Air";
  entity.entityKind = 1;
  entity.entityDomain = 2;
  entity.countryCode = 71;
  entity.category = 1;
  entity.latitudeDegrees = 40.0;
  entity.longitudeDegrees = -4.0;
  entity.altitudeMeters = 3000.0;
  entity.headingDegrees = 90.0;
  entity.speedKnots = 320.0;

  const auto attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      entity, 1, 2, 3);

  ASSERT_EQ(attributes.size(), 7U);
  ASSERT_NE(findAttribute(attributes, "EntityType"), nullptr);
  EXPECT_EQ(findAttribute(attributes, "EntityType")->value.size(), 8U);
  EXPECT_EQ(
      findAttribute(attributes, "EntityType")->value,
      (tactical::hla::ByteBuffer{1, 2, 0, 71, 1, 0, 0, 0}));
  EXPECT_EQ(findAttribute(attributes, "EntityIdentifier")->value.size(), 6U);
  EXPECT_EQ(findAttribute(attributes, "Spatial")->value.size(), 60U);
  EXPECT_EQ(findAttribute(attributes, "Spatial")->value.front(), 2U);
  EXPECT_EQ(findAttribute(attributes, "DamageState")->value.size(), 4U);
  EXPECT_EQ(findAttribute(attributes, "ForceIdentifier")->value.size(), 1U);
  const auto* measuredSpeed = findAttribute(
      attributes, "LiveEntityMeasuredSpeed");
  ASSERT_NE(measuredSpeed, nullptr);
  ASSERT_EQ(measuredSpeed->value.size(), 2U);
  EXPECT_EQ(measuredSpeed->value, (tactical::hla::ByteBuffer{0x06, 0x6e}));
  EXPECT_EQ(findAttribute(attributes, "Marking")->value.size(), 12U);
  EXPECT_EQ(findAttribute(attributes, "Marking")->value.front(), 1U);
}

TEST(RprFomEncoding, DecodesPublishedStateBackToLocalCoordinates) {
  tactical::hla::RprEntityState source;
  source.name = "mirage2000";
  source.domain = "Air";
  source.entityKind = 1;
  source.entityDomain = 2;
  source.countryCode = 71;
  source.category = 1;
  source.forceIdentifier = 1;
  source.latitudeDegrees = 40.3366;
  source.longitudeDegrees = -4.5017;
  source.altitudeMeters = 2087.0;
  source.headingDegrees = 325.0;
  source.pitchDegrees = 8.0;
  source.rollDegrees = -20.0;
  source.speedKnots = 370.0;
  source.verticalSpeedMetersPerSecond = 12.5;

  const auto encoded = tactical::hla::RprFomEncoding::encodeAttributes(
      source, 1, 1, 7);
  tactical::hla::RprEntityState decoded;
  const auto result = tactical::hla::RprFomEncoding::decodeAttributes(
      encoded, decoded);

  ASSERT_TRUE(result.success) << result.message;
  EXPECT_EQ(decoded.name, source.name);
  EXPECT_EQ(decoded.entityKind, source.entityKind);
  EXPECT_EQ(decoded.forceIdentifier, source.forceIdentifier);
  EXPECT_NEAR(decoded.latitudeDegrees, source.latitudeDegrees, 1e-5);
  EXPECT_NEAR(decoded.longitudeDegrees, source.longitudeDegrees, 1e-5);
  EXPECT_NEAR(decoded.altitudeMeters, source.altitudeMeters, 0.1);
  EXPECT_NEAR(decoded.headingDegrees, source.headingDegrees, 0.05);
  EXPECT_NEAR(decoded.pitchDegrees, source.pitchDegrees, 0.05);
  EXPECT_NEAR(decoded.rollDegrees, source.rollDegrees, 0.05);
  EXPECT_NEAR(decoded.speedKnots, source.speedKnots, 0.1);
  EXPECT_NEAR(
      decoded.verticalSpeedMetersPerSecond,
      source.verticalSpeedMetersPerSecond,
      0.01);
  EXPECT_EQ(decoded.deadReckoningAlgorithm, 2U);
}

TEST(RprFomEncoding, DecodesVelocityDirectlyFromFpwSpatialVariant) {
  tactical::hla::RprEntityState source;
  source.latitudeDegrees = 52.0;
  source.longitudeDegrees = 4.0;
  source.altitudeMeters = 5000.0;
  source.headingDegrees = 37.0;
  source.speedKnots = 420.0;
  source.verticalSpeedMetersPerSecond = -18.0;
  auto attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      source, 1, 1, 9);
  attributes.erase(std::remove_if(
      attributes.begin(), attributes.end(), [](const auto& attribute) {
        return attribute.name == "LiveEntityMeasuredSpeed";
      }), attributes.end());

  tactical::hla::RprEntityState decoded;
  const auto result = tactical::hla::RprFomEncoding::decodeAttributes(
      attributes, decoded);

  ASSERT_TRUE(result.success) << result.message;
  EXPECT_EQ(decoded.deadReckoningAlgorithm, 2U);
  EXPECT_NEAR(decoded.speedKnots, source.speedKnots, 0.01);
  EXPECT_NEAR(
      decoded.verticalSpeedMetersPerSecond,
      source.verticalSpeedMetersPerSecond,
      0.01);
}

TEST(RprFomEncoding, UsesStaticSpatialVariantForStationaryEntity) {
  tactical::hla::RprEntityState entity;
  entity.latitudeDegrees = 40.0;
  entity.longitudeDegrees = -4.0;
  entity.altitudeMeters = 3000.0;

  const auto attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      entity, 1, 2, 3);
  const auto* spatial = findAttribute(attributes, "Spatial");

  ASSERT_NE(spatial, nullptr);
  ASSERT_FALSE(spatial->value.empty());
  EXPECT_EQ(spatial->value.front(), 1U);
  EXPECT_EQ(spatial->value.size(), 48U);
}

TEST(RprFomEncoding, ClampsMeasuredSpeedToRprInteger16Range) {
  tactical::hla::RprEntityState entity;
  entity.speedKnots = -20.0;
  auto attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      entity, 1, 2, 3);
  ASSERT_NE(findAttribute(attributes, "LiveEntityMeasuredSpeed"), nullptr);
  EXPECT_EQ(
      findAttribute(attributes, "LiveEntityMeasuredSpeed")->value,
      (tactical::hla::ByteBuffer{0x00, 0x00}));

  entity.speedKnots = 100000.0;
  attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      entity, 1, 2, 3);
  EXPECT_EQ(
      findAttribute(attributes, "LiveEntityMeasuredSpeed")->value,
      (tactical::hla::ByteBuffer{0xff, 0xff}));

  entity.speedKnots = std::numeric_limits<double>::quiet_NaN();
  attributes = tactical::hla::RprFomEncoding::encodeAttributes(
      entity, 1, 2, 3);
  EXPECT_EQ(
      findAttribute(attributes, "LiveEntityMeasuredSpeed")->value,
      (tactical::hla::ByteBuffer{0x00, 0x00}));
}

TEST(RprFomEncoding, RejectsMalformedSpatialPayload) {
  tactical::hla::RprEntityState decoded;
  const auto result = tactical::hla::RprFomEncoding::decodeAttributes(
      {{"Spatial", {1, 2, 3}}}, decoded);
  EXPECT_FALSE(result.success);

  tactical::hla::ByteBuffer rvw(60, 0);
  rvw.front() = 4;
  const auto truncatedRvw = tactical::hla::RprFomEncoding::decodeAttributes(
      {{"Spatial", rvw}}, decoded);
  EXPECT_FALSE(truncatedRvw.success);
}
