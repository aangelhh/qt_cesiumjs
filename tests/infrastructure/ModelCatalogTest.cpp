#include <gtest/gtest.h>

#include "infrastructure/JsbsimAircraftCatalog.h"
#include "infrastructure/JsbsimModelRepository.h"
#include "infrastructure/ModelCatalog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

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

TEST(ModelCatalog, AppliesGroupDynamicsDefaultsToModernAircraftFamilies) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* typhoon = findModel(
      catalog, QStringLiteral("Generic Typhoon"));
  const ModelCatalogEntry* f2000 = findModel(
      catalog, QStringLiteral("F-2000A Italian"));
  const ModelCatalogEntry* rafale = findModel(
      catalog, QStringLiteral("Rafale C"));
  const ModelCatalogEntry* mirage = findModel(
      catalog, QStringLiteral("Mirage 2000-5"));
  const ModelCatalogEntry* b52 = findModel(catalog, QStringLiteral("B-52H"));
  const ModelCatalogEntry* chinook = findModel(
      catalog, QStringLiteral("CH-47 Chinook"));

  ASSERT_NE(typhoon, nullptr);
  ASSERT_NE(f2000, nullptr);
  ASSERT_NE(rafale, nullptr);
  ASSERT_NE(mirage, nullptr);
  ASSERT_NE(b52, nullptr);
  ASSERT_NE(chinook, nullptr);
  EXPECT_EQ(
      typhoon->jsbsimAircraftModel,
      QStringLiteral("eurofighter-typhoon"));
  EXPECT_EQ(
      f2000->jsbsimAircraftModel,
      QStringLiteral("eurofighter-typhoon"));
  EXPECT_EQ(
      rafale->jsbsimAircraftModel,
      QStringLiteral("rafale-open-data"));
  EXPECT_EQ(
      mirage->jsbsimAircraftModel,
      QStringLiteral("mirage2000-open-data"));
  EXPECT_EQ(b52->jsbsimAircraftModel, QStringLiteral("B747"));
  EXPECT_EQ(chinook->jsbsimAircraftModel, QStringLiteral("ah1s"));
  EXPECT_EQ(typhoon->dynamicsModelCompatibility,
            QStringLiteral("experimental"));
  EXPECT_DOUBLE_EQ(typhoon->fuelCapacityKilograms, 4996.0);
  EXPECT_EQ(rafale->dynamicsModelCompatibility,
            QStringLiteral("experimental"));
  EXPECT_EQ(rafale->engineCount, 2);
  EXPECT_DOUBLE_EQ(rafale->fuelCapacityKilograms, 4700.0);
  EXPECT_EQ(mirage->dynamicsModelCompatibility,
            QStringLiteral("experimental"));
  EXPECT_DOUBLE_EQ(mirage->fuelCapacityKilograms, 3209.2);
  EXPECT_EQ(b52->engineCount, 8);
}

TEST(ModelCatalog, EveryJsbsimBindingReferencesAnAvailableConfiguration) {
  const QVector<ModelCatalogEntry> models = ModelCatalog::loadModels();
#ifdef QTTEST_SOURCE_DIR
  const QStringList availableModels = JsbsimAircraftCatalog::discoverModelRoots(
      JsbsimModelRepository::modelRootsForProject(
          QString::fromUtf8(QTTEST_SOURCE_DIR)))
      .modelIds();

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
  EXPECT_GE(mappedModels, 70);
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

TEST(ModelCatalog, ResolvesMissingVariantAssetFromDeclaredFamilyFallback) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

  const ModelCatalogEntry* genericTyphoon = findModel(
      catalog,
      QStringLiteral("Generic Typhoon"));
  const ModelCatalogEntry* typhoonF2 = findModel(
      catalog,
      QStringLiteral("Typhoon F2"));

  ASSERT_NE(genericTyphoon, nullptr);
  ASSERT_NE(typhoonF2, nullptr);
  EXPECT_EQ(genericTyphoon->visualModelCompatibility,
            QStringLiteral("exact"));
  EXPECT_EQ(typhoonF2->visualModelCompatibility,
            QStringLiteral("family"));
  EXPECT_EQ(typhoonF2->configuredRelativePath,
            QStringLiteral("/models/Air/Fighter/typhoon_f2.glb"));
  EXPECT_EQ(typhoonF2->relativePath,
            QStringLiteral("/models/Air/Fighter/eurofighter_typhoon.glb"));
  EXPECT_EQ(typhoonF2->absolutePath, genericTyphoon->absolutePath);
}

TEST(ModelCatalog, EveryConfiguredModelResolvesToAnAvailableVisualAsset) {
  const QVector<ModelCatalogEntry> catalog = ModelCatalog::loadModels();

#ifdef QTTEST_SOURCE_DIR
  QFile config(QDir(QString::fromUtf8(QTTEST_SOURCE_DIR))
                   .filePath(QStringLiteral("Data/config3DModel.yaml")));
  ASSERT_TRUE(config.open(QIODevice::ReadOnly | QIODevice::Text));

  int configuredModelCount = 0;
  QTextStream input(&config);
  while (!input.atEnd()) {
    if (input.readLine().trimmed().startsWith(QStringLiteral("- name:"))) {
      ++configuredModelCount;
    }
  }

  ASSERT_EQ(catalog.size(), configuredModelCount);
  for (const ModelCatalogEntry& entry : catalog) {
    EXPECT_FALSE(entry.configuredRelativePath.isEmpty())
        << entry.name.toStdString();
    EXPECT_TRUE(QFileInfo::exists(entry.absolutePath))
        << entry.name.toStdString() << " resolves to "
        << entry.absolutePath.toStdString();
    EXPECT_TRUE(entry.visualModelCompatibility == QStringLiteral("exact") ||
                entry.visualModelCompatibility == QStringLiteral("family"))
        << entry.name.toStdString();
  }
#endif
}
