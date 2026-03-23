#include "infrastructure/ModelCatalog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
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

QString inferDomain(const QString& relativePath) {
  QString cleaned = relativePath;
  if (cleaned.startsWith('/')) {
    cleaned.remove(0, 1);
  }
  const QStringList parts = cleaned.split('/', Qt::SkipEmptyParts);
  return parts.size() >= 2 ? parts.at(1) : QStringLiteral("Air");
}

QString inferCategory(const QString& relativePath) {
  QString cleaned = relativePath;
  if (cleaned.startsWith('/')) {
    cleaned.remove(0, 1);
  }
  const QStringList parts = cleaned.split('/', Qt::SkipEmptyParts);
  return parts.size() >= 3 ? parts.at(2) : QStringLiteral("Fighter");
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

    if (line.startsWith(QStringLiteral("urlLocation:"))) {
      const QString relativePath = trimValue(line.mid(QStringLiteral("urlLocation:").size()));
      pendingEntry.relativePath = relativePath;
      pendingEntry.absolutePath = normalizeAbsolutePath(relativePath);
      pendingEntry.domain = inferDomain(relativePath);
      pendingEntry.category = inferCategory(relativePath);
      if (QFileInfo::exists(pendingEntry.absolutePath)) {
        entries.push_back(pendingEntry);
      }
      pendingEntry = ModelCatalogEntry{};
      hasPendingEntry = false;
    }
  }

  return entries;
}
