#include "infrastructure/SensorModelProviderBootstrap.h"

#include "application/ScenarioState.h"
#include "infrastructure/sensors/SharedLibrarySensorModel.h"
#include "infrastructure/sensors/StoneSoupSensorModel.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QSet>

#include <algorithm>
#include <memory>

namespace infrastructure {
namespace {

QString sourceRoot() {
  QDir cursor(QCoreApplication::applicationDirPath());
  for (int depth = 0; depth < 6; ++depth) {
    if (QFileInfo::exists(cursor.absoluteFilePath(
            QStringLiteral("Data/sensor_model_providers.json")))) {
      return cursor.absolutePath();
    }
    if (!cursor.cdUp()) {
      break;
    }
  }

#ifdef QTTEST_SOURCE_DIR
  const QDir source(QString::fromUtf8(QTTEST_SOURCE_DIR));
  if (QFileInfo::exists(source.absoluteFilePath(
          QStringLiteral("Data/sensor_model_providers.json")))) {
    return source.absolutePath();
  }
#endif

  return QDir::currentPath();
}

QString absoluteProviderPath(const QString& configuredPath) {
  const QString trimmed = configuredPath.trimmed();
  if (trimmed.isEmpty() || QFileInfo(trimmed).isAbsolute()) {
    return trimmed;
  }
  return QDir(sourceRoot()).absoluteFilePath(trimmed);
}

QString sharedLibraryPath(const SensorModelProviderEntry& provider) {
  QString configuredPath = provider.libraryPath;
  if (configuredPath.trimmed().isEmpty() &&
      provider.id == QStringLiteral("mixr")) {
    configuredPath = QProcessEnvironment::systemEnvironment().value(
        QStringLiteral("QTTEST_MIXR_SENSOR_PLUGIN"));
  }
  if (configuredPath.trimmed().isEmpty() && provider.id == QStringLiteral("mixr")) {
#ifdef Q_OS_WIN
    const QString name = QStringLiteral("qttest_mixr_sensor.dll");
#elif defined(Q_OS_MACOS)
    const QString name = QStringLiteral("libqttest_mixr_sensor.dylib");
#else
    const QString name = QStringLiteral("libqttest_mixr_sensor.so");
#endif
    const QString packaged = QDir(QCoreApplication::applicationDirPath())
        .filePath(QStringLiteral("sensor-plugins/") + name);
    if (QFileInfo::exists(packaged)) return packaged;
#ifdef QTTEST_BINARY_DIR
    const QString development = QDir(QString::fromUtf8(QTTEST_BINARY_DIR))
        .filePath(QStringLiteral("sensor-plugins/") + name);
    if (QFileInfo::exists(development)) return development;
#endif
    return packaged;
  }
  return absoluteProviderPath(configuredPath);
}

QStringList pythonCandidates(const SensorModelProviderEntry& provider) {
  QStringList candidates;
  candidates.push_back(
      provider.options.value(QStringLiteral("pythonExecutable")).toString());
  candidates.push_back(QProcessEnvironment::systemEnvironment().value(
      QStringLiteral("QTTEST_STONESOUP_PYTHON")));
#ifdef QTTEST_BINARY_DIR
  candidates.push_back(
      QDir(QString::fromUtf8(QTTEST_BINARY_DIR)).absoluteFilePath(
          QStringLiteral("stonesoup-venv/bin/python")));
#endif
  candidates.push_back(QDir(sourceRoot()).absoluteFilePath(
      QStringLiteral("build-macos-debug/stonesoup-venv/bin/python")));
  candidates.push_back(QDir(sourceRoot()).absoluteFilePath(
      QStringLiteral("build/stonesoup-venv/bin/python")));
  candidates.push_back(QStringLiteral("python3"));

  QStringList unique;
  QSet<QString> seen;
  for (const QString& candidate : candidates) {
    const QString trimmed = candidate.trimmed();
    if (!trimmed.isEmpty() && !seen.contains(trimmed)) {
      unique.push_back(trimmed);
      seen.insert(trimmed);
    }
  }
  return unique;
}

QString stoneSoupServiceScript(const SensorModelProviderEntry& provider) {
  if (!provider.endpoint.trimmed().isEmpty()) {
    return absoluteProviderPath(provider.endpoint);
  }
  return QDir(sourceRoot()).absoluteFilePath(
      QStringLiteral("Tools/stonesoup_sensor_service.py"));
}

} // namespace

QStringList SensorModelProviderBootstrap::registerEnabledProviders(
    ScenarioState& scenarioState,
    const QVector<SensorModelProviderEntry>& providers) {
  QStringList diagnostics;
  for (const SensorModelProviderEntry& provider : providers) {
    if (!provider.enabled || provider.id == QStringLiteral("native")) {
      continue;
    }

    if (provider.adapterType == QStringLiteral("shared-library")) {
      auto model = std::make_shared<sensors::SharedLibrarySensorModel>(
          provider.id,
          sharedLibraryPath(provider));
      if (model->isAvailable() && scenarioState.registerSensorModel(model)) {
        diagnostics.push_back(QStringLiteral("Sensor provider %1 loaded from %2")
            .arg(provider.id, provider.libraryPath));
      } else {
        diagnostics.push_back(QStringLiteral("Sensor provider %1 unavailable: %2")
            .arg(provider.id, model->errorString()));
      }
      continue;
    }

    if (provider.adapterType == QStringLiteral("external-service") &&
        provider.id == QStringLiteral("stone-soup")) {
      QString lastError = QStringLiteral("No usable Python runtime found");
      bool registered = false;
      for (const QString& pythonExecutable : pythonCandidates(provider)) {
        sensors::StoneSoupSensorModel::Configuration configuration;
        configuration.pythonExecutable = pythonExecutable;
        configuration.serviceScript = stoneSoupServiceScript(provider);
        configuration.options = provider.options;
        configuration.requestTimeoutMilliseconds = std::max(
            25,
            provider.options.value(
                QStringLiteral("requestTimeoutMilliseconds"),
                100).toInt());
        auto model = std::make_shared<sensors::StoneSoupSensorModel>(
            std::move(configuration));
        if (!model->isAvailable()) {
          lastError = model->errorString();
          continue;
        }
        if (scenarioState.registerSensorModel(model)) {
          diagnostics.push_back(QStringLiteral(
              "Sensor provider stone-soup available via %1")
              .arg(pythonExecutable));
          registered = true;
          break;
        }
      }
      if (!registered) {
        diagnostics.push_back(QStringLiteral("Sensor provider stone-soup unavailable: %1")
            .arg(lastError));
      }
      continue;
    }

    diagnostics.push_back(QStringLiteral("Sensor provider %1 uses unsupported adapter type %2")
        .arg(provider.id, provider.adapterType));
  }
  return diagnostics;
}

} // namespace infrastructure
