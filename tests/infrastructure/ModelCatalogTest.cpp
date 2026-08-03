#include <gtest/gtest.h>

#include "infrastructure/ModelCatalog.h"

namespace {

const ModelCatalogEntry* findModel(
    const QVector<ModelCatalogEntry>& catalog,
    const QString& name) {
  for (const ModelCatalogEntry& entry : catalog) {
    if (entry.name == name) {
      return &entry;
    }
  }
  return nullptr;
}

} // namespace

TEST(ModelCatalog, LoadsDeclarativeFighterDynamicsConfiguration) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* f16 = findModel(
      catalog,
      QStringLiteral("F-16 Fighting Falcon (generic)"));
  ASSERT_NE(f16, nullptr);
  EXPECT_EQ(f16->dynamicsBackend, QStringLiteral("jsbsim"));
  EXPECT_EQ(f16->jsbsimAircraftModel, QStringLiteral("f16"));
  EXPECT_EQ(f16->controlProfileId, QStringLiteral("fighter-generic"));
  EXPECT_EQ(f16->cesiumAxes, QStringLiteral("x-forward-y-up"));
  EXPECT_EQ(
      f16->systemsDisplayProfileId,
      QStringLiteral("air-turbine-1-engine"));
  EXPECT_EQ(f16->engineCount, 1);
  EXPECT_DOUBLE_EQ(f16->fuelCapacityKilograms, 5875.0);
  EXPECT_DOUBLE_EQ(f16->initialFuelKilograms, 3200.0);
}

TEST(ModelCatalog, AddsVerifiedOrientationWithoutDynamicsMetadata) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* legacy = findModel(
      catalog,
      QStringLiteral("A-10 Thunderbolt II (base)"));
  ASSERT_NE(legacy, nullptr);
  EXPECT_TRUE(legacy->dynamicsBackend.isEmpty());
  EXPECT_TRUE(legacy->jsbsimAircraftModel.isEmpty());
  EXPECT_TRUE(legacy->controlProfileId.isEmpty());
  EXPECT_EQ(legacy->cesiumAxes, QStringLiteral("z-forward-y-up"));
}

TEST(ModelCatalog, AppliesAssetOrientationToEveryAvailableAirVariant) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  int airModelCount = 0;
  for (const ModelCatalogEntry& entry : catalog) {
    if (entry.domain != QStringLiteral("Air")) {
      continue;
    }
    ++airModelCount;
    EXPECT_FALSE(entry.cesiumAxes.isEmpty())
        << entry.name.toStdString() << " uses "
        << entry.relativePath.toStdString();
  }
  EXPECT_GT(airModelCount, 0);
}

TEST(ModelCatalog, LoadsMirageCesiumAxesConfiguration) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* mirage = findModel(
      catalog,
      QStringLiteral("Mirage 2000 (Generic)"));

  ASSERT_NE(mirage, nullptr);
  EXPECT_EQ(mirage->cesiumAxes, QStringLiteral("x-forward-y-up"));
  EXPECT_TRUE(mirage->absolutePath.endsWith(
      QStringLiteral("/dassault_mirage_2000.glb")));
}
