#include "infrastructure/interoperability/hla/HlaSensorPublisher.h"

#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <unordered_set>

namespace tactical::hla {
namespace {

constexpr double kPi = 3.14159265358979323846;
const char* kEmitterSystemClass = "HLAobjectRoot.EmbeddedSystem.EmitterSystem";
const char* kRadarBeamClass = "HLAobjectRoot.EmitterBeam.RadarBeam";

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
  output.push_back(static_cast<std::uint8_t>((bits >> 24U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((bits >> 16U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((bits >> 8U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>(bits & 0xffU));
}

ByteBuffer float32(double value) {
  ByteBuffer output;
  appendFloat32(output, static_cast<float>(value));
  return output;
}

ByteBuffer unsigned16(std::uint16_t value) {
  ByteBuffer output;
  appendUnsigned16(output, value);
  return output;
}

ByteBuffer entityIdentifier(std::uint16_t entityNumber) {
  ByteBuffer output;
  appendUnsigned16(output, 1);
  appendUnsigned16(output, 1);
  appendUnsigned16(output, entityNumber);
  return output;
}

ByteBuffer relativePosition() {
  ByteBuffer output;
  output.reserve(12);
  appendFloat32(output, 0.0F);
  appendFloat32(output, 0.0F);
  appendFloat32(output, 0.0F);
  return output;
}

ByteBuffer objectIdentifier(const std::string& instanceName) {
  ByteBuffer output(instanceName.begin(), instanceName.end());
  output.push_back(0);
  return output;
}

ByteBuffer objectIdentifierArray(
    const std::vector<std::string>& instanceNames) {
  ByteBuffer output;
  appendUnsigned32(
      output, static_cast<std::uint32_t>(instanceNames.size()));
  for (const std::string& instanceName : instanceNames) {
    output.insert(output.end(), instanceName.begin(), instanceName.end());
    output.push_back(0);
  }
  return output;
}

ByteBuffer eventIdentifier(std::uint16_t eventNumber) {
  return entityIdentifier(eventNumber);
}

std::string sensorKey(const RprSensorState& sensor) {
  return sensor.entityStableId + ":" + sensor.sensorId;
}

std::string systemInstanceName(const RprSensorState& sensor) {
  return RprFomEncoding::objectInstanceName(
      "sensor-" + sensor.entityStableId + "-" + sensor.sensorId);
}

std::string beamInstanceName(const RprSensorState& sensor) {
  return RprFomEncoding::objectInstanceName(
      "radar-beam-" + sensor.entityStableId + "-" + sensor.sensorId);
}

} // namespace

HlaSensorPublisher::HlaSensorPublisher(HlaRuntime& runtime)
    : _runtime(runtime) {}

Result HlaSensorPublisher::synchronize(
    const std::vector<RprSensorState>& sensors) {
  std::unordered_set<std::string> activeKeys;
  for (const RprSensorState& sensor : sensors) {
    if (!sensor.enabled || sensor.entityStableId.empty() ||
        sensor.sensorId.empty()) {
      continue;
    }
    Result result = this->ensurePublished();
    if (!result.success) return result;
    const std::string key = sensorKey(sensor);
    activeKeys.insert(key);
    auto iterator = _registeredSensors.find(key);
    if (iterator == _registeredSensors.end()) {
      RegisteredSensor registered;
      registered.entityNumber = _nextEntityNumber++;
      result = _runtime.registerObjectInstance(
          kEmitterSystemClass, systemInstanceName(sensor),
          registered.systemInstanceId);
      if (!result.success) return result;
      iterator = _registeredSensors.emplace(key, registered).first;
    }
    result = _runtime.updateObjectAttributes(
        iterator->second.systemInstanceId,
        this->encodeSystem(sensor, iterator->second));
    if (!result.success) return result;

    if (sensor.emitting) {
      if (iterator->second.beamInstanceId == 0) {
        result = _runtime.registerObjectInstance(
            kRadarBeamClass, beamInstanceName(sensor),
            iterator->second.beamInstanceId);
        if (!result.success) return result;
      }
      result = _runtime.updateObjectAttributes(
          iterator->second.beamInstanceId,
          this->encodeBeam(sensor, iterator->second));
      if (!result.success) return result;
    } else if (iterator->second.beamInstanceId != 0) {
      result = _runtime.deleteObjectInstance(iterator->second.beamInstanceId);
      if (!result.success) return result;
      iterator->second.beamInstanceId = 0;
    }
  }

  for (auto iterator = _registeredSensors.begin();
       iterator != _registeredSensors.end();) {
    if (activeKeys.count(iterator->first) != 0) {
      ++iterator;
      continue;
    }
    if (iterator->second.beamInstanceId != 0) {
      const Result result =
          _runtime.deleteObjectInstance(iterator->second.beamInstanceId);
      if (!result.success) return result;
    }
    const Result result =
        _runtime.deleteObjectInstance(iterator->second.systemInstanceId);
    if (!result.success) return result;
    iterator = _registeredSensors.erase(iterator);
  }
  return Result::ok();
}

Result HlaSensorPublisher::removeAll() {
  for (const auto& item : _registeredSensors) {
    if (item.second.beamInstanceId != 0) {
      const Result result = _runtime.deleteObjectInstance(
          item.second.beamInstanceId);
      if (!result.success) return result;
    }
    const Result result = _runtime.deleteObjectInstance(
        item.second.systemInstanceId);
    if (!result.success) return result;
  }
  _registeredSensors.clear();
  return Result::ok();
}

std::size_t HlaSensorPublisher::registeredSensorCount() const {
  return _registeredSensors.size();
}

Result HlaSensorPublisher::ensurePublished() {
  if (_published) return Result::ok();
  Result result = _runtime.publishObjectClass(
      kEmitterSystemClass,
      {"EntityIdentifier", "HostObjectIdentifier", "RelativePosition",
       "EmitterFunctionCode", "EmitterType", "EmitterIndex",
       "EventIdentifier"});
  if (!result.success) return result;
  result = _runtime.publishObjectClass(
      kRadarBeamClass,
      {"BeamAzimuthCenter", "BeamAzimuthSweep", "BeamElevationCenter",
       "BeamElevationSweep", "BeamFunctionCode", "BeamIdentifier",
       "BeamParameterIndex", "EffectiveRadiatedPower",
       "EmissionFrequency", "EmitterSystemIdentifier", "EventIdentifier",
       "FrequencyRange", "SweepSynch", "HighDensityTrack",
       "TrackObjectIdentifiers"});
  if (result.success) _published = true;
  return result;
}

std::vector<NamedValue> HlaSensorPublisher::encodeSystem(
    const RprSensorState& sensor,
    const RegisteredSensor& registered) const {
  const auto emitterIndex = static_cast<std::uint8_t>(
      ((registered.entityNumber - 1U) % 255U) + 1U);
  return {
      {"EntityIdentifier", entityIdentifier(registered.entityNumber)},
      {"HostObjectIdentifier", objectIdentifier(
           RprFomEncoding::objectInstanceName(sensor.entityStableId))},
      {"RelativePosition", relativePosition()},
      {"EmitterFunctionCode", {1}},
      {"EmitterType", unsigned16(0)},
      {"EmitterIndex", {emitterIndex}},
      {"EventIdentifier", eventIdentifier(registered.entityNumber)}};
}

std::vector<NamedValue> HlaSensorPublisher::encodeBeam(
    const RprSensorState& sensor,
    const RegisteredSensor& registered) const {
  const double powerDbm = sensor.peakPowerWatts > 0.0
      ? 10.0 * std::log10(sensor.peakPowerWatts * 1000.0)
      : 0.0;
  const auto beamIdentifier = static_cast<std::uint8_t>(
      ((registered.entityNumber - 1U) % 255U) + 1U);
  return {
      {"BeamAzimuthCenter", float32(
           sensor.azimuthCenterDegrees * kPi / 180.0)},
      {"BeamAzimuthSweep", float32(
           sensor.azimuthWidthDegrees * 0.5 * kPi / 180.0)},
      {"BeamElevationCenter", float32(
           sensor.elevationCenterDegrees * kPi / 180.0)},
      {"BeamElevationSweep", float32(
           sensor.elevationWidthDegrees * 0.5 * kPi / 180.0)},
      {"BeamFunctionCode", {static_cast<std::uint8_t>(
           sensor.hasTracks ? 5 : 1)}},
      {"BeamIdentifier", {beamIdentifier}},
      {"BeamParameterIndex", unsigned16(0)},
      {"EffectiveRadiatedPower", float32(powerDbm)},
      {"EmissionFrequency", float32(sensor.frequencyHertz)},
      {"EmitterSystemIdentifier", objectIdentifier(systemInstanceName(sensor))},
      {"EventIdentifier", eventIdentifier(registered.entityNumber)},
      {"FrequencyRange", float32(sensor.bandwidthHertz)},
      {"SweepSynch", float32(0.0)},
      {"HighDensityTrack", {static_cast<std::uint8_t>(
           sensor.hasTracks ? 1 : 0)}},
      {"TrackObjectIdentifiers", objectIdentifierArray(
           sensor.trackedObjectInstanceNames)}};
}

} // namespace tactical::hla
