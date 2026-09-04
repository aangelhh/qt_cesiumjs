#pragma once

#include <cstdint>
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

struct RemoteObjectDiscovery {
  ObjectInstanceId instanceId = 0;
  std::string objectClassName;
  std::string instanceName;
};

struct RemoteObjectReflection {
  ObjectInstanceId instanceId = 0;
  std::vector<NamedValue> attributes;
  ByteBuffer tag;
};

struct RemoteObjectRemoval {
  ObjectInstanceId instanceId = 0;
  ByteBuffer tag;
};

struct RemoteInteraction {
  std::string interactionClassName;
  std::vector<NamedValue> parameters;
  ByteBuffer tag;
};

struct SynchronizationPointAnnouncement {
  std::string label;
  ByteBuffer tag;
};

class IHlaEventSink {
public:
  virtual ~IHlaEventSink() = default;
  virtual void onObjectDiscovered(const RemoteObjectDiscovery& event) = 0;
  virtual void onObjectReflected(const RemoteObjectReflection& event) = 0;
  virtual void onObjectRemoved(const RemoteObjectRemoval& event) = 0;
  virtual void onInteractionReceived(const RemoteInteraction& event) = 0;
  virtual void onSynchronizationPointAnnounced(
      const SynchronizationPointAnnouncement& event) = 0;
  virtual void onFederationSynchronized(const std::string& label) = 0;
};

} // namespace tactical::hla
