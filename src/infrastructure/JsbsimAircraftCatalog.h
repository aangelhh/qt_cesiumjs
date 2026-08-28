#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

struct JsbsimAircraftCatalogEntry {
  QString modelId;
  QString displayName;
  QString configurationPath;
  QString formatVersion;
};

struct JsbsimAircraftCatalogDiagnostic {
  QString candidateId;
  QString reason;
};

class JsbsimAircraftCatalog {
public:
  static JsbsimAircraftCatalog discover(const QString& aircraftRootPath);

  const QVector<JsbsimAircraftCatalogEntry>& entries() const;
  const QVector<JsbsimAircraftCatalogDiagnostic>& diagnostics() const;
  QStringList modelIds() const;

private:
  QVector<JsbsimAircraftCatalogEntry> _entries;
  QVector<JsbsimAircraftCatalogDiagnostic> _diagnostics;
};
