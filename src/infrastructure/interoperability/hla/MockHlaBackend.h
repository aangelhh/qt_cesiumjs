#pragma once

#include "infrastructure/interoperability/hla/IHlaBackend.h"

#include <optional>

namespace tactical::hla {

class MockHlaBackend final : public IHlaBackend {
public:
  struct AttributeUpdate {
    ObjectInstanceId instanceId = 0;
    std::vector<NamedValue> attributes;
  };

  struct SentInteraction {
    std::string interactionClassName;
    std::vector<NamedValue> parameters;
  };

  enum class Operation {
    Connect,
    CreateFederation,
    JoinFederation,
    PublishObjectClass,
    SubscribeObjectClass,
    RegisterObjectInstance,
    UpdateObjectAttributes,
    DeleteObjectInstance,
    PublishInteractionClass,
    SubscribeInteractionClass,
    SendInteraction,
    RegisterSynchronizationPoint,
    AchieveSynchronizationPoint,
    EnableTimeRegulation,
    EnableTimeConstrained,
    RequestTimeAdvance,
    Poll,
    Resign,
    Disconnect
  };

  std::string id() const override;
  std::string version() const override;
  std::vector<std::string> capabilities() const override;
  Result connect(const std::string& localSettingsDesignator) override;
  Result createFederation(
      const std::string& federationName,
      const std::vector<std::string>& fomModules) override;
  Result joinFederation(
      const std::string& federateName,
      const std::string& federateType,
      const std::string& federationName,
      const std::vector<std::string>& additionalFomModules) override;
  Result publishObjectClass(
      const std::string& objectClassName,
      const std::vector<std::string>& attributeNames) override;
  Result subscribeObjectClass(
      const std::string& objectClassName,
      const std::vector<std::string>& attributeNames) override;
  Result registerObjectInstance(
      const std::string& objectClassName,
      const std::string& instanceName,
      ObjectInstanceId& instanceId) override;
  Result updateObjectAttributes(
      ObjectInstanceId instanceId,
      const std::vector<NamedValue>& attributes,
      const ByteBuffer& tag) override;
  Result deleteObjectInstance(
      ObjectInstanceId instanceId,
      const ByteBuffer& tag) override;
  Result publishInteractionClass(
      const std::string& interactionClassName) override;
  Result subscribeInteractionClass(
      const std::string& interactionClassName,
      const std::vector<std::string>& parameterNames) override;
  Result sendInteraction(
      const std::string& interactionClassName,
      const std::vector<NamedValue>& parameters,
      const ByteBuffer& tag) override;
  Result registerSynchronizationPoint(
      const std::string& label,
      const ByteBuffer& tag) override;
  Result achieveSynchronizationPoint(const std::string& label) override;
  Result enableTimeRegulation(double lookaheadSeconds) override;
  Result enableTimeConstrained() override;
  Result requestTimeAdvance(double logicalTimeSeconds) override;
  Result poll(double maximumSeconds) override;
  void setEventSink(IHlaEventSink* eventSink) override;
  Result resign() override;
  Result disconnect() override;
  BackendState state() const override;
  std::string lastError() const override;

  void failNext(Operation operation, std::string message);
  const std::vector<Operation>& operations() const;
  const std::vector<std::string>& publishedObjectClasses() const;
  const std::vector<std::string>& subscribedObjectClasses() const;
  const std::vector<std::string>& sentInteractionClasses() const;
  const std::vector<SentInteraction>& sentInteractions() const;
  const std::vector<AttributeUpdate>& attributeUpdates() const;
  void emitObjectDiscovered(const RemoteObjectDiscovery& event);
  void emitObjectReflected(const RemoteObjectReflection& event);
  void emitObjectRemoved(const RemoteObjectRemoval& event);
  void emitInteraction(const RemoteInteraction& event);
  void emitSynchronizationPointAnnounced(
      const SynchronizationPointAnnouncement& event);
  void emitFederationSynchronized(const std::string& label);
  void emitTimeRegulationEnabled(double logicalTimeSeconds);
  void emitTimeConstrainedEnabled(double logicalTimeSeconds);
  void emitTimeAdvanceGranted(double logicalTimeSeconds);

private:
  Result begin(Operation operation);
  Result fail(std::string message);

  BackendState _state = BackendState::Disconnected;
  std::string _lastError;
  std::optional<Operation> _failureOperation;
  std::string _failureMessage;
  std::vector<Operation> _operations;
  ObjectInstanceId _nextObjectInstanceId = 1;
  std::vector<ObjectInstanceId> _objectInstances;
  std::vector<std::string> _publishedObjectClasses;
  std::vector<std::string> _subscribedObjectClasses;
  std::vector<std::string> _sentInteractionClasses;
  std::vector<SentInteraction> _sentInteractions;
  std::vector<AttributeUpdate> _attributeUpdates;
  IHlaEventSink* _eventSink = nullptr;
};

} // namespace tactical::hla
