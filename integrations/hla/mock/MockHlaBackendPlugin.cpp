#include "infrastructure/interoperability/hla/HlaBackendPluginApi.h"

#include <string>
#include <set>

namespace {

struct MockSession {
  QttestHlaBackendStateV1 state = QTTEST_HLA_STATE_DISCONNECTED;
  std::string error;
  uint64_t nextObjectId = 1;
  std::set<uint64_t> objects;
  QttestHlaCallbacksV5 callbacks = {};
  bool timeRegulating = false;
  bool timeConstrained = false;
  double logicalTimeSeconds = 0.0;
};

MockSession* session(QttestHlaBackendHandle handle) {
  return static_cast<MockSession*>(handle);
}

int fail(MockSession* value, std::string message) {
  if (value) {
    value->error = std::move(message);
    value->state = QTTEST_HLA_STATE_ERROR;
  }
  return 1;
}

QttestHlaBackendHandle createBackend() {
  return new MockSession();
}

void destroyBackend(QttestHlaBackendHandle handle) {
  delete session(handle);
}

int connectBackend(QttestHlaBackendHandle handle, const char*) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_DISCONNECTED) {
    return fail(value, "Mock plugin is already connected");
  }
  value->error.clear();
  value->state = QTTEST_HLA_STATE_CONNECTED;
  return 0;
}

int createFederation(
    QttestHlaBackendHandle handle,
    const char* federationName,
    const QttestHlaStringArrayV1*) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_CONNECTED) {
    return fail(value, "Mock plugin is not connected");
  }
  if (!federationName || !*federationName) {
    return fail(value, "Federation name is empty");
  }
  value->error.clear();
  return 0;
}

int joinFederation(
    QttestHlaBackendHandle handle,
    const char* federateName,
    const char* federateType,
    const char* federationName,
    const QttestHlaStringArrayV1*) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_CONNECTED) {
    return fail(value, "Mock plugin is not connected");
  }
  if (!federateName || !*federateName || !federateType || !*federateType ||
      !federationName || !*federationName) {
    return fail(value, "Federate and federation identifiers are required");
  }
  value->error.clear();
  value->state = QTTEST_HLA_STATE_JOINED;
  return 0;
}

int publishObjectClass(
    QttestHlaBackendHandle handle,
    const char* className,
    const QttestHlaStringArrayV1* attributes) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !className ||
      !*className || !attributes || attributes->count == 0) {
    return fail(value, "Invalid mock object publication");
  }
  value->error.clear();
  return 0;
}

int subscribeObjectClass(
    QttestHlaBackendHandle handle,
    const char* className,
    const QttestHlaStringArrayV1* attributes) {
  return publishObjectClass(handle, className, attributes);
}

int registerObjectInstance(
    QttestHlaBackendHandle handle,
    const char* className,
    const char* instanceName,
    uint64_t* instanceId) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !className ||
      !*className || !instanceName || !*instanceName || !instanceId) {
    return fail(value, "Invalid mock object registration");
  }
  *instanceId = value->nextObjectId++;
  value->objects.insert(*instanceId);
  value->error.clear();
  return 0;
}

int updateObjectAttributes(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaNamedValueArrayV2* attributes,
    const QttestHlaByteSpanV2*) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      value->objects.count(instanceId) == 0 || !attributes ||
      attributes->count == 0) {
    return fail(value, "Invalid mock object update");
  }
  value->error.clear();
  return 0;
}

int deleteObjectInstance(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaByteSpanV2*) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      value->objects.erase(instanceId) == 0) {
    return fail(value, "Invalid mock object deletion");
  }
  value->error.clear();
  return 0;
}

int publishInteractionClass(
    QttestHlaBackendHandle handle,
    const char* className) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !className ||
      !*className) {
    return fail(value, "Invalid mock interaction publication");
  }
  value->error.clear();
  return 0;
}

int subscribeInteractionClass(
    QttestHlaBackendHandle handle,
    const char* className,
    const QttestHlaStringArrayV1*) {
  return publishInteractionClass(handle, className);
}

int sendInteraction(
    QttestHlaBackendHandle handle,
    const char* className,
    const QttestHlaNamedValueArrayV2* parameters,
    const QttestHlaByteSpanV2*) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !className ||
      !*className || !parameters) {
    return fail(value, "Invalid mock interaction");
  }
  value->error.clear();
  return 0;
}

int registerSynchronizationPoint(
    QttestHlaBackendHandle handle,
    const char* label,
    const QttestHlaByteSpanV2* tag) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !label || !*label) {
    return fail(value, "Invalid mock synchronization point");
  }
  if (value->callbacks.synchronizationPointAnnounced) {
    value->callbacks.synchronizationPointAnnounced(
        value->callbacks.context, label, tag);
  }
  value->error.clear();
  return 0;
}

