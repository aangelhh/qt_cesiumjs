#include "application/HlaStartupSession.h"

#include "infrastructure/interoperability/hla/HlaBackendFactory.h"

#include <utility>
#include <algorithm>

namespace application {

tactical::hla::Result HlaStartupSession::start(
    const HlaStartupConfiguration& configuration) {
  this->stop();

  tactical::hla::BackendConfiguration backendConfiguration;
  backendConfiguration.adapter = tactical::hla::BackendAdapter::SharedLibrary;
  backendConfiguration.expectedBackendId = configuration.backendId.toStdString();
  backendConfiguration.libraryPath =
      configuration.backendLibraryPath.toStdString();
  tactical::hla::BackendCreationResult backendResult =
      tactical::hla::HlaBackendFactory::create(backendConfiguration);
  if (!backendResult) {
    return tactical::hla::Result::failure(backendResult.error);
  }

  tactical::hla::SessionConfiguration sessionConfiguration;
  sessionConfiguration.localSettingsDesignator =
      configuration.localSettingsDesignator.toStdString();
  sessionConfiguration.federationName = configuration.federationName.toStdString();
  sessionConfiguration.federateName = configuration.federateName.toStdString();
  sessionConfiguration.federateType = configuration.federateType.toStdString();
  sessionConfiguration.createFederationIfMissing =
      configuration.createFederationIfMissing;
  sessionConfiguration.fomModules.reserve(
      static_cast<std::size_t>(configuration.fomModules.size()));
  for (const QString& module : configuration.fomModules) {
    sessionConfiguration.fomModules.push_back(module.toStdString());
  }

  auto runtime = std::make_unique<tactical::hla::HlaRuntime>(
      std::move(backendResult.backend));
  const tactical::hla::Result result = runtime->start(sessionConfiguration);
  if (!result.success) {
    return result;
  }

  _backendId = configuration.backendId;
  _runtime = std::move(runtime);
  _inboundAdapter = std::make_unique<tactical::hla::HlaInboundAdapter>();
  _runtime->setEventSink(_inboundAdapter.get());
  const std::vector<std::string> entityAttributes =
      tactical::hla::RprFomEncoding::publishedAttributeNames();
  const std::vector<std::string> platformClasses = {
      tactical::hla::RprFomEncoding::objectClassName("Air"),
      tactical::hla::RprFomEncoding::objectClassName("Ground"),
      tactical::hla::RprFomEncoding::objectClassName("Surface"),
      tactical::hla::RprFomEncoding::objectClassName("Subsurface"),
      tactical::hla::RprFomEncoding::objectClassName("Space"),
      tactical::hla::RprFomEncoding::objectClassName("Unknown")};
  for (const std::string& objectClassName : platformClasses) {
    const tactical::hla::Result subscribeResult =
        _runtime->subscribeObjectClass(objectClassName, entityAttributes);
    if (!subscribeResult.success) {
      this->stop();
      return subscribeResult;
    }
  }
  const tactical::hla::Result startResumeResult =
      _runtime->subscribeInteractionClass(
          "HLAinteractionRoot.StartResume",
          {"OriginatingEntity", "ReceivingEntity", "RealWorldTime",
           "RequestIdentifier", "SimulationTime"});
  if (!startResumeResult.success) {
    this->stop();
    return startResumeResult;
  }
  const tactical::hla::Result stopFreezeResult =
      _runtime->subscribeInteractionClass(
          "HLAinteractionRoot.StopFreeze",
          {"OriginatingEntity", "ReceivingEntity", "RequestIdentifier",
           "RealWorldTime", "Reason", "ReflectValues",
           "RunInternalSimulationClock", "UpdateAttributes"});
  if (!stopFreezeResult.success) {
    this->stop();
    return stopFreezeResult;
  }
  _entityPublisher =
      std::make_unique<tactical::hla::HlaEntityPublisher>(*_runtime);
  _warfarePublisher =
      std::make_unique<tactical::hla::HlaWarfarePublisher>(*_runtime);
  _simulationControlPublisher =
      std::make_unique<tactical::hla::HlaSimulationControlPublisher>(*_runtime);
  _sensorPublisher =
      std::make_unique<tactical::hla::HlaSensorPublisher>(*_runtime);
  return tactical::hla::Result::ok();
}

std::vector<tactical::hla::RemoteEntityChange>
HlaStartupSession::takeRemoteEntityChanges() {
  return _inboundAdapter ? _inboundAdapter->takeEntityChanges()
                         : std::vector<tactical::hla::RemoteEntityChange>{};
}

std::vector<tactical::hla::RemoteSimulationControl>
HlaStartupSession::takeRemoteSimulationControls() {
  return _inboundAdapter ? _inboundAdapter->takeSimulationControls()
                         : std::vector<tactical::hla::RemoteSimulationControl>{};
}

tactical::hla::Result HlaStartupSession::publishSimulationControl(
    tactical::hla::RemoteSimulationControl control,
    double simulationTimeSeconds) {
  if (!_simulationControlPublisher) {
    return tactical::hla::Result::failure(
        "HLA simulation control publisher is not active");
  }
  return _simulationControlPublisher->publish(control, simulationTimeSeconds);
}

tactical::hla::Result HlaStartupSession::poll(double maximumSeconds) {
  if (!_runtime) {
    return tactical::hla::Result::failure("HLA session is not active");
  }
  return _runtime->poll(maximumSeconds);
}

tactical::hla::Result HlaStartupSession::publishEntities(
    const QVector<Entity>& entities) {
  if (!_entityPublisher) {
    return tactical::hla::Result::failure("HLA entity publisher is not active");
  }

  std::vector<tactical::hla::RprEntityState> states;
  states.reserve(static_cast<std::size_t>(entities.size()));
  for (const Entity& entity : entities) {
    if (entity.externallyControlled) continue;
    tactical::hla::RprEntityState state;
    state.stableId = entity.entityId.toStdString();
    state.name = entity.name.toStdString();
    state.domain = entity.domain.toStdString();
    state.entityKind = static_cast<std::uint8_t>(
        std::clamp(entity.entityKind, 0, 255));
    state.entityDomain = static_cast<std::uint8_t>(
        std::clamp(entity.entityDomain, 0, 255));
    state.countryCode = static_cast<std::uint16_t>(
        std::clamp(entity.entityCountry, 0, 65535));
    state.category = static_cast<std::uint8_t>(
        std::clamp(entity.entityCategory, 0, 255));
    state.subcategory = static_cast<std::uint8_t>(
        std::clamp(entity.entitySubcategory, 0, 255));
    state.specific = static_cast<std::uint8_t>(
        std::clamp(entity.entitySpecific, 0, 255));
    state.extra = static_cast<std::uint8_t>(
        std::clamp(entity.entityExtra, 0, 255));
    state.forceIdentifier = static_cast<std::uint8_t>(
        std::clamp(entity.forceIdentifier, 0, 255));
    state.latitudeDegrees = entity.latitude;
    state.longitudeDegrees = entity.longitude;
    state.altitudeMeters = entity.altitude;
    state.headingDegrees = entity.headingDegrees;
    state.pitchDegrees = entity.pitchDegrees;
    state.rollDegrees = entity.rollDegrees;
    state.speedKnots = entity.speedKnots;
    state.damagePercent = entity.damagePercent;
    state.destroyed = entity.destroyed;
    states.push_back(std::move(state));
  }
  return _entityPublisher->synchronize(states);
}

tactical::hla::Result HlaStartupSession::publishMunitions(
    const QVector<ActiveMunition>& activeMunitions) {
  if (!_warfarePublisher) {
    return tactical::hla::Result::failure("HLA warfare publisher is not active");
  }
  std::vector<tactical::hla::RprWeaponFireState> states;
  states.reserve(static_cast<std::size_t>(activeMunitions.size()));
  for (const ActiveMunition& munition : activeMunitions) {
    tactical::hla::RprWeaponFireState state;
    state.stableId = munition.id.toStdString();
    state.munitionType = munition.munitionType.toStdString();
    state.latitudeDegrees = munition.latitude;
    state.longitudeDegrees = munition.longitude;
    state.altitudeMeters = munition.altitudeMeters;
    state.headingDegrees = munition.headingDegrees;
    state.pitchDegrees = munition.pitchDegrees;
    state.speedMetersPerSecond = munition.speedMetersPerSecond;
    states.push_back(std::move(state));
  }
  return _warfarePublisher->synchronize(states);
}

tactical::hla::Result HlaStartupSession::publishDetonations(
    const QVector<TransientEffect>& transientEffects) {
  if (!_warfarePublisher) {
    return tactical::hla::Result::failure("HLA warfare publisher is not active");
  }
  std::vector<tactical::hla::RprMunitionDetonationState> states;
  for (const TransientEffect& effect : transientEffects) {
    const bool missileImpact = effect.effectType == QStringLiteral("ImpactFlash");
    const bool bombImpact =
        effect.effectType == QStringLiteral("BombImpactFlash");
    if (!missileImpact && !bombImpact) continue;
    tactical::hla::RprMunitionDetonationState state;
    state.effectId = effect.id.toStdString();
    const QString suffix = bombImpact
        ? QStringLiteral("-bombimpactflash")
        : QStringLiteral("-impact");
    state.munitionStableId = effect.id.chopped(suffix.size()).toStdString();
    state.munitionType = bombImpact ? "Bomb" : "Missile";
    state.latitudeDegrees = effect.latitude;
    state.longitudeDegrees = effect.longitude;
    state.altitudeMeters = effect.altitudeMeters;
    states.push_back(std::move(state));
  }
  return _warfarePublisher->synchronizeDetonations(states);
}

tactical::hla::Result HlaStartupSession::publishSensors(
    const QVector<Entity>& entities) {
  if (!_sensorPublisher) {
    return tactical::hla::Result::failure("HLA sensor publisher is not active");
  }
  std::vector<tactical::hla::RprSensorState> states;
  for (const Entity& entity : entities) {
    if (entity.externallyControlled) continue;
    for (const SensorDefinition& definition : entity.sensors) {
      if (definition.sensorType.compare(
              QStringLiteral("radar"), Qt::CaseInsensitive) != 0) {
        continue;
      }
      tactical::hla::RprSensorState state;
      state.entityStableId = entity.entityId.toStdString();
      state.sensorId = definition.id.toStdString();
      state.enabled = definition.enabled && !entity.destroyed;
      state.emitting = definition.emitting && state.enabled;
      state.azimuthCenterDegrees = definition.azimuthCenterDegrees;
      state.azimuthWidthDegrees = definition.azimuthWidthDegrees;
      state.elevationCenterDegrees = definition.elevationCenterDegrees;
      state.elevationWidthDegrees = definition.elevationWidthDegrees;
      state.frequencyHertz = definition.radarProfile.frequencyHertz;
      state.bandwidthHertz = definition.radarProfile.bandwidthHertz;
      state.peakPowerWatts = definition.radarProfile.peakPowerWatts;
      state.hasTracks = std::any_of(
          entity.sensorContacts.begin(),
          entity.sensorContacts.end(),
          [&definition](const SensorContact& contact) {
            return contact.detected && contact.sensorId == definition.id;
          });
      states.push_back(std::move(state));
    }
  }
  return _sensorPublisher->synchronize(states);
}

tactical::hla::Result HlaStartupSession::stop() {
  if (!_runtime) {
    return tactical::hla::Result::ok();
  }
  if (_sensorPublisher) {
    _sensorPublisher->removeAll();
    _sensorPublisher.reset();
  }
  if (_entityPublisher) {
    _entityPublisher->removeAll();
    _entityPublisher.reset();
  }
  _warfarePublisher.reset();
  _simulationControlPublisher.reset();
  _runtime->setEventSink(nullptr);
  _inboundAdapter.reset();
  const tactical::hla::Result result = _runtime->stop();
  _runtime.reset();
  _backendId.clear();
  return result;
}

bool HlaStartupSession::isActive() const {
  return _runtime &&
      _runtime->state() == tactical::hla::BackendState::Joined;
}

QString HlaStartupSession::backendId() const {
  return _backendId;
}

} // namespace application
