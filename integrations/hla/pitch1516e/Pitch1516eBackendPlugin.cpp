#include "infrastructure/interoperability/hla/HlaBackendPluginApi.h"

#include <RTI/Exception.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTIambassador.h>
#include <RTI/RTIambassadorFactory.h>

#include <codecvt>
#include <chrono>
#include <locale>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#ifndef QTTEST_HLA_1516E_BACKEND_ID
#define QTTEST_HLA_1516E_BACKEND_ID "pitch1516e"
#endif

#ifndef QTTEST_HLA_1516E_BACKEND_NAME
#define QTTEST_HLA_1516E_BACKEND_NAME "pRTI IEEE 1516e"
#endif

namespace {

std::wstring fromUtf8(const char* value) {
  if (!value) {
    return {};
  }
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(value);
}

std::string toUtf8(const std::wstring& value) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(value);
}

std::vector<std::wstring> fromStringArray(
    const QttestHlaStringArrayV1* values) {
  std::vector<std::wstring> result;
  if (!values || values->structSize < sizeof(QttestHlaStringArrayV1) ||
      !values->values) {
    return result;
  }
  result.reserve(values->count);
  for (size_t index = 0; index < values->count; ++index) {
    result.push_back(fromUtf8(values->values[index]));
  }
  return result;
}

class PluginFederateAmbassador final
    : public rti1516e::NullFederateAmbassador {
public:
  void objectInstanceNameReservationSucceeded(
      const std::wstring& objectInstanceName)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    reservedNames.insert(objectInstanceName);
    failedNames.erase(objectInstanceName);
  }

  void objectInstanceNameReservationFailed(
      const std::wstring& objectInstanceName)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    failedNames.insert(objectInstanceName);
    reservedNames.erase(objectInstanceName);
  }

  std::set<std::wstring> reservedNames;
  std::set<std::wstring> failedNames;
};

struct PitchSession {
  PluginFederateAmbassador federateAmbassador;
  std::auto_ptr<rti1516e::RTIambassador> rtiAmbassador;
  QttestHlaBackendStateV1 state = QTTEST_HLA_STATE_DISCONNECTED;
  std::string error;
  uint64_t nextObjectId = 1;
  struct RegisteredObject {
    rti1516e::ObjectInstanceHandle handle;
    rti1516e::ObjectClassHandle classHandle;
  };
  std::map<std::wstring, rti1516e::ObjectClassHandle> objectClasses;
  std::map<std::wstring, rti1516e::InteractionClassHandle> interactions;
  std::unordered_map<uint64_t, RegisteredObject> objects;
};

PitchSession* session(QttestHlaBackendHandle handle) {
  return static_cast<PitchSession*>(handle);
}

int fail(
    PitchSession* value,
    std::string message,
    QttestHlaBackendStateV1 state) {
  if (value) {
    value->error = std::move(message);
    value->state = state;
  }
  return 1;
}

int fail(
    PitchSession* value,
    const rti1516e::Exception& exception,
    QttestHlaBackendStateV1 state) {
  return fail(value, toUtf8(exception.what()), state);
}

QttestHlaBackendHandle createBackend() {
  try {
    std::unique_ptr<PitchSession> value(new PitchSession());
    rti1516e::RTIambassadorFactory factory;
    value->rtiAmbassador = factory.createRTIambassador();
    return value.release();
  } catch (...) {
    return nullptr;
  }
}

void destroyBackend(QttestHlaBackendHandle handle) {
  PitchSession* value = session(handle);
  if (!value) {
    return;
  }
  if (value->rtiAmbassador.get()) {
    if (value->state == QTTEST_HLA_STATE_JOINED) {
      try {
        value->rtiAmbassador->resignFederationExecution(
            rti1516e::CANCEL_THEN_DELETE_THEN_DIVEST);
      } catch (...) {
      }
    }
    if (value->state != QTTEST_HLA_STATE_DISCONNECTED) {
      try {
        value->rtiAmbassador->disconnect();
      } catch (...) {
      }
    }
  }
  delete value;
}