int achieveSynchronizationPoint(
    QttestHlaBackendHandle handle,
    const char* label) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !label || !*label) {
    return fail(value, "Invalid mock synchronization achievement");
  }
  if (value->callbacks.federationSynchronized) {
    value->callbacks.federationSynchronized(value->callbacks.context, label);
  }
  value->error.clear();
  return 0;
}

int enableTimeRegulation(
    QttestHlaBackendHandle handle,
    double lookaheadSeconds) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      lookaheadSeconds <= 0.0) {
    return fail(value, "Invalid mock time regulation request");
  }
  value->timeRegulating = true;
  if (value->callbacks.timeRegulationEnabled) {
    value->callbacks.timeRegulationEnabled(
        value->callbacks.context, value->logicalTimeSeconds);
  }
  value->error.clear();
  return 0;
}

int enableTimeConstrained(QttestHlaBackendHandle handle) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "Invalid mock time constrained request");
  }
  value->timeConstrained = true;
  if (value->callbacks.timeConstrainedEnabled) {
    value->callbacks.timeConstrainedEnabled(
        value->callbacks.context, value->logicalTimeSeconds);
  }
  value->error.clear();
  return 0;
}

int requestTimeAdvance(
    QttestHlaBackendHandle handle,
    double logicalTimeSeconds) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !value->timeRegulating || !value->timeConstrained ||
      logicalTimeSeconds < value->logicalTimeSeconds) {
    return fail(value, "Invalid mock time advance request");
  }
  value->logicalTimeSeconds = logicalTimeSeconds;
  if (value->callbacks.timeAdvanceGranted) {
    value->callbacks.timeAdvanceGranted(
        value->callbacks.context, logicalTimeSeconds);
  }
  value->error.clear();
  return 0;
}

int setCallbacks(
    QttestHlaBackendHandle handle,
    const QttestHlaCallbacksV5* callbacks) {
  MockSession* value = session(handle);
  if (!value || !callbacks ||
      callbacks->structSize < sizeof(QttestHlaCallbacksV5)) {
    return fail(value, "Invalid mock callback configuration");
  }
  value->callbacks = *callbacks;
  value->error.clear();
  return 0;
}

int pollBackend(QttestHlaBackendHandle handle, double maximumSeconds) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "Mock plugin federate is not joined");
  }
  if (maximumSeconds < 0.0) {
    return fail(value, "Polling duration cannot be negative");
  }
  value->error.clear();
  return 0;
}

int resignBackend(QttestHlaBackendHandle handle) {
  MockSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "Mock plugin federate is not joined");
  }
  value->error.clear();
  value->state = QTTEST_HLA_STATE_CONNECTED;
  return 0;
}

int disconnectBackend(QttestHlaBackendHandle handle) {
  MockSession* value = session(handle);
  if (!value || value->state == QTTEST_HLA_STATE_JOINED) {
    return fail(value, "Mock plugin must resign before disconnecting");
  }
  value->error.clear();
  value->state = QTTEST_HLA_STATE_DISCONNECTED;
  return 0;
}

QttestHlaBackendStateV1 backendState(QttestHlaBackendHandle handle) {
  const MockSession* value = session(handle);
  return value ? value->state : QTTEST_HLA_STATE_UNAVAILABLE;
}

const char* lastError(QttestHlaBackendHandle handle) {
  const MockSession* value = session(handle);
  return value ? value->error.c_str() : "Mock plugin session is unavailable";
}

const QttestHlaBackendApiV5 api = {
    sizeof(QttestHlaBackendApiV5),
    QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION,
    "mock-plugin",
    "1.0",
    "federation-management,object-management,interactions,evoked-callbacks,test-backend",
    &createBackend,
    &destroyBackend,
    &connectBackend,
    &createFederation,
    &joinFederation,
    &publishObjectClass,
    &subscribeObjectClass,
    &registerObjectInstance,
    &updateObjectAttributes,
    &deleteObjectInstance,
    &publishInteractionClass,
    &subscribeInteractionClass,
    &sendInteraction,
    &registerSynchronizationPoint,
    &achieveSynchronizationPoint,
    &enableTimeRegulation,
    &enableTimeConstrained,
    &requestTimeAdvance,
    &setCallbacks,
    &pollBackend,
    &resignBackend,
    &disconnectBackend,
    &backendState,
    &lastError};

} // namespace

extern "C" QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV5*
qttest_hla_backend_api_v5(void) {
  return &api;
}
