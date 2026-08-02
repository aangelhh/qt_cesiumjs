#pragma once

#include <QString>
#include <QVector>

enum class SensorType {
  Radar,
  Infrared,
  Visual,
  Unknown,
};

enum class SensorSubType {
  Generic,
  AirborneRadar,
  GroundRadar,
  NavalRadar,
  Unknown,
};

struct SensorDefinition {
  QString id;
  QString name;
  QString sensorType = QStringLiteral("radar");
  QString sensorSubType = QStringLiteral("generic");
  bool enabled = true;
  bool emitting = true;
  double maxRangeMeters = 250000.0;
  double minRangeMeters = 0.0;
  double azimuthCenterDegrees = 0.0;
  double azimuthWidthDegrees = 360.0;
  double elevationCenterDegrees = 0.0;
  double elevationWidthDegrees = 90.0;
  double updatePeriodSeconds = 5.0;
  int maxTracks = 32;
  bool iffCapable = true;
  bool canDetectAir = true;
  bool canDetectGround = false;
  bool canDetectSurface = false;
  bool terrainMaskingEnabled = false;
  double probabilityOfDetection = 1.0;
};

struct SensorContact {
  QString sensorId;
  QString sensorType;
  QString sensorSubType;
  QString targetEntityId;
  QString targetEntityName;
  double rangeMeters = 0.0;
  double bearingDegrees = 0.0;
  bool lineOfSight = true;
  bool detected = false;
};

using SensorDefinitions = QVector<SensorDefinition>;
using SensorContacts = QVector<SensorContact>;

namespace sensor {

SensorType typeFromString(const QString& value);
QString typeToString(SensorType type);

SensorSubType subTypeFromString(const QString& value);
QString subTypeToString(SensorSubType subType);

bool canDetectDomain(const SensorDefinition& definition, const QString& domain);
bool canOperateFromDomain(const SensorDefinition& definition, const QString& domain);
bool supportsTargetDomain(const SensorDefinition& definition, const QString& domain);

} // namespace sensor
