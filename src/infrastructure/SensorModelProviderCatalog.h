#pragma once

#include <QString>
#include <QVariantMap>
#include <QVector>

namespace infrastructure {

struct SensorModelProviderEntry {
  QString id;
  QString displayName;
  QString adapterType;
  QString libraryPath;
  QString endpoint;
  QVariantMap options;
  bool enabled = true;
  bool available = false;
  bool defaultForNewSensors = false;
};

class SensorModelProviderCatalog {
public:
  static QVector<SensorModelProviderEntry> loadProviders(
      const QString& filePath = {});
  static QString defaultConfigPath();
};

} // namespace infrastructure
