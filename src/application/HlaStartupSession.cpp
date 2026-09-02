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
  _entityPublisher =
      std::make_unique<tactical::hla::HlaEntityPublisher>(*_runtime);
  _warfarePublisher =
      std::make_unique<tactical::hla::HlaWarfarePublisher>(*_runtime);
  return tactical::hla::Result::ok();
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

tactical::hla::Result HlaStartupSession::stop() {
  if (!_runtime) {
    return tactical::hla::Result::ok();
  }
  if (_entityPublisher) {
    _entityPublisher->removeAll();
    _entityPublisher.reset();
  }
  _warfarePublisher.reset();
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
