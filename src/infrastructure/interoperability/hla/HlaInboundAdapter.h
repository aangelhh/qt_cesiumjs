#pragma once

#include "infrastructure/interoperability/hla/HlaTypes.h"
#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <unordered_map>
#include <unordered_set>
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

struct RemoteMunitionChange {
  ObjectInstanceId instanceId = 0;
  RprEntityState state;
  bool removed = false;
};

struct RemoteSensorChange {
  ObjectInstanceId instanceId = 0;
  std::string sensorId;
  std::string hostEntityId;
  bool emitting = false;
  bool removed = false;
  double azimuthCenterDegrees = 0.0;
  double azimuthWidthDegrees = 360.0;
  double elevationCenterDegrees = 0.0;
  double elevationWidthDegrees = 90.0;
  double frequencyHertz = 0.0;
  double bandwidthHertz = 0.0;
  double effectiveRadiatedPowerDbm = 0.0;
  bool hasTracks = false;
  std::vector<std::string> trackedObjectInstanceNames;
};

enum class RemoteWarfareEventKind {
  WeaponFire,
  MunitionDetonation
};

struct RemoteWarfareEvent {
  RemoteWarfareEventKind kind = RemoteWarfareEventKind::WeaponFire;
  std::string eventId;
  std::string munitionType;
  std::string firingObjectInstanceName;
  std::string munitionObjectInstanceName;
  std::string targetObjectInstanceName;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
};

struct RemoteSynchronizationChange {
  std::string label;
  ByteBuffer tag;
  bool federationSynchronized = false;
};

enum class RemoteTimeManagementEventKind {
  RegulationEnabled,
  ConstrainedEnabled,
  AdvanceGranted
};

struct RemoteTimeManagementEvent {
  RemoteTimeManagementEventKind kind =
      RemoteTimeManagementEventKind::AdvanceGranted;
  double logicalTimeSeconds = 0.0;
};

class HlaInboundAdapter final : public IHlaEventSink {
public:
  enum class RemoteObjectKind {
    Platform,
    Munition,
    EmitterSystem,
    RadarBeam,
    Unsupported
  };

  void onObjectDiscovered(const RemoteObjectDiscovery& event) override;
  void onObjectReflected(const RemoteObjectReflection& event) override;
  void onObjectRemoved(const RemoteObjectRemoval& event) override;
  void onInteractionReceived(const RemoteInteraction& event) override;
  void onSynchronizationPointAnnounced(
      const SynchronizationPointAnnouncement& event) override;
  void onFederationSynchronized(const std::string& label) override;
  void onTimeRegulationEnabled(double logicalTimeSeconds) override;
  void onTimeConstrainedEnabled(double logicalTimeSeconds) override;
  void onTimeAdvanceGranted(double logicalTimeSeconds) override;

  std::vector<RemoteEntityChange> takeEntityChanges();
  std::vector<RemoteMunitionChange> takeMunitionChanges();
  std::vector<RemoteSensorChange> takeSensorChanges();
  std::vector<RemoteWarfareEvent> takeWarfareEvents();
  std::vector<RemoteSynchronizationChange> takeSynchronizationChanges();
  std::vector<RemoteTimeManagementEvent> takeTimeManagementEvents();
  std::vector<RemoteSimulationControl> takeSimulationControls();

private:
  struct RemoteObject {
    RemoteObjectKind kind = RemoteObjectKind::Unsupported;
    std::string instanceName;
  };

  struct RemoteEntity {
    RprEntityState state;
    bool dirty = false;
  };

  struct RemoteEmitter {
    std::string sensorId;
    std::string hostEntityId;
  };

  std::unordered_map<ObjectInstanceId, RemoteObject> _objects;
  std::unordered_map<ObjectInstanceId, RemoteEntity> _entities;
  std::unordered_map<ObjectInstanceId, RemoteEntity> _munitions;
  std::unordered_map<ObjectInstanceId, RemoteEmitter> _emitters;
  std::unordered_map<std::string, ObjectInstanceId> _emitterIdsByName;
  std::unordered_map<ObjectInstanceId, ObjectInstanceId> _beamEmitterIds;
  std::vector<RemoteEntityChange> _removedEntities;
  std::vector<RemoteMunitionChange> _removedMunitions;
  std::vector<RemoteSensorChange> _sensorChanges;
  std::vector<RemoteWarfareEvent> _warfareEvents;
  std::vector<RemoteSynchronizationChange> _synchronizationChanges;
  std::vector<RemoteTimeManagementEvent> _timeManagementEvents;
  std::vector<RemoteSimulationControl> _simulationControls;
  std::unordered_set<std::string> _seenWarfareEvents;
};

} // namespace tactical::hla
