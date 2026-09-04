#pragma once

#include "infrastructure/interoperability/hla/IHlaBackend.h"

#include <memory>

namespace tactical::hla {

class HlaRuntime {
public:
  explicit HlaRuntime(std::unique_ptr<IHlaBackend> backend);
  ~HlaRuntime();

  HlaRuntime(const HlaRuntime&) = delete;
  HlaRuntime& operator=(const HlaRuntime&) = delete;

  Result start(const SessionConfiguration& configuration);
  Result publishObjectClass(
      const std::string& objectClassName,
      const std::vector<std::string>& attributeNames);
  Result subscribeObjectClass(
      const std::string& objectClassName,
      const std::vector<std::string>& attributeNames);
  Result registerObjectInstance(
      const std::string& objectClassName,
      const std::string& instanceName,
      ObjectInstanceId& instanceId);
  Result updateObjectAttributes(
      ObjectInstanceId instanceId,
      const std::vector<NamedValue>& attributes,
      const ByteBuffer& tag = {});
  Result deleteObjectInstance(
      ObjectInstanceId instanceId,
      const ByteBuffer& tag = {});
  Result publishInteractionClass(const std::string& interactionClassName);
  Result subscribeInteractionClass(
      const std::string& interactionClassName,
      const std::vector<std::string>& parameterNames);
  Result sendInteraction(
      const std::string& interactionClassName,
      const std::vector<NamedValue>& parameters,
      const ByteBuffer& tag = {});
  Result registerSynchronizationPoint(
      const std::string& label,
      const ByteBuffer& tag = {});
  Result achieveSynchronizationPoint(const std::string& label);
  Result poll(double maximumSeconds);
  void setEventSink(IHlaEventSink* eventSink);
  Result stop();

  const IHlaBackend* backend() const;
  BackendState state() const;

private:
  Result rollback(const Result& originalFailure);

  std::unique_ptr<IHlaBackend> _backend;
};

} // namespace tactical::hla
