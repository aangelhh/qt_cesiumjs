#include "infrastructure/ModelCatalog.h"

#include "infrastructure/ModelOrientation.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QStringList>
#include <QTextStream>

namespace {

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

QString configPath() {
  const QString root = projectRoot();
  const QString canonical = QDir(root).absoluteFilePath(QStringLiteral("Data/config3DModel.yaml"));
  if (QFileInfo::exists(canonical)) {
    return canonical;
  }
  return QDir(root).absoluteFilePath(QStringLiteral("Data/config3dmodel.yaml"));
}

QString trimValue(QString value) {
  value = value.trimmed();
  if ((value.startsWith('"') && value.endsWith('"')) ||
      (value.startsWith('\'') && value.endsWith('\''))) {
    value = value.mid(1, value.size() - 2);
  }
  return value;
}

QString normalizeAbsolutePath(const QString& relativePath) {
  QString cleaned = relativePath.trimmed();
  if (cleaned.startsWith('/')) {
    cleaned.remove(0, 1);
  }
  return QDir(projectRoot()).absoluteFilePath(cleaned);
}

QString normalizeDomainName(const QString& rawDomain) {
  const QString normalized = rawDomain.trimmed().toLower();
  if (normalized == QStringLiteral("air")) {
    return QStringLiteral("Air");
  }
  if (normalized == QStringLiteral("ground")) {
    return QStringLiteral("Ground");
  }
  if (normalized == QStringLiteral("surface")) {
    return QStringLiteral("Surface");
  }
  return rawDomain.trimmed();
}

QString normalizeCategoryName(const QString& rawCategory) {
  const QString normalized = rawCategory.trimmed().toLower();
  if (normalized == QStringLiteral("fighter")) {
    return QStringLiteral("Fighter");
  }
  if (normalized == QStringLiteral("bomber")) {
    return QStringLiteral("Bomber");
  }
  if (normalized == QStringLiteral("helicopter")) {
    return QStringLiteral("Helicopter");
  }
  if (normalized == QStringLiteral("transport")) {
    return QStringLiteral("Transport");
  }
  if (normalized == QStringLiteral("tank")) {
    return QStringLiteral("Tank");
  }
  if (normalized == QStringLiteral("truck")) {
    return QStringLiteral("Truck");
  }
  if (normalized == QStringLiteral("armored_vehicle") ||
      normalized == QStringLiteral("armored vehicle") ||
      normalized == QStringLiteral("armoredvehicle") ||
      normalized == QStringLiteral("armoured_vehicle") ||
      normalized == QStringLiteral("armoured vehicle") ||
      normalized == QStringLiteral("armouredvehicle")) {
    return QStringLiteral("ArmoredVehicle");
  }
  if (normalized == QStringLiteral("sam_launcher") ||
      normalized == QStringLiteral("sam launcher") ||
      normalized == QStringLiteral("samlauncher")) {
    return QStringLiteral("SAMLauncher");
  }
  if (normalized == QStringLiteral("radar")) {
    return QStringLiteral("Radar");
  }
  return rawCategory.trimmed();
}

QString inferDomain(const QString& relativePath) {
  QString cleaned = relativePath;
  if (cleaned.startsWith('/')) {
    cleaned.remove(0, 1);
  }
  const QStringList parts = cleaned.split('/', Qt::SkipEmptyParts);
  return parts.size() >= 2 ? normalizeDomainName(parts.at(1)) : QStringLiteral("Air");
}

QString inferCategory(const QString& relativePath) {
  QString cleaned = relativePath;
  if (cleaned.startsWith('/')) {
    cleaned.remove(0, 1);
  }
  const QStringList parts = cleaned.split('/', Qt::SkipEmptyParts);
  return parts.size() >= 3 ? normalizeCategoryName(parts.at(2)) : QStringLiteral("Fighter");
}

int parseTrailingInteger(const QString& line, const QString& key) {
  if (!line.startsWith(key)) {
    return 0;
  }

  QString value = line.mid(key.size()).trimmed();
  const int commentIndex = value.indexOf('#');
  if (commentIndex >= 0) {
    value = value.left(commentIndex).trimmed();
  }

  bool ok = false;
  const int parsed = value.toInt(&ok);
  return ok ? parsed : 0;
}

double parseTrailingDouble(const QString& line, const QString& key) {
  if (!line.startsWith(key)) {
    return 0.0;
  }

  QString value = line.mid(key.size()).trimmed();
  const int commentIndex = value.indexOf('#');
  if (commentIndex >= 0) {
    value = value.left(commentIndex).trimmed();
  }

  bool ok = false;
  const double parsed = value.toDouble(&ok);
  return ok ? parsed : 0.0;
}

struct DynamicsBinding {
  QString backend;
  QString jsbsimAircraftModel;
  QString compatibility;
  QString controlProfileId;
  QString systemsDisplayProfileId;
  int engineCount = 0;
};

void propagateSharedAssetDynamicsBindings(QVector<ModelCatalogEntry>& entries) {
  QHash<QString, DynamicsBinding> bindingsByAsset;
  for (const ModelCatalogEntry& entry : entries) {
    if (entry.dynamicsBackend.isEmpty() || entry.absolutePath.isEmpty()) {
      continue;
    }
    bindingsByAsset.insert(
        entry.absolutePath,
        {entry.dynamicsBackend,
         entry.jsbsimAircraftModel,
         entry.dynamicsModelCompatibility,
         entry.controlProfileId,
         entry.systemsDisplayProfileId,
         entry.engineCount});
  }

  for (ModelCatalogEntry& entry : entries) {
    if (!entry.dynamicsBackend.isEmpty()) {
      continue;
    }
    const auto binding = bindingsByAsset.constFind(entry.absolutePath);
    if (binding == bindingsByAsset.constEnd()) {
      continue;
    }
    entry.dynamicsBackend = binding->backend;
    entry.jsbsimAircraftModel = binding->jsbsimAircraftModel;
    entry.dynamicsModelCompatibility = binding->compatibility;
    entry.controlProfileId = binding->controlProfileId;
    entry.systemsDisplayProfileId = binding->systemsDisplayProfileId;
    entry.engineCount = binding->engineCount;
  }
}

} // namespace

