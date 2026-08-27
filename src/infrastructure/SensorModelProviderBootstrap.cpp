#include "infrastructure/SensorModelProviderBootstrap.h"

#include "application/ScenarioState.h"
#include "infrastructure/sensors/SharedLibrarySensorModel.h"
#include "infrastructure/sensors/StoneSoupSensorModel.h"

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QSet>

#include <algorithm>
#include <memory>

namespace infrastructure {
namespace {

QString sourceRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
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
