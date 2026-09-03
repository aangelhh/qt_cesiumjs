#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"

#include "geospatial/GeographicLibGeospatialService.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace tactical::hla {
namespace {

const ByteBuffer* findValue(
    const std::vector<NamedValue>& values,
    const char* name) {
  const auto iterator = std::find_if(
      values.begin(), values.end(), [name](const NamedValue& value) {
        return value.name == name;
      });
  return iterator == values.end() ? nullptr : &iterator->value;
}

std::string stableRemoteId(const std::string& instanceName) {
  std::string result = "hla:";
  for (const char character : instanceName) {
    const bool valid =
        (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        (character >= '0' && character <= '9') || character == '-' ||
        character == '_' || character == '.';
    result.push_back(valid ? character : '_');
  }
  return result;
}

bool endsWith(const std::string& value, const char* suffix) {
  const std::string expected(suffix);
  return value.size() >= expected.size() &&
      value.compare(value.size() - expected.size(), expected.size(), expected) == 0;
}

HlaInboundAdapter::RemoteObjectKind objectKind(const std::string& className) {
  if (endsWith(className, "EmitterSystem")) {
    return HlaInboundAdapter::RemoteObjectKind::EmitterSystem;
  }
  if (endsWith(className, "RadarBeam")) {
    return HlaInboundAdapter::RemoteObjectKind::RadarBeam;
  }
  if (className.find(".Platform.") != std::string::npos) {
    return HlaInboundAdapter::RemoteObjectKind::Platform;
  }
  return HlaInboundAdapter::RemoteObjectKind::Unsupported;
}

std::string decodeString(const ByteBuffer* value) {
  if (!value) return {};
  const auto terminator = std::find(value->begin(), value->end(), 0);
  return std::string(value->begin(), terminator);
}

bool decodeFloat32(const ByteBuffer* value, double& output) {
  if (!value || value->size() != 4) return false;
  std::uint32_t bits = 0;
  for (const std::uint8_t byte : *value) {
    bits = (bits << 8U) | byte;
  }
  float decoded = 0.0F;
  std::memcpy(&decoded, &bits, sizeof(decoded));
  output = decoded;
  return std::isfinite(output);
}

bool decodeFloat64(const ByteBuffer& value, std::size_t offset, double& output) {
  if (value.size() < offset + 8) return false;
  std::uint64_t bits = 0;
  for (std::size_t index = offset; index < offset + 8; ++index) {
    bits = (bits << 8U) | value[index];
  }
  std::memcpy(&output, &bits, sizeof(output));
  return std::isfinite(output);
}

bool decodeWorldLocation(
    const ByteBuffer* value,
    double& latitude,
    double& longitude,
    double& altitude) {
  if (!value || value->size() != 24) return false;
  geospatial::EcefCoordinate ecef;
  if (!decodeFloat64(*value, 0, ecef.xMeters) ||
      !decodeFloat64(*value, 8, ecef.yMeters) ||
      !decodeFloat64(*value, 16, ecef.zMeters)) {
    return false;
  }
  const geospatial::GeographicLibGeospatialService service;
  const geospatial::GeodeticCoordinate geodetic = service.ecefToGeodetic(ecef);
  latitude = geodetic.latitude;
  longitude = geodetic.longitude;
  altitude = geodetic.altitudeMeters;
  return std::isfinite(latitude) && std::isfinite(longitude) &&
      std::isfinite(altitude);
}

std::string hexadecimal(const ByteBuffer* value) {
  if (!value) return {};
  static constexpr char digits[] = "0123456789abcdef";
  std::string output;
  output.reserve(value->size() * 2);
  for (const std::uint8_t byte : *value) {
    output.push_back(digits[(byte >> 4U) & 0x0fU]);
    output.push_back(digits[byte & 0x0fU]);
  }
  return output;
}

std::string decodeMunitionType(const ByteBuffer* value) {
  if (!value || value->size() < 5) return "Unknown";
  return value->at(4) == 2 ? "Bomb" : "Missile";
}

} // namespace

void HlaInboundAdapter::onObjectDiscovered(
    const RemoteObjectDiscovery& event) {
  const RemoteObjectKind kind = objectKind(event.objectClassName);
  _objects[event.instanceId] = {kind, event.instanceName};
  if (kind == RemoteObjectKind::EmitterSystem) {
    const std::string sensorId = stableRemoteId(event.instanceName);
    _emitters[event.instanceId] = {sensorId, {}};
    _emitterIdsByName[event.instanceName] = event.instanceId;
    return;
  }
  if (kind != RemoteObjectKind::Platform) return;
  RprEntityState state;
  state.stableId = stableRemoteId(event.instanceName);
  state.name = event.instanceName;
  state.domain = RprFomEncoding::domainFromObjectClassName(
      event.objectClassName);
  _entities[event.instanceId] = {std::move(state), true};
}

void HlaInboundAdapter::onObjectReflected(
    const RemoteObjectReflection& event) {
  const auto object = _objects.find(event.instanceId);
  if (object == _objects.end()) return;
  if (object->second.kind == RemoteObjectKind::EmitterSystem) {
    auto emitter = _emitters.find(event.instanceId);
    if (emitter == _emitters.end()) return;
    const std::string hostObject = decodeString(
        findValue(event.attributes, "HostObjectIdentifier"));
    if (!hostObject.empty()) {
      emitter->second.hostEntityId = stableRemoteId(hostObject);
    }
    if (!emitter->second.hostEntityId.empty()) {
      _sensorChanges.push_back({
          event.instanceId,
          emitter->second.sensorId,
          emitter->second.hostEntityId,
          false,
          false});
    }
    return;
  }
  if (object->second.kind == RemoteObjectKind::RadarBeam) {
    const std::string emitterName = decodeString(
        findValue(event.attributes, "EmitterSystemIdentifier"));
    const auto emitterId = _emitterIdsByName.find(emitterName);
    if (emitterId == _emitterIdsByName.end()) return;
    const auto emitter = _emitters.find(emitterId->second);
    if (emitter == _emitters.end() || emitter->second.hostEntityId.empty()) return;
    _beamEmitterIds[event.instanceId] = emitterId->second;
    RemoteSensorChange change;
    change.instanceId = emitterId->second;
    change.sensorId = emitter->second.sensorId;
    change.hostEntityId = emitter->second.hostEntityId;
    change.emitting = true;
    double azimuthSweepRadians = 0.0;
    double elevationSweepRadians = 0.0;
    decodeFloat32(findValue(event.attributes, "BeamAzimuthCenter"),
                  change.azimuthCenterDegrees);
    decodeFloat32(findValue(event.attributes, "BeamAzimuthSweep"),
                  azimuthSweepRadians);
    decodeFloat32(findValue(event.attributes, "BeamElevationCenter"),
                  change.elevationCenterDegrees);
    decodeFloat32(findValue(event.attributes, "BeamElevationSweep"),
                  elevationSweepRadians);
    constexpr double radiansToDegrees = 57.2957795130823208768;
    change.azimuthCenterDegrees *= radiansToDegrees;
    change.azimuthWidthDegrees = azimuthSweepRadians * 2.0 * radiansToDegrees;
    change.elevationCenterDegrees *= radiansToDegrees;
    change.elevationWidthDegrees = elevationSweepRadians * 2.0 * radiansToDegrees;
    decodeFloat32(findValue(event.attributes, "EmissionFrequency"),
                  change.frequencyHertz);
    decodeFloat32(findValue(event.attributes, "FrequencyRange"),
                  change.bandwidthHertz);
    decodeFloat32(findValue(event.attributes, "EffectiveRadiatedPower"),
                  change.effectiveRadiatedPowerDbm);
    const ByteBuffer* highDensityTrack =
        findValue(event.attributes, "HighDensityTrack");
    change.hasTracks = highDensityTrack && !highDensityTrack->empty() &&
        highDensityTrack->front() != 0;
    _sensorChanges.push_back(std::move(change));
    return;
  }
  if (object->second.kind != RemoteObjectKind::Platform) return;
  const auto iterator = _entities.find(event.instanceId);
  if (iterator == _entities.end()) return;
  if (RprFomEncoding::decodeAttributes(
          event.attributes, iterator->second.state).success) {
    iterator->second.dirty = true;
  }
}

void HlaInboundAdapter::onObjectRemoved(const RemoteObjectRemoval& event) {
  const auto object = _objects.find(event.instanceId);
  if (object == _objects.end()) return;
  if (object->second.kind == RemoteObjectKind::EmitterSystem) {
    const auto emitter = _emitters.find(event.instanceId);
    if (emitter != _emitters.end()) {
      _sensorChanges.push_back({
          event.instanceId,
          emitter->second.sensorId,
          emitter->second.hostEntityId,
          false,
          true});
      _emitterIdsByName.erase(object->second.instanceName);
      _emitters.erase(emitter);
    }
    _objects.erase(object);
    return;
  }
  if (object->second.kind == RemoteObjectKind::RadarBeam) {
    const auto beamEmitter = _beamEmitterIds.find(event.instanceId);
    if (beamEmitter != _beamEmitterIds.end()) {
      const auto emitter = _emitters.find(beamEmitter->second);
      if (emitter != _emitters.end()) {
        _sensorChanges.push_back({
            emitter->first,
            emitter->second.sensorId,
            emitter->second.hostEntityId,
            false,
            false});
      }
      _beamEmitterIds.erase(beamEmitter);
    }
    _objects.erase(object);
    return;
  }
  _objects.erase(object);
  const auto iterator = _entities.find(event.instanceId);
  if (iterator == _entities.end()) return;
  _removedEntities.push_back({event.instanceId, iterator->second.state, true});
  _entities.erase(iterator);
}

void HlaInboundAdapter::onInteractionReceived(
    const RemoteInteraction& event) {
  if (event.interactionClassName == "HLAinteractionRoot.StartResume") {
    _simulationControls.push_back(RemoteSimulationControl::StartResume);
    return;
  }
  if (event.interactionClassName == "HLAinteractionRoot.WeaponFire" ||
      event.interactionClassName == "HLAinteractionRoot.MunitionDetonation") {
    const ByteBuffer* identifier = findValue(event.parameters, "EventIdentifier");
    if (!identifier || identifier->empty()) return;
    const std::string eventId = event.interactionClassName + ":" +
        hexadecimal(identifier);
    if (!_seenWarfareEvents.insert(eventId).second) return;
    RemoteWarfareEvent warfare;
    warfare.kind = event.interactionClassName == "HLAinteractionRoot.WeaponFire"
        ? RemoteWarfareEventKind::WeaponFire
        : RemoteWarfareEventKind::MunitionDetonation;
    warfare.eventId = eventId;
    warfare.munitionType = decodeMunitionType(
        findValue(event.parameters, "MunitionType"));
    const char* locationName =
        warfare.kind == RemoteWarfareEventKind::WeaponFire
            ? "FiringLocation"
            : "DetonationLocation";
    if (decodeWorldLocation(
            findValue(event.parameters, locationName),
            warfare.latitudeDegrees,
            warfare.longitudeDegrees,
            warfare.altitudeMeters)) {
      _warfareEvents.push_back(std::move(warfare));
    }
    return;
  }
  if (event.interactionClassName != "HLAinteractionRoot.StopFreeze") return;
  const ByteBuffer* reason = findValue(event.parameters, "Reason");
  const bool terminal = reason && !reason->empty() &&
      (reason->front() == 2 || reason->front() == 6);
  _simulationControls.push_back(
      terminal ? RemoteSimulationControl::Stop
               : RemoteSimulationControl::Pause);
}

std::vector<RemoteSensorChange> HlaInboundAdapter::takeSensorChanges() {
  std::vector<RemoteSensorChange> result = std::move(_sensorChanges);
  _sensorChanges.clear();
  return result;
}

std::vector<RemoteWarfareEvent> HlaInboundAdapter::takeWarfareEvents() {
  std::vector<RemoteWarfareEvent> result = std::move(_warfareEvents);
  _warfareEvents.clear();
  return result;
}

std::vector<RemoteEntityChange> HlaInboundAdapter::takeEntityChanges() {
  std::vector<RemoteEntityChange> result = std::move(_removedEntities);
  _removedEntities.clear();
  for (auto& item : _entities) {
    if (!item.second.dirty) continue;
    result.push_back({item.first, item.second.state, false});
    item.second.dirty = false;
  }
  return result;
}

std::vector<RemoteSimulationControl>
HlaInboundAdapter::takeSimulationControls() {
  std::vector<RemoteSimulationControl> result =
      std::move(_simulationControls);
  _simulationControls.clear();
  return result;
}

} // namespace tactical::hla
