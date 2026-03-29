#pragma once

#include "domain/Sensor.h"
#include "domain/Task.h"

#include <QString>
#include <QVector>

struct Entity {
  static QString buildEntityTypeCode(
      int kind,
      int domainCode,
      int country,
      int categoryCode,
      int subcategory,
      int specific,
      int extra) {
    return QStringLiteral("%1:%2:%3:%4:%5:%6:%7")
        .arg(kind)
        .arg(domainCode)
        .arg(country)
        .arg(categoryCode)
        .arg(subcategory)
        .arg(specific)
        .arg(extra);
  }

  void refreshEntityTypeCode() {
    entityTypeCode = buildEntityTypeCode(
        entityKind,
        entityDomain,
        entityCountry,
        entityCategory,
        entitySubcategory,
        entitySpecific,
        entityExtra);
  }

  QString name;
  QString type = QStringLiteral("Entity");
  QString domain = QStringLiteral("Air");
  QString category = QStringLiteral("Fighter");
  QString entityTypeCode;
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
  double headingDegrees = 0.0;
  bool flightDynamicsEnabled = false;
  QString flightDynamicsMode = QStringLiteral("kinematic");
  QString jsbsimAircraftModel;
  double speedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
  EntityTask currentTask;
  SensorDefinitions sensors;
  SensorContacts sensorContacts;
};
