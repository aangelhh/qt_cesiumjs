#include "CesiumRuntime.h"

#include "NullPrepareRendererResources.h"

#include <Cesium3DTilesSelection/TilesetOptions.h>
#include <Cesium3DTilesSelection/TilesetLoadFailureDetails.h>
#include <CesiumCurl/CurlAssetAccessor.h>
#include <CesiumUtility/CreditSystem.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace {

std::string trimCopy(std::string value) {
  const auto first = std::find_if_not(
      value.begin(),
      value.end(),
      [](unsigned char ch) { return std::isspace(ch) != 0; });
  const auto last = std::find_if_not(
      value.rbegin(),
      value.rend(),
      [](unsigned char ch) { return std::isspace(ch) != 0; })
                        .base();

  if (first >= last) {
    return {};
  }

  return std::string(first, last);
}

std::unordered_map<std::string, std::string> readConfigFile(
    const std::string& path) {
  std::unordered_map<std::string, std::string> values;
  std::ifstream input(path);
  if (!input) {
    return values;
  }

  std::string line;
  while (std::getline(input, line)) {
    const std::string trimmed = trimCopy(line);
    if (trimmed.empty() || trimmed[0] == '#') {
      continue;
    }

    const std::size_t separator = trimmed.find('=');
    if (separator == std::string::npos) {
      continue;
    }

    std::string key = trimCopy(trimmed.substr(0, separator));
    std::string value = trimCopy(trimmed.substr(separator + 1));
    if (!value.empty() && value.front() == '\'' && value.back() == '\'') {
      value = value.substr(1, value.size() - 2);
    }
    if (!value.empty() && value.front() == '"' && value.back() == '"') {
      value = value.substr(1, value.size() - 2);
    }

    values[std::move(key)] = std::move(value);
  }

  return values;
}

} // namespace

class CesiumRuntime::TaskProcessor : public CesiumAsync::ITaskProcessor {
public:
  void startTask(std::function<void()> f) override {
    std::thread(std::move(f)).detach();
  }
};

CesiumRuntime::CesiumRuntime()
    : _taskProcessor(std::make_shared<TaskProcessor>()),
      _assetAccessor(std::make_shared<CesiumCurl::CurlAssetAccessor>()),
      _prepareRendererResources(
          std::make_shared<NullPrepareRendererResources>()),
      _creditSystem(std::make_shared<CesiumUtility::CreditSystem>()),
      _status(
          "Cesium Native listo. Configura cesium.conf o define "
          "CESIUM_ION_ACCESS_TOKEN y CESIUM_ION_ASSET_ID.") {}

CesiumRuntime::~CesiumRuntime() = default;

bool CesiumRuntime::startFromEnvironment() {
  const std::unordered_map<std::string, std::string> config =
      readConfigFile("cesium.conf");

  std::string token;
  if (const auto configToken = config.find("ion_access_token");
      configToken != config.end()) {
    token = configToken->second;
  } else if (const char* envToken = std::getenv("CESIUM_ION_ACCESS_TOKEN")) {
    token = envToken;
  }

  std::string assetIdValue;
  if (const auto configAssetId = config.find("ion_asset_id");
      configAssetId != config.end()) {
    assetIdValue = configAssetId->second;
  } else if (
      const char* envAssetId = std::getenv("CESIUM_ION_ASSET_ID")) {
    assetIdValue = envAssetId;
  }

  if (token.empty()) {
    this->setStatus(
        "Falta el token de ion. Ponlo en cesium.conf con "
        "ion_access_token=... o usa CESIUM_ION_ACCESS_TOKEN.");
    return false;
  }

  if (assetIdValue.empty()) {
    this->setStatus(
        "Falta el asset id de ion. Anade ion_asset_id=... en cesium.conf o "
        "usa CESIUM_ION_ASSET_ID.");
    return false;
  }

  char* end = nullptr;
  const long long parsedAssetId = std::strtoll(assetIdValue.c_str(), &end, 10);
  if (!end || *end != '\0') {
    this->setStatus(
        "El asset id de ion no es numerico. Usa un entero, por ejemplo 1.");
    return false;
  }

  this->setStatus("Creando Tileset de Cesium ion...");

  this->_tileset = this->createIonTileset(parsedAssetId, token);
  this->_mainThreadScope = this->_tileset->getAsyncSystem().enterMainThread();

  this->_tileset->getRootTileAvailableEvent().thenInMainThread([this, parsedAssetId]() {
    std::ostringstream stream;
    stream << "Tileset de ion creado. Asset " << parsedAssetId
           << " con root tile disponible. Siguiente paso: updateView() y "
              "renderer OpenGL.";
    this->setStatus(stream.str());
  });

  return true;
}

void CesiumRuntime::tick() {
  if (!this->_tileset) {
    return;
  }

  this->_tileset->getAsyncSystem().dispatchMainThreadTasks();
  this->_assetAccessor->tick();
}

std::string CesiumRuntime::statusText() const {
  std::lock_guard<std::mutex> lock(this->_statusMutex);
  return this->_status;
}

Cesium3DTilesSelection::TilesetExternals CesiumRuntime::createExternals() {
  return Cesium3DTilesSelection::TilesetExternals{
      this->_assetAccessor,
      this->_prepareRendererResources,
      CesiumAsync::AsyncSystem(this->_taskProcessor),
      this->_creditSystem};
}

std::unique_ptr<Cesium3DTilesSelection::Tileset> CesiumRuntime::createIonTileset(
    int64_t assetId,
    const std::string& accessToken) {
  Cesium3DTilesSelection::TilesetOptions options;
  options.maximumScreenSpaceError = 16.0;
  options.enableFrustumCulling = true;
  options.loadErrorCallback = [this](const Cesium3DTilesSelection::TilesetLoadFailureDetails& details) {
    std::ostringstream stream;
    stream << "Error cargando tileset";
    if (details.statusCode != 200) {
      stream << " (HTTP " << details.statusCode << ")";
    }
    if (!details.message.empty()) {
      stream << ": " << details.message;
    }
    this->setStatus(stream.str());
  };

  return std::make_unique<Cesium3DTilesSelection::Tileset>(
      this->createExternals(),
      assetId,
      accessToken,
      options);
}

void CesiumRuntime::setStatus(std::string status) {
  std::lock_guard<std::mutex> lock(this->_statusMutex);
  this->_status = std::move(status);
}
