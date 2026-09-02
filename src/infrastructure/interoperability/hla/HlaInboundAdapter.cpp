#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"

#include <algorithm>

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

} // namespace

void HlaInboundAdapter::onObjectDiscovered(
    const RemoteObjectDiscovery& event) {
  RprEntityState state;
  state.stableId = stableRemoteId(event.instanceName);
  state.name = event.instanceName;
  state.domain = RprFomEncoding::domainFromObjectClassName(
      event.objectClassName);
  _entities[event.instanceId] = {std::move(state), true};
}

void HlaInboundAdapter::onObjectReflected(
    const RemoteObjectReflection& event) {
  const auto iterator = _entities.find(event.instanceId);
  if (iterator == _entities.end()) return;
  if (RprFomEncoding::decodeAttributes(
          event.attributes, iterator->second.state).success) {
    iterator->second.dirty = true;
  }
}

void HlaInboundAdapter::onObjectRemoved(const RemoteObjectRemoval& event) {
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
  if (event.interactionClassName != "HLAinteractionRoot.StopFreeze") return;
  const ByteBuffer* reason = findValue(event.parameters, "Reason");
  const bool terminal = reason && !reason->empty() &&
      (reason->front() == 2 || reason->front() == 6);
  _simulationControls.push_back(
      terminal ? RemoteSimulationControl::Stop
               : RemoteSimulationControl::Pause);
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
