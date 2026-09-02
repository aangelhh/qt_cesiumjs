#include "infrastructure/interoperability/hla/HlaBackendPluginApi.h"

#include <string>
#include <set>

namespace {

struct MockSession {
  QttestHlaBackendStateV1 state = QTTEST_HLA_STATE_DISCONNECTED;
  std::string error;
  uint64_t nextObjectId = 1;
  std::set<uint64_t> objects;
  QttestHlaCallbacksV3 callbacks = {};
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

int setCallbacks(
    QttestHlaBackendHandle handle,
    const QttestHlaCallbacksV3* callbacks) {
  MockSession* value = session(handle);
  if (!value || !callbacks ||
      callbacks->structSize < sizeof(QttestHlaCallbacksV3)) {
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

const QttestHlaBackendApiV3 api = {
    sizeof(QttestHlaBackendApiV3),
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
    &setCallbacks,
    &pollBackend,
    &resignBackend,
    &disconnectBackend,
    &backendState,
    &lastError};

} // namespace

extern "C" QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV3*
qttest_hla_backend_api_v3(void) {
  return &api;
}
