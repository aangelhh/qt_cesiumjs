#pragma once

#include "domain/Sensor.h"
#include "domain/Task.h"

#include <QString>
#include <QUuid>
#include <QVector>

struct WeaponInventoryItem {
  QString weaponType;
  int quantity = 0;
};

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

  QString damageStateLabel() const {
    if (destroyed || damagePercent >= 100.0) {
      return QStringLiteral("Destroyed");
    }
    if (damagePercent >= 30.0) {
      return QStringLiteral("Damaged");
    }
    if (damagePercent > 0.0) {
      return QStringLiteral("Lightly Damaged");
    }
    return QStringLiteral("Intact");
  }

  QString entityId = QUuid::createUuid().toString(QUuid::WithoutBraces);
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
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;
  bool flightDynamicsEnabled = false;
  QString flightDynamicsMode = QStringLiteral("kinematic");
  // Runtime-only backend used by the most recent simulation tick.
  QString activeDynamicsBackend;
  QString dynamicsFallbackReason;
  double dynamicsStepDurationMilliseconds = 0.0;
  QString jsbsimAircraftModel;
  QString dynamicsModelCompatibility;
  QString controlProfileId;
  QString systemsDisplayProfileId;
  QString cesiumModelAxes;
  int engineCount = 0;
  double fuelCapacityKilograms = 0.0;
  double fuelRemainingKilograms = 0.0;
  double speedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
  bool destroyed = false;
  double damagePercent = 0.0;
  double radarSignature = 1.0;
  double thermalSignature = 1.0;
  double visualSignature = 1.0;
  QString behaviorMode = QStringLiteral("Manual");
  QString behaviorTargetEntityId;
  QString behaviorTargetEntityName;
  EntityTask currentTask;
  QVector<WeaponInventoryItem> weapons;
  SensorDefinitions sensors;
  SensorContacts sensorContacts;
  SensorRuntimeStatuses sensorRuntimeStatuses;
};
