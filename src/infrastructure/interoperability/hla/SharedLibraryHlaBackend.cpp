#include "infrastructure/interoperability/hla/SharedLibraryHlaBackend.h"

#include <QString>

#include <sstream>
#include <utility>

namespace {

QttestHlaStringArrayV1 makeStringArray(
    const std::vector<std::string>& strings,
    std::vector<const char*>& values) {
  values.reserve(strings.size());
  for (const std::string& value : strings) {
    values.push_back(value.c_str());
  }
  return {
      sizeof(QttestHlaStringArrayV1),
      values.empty() ? nullptr : values.data(),
      values.size()};
}

QttestHlaByteSpanV2 makeByteSpan(const tactical::hla::ByteBuffer& bytes) {
  return {
      sizeof(QttestHlaByteSpanV2),
      bytes.empty() ? nullptr : bytes.data(),
      bytes.size()};
}

QttestHlaNamedValueArrayV2 makeNamedValueArray(
    const std::vector<tactical::hla::NamedValue>& source,
    std::vector<QttestHlaNamedValueV2>& values) {
  values.reserve(source.size());
  for (const tactical::hla::NamedValue& item : source) {
    values.push_back({
        sizeof(QttestHlaNamedValueV2),
        item.name.c_str(),
        makeByteSpan(item.value)});
  }
  return {
      sizeof(QttestHlaNamedValueArrayV2),
      values.empty() ? nullptr : values.data(),
      values.size()};
}

std::string safeString(const char* value) {
  return value ? std::string(value) : std::string();
}

} // namespace

namespace tactical::hla {

SharedLibraryHlaBackend::SharedLibraryHlaBackend(std::string libraryPath)
    : _libraryPath(std::move(libraryPath)),
      _library(QString::fromUtf8(_libraryPath.c_str())) {
  if (_libraryPath.empty()) {
    _loadError = "HLA backend library path is empty";
    return;
  }
  if (!_library.load()) {
    _loadError = _library.errorString().toStdString();
    return;
  }

  const auto apiFactory = reinterpret_cast<QttestHlaBackendApiFn>(
      _library.resolve("qttest_hla_backend_api_v2"));
  if (!apiFactory) {
    _loadError = "Required HLA backend API symbol is missing";
    _library.unload();
    return;
  }

  _api = apiFactory();
  if (!_api || _api->structSize < sizeof(QttestHlaBackendApiV2) ||
      _api->abiVersion != QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION) {
    _loadError = "Unsupported HLA backend plugin ABI";
    _api = nullptr;
    _library.unload();
    return;
  }
  if (!_api->backendId || !_api->create || !_api->destroy ||
      !_api->connect || !_api->createFederation || !_api->joinFederation ||
      !_api->publishObjectClass || !_api->registerObjectInstance ||
      !_api->updateObjectAttributes || !_api->deleteObjectInstance ||
      !_api->publishInteractionClass || !_api->sendInteraction ||
      !_api->poll || !_api->resign || !_api->disconnect || !_api->state ||
      !_api->lastError) {
    _loadError = "HLA backend plugin API is incomplete";
    _api = nullptr;
    _library.unload();
    return;
  }

  _handle = _api->create();
  if (!_handle) {
    _loadError = "HLA backend plugin could not create a session";
    _api = nullptr;
    _library.unload();
  }
}

SharedLibraryHlaBackend::~SharedLibraryHlaBackend() {
  if (_api && _handle) {
    _api->destroy(_handle);
  }
  _handle = nullptr;
  _api = nullptr;
  if (_library.isLoaded()) {
    _library.unload();
  }
}

std::string SharedLibraryHlaBackend::id() const {
  return _api ? safeString(_api->backendId) : std::string();
}

std::string SharedLibraryHlaBackend::version() const {
  return _api ? safeString(_api->backendVersion) : std::string();
}

std::vector<std::string> SharedLibraryHlaBackend::capabilities() const {
  std::vector<std::string> values;
  if (!_api || !_api->capabilities) {
    return values;
  }

  std::stringstream stream(_api->capabilities);
  std::string value;
  while (std::getline(stream, value, ',')) {
    if (!value.empty()) {
      values.push_back(std::move(value));
    }
  }
  return values;
}

Result SharedLibraryHlaBackend::connect(
    const std::string& localSettingsDesignator) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  return this->pluginResult(
      _api->connect(_handle, localSettingsDesignator.c_str()));
}

Result SharedLibraryHlaBackend::createFederation(
    const std::string& federationName,
    const std::vector<std::string>& fomModules) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  std::vector<const char*> values;
  const QttestHlaStringArrayV1 modules = makeStringArray(fomModules, values);
  return this->pluginResult(
      _api->createFederation(_handle, federationName.c_str(), &modules));
}

