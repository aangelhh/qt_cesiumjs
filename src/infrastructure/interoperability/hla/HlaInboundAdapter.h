#pragma once

#include "infrastructure/interoperability/hla/HlaTypes.h"
#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <unordered_map>
#include <vector>

namespace tactical::hla {

enum class RemoteSimulationControl {
  StartResume,
  Pause,
  Stop
};

struct RemoteEntityChange {
  ObjectInstanceId instanceId = 0;
  RprEntityState state;
  bool removed = false;
};

class HlaInboundAdapter final : public IHlaEventSink {
public:
  void onObjectDiscovered(const RemoteObjectDiscovery& event) override;
  void onObjectReflected(const RemoteObjectReflection& event) override;
  void onObjectRemoved(const RemoteObjectRemoval& event) override;
  void onInteractionReceived(const RemoteInteraction& event) override;

  std::vector<RemoteEntityChange> takeEntityChanges();
  std::vector<RemoteSimulationControl> takeSimulationControls();

private:
  struct RemoteEntity {
    RprEntityState state;
    bool dirty = false;
  };

  std::unordered_map<ObjectInstanceId, RemoteEntity> _entities;
  std::vector<RemoteEntityChange> _removedEntities;
  std::vector<RemoteSimulationControl> _simulationControls;
};

} // namespace tactical::hla
