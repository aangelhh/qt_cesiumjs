#include <gtest/gtest.h>

#include "infrastructure/sensors/SharedLibrarySensorModel.h"
#include "infrastructure/sensors/StoneSoupSensorModel.h"
#include "infrastructure/SensorModelProviderBootstrap.h"
#include "infrastructure/SensorModelProviderCatalog.h"
#include "application/ScenarioState.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <algorithm>
#include <cmath>
#include <memory>

namespace {

Entity makeEntity(const QString& id, double longitude) {
  Entity entity;
  entity.entityId = id;
  entity.name = id;
  entity.domain = QStringLiteral("Air");
  entity.latitude = 40.0;
  entity.longitude = longitude;
  entity.altitude = 5000;
  entity.radarSignature = 1.0;
  return entity;
}

QString stoneSoupPython() {
#ifdef Q_OS_WIN
  return QDir(QString::fromUtf8(QTTEST_BINARY_DIR)).absoluteFilePath(
      QStringLiteral("stonesoup-venv/Scripts/python.exe"));
#else
  return QDir(QString::fromUtf8(QTTEST_BINARY_DIR)).absoluteFilePath(
      QStringLiteral("stonesoup-venv/bin/python"));
#endif
}

QString mixrPluginPath() {
#ifdef QTTEST_MIXR_PLUGIN_PATH
  return QString::fromUtf8(QTTEST_MIXR_PLUGIN_PATH);
#else
  return {};
#endif
}

} // namespace

TEST(SensorProviderAdapter, InvalidSharedLibraryRemainsUnavailable) {
  infrastructure::sensors::SharedLibrarySensorModel model(
      QStringLiteral("mixr"),
      QStringLiteral("/path/that/does/not/exist/libmixr_sensor"));

  EXPECT_FALSE(model.isAvailable());
  EXPECT_FALSE(model.errorString().isEmpty());
}

TEST(SensorProviderAdapter, MixrPluginUsesRfRangeLossWhenBuilt) {
  const QString pluginPath = mixrPluginPath();
  if (!QFileInfo::exists(pluginPath)) {
    GTEST_SKIP() << "Run the qttest_setup_mixr target first";
  }

  infrastructure::sensors::SharedLibrarySensorModel model(
      QStringLiteral("mixr"), pluginPath);
  ASSERT_TRUE(model.isAvailable()) << model.errorString().toStdString();

  const Entity observer = makeEntity(QStringLiteral("observer"), 0.0);
  const Entity target = makeEntity(QStringLiteral("target"), 0.2);
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-primary");
  sensor.modelProviderId = QStringLiteral("mixr");
  sensor.probabilityOfDetection = 1.0;
  sensor.maxRangeMeters = 100000.0;

  const auto thresholdResult = model.evaluate(
      {42U, 5.0, 1, 100000.0, observer, sensor, target});
  const auto nearResult = model.evaluate(
      {42U, 5.0, 2, 50000.0, observer, sensor, target});
  SensorDefinition strongerSensor = sensor;
  strongerSensor.radarProfile.peakPowerWatts *= 4.0;
  const auto strongerResult = model.evaluate(
      {42U, 5.0, 3, 100000.0, observer, strongerSensor, target});

  EXPECT_EQ(thresholdResult.effectiveModelId, QStringLiteral("mixr"));
  EXPECT_NEAR(thresholdResult.probability, 0.5, 1e-6);
  EXPECT_GT(nearResult.probability, thresholdResult.probability);
  EXPECT_LE(nearResult.probability, 1.0);
  EXPECT_FALSE(nearResult.providerVersion.isEmpty());
  EXPECT_TRUE(std::isfinite(nearResult.rangeLossDecibels));
  EXPECT_TRUE(std::isfinite(nearResult.echoRatio));
  EXPECT_TRUE(std::isfinite(nearResult.signalToNoiseRatio));
  EXPECT_TRUE(std::isfinite(nearResult.signalToNoiseRatioDecibels));
  EXPECT_GT(nearResult.receivedPowerWatts, 0.0);
  EXPECT_GT(nearResult.noisePowerWatts, 0.0);
  EXPECT_GT(nearResult.echoRatio, thresholdResult.echoRatio);
  EXPECT_GT(strongerResult.probability, thresholdResult.probability);
}