int connectBackend(
    QttestHlaBackendHandle handle,
    const char* localSettingsDesignator) {
  PitchSession* value = session(handle);
  if (!value || !value->rtiAmbassador.get()) {
    return fail(value, "RTI ambassador is unavailable", QTTEST_HLA_STATE_UNAVAILABLE);
  }
  if (value->state != QTTEST_HLA_STATE_DISCONNECTED) {
    return fail(value, "HLA backend is already connected", value->state);
  }
  try {
    value->rtiAmbassador->connect(
        value->federateAmbassador,
        rti1516e::HLA_EVOKED,
        fromUtf8(localSettingsDesignator));
    value->error.clear();
    value->state = QTTEST_HLA_STATE_CONNECTED;
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_DISCONNECTED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_DISCONNECTED);
  }
}

int createFederation(
    QttestHlaBackendHandle handle,
    const char* federationName,
    const QttestHlaStringArrayV1* fomModules) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_CONNECTED) {
    return fail(value, "HLA backend is not connected", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->createFederationExecution(
        fromUtf8(federationName),
        fromStringArray(fomModules));
    value->error.clear();
    return 0;
  } catch (const rti1516e::FederationExecutionAlreadyExists&) {
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_CONNECTED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_CONNECTED);
  }
}

int joinFederation(
    QttestHlaBackendHandle handle,
    const char* federateName,
    const char* federateType,
    const char* federationName,
    const QttestHlaStringArrayV1* additionalFomModules) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_CONNECTED) {
    return fail(value, "HLA backend is not connected", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->joinFederationExecution(
        fromUtf8(federateName),
        fromUtf8(federateType),
        fromUtf8(federationName),
        fromStringArray(additionalFomModules));
    value->error.clear();
    value->state = QTTEST_HLA_STATE_JOINED;
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_CONNECTED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_CONNECTED);
  }
}

rti1516e::VariableLengthData variableLengthData(
    const QttestHlaByteSpanV2* value) {
  if (!value || value->structSize < sizeof(QttestHlaByteSpanV2) ||
      !value->data || value->size == 0) {
    return rti1516e::VariableLengthData();
  }
  return rti1516e::VariableLengthData(value->data, value->size);
}

