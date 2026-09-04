#pragma once

#include "infrastructure/interoperability/hla/HlaTypes.h"

#include <string>
#include <vector>

namespace tactical::hla {

class IHlaBackend {
public:
  virtual ~IHlaBackend() = default;

  virtual std::string id() const = 0;
  virtual std::string version() const = 0;
  virtual std::vector<std::string> capabilities() const = 0;

  virtual Result connect(const std::string& localSettingsDesignator) = 0;
  virtual Result createFederation(
      const std::string& federationName,
      const std::vector<std::string>& fomModules) = 0;
  virtual Result joinFederation(
      const std::string& federateName,
      const std::string& federateType,
      const std::string& federationName,
      const std::vector<std::string>& additionalFomModules) = 0;
  virtual Result publishObjectClass(
      const std::string& objectClassName,
      const std::vector<std::string>& attributeNames) = 0;
  virtual Result subscribeObjectClass(
      const std::string& objectClassName,
      const std::vector<std::string>& attributeNames) = 0;
  virtual Result registerObjectInstance(
      const std::string& objectClassName,
      const std::string& instanceName,
      ObjectInstanceId& instanceId) = 0;
  virtual Result updateObjectAttributes(
      ObjectInstanceId instanceId,
      const std::vector<NamedValue>& attributes,
      const ByteBuffer& tag = {}) = 0;
  virtual Result deleteObjectInstance(
      ObjectInstanceId instanceId,
      const ByteBuffer& tag = {}) = 0;
  virtual Result publishInteractionClass(
      const std::string& interactionClassName) = 0;
  virtual Result subscribeInteractionClass(
      const std::string& interactionClassName,
      const std::vector<std::string>& parameterNames) = 0;
  virtual Result sendInteraction(
      const std::string& interactionClassName,
      const std::vector<NamedValue>& parameters,
      const ByteBuffer& tag = {}) = 0;
  virtual Result registerSynchronizationPoint(
      const std::string& label,
      const ByteBuffer& tag = {}) = 0;
  virtual Result achieveSynchronizationPoint(
      const std::string& label) = 0;
  virtual Result poll(double maximumSeconds) = 0;
  virtual void setEventSink(IHlaEventSink* eventSink) = 0;
  virtual Result resign() = 0;
  virtual Result disconnect() = 0;

  virtual BackendState state() const = 0;
  virtual std::string lastError() const = 0;
};

} // namespace tactical::hla
