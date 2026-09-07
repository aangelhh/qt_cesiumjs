#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace tactical::hla {

enum class BackendState {
  Unavailable,
  Disconnected,
  Connected,
  Joined,
  Error
};

struct Result {
  bool success = false;
  std::string message;

  static Result ok() { return {true, {}}; }
  static Result failure(std::string message) {
    return {false, std::move(message)};
  }
};

struct SessionConfiguration {
  std::string localSettingsDesignator;
  std::string federationName;
  std::string federateName;
  std::string federateType;
  std::vector<std::string> fomModules;
  bool createFederationIfMissing = true;
};

using ObjectInstanceId = std::uint64_t;
using ByteBuffer = std::vector<std::uint8_t>;

struct NamedValue {
  std::string name;
  ByteBuffer value;
};

enum class DeliveryOrder {
  Receive,
  Timestamp
};

struct ReceiveMetadata {
  DeliveryOrder order = DeliveryOrder::Receive;
  std::optional<double> logicalTimeSeconds;
};

struct RemoteObjectDiscovery {
  ObjectInstanceId instanceId = 0;
  std::string objectClassName;
  std::string instanceName;
};

struct RemoteObjectReflection {
  ObjectInstanceId instanceId = 0;
  std::vector<NamedValue> attributes;
  ByteBuffer tag;
  ReceiveMetadata receiveMetadata;

  RemoteObjectReflection() = default;
  RemoteObjectReflection(
      ObjectInstanceId instanceIdValue,
      std::vector<NamedValue> attributeValues,
      ByteBuffer tagValue,
      ReceiveMetadata metadata = {})
      : instanceId(instanceIdValue),
        attributes(std::move(attributeValues)),
        tag(std::move(tagValue)),
        receiveMetadata(std::move(metadata)) {}
};

struct RemoteObjectRemoval {
  ObjectInstanceId instanceId = 0;
  ByteBuffer tag;
  ReceiveMetadata receiveMetadata;

  RemoteObjectRemoval() = default;
  RemoteObjectRemoval(
      ObjectInstanceId instanceIdValue,
      ByteBuffer tagValue,
      ReceiveMetadata metadata = {})
      : instanceId(instanceIdValue),
        tag(std::move(tagValue)),
        receiveMetadata(std::move(metadata)) {}
};

struct RemoteInteraction {
  std::string interactionClassName;
  std::vector<NamedValue> parameters;
  ByteBuffer tag;
  ReceiveMetadata receiveMetadata;

  RemoteInteraction() = default;
  RemoteInteraction(
      std::string className,
      std::vector<NamedValue> parameterValues,
      ByteBuffer tagValue,
      ReceiveMetadata metadata = {})
      : interactionClassName(std::move(className)),
        parameters(std::move(parameterValues)),
        tag(std::move(tagValue)),
        receiveMetadata(std::move(metadata)) {}
};

struct SynchronizationPointAnnouncement {
  std::string label;
  ByteBuffer tag;
};

struct SynchronizationPointRegistrationResult {
  std::string label;
  bool succeeded = false;
  std::string reason;
};

enum class OwnershipEventKind {
  Acquired,
  Unavailable,
  ReleaseRequested
};

struct AttributeOwnershipEvent {
  OwnershipEventKind kind = OwnershipEventKind::Unavailable;
  ObjectInstanceId instanceId = 0;
  std::vector<std::string> attributeNames;
  ByteBuffer tag;
};

struct ConnectionLostEvent {
  std::string reason;
};

class IHlaEventSink {
public:
  virtual ~IHlaEventSink() = default;
  virtual void onObjectDiscovered(const RemoteObjectDiscovery& event) = 0;
  virtual void onObjectReflected(const RemoteObjectReflection& event) = 0;
  virtual void onObjectRemoved(const RemoteObjectRemoval& event) = 0;
  virtual void onInteractionReceived(const RemoteInteraction& event) = 0;
  virtual void onSynchronizationPointRegistrationResult(
      const SynchronizationPointRegistrationResult& event) = 0;
  virtual void onSynchronizationPointAnnounced(
      const SynchronizationPointAnnouncement& event) = 0;
  virtual void onFederationSynchronized(const std::string& label) = 0;
  virtual void onTimeRegulationEnabled(double logicalTimeSeconds) = 0;
  virtual void onTimeConstrainedEnabled(double logicalTimeSeconds) = 0;
  virtual void onTimeAdvanceGranted(double logicalTimeSeconds) = 0;
  virtual void onAttributeOwnershipChanged(
      const AttributeOwnershipEvent& event) = 0;
  virtual void onConnectionLost(const ConnectionLostEvent& event) = 0;
};

} // namespace tactical::hla
