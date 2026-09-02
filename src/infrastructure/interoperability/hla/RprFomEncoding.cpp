#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include "geospatial/GeographicLibGeospatialService.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>

namespace tactical::hla {
namespace {

constexpr double kPi = 3.14159265358979323846;

void appendUnsigned16(ByteBuffer& output, std::uint16_t value) {
  output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void appendUnsigned32(ByteBuffer& output, std::uint32_t value) {
  output.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void appendFloat32(ByteBuffer& output, float value) {
  std::uint32_t bits = 0;
  static_assert(sizeof(bits) == sizeof(value), "Unexpected float size");
  std::memcpy(&bits, &value, sizeof(bits));
  appendUnsigned32(output, bits);
}

void appendFloat64(ByteBuffer& output, double value) {
  std::uint64_t bits = 0;
  static_assert(sizeof(bits) == sizeof(value), "Unexpected double size");
  std::memcpy(&bits, &value, sizeof(bits));
  for (int shift = 56; shift >= 0; shift -= 8) {
    output.push_back(static_cast<std::uint8_t>((bits >> shift) & 0xffU));
  }
}

std::array<float, 3> ecefEulerAngles(const RprEntityState& entity) {
  const double latitude = entity.latitudeDegrees * kPi / 180.0;
  const double longitude = entity.longitudeDegrees * kPi / 180.0;
  const double heading = entity.headingDegrees * kPi / 180.0;
  const double pitch = entity.pitchDegrees * kPi / 180.0;
  const double roll = entity.rollDegrees * kPi / 180.0;

  const double sinLat = std::sin(latitude);
  const double cosLat = std::cos(latitude);
  const double sinLon = std::sin(longitude);
  const double cosLon = std::cos(longitude);
  const double sinHeading = std::sin(heading);
  const double cosHeading = std::cos(heading);
  const double sinPitch = std::sin(pitch);
  const double cosPitch = std::cos(pitch);
  const double sinRoll = std::sin(roll);
  const double cosRoll = std::cos(roll);

  const double nedToBody[3][3] = {
      {cosHeading * cosPitch,
       cosHeading * sinPitch * sinRoll - sinHeading * cosRoll,
       cosHeading * sinPitch * cosRoll + sinHeading * sinRoll},
      {sinHeading * cosPitch,
       sinHeading * sinPitch * sinRoll + cosHeading * cosRoll,
       sinHeading * sinPitch * cosRoll - cosHeading * sinRoll},
      {-sinPitch, cosPitch * sinRoll, cosPitch * cosRoll}};
  const double ecefFromNed[3][3] = {
      {-sinLat * cosLon, -sinLon, -cosLat * cosLon},
      {-sinLat * sinLon, cosLon, -cosLat * sinLon},
      {cosLat, 0.0, -sinLat}};

  double ecefFromBody[3][3] = {};
  for (int row = 0; row < 3; ++row) {
    for (int column = 0; column < 3; ++column) {
      for (int index = 0; index < 3; ++index) {
        ecefFromBody[row][column] +=
            ecefFromNed[row][index] * nedToBody[index][column];
      }
    }
  }

  const double theta = std::asin(std::clamp(-ecefFromBody[2][0], -1.0, 1.0));
  const double psi = std::atan2(ecefFromBody[1][0], ecefFromBody[0][0]);
  const double phi = std::atan2(ecefFromBody[2][1], ecefFromBody[2][2]);
  return {
      static_cast<float>(psi),
      static_cast<float>(theta),
      static_cast<float>(phi)};
}

ByteBuffer encodeEntityType(const RprEntityState& entity) {
  ByteBuffer output;
  output.reserve(8);
  output.push_back(entity.entityKind);
  output.push_back(entity.entityDomain);
  appendUnsigned16(output, entity.countryCode);
  output.push_back(entity.category);
  output.push_back(entity.subcategory);
  output.push_back(entity.specific);
  output.push_back(entity.extra);
  return output;
}

ByteBuffer encodeEntityIdentifier(
    std::uint16_t siteId,
    std::uint16_t applicationId,
    std::uint16_t entityNumber) {
  ByteBuffer output;
  output.reserve(6);
  appendUnsigned16(output, siteId);
  appendUnsigned16(output, applicationId);
  appendUnsigned16(output, entityNumber);
  return output;
}

ByteBuffer encodeSpatial(const RprEntityState& entity) {
  const geospatial::GeographicLibGeospatialService geospatialService;
  const geospatial::EcefCoordinate ecef = geospatialService.geodeticToEcef({
      entity.latitudeDegrees,
      entity.longitudeDegrees,
      entity.altitudeMeters});
  const std::array<float, 3> orientation = ecefEulerAngles(entity);

  ByteBuffer output;
  output.reserve(48);
  output.push_back(1); // DeadReckoningAlgorithmEnum8::Static
  output.insert(output.end(), 7, 0); // Align SpatialStaticStruct to Float64.
  appendFloat64(output, ecef.xMeters);
  appendFloat64(output, ecef.yMeters);
  appendFloat64(output, ecef.zMeters);
  output.push_back(0); // RPRboolean::False (not frozen).
  output.insert(output.end(), 3, 0); // Align OrientationStruct to Float32.
  appendFloat32(output, orientation[0]);
  appendFloat32(output, orientation[1]);
  appendFloat32(output, orientation[2]);
  return output;
}

ByteBuffer encodeMarking(const std::string& name) {
  ByteBuffer output(12, 0);
  output[0] = 1; // MarkingEncodingEnum8::ASCII
  const std::size_t count = std::min<std::size_t>(11, name.size());
  std::copy_n(name.begin(), count, output.begin() + 1);
  return output;
}

ByteBuffer encodeDamage(const RprEntityState& entity) {
  std::uint32_t value = 0;
  if (entity.destroyed || entity.damagePercent >= 100.0) value = 3;
  else if (entity.damagePercent >= 30.0) value = 2;
  else if (entity.damagePercent > 0.0) value = 1;
  ByteBuffer output;
  appendUnsigned32(output, value);
  return output;
}

ByteBuffer encodeMeasuredSpeed(double speedKnots) {
  ByteBuffer output;
  appendFloat32(output, static_cast<float>(speedKnots * 0.514444));
  return output;
}

} // namespace

std::string RprFomEncoding::objectClassName(const std::string& domain) {
  const std::string prefix =
      "HLAobjectRoot.BaseEntity.PhysicalEntity.Platform.";
  if (domain == "Air") return prefix + "Aircraft";
  if (domain == "Ground") return prefix + "GroundVehicle";
  if (domain == "Surface") return prefix + "SurfaceVessel";
  if (domain == "Subsurface") return prefix + "SubmersibleVessel";
  if (domain == "Space") return prefix + "Spacecraft";
  return prefix + "MultiDomainPlatform";
}

std::string RprFomEncoding::objectInstanceName(const std::string& stableId) {
  std::string result = "qttest.";
  for (const char character : stableId) {
    const bool valid =
        (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        (character >= '0' && character <= '9') || character == '-' ||
        character == '_';
    result.push_back(valid ? character : '_');
  }
  return result;
}

std::vector<std::string> RprFomEncoding::publishedAttributeNames() {
  return {
      "EntityType",
      "EntityIdentifier",
      "Spatial",
      "DamageState",
      "ForceIdentifier",
      "LiveEntityMeasuredSpeed",
      "Marking"};
}

std::vector<NamedValue> RprFomEncoding::encodeAttributes(
    const RprEntityState& entity,
    std::uint16_t siteId,
    std::uint16_t applicationId,
    std::uint16_t entityNumber) {
  return {
      {"EntityType", encodeEntityType(entity)},
      {"EntityIdentifier", encodeEntityIdentifier(
           siteId, applicationId, entityNumber)},
      {"Spatial", encodeSpatial(entity)},
      {"DamageState", encodeDamage(entity)},
      {"ForceIdentifier", {entity.forceIdentifier}},
      {"LiveEntityMeasuredSpeed", encodeMeasuredSpeed(entity.speedKnots)},
      {"Marking", encodeMarking(entity.name)}};
}

} // namespace tactical::hla
