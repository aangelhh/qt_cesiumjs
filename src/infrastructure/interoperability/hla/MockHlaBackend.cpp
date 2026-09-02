#include "infrastructure/interoperability/hla/MockHlaBackend.h"

#include <utility>
#include <algorithm>

namespace tactical::hla {

std::string MockHlaBackend::id() const { return "mock"; }

std::string MockHlaBackend::version() const { return "1.0"; }

std::vector<std::string> MockHlaBackend::capabilities() const {
  return {"federation-management", "evoked-callbacks", "test-backend"};
}

Result MockHlaBackend::connect(const std::string&) {
  const Result beginResult = this->begin(Operation::Connect);
  if (!beginResult.success) {
    return beginResult;
  }
  if (_state != BackendState::Disconnected) {
    return this->fail("Mock backend is already connected");
  }
  _state = BackendState::Connected;
  return Result::ok();
}

Result MockHlaBackend::createFederation(
    const std::string& federationName,
    const std::vector<std::string>&) {
  const Result beginResult = this->begin(Operation::CreateFederation);
  if (!beginResult.success) {
    return beginResult;
  }
  if (_state != BackendState::Connected) {
    return this->fail("Mock backend is not connected");
  }
  if (federationName.empty()) {
    return this->fail("Federation name is empty");
  }
  return Result::ok();
}

Result MockHlaBackend::joinFederation(
    const std::string& federateName,
    const std::string& federateType,
    const std::string& federationName,
    const std::vector<std::string>&) {
  const Result beginResult = this->begin(Operation::JoinFederation);
  if (!beginResult.success) {
    return beginResult;
  }
  if (_state != BackendState::Connected) {
    return this->fail("Mock backend is not connected");
  }
  if (federateName.empty() || federateType.empty() || federationName.empty()) {
    return this->fail("Federate and federation identifiers are required");
  }
  _state = BackendState::Joined;
  return Result::ok();
}

Result MockHlaBackend::publishObjectClass(
    const std::string& objectClassName,
    const std::vector<std::string>& attributeNames) {
  const Result beginResult = this->begin(Operation::PublishObjectClass);
  if (!beginResult.success) return beginResult;
  if (_state != BackendState::Joined || objectClassName.empty() ||
      attributeNames.empty()) {
    return this->fail("Mock object publication is invalid");
  }
  return Result::ok();
}

Result MockHlaBackend::registerObjectInstance(
    const std::string& objectClassName,
    const std::string& instanceName,
    ObjectInstanceId& instanceId) {
  const Result beginResult = this->begin(Operation::RegisterObjectInstance);
  if (!beginResult.success) return beginResult;
  if (_state != BackendState::Joined || objectClassName.empty() ||
      instanceName.empty()) {
    return this->fail("Mock object registration is invalid");
  }
  instanceId = _nextObjectInstanceId++;
  _objectInstances.push_back(instanceId);
  return Result::ok();
}

Result MockHlaBackend::updateObjectAttributes(
    ObjectInstanceId instanceId,
    const std::vector<NamedValue>& attributes,
    const ByteBuffer&) {
  const Result beginResult = this->begin(Operation::UpdateObjectAttributes);
  if (!beginResult.success) return beginResult;
  if (_state != BackendState::Joined || attributes.empty() ||
      std::find(_objectInstances.begin(), _objectInstances.end(), instanceId) ==
          _objectInstances.end()) {
    return this->fail("Mock object update is invalid");
  }
  return Result::ok();
}

Result MockHlaBackend::deleteObjectInstance(
    ObjectInstanceId instanceId,
    const ByteBuffer&) {
  const Result beginResult = this->begin(Operation::DeleteObjectInstance);
  if (!beginResult.success) return beginResult;
  const auto iterator = std::find(
      _objectInstances.begin(), _objectInstances.end(), instanceId);
  if (_state != BackendState::Joined || iterator == _objectInstances.end()) {
    return this->fail("Mock object deletion is invalid");
  }
  _objectInstances.erase(iterator);
  return Result::ok();
}

Result MockHlaBackend::publishInteractionClass(
    const std::string& interactionClassName) {
  const Result beginResult = this->begin(Operation::PublishInteractionClass);
  if (!beginResult.success) return beginResult;
  if (_state != BackendState::Joined || interactionClassName.empty()) {
    return this->fail("Mock interaction publication is invalid");
  }
  return Result::ok();
}

Result MockHlaBackend::sendInteraction(
    const std::string& interactionClassName,
    const std::vector<NamedValue>& parameters,
    const ByteBuffer&) {
  const Result beginResult = this->begin(Operation::SendInteraction);
  if (!beginResult.success) return beginResult;
  if (_state != BackendState::Joined || interactionClassName.empty()) {
    return this->fail("Mock interaction is invalid");
  }
  return Result::ok();
}

Result MockHlaBackend::poll(double maximumSeconds) {
  const Result beginResult = this->begin(Operation::Poll);
  if (!beginResult.success) {
    return beginResult;
  }
  if (_state != BackendState::Joined) {
    return this->fail("Mock federate is not joined");
  }
  if (maximumSeconds < 0.0) {
    return this->fail("Polling duration cannot be negative");
  }
  return Result::ok();
}

Result MockHlaBackend::resign() {
  const Result beginResult = this->begin(Operation::Resign);
  if (!beginResult.success) {
    return beginResult;
  }
  if (_state != BackendState::Joined) {
    return this->fail("Mock federate is not joined");
  }
  _state = BackendState::Connected;
  return Result::ok();
}

Result MockHlaBackend::disconnect() {
  const Result beginResult = this->begin(Operation::Disconnect);
  if (!beginResult.success) {
    return beginResult;
  }
  if (_state == BackendState::Joined) {
    return this->fail("Mock federate must resign before disconnecting");
  }
  _state = BackendState::Disconnected;
  return Result::ok();
}

BackendState MockHlaBackend::state() const { return _state; }

std::string MockHlaBackend::lastError() const { return _lastError; }

void MockHlaBackend::failNext(Operation operation, std::string message) {
  _failureOperation = operation;
  _failureMessage = std::move(message);
}

const std::vector<MockHlaBackend::Operation>& MockHlaBackend::operations() const {
  return _operations;
}

Result MockHlaBackend::begin(Operation operation) {
  _operations.push_back(operation);
  if (_failureOperation && *_failureOperation == operation) {
    _failureOperation.reset();
    return this->fail(std::move(_failureMessage));
  }
  _lastError.clear();
  return Result::ok();
}

Result MockHlaBackend::fail(std::string message) {
  _lastError = std::move(message);
  _state = BackendState::Error;
  return Result::failure(_lastError);
}

} // namespace tactical::hla
