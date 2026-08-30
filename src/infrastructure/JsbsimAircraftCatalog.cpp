#include "infrastructure/JsbsimAircraftCatalog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QSet>

#include <algorithm>

JsbsimAircraftCatalog JsbsimAircraftCatalog::discover(
    const QString& aircraftRootPath) {
  JsbsimAircraftCatalog catalog;
  const QDir aircraftRoot(aircraftRootPath);
  const QFileInfoList candidates = aircraftRoot.entryInfoList(
      QDir::Dirs | QDir::NoDotAndDotDot,
      QDir::Name | QDir::IgnoreCase);

  for (const QFileInfo& candidate : candidates) {
    const QString modelId = candidate.fileName();
    const QString configurationPath = QDir(candidate.absoluteFilePath())
        .absoluteFilePath(modelId + QStringLiteral(".xml"));
    QFile configuration(configurationPath);
    if (!configuration.exists()) {
      catalog._diagnostics.push_back({
          modelId,
          QStringLiteral("Canonical configuration %1.xml is missing")
              .arg(modelId)});
      continue;
    }
    if (!configuration.open(QIODevice::ReadOnly | QIODevice::Text)) {
      catalog._diagnostics.push_back({
          modelId,
          QStringLiteral("Configuration cannot be opened")});
      continue;
    }

    QXmlStreamReader xml(&configuration);
    if (!xml.readNextStartElement() ||
        xml.name().compare(QStringLiteral("fdm_config"),
                           Qt::CaseInsensitive) != 0) {
      catalog._diagnostics.push_back({
          modelId,
          xml.hasError()
              ? QStringLiteral("Configuration XML is invalid: %1")
                    .arg(xml.errorString())
              : QStringLiteral("Root element is not fdm_config")});
      continue;
    }

    const auto attributes = xml.attributes();
    const QString configuredName =
        attributes.value(QStringLiteral("name")).toString().trimmed();
    catalog._entries.push_back({
        modelId,
        configuredName.isEmpty() ? modelId : configuredName,
        configurationPath,
        attributes.value(QStringLiteral("version")).toString().trimmed(),
        aircraftRoot.dirName().compare(
            QStringLiteral("aircraft"), Qt::CaseInsensitive) == 0
            ? QFileInfo(aircraftRoot.absolutePath()).absoluteDir().absolutePath()
            : QString()});
  }

  std::sort(
      catalog._entries.begin(),
      catalog._entries.end(),
      [](const JsbsimAircraftCatalogEntry& left,
         const JsbsimAircraftCatalogEntry& right) {
        return left.modelId.compare(right.modelId, Qt::CaseInsensitive) < 0;
      });
  return catalog;
}

JsbsimAircraftCatalog JsbsimAircraftCatalog::discoverModelRoots(
    const QStringList& modelRootPaths) {
  JsbsimAircraftCatalog merged;
  QSet<QString> discoveredIds;

  for (const QString& modelRootPath : modelRootPaths) {
    const QString aircraftRoot =
        QDir(modelRootPath).absoluteFilePath(QStringLiteral("aircraft"));
    JsbsimAircraftCatalog catalog = discover(aircraftRoot);
    for (const JsbsimAircraftCatalogDiagnostic& diagnostic :
         catalog._diagnostics) {
      merged._diagnostics.push_back(diagnostic);
    }
    for (const JsbsimAircraftCatalogEntry& entry : catalog._entries) {
      const QString normalizedId = entry.modelId.toCaseFolded();
      if (discoveredIds.contains(normalizedId)) {
        merged._diagnostics.push_back({
            entry.modelId,
            QStringLiteral("Duplicate model id ignored from %1")
                .arg(modelRootPath)});
        continue;
      }
      discoveredIds.insert(normalizedId);
      merged._entries.push_back(entry);
    }
  }

  std::sort(
      merged._entries.begin(),
      merged._entries.end(),
      [](const JsbsimAircraftCatalogEntry& left,
         const JsbsimAircraftCatalogEntry& right) {
        return left.modelId.compare(right.modelId, Qt::CaseInsensitive) < 0;
      });
  return merged;
}

const QVector<JsbsimAircraftCatalogEntry>&
JsbsimAircraftCatalog::entries() const {
  return _entries;
}

const QVector<JsbsimAircraftCatalogDiagnostic>&
JsbsimAircraftCatalog::diagnostics() const {
  return _diagnostics;
}

QStringList JsbsimAircraftCatalog::modelIds() const {
  QStringList ids;
  ids.reserve(_entries.size());
  for (const JsbsimAircraftCatalogEntry& entry : _entries) {
    ids.push_back(entry.modelId);
  }
  return ids;
}
