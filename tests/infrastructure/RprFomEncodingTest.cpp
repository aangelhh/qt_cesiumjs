#include <gtest/gtest.h>

#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <algorithm>

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
  EXPECT_EQ(findAttribute(attributes, "EntityIdentifier")->value.size(), 6U);
  EXPECT_EQ(findAttribute(attributes, "Spatial")->value.size(), 48U);
  EXPECT_EQ(findAttribute(attributes, "DamageState")->value.size(), 4U);
  EXPECT_EQ(findAttribute(attributes, "ForceIdentifier")->value.size(), 1U);
  EXPECT_EQ(findAttribute(attributes, "LiveEntityMeasuredSpeed")->value.size(), 4U);
  EXPECT_EQ(findAttribute(attributes, "Marking")->value.size(), 12U);
  EXPECT_EQ(findAttribute(attributes, "Marking")->value.front(), 1U);
}
