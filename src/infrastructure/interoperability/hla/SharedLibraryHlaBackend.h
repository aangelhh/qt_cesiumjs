#pragma once

#include "infrastructure/interoperability/hla/HlaBackendPluginApi.h"
#include "infrastructure/interoperability/hla/IHlaBackend.h"

#include <QLibrary>

namespace tactical::hla {

class SharedLibraryHlaBackend final : public IHlaBackend {
public:
  explicit SharedLibraryHlaBackend(std::string libraryPath);
  ~SharedLibraryHlaBackend() override;

  SharedLibraryHlaBackend(const SharedLibraryHlaBackend&) = delete;
  SharedLibraryHlaBackend& operator=(const SharedLibraryHlaBackend&) = delete;

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
  Result poll(double maximumSeconds) override;
  void setEventSink(IHlaEventSink* eventSink) override;
  Result resign() override;
  Result disconnect() override;
  BackendState state() const override;
  std::string lastError() const override;

  bool isAvailable() const;
  const std::string& loadError() const;
  const std::string& libraryPath() const;

private:
  Result pluginResult(int status) const;
  static BackendState mapState(QttestHlaBackendStateV1 state);
  static void objectDiscoveredCallback(
      void* context,
      uint64_t instanceId,
      const char* objectClassName,
      const char* instanceName);
  static void objectReflectedCallback(
      void* context,
      uint64_t instanceId,
      const QttestHlaNamedValueArrayV2* attributes,
      const QttestHlaByteSpanV2* tag);
  static void objectRemovedCallback(
      void* context,
      uint64_t instanceId,
      const QttestHlaByteSpanV2* tag);
  static void interactionReceivedCallback(
      void* context,
      const char* interactionClassName,
      const QttestHlaNamedValueArrayV2* parameters,
      const QttestHlaByteSpanV2* tag);
  static void synchronizationPointAnnouncedCallback(
      void* context,
      const char* label,
      const QttestHlaByteSpanV2* tag);
  static void federationSynchronizedCallback(
      void* context,
      const char* label);

  std::string _libraryPath;
  mutable QLibrary _library;
  const QttestHlaBackendApiV4* _api = nullptr;
  QttestHlaBackendHandle _handle = nullptr;
  std::string _loadError;
  IHlaEventSink* _eventSink = nullptr;
};

} // namespace tactical::hla
