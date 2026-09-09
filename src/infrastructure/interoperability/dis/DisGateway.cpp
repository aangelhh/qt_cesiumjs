#include "infrastructure/interoperability/dis/DisGateway.h"

#include "infrastructure/interoperability/dis/DisEntityStateCodec.h"
#include "infrastructure/interoperability/dis/DisWarfareCodec.h"
#include "infrastructure/interoperability/dis/DisSimulationControlCodec.h"
#include "infrastructure/interoperability/dis/DisEmissionCodec.h"
#include "infrastructure/interoperability/dis/DisInteractionCodec.h"

#include <QNetworkDatagram>

namespace tactical::dis {

Result DisGateway::start(const GatewayConfiguration& configuration) {
  stop();
  const QHostAddress destination(configuration.address);
  if (destination.isNull()) {
    return Result::fail(QStringLiteral("Invalid DIS destination address."));
  }
  if (configuration.port == 0) {
    return Result::fail(QStringLiteral("Invalid DIS UDP port."));
  }
  if (configuration.remoteEntityTimeoutSeconds <= 0.0) {
    return Result::fail(QStringLiteral("Invalid DIS remote timeout."));
  }

  if (!_socket.bind(
          QHostAddress::AnyIPv4,
          configuration.port,
          QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
    return Result::fail(
        QStringLiteral("Cannot bind DIS UDP port %1: %2")
            .arg(configuration.port)
            .arg(_socket.errorString()));
  }
  if (destination.isMulticast() && !_socket.joinMulticastGroup(destination)) {
    const QString detail = _socket.errorString();
    _socket.close();
    return Result::fail(
        QStringLiteral("Cannot join DIS multicast group %1: %2")
            .arg(configuration.address, detail));
  }

  _configuration = configuration;
  _destination = destination;
  _clock.start();
  _remoteRegistry.configure(
      configuration.exerciseId,
      configuration.siteId,
      configuration.applicationId,
      static_cast<qint64>(configuration.remoteEntityTimeoutSeconds * 1000.0));
  _publishedPduCount = 0;
  _receivedPduCount = 0;
  _remoteWarfareEvents.clear();
  _active = true;
  return {};
}

Result DisGateway::stop() {
  _managementReplies.clear();
  _managementOrder.clear();
  _collisionIds.clear();
  _collisionOrder.clear();
  _remoteIff.clear();
  _remoteCollisions.clear();
  _pendingControls.clear();
  _pendingOrder.clear();
  _remoteAcknowledgements.clear();
  _remoteControls.clear();
  _remoteRadar.clear();
  _seenControls.clear();
  _controlHistory.clear();
  if (_active && _destination.isMulticast()) {
    _socket.leaveMulticastGroup(_destination);
  }
  _socket.close();
  _active = false;
  _destination = {};
  _remoteRegistry.clear();
  _remoteWarfareEvents.clear();
  return {};
}

bool DisGateway::isActive() const { return _active; }

Result DisGateway::sendInteraction(const QByteArray& data, const QString& error) {
  if (!_active) return Result::fail(QStringLiteral("DIS gateway is not active."));
  if (data.isEmpty()) return Result::fail(error);
  if (_socket.writeDatagram(data, _destination, _configuration.port) != data.size()) return Result::fail(_socket.errorString());
  ++_publishedPduCount;
  return {};
}
Result DisGateway::publishIff(const IffState& state) {
  QString error;
  const auto data = DisInteractionCodec::encode(state, &error);
  return sendInteraction(data, error);
}
Result DisGateway::publishCollision(const CollisionEvent& event) {
  QString error;
  const auto data = DisInteractionCodec::encode(event, &error);
  return sendInteraction(data, error);
}
Result DisGateway::publishEntityManagement(EntityManagementRequest request) {
  request.source = {_configuration.siteId, _configuration.applicationId, 0};
  request.exerciseId = _configuration.exerciseId;
  request.requestId = _nextRequestId++;
  QString error;
  const auto data = DisInteractionCodec::encode(request, &error);
  const auto result = sendInteraction(data, error);
  if (result.success) {
    _pendingControls.insert(request.requestId, {static_cast<std::uint16_t>(request.remove ? 2 : 1), {}});
    _pendingOrder.push_back(request.requestId);
    if (_pendingOrder.size() > 1024) {
      _pendingControls.remove(_pendingOrder.front());
      _pendingOrder.pop_front();
    }
  }
  return result;
}
void DisGateway::setEntityManagementHandler(std::function<bool(const EntityManagementRequest&)> handler) {
  _managementHandler = std::move(handler);
}
std::vector<IffState> DisGateway::takeRemoteIffStates() {
  std::vector<IffState> result;
  result.swap(_remoteIff);
  return result;
}
std::vector<CollisionEvent> DisGateway::takeRemoteCollisions() {
  std::vector<CollisionEvent> result;
  result.swap(_remoteCollisions);
  return result;
}

Result DisGateway::publishRadar(const RadarEmission& emission) {
  if (!_active) return Result::fail(QStringLiteral("DIS gateway is not active."));
  QString error;
  const auto data = DisEmissionCodec::encode(emission, &error);
  if (data.isEmpty()) return Result::fail(error);
  if (_socket.writeDatagram(data, _destination, _configuration.port) != data.size())
    return Result::fail(_socket.errorString());
  ++_publishedPduCount;
  return {};
}

std::vector<RadarEmission> DisGateway::takeRemoteRadarEmissions() {
  std::vector<RadarEmission> result;
  result.swap(_remoteRadar);
  return result;
}

Result DisGateway::publishSimulationControl(SimulationControl control, double simulationTimeSeconds) {
  if (!_active) return Result::fail(QStringLiteral("DIS gateway is not active."));
  SimulationControlMessage message;
  message.control = control;
  message.source = {_configuration.siteId, _configuration.applicationId, 0};
  message.exerciseId = _configuration.exerciseId;
  message.requestId = _nextRequestId++;
  message.simulationTimeSeconds = simulationTimeSeconds;
  QString error;
  const auto data = DisSimulationControlCodec::encode(message, &error);
  if (data.isEmpty()) return Result::fail(error);
  if (_socket.writeDatagram(data, _destination, _configuration.port) != data.size())
    return Result::fail(_socket.errorString());
  ++_publishedPduCount;
  _pendingControls.insert(message.requestId, {static_cast<std::uint16_t>(
      control == SimulationControl::StartResume ? 3 : 4), {}});
  _pendingOrder.push_back(message.requestId);
  if (_pendingOrder.size() > 1024) {
    _pendingControls.remove(_pendingOrder.front());
    _pendingOrder.pop_front();
  }
  return {};
}

std::vector<SimulationControl> DisGateway::takeRemoteSimulationControls() {
  std::vector<SimulationControl> result;
  result.swap(_remoteControls);
  return result;
}

std::vector<Acknowledgement> DisGateway::takeRemoteAcknowledgements() {
  std::vector<Acknowledgement> result;
  result.swap(_remoteAcknowledgements);
  return result;
}

Result DisGateway::publish(const std::vector<EntityState>& states) {
  if (!_active) return Result::fail(QStringLiteral("DIS gateway is not active."));
  for (const EntityState& state : states) {
    QString error;
    const QByteArray datagram = DisEntityStateCodec::encode(state, &error);
    if (datagram.isEmpty()) return Result::fail(error);
    const qint64 sent = _socket.writeDatagram(
        datagram, _destination, _configuration.port);
    if (sent != datagram.size()) {
      return Result::fail(
          QStringLiteral("DIS datagram publication failed: %1")
              .arg(_socket.errorString()));
    }
    ++_publishedPduCount;
  }
  return {};
}

Result DisGateway::publishWarfareEvent(const WarfareEvent& event) {
  if (!_active) return Result::fail(QStringLiteral("DIS gateway is not active."));
  QString error;
  const QByteArray datagram = event.kind == WarfareEventKind::Fire
      ? DisWarfareCodec::encodeFire(event, &error)
      : DisWarfareCodec::encodeDetonation(event, &error);
  if (datagram.isEmpty()) return Result::fail(error);
  const qint64 sent = _socket.writeDatagram(
      datagram, _destination, _configuration.port);
  if (sent != datagram.size()) {
    return Result::fail(
        QStringLiteral("DIS warfare PDU publication failed: %1")
            .arg(_socket.errorString()));
  }
  ++_publishedPduCount;
  return {};
}

Result DisGateway::poll() {
  if (!_active) return Result::fail(QStringLiteral("DIS gateway is not active."));
  while (_socket.hasPendingDatagrams()) {
    const QNetworkDatagram networkDatagram = _socket.receiveDatagram();
    QString error;
    if (!acceptDatagram(networkDatagram.data(), &error) && !error.isEmpty()) {
      return Result::fail(error);
    }
  }
  _remoteRegistry.expire(_clock.elapsed());
  return {};
}

std::vector<RemoteEntityChange> DisGateway::takeRemoteEntityChanges() {
  return _remoteRegistry.takeChanges();
}

std::vector<WarfareEvent> DisGateway::takeRemoteWarfareEvents() {
  std::vector<WarfareEvent> result;
  result.swap(_remoteWarfareEvents);
  return result;
}

std::uint64_t DisGateway::publishedPduCount() const {
  return _publishedPduCount;
}

std::uint64_t DisGateway::receivedPduCount() const {
  return _receivedPduCount;
}

qsizetype DisGateway::remoteEntityCount() const {
  return _remoteRegistry.size();
}

bool DisGateway::acceptDatagram(const QByteArray& datagram, QString* error) {
  if (datagram.size() < 3) {
    if (error) *error = QStringLiteral("DIS datagram is shorter than its header.");
    return false;
  }
  const std::uint8_t pduType =
      static_cast<std::uint8_t>(datagram.at(2));
  const auto remote = [this](std::uint8_t exercise, const EntityIdentifier& source) {
    return exercise == _configuration.exerciseId &&
        (source.site != _configuration.siteId || source.application != _configuration.applicationId);
  };
  if (pduType == 11 || pduType == 12) {
    EntityManagementRequest request;
    if (!DisInteractionCodec::decode(datagram, request, error)) return false;
    ++_receivedPduCount;
    if (!remote(request.exerciseId, request.source) || request.destination.site != _configuration.siteId ||
        request.destination.application != _configuration.applicationId) return true;
    const auto key = request.source.key() + QStringLiteral(":%1:%2").arg(request.requestId).arg(pduType);
    if (!_managementReplies.contains(key)) {
      // Apply before acknowledging; absent handlers and protected entities return UnableToComply.
      const bool accepted = request.destination.entity > 0 && request.destination.entity < 65535 &&
          _managementHandler && _managementHandler(request);
      Acknowledgement ack;
      ack.source = request.destination;
      ack.destination = request.source;
      ack.exerciseId = request.exerciseId;
      ack.requestId = request.requestId;
      ack.acknowledgeFlag = request.remove ? 2 : 1;
      ack.responseFlag = accepted ? 1 : 2;
      _managementReplies.insert(key, ack);
      _managementOrder.push_back(key);
      if (_managementOrder.size() > 1024) {
        _managementReplies.remove(_managementOrder.front());
        _managementOrder.pop_front();
      }
    }
    if (_active) {
      const auto data = DisSimulationControlCodec::encodeAcknowledgement(_managementReplies.value(key), error);
      const auto result = sendInteraction(data, error ? *error : QString());
      if (!result.success) { if (error) *error = result.message; return false; }
    }
    return true;
  }
  if (pduType == 28) {
    IffState state;
    if (!DisInteractionCodec::decode(datagram, state, error)) return false;
    ++_receivedPduCount;
    if (remote(state.exerciseId, state.host)) _remoteIff.push_back(state);
    return true;
  }
  if (pduType == 4) {
    CollisionEvent event;
    if (!DisInteractionCodec::decode(datagram, event, error)) return false;
    ++_receivedPduCount;
    if (!remote(event.exerciseId, event.source)) return true;
    const auto key = event.source.key() + QLatin1Char('/') + event.event.key();
    if (_collisionIds.contains(key)) return true;
    _collisionIds.insert(key);
    _collisionOrder.push_back(key);
    if (_collisionOrder.size() > 1024) {
      _collisionIds.remove(_collisionOrder.front());
      _collisionOrder.pop_front();
    }
    _remoteCollisions.push_back(event);
    return true;
  }
  if (pduType == 15) {
    Acknowledgement ack;
    if (!DisSimulationControlCodec::decodeAcknowledgement(datagram, ack, error)) return false;
    ++_receivedPduCount;
    const EntityIdentifier local{_configuration.siteId, _configuration.applicationId, 0};
    if (ack.exerciseId != _configuration.exerciseId || !(ack.destination == local) ||
        (ack.source.site == local.site && ack.source.application == local.application)) return true;
    auto pending = _pendingControls.find(ack.requestId);
    if (pending == _pendingControls.end() || pending->flag != ack.acknowledgeFlag ||
        ack.responseFlag > 2 || pending->responders.contains(ack.source.key())) return true;
    // Broadcast requests can receive one response from each participating application.
    if (pending->responders.size() >= 1024) return true;
    pending->responders.insert(ack.source.key());
    _remoteAcknowledgements.push_back(ack);
    return true;
  }
  if (pduType == 23) {
    RadarEmission emission;
    if (!DisEmissionCodec::decode(datagram, emission, error)) return false;
    ++_receivedPduCount;
    if (emission.exerciseId != _configuration.exerciseId ||
        (emission.host.site == _configuration.siteId &&
         emission.host.application == _configuration.applicationId)) return true;
    _remoteRadar.push_back(std::move(emission));
    return true;
  }
  if (pduType == 13 || pduType == 14) {
    SimulationControlMessage message;
    if (!DisSimulationControlCodec::decode(datagram, message, error)) return false;
    ++_receivedPduCount;
    if (message.exerciseId != _configuration.exerciseId ||
        (message.source.site == _configuration.siteId &&
         message.source.application == _configuration.applicationId)) return true;
    const auto& destination = message.destination;
    if ((destination.site != 65535 && destination.site != _configuration.siteId) ||
        (destination.application != 65535 && destination.application != _configuration.applicationId) ||
        (destination.entity != 65535 && destination.entity != 0)) return true;
    const QString key = message.source.key() + QStringLiteral(":%1:%2")
        .arg(message.requestId).arg(pduType);
    // Repeat the acknowledgement on retries, but enqueue the control only once.
    // AbleToComply confirms acceptance by the gateway, not completed execution.
    if (_active) {
      Acknowledgement ack;
      ack.source = {_configuration.siteId, _configuration.applicationId, 0};
      ack.destination = message.source;
      ack.exerciseId = message.exerciseId;
      ack.requestId = message.requestId;
      ack.acknowledgeFlag = pduType == 13 ? 3 : 4;
      const auto data = DisSimulationControlCodec::encodeAcknowledgement(ack, error);
      if (data.isEmpty()) return false;
      if (_socket.writeDatagram(data, _destination, _configuration.port) != data.size()) {
        if (error) *error = _socket.errorString();
        return false;
      }
      ++_publishedPduCount;
    }
    if (_seenControls.contains(key)) return true;
    _seenControls.insert(key);
    _controlHistory.push_back(key);
    if (_controlHistory.size() > 1024) {
      _seenControls.remove(_controlHistory.front());
      _controlHistory.pop_front();
    }
    _remoteControls.push_back(message.control);
    return true;
  }
  if (pduType == 2 || pduType == 3) {
    WarfareEvent event;
    if (!DisWarfareCodec::decode(datagram, event, error)) return false;
    ++_receivedPduCount;
    if (event.exerciseId != _configuration.exerciseId) return true;
    const EntityIdentifier& source = event.firingEntity.entity != 0
        ? event.firingEntity : event.eventIdentifier;
    if (source.site == _configuration.siteId &&
        source.application == _configuration.applicationId) {
      return true;
    }
    _remoteWarfareEvents.push_back(std::move(event));
    return true;
  }
  if (pduType != 1) return true;
  EntityState state;
  if (!DisEntityStateCodec::decode(datagram, state, error)) return false;
  ++_receivedPduCount;
  _remoteRegistry.accept(state, _clock.isValid() ? _clock.elapsed() : 0);
  return true;
}

} // namespace tactical::dis
