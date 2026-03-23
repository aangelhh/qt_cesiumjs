#pragma once

#include <QString>
#include <QVector>

struct ModelCatalogEntry {
  QString name;
  QString domain;
  QString category;
  QString relativePath;
  QString absolutePath;
  int entityKind = 0;
  int entityDomain = 0;
  int entityCountry = 0;
  int entityCategory = 0;
  int entitySubcategory = 0;
  int entitySpecific = 0;
  int entityExtra = 0;
};

class ModelCatalog {
public:
  static QVector<ModelCatalogEntry> loadModels();
};