int publishObjectClass(
    QttestHlaBackendHandle handle,
    const char* objectClassName,
    const QttestHlaStringArrayV1* attributeNames) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !objectClassName || !*objectClassName || !attributeNames ||
      attributeNames->structSize < sizeof(QttestHlaStringArrayV1) ||
      !attributeNames->values || attributeNames->count == 0) {
    return fail(value, "Invalid HLA object publication", QTTEST_HLA_STATE_ERROR);
  }
  try {
    const std::wstring className = fromUtf8(objectClassName);
    const rti1516e::ObjectClassHandle classHandle =
        value->rtiAmbassador->getObjectClassHandle(className);
    rti1516e::AttributeHandleSet attributes;
    for (size_t index = 0; index < attributeNames->count; ++index) {
      attributes.insert(value->rtiAmbassador->getAttributeHandle(
          classHandle, fromUtf8(attributeNames->values[index])));
    }
    value->rtiAmbassador->publishObjectClassAttributes(classHandle, attributes);
    value->objectClasses[className] = classHandle;
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int registerObjectInstance(
    QttestHlaBackendHandle handle,
    const char* objectClassName,
    const char* instanceName,
    uint64_t* instanceId) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !objectClassName || !*objectClassName || !instanceName ||
      !*instanceName || !instanceId) {
    return fail(value, "Invalid HLA object registration", QTTEST_HLA_STATE_ERROR);
  }
  try {
    const std::wstring className = fromUtf8(objectClassName);
    auto classIterator = value->objectClasses.find(className);
    if (classIterator == value->objectClasses.end()) {
      return fail(
          value,
          "HLA object class has not been published",
          QTTEST_HLA_STATE_JOINED);
    }
    const std::wstring requestedName = fromUtf8(instanceName);
    value->federateAmbassador.reservedNames.erase(requestedName);
    value->federateAmbassador.failedNames.erase(requestedName);
    value->rtiAmbassador->reserveObjectInstanceName(requestedName);
    const auto reservationDeadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (std::chrono::steady_clock::now() < reservationDeadline) {
      value->rtiAmbassador->evokeMultipleCallbacks(0.0, 0.05);
      if (value->federateAmbassador.reservedNames.count(requestedName) != 0 ||
          value->federateAmbassador.failedNames.count(requestedName) != 0) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (value->federateAmbassador.failedNames.count(requestedName) != 0) {
      return fail(
          value,
          "HLA object instance name is already in use",
          QTTEST_HLA_STATE_JOINED);
    }
    if (value->federateAmbassador.reservedNames.count(requestedName) == 0) {
      return fail(
          value,
          "Timed out while reserving HLA object instance name",
          QTTEST_HLA_STATE_JOINED);
    }
    const rti1516e::ObjectInstanceHandle objectHandle =
        value->rtiAmbassador->registerObjectInstance(
            classIterator->second, requestedName);
    value->federateAmbassador.reservedNames.erase(requestedName);
    const uint64_t localId = value->nextObjectId++;
    value->objects.emplace(
        localId,
        PitchSession::RegisteredObject{objectHandle, classIterator->second});
    *instanceId = localId;
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int updateObjectAttributes(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaNamedValueArrayV2* attributes,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !attributes ||
      attributes->structSize < sizeof(QttestHlaNamedValueArrayV2) ||
      !attributes->values || attributes->count == 0) {
    return fail(value, "Invalid HLA attribute update", QTTEST_HLA_STATE_ERROR);
  }
  const auto objectIterator = value->objects.find(instanceId);
  if (objectIterator == value->objects.end()) {
    return fail(value, "Unknown HLA object instance", QTTEST_HLA_STATE_JOINED);
  }
  try {
    rti1516e::AttributeHandleValueMap attributeValues;
    for (size_t index = 0; index < attributes->count; ++index) {
      const QttestHlaNamedValueV2& attribute = attributes->values[index];
      if (attribute.structSize < sizeof(QttestHlaNamedValueV2) ||
          !attribute.name || !*attribute.name) {
        return fail(value, "Invalid HLA attribute value", QTTEST_HLA_STATE_JOINED);
      }
      const rti1516e::AttributeHandle attributeHandle =
          value->rtiAmbassador->getAttributeHandle(
              objectIterator->second.classHandle, fromUtf8(attribute.name));
      attributeValues[attributeHandle] = variableLengthData(&attribute.value);
    }
    value->rtiAmbassador->updateAttributeValues(
        objectIterator->second.handle, attributeValues, variableLengthData(tag));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int deleteObjectInstance(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "HLA federate is not joined", QTTEST_HLA_STATE_ERROR);
  }
  const auto objectIterator = value->objects.find(instanceId);
  if (objectIterator == value->objects.end()) {
    return fail(value, "Unknown HLA object instance", QTTEST_HLA_STATE_JOINED);
  }
  try {
    value->rtiAmbassador->deleteObjectInstance(
        objectIterator->second.handle, variableLengthData(tag));
    value->objects.erase(objectIterator);
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int publishInteractionClass(
    QttestHlaBackendHandle handle,
    const char* interactionClassName) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !interactionClassName || !*interactionClassName) {
    return fail(value, "Invalid HLA interaction publication", QTTEST_HLA_STATE_ERROR);
  }
  try {
    const std::wstring className = fromUtf8(interactionClassName);
    const rti1516e::InteractionClassHandle interactionHandle =
        value->rtiAmbassador->getInteractionClassHandle(className);
    value->rtiAmbassador->publishInteractionClass(interactionHandle);
    value->interactions[className] = interactionHandle;
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int sendInteraction(
    QttestHlaBackendHandle handle,
    const char* interactionClassName,
    const QttestHlaNamedValueArrayV2* parameters,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !interactionClassName || !*interactionClassName || !parameters ||
      parameters->structSize < sizeof(QttestHlaNamedValueArrayV2)) {
    return fail(value, "Invalid HLA interaction", QTTEST_HLA_STATE_ERROR);
  }
  try {
    const std::wstring className = fromUtf8(interactionClassName);
    const auto interactionIterator = value->interactions.find(className);
    if (interactionIterator == value->interactions.end()) {
      return fail(
          value,
          "HLA interaction class has not been published",
          QTTEST_HLA_STATE_JOINED);
    }
    rti1516e::ParameterHandleValueMap parameterValues;
    for (size_t index = 0; index < parameters->count; ++index) {
      const QttestHlaNamedValueV2& parameter = parameters->values[index];
      if (parameter.structSize < sizeof(QttestHlaNamedValueV2) ||
          !parameter.name || !*parameter.name) {
        return fail(value, "Invalid HLA parameter value", QTTEST_HLA_STATE_JOINED);
      }
      const rti1516e::ParameterHandle parameterHandle =
          value->rtiAmbassador->getParameterHandle(
              interactionIterator->second, fromUtf8(parameter.name));
      parameterValues[parameterHandle] = variableLengthData(&parameter.value);
    }
    value->rtiAmbassador->sendInteraction(
        interactionIterator->second, parameterValues, variableLengthData(tag));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int pollBackend(QttestHlaBackendHandle handle, double maximumSeconds) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "HLA federate is not joined", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->evokeMultipleCallbacks(0.0, maximumSeconds);
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int resignBackend(QttestHlaBackendHandle handle) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "HLA federate is not joined", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->resignFederationExecution(
        rti1516e::CANCEL_THEN_DELETE_THEN_DIVEST);
    value->error.clear();
    value->state = QTTEST_HLA_STATE_CONNECTED;
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int disconnectBackend(QttestHlaBackendHandle handle) {
  PitchSession* value = session(handle);
  if (!value || !value->rtiAmbassador.get()) {
    return fail(value, "RTI ambassador is unavailable", QTTEST_HLA_STATE_UNAVAILABLE);
  }
  if (value->state == QTTEST_HLA_STATE_JOINED) {
    return fail(value, "HLA federate must resign before disconnecting", value->state);
  }
  if (value->state == QTTEST_HLA_STATE_DISCONNECTED) {
    value->error.clear();
    return 0;
  }
  try {
    value->rtiAmbassador->disconnect();
    value->error.clear();
    value->state = QTTEST_HLA_STATE_DISCONNECTED;
    return 0;
  } catch (const rti1516e::NotConnected&) {
    value->error.clear();
    value->state = QTTEST_HLA_STATE_DISCONNECTED;
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_ERROR);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_ERROR);
  }
}

QttestHlaBackendStateV1 backendState(QttestHlaBackendHandle handle) {
  const PitchSession* value = session(handle);
  return value ? value->state : QTTEST_HLA_STATE_UNAVAILABLE;
}

const char* lastError(QttestHlaBackendHandle handle) {
  const PitchSession* value = session(handle);
  return value ? value->error.c_str() : "HLA backend session is unavailable";
}

const QttestHlaBackendApiV2 api = {
    sizeof(QttestHlaBackendApiV2),
    QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION,
    QTTEST_HLA_1516E_BACKEND_ID,
    QTTEST_HLA_1516E_BACKEND_NAME,
    "federation-management,object-management,interactions,evoked-callbacks,ieee1516e",
    &createBackend,
    &destroyBackend,
    &connectBackend,
    &createFederation,
    &joinFederation,
    &publishObjectClass,
    &registerObjectInstance,
    &updateObjectAttributes,
    &deleteObjectInstance,
    &publishInteractionClass,
    &sendInteraction,
    &pollBackend,
    &resignBackend,
    &disconnectBackend,
    &backendState,
    &lastError};

} // namespace

extern "C" QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV2*
qttest_hla_backend_api_v2(void) {
  return &api;
}
