#include "infrastructure/interoperability/hla/HlaBackendPluginApi.h"

#include <RTI/Exception.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTIambassador.h>
#include <RTI/RTIambassadorFactory.h>
#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>

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
  struct SubscribedObjectClass {
    std::string name;
    std::map<rti1516e::AttributeHandle, std::string> attributes;
  };

  struct RemoteObject {
    uint64_t id = 0;
    rti1516e::ObjectClassHandle classHandle;
  };

  uint64_t objectId(rti1516e::ObjectInstanceHandle objectHandle) const {
    const auto remote = remoteObjects.find(objectHandle);
    if (remote != remoteObjects.end()) return remote->second.id;
    const auto local = localObjectIds.find(objectHandle);
    return local == localObjectIds.end() ? 0 : local->second;
  }

  rti1516e::ObjectClassHandle objectClass(
      rti1516e::ObjectInstanceHandle objectHandle) const {
    const auto remote = remoteObjects.find(objectHandle);
    if (remote != remoteObjects.end()) return remote->second.classHandle;
    const auto local = localObjectClasses.find(objectHandle);
    return local == localObjectClasses.end()
        ? rti1516e::ObjectClassHandle()
        : local->second;
  }

  std::vector<std::string> ownershipAttributeNames(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleSet& attributes) const {
    std::vector<std::string> result;
    const auto names = attributeNames.find(this->objectClass(objectHandle));
    if (names == attributeNames.end()) return result;
    for (const rti1516e::AttributeHandle attribute : attributes) {
      const auto name = names->second.find(attribute);
      if (name != names->second.end()) result.push_back(name->second);
    }
    return result;
  }

  QttestHlaStringArrayV1 ownershipAttributeArray(
      const std::vector<std::string>& names,
      std::vector<const char*>& values) const {
    values.reserve(names.size());
    for (const std::string& name : names) values.push_back(name.c_str());
    return {
        sizeof(QttestHlaStringArrayV1),
        values.empty() ? nullptr : values.data(),
        values.size()};
  }

  void announceSynchronizationPoint(
      const std::wstring& label,
      const rti1516e::VariableLengthData& tag)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.synchronizationPointAnnounced) return;
    const std::string utf8Label = toUtf8(label);
    const QttestHlaByteSpanV2 tagSpan = {
        sizeof(QttestHlaByteSpanV2),
        static_cast<const uint8_t*>(tag.data()),
        tag.size()};
    callbacks.synchronizationPointAnnounced(
        callbacks.context, utf8Label.c_str(), &tagSpan);
  }

  void federationSynchronized(
      const std::wstring& label,
      const rti1516e::FederateHandleSet&)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.federationSynchronized) return;
    const std::string utf8Label = toUtf8(label);
    callbacks.federationSynchronized(callbacks.context, utf8Label.c_str());
  }

  void timeRegulationEnabled(const rti1516e::LogicalTime& logicalTime)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.timeRegulationEnabled) return;
    const rti1516e::HLAfloat64Time time(logicalTime);
    callbacks.timeRegulationEnabled(callbacks.context, time.getTime());
  }

  void timeConstrainedEnabled(const rti1516e::LogicalTime& logicalTime)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.timeConstrainedEnabled) return;
    const rti1516e::HLAfloat64Time time(logicalTime);
    callbacks.timeConstrainedEnabled(callbacks.context, time.getTime());
  }

  void timeAdvanceGrant(const rti1516e::LogicalTime& logicalTime)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.timeAdvanceGranted) return;
    const rti1516e::HLAfloat64Time time(logicalTime);
    callbacks.timeAdvanceGranted(callbacks.context, time.getTime());
  }

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

  void discoverObjectInstance(
      rti1516e::ObjectInstanceHandle objectHandle,
      rti1516e::ObjectClassHandle classHandle,
      const std::wstring& objectInstanceName)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const auto classIterator = subscribedObjectClasses.find(classHandle);
    if (classIterator == subscribedObjectClasses.end()) return;
    const uint64_t remoteId = nextRemoteObjectId++;
    remoteObjects.emplace(objectHandle, RemoteObject{remoteId, classHandle});
    if (callbacks.objectDiscovered) {
      const std::string instanceName = toUtf8(objectInstanceName);
      callbacks.objectDiscovered(
          callbacks.context,
          remoteId,
          classIterator->second.name.c_str(),
          instanceName.c_str());
    }
  }

  void discoverObjectInstance(
      rti1516e::ObjectInstanceHandle objectHandle,
      rti1516e::ObjectClassHandle classHandle,
      const std::wstring& objectInstanceName,
      rti1516e::FederateHandle)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    this->discoverObjectInstance(
        objectHandle, classHandle, objectInstanceName);
  }

  void emitObjectReflection(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleValueMap& attributeValues,
      const rti1516e::VariableLengthData& tag,
      const QttestHlaReceiveInfoV7& receiveInfo) {
    const auto objectIterator = remoteObjects.find(objectHandle);
    if (objectIterator == remoteObjects.end() || !callbacks.objectReflected) return;
    const auto classIterator =
        subscribedObjectClasses.find(objectIterator->second.classHandle);
    if (classIterator == subscribedObjectClasses.end()) return;

    std::vector<std::string> names;
    std::vector<QttestHlaNamedValueV2> values;
    names.reserve(attributeValues.size());
    values.reserve(attributeValues.size());
    for (const auto& item : attributeValues) {
      const auto nameIterator = classIterator->second.attributes.find(item.first);
      if (nameIterator == classIterator->second.attributes.end()) continue;
      names.push_back(nameIterator->second);
      values.push_back({
          sizeof(QttestHlaNamedValueV2),
          names.back().c_str(),
          {sizeof(QttestHlaByteSpanV2),
           static_cast<const uint8_t*>(item.second.data()),
           item.second.size()}});
    }
    const QttestHlaNamedValueArrayV2 array = {
        sizeof(QttestHlaNamedValueArrayV2),
        values.empty() ? nullptr : values.data(),
        values.size()};
    const QttestHlaByteSpanV2 tagSpan = {
        sizeof(QttestHlaByteSpanV2),
        static_cast<const uint8_t*>(tag.data()),
        tag.size()};
    callbacks.objectReflected(
        callbacks.context, objectIterator->second.id, &array, &tagSpan,
        &receiveInfo);
  }

  void reflectAttributeValues(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleValueMap& attributeValues,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType,
      rti1516e::TransportationType,
      rti1516e::SupplementalReflectInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const QttestHlaReceiveInfoV7 receiveInfo = {
        sizeof(QttestHlaReceiveInfoV7), QTTEST_HLA_RECEIVE_ORDER, 0, 0.0};
    this->emitObjectReflection(
        objectHandle, attributeValues, tag, receiveInfo);
  }

  void reflectAttributeValues(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleValueMap& attributeValues,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType sentOrder,
      rti1516e::TransportationType transportation,
      const rti1516e::LogicalTime& logicalTime,
      rti1516e::OrderType,
      rti1516e::SupplementalReflectInfo reflectInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const rti1516e::HLAfloat64Time time(logicalTime);
    const QttestHlaReceiveInfoV7 receiveInfo = {
        sizeof(QttestHlaReceiveInfoV7), QTTEST_HLA_TIMESTAMP_ORDER, 1,
        time.getTime()};
    this->emitObjectReflection(
        objectHandle, attributeValues, tag, receiveInfo);
  }

  void reflectAttributeValues(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleValueMap& attributeValues,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType sentOrder,
      rti1516e::TransportationType transportation,
      const rti1516e::LogicalTime& logicalTime,
      rti1516e::OrderType receivedOrder,
      rti1516e::MessageRetractionHandle,
      rti1516e::SupplementalReflectInfo reflectInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    this->reflectAttributeValues(
        objectHandle, attributeValues, tag, sentOrder, transportation,
        logicalTime, receivedOrder, reflectInfo);
  }

  void emitObjectRemoval(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::VariableLengthData& tag,
      const QttestHlaReceiveInfoV7& receiveInfo) {
    const auto iterator = remoteObjects.find(objectHandle);
    if (iterator == remoteObjects.end()) return;
    const uint64_t remoteId = iterator->second.id;
    remoteObjects.erase(iterator);
    if (callbacks.objectRemoved) {
      const QttestHlaByteSpanV2 tagSpan = {
          sizeof(QttestHlaByteSpanV2),
          static_cast<const uint8_t*>(tag.data()),
          tag.size()};
      callbacks.objectRemoved(
          callbacks.context, remoteId, &tagSpan, &receiveInfo);
    }
  }

  void removeObjectInstance(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType,
      rti1516e::SupplementalRemoveInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const QttestHlaReceiveInfoV7 receiveInfo = {
        sizeof(QttestHlaReceiveInfoV7), QTTEST_HLA_RECEIVE_ORDER, 0, 0.0};
    this->emitObjectRemoval(objectHandle, tag, receiveInfo);
  }

  void removeObjectInstance(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType sentOrder,
      const rti1516e::LogicalTime& logicalTime,
      rti1516e::OrderType,
      rti1516e::SupplementalRemoveInfo removeInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const rti1516e::HLAfloat64Time time(logicalTime);
    const QttestHlaReceiveInfoV7 receiveInfo = {
        sizeof(QttestHlaReceiveInfoV7), QTTEST_HLA_TIMESTAMP_ORDER, 1,
        time.getTime()};
    this->emitObjectRemoval(objectHandle, tag, receiveInfo);
  }

  void removeObjectInstance(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType sentOrder,
      const rti1516e::LogicalTime& logicalTime,
      rti1516e::OrderType receivedOrder,
      rti1516e::MessageRetractionHandle,
      rti1516e::SupplementalRemoveInfo removeInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    this->removeObjectInstance(
        objectHandle, tag, sentOrder, logicalTime, receivedOrder, removeInfo);
  }

  void emitInteraction(
      rti1516e::InteractionClassHandle interactionHandle,
      const rti1516e::ParameterHandleValueMap& parameterValues,
      const rti1516e::VariableLengthData& tag,
      const QttestHlaReceiveInfoV7& receiveInfo) {
    const auto interactionIterator = subscribedInteractions.find(interactionHandle);
    if (interactionIterator == subscribedInteractions.end() ||
        !callbacks.interactionReceived) {
      return;
    }
    std::vector<std::string> names;
    std::vector<QttestHlaNamedValueV2> values;
    names.reserve(parameterValues.size());
    values.reserve(parameterValues.size());
    for (const auto& item : parameterValues) {
      const auto nameIterator = interactionIterator->second.second.find(item.first);
      if (nameIterator == interactionIterator->second.second.end()) continue;
      names.push_back(nameIterator->second);
      values.push_back({
          sizeof(QttestHlaNamedValueV2),
          names.back().c_str(),
          {sizeof(QttestHlaByteSpanV2),
           static_cast<const uint8_t*>(item.second.data()),
           item.second.size()}});
    }
    const QttestHlaNamedValueArrayV2 array = {
        sizeof(QttestHlaNamedValueArrayV2),
        values.empty() ? nullptr : values.data(),
        values.size()};
    const QttestHlaByteSpanV2 tagSpan = {
        sizeof(QttestHlaByteSpanV2),
        static_cast<const uint8_t*>(tag.data()),
        tag.size()};
    callbacks.interactionReceived(
        callbacks.context,
        interactionIterator->second.first.c_str(),
        &array,
        &tagSpan,
        &receiveInfo);
  }

  void receiveInteraction(
      rti1516e::InteractionClassHandle interactionHandle,
      const rti1516e::ParameterHandleValueMap& parameterValues,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType,
      rti1516e::TransportationType,
      rti1516e::SupplementalReceiveInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const QttestHlaReceiveInfoV7 receiveInfo = {
        sizeof(QttestHlaReceiveInfoV7), QTTEST_HLA_RECEIVE_ORDER, 0, 0.0};
    this->emitInteraction(
        interactionHandle, parameterValues, tag, receiveInfo);
  }

  void attributeOwnershipAcquisitionNotification(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleSet& attributes,
      const rti1516e::VariableLengthData& tag)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.attributeOwnershipAcquired) return;
    const std::vector<std::string> names =
        this->ownershipAttributeNames(objectHandle, attributes);
    std::vector<const char*> values;
    const QttestHlaStringArrayV1 array =
        this->ownershipAttributeArray(names, values);
    const QttestHlaByteSpanV2 tagSpan = {
        sizeof(QttestHlaByteSpanV2),
        static_cast<const uint8_t*>(tag.data()),
        tag.size()};
    callbacks.attributeOwnershipAcquired(
        callbacks.context, this->objectId(objectHandle), &array, &tagSpan);
  }

  void attributeOwnershipUnavailable(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleSet& attributes)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.attributeOwnershipUnavailable) return;
    const std::vector<std::string> names =
        this->ownershipAttributeNames(objectHandle, attributes);
    std::vector<const char*> values;
    const QttestHlaStringArrayV1 array =
        this->ownershipAttributeArray(names, values);
    callbacks.attributeOwnershipUnavailable(
        callbacks.context, this->objectId(objectHandle), &array);
  }

  void requestAttributeOwnershipRelease(
      rti1516e::ObjectInstanceHandle objectHandle,
      const rti1516e::AttributeHandleSet& attributes,
      const rti1516e::VariableLengthData& tag)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    if (!callbacks.attributeOwnershipReleaseRequested) return;
    const std::vector<std::string> names =
        this->ownershipAttributeNames(objectHandle, attributes);
    std::vector<const char*> values;
    const QttestHlaStringArrayV1 array =
        this->ownershipAttributeArray(names, values);
    const QttestHlaByteSpanV2 tagSpan = {
        sizeof(QttestHlaByteSpanV2),
        static_cast<const uint8_t*>(tag.data()),
        tag.size()};
    callbacks.attributeOwnershipReleaseRequested(
        callbacks.context, this->objectId(objectHandle), &array, &tagSpan);
  }

  void receiveInteraction(
      rti1516e::InteractionClassHandle interactionHandle,
      const rti1516e::ParameterHandleValueMap& parameterValues,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType sentOrder,
      rti1516e::TransportationType transportation,
      const rti1516e::LogicalTime& logicalTime,
      rti1516e::OrderType,
      rti1516e::SupplementalReceiveInfo receiveInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    const rti1516e::HLAfloat64Time time(logicalTime);
    const QttestHlaReceiveInfoV7 metadata = {
        sizeof(QttestHlaReceiveInfoV7), QTTEST_HLA_TIMESTAMP_ORDER, 1,
        time.getTime()};
    this->emitInteraction(
        interactionHandle, parameterValues, tag, metadata);
  }

  void receiveInteraction(
      rti1516e::InteractionClassHandle interactionHandle,
      const rti1516e::ParameterHandleValueMap& parameterValues,
      const rti1516e::VariableLengthData& tag,
      rti1516e::OrderType sentOrder,
      rti1516e::TransportationType transportation,
      const rti1516e::LogicalTime& logicalTime,
      rti1516e::OrderType receivedOrder,
      rti1516e::MessageRetractionHandle,
      rti1516e::SupplementalReceiveInfo receiveInfo)
