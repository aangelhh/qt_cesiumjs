#include "domain/Sensor.h"

namespace sensor {

SensorType typeFromString(const QString& value) {
  const QString normalized = value.trimmed();
  if (normalized.compare(QStringLiteral("radar"), Qt::CaseInsensitive) == 0) {
    return SensorType::Radar;
  }
  if (normalized.compare(QStringLiteral("ir"), Qt::CaseInsensitive) == 0 ||
      normalized.compare(QStringLiteral("infrared"), Qt::CaseInsensitive) == 0) {
    return SensorType::Infrared;
  }
  if (normalized.compare(QStringLiteral("visual"), Qt::CaseInsensitive) == 0 ||
      normalized.compare(QStringLiteral("optical"), Qt::CaseInsensitive) == 0) {
    return SensorType::Visual;
  }
  return SensorType::Unknown;
}

QString typeToString(SensorType type) {
  switch (type) {
    case SensorType::Radar:
      return QStringLiteral("radar");
    case SensorType::Infrared:
      return QStringLiteral("ir");
    case SensorType::Visual:
      return QStringLiteral("visual");
    case SensorType::Unknown:
      return QStringLiteral("unknown");
  }
  return QStringLiteral("unknown");
}

SensorSubType subTypeFromString(const QString& value) {
  QString normalized = value.trimmed().toLower();
  normalized.remove(QLatin1Char('_'));
  normalized.remove(QLatin1Char('-'));
  normalized.remove(QLatin1Char(' '));

  if (normalized.isEmpty() || normalized == QStringLiteral("generic")) {
    return SensorSubType::Generic;
  }
  if (normalized == QStringLiteral("airborneradar")) {
    return SensorSubType::AirborneRadar;
  }
  if (normalized == QStringLiteral("groundradar")) {
    return SensorSubType::GroundRadar;
  }
  if (normalized == QStringLiteral("navalradar")) {
    return SensorSubType::NavalRadar;
  }
  return SensorSubType::Unknown;
}

QString subTypeToString(SensorSubType subType) {
  switch (subType) {
    case SensorSubType::Generic:
      return QStringLiteral("generic");
    case SensorSubType::AirborneRadar:
      return QStringLiteral("airborneRadar");
    case SensorSubType::GroundRadar:
      return QStringLiteral("groundRadar");
    case SensorSubType::NavalRadar:
      return QStringLiteral("navalRadar");
    case SensorSubType::Unknown:
      return QStringLiteral("unknown");
  }
  return QStringLiteral("unknown");
}

bool canDetectDomain(const SensorDefinition& definition, const QString& domain) {
  if (domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
    return definition.canDetectAir;
  }
  if (domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0) {
    return definition.canDetectGround;
  }
  if (domain.compare(QStringLiteral("Surface"), Qt::CaseInsensitive) == 0) {
    return definition.canDetectSurface;
  }
  return false;
}

bool canOperateFromDomain(const SensorDefinition& definition, const QString& domain) {
  switch (subTypeFromString(definition.sensorSubType)) {
    case SensorSubType::AirborneRadar:
      return domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0;
    case SensorSubType::GroundRadar:
      return domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
    case SensorSubType::NavalRadar:
      return domain.compare(QStringLiteral("Surface"), Qt::CaseInsensitive) == 0;
    case SensorSubType::Generic:
    case SensorSubType::Unknown:
      return true;
  }
  return true;
}

bool supportsTargetDomain(const SensorDefinition& definition, const QString& domain) {
  if (!canDetectDomain(definition, domain)) {
    return false;
  }

  switch (subTypeFromString(definition.sensorSubType)) {
    case SensorSubType::AirborneRadar:
    case SensorSubType::GroundRadar:
      return domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0;
    case SensorSubType::Generic:
    case SensorSubType::NavalRadar:
    case SensorSubType::Unknown:
      return true;
  }
  return true;
}

} // namespace sensor
