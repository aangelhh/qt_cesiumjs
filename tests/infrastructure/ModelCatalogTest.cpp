#include <gtest/gtest.h>

#include "infrastructure/JsbsimAircraftCatalog.h"
#include "infrastructure/ModelCatalog.h"

#include <QDir>

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
  EXPECT_EQ(
      f16->dynamicsModelCompatibility,
      QStringLiteral("family"));
  EXPECT_EQ(f16->controlProfileId, QStringLiteral("fighter-generic"));
  EXPECT_EQ(f16->cesiumAxes, QStringLiteral("x-forward-y-up"));
  EXPECT_EQ(
      f16->systemsDisplayProfileId,
      QStringLiteral("air-turbine-1-engine"));
  EXPECT_EQ(f16->engineCount, 1);
  EXPECT_DOUBLE_EQ(f16->fuelCapacityKilograms, 5875.0);
  EXPECT_DOUBLE_EQ(f16->initialFuelKilograms, 3200.0);
}

TEST(ModelCatalog, PropagatesDynamicsBindingAcrossSharedVisualAsset) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* f16c = findModel(catalog, QStringLiteral("F-16C"));
  const ModelCatalogEntry* f22a = findModel(catalog, QStringLiteral("F-22A"));
  ASSERT_NE(f16c, nullptr);
  ASSERT_NE(f22a, nullptr);
  EXPECT_EQ(f16c->jsbsimAircraftModel, QStringLiteral("f16"));
  EXPECT_EQ(f22a->jsbsimAircraftModel, QStringLiteral("f22"));
  EXPECT_EQ(
      f16c->dynamicsModelCompatibility,
      QStringLiteral("family"));
}

TEST(ModelCatalog, MapsDistinctF16AndC130AssetsExplicitly) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* f16a = findModel(catalog, QStringLiteral("F-16A"));
  const ModelCatalogEntry* c130 = findModel(
      catalog,
      QStringLiteral("Lockheed C-130 Hercules"));
  ASSERT_NE(f16a, nullptr);
  ASSERT_NE(c130, nullptr);
  EXPECT_EQ(f16a->jsbsimAircraftModel, QStringLiteral("f16"));
  EXPECT_EQ(c130->jsbsimAircraftModel, QStringLiteral("C130"));
  EXPECT_EQ(c130->engineCount, 4);
}

TEST(ModelCatalog, EveryJsbsimBindingReferencesAnAvailableConfiguration) {
  const QVector<ModelCatalogEntry> models = ModelCatalog::loadModels();
#ifdef QTTEST_SOURCE_DIR
  const QString aircraftRoot = QDir(QString::fromUtf8(QTTEST_SOURCE_DIR))
      .absoluteFilePath(QStringLiteral("Dependencies/jsbsim/aircraft"));
  const QStringList availableModels =
      JsbsimAircraftCatalog::discover(aircraftRoot).modelIds();

  int mappedModels = 0;
  for (const ModelCatalogEntry& model : models) {
    if (model.dynamicsBackend != QStringLiteral("jsbsim")) {
      continue;
    }
    ++mappedModels;
    EXPECT_TRUE(availableModels.contains(model.jsbsimAircraftModel))
        << model.name.toStdString() << " references "
        << model.jsbsimAircraftModel.toStdString();
    EXPECT_FALSE(model.dynamicsModelCompatibility.isEmpty())
        << model.name.toStdString();
  }
  EXPECT_GE(mappedModels, 16);
#endif
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
