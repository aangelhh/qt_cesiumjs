#pragma once

#include <QString>
#include <QVector>

struct ModelCatalogEntry {
  QString name;
  QString domain;
  QString category;
  QString configuredRelativePath;
  QString relativePath;
  QString absolutePath;
  QString visualModelCompatibility;
  QString dynamicsBackend;
  QString jsbsimAircraftModel;
  QString dynamicsModelCompatibility;
  QString controlProfileId;
  QString systemsDisplayProfileId;
  QString cesiumAxes;
  int engineCount = 0;
  double fuelCapacityKilograms = 0.0;
  double initialFuelKilograms = 0.0;
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
