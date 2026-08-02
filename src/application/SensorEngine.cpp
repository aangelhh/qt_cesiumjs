#include "application/SensorEngine.h"

#include "domain/EntityIdentity.h"

#include <QtMath>

namespace {

constexpr double kEarthRadiusMeters = 6371000.0;

double normalizeDegrees180(double degrees) {
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

double horizontalDistanceMeters(const Entity& source, const Entity& target) {
  const double lat1 = qDegreesToRadians(source.latitude);
  const double lon1 = qDegreesToRadians(source.longitude);
  const double lat2 = qDegreesToRadians(target.latitude);
  const double lon2 = qDegreesToRadians(target.longitude);

  const double deltaLat = lat2 - lat1;
  const double deltaLon = lon2 - lon1;
  const double a = qPow(qSin(deltaLat / 2.0), 2.0) +
      qCos(lat1) * qCos(lat2) * qPow(qSin(deltaLon / 2.0), 2.0);
  const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
  return kEarthRadiusMeters * c;
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

bool isCombatObserver(const Entity& entity) {
  return entity.forceIdentifier == 1 || entity.forceIdentifier == 2;
}

bool isEnemyTarget(const Entity& source, const Entity& target) {
  if (source.forceIdentifier == 1) {
    return target.forceIdentifier == 2;
  }
  if (source.forceIdentifier == 2) {
    return target.forceIdentifier == 1;
  }
  return false;
}

bool targetInsideHorizontalBeam(
    const Entity& source,
    const SensorDefinition& sensor,
    const Entity& target) {
  if (sensor.azimuthWidthDegrees >= 360.0) {
    return true;
  }

  const double targetBearing = bearingDegrees(source, target);
  const double beamCenter = source.headingDegrees + sensor.azimuthCenterDegrees;
  const double deltaDegrees =
      qAbs(normalizeDegrees180(targetBearing - beamCenter));
  return deltaDegrees <= sensor.azimuthWidthDegrees / 2.0;
}

bool targetInsideVerticalBeam(
    const Entity& source,
    const SensorDefinition& sensor,
    const Entity& target) {
  if (sensor.elevationWidthDegrees <= 0.0) {
    // Zero or negative width means the vertical beam is effectively closed.
    return false;
  }
  if (sensor.elevationWidthDegrees >= 180.0) {
    // The elevation angle is relative to the local horizon, so 180 deg covers all.
    return true;
  }

  const double horizontalRange = horizontalDistanceMeters(source, target);
  const double altitudeDelta =
      static_cast<double>(target.altitude - source.altitude);
  // Entity has no pitch/roll, so elevation is interpreted against the local horizon.
  const double targetElevationDegrees =
      qRadiansToDegrees(qAtan2(altitudeDelta, horizontalRange));
  const double deltaDegrees =
      qAbs(normalizeDegrees180(
          targetElevationDegrees - sensor.elevationCenterDegrees));
  return deltaDegrees <= sensor.elevationWidthDegrees / 2.0;
}

} // namespace

void SensorEngine::updateEntityContacts(QVector<Entity>& entities) {
  for (Entity& entity : entities) {
    entity.sensorContacts.clear();
  }

  for (int sourceIndex = 0; sourceIndex < entities.size(); ++sourceIndex) {
    Entity& source = entities[sourceIndex];
    if (source.destroyed) {
      continue;
    }
    if (!isCombatObserver(source)) {
      continue;
    }

    for (const SensorDefinition& sensor : source.sensors) {
      if (!sensor.enabled || !sensor.emitting || sensor.maxRangeMeters <= 0.0) {
        continue;
      }
      if (!sensor::canOperateFromDomain(sensor, source.domain)) {
        continue;
      }

      int tracksAdded = 0;
      for (int targetIndex = 0; targetIndex < entities.size(); ++targetIndex) {
        if (sourceIndex == targetIndex) {
          continue;
        }

        const Entity& target = entities[targetIndex];
        if (target.destroyed) {
          continue;
        }
        if (!isEnemyTarget(source, target)) {
          continue;
        }
        if (!sensor::supportsTargetDomain(sensor, target.domain)) {
          continue;
        }

        const double range = distanceMeters(source, target);
        if (range < sensor.minRangeMeters || range > sensor.maxRangeMeters) {
          continue;
        }

        if (!targetInsideHorizontalBeam(source, sensor, target)) {
          continue;
        }

        if (!targetInsideVerticalBeam(source, sensor, target)) {
          continue;
        }

        SensorContact contact;
        contact.sensorId = sensor.id;
        contact.sensorType = sensor.sensorType;
        contact.sensorSubType = sensor.sensorSubType;
        contact.targetEntityId = domain::entityKey(target);
        contact.targetEntityName = target.name;
        contact.rangeMeters = range;
        contact.bearingDegrees = bearingDegrees(source, target);
        contact.lineOfSight = true;
        contact.detected = true;
        source.sensorContacts.push_back(contact);

        ++tracksAdded;
        if (tracksAdded >= sensor.maxTracks) {
          break;
        }
      }
    }
  }
}
