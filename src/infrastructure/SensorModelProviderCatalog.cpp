#include "infrastructure/SensorModelProviderCatalog.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

#include <algorithm>

namespace infrastructure {
namespace {

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

SensorModelProviderEntry nativeProvider() {
  SensorModelProviderEntry provider;
  provider.id = QStringLiteral("native");
  provider.displayName = QStringLiteral("Native deterministic");
  provider.adapterType = QStringLiteral("native");
  provider.enabled = true;
  provider.available = true;
  provider.defaultForNewSensors = true;
  return provider;
}

} // namespace

QVector<SensorModelProviderEntry> SensorModelProviderCatalog::loadProviders(
    const QString& filePath) {
  QVector<SensorModelProviderEntry> providers;
  QFile file(filePath.trimmed().isEmpty() ? defaultConfigPath() : filePath);
  if (file.open(QIODevice::ReadOnly)) {
    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error == QJsonParseError::NoError && document.isObject()) {
      QSet<QString> providerIds;
      const QJsonArray configuredProviders =
          document.object().value(QStringLiteral("providers")).toArray();
      for (const QJsonValue& value : configuredProviders) {
        const QJsonObject object = value.toObject();
        SensorModelProviderEntry provider;
        provider.id = object.value(QStringLiteral("id"))
            .toString().trimmed().toLower();
        if (provider.id.isEmpty() || providerIds.contains(provider.id)) {
          continue;
        }
        provider.displayName = object.value(QStringLiteral("displayName"))
            .toString(provider.id).trimmed();
        provider.adapterType = object.value(QStringLiteral("adapterType"))
            .toString(QStringLiteral("external-service")).trimmed().toLower();
        provider.libraryPath = object.value(QStringLiteral("libraryPath"))
            .toString().trimmed();
        provider.endpoint = object.value(QStringLiteral("endpoint"))
            .toString().trimmed();
        provider.options = object.value(QStringLiteral("options"))
            .toObject().toVariantMap();
        provider.enabled = object.value(QStringLiteral("enabled")).toBool(true);
        provider.available = provider.id == QStringLiteral("native");
        provider.defaultForNewSensors =
            object.value(QStringLiteral("defaultForNewSensors")).toBool(false);
        providers.push_back(provider);
        providerIds.insert(provider.id);
      }
    }
  }

  auto nativeIt = std::find_if(
      providers.begin(),
      providers.end(),
      [](const SensorModelProviderEntry& provider) {
        return provider.id == QStringLiteral("native");
      });
  if (nativeIt == providers.end()) {
    providers.prepend(nativeProvider());
  } else {
    nativeIt->enabled = true;
    nativeIt->available = true;
    if (nativeIt->displayName.isEmpty()) {
      nativeIt->displayName = QStringLiteral("Native deterministic");
    }
  }
  int defaultIndex = -1;
  for (int index = 0; index < providers.size(); ++index) {
    const SensorModelProviderEntry& provider = providers.at(index);
    if (provider.enabled && provider.defaultForNewSensors &&
        provider.id != QStringLiteral("native")) {
      defaultIndex = index;
      break;
    }
  }
  if (defaultIndex < 0) {
    for (int index = 0; index < providers.size(); ++index) {
      const SensorModelProviderEntry& provider = providers.at(index);
      if (provider.enabled && provider.defaultForNewSensors) {
        defaultIndex = index;
        break;
      }
    }
  }
  if (defaultIndex < 0) {
    for (int index = 0; index < providers.size(); ++index) {
      if (providers.at(index).id == QStringLiteral("native")) {
        defaultIndex = index;
        break;
      }
    }
  }
  for (int index = 0; index < providers.size(); ++index) {
    providers[index].defaultForNewSensors = index == defaultIndex;
  }
  return providers;
}

QString SensorModelProviderCatalog::defaultConfigPath() {
  return QDir(projectRoot()).absoluteFilePath(
      QStringLiteral("Data/sensor_model_providers.json"));
}

} // namespace infrastructure
