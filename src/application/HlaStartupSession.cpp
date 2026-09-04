#include "application/HlaStartupSession.h"

#include "domain/EntityIdentity.h"
#include "infrastructure/interoperability/hla/HlaBackendFactory.h"

#include <utility>
#include <algorithm>
#include <chrono>
#include <thread>

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
  const tactical::hla::Result munitionSubscribeResult =
      _runtime->subscribeObjectClass(
          "HLAobjectRoot.BaseEntity.PhysicalEntity.Munition",
          entityAttributes);
  if (!munitionSubscribeResult.success) {
    this->stop();
    return munitionSubscribeResult;
  }
  const tactical::hla::Result emitterSubscribeResult =
      _runtime->subscribeObjectClass(
          "HLAobjectRoot.EmbeddedSystem.EmitterSystem",
          {"EntityIdentifier", "HostObjectIdentifier", "RelativePosition",
           "EmitterFunctionCode", "EmitterType", "EmitterIndex",
           "EventIdentifier"});
  if (!emitterSubscribeResult.success) {
    this->stop();
    return emitterSubscribeResult;
  }
  const tactical::hla::Result beamSubscribeResult =
      _runtime->subscribeObjectClass(
          "HLAobjectRoot.EmitterBeam.RadarBeam",
          {"BeamAzimuthCenter", "BeamAzimuthSweep",
           "BeamElevationCenter", "BeamElevationSweep", "BeamFunctionCode",
           "BeamIdentifier", "BeamParameterIndex", "EffectiveRadiatedPower",
           "EmissionFrequency", "EmitterSystemIdentifier", "EventIdentifier",
           "FrequencyRange", "SweepSynch", "HighDensityTrack",
           "TrackObjectIdentifiers"});
  if (!beamSubscribeResult.success) {
    this->stop();
    return beamSubscribeResult;
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
  const tactical::hla::Result weaponFireResult =
      _runtime->subscribeInteractionClass(
          "HLAinteractionRoot.WeaponFire",
          {"EventIdentifier", "FireControlSolutionRange", "FireMissionIndex",
           "FiringLocation", "FiringObjectIdentifier", "FuseType",
           "InitialVelocityVector", "MunitionObjectIdentifier",
           "MunitionType", "QuantityFired", "RateOfFire",
           "TargetObjectIdentifier", "WarheadType"});
  if (!weaponFireResult.success) {
    this->stop();
    return weaponFireResult;
  }
  const tactical::hla::Result detonationResult =
      _runtime->subscribeInteractionClass(
          "HLAinteractionRoot.MunitionDetonation",
          {"DetonationLocation", "DetonationResultCode", "EventIdentifier",
           "FiringObjectIdentifier", "FinalVelocityVector", "FuseType",
           "MunitionObjectIdentifier", "MunitionType", "QuantityFired",
           "RateOfFire", "RelativeDetonationLocation",
           "TargetObjectIdentifier", "WarheadType"});
  if (!detonationResult.success) {
    this->stop();
    return detonationResult;
  }
  _entityPublisher =
      std::make_unique<tactical::hla::HlaEntityPublisher>(*_runtime);
  _warfarePublisher =
      std::make_unique<tactical::hla::HlaWarfarePublisher>(*_runtime);
  _simulationControlPublisher =
      std::make_unique<tactical::hla::HlaSimulationControlPublisher>(*_runtime);
  _sensorPublisher =
      std::make_unique<tactical::hla::HlaSensorPublisher>(*_runtime);
  if (configuration.timeManagementEnabled) {
    const auto awaitTimeEvent =
        [this](tactical::hla::RemoteTimeManagementEventKind expectedKind) {
          const auto deadline = std::chrono::steady_clock::now() +
                                std::chrono::seconds(3);
          while (std::chrono::steady_clock::now() < deadline) {
            const tactical::hla::Result pollResult = _runtime->poll(0.05);
            if (!pollResult.success) return pollResult;
            for (const tactical::hla::RemoteTimeManagementEvent& event :
                 _inboundAdapter->takeTimeManagementEvents()) {
              _grantedLogicalTimeSeconds = event.logicalTimeSeconds;
              if (event.kind == expectedKind) {
                return tactical::hla::Result::ok();
              }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
          }
          return tactical::hla::Result::failure(
              "Timed out enabling HLA time management");
        };

    tactical::hla::Result timeResult = _runtime->enableTimeRegulation(
        configuration.timeLookaheadSeconds);
    if (!timeResult.success) {
      this->stop();
      return timeResult;
    }
    timeResult = awaitTimeEvent(
        tactical::hla::RemoteTimeManagementEventKind::RegulationEnabled);
    if (!timeResult.success) {
      this->stop();
      return timeResult;
    }
    timeResult = _runtime->enableTimeConstrained();
    if (!timeResult.success) {
      this->stop();
      return timeResult;
    }
    timeResult = awaitTimeEvent(
        tactical::hla::RemoteTimeManagementEventKind::ConstrainedEnabled);
    if (!timeResult.success) {
      this->stop();
      return timeResult;
    }
    _timeManagementActive = true;
  }
  return tactical::hla::Result::ok();
}

std::vector<tactical::hla::RemoteEntityChange>
HlaStartupSession::takeRemoteEntityChanges() {
  return _inboundAdapter ? _inboundAdapter->takeEntityChanges()
                         : std::vector<tactical::hla::RemoteEntityChange>{};
}

std::vector<tactical::hla::RemoteMunitionChange>
HlaStartupSession::takeRemoteMunitionChanges() {
  return _inboundAdapter ? _inboundAdapter->takeMunitionChanges()
                         : std::vector<tactical::hla::RemoteMunitionChange>{};
}

std::vector<tactical::hla::RemoteSensorChange>
HlaStartupSession::takeRemoteSensorChanges() {
  return _inboundAdapter ? _inboundAdapter->takeSensorChanges()
                         : std::vector<tactical::hla::RemoteSensorChange>{};
}

std::vector<tactical::hla::RemoteWarfareEvent>
HlaStartupSession::takeRemoteWarfareEvents() {
  return _inboundAdapter ? _inboundAdapter->takeWarfareEvents()
                         : std::vector<tactical::hla::RemoteWarfareEvent>{};
}

std::vector<tactical::hla::RemoteSynchronizationChange>
HlaStartupSession::takeRemoteSynchronizationChanges() {
  return _inboundAdapter
      ? _inboundAdapter->takeSynchronizationChanges()
      : std::vector<tactical::hla::RemoteSynchronizationChange>{};
}

std::vector<tactical::hla::RemoteTimeManagementEvent>
HlaStartupSession::takeRemoteTimeManagementEvents() {
  std::vector<tactical::hla::RemoteTimeManagementEvent> events =
      _inboundAdapter
          ? _inboundAdapter->takeTimeManagementEvents()
          : std::vector<tactical::hla::RemoteTimeManagementEvent>{};
  for (const tactical::hla::RemoteTimeManagementEvent& event : events) {
    if (event.kind ==
        tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted) {
      _grantedLogicalTimeSeconds = event.logicalTimeSeconds;
      _timeAdvancePending = false;
    }
  }
  return events;
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

tactical::hla::Result HlaStartupSession::registerSynchronizationPoint(
    const std::string& label,
    const tactical::hla::ByteBuffer& tag) {
  return _runtime
      ? _runtime->registerSynchronizationPoint(label, tag)
      : tactical::hla::Result::failure("HLA session is not active");
}

tactical::hla::Result HlaStartupSession::achieveSynchronizationPoint(
    const std::string& label) {
  return _runtime
      ? _runtime->achieveSynchronizationPoint(label)
      : tactical::hla::Result::failure("HLA session is not active");
}

tactical::hla::Result HlaStartupSession::requestTimeAdvance(
    double logicalTimeSeconds) {
  if (!_runtime || !_timeManagementActive) {
    return tactical::hla::Result::failure(
        "HLA time management is not active");
  }
  if (_timeAdvancePending) {
    return tactical::hla::Result::failure(
        "An HLA time advance request is already pending");
  }
  if (logicalTimeSeconds <= _grantedLogicalTimeSeconds) {
    return tactical::hla::Result::failure(
        "HLA requested time must be greater than granted time");
  }
  const tactical::hla::Result result =
      _runtime->requestTimeAdvance(logicalTimeSeconds);
  if (result.success) _timeAdvancePending = true;
  return result;
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
  const auto objectInstanceName = [](const QString& entityId) {
    const std::string stableId = entityId.trimmed().toStdString();
    if (stableId.empty()) return stableId;
    constexpr const char* remotePrefix = "hla:";
    if (stableId.rfind(remotePrefix, 0) == 0) return stableId.substr(4);
    return tactical::hla::RprFomEncoding::objectInstanceName(stableId);
  };
  for (const ActiveMunition& munition : activeMunitions) {
    tactical::hla::RprWeaponFireState state;
    state.stableId = munition.id.toStdString();
    state.munitionType = munition.munitionType.toStdString();
    state.firingObjectInstanceName = objectInstanceName(
        munition.launcherEntityId);
    state.targetObjectInstanceName = objectInstanceName(
        munition.targetEntityId);
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
  const auto objectInstanceName = [](const Entity& entity) {
    const std::string stableId = domain::entityKey(entity).toStdString();
    constexpr const char* remotePrefix = "hla:";
    if (entity.externallyControlled &&
        stableId.rfind(remotePrefix, 0) == 0) {
      return stableId.substr(4);
    }
    return tactical::hla::RprFomEncoding::objectInstanceName(stableId);
  };
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
      for (const SensorContact& contact : entity.sensorContacts) {
        if (!contact.detected || contact.sensorId != definition.id) continue;
        const QString targetReference = contact.targetEntityId.isEmpty()
            ? contact.targetEntityName
            : contact.targetEntityId;
        const auto target = std::find_if(
            entities.cbegin(), entities.cend(),
            [&targetReference](const Entity& value) {
              return domain::entityMatchesReference(value, targetReference);
            });
        if (target != entities.cend()) {
          state.trackedObjectInstanceNames.push_back(
              objectInstanceName(*target));
        }
      }
      state.hasTracks = !state.trackedObjectInstanceNames.empty();
      states.push_back(std::move(state));
    }
  }
  return _sensorPublisher->synchronize(states);
}

tactical::hla::Result HlaStartupSession::stop() {
  _timeManagementActive = false;
  _timeAdvancePending = false;
  _grantedLogicalTimeSeconds = 0.0;
  if (!_runtime) {
    return tactical::hla::Result::ok();
  }
  if (_sensorPublisher) {
    _sensorPublisher->removeAll();
    _sensorPublisher.reset();
  }
  if (_warfarePublisher) {
    _warfarePublisher->removeAll();
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

bool HlaStartupSession::isTimeManagementActive() const {
  return _timeManagementActive;
}

double HlaStartupSession::grantedLogicalTimeSeconds() const {
  return _grantedLogicalTimeSeconds;
}

QString HlaStartupSession::backendId() const {
  return _backendId;
}

} // namespace application
