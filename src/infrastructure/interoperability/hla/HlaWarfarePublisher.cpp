#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"

#include "geospatial/GeographicLibGeospatialService.h"
#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <cmath>
#include <cstring>
#include <unordered_set>

namespace tactical::hla {
namespace {

constexpr double kPi = 3.14159265358979323846;
const char* kMunitionClass =
    "HLAobjectRoot.BaseEntity.PhysicalEntity.Munition";

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

ByteBuffer encodeWorldLocation(const RprMunitionDetonationState& detonation) {
  RprWeaponFireState location;
  location.latitudeDegrees = detonation.latitudeDegrees;
  location.longitudeDegrees = detonation.longitudeDegrees;
  location.altitudeMeters = detonation.altitudeMeters;
  return encodeWorldLocation(location);
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

ByteBuffer encodeObjectIdentifier(const std::string& instanceName) {
  ByteBuffer output(instanceName.begin(), instanceName.end());
  output.push_back(0);
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

std::string munitionObjectInstanceName(const std::string& stableId) {
  return RprFomEncoding::objectInstanceName("munition-" + stableId);
}

RprEntityState munitionEntityState(const RprWeaponFireState& munition) {
  RprEntityState state;
  state.stableId = "munition-" + munition.stableId;
  state.name = munition.stableId;
  state.domain = "Air";
  state.entityKind = 2;
  state.entityDomain = 2;
  state.category = munition.munitionType == "Bomb" ? 2 : 1;
  state.latitudeDegrees = munition.latitudeDegrees;
  state.longitudeDegrees = munition.longitudeDegrees;
  state.altitudeMeters = munition.altitudeMeters;
  state.headingDegrees = munition.headingDegrees;
  state.pitchDegrees = munition.pitchDegrees;
  state.speedKnots = munition.speedMetersPerSecond * 1.9438444924406;
  return state;
}

} // namespace

HlaWarfarePublisher::HlaWarfarePublisher(HlaRuntime& runtime)
    : _runtime(runtime) {}

Result HlaWarfarePublisher::synchronize(
    const std::vector<RprWeaponFireState>& activeMunitions) {
  std::unordered_set<std::string> activeIds;
  for (const RprWeaponFireState& munition : activeMunitions) {
    if (munition.stableId.empty()) continue;
    activeIds.insert(munition.stableId);
    Result result = this->ensurePublished();
    if (!result.success) return result;
    auto registered = _registeredMunitions.find(munition.stableId);
    if (registered == _registeredMunitions.end()) {
      RegisteredMunition value;
      value.entityNumber = _nextMunitionEntityNumber++;
      if (_nextMunitionEntityNumber == 0) _nextMunitionEntityNumber = 32768;
      result = _runtime.registerObjectInstance(
          kMunitionClass,
          munitionObjectInstanceName(munition.stableId),
          value.instanceId);
      if (!result.success) return result;
      registered = _registeredMunitions.emplace(
          munition.stableId, value).first;
    }
    result = _runtime.updateObjectAttributes(
        registered->second.instanceId,
        RprFomEncoding::encodeAttributes(
            munitionEntityState(munition),
            1,
            1,
            registered->second.entityNumber));
    if (!result.success) return result;
    if (_sentMunitionIds.count(munition.stableId) == 0) {
      result = _runtime.sendInteraction(
          "HLAinteractionRoot.WeaponFire",
          this->encodeWeaponFire(munition, _nextEventNumber));
      if (!result.success) return result;
      _fireCorrelations[munition.stableId] = {
          _nextEventNumber,
          munition.firingObjectInstanceName,
          munition.targetObjectInstanceName};
      _sentMunitionIds.insert(munition.stableId);
      if (++_nextEventNumber == 0) _nextEventNumber = 1;
    }
  }
  for (auto iterator = _registeredMunitions.begin();
       iterator != _registeredMunitions.end();) {
    if (activeIds.count(iterator->first) != 0) {
      ++iterator;
      continue;
    }
    const Result result = _runtime.deleteObjectInstance(
        iterator->second.instanceId);
    if (!result.success) return result;
    iterator = _registeredMunitions.erase(iterator);
  }
  return Result::ok();
}

Result HlaWarfarePublisher::synchronizeDetonations(
    const std::vector<RprMunitionDetonationState>& detonations) {
  for (const RprMunitionDetonationState& detonation : detonations) {
    if (detonation.effectId.empty() ||
        _sentDetonationIds.count(detonation.effectId) != 0) {
      continue;
    }
    if (!_detonationPublished) {
      const Result publishResult = _runtime.publishInteractionClass(
          "HLAinteractionRoot.MunitionDetonation");
      if (!publishResult.success) return publishResult;
      _detonationPublished = true;
    }
    std::uint16_t eventNumber = _nextEventNumber;
    const auto correlation = _fireCorrelations.find(
        detonation.munitionStableId);
    if (correlation != _fireCorrelations.end()) {
      eventNumber = correlation->second.eventNumber;
    }
    const Result result = _runtime.sendInteraction(
        "HLAinteractionRoot.MunitionDetonation",
        this->encodeDetonation(detonation, eventNumber));
    if (!result.success) return result;
    _sentDetonationIds.insert(detonation.effectId);
    const bool standaloneDetonation = correlation == _fireCorrelations.end();
    _fireCorrelations.erase(detonation.munitionStableId);
    if (standaloneDetonation &&
        ++_nextEventNumber == 0) {
      _nextEventNumber = 1;
    }
  }
  return Result::ok();
}

std::size_t HlaWarfarePublisher::sentWeaponFireCount() const {
  return _sentMunitionIds.size();
}

std::size_t HlaWarfarePublisher::sentDetonationCount() const {
  return _sentDetonationIds.size();
}

std::size_t HlaWarfarePublisher::registeredMunitionCount() const {
  return _registeredMunitions.size();
}

Result HlaWarfarePublisher::removeAll() {
  for (const auto& item : _registeredMunitions) {
    const Result result = _runtime.deleteObjectInstance(
        item.second.instanceId);
    if (!result.success) return result;
  }
  _registeredMunitions.clear();
  return Result::ok();
}

Result HlaWarfarePublisher::ensurePublished() {
  if (_published) return Result::ok();
  Result result =
      _runtime.publishInteractionClass("HLAinteractionRoot.WeaponFire");
  if (!result.success) return result;
  result = _runtime.publishObjectClass(
      kMunitionClass, RprFomEncoding::publishedAttributeNames());
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
      {"FiringObjectIdentifier", encodeObjectIdentifier(
           munition.firingObjectInstanceName)},
      {"FuseType", encodeUnsigned16(0)},
      {"InitialVelocityVector", encodeVelocity(munition)},
      {"MunitionObjectIdentifier", encodeObjectIdentifier(
           munitionObjectInstanceName(munition.stableId))},
      {"MunitionType", encodeMunitionType(munition.munitionType)},
      {"QuantityFired", encodeUnsigned16(1)},
      {"RateOfFire", encodeUnsigned16(0)},
      {"TargetObjectIdentifier", encodeObjectIdentifier(
           munition.targetObjectInstanceName)},
      {"WarheadType", encodeUnsigned16(0)}};
}

std::vector<NamedValue> HlaWarfarePublisher::encodeDetonation(
    const RprMunitionDetonationState& detonation,
    std::uint16_t eventNumber) const {
  RprWeaponFireState munition;
  munition.munitionType = detonation.munitionType;
  const auto correlation = _fireCorrelations.find(
      detonation.munitionStableId);
  const std::string firingObjectInstanceName =
      correlation == _fireCorrelations.end()
          ? std::string{}
          : correlation->second.firingObjectInstanceName;
  const std::string targetObjectInstanceName =
      correlation == _fireCorrelations.end()
          ? std::string{}
          : correlation->second.targetObjectInstanceName;
  return {
      {"DetonationLocation", encodeWorldLocation(detonation)},
      {"DetonationResultCode", {1}},
      {"EventIdentifier", encodeEventIdentifier(eventNumber)},
      {"FiringObjectIdentifier", encodeObjectIdentifier(
           firingObjectInstanceName)},
      {"FinalVelocityVector", ByteBuffer(12, 0)},
      {"FuseType", encodeUnsigned16(0)},
      {"MunitionObjectIdentifier", encodeObjectIdentifier(
           munitionObjectInstanceName(detonation.munitionStableId))},
      {"MunitionType", encodeMunitionType(munition.munitionType)},
      {"QuantityFired", encodeUnsigned16(1)},
      {"RateOfFire", encodeUnsigned16(0)},
      {"RelativeDetonationLocation", ByteBuffer(12, 0)},
      {"TargetObjectIdentifier", encodeObjectIdentifier(
           targetObjectInstanceName)},
      {"WarheadType", encodeUnsigned16(0)}};
}

} // namespace tactical::hla
