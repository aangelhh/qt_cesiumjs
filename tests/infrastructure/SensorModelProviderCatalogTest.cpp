#include <gtest/gtest.h>

#include "infrastructure/SensorModelProviderCatalog.h"

#include <QFile>
#include <QTemporaryFile>

#include <algorithm>

namespace {

QString writeCatalog(const QByteArray& json) {
  QTemporaryFile file;
  file.setAutoRemove(false);
  if (!file.open()) {
    return {};
  }
  file.write(json);
  const QString path = file.fileName();
  file.close();
  return path;
}

const infrastructure::SensorModelProviderEntry* findProvider(
    const QVector<infrastructure::SensorModelProviderEntry>& providers,
    const QString& id) {
  for (const auto& provider : providers) {
    if (provider.id == id) {
      return &provider;
    }
  }
  return nullptr;
}

} // namespace

TEST(SensorModelProviderCatalog, MissingFileStillProvidesNativeFallback) {
  const auto providers =
      infrastructure::SensorModelProviderCatalog::loadProviders(
          QStringLiteral("/path/that/does/not/exist.json"));

  ASSERT_EQ(providers.size(), 1);
  EXPECT_EQ(providers.front().id, QStringLiteral("native"));
  EXPECT_TRUE(providers.front().enabled);
  EXPECT_TRUE(providers.front().available);
  EXPECT_TRUE(providers.front().defaultForNewSensors);
}

TEST(SensorModelProviderCatalog, LoadsSharedLibraryAndExternalServiceMetadata) {
  const QString path = writeCatalog(R"json({
    "providers": [
      {
        "id": "MIXR",
        "displayName": "MIXR Radar",
        "adapterType": "shared-library",
        "enabled": true,
        "defaultForNewSensors": true,
        "libraryPath": "/opt/models/libmixr_sensor.dylib",
        "options": {"profile": "airborne"}
      },
      {
        "id": "stone-soup",
        "displayName": "Stone Soup",
        "adapterType": "external-service",
        "enabled": true,
        "endpoint": "ros2://sensor_tracks"
      }
    ]
  })json");
  ASSERT_FALSE(path.isEmpty());

  const auto providers =
      infrastructure::SensorModelProviderCatalog::loadProviders(path);
  QFile::remove(path);

  ASSERT_EQ(providers.size(), 3);
  const auto* mixr = findProvider(providers, QStringLiteral("mixr"));
  ASSERT_NE(mixr, nullptr);
  EXPECT_EQ(mixr->adapterType, QStringLiteral("shared-library"));
  EXPECT_EQ(mixr->libraryPath, QStringLiteral("/opt/models/libmixr_sensor.dylib"));
  EXPECT_EQ(mixr->options.value(QStringLiteral("profile")).toString(), QStringLiteral("airborne"));
  EXPECT_FALSE(mixr->available);
  EXPECT_TRUE(mixr->defaultForNewSensors);

  const auto* native = findProvider(providers, QStringLiteral("native"));
  ASSERT_NE(native, nullptr);
  EXPECT_FALSE(native->defaultForNewSensors);
  EXPECT_EQ(std::count_if(providers.cbegin(),
                          providers.cend(),
                          [](const auto& provider) {
                            return provider.defaultForNewSensors;
                          }),
            1);

  const auto* stoneSoup = findProvider(providers, QStringLiteral("stone-soup"));
  ASSERT_NE(stoneSoup, nullptr);
  EXPECT_EQ(stoneSoup->endpoint, QStringLiteral("ros2://sensor_tracks"));
}

TEST(SensorModelProviderCatalog, InvalidJsonFallsBackToNative) {
  const QString path = writeCatalog(QByteArrayLiteral("not-json"));
  ASSERT_FALSE(path.isEmpty());

  const auto providers =
      infrastructure::SensorModelProviderCatalog::loadProviders(path);
  QFile::remove(path);

  ASSERT_EQ(providers.size(), 1);
  EXPECT_EQ(providers.front().id, QStringLiteral("native"));
}

TEST(SensorModelProviderCatalog, RepositoryConfigurationEnablesExternalAdapters) {
  const auto providers =
      infrastructure::SensorModelProviderCatalog::loadProviders();

  const auto* native = findProvider(providers, QStringLiteral("native"));
  const auto* mixr = findProvider(providers, QStringLiteral("mixr"));
  const auto* stoneSoup = findProvider(providers, QStringLiteral("stone-soup"));
  ASSERT_NE(native, nullptr);
  ASSERT_NE(mixr, nullptr);
  ASSERT_NE(stoneSoup, nullptr);
  EXPECT_TRUE(native->enabled);
  EXPECT_TRUE(native->defaultForNewSensors);
  EXPECT_TRUE(mixr->enabled);
  EXPECT_TRUE(stoneSoup->enabled);
  EXPECT_EQ(mixr->adapterType, QStringLiteral("shared-library"));
  EXPECT_EQ(stoneSoup->adapterType, QStringLiteral("external-service"));
  EXPECT_EQ(
      stoneSoup->endpoint,
      QStringLiteral("Tools/stonesoup_sensor_service.py"));
}
