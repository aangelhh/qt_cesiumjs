#pragma once

#include "infrastructure/interoperability/hla/IHlaBackend.h"

#include <memory>

namespace tactical::hla {

class OpenRtiCompatibilityBackend final : public IHlaBackend {
public:
  explicit OpenRtiCompatibilityBackend(std::unique_ptr<IHlaBackend> backend);

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
  Result poll(double maximumSeconds) override;
  void setEventSink(IHlaEventSink* eventSink) override;
  Result resign() override;
  Result disconnect() override;

  BackendState state() const override;
  std::string lastError() const override;

private:
  std::unique_ptr<IHlaBackend> _backend;
  std::string _compatibilityError;
};

} // namespace tactical::hla
