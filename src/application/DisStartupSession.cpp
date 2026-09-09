#include "application/DisStartupSession.h"

#include "domain/EntityIdentity.h"
#include "domain/Munition.h"

#include <algorithm>
#include <cmath>
namespace application {
namespace {

tactical::dis::EntityState toDisState(
    const Entity& entity,
    const DisStartupConfiguration& configuration,
    std::uint16_t entityNumber) {
  tactical::dis::EntityState state;
  state.identifier = {
      static_cast<std::uint16_t>(configuration.siteId),
      static_cast<std::uint16_t>(configuration.applicationId),
      entityNumber};
  state.exerciseId = static_cast<std::uint8_t>(configuration.exerciseId);
  state.marking = entity.callsign.trimmed().isEmpty()
      ? entity.name : entity.callsign;
  state.forceIdentifier = static_cast<std::uint8_t>(entity.forceIdentifier);
  state.entityKind = static_cast<std::uint8_t>(entity.entityKind);
  state.entityDomain = static_cast<std::uint8_t>(entity.entityDomain);
  state.country = static_cast<std::uint16_t>(entity.entityCountry);
  state.category = static_cast<std::uint8_t>(entity.entityCategory);
  state.subcategory = static_cast<std::uint8_t>(entity.entitySubcategory);
  state.specific = static_cast<std::uint8_t>(entity.entitySpecific);
  state.extra = static_cast<std::uint8_t>(entity.entityExtra);
  state.latitudeDegrees = entity.latitude;
  state.longitudeDegrees = entity.longitude;
  state.altitudeMeters = entity.altitude;
  state.headingDegrees = entity.headingDegrees;
  state.pitchDegrees = entity.pitchDegrees;
  state.rollDegrees = entity.rollDegrees;
  state.speedKnots = entity.speedKnots;
  state.verticalSpeedMetersPerSecond = entity.verticalSpeedMetersPerSecond;
  state.damagePercent = entity.damagePercent;
  state.destroyed = entity.destroyed;
  return state;
}

tactical::dis::EntityIdentifier parseDisIdentifier(const QString& value) {
  const QStringList parts = value.split(QLatin1Char(':'));
  if (parts.size() != 4 || parts[0] != QStringLiteral("dis")) return {};
  bool siteOk = false;
  bool applicationOk = false;
  bool entityOk = false;
  const int site = parts[1].toInt(&siteOk);
  const int application = parts[2].toInt(&applicationOk);
  const int entity = parts[3].toInt(&entityOk);
  if (!siteOk || !applicationOk || !entityOk ||
      site < 0 || site > 65535 ||
      application < 0 || application > 65535 ||
      entity < 0 || entity > 65535) {
    return {};
  }
  return {
      static_cast<std::uint16_t>(site),
      static_cast<std::uint16_t>(application),
      static_cast<std::uint16_t>(entity)};
}

} // namespace

tactical::dis::Result DisStartupSession::start(
    const DisStartupConfiguration& configuration) {
  _configuration = configuration;
  _entityNumbers.clear();
  _sensorNumbers.clear();
  _munitionNumbers.clear();
  _warfareCorrelations.clear();
  _sentFireIds.clear();
  _sentDetonationIds.clear();
  _nextEntityNumber = 1;
  _nextMunitionNumber = 32768;
  _nextEventNumber = 1;
  return _gateway.start({
      configuration.address,
      static_cast<std::uint16_t>(configuration.port),
      static_cast<std::uint8_t>(configuration.exerciseId),
      static_cast<std::uint16_t>(configuration.siteId),
      static_cast<std::uint16_t>(configuration.applicationId),
      configuration.remoteEntityTimeoutSeconds});
}

tactical::dis::Result DisStartupSession::stop() { return _gateway.stop(); }
bool DisStartupSession::isActive() const { return _gateway.isActive(); }

void DisStartupSession::setEntityManagementHandler(std::function<bool(const tactical::dis::EntityManagementRequest&)> handler) {
  _gateway.setEntityManagementHandler([this, handler = std::move(handler)](const auto& request) {
    const auto number = request.destination.entity;
    // Upper IDs are reserved for munitions; never alias an existing local entity.
    if (number == 0 || number >= 32768) return false;
    const auto key = QStringLiteral("managed:") + request.destination.key();
    for (auto it = _entityNumbers.cbegin(); it != _entityNumbers.cend(); ++it)
      if (it.value() == number && it.key() != key) return false;
    if (!handler(request)) return false;
    _entityNumbers.insert(key, number);
    return true;
  });
}
tactical::dis::Result DisStartupSession::publishIff(tactical::dis::IffState state) {
  state.exerciseId = static_cast<std::uint8_t>(_configuration.exerciseId);
  state.host.site = static_cast<std::uint16_t>(_configuration.siteId);
  state.host.application = static_cast<std::uint16_t>(_configuration.applicationId);
  return _gateway.publishIff(state);
}
tactical::dis::Result DisStartupSession::publishCollision(tactical::dis::CollisionEvent event) {
  event.exerciseId = static_cast<std::uint8_t>(_configuration.exerciseId);
  event.source.site = static_cast<std::uint16_t>(_configuration.siteId);
  event.source.application = static_cast<std::uint16_t>(_configuration.applicationId);
  event.event = {event.source.site, event.source.application, _nextEventNumber++};
  if (_nextEventNumber == 0) _nextEventNumber = 1;
  return _gateway.publishCollision(event);
}

tactical::dis::Result DisStartupSession::publishEntities(
    const QVector<Entity>& entities) {
  // Reserve remotely assigned IDs before allocating ordinary entities on reconnect.
  for (const auto& entity : entities) {
    if (!entity.externallyControlled && domain::entityKey(entity).startsWith(QStringLiteral("managed:")) &&
        entityNumberFor(domain::entityKey(entity)) == 0)
      return tactical::dis::Result::fail(QStringLiteral("DIS managed entity identifier conflict."));
  }
  std::vector<tactical::dis::EntityState> states;
  states.reserve(static_cast<std::size_t>(entities.size()));
  for (const Entity& entity : entities) {
    if (entity.externallyControlled) continue;
    const QString stableId = domain::entityKey(entity);
    const auto number = entityNumberFor(stableId);
    if (number == 0) return tactical::dis::Result::fail(QStringLiteral("DIS entity identifier limit reached."));
    states.push_back(toDisState(
        entity, _configuration, number));
  }
  return _gateway.publish(states);
}

tactical::dis::Result DisStartupSession::publishSensors(const QVector<Entity>& entities) {
  for (const auto& entity : entities) {
    if (entity.externallyControlled) continue;
    const auto key = domain::entityKey(entity);
    const auto number = entityNumberFor(key);
    if (number == 0) return tactical::dis::Result::fail(QStringLiteral("DIS entity identifier limit reached."));
    tactical::dis::RadarEmission emission;
    emission.host = {static_cast<std::uint16_t>(_configuration.siteId),
        static_cast<std::uint16_t>(_configuration.applicationId), number};
    emission.exerciseId = static_cast<std::uint8_t>(_configuration.exerciseId);
    auto& ids = _sensorNumbers[key];
    for (const auto& sensor : entity.sensors) {
      if (sensor.sensorType.compare(QStringLiteral("radar"), Qt::CaseInsensitive) != 0) continue;
      if (!ids.contains(sensor.id)) {
        if (ids.size() >= 255) return tactical::dis::Result::fail(QStringLiteral("DIS radar identifier limit reached."));
        ids.insert(sensor.id, static_cast<std::uint8_t>(ids.size() + 1));
      }
      tactical::dis::RadarBeam beam;
      beam.systemId = ids.value(sensor.id);
      beam.emitting = sensor.enabled && sensor.emitting && !entity.destroyed;
      beam.azimuthCenterDegrees = sensor.azimuthCenterDegrees;
      beam.azimuthWidthDegrees = sensor.azimuthWidthDegrees;
      beam.elevationCenterDegrees = sensor.elevationCenterDegrees;
      beam.elevationWidthDegrees = sensor.elevationWidthDegrees;
      beam.frequencyHertz = sensor.radarProfile.frequencyHertz;
      beam.bandwidthHertz = sensor.radarProfile.bandwidthHertz;
      beam.effectiveRadiatedPowerDbm = sensor.radarProfile.peakPowerWatts > 0
          ? 10 * std::log10(sensor.radarProfile.peakPowerWatts * 1000) + sensor.radarProfile.antennaGainDecibels
          : 0;
      emission.beams.push_back(beam);
    }
    if (ids.isEmpty()) continue;
    const auto result = _gateway.publishRadar(emission);
    if (!result.success) return result;
  }
  return {};
}

tactical::dis::Result DisStartupSession::publishMunitions(
    const QVector<ActiveMunition>& activeMunitions) {
  const auto identifierForReference = [this](const QString& reference) {
    const tactical::dis::EntityIdentifier remote = parseDisIdentifier(reference);
    if (remote.entity != 0) return remote;
    if (reference.trimmed().isEmpty()) return tactical::dis::EntityIdentifier{};
    return tactical::dis::EntityIdentifier{
        static_cast<std::uint16_t>(_configuration.siteId),
        static_cast<std::uint16_t>(_configuration.applicationId),
        entityNumberFor(reference)};
  };

  for (const ActiveMunition& munition : activeMunitions) {
    if (!munition.active || munition.id.trimmed().isEmpty() ||
        _sentFireIds.contains(munition.id)) {
      continue;
    }
    std::uint16_t munitionNumber = _munitionNumbers.value(munition.id, 0);
    if (munitionNumber == 0) {
      munitionNumber = _nextMunitionNumber++;
      if (_nextMunitionNumber == 0) _nextMunitionNumber = 32768;
      _munitionNumbers.insert(munition.id, munitionNumber);
    }
    const std::uint16_t eventNumber = _nextEventNumber++;
    if (_nextEventNumber == 0) _nextEventNumber = 1;

    WarfareCorrelation correlation;
    correlation.firingEntity = identifierForReference(
        munition.launcherEntityId.trimmed().isEmpty()
            ? munition.launcherEntityName : munition.launcherEntityId);
    correlation.targetEntity = identifierForReference(
        munition.targetEntityId.trimmed().isEmpty()
            ? munition.targetEntityName : munition.targetEntityId);
    correlation.munitionEntity = {
        static_cast<std::uint16_t>(_configuration.siteId),
        static_cast<std::uint16_t>(_configuration.applicationId),
        munitionNumber};
    correlation.eventIdentifier = {
        static_cast<std::uint16_t>(_configuration.siteId),
        static_cast<std::uint16_t>(_configuration.applicationId),
        eventNumber};
    correlation.munitionType = munition.munitionType;

    tactical::dis::WarfareEvent event;
    event.kind = tactical::dis::WarfareEventKind::Fire;
    event.exerciseId = static_cast<std::uint8_t>(_configuration.exerciseId);
    event.firingEntity = correlation.firingEntity;
    event.targetEntity = correlation.targetEntity;
    event.munitionEntity = correlation.munitionEntity;
    event.eventIdentifier = correlation.eventIdentifier;
    event.munitionType = correlation.munitionType;
    event.latitudeDegrees = munition.latitude;
    event.longitudeDegrees = munition.longitude;
    event.altitudeMeters = munition.altitudeMeters;
    event.headingDegrees = munition.headingDegrees;
    event.pitchDegrees = munition.pitchDegrees;
    event.speedMetersPerSecond = munition.speedMetersPerSecond;
    const tactical::dis::Result result = _gateway.publishWarfareEvent(event);
    if (!result.success) return result;
    _warfareCorrelations.insert(munition.id, correlation);
    _sentFireIds.insert(munition.id);
  }
  return {};
}

tactical::dis::Result DisStartupSession::publishDetonations(
    const QVector<TransientEffect>& transientEffects) {
  for (const TransientEffect& effect : transientEffects) {
    const bool missileImpact =
        effect.effectType == QStringLiteral("ImpactFlash");
    const bool bombImpact =
        effect.effectType == QStringLiteral("BombImpactFlash");
    if ((!missileImpact && !bombImpact) ||
        _sentDetonationIds.contains(effect.id)) {
      continue;
    }
    const QString suffix = bombImpact
        ? QStringLiteral("-bombimpactflash")
        : QStringLiteral("-impact");
    const QString munitionId = effect.id.chopped(suffix.size());
    WarfareCorrelation correlation = _warfareCorrelations.value(munitionId);
    if (correlation.eventIdentifier.entity == 0) {
      const std::uint16_t eventNumber = _nextEventNumber++;
      if (_nextEventNumber == 0) _nextEventNumber = 1;
      correlation.eventIdentifier = {
          static_cast<std::uint16_t>(_configuration.siteId),
          static_cast<std::uint16_t>(_configuration.applicationId),
          eventNumber};
      correlation.munitionType = bombImpact
          ? QStringLiteral("Bomb") : QStringLiteral("Missile");
    }

    tactical::dis::WarfareEvent event;
    event.kind = tactical::dis::WarfareEventKind::Detonation;
    event.exerciseId = static_cast<std::uint8_t>(_configuration.exerciseId);
    event.firingEntity = correlation.firingEntity;
    event.targetEntity = correlation.targetEntity;
    event.munitionEntity = correlation.munitionEntity;
    event.eventIdentifier = correlation.eventIdentifier;
    event.munitionType = correlation.munitionType;
    event.latitudeDegrees = effect.latitude;
    event.longitudeDegrees = effect.longitude;
    event.altitudeMeters = effect.altitudeMeters;
    event.detonationResult = bombImpact ? 3 : 1;
    const tactical::dis::Result result = _gateway.publishWarfareEvent(event);
    if (!result.success) return result;
    _sentDetonationIds.insert(effect.id);
    _warfareCorrelations.remove(munitionId);
  }
  return {};
}

tactical::dis::Result DisStartupSession::publishTestEntity(
    tactical::dis::EntityState state) {
  state.exerciseId = static_cast<std::uint8_t>(_configuration.exerciseId);
  state.identifier.site = static_cast<std::uint16_t>(_configuration.siteId);
  state.identifier.application =
      static_cast<std::uint16_t>(_configuration.applicationId);
  if (state.identifier.entity == 0) state.identifier.entity = 65000;
  return _gateway.publish({state});
}

tactical::dis::Result DisStartupSession::poll() { return _gateway.poll(); }

std::vector<tactical::dis::RemoteEntityChange>
DisStartupSession::takeRemoteEntityChanges() {
  return _gateway.takeRemoteEntityChanges();
}

std::vector<tactical::dis::WarfareEvent>
DisStartupSession::takeRemoteWarfareEvents() {
  return _gateway.takeRemoteWarfareEvents();
}

std::uint64_t DisStartupSession::publishedPduCount() const {
  return _gateway.publishedPduCount();
}

std::uint64_t DisStartupSession::receivedPduCount() const {
  return _gateway.receivedPduCount();
}

qsizetype DisStartupSession::remoteEntityCount() const {
  return _gateway.remoteEntityCount();
}

std::uint16_t DisStartupSession::entityNumberFor(const QString& stableId) {
  const auto existing = _entityNumbers.constFind(stableId);
  if (existing != _entityNumbers.cend()) return existing.value();
  if (stableId.startsWith(QStringLiteral("managed:"))) {
    const auto requested = parseDisIdentifier(stableId.mid(8));
    if (requested.site == _configuration.siteId && requested.application == _configuration.applicationId) {
      if (requested.entity == 0 || requested.entity >= 32768 || _entityNumbers.values().contains(requested.entity)) return 0;
      _entityNumbers.insert(stableId, requested.entity);
      return requested.entity;
    }
  }
  while (_entityNumbers.values().contains(_nextEntityNumber) && _nextEntityNumber < 32768) ++_nextEntityNumber;
  if (_nextEntityNumber >= 32768) return 0;
  const std::uint16_t assigned = _nextEntityNumber++;
  _entityNumbers.insert(stableId, assigned);
  return assigned;
}

} // namespace application
