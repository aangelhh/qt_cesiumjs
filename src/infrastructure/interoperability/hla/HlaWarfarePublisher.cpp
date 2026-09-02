#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"

#include "geospatial/GeographicLibGeospatialService.h"

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
  std::memcpy(&bits, &value, sizeof(bits));
  appendUnsigned32(output, bits);
}

void appendFloat64(ByteBuffer& output, double value) {
  std::uint64_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  for (int shift = 56; shift >= 0; shift -= 8) {
    output.push_back(static_cast<std::uint8_t>((bits >> shift) & 0xffU));
  }
}

ByteBuffer encodeEventIdentifier(std::uint16_t eventNumber) {
  ByteBuffer output;
  appendUnsigned16(output, 1); // Site ID
  appendUnsigned16(output, 1); // Application ID
  appendUnsigned16(output, eventNumber);
  return output;
}

ByteBuffer encodeWorldLocation(const RprWeaponFireState& munition) {
  const geospatial::GeographicLibGeospatialService service;
  const geospatial::EcefCoordinate ecef = service.geodeticToEcef({
      munition.latitudeDegrees,
      munition.longitudeDegrees,
      munition.altitudeMeters});
  ByteBuffer output;
  appendFloat64(output, ecef.xMeters);
  appendFloat64(output, ecef.yMeters);
  appendFloat64(output, ecef.zMeters);
  return output;
}

ByteBuffer encodeVelocity(const RprWeaponFireState& munition) {
  const double latitude = munition.latitudeDegrees * kPi / 180.0;
  const double longitude = munition.longitudeDegrees * kPi / 180.0;
  const double heading = munition.headingDegrees * kPi / 180.0;
  const double pitch = munition.pitchDegrees * kPi / 180.0;
  const double north =
      munition.speedMetersPerSecond * std::cos(pitch) * std::cos(heading);
  const double east =
      munition.speedMetersPerSecond * std::cos(pitch) * std::sin(heading);
  const double down = -munition.speedMetersPerSecond * std::sin(pitch);
  const double x = -std::sin(latitude) * std::cos(longitude) * north -
      std::sin(longitude) * east -
      std::cos(latitude) * std::cos(longitude) * down;
  const double y = -std::sin(latitude) * std::sin(longitude) * north +
      std::cos(longitude) * east -
      std::cos(latitude) * std::sin(longitude) * down;
  const double z = std::cos(latitude) * north - std::sin(latitude) * down;
  ByteBuffer output;
  appendFloat32(output, static_cast<float>(x));
  appendFloat32(output, static_cast<float>(y));
  appendFloat32(output, static_cast<float>(z));
  return output;
}

ByteBuffer encodeMunitionType(const std::string& munitionType) {
  ByteBuffer output;
  output.push_back(2); // EntityKind::Munition
  output.push_back(2); // Domain::Air
  appendUnsigned16(output, 0);
  output.push_back(munitionType == "Bomb" ? 2 : 1);
  output.insert(output.end(), 3, 0);
  return output;
}

ByteBuffer encodeUnsigned16(std::uint16_t value) {
  ByteBuffer output;
  appendUnsigned16(output, value);
  return output;
}

ByteBuffer encodeUnsigned32(std::uint32_t value) {
  ByteBuffer output;
  appendUnsigned32(output, value);
  return output;
}

ByteBuffer encodeFloat32(float value) {
  ByteBuffer output;
  appendFloat32(output, value);
  return output;
}

} // namespace

HlaWarfarePublisher::HlaWarfarePublisher(HlaRuntime& runtime)
    : _runtime(runtime) {}

Result HlaWarfarePublisher::synchronize(
    const std::vector<RprWeaponFireState>& activeMunitions) {
  for (const RprWeaponFireState& munition : activeMunitions) {
    if (munition.stableId.empty() ||
        _sentMunitionIds.count(munition.stableId) != 0) {
      continue;
    }
    Result result = this->ensurePublished();
    if (!result.success) return result;
    result = _runtime.sendInteraction(
        "HLAinteractionRoot.WeaponFire",
        this->encodeWeaponFire(munition, _nextEventNumber));
    if (!result.success) return result;
    _sentMunitionIds.insert(munition.stableId);
    if (++_nextEventNumber == 0) _nextEventNumber = 1;
  }
  return Result::ok();
}

std::size_t HlaWarfarePublisher::sentWeaponFireCount() const {
  return _sentMunitionIds.size();
}

Result HlaWarfarePublisher::ensurePublished() {
  if (_published) return Result::ok();
  const Result result =
      _runtime.publishInteractionClass("HLAinteractionRoot.WeaponFire");
  if (result.success) _published = true;
  return result;
}

std::vector<NamedValue> HlaWarfarePublisher::encodeWeaponFire(
    const RprWeaponFireState& munition,
    std::uint16_t eventNumber) const {
  return {
      {"EventIdentifier", encodeEventIdentifier(eventNumber)},
      {"FireControlSolutionRange", encodeFloat32(0.0F)},
      {"FireMissionIndex", encodeUnsigned32(eventNumber)},
      {"FiringLocation", encodeWorldLocation(munition)},
      {"FuseType", encodeUnsigned16(0)},
      {"InitialVelocityVector", encodeVelocity(munition)},
      {"MunitionType", encodeMunitionType(munition.munitionType)},
      {"QuantityFired", encodeUnsigned16(1)},
      {"RateOfFire", encodeUnsigned16(0)},
      {"WarheadType", encodeUnsigned16(0)}};
}

} // namespace tactical::hla
