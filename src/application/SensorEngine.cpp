#include "application/SensorEngine.h"

#include <QtMath>

namespace {

constexpr double kEarthRadiusMeters = 6371000.0;

double normalizeDegrees(double degrees) {
  while (degrees < -180.0) {
    degrees += 360.0;
  }
  while (degrees > 180.0) {
    degrees -= 360.0;
  }
  return degrees;
}

double distanceMeters(const Entity& source, const Entity& target) {
  const double lat1 = qDegreesToRadians(source.latitude);
  const double lon1 = qDegreesToRadians(source.longitude);
  const double lat2 = qDegreesToRadians(target.latitude);
  const double lon2 = qDegreesToRadians(target.longitude);

  const double deltaLat = lat2 - lat1;
  const double deltaLon = lon2 - lon1;
  const double a = qPow(qSin(deltaLat / 2.0), 2.0) +
      qCos(lat1) * qCos(lat2) * qPow(qSin(deltaLon / 2.0), 2.0);
  const double surfaceDistance = 2.0 * kEarthRadiusMeters * qAtan2(qSqrt(a), qSqrt(1.0 - a));

  const double altitudeDelta = static_cast<double>(target.altitude - source.altitude);
  return qSqrt(surfaceDistance * surfaceDistance + altitudeDelta * altitudeDelta);
}

double bearingDegrees(const Entity& source, const Entity& target) {
  const double lat1 = qDegreesToRadians(source.latitude);
  const double lon1 = qDegreesToRadians(source.longitude);
  const double lat2 = qDegreesToRadians(target.latitude);
  const double lon2 = qDegreesToRadians(target.longitude);

  const double y = qSin(lon2 - lon1) * qCos(lat2);
  const double x = qCos(lat1) * qSin(lat2) -
      qSin(lat1) * qCos(lat2) * qCos(lon2 - lon1);
  double bearing = qRadiansToDegrees(qAtan2(y, x));
  if (bearing < 0.0) {
    bearing += 360.0;
  }
  return bearing;
}

bool domainDetectedBySensor(const SensorDefinition& sensor, const Entity& target) {
  const QString domain = target.domain.trimmed().toLower();
  if (domain == QStringLiteral("air")) {
    return sensor.canDetectAir;
  }
  if (domain == QStringLiteral("surface")) {
    return sensor.canDetectSurface;
  }
  if (domain == QStringLiteral("land") || domain == QStringLiteral("ground")) {
    return sensor.canDetectGround;
  }
  return sensor.canDetectAir || sensor.canDetectGround || sensor.canDetectSurface;
}

bool targetInsideAzimuth(const Entity& source, const SensorDefinition& sensor, const Entity& target) {
  if (sensor.azimuthWidthDegrees >= 360.0) {
    return true;
  }

  const double bearing = bearingDegrees(source, target);
  const double sensorCenter = source.headingDegrees + sensor.azimuthCenterDegrees;
  const double delta = qAbs(normalizeDegrees(bearing - sensorCenter));
  return delta <= sensor.azimuthWidthDegrees / 2.0;
}

} // namespace

void SensorEngine::updateEntityContacts(QVector<Entity>& entities) {
  for (Entity& entity : entities) {
    entity.sensorContacts.clear();
  }

  for (int sourceIndex = 0; sourceIndex < entities.size(); ++sourceIndex) {
    Entity& source = entities[sourceIndex];

    for (const SensorDefinition& sensor : source.sensors) {
      if (!sensor.enabled || !sensor.emitting) {
        continue;
      }

      int tracksAdded = 0;
      for (int targetIndex = 0; targetIndex < entities.size(); ++targetIndex) {
        if (sourceIndex == targetIndex) {
          continue;
        }

        const Entity& target = entities[targetIndex];
        if (!domainDetectedBySensor(sensor, target)) {
          continue;
        }

        const double range = distanceMeters(source, target);
        if (range < sensor.minRangeMeters || range > sensor.maxRangeMeters) {
          continue;
        }

        if (!targetInsideAzimuth(source, sensor, target)) {
          continue;
        }

        SensorContact contact;
        contact.sensorId = sensor.id;
        contact.targetEntityName = target.name;
        contact.rangeMeters = range;
        contact.bearingDegrees = bearingDegrees(source, target);
        contact.lineOfSight = !sensor.terrainMaskingEnabled;
        contact.detected = sensor.probabilityOfDetection > 0.0;
        source.sensorContacts.push_back(contact);

        ++tracksAdded;
        if (tracksAdded >= sensor.maxTracks) {
          break;
        }
      }
    }
  }
}
