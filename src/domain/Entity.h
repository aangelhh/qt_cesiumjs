#pragma once

#include <QString>

struct Entity {
  QString name;
  QString type = QStringLiteral("Entity");
  QString domain = QStringLiteral("Air");
  QString category = QStringLiteral("Fighter");
  QString callsign;
  int forceIdentifier = 1;
  double latitude = 0.0;
  double longitude = 0.0;
  double groundHeight = 0.0;
  int altitude = 0;
  QString modelName;
  QString modelUri;
  int entityKind = 0;
  int entityDomain = 0;
  int entityCountry = 0;
  int entityCategory = 0;
  int entitySubcategory = 0;
  int entitySpecific = 0;
  int entityExtra = 0;
};