TEST(SensorProviderAdapter, BootstrapRegistersBuiltMixrProvider) {
  const QString pluginPath = mixrPluginPath();
  if (!QFileInfo::exists(pluginPath)) {
    GTEST_SKIP() << "Run the qttest_setup_mixr target first";
  }

  const QByteArray previousPath = qgetenv("QTTEST_MIXR_SENSOR_PLUGIN");
  qputenv("QTTEST_MIXR_SENSOR_PLUGIN", pluginPath.toUtf8());

  ScenarioState state;
  const auto providers =
      infrastructure::SensorModelProviderCatalog::loadProviders();
  const QStringList diagnostics =
      infrastructure::SensorModelProviderBootstrap::registerEnabledProviders(
          state,
          providers);

  if (previousPath.isNull()) {
    qunsetenv("QTTEST_MIXR_SENSOR_PLUGIN");
  } else {
    qputenv("QTTEST_MIXR_SENSOR_PLUGIN", previousPath);
  }

  EXPECT_TRUE(state.sensorModelIds().contains(QStringLiteral("mixr")));
  EXPECT_TRUE(std::any_of(
      diagnostics.cbegin(),
      diagnostics.cend(),
      [](const QString& diagnostic) {
        return diagnostic.contains(QStringLiteral("mixr loaded"));
      }));
}

TEST(SensorProviderAdapter, StoneSoupUsesAesaProbabilityWhenInstalled) {
  int argc = 1;
  char applicationName[] = "sensor-provider-test";
  char* argv[] = {applicationName, nullptr};
  std::unique_ptr<QCoreApplication> application;
  if (!QCoreApplication::instance()) {
    application = std::make_unique<QCoreApplication>(argc, argv);
  }

  const QString python = stoneSoupPython();
  if (!QFileInfo::exists(python)) {
    GTEST_SKIP() << "Run the qttest_setup_stonesoup target first";
  }

  infrastructure::sensors::StoneSoupSensorModel::Configuration configuration;
  configuration.pythonExecutable = python;
  configuration.serviceScript = QDir(QString::fromUtf8(QTTEST_SOURCE_DIR))
      .absoluteFilePath(QStringLiteral("Tools/stonesoup_sensor_service.py"));
  configuration.requestTimeoutMilliseconds = 250;
  infrastructure::sensors::StoneSoupSensorModel model(std::move(configuration));
  ASSERT_TRUE(model.isAvailable()) << model.errorString().toStdString();

  const Entity observer = makeEntity(QStringLiteral("observer"), 0.0);
  const Entity target = makeEntity(QStringLiteral("target"), 0.2);
  SensorDefinition sensor;
  sensor.id = QStringLiteral("radar-primary");
  sensor.modelProviderId = QStringLiteral("stone-soup");
  sensor.probabilityOfDetection = 1.0;
  const auto result = model.evaluate(
      {42U, 5.0, 1, 25000.0, observer, sensor, target});
  SensorDefinition strongerSensor = sensor;
  strongerSensor.radarProfile.peakPowerWatts *= 4.0;
  const auto strongerResult = model.evaluate(
      {42U, 5.0, 2, 25000.0, observer, strongerSensor, target});

  EXPECT_EQ(result.effectiveModelId, QStringLiteral("stone-soup"));
  EXPECT_GT(result.probability, 0.0);
  EXPECT_LT(result.probability, 1.0);
  EXPECT_GE(result.sample, 0.0);
  EXPECT_LT(result.sample, 1.0);
  EXPECT_FALSE(result.providerVersion.isEmpty());
  EXPECT_TRUE(std::isfinite(result.signalToNoiseRatio));
  EXPECT_TRUE(std::isfinite(result.signalToNoiseRatioDecibels));
  EXPECT_GT(result.receivedPowerWatts, 0.0);
  EXPECT_GT(result.noisePowerWatts, 0.0);
  EXPECT_GT(strongerResult.signalToNoiseRatio, result.signalToNoiseRatio);
}

TEST(SensorProviderAdapter, BootstrapRegistersInstalledStoneSoupProvider) {
  const QString python = stoneSoupPython();
  if (!QFileInfo::exists(python)) {
    GTEST_SKIP() << "Run the qttest_setup_stonesoup target first";
  }

  int argc = 1;
  char applicationName[] = "sensor-bootstrap-test";
  char* argv[] = {applicationName, nullptr};
  std::unique_ptr<QCoreApplication> application;
  if (!QCoreApplication::instance()) {
    application = std::make_unique<QCoreApplication>(argc, argv);
  }

  ScenarioState state;
  const auto providers =
      infrastructure::SensorModelProviderCatalog::loadProviders();
  const QStringList diagnostics =
      infrastructure::SensorModelProviderBootstrap::registerEnabledProviders(
          state,
          providers);

  EXPECT_TRUE(state.sensorModelIds().contains(QStringLiteral("stone-soup")));
  EXPECT_FALSE(state.sensorModelIds().contains(QStringLiteral("mixr")));
  EXPECT_TRUE(std::any_of(
      diagnostics.cbegin(),
      diagnostics.cend(),
      [](const QString& diagnostic) {
        return diagnostic.contains(QStringLiteral("stone-soup available"));
      }));
}
