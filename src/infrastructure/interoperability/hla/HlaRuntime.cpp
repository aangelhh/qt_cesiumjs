#include "infrastructure/interoperability/hla/HlaRuntime.h"

#include <cmath>
#include <utility>

namespace tactical::hla {

HlaRuntime::HlaRuntime(std::unique_ptr<IHlaBackend> backend)
    : _backend(std::move(backend)) {}

HlaRuntime::~HlaRuntime() {
  this->stop();
}

Result HlaRuntime::start(const SessionConfiguration& configuration) {
  if (!_backend) {
    return Result::failure("No HLA backend was provided");
  }
  if (configuration.federationName.empty()) {
    return Result::failure("Federation name is required");
  }
  if (configuration.federateName.empty()) {
    return Result::failure("Federate name is required");
  }
  if (configuration.federateType.empty()) {
    return Result::failure("Federate type is required");
  }
  if (_backend->state() != BackendState::Disconnected) {
    return Result::failure("HLA backend is not disconnected");
  }

  Result result = _backend->connect(configuration.localSettingsDesignator);
  if (!result.success) {
    return result;
  }

  if (configuration.createFederationIfMissing) {
    result = _backend->createFederation(
        configuration.federationName,
        configuration.fomModules);
    if (!result.success) {
      return this->rollback(result);
    }
  }

  result = _backend->joinFederation(
      configuration.federateName,
      configuration.federateType,
      configuration.federationName,
      {});
  if (!result.success) {
    return this->rollback(result);
  }

  return Result::ok();
}

Result HlaRuntime::poll(double maximumSeconds) {
  if (!_backend) {
    return Result::failure("No HLA backend was provided");
  }
  if (_backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  if (maximumSeconds < 0.0) {
    return Result::failure("Callback polling duration cannot be negative");
  }
  return _backend->poll(maximumSeconds);
}

Result HlaRuntime::publishObjectClass(
    const std::string& objectClassName,
    const std::vector<std::string>& attributeNames) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->publishObjectClass(objectClassName, attributeNames);
}

Result HlaRuntime::subscribeObjectClass(
    const std::string& objectClassName,
    const std::vector<std::string>& attributeNames) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->subscribeObjectClass(objectClassName, attributeNames);
}

Result HlaRuntime::registerObjectInstance(
    const std::string& objectClassName,
    const std::string& instanceName,
    ObjectInstanceId& instanceId) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->registerObjectInstance(
      objectClassName, instanceName, instanceId);
}

Result HlaRuntime::updateObjectAttributes(
    ObjectInstanceId instanceId,
    const std::vector<NamedValue>& attributes,
    const ByteBuffer& tag) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->updateObjectAttributes(instanceId, attributes, tag);
}

Result HlaRuntime::deleteObjectInstance(
    ObjectInstanceId instanceId,
    const ByteBuffer& tag) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->deleteObjectInstance(instanceId, tag);
}

Result HlaRuntime::publishInteractionClass(
    const std::string& interactionClassName) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->publishInteractionClass(interactionClassName);
}

Result HlaRuntime::subscribeInteractionClass(
    const std::string& interactionClassName,
    const std::vector<std::string>& parameterNames) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->subscribeInteractionClass(
      interactionClassName, parameterNames);
}

Result HlaRuntime::sendInteraction(
    const std::string& interactionClassName,
    const std::vector<NamedValue>& parameters,
    const ByteBuffer& tag) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->sendInteraction(interactionClassName, parameters, tag);
}

Result HlaRuntime::registerSynchronizationPoint(
    const std::string& label,
    const ByteBuffer& tag) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  if (label.empty()) {
    return Result::failure("HLA synchronization point label is required");
  }
  return _backend->registerSynchronizationPoint(label, tag);
}

Result HlaRuntime::achieveSynchronizationPoint(const std::string& label) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  if (label.empty()) {
    return Result::failure("HLA synchronization point label is required");
  }
  return _backend->achieveSynchronizationPoint(label);
}

Result HlaRuntime::enableTimeRegulation(double lookaheadSeconds) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  if (!std::isfinite(lookaheadSeconds) || lookaheadSeconds <= 0.0) {
    return Result::failure("HLA time lookahead must be positive and finite");
  }
  return _backend->enableTimeRegulation(lookaheadSeconds);
}

Result HlaRuntime::enableTimeConstrained() {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  return _backend->enableTimeConstrained();
}

Result HlaRuntime::requestTimeAdvance(double logicalTimeSeconds) {
  if (!_backend || _backend->state() != BackendState::Joined) {
    return Result::failure("HLA federate is not joined");
  }
  if (!std::isfinite(logicalTimeSeconds) || logicalTimeSeconds < 0.0) {
    return Result::failure("HLA requested logical time must be finite and non-negative");
  }
  return _backend->requestTimeAdvance(logicalTimeSeconds);
}

void HlaRuntime::setEventSink(IHlaEventSink* eventSink) {
  if (_backend) {
    _backend->setEventSink(eventSink);
  }
}

Result HlaRuntime::stop() {
  if (!_backend) {
    return Result::ok();
  }

  Result firstFailure = Result::ok();
  if (_backend->state() == BackendState::Joined) {
    const Result resignResult = _backend->resign();
    if (!resignResult.success) {
      firstFailure = resignResult;
    }
  }

  if (_backend->state() == BackendState::Connected ||
      _backend->state() == BackendState::Error) {
    const Result disconnectResult = _backend->disconnect();
    if (firstFailure.success && !disconnectResult.success) {
      firstFailure = disconnectResult;
    }
  }
  return firstFailure;
}

const IHlaBackend* HlaRuntime::backend() const {
  return _backend.get();
}

BackendState HlaRuntime::state() const {
  return _backend ? _backend->state() : BackendState::Unavailable;
}

Result HlaRuntime::rollback(const Result& originalFailure) {
  if (_backend && (_backend->state() == BackendState::Connected ||
                   _backend->state() == BackendState::Error)) {
    _backend->disconnect();
  }
  return originalFailure;
}

} // namespace tactical::hla
