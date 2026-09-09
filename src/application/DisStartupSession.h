#pragma once

#include "application/StartupConfiguration.h"
#include "domain/Entity.h"
#include "domain/Munition.h"
#include "infrastructure/interoperability/dis/DisGateway.h"

#include <QHash>
#include <QSet>
#include <QVector>

#include <vector>

namespace application {

class DisStartupSession {
public:
  tactical::dis::Result start(const DisStartupConfiguration& configuration);
  tactical::dis::Result stop();
  bool isActive() const;
  void setEntityManagementHandler(std::function<bool(const tactical::dis::EntityManagementRequest&)> handler);
  tactical::dis::Result publishEntityManagement(tactical::dis::EntityManagementRequest request) { return _gateway.publishEntityManagement(request); }
  tactical::dis::Result publishIff(tactical::dis::IffState state);
  tactical::dis::Result publishCollision(tactical::dis::CollisionEvent event);
  std::vector<tactical::dis::IffState> takeRemoteIffStates() { return _gateway.takeRemoteIffStates(); }
  std::vector<tactical::dis::CollisionEvent> takeRemoteCollisions() { return _gateway.takeRemoteCollisions(); }
  std::vector<tactical::dis::Acknowledgement> takeRemoteAcknowledgements() {
    return _gateway.takeRemoteAcknowledgements();
  }
  tactical::dis::Result publishSensors(const QVector<Entity>& entities);
  std::vector<tactical::dis::RadarEmission> takeRemoteRadarEmissions() {
    return _gateway.takeRemoteRadarEmissions();
  }
  tactical::dis::Result publishSimulationControl(tactical::dis::SimulationControl control, double time) {
    return _gateway.publishSimulationControl(control, time);
  }
  std::vector<tactical::dis::SimulationControl> takeRemoteSimulationControls() {
    return _gateway.takeRemoteSimulationControls();
  }
  tactical::dis::Result publishEntities(const QVector<Entity>& entities);
  tactical::dis::Result publishMunitions(
      const QVector<ActiveMunition>& activeMunitions);
  tactical::dis::Result publishDetonations(
      const QVector<TransientEffect>& transientEffects);
  tactical::dis::Result publishTestEntity(
      tactical::dis::EntityState state);
  tactical::dis::Result poll();
  std::vector<tactical::dis::RemoteEntityChange> takeRemoteEntityChanges();
  std::vector<tactical::dis::WarfareEvent> takeRemoteWarfareEvents();
  std::uint64_t publishedPduCount() const;
  std::uint64_t receivedPduCount() const;
  qsizetype remoteEntityCount() const;

private:
  std::uint16_t entityNumberFor(const QString& stableId);

  DisStartupConfiguration _configuration;
  tactical::dis::DisGateway _gateway;
  QHash<QString, std::uint16_t> _entityNumbers;
  QHash<QString, QHash<QString, std::uint8_t>> _sensorNumbers;
  QHash<QString, std::uint16_t> _munitionNumbers;
  struct WarfareCorrelation {
    tactical::dis::EntityIdentifier firingEntity;
    tactical::dis::EntityIdentifier targetEntity;
    tactical::dis::EntityIdentifier munitionEntity;
    tactical::dis::EntityIdentifier eventIdentifier;
    QString munitionType;
  };
  QHash<QString, WarfareCorrelation> _warfareCorrelations;
  QSet<QString> _sentFireIds;
  QSet<QString> _sentDetonationIds;
  std::uint16_t _nextEntityNumber = 1;
  std::uint16_t _nextMunitionNumber = 32768;
  std::uint16_t _nextEventNumber = 1;
};

} // namespace application