Result SharedLibraryHlaBackend::joinFederation(
    const std::string& federateName,
    const std::string& federateType,
    const std::string& federationName,
    const std::vector<std::string>& additionalFomModules) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  std::vector<const char*> values;
  const QttestHlaStringArrayV1 modules =
      makeStringArray(additionalFomModules, values);
  return this->pluginResult(_api->joinFederation(
      _handle,
      federateName.c_str(),
      federateType.c_str(),
      federationName.c_str(),
      &modules));
}

Result SharedLibraryHlaBackend::publishObjectClass(
    const std::string& objectClassName,
    const std::vector<std::string>& attributeNames) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  std::vector<const char*> values;
  const QttestHlaStringArrayV1 attributes =
      makeStringArray(attributeNames, values);
  return this->pluginResult(_api->publishObjectClass(
      _handle, objectClassName.c_str(), &attributes));
}

Result SharedLibraryHlaBackend::registerObjectInstance(
    const std::string& objectClassName,
    const std::string& instanceName,
    ObjectInstanceId& instanceId) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  return this->pluginResult(_api->registerObjectInstance(
      _handle, objectClassName.c_str(), instanceName.c_str(), &instanceId));
}

Result SharedLibraryHlaBackend::updateObjectAttributes(
    ObjectInstanceId instanceId,
    const std::vector<NamedValue>& attributes,
    const ByteBuffer& tag) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  std::vector<QttestHlaNamedValueV2> values;
  const QttestHlaNamedValueArrayV2 valueArray =
      makeNamedValueArray(attributes, values);
  const QttestHlaByteSpanV2 tagSpan = makeByteSpan(tag);
  return this->pluginResult(_api->updateObjectAttributes(
      _handle, instanceId, &valueArray, &tagSpan));
}

Result SharedLibraryHlaBackend::deleteObjectInstance(
    ObjectInstanceId instanceId,
    const ByteBuffer& tag) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  const QttestHlaByteSpanV2 tagSpan = makeByteSpan(tag);
  return this->pluginResult(
      _api->deleteObjectInstance(_handle, instanceId, &tagSpan));
}

Result SharedLibraryHlaBackend::publishInteractionClass(
    const std::string& interactionClassName) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  return this->pluginResult(_api->publishInteractionClass(
      _handle, interactionClassName.c_str()));
}

Result SharedLibraryHlaBackend::sendInteraction(
    const std::string& interactionClassName,
    const std::vector<NamedValue>& parameters,
    const ByteBuffer& tag) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  std::vector<QttestHlaNamedValueV2> values;
  const QttestHlaNamedValueArrayV2 valueArray =
      makeNamedValueArray(parameters, values);
  const QttestHlaByteSpanV2 tagSpan = makeByteSpan(tag);
  return this->pluginResult(_api->sendInteraction(
      _handle, interactionClassName.c_str(), &valueArray, &tagSpan));
}

Result SharedLibraryHlaBackend::poll(double maximumSeconds) {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  return this->pluginResult(_api->poll(_handle, maximumSeconds));
}

Result SharedLibraryHlaBackend::resign() {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  return this->pluginResult(_api->resign(_handle));
}

Result SharedLibraryHlaBackend::disconnect() {
  if (!_api || !_handle) {
    return Result::failure(_loadError);
  }
  return this->pluginResult(_api->disconnect(_handle));
}

BackendState SharedLibraryHlaBackend::state() const {
  if (!_api || !_handle) {
    return BackendState::Unavailable;
  }
  return mapState(_api->state(_handle));
}

std::string SharedLibraryHlaBackend::lastError() const {
  if (!_api || !_handle) {
    return _loadError;
  }
  return safeString(_api->lastError(_handle));
}

bool SharedLibraryHlaBackend::isAvailable() const {
  return _api && _handle && _library.isLoaded();
}

const std::string& SharedLibraryHlaBackend::loadError() const {
  return _loadError;
}

const std::string& SharedLibraryHlaBackend::libraryPath() const {
  return _libraryPath;
}

Result SharedLibraryHlaBackend::pluginResult(int status) const {
  if (status == 0) {
    return Result::ok();
  }
  std::string error = this->lastError();
  if (error.empty()) {
    error = "HLA backend operation failed with status " +
        std::to_string(status);
  }
  return Result::failure(std::move(error));
}

BackendState SharedLibraryHlaBackend::mapState(
    QttestHlaBackendStateV1 state) {
  switch (state) {
    case QTTEST_HLA_STATE_DISCONNECTED:
      return BackendState::Disconnected;
    case QTTEST_HLA_STATE_CONNECTED:
      return BackendState::Connected;
    case QTTEST_HLA_STATE_JOINED:
      return BackendState::Joined;
    case QTTEST_HLA_STATE_ERROR:
      return BackendState::Error;
    case QTTEST_HLA_STATE_UNAVAILABLE:
    default:
      return BackendState::Unavailable;
  }
}

} // namespace tactical::hla