#ifdef QTTEST_HLA_OPENRTI_BACKEND
      RTI_THROW ((rti1516e::FederateInternalError))
#elif __cplusplus < 201703L
      RTI_THROW (rti1516e::FederateInternalError)
#endif
      override {
    this->receiveInteraction(
        interactionHandle, parameterValues, tag, sentOrder, transportation,
        logicalTime, receivedOrder, receiveInfo);
  }

  std::set<std::wstring> reservedNames;
  std::set<std::wstring> failedNames;
  QttestHlaCallbacksV8 callbacks = {};
  uint64_t nextRemoteObjectId = (uint64_t{1} << 63U);
  std::map<rti1516e::ObjectClassHandle, SubscribedObjectClass>
      subscribedObjectClasses;
  std::map<rti1516e::ObjectInstanceHandle, RemoteObject> remoteObjects;
  std::map<rti1516e::ObjectInstanceHandle, uint64_t> localObjectIds;
  std::map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle>
      localObjectClasses;
  std::map<
      rti1516e::ObjectClassHandle,
      std::map<rti1516e::AttributeHandle, std::string>> attributeNames;
  std::map<
      rti1516e::InteractionClassHandle,
      std::pair<std::string, std::map<rti1516e::ParameterHandle, std::string>>>
      subscribedInteractions;
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