QVector<ModelCatalogEntry> ModelCatalog::loadModels() {
  QVector<ModelCatalogEntry> entries;

  QFile file(configPath());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return entries;
  }

  QTextStream input(&file);
  ModelCatalogEntry pendingEntry;
  bool hasPendingEntry = false;

  while (!input.atEnd()) {
    const QString rawLine = input.readLine();
    const QString line = rawLine.trimmed();

    if (line.startsWith(QStringLiteral("- name:"))) {
      pendingEntry = ModelCatalogEntry{};
      pendingEntry.name = trimValue(line.mid(QStringLiteral("- name:").size()));
      hasPendingEntry = true;
      continue;
    }

    if (!hasPendingEntry) {
      continue;
    }

    if (line.startsWith(QStringLiteral("Kind:"))) {
      pendingEntry.entityKind = parseTrailingInteger(line, QStringLiteral("Kind:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("Domain:"))) {
      pendingEntry.entityDomain = parseTrailingInteger(line, QStringLiteral("Domain:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("Country:"))) {
      pendingEntry.entityCountry = parseTrailingInteger(line, QStringLiteral("Country:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("Category:"))) {
      pendingEntry.entityCategory = parseTrailingInteger(line, QStringLiteral("Category:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("Subcategory:"))) {
      pendingEntry.entitySubcategory = parseTrailingInteger(line, QStringLiteral("Subcategory:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("Specific:"))) {
      pendingEntry.entitySpecific = parseTrailingInteger(line, QStringLiteral("Specific:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("Extra:"))) {
      pendingEntry.entityExtra = parseTrailingInteger(line, QStringLiteral("Extra:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("systemsDisplayProfile:"))) {
      pendingEntry.systemsDisplayProfileId = trimValue(
          line.mid(QStringLiteral("systemsDisplayProfile:").size()));
      continue;
    }
    if (line.startsWith(QStringLiteral("dynamicsBackend:"))) {
      pendingEntry.dynamicsBackend = trimValue(
          line.mid(QStringLiteral("dynamicsBackend:").size())).toLower();
      continue;
    }
    if (line.startsWith(QStringLiteral("jsbsimAircraftModel:"))) {
      pendingEntry.jsbsimAircraftModel = trimValue(
          line.mid(QStringLiteral("jsbsimAircraftModel:").size()));
      continue;
    }
    if (line.startsWith(QStringLiteral("dynamicsModelCompatibility:"))) {
      pendingEntry.dynamicsModelCompatibility = trimValue(
          line.mid(QStringLiteral("dynamicsModelCompatibility:").size()))
          .toLower();
      continue;
    }
    if (line.startsWith(QStringLiteral("controlProfile:"))) {
      pendingEntry.controlProfileId = trimValue(
          line.mid(QStringLiteral("controlProfile:").size()));
      continue;
    }
    if (line.startsWith(QStringLiteral("cesiumAxes:"))) {
      pendingEntry.cesiumAxes = trimValue(
          line.mid(QStringLiteral("cesiumAxes:").size())).toLower();
      continue;
    }
    if (line.startsWith(QStringLiteral("engineCount:"))) {
      pendingEntry.engineCount = parseTrailingInteger(
          line,
          QStringLiteral("engineCount:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("fuelCapacityKilograms:"))) {
      pendingEntry.fuelCapacityKilograms = parseTrailingDouble(
          line,
          QStringLiteral("fuelCapacityKilograms:"));
      continue;
    }
    if (line.startsWith(QStringLiteral("initialFuelKilograms:"))) {
      pendingEntry.initialFuelKilograms = parseTrailingDouble(
          line,
          QStringLiteral("initialFuelKilograms:"));
      continue;
    }

    if (line.startsWith(QStringLiteral("urlLocation:"))) {
      const QString relativePath = trimValue(line.mid(QStringLiteral("urlLocation:").size()));
      pendingEntry.relativePath = relativePath;
      pendingEntry.absolutePath = normalizeAbsolutePath(relativePath);
      pendingEntry.domain = inferDomain(relativePath);
      pendingEntry.category = inferCategory(relativePath);
      pendingEntry.cesiumAxes = ModelOrientation::resolveCesiumAxes(
          pendingEntry.cesiumAxes,
          relativePath);
      if (QFileInfo::exists(pendingEntry.absolutePath)) {
        entries.push_back(pendingEntry);
      }
      pendingEntry = ModelCatalogEntry{};
      hasPendingEntry = false;
    }
  }

  propagateSharedAssetDynamicsBindings(entries);
  return entries;
}
