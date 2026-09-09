#pragma once

#include "infrastructure/interoperability/dis/DisTypes.h"
#include "infrastructure/interoperability/dis/DisRemoteEntityRegistry.h"

#include <QElapsedTimer>
#include <QHostAddress>
#include <QUdpSocket>
#include <QSet>
#include <QHash>
#include <deque>
#include <functional>

#include <vector>

namespace tactical::dis {

struct GatewayConfiguration {
  QString address;
  std::uint16_t port = 3000;
  std::uint8_t exerciseId = 1;
  std::uint16_t siteId = 1;
  std::uint16_t applicationId = 1;
  double remoteEntityTimeoutSeconds = 12.0;
};

class DisGateway {
public:
  Result start(const GatewayConfiguration& configuration);
  Result stop();
  bool isActive() const;
  Result publish(const std::vector<EntityState>& states);
  Result publishWarfareEvent(const WarfareEvent& event);
  Result publishSimulationControl(SimulationControl control, double simulationTimeSeconds);
  Result publishRadar(const RadarEmission& emission);
  Result publishIff(const IffState& state);
  Result publishCollision(const CollisionEvent& event);
  Result publishEntityManagement(EntityManagementRequest request);
  void setEntityManagementHandler(std::function<bool(const EntityManagementRequest&)> handler);
  std::vector<IffState> takeRemoteIffStates();
  std::vector<CollisionEvent> takeRemoteCollisions();
  std::vector<RadarEmission> takeRemoteRadarEmissions();
  std::vector<SimulationControl> takeRemoteSimulationControls();
  std::vector<Acknowledgement> takeRemoteAcknowledgements();
  Result poll();
  std::vector<RemoteEntityChange> takeRemoteEntityChanges();
  std::vector<WarfareEvent> takeRemoteWarfareEvents();
  std::uint64_t publishedPduCount() const;
  std::uint64_t receivedPduCount() const;
  qsizetype remoteEntityCount() const;

  // Kept public for deterministic codec/ownership tests without UDP timing.
  bool acceptDatagram(const QByteArray& datagram, QString* error = nullptr);

private:
  Result sendInteraction(const QByteArray& data, const QString& error);
  QUdpSocket _socket;
  GatewayConfiguration _configuration;
  QHostAddress _destination;
  QElapsedTimer _clock;
  DisRemoteEntityRegistry _remoteRegistry;
  bool _active = false;
  std::uint64_t _publishedPduCount = 0;
  std::uint64_t _receivedPduCount = 0;
  std::vector<WarfareEvent> _remoteWarfareEvents;
  std::vector<SimulationControl> _remoteControls;
  std::vector<RadarEmission> _remoteRadar;
  QSet<QString> _seenControls;
  std::deque<QString> _controlHistory;
  std::uint32_t _nextRequestId = 1;
  struct PendingControl {
    std::uint16_t flag;
    QSet<QString> responders;
  };
  QHash<std::uint32_t, PendingControl> _pendingControls;
  std::deque<std::uint32_t> _pendingOrder;
  std::vector<Acknowledgement> _remoteAcknowledgements;
  std::function<bool(const EntityManagementRequest&)> _managementHandler;
  QHash<QString, Acknowledgement> _managementReplies;
  std::deque<QString> _managementOrder;
  QSet<QString> _collisionIds;
  std::deque<QString> _collisionOrder;
  std::vector<IffState> _remoteIff;
  std::vector<CollisionEvent> _remoteCollisions;
};

} // namespace tactical::dis