bool resolveObject(
    PitchSession* value,
    uint64_t instanceId,
    rti1516e::ObjectInstanceHandle& objectHandle,
    rti1516e::ObjectClassHandle& classHandle) {
  const auto local = value->objects.find(instanceId);
  if (local != value->objects.end()) {
    objectHandle = local->second.handle;
    classHandle = local->second.classHandle;
    return true;
  }
  for (const auto& remote : value->federateAmbassador.remoteObjects) {
    if (remote.second.id != instanceId) continue;
    objectHandle = remote.first;
    classHandle = remote.second.classHandle;
    return true;
  }
  return false;
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
      const rti1516e::AttributeHandle attributeHandle =
          value->rtiAmbassador->getAttributeHandle(
              classHandle, fromUtf8(attributeNames->values[index]));
      attributes.insert(attributeHandle);
      value->federateAmbassador.attributeNames[classHandle][attributeHandle] =
          attributeNames->values[index];
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

int subscribeObjectClass(
    QttestHlaBackendHandle handle,
    const char* objectClassName,
    const QttestHlaStringArrayV1* attributeNames) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !objectClassName || !*objectClassName || !attributeNames ||
      attributeNames->structSize < sizeof(QttestHlaStringArrayV1) ||
      !attributeNames->values || attributeNames->count == 0) {
    return fail(value, "Invalid HLA object subscription", QTTEST_HLA_STATE_ERROR);
  }
  try {
    const std::wstring className = fromUtf8(objectClassName);
    const rti1516e::ObjectClassHandle classHandle =
        value->rtiAmbassador->getObjectClassHandle(className);
    rti1516e::AttributeHandleSet attributes;
    PluginFederateAmbassador::SubscribedObjectClass subscription;
    subscription.name = objectClassName;
    for (size_t index = 0; index < attributeNames->count; ++index) {
      const std::string attributeName = attributeNames->values[index];
      const rti1516e::AttributeHandle attributeHandle =
          value->rtiAmbassador->getAttributeHandle(
              classHandle, fromUtf8(attributeNames->values[index]));
      attributes.insert(attributeHandle);
      subscription.attributes.emplace(attributeHandle, attributeName);
      value->federateAmbassador.attributeNames[classHandle][attributeHandle] =
          attributeName;
    }
    value->rtiAmbassador->subscribeObjectClassAttributes(classHandle, attributes);
    value->federateAmbassador.subscribedObjectClasses[classHandle] =
        std::move(subscription);
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
    value->federateAmbassador.localObjectIds[objectHandle] = localId;
    value->federateAmbassador.localObjectClasses[objectHandle] =
        classIterator->second;
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
  rti1516e::ObjectInstanceHandle objectHandle;
  rti1516e::ObjectClassHandle classHandle;
  if (!resolveObject(value, instanceId, objectHandle, classHandle)) {
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
              classHandle, fromUtf8(attribute.name));
      attributeValues[attributeHandle] = variableLengthData(&attribute.value);
    }
    value->rtiAmbassador->updateAttributeValues(
        objectHandle, attributeValues, variableLengthData(tag));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int updateObjectAttributesAtTime(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaNamedValueArrayV2* attributes,
    double logicalTimeSeconds,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !attributes ||
      attributes->structSize < sizeof(QttestHlaNamedValueArrayV2) ||
      !attributes->values || attributes->count == 0 ||
      logicalTimeSeconds < 0.0) {
    return fail(
        value, "Invalid timestamped HLA attribute update",
        QTTEST_HLA_STATE_ERROR);
  }
  rti1516e::ObjectInstanceHandle objectHandle;
  rti1516e::ObjectClassHandle classHandle;
  if (!resolveObject(value, instanceId, objectHandle, classHandle)) {
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
              classHandle, fromUtf8(attribute.name));
      attributeValues[attributeHandle] = variableLengthData(&attribute.value);
    }
    value->rtiAmbassador->updateAttributeValues(
        objectHandle,
        attributeValues,
        variableLengthData(tag),
        rti1516e::HLAfloat64Time(logicalTimeSeconds));
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
    value->federateAmbassador.localObjectIds.erase(
        objectIterator->second.handle);
    value->federateAmbassador.localObjectClasses.erase(
        objectIterator->second.handle);
    value->objects.erase(objectIterator);
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int deleteObjectInstanceAtTime(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    double logicalTimeSeconds,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      logicalTimeSeconds < 0.0) {
    return fail(
        value, "Invalid timestamped HLA object deletion",
        QTTEST_HLA_STATE_ERROR);
  }
  const auto objectIterator = value->objects.find(instanceId);
  if (objectIterator == value->objects.end()) {
    return fail(value, "Unknown HLA object instance", QTTEST_HLA_STATE_JOINED);
  }
  try {
    value->rtiAmbassador->deleteObjectInstance(
        objectIterator->second.handle,
        variableLengthData(tag),
        rti1516e::HLAfloat64Time(logicalTimeSeconds));
    value->federateAmbassador.localObjectIds.erase(
        objectIterator->second.handle);
    value->federateAmbassador.localObjectClasses.erase(
        objectIterator->second.handle);
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

int subscribeInteractionClass(
    QttestHlaBackendHandle handle,
    const char* interactionClassName,
    const QttestHlaStringArrayV1* parameterNames) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !interactionClassName || !*interactionClassName || !parameterNames ||
      parameterNames->structSize < sizeof(QttestHlaStringArrayV1)) {
    return fail(
        value, "Invalid HLA interaction subscription", QTTEST_HLA_STATE_ERROR);
  }
  try {
    const rti1516e::InteractionClassHandle interactionHandle =
        value->rtiAmbassador->getInteractionClassHandle(
            fromUtf8(interactionClassName));
    std::map<rti1516e::ParameterHandle, std::string> parameters;
    for (size_t index = 0; index < parameterNames->count; ++index) {
      const rti1516e::ParameterHandle parameterHandle =
          value->rtiAmbassador->getParameterHandle(
              interactionHandle, fromUtf8(parameterNames->values[index]));
      parameters.emplace(parameterHandle, parameterNames->values[index]);
    }
    value->rtiAmbassador->subscribeInteractionClass(interactionHandle);
    value->federateAmbassador.subscribedInteractions[interactionHandle] = {
        interactionClassName, std::move(parameters)};
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int registerSynchronizationPoint(
    QttestHlaBackendHandle handle,
    const char* label,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !label || !*label) {
    return fail(
        value, "Invalid HLA synchronization point", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->registerFederationSynchronizationPoint(
        fromUtf8(label), variableLengthData(tag));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int achieveSynchronizationPoint(
    QttestHlaBackendHandle handle,
    const char* label) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED || !label || !*label) {
    return fail(
        value,
        "Invalid HLA synchronization achievement",
        QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->synchronizationPointAchieved(fromUtf8(label), true);
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int enableTimeRegulation(
    QttestHlaBackendHandle handle,
    double lookaheadSeconds) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      lookaheadSeconds <= 0.0) {
    return fail(value, "Invalid HLA time regulation request", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->enableTimeRegulation(
        rti1516e::HLAfloat64Interval(lookaheadSeconds));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int enableTimeConstrained(QttestHlaBackendHandle handle) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED) {
    return fail(value, "Invalid HLA time constrained request", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->enableTimeConstrained();
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int requestTimeAdvance(
    QttestHlaBackendHandle handle,
    double logicalTimeSeconds) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      logicalTimeSeconds < 0.0) {
    return fail(value, "Invalid HLA time advance request", QTTEST_HLA_STATE_ERROR);
  }
  try {
    value->rtiAmbassador->timeAdvanceRequest(
        rti1516e::HLAfloat64Time(logicalTimeSeconds));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int requestAttributeOwnershipAcquisition(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaStringArrayV1* attributeNames,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      instanceId == 0 || !attributeNames || !attributeNames->values ||
      attributeNames->count == 0) {
    return fail(value, "Invalid HLA ownership acquisition", QTTEST_HLA_STATE_ERROR);
  }
  try {
    rti1516e::ObjectInstanceHandle objectHandle;
    rti1516e::ObjectClassHandle classHandle;
    if (!resolveObject(value, instanceId, objectHandle, classHandle)) {
      return fail(
          value, "Unknown HLA object instance",
          QTTEST_HLA_STATE_JOINED);
    }
    rti1516e::AttributeHandleSet attributes;
    for (size_t index = 0; index < attributeNames->count; ++index) {
      attributes.insert(value->rtiAmbassador->getAttributeHandle(
          classHandle, fromUtf8(attributeNames->values[index])));
    }
    value->rtiAmbassador->attributeOwnershipAcquisition(
        objectHandle, attributes, variableLengthData(tag));
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int unconditionalAttributeOwnershipDivestiture(
    QttestHlaBackendHandle handle,
    uint64_t instanceId,
    const QttestHlaStringArrayV1* attributeNames) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      instanceId == 0 || !attributeNames || !attributeNames->values ||
      attributeNames->count == 0) {
    return fail(value, "Invalid HLA ownership divestiture", QTTEST_HLA_STATE_ERROR);
  }
  rti1516e::ObjectInstanceHandle objectHandle;
  rti1516e::ObjectClassHandle classHandle;
  if (!resolveObject(value, instanceId, objectHandle, classHandle)) {
    return fail(
        value, "Unknown HLA object instance",
        QTTEST_HLA_STATE_JOINED);
  }
  try {
    rti1516e::AttributeHandleSet attributes;
    for (size_t index = 0; index < attributeNames->count; ++index) {
      attributes.insert(value->rtiAmbassador->getAttributeHandle(
          classHandle,
          fromUtf8(attributeNames->values[index])));
    }
    value->rtiAmbassador->unconditionalAttributeOwnershipDivestiture(
        objectHandle, attributes);
    value->error.clear();
    return 0;
  } catch (const rti1516e::Exception& exception) {
    return fail(value, exception, QTTEST_HLA_STATE_JOINED);
  } catch (const std::exception& exception) {
    return fail(value, exception.what(), QTTEST_HLA_STATE_JOINED);
  }
}

int setCallbacks(
    QttestHlaBackendHandle handle,
    const QttestHlaCallbacksV8* callbacks) {
  PitchSession* value = session(handle);
  if (!value || !callbacks ||
      callbacks->structSize < sizeof(QttestHlaCallbacksV8)) {
    return fail(value, "Invalid HLA callback configuration", QTTEST_HLA_STATE_ERROR);
  }
  value->federateAmbassador.callbacks = *callbacks;
  value->error.clear();
  return 0;
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

int sendInteractionAtTime(
    QttestHlaBackendHandle handle,
    const char* interactionClassName,
    const QttestHlaNamedValueArrayV2* parameters,
    double logicalTimeSeconds,
    const QttestHlaByteSpanV2* tag) {
  PitchSession* value = session(handle);
  if (!value || value->state != QTTEST_HLA_STATE_JOINED ||
      !interactionClassName || !*interactionClassName || !parameters ||
      parameters->structSize < sizeof(QttestHlaNamedValueArrayV2) ||
      logicalTimeSeconds < 0.0) {
    return fail(
        value, "Invalid timestamped HLA interaction",
        QTTEST_HLA_STATE_ERROR);
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
        interactionIterator->second,
        parameterValues,
        variableLengthData(tag),
        rti1516e::HLAfloat64Time(logicalTimeSeconds));
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

#ifdef QTTEST_HLA_OPENRTI_BACKEND
constexpr const char* backendCapabilities =
    "federation-management,object-management,interactions,synchronization-points,time-management,timestamp-order,evoked-callbacks,ieee1516e";
#else
constexpr const char* backendCapabilities =
    "federation-management,object-management,ownership-management,interactions,synchronization-points,time-management,timestamp-order,evoked-callbacks,ieee1516e";
#endif

const QttestHlaBackendApiV8 api = {
    sizeof(QttestHlaBackendApiV8),
    QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION,
    QTTEST_HLA_1516E_BACKEND_ID,
    QTTEST_HLA_1516E_BACKEND_NAME,
    backendCapabilities,
    &createBackend,
    &destroyBackend,
    &connectBackend,
    &createFederation,
    &joinFederation,
    &publishObjectClass,
    &subscribeObjectClass,
    &registerObjectInstance,
    &updateObjectAttributes,
    &updateObjectAttributesAtTime,
    &deleteObjectInstance,
    &deleteObjectInstanceAtTime,
    &publishInteractionClass,
    &subscribeInteractionClass,
    &sendInteraction,
    &sendInteractionAtTime,
    &registerSynchronizationPoint,
    &achieveSynchronizationPoint,
    &enableTimeRegulation,
    &enableTimeConstrained,
    &requestTimeAdvance,
    &requestAttributeOwnershipAcquisition,
    &unconditionalAttributeOwnershipDivestiture,
    &setCallbacks,
    &pollBackend,
    &resignBackend,
    &disconnectBackend,
    &backendState,
    &lastError};

} // namespace

extern "C" QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV8*
qttest_hla_backend_api_v8(void) {
  return &api;
}
