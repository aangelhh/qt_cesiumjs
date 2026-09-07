#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include "geospatial/GeographicLibGeospatialService.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>

namespace tactical::hla {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr std::uint8_t kSpatialStatic = 1;
constexpr std::uint8_t kSpatialFpw = 2;
constexpr std::uint8_t kSpatialFpb = 6;

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

bool readUnsigned16(
    const ByteBuffer& input, std::size_t offset, std::uint16_t& value) {
  if (offset + 2 > input.size()) return false;
  value = static_cast<std::uint16_t>(
      (static_cast<std::uint16_t>(input[offset]) << 8U) |
      input[offset + 1]);
  return true;
}

bool readUnsigned32(
    const ByteBuffer& input, std::size_t offset, std::uint32_t& value) {
  if (offset + 4 > input.size()) return false;
  value = (static_cast<std::uint32_t>(input[offset]) << 24U) |
      (static_cast<std::uint32_t>(input[offset + 1]) << 16U) |
      (static_cast<std::uint32_t>(input[offset + 2]) << 8U) |
      input[offset + 3];
  return true;
}

bool readFloat32(const ByteBuffer& input, std::size_t offset, float& value) {
  std::uint32_t bits = 0;
  if (!readUnsigned32(input, offset, bits)) return false;
  std::memcpy(&value, &bits, sizeof(value));
  return std::isfinite(value);
}

bool readFloat64(const ByteBuffer& input, std::size_t offset, double& value) {
  if (offset + 8 > input.size()) return false;
  std::uint64_t bits = 0;
  for (std::size_t index = 0; index < 8; ++index) {
    bits = (bits << 8U) | input[offset + index];
  }
  std::memcpy(&value, &bits, sizeof(value));
  return std::isfinite(value);
}

const ByteBuffer* findValue(
    const std::vector<NamedValue>& attributes,
    const char* name) {
  const auto iterator = std::find_if(
      attributes.begin(), attributes.end(), [name](const NamedValue& value) {
        return value.name == name;
      });
  return iterator == attributes.end() ? nullptr : &iterator->value;
}

void decodeLocalAttitude(
    double latitudeDegrees,
    double longitudeDegrees,
    float psi,
    float theta,
    float phi,
    RprEntityState& entity) {
  const double latitude = latitudeDegrees * kPi / 180.0;
  const double longitude = longitudeDegrees * kPi / 180.0;
  const double sinPsi = std::sin(psi);
  const double cosPsi = std::cos(psi);
  const double sinTheta = std::sin(theta);
  const double cosTheta = std::cos(theta);
  const double sinPhi = std::sin(phi);
  const double cosPhi = std::cos(phi);
  const double ecefFromBody[3][3] = {
      {cosPsi * cosTheta,
       cosPsi * sinTheta * sinPhi - sinPsi * cosPhi,
       cosPsi * sinTheta * cosPhi + sinPsi * sinPhi},
      {sinPsi * cosTheta,
       sinPsi * sinTheta * sinPhi + cosPsi * cosPhi,
       sinPsi * sinTheta * cosPhi - cosPsi * sinPhi},
      {-sinTheta, cosTheta * sinPhi, cosTheta * cosPhi}};
  const double sinLat = std::sin(latitude);
  const double cosLat = std::cos(latitude);
  const double sinLon = std::sin(longitude);
  const double cosLon = std::cos(longitude);
  const double ecefFromNed[3][3] = {
      {-sinLat * cosLon, -sinLon, -cosLat * cosLon},
      {-sinLat * sinLon, cosLon, -cosLat * sinLon},
      {cosLat, 0.0, -sinLat}};
  double nedFromBody[3][3] = {};
  for (int row = 0; row < 3; ++row) {
    for (int column = 0; column < 3; ++column) {
      for (int index = 0; index < 3; ++index) {
        nedFromBody[row][column] +=
            ecefFromNed[index][row] * ecefFromBody[index][column];
      }
    }
  }
  entity.headingDegrees =
      std::atan2(nedFromBody[1][0], nedFromBody[0][0]) * 180.0 / kPi;
  if (entity.headingDegrees < 0.0) entity.headingDegrees += 360.0;
  entity.pitchDegrees = std::asin(std::clamp(
      -nedFromBody[2][0], -1.0, 1.0)) * 180.0 / kPi;
  entity.rollDegrees =
      std::atan2(nedFromBody[2][1], nedFromBody[2][2]) * 180.0 / kPi;
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

std::array<float, 3> ecefVelocity(const RprEntityState& entity) {
  const double latitude = entity.latitudeDegrees * kPi / 180.0;
  const double longitude = entity.longitudeDegrees * kPi / 180.0;
  const double heading = entity.headingDegrees * kPi / 180.0;
  const double horizontalSpeed = std::max(0.0, entity.speedKnots) * 0.514444;
  const double north = horizontalSpeed * std::cos(heading);
  const double east = horizontalSpeed * std::sin(heading);
  const double down = -entity.verticalSpeedMetersPerSecond;
  const double sinLat = std::sin(latitude);
  const double cosLat = std::cos(latitude);
  const double sinLon = std::sin(longitude);
  const double cosLon = std::cos(longitude);
  return {
      static_cast<float>(
          -sinLat * cosLon * north - sinLon * east - cosLat * cosLon * down),
      static_cast<float>(
          -sinLat * sinLon * north + cosLon * east - cosLat * sinLon * down),
      static_cast<float>(cosLat * north - sinLat * down)};
}

std::array<double, 3> ecefVelocityFromBody(
    float psi,
    float theta,
    float phi,
    const std::array<float, 3>& bodyVelocity) {
  const double sinPsi = std::sin(psi);
  const double cosPsi = std::cos(psi);
  const double sinTheta = std::sin(theta);
  const double cosTheta = std::cos(theta);
  const double sinPhi = std::sin(phi);
  const double cosPhi = std::cos(phi);
  const double ecefFromBody[3][3] = {
      {cosPsi * cosTheta,
       cosPsi * sinTheta * sinPhi - sinPsi * cosPhi,
       cosPsi * sinTheta * cosPhi + sinPsi * sinPhi},
      {sinPsi * cosTheta,
       sinPsi * sinTheta * sinPhi + cosPsi * cosPhi,
       sinPsi * sinTheta * cosPhi - cosPsi * sinPhi},
      {-sinTheta, cosTheta * sinPhi, cosTheta * cosPhi}};
  std::array<double, 3> result = {};
  for (int row = 0; row < 3; ++row) {
    for (int column = 0; column < 3; ++column) {
      result[row] += ecefFromBody[row][column] * bodyVelocity[column];
    }
  }
  return result;
}

void decodeVelocity(
    const ByteBuffer& value,
    std::uint8_t algorithm,
    double latitudeDegrees,
    double longitudeDegrees,
    float psi,
    float theta,
    float phi,
    RprEntityState& entity) {
  std::array<float, 3> encodedVelocity = {};
  if (!readFloat32(value, 48, encodedVelocity[0]) ||
      !readFloat32(value, 52, encodedVelocity[1]) ||
      !readFloat32(value, 56, encodedVelocity[2])) {
    return;
  }
  std::array<double, 3> worldVelocity = {
      encodedVelocity[0], encodedVelocity[1], encodedVelocity[2]};
  if (algorithm >= kSpatialFpb) {
    worldVelocity = ecefVelocityFromBody(
        psi, theta, phi, encodedVelocity);
  }
  const double latitude = latitudeDegrees * kPi / 180.0;
  const double longitude = longitudeDegrees * kPi / 180.0;
  const double cosLat = std::cos(latitude);
  const double sinLat = std::sin(latitude);
  const double cosLon = std::cos(longitude);
  const double sinLon = std::sin(longitude);
  const double north =
      -sinLat * cosLon * worldVelocity[0] -
      sinLat * sinLon * worldVelocity[1] + cosLat * worldVelocity[2];
  const double east =
      -sinLon * worldVelocity[0] + cosLon * worldVelocity[1];
  const double up =
      cosLat * cosLon * worldVelocity[0] +
      cosLat * sinLon * worldVelocity[1] + sinLat * worldVelocity[2];
  entity.speedKnots = std::hypot(north, east) / 0.514444;
  entity.verticalSpeedMetersPerSecond = up;
}

std::size_t spatialVariantMinimumSize(std::uint8_t algorithm) {
  switch (algorithm) {
    case 1: return 48; // Static
    case 2: // FPW
    case 6: // FPB
      return 60;
    case 3: // RPW
    case 5: // FVW
    case 7: // RPB
    case 9: // FVB
      return 72;
    case 4: // RVW
    case 8: // RVB
      return 84;
    default:
      return 0;
  }
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
  const bool moving = entity.speedKnots > 0.001 ||
      std::abs(entity.verticalSpeedMetersPerSecond) > 0.001;

  ByteBuffer output;
  output.reserve(moving ? 60 : 48);
  output.push_back(moving ? kSpatialFpw : kSpatialStatic);
  output.insert(output.end(), 7, 0); // Align the variant payload to Float64.
  appendFloat64(output, ecef.xMeters);
  appendFloat64(output, ecef.yMeters);
  appendFloat64(output, ecef.zMeters);
  output.push_back(0); // RPRboolean::False (not frozen).
  output.insert(output.end(), 3, 0); // Align OrientationStruct to Float32.
  appendFloat32(output, orientation[0]);
  appendFloat32(output, orientation[1]);
  appendFloat32(output, orientation[2]);
  if (moving) {
    const std::array<float, 3> velocity = ecefVelocity(entity);
    appendFloat32(output, velocity[0]);
    appendFloat32(output, velocity[1]);
    appendFloat32(output, velocity[2]);
  }
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
  const double safeSpeedKnots =
      std::isfinite(speedKnots) ? std::max(0.0, speedKnots) : 0.0;
  const double decimetersPerSecond = safeSpeedKnots * 0.514444 * 10.0;
  const auto encoded = static_cast<std::uint16_t>(std::lround(std::clamp(
      decimetersPerSecond,
      0.0,
      static_cast<double>(std::numeric_limits<std::uint16_t>::max()))));
  appendUnsigned16(output, encoded);
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

std::string RprFomEncoding::domainFromObjectClassName(
    const std::string& objectClassName) {
  if (objectClassName.size() >= 8 &&
      objectClassName.compare(objectClassName.size() - 8, 8, "Aircraft") == 0) {
    return "Air";
  }
  if (objectClassName.size() >= 13 &&
      objectClassName.compare(
          objectClassName.size() - 13, 13, "GroundVehicle") == 0) {
    return "Ground";
  }
  if (objectClassName.size() >= 13 &&
      objectClassName.compare(
          objectClassName.size() - 13, 13, "SurfaceVessel") == 0) {
    return "Surface";
  }
  if (objectClassName.size() >= 17 &&
      objectClassName.compare(
          objectClassName.size() - 17, 17, "SubmersibleVessel") == 0) {
    return "Subsurface";
  }
  if (objectClassName.size() >= 10 &&
      objectClassName.compare(
          objectClassName.size() - 10, 10, "Spacecraft") == 0) {
    return "Space";
  }
  return "Unknown";
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

Result RprFomEncoding::decodeAttributes(
    const std::vector<NamedValue>& attributes,
    RprEntityState& entity) {
  if (const ByteBuffer* value = findValue(attributes, "EntityType")) {
    if (value->size() < 8) return Result::failure("Invalid RPR EntityType");
    entity.entityKind = (*value)[0];
    entity.entityDomain = (*value)[1];
    if (!readUnsigned16(*value, 2, entity.countryCode)) {
      return Result::failure("Invalid RPR EntityType country");
    }
    entity.category = (*value)[4];
    entity.subcategory = (*value)[5];
    entity.specific = (*value)[6];
    entity.extra = (*value)[7];
  }
  if (const ByteBuffer* value = findValue(attributes, "Spatial")) {
    const std::uint8_t algorithm = value->empty() ? 0 : (*value)[0];
    const std::size_t minimumSize = spatialVariantMinimumSize(algorithm);
    if (minimumSize == 0 || value->size() < minimumSize) {
      return Result::failure("Unsupported RPR Spatial representation");
    }
    entity.deadReckoningAlgorithm = algorithm;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    float psi = 0.0F;
    float theta = 0.0F;
    float phi = 0.0F;
    if (!readFloat64(*value, 8, x) || !readFloat64(*value, 16, y) ||
        !readFloat64(*value, 24, z) || !readFloat32(*value, 36, psi) ||
        !readFloat32(*value, 40, theta) || !readFloat32(*value, 44, phi)) {
      return Result::failure("Invalid RPR Spatial payload");
    }
    const geospatial::GeodeticCoordinate position =
        geospatial::wgs84GeospatialService().ecefToGeodetic({x, y, z});
    entity.latitudeDegrees = position.latitude;
    entity.longitudeDegrees = position.longitude;
    entity.altitudeMeters = position.altitudeMeters;
    decodeLocalAttitude(
        position.latitude, position.longitude, psi, theta, phi, entity);
    if (algorithm != kSpatialStatic) {
      decodeVelocity(
          *value,
          algorithm,
          position.latitude,
          position.longitude,
          psi,
          theta,
          phi,
          entity);
    } else {
      entity.speedKnots = 0.0;
      entity.verticalSpeedMetersPerSecond = 0.0;
    }
  }
  if (const ByteBuffer* value = findValue(attributes, "DamageState")) {
    std::uint32_t damage = 0;
    if (!readUnsigned32(*value, 0, damage)) {
      return Result::failure("Invalid RPR DamageState");
    }
    entity.destroyed = damage >= 3;
    entity.damagePercent = damage == 0 ? 0.0 : (damage == 1 ? 10.0 :
        (damage == 2 ? 50.0 : 100.0));
  }
  if (const ByteBuffer* value = findValue(attributes, "ForceIdentifier")) {
    if (value->empty()) return Result::failure("Invalid RPR ForceIdentifier");
    entity.forceIdentifier = value->front();
  }
  if (const ByteBuffer* value = findValue(attributes, "LiveEntityMeasuredSpeed")) {
    std::uint16_t decimetersPerSecond = 0;
    if (!readUnsigned16(*value, 0, decimetersPerSecond)) {
      return Result::failure("Invalid RPR LiveEntityMeasuredSpeed");
    }
    entity.speedKnots = decimetersPerSecond / 10.0 / 0.514444;
  }
  if (const ByteBuffer* value = findValue(attributes, "Marking")) {
    if (value->size() < 2) return Result::failure("Invalid RPR Marking");
    const auto begin = value->begin() + 1;
    const auto end = std::find(begin, value->end(), 0);
    entity.name.assign(begin, end);
  }
  return Result::ok();
}

} // namespace tactical::hla
