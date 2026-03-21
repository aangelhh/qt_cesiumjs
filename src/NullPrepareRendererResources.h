#pragma once

#include <Cesium3DTilesSelection/IPrepareRendererResources.h>

class NullPrepareRendererResources
    : public Cesium3DTilesSelection::IPrepareRendererResources {
public:
  CesiumAsync::Future<Cesium3DTilesSelection::TileLoadResultAndRenderResources>
  prepareInLoadThread(
      const CesiumAsync::AsyncSystem& asyncSystem,
      Cesium3DTilesSelection::TileLoadResult&& tileLoadResult,
      const glm::dmat4&,
      const std::any&) override {
    return asyncSystem.createResolvedFuture(
        Cesium3DTilesSelection::TileLoadResultAndRenderResources{
            std::move(tileLoadResult),
            nullptr});
  }

  void* prepareInMainThread(Cesium3DTilesSelection::Tile&, void*) override {
    return nullptr;
  }

  void free(Cesium3DTilesSelection::Tile&, void*, void*) noexcept override {}

  void* prepareRasterInLoadThread(CesiumGltf::ImageAsset&, const std::any&)
      override {
    return nullptr;
  }

  void* prepareRasterInMainThread(
      CesiumRasterOverlays::RasterOverlayTile&,
      void*) override {
    return nullptr;
  }

  void freeRaster(
      const CesiumRasterOverlays::RasterOverlayTile&,
      void*,
      void*) noexcept override {}

  void attachRasterInMainThread(
      const Cesium3DTilesSelection::Tile&,
      int32_t,
      const CesiumRasterOverlays::RasterOverlayTile&,
      void*,
      const glm::dvec2&,
      const glm::dvec2&) override {}

  void detachRasterInMainThread(
      const Cesium3DTilesSelection::Tile&,
      int32_t,
      const CesiumRasterOverlays::RasterOverlayTile&,
      void*) noexcept override {}
};
