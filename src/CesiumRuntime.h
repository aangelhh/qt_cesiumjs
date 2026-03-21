#pragma once

#include <Cesium3DTilesSelection/Tileset.h>
#include <Cesium3DTilesSelection/TilesetExternals.h>

#include <memory>
#include <mutex>
#include <optional>
#include <string>

class CesiumRuntime {
public:
  CesiumRuntime();
  ~CesiumRuntime();

  CesiumRuntime(const CesiumRuntime&) = delete;
  CesiumRuntime& operator=(const CesiumRuntime&) = delete;

  bool startFromEnvironment();
  void tick();
  std::string statusText() const;
  Cesium3DTilesSelection::TilesetExternals createExternals();

  std::unique_ptr<Cesium3DTilesSelection::Tileset> createIonTileset(
      int64_t assetId,
      const std::string& accessToken);

private:
  class TaskProcessor;

  void setStatus(std::string status);

  std::shared_ptr<TaskProcessor> _taskProcessor;
  std::shared_ptr<CesiumAsync::IAssetAccessor> _assetAccessor;
  std::shared_ptr<Cesium3DTilesSelection::IPrepareRendererResources>
      _prepareRendererResources;
  std::shared_ptr<CesiumUtility::CreditSystem> _creditSystem;
  mutable std::mutex _statusMutex;
  std::string _status;
  std::unique_ptr<Cesium3DTilesSelection::Tileset> _tileset;
  std::optional<CesiumAsync::AsyncSystem::MainThreadScope> _mainThreadScope;
};
