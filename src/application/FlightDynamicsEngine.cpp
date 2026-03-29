#include "application/FlightDynamicsEngine.h"

#if defined(QTTEST_HAS_JSBSIM)
#include <FGFDMExec.h>
#endif

#include <QDir>
#include <QHash>
#include <QtMath>

#include <memory>

namespace {

constexpr double kEarthRadiusMeters = 6371000.0;
constexpr double kKnotsToMetersPerSecond = 0.514444;
constexpr double kHeadingRateDegreesPerSecond = 3.0;
constexpr double kAccelerationKnotsPerSecond = 8.0;
constexpr double kClimbRateMetersPerSecond = 20.0;
constexpr double kMetersToFeet = 3.28084;
constexpr double kFeetToMeters = 1.0 / kMetersToFeet;

double clampStep(double currentValue, double targetValue, double maxStep) {
  const double delta = targetValue - currentValue;
  if (qAbs(delta) <= maxStep) {
    return targetValue;
  }
  return currentValue + (delta > 0.0 ? maxStep : -maxStep);
}

double normalizeDegrees360(double degrees) {
  while (degrees < 0.0) {
    degrees += 360.0;
  }
  while (degrees >= 360.0) {
    degrees -= 360.0;
  }
  return degrees;
}

double shortestSignedAngle(double currentHeading, double targetHeading) {
  double delta = normalizeDegrees360(targetHeading) - normalizeDegrees360(currentHeading);
  while (delta > 180.0) {
    delta -= 360.0;
  }
  while (delta < -180.0) {
    delta += 360.0;
  }
  return delta;
}

double bearingDegrees(
    double latitude1,
    double longitude1,
    double latitude2,
    double longitude2) {
  const double lat1 = qDegreesToRadians(latitude1);
  const double lat2 = qDegreesToRadians(latitude2);
  const double deltaLongitude = qDegreesToRadians(longitude2 - longitude1);
  const double y = qSin(deltaLongitude) * qCos(lat2);
  const double x = qCos(lat1) * qSin(lat2) -
                   qSin(lat1) * qCos(lat2) * qCos(deltaLongitude);
  return normalizeDegrees360(qRadiansToDegrees(qAtan2(y, x)));
}

double distanceMeters(
    double latitude1,
    double longitude1,
    double latitude2,
    double longitude2) {
  const double lat1 = qDegreesToRadians(latitude1);
  const double lon1 = qDegreesToRadians(longitude1);
  const double lat2 = qDegreesToRadians(latitude2);
  const double lon2 = qDegreesToRadians(longitude2);
  const double deltaLat = lat2 - lat1;
  const double deltaLon = lon2 - lon1;
  const double a = qPow(qSin(deltaLat / 2.0), 2.0) +
                   qCos(lat1) * qCos(lat2) * qPow(qSin(deltaLon / 2.0), 2.0);
  const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
  return kEarthRadiusMeters * c;
}

QString defaultJsbsimAircraftModel(const Entity& entity) {
  if (!entity.jsbsimAircraftModel.trimmed().isEmpty()) {
    return entity.jsbsimAircraftModel.trimmed();
  }

  const QString modelName = entity.modelName.trimmed().toLower();
  const QString type = entity.type.trimmed().toLower();
  const QString normalizedDomain = entity.domain.trimmed().toLower();
  const QString normalizedCategory = entity.category.trimmed().toLower();

  if (modelName.contains(QStringLiteral("f-16")) || type.contains(QStringLiteral("f-16"))) {
    return QStringLiteral("f16");
  }
  if (modelName.contains(QStringLiteral("f-22")) || type.contains(QStringLiteral("f-22"))) {
    return QStringLiteral("f22");
  }
  if (modelName.contains(QStringLiteral("a-4")) || type.contains(QStringLiteral("a-4"))) {
    return QStringLiteral("A4");
  }
  if (modelName.contains(QStringLiteral("t-38")) || type.contains(QStringLiteral("t-38"))) {
    return QStringLiteral("T38");
  }
  if (modelName.contains(QStringLiteral("b747")) || type.contains(QStringLiteral("b747"))) {
    return QStringLiteral("B747");
  }
  if (modelName.contains(QStringLiteral("a320")) || type.contains(QStringLiteral("a320"))) {
    return QStringLiteral("A320");
  }
  if (modelName.contains(QStringLiteral("c172")) || type.contains(QStringLiteral("c172"))) {
    return QStringLiteral("c172r");
  }
  if (modelName.contains(QStringLiteral("c182")) || type.contains(QStringLiteral("c182"))) {
    return QStringLiteral("c182");
  }
  if (modelName.contains(QStringLiteral("dhc")) || type.contains(QStringLiteral("dhc"))) {
    return QStringLiteral("DHC6");
  }
  if (modelName.contains(QStringLiteral("ov-10")) || type.contains(QStringLiteral("ov-10"))) {
    return QStringLiteral("OV10");
  }
  if (modelName.contains(QStringLiteral("texan")) || type.contains(QStringLiteral("texan"))) {
    return QStringLiteral("t6texan2");
  }

  if (normalizedDomain == QStringLiteral("air")) {
    if (normalizedCategory == QStringLiteral("fighter")) {
      return QStringLiteral("f16");
    }
    if (normalizedCategory == QStringLiteral("bomber")) {
      return QStringLiteral("B17");
    }
    if (normalizedCategory == QStringLiteral("transport")) {
      return QStringLiteral("DHC6");
    }
    if (normalizedCategory == QStringLiteral("helicopter")) {
      return QStringLiteral("F450");
    }
  }
  return QStringLiteral("c172r");
}

void applyKinematicStep(Entity& entity, double deltaSeconds) {
  const double speedMetersPerSecond = entity.speedKnots * kKnotsToMetersPerSecond;
  const double distance = speedMetersPerSecond * deltaSeconds;
  const double angularDistance = distance / kEarthRadiusMeters;
  const double headingRadians = qDegreesToRadians(entity.headingDegrees);
  const double lat1 = qDegreesToRadians(entity.latitude);
  const double lon1 = qDegreesToRadians(entity.longitude);

  const double sinLat1 = qSin(lat1);
  const double cosLat1 = qCos(lat1);
  const double sinAngular = qSin(angularDistance);
  const double cosAngular = qCos(angularDistance);

  const double lat2 = qAsin(
      sinLat1 * cosAngular +
      cosLat1 * sinAngular * qCos(headingRadians));
  const double lon2 = lon1 + qAtan2(
      qSin(headingRadians) * sinAngular * cosLat1,
      cosAngular - sinLat1 * qSin(lat2));

  entity.latitude = qRadiansToDegrees(lat2);
  entity.longitude = qRadiansToDegrees(lon2);
  entity.altitude = qMax(
      0,
      static_cast<int>(qRound(entity.altitude + entity.verticalSpeedMetersPerSecond * deltaSeconds)));
}

void resolveTaskTargets(Entity& entity, const QVector<Entity>& snapshot, double deltaSeconds) {
  if (!entity.currentTask.enabled) {
    return;
  }

  if (entity.currentTask.taskType == QStringLiteral("FlyHeadingAltitudeSpeed")) {
    const double headingDelta = shortestSignedAngle(
        entity.headingDegrees,
        entity.currentTask.targetHeadingDegrees);
    entity.headingDegrees = normalizeDegrees360(
        entity.headingDegrees +
        clampStep(0.0, headingDelta, kHeadingRateDegreesPerSecond * deltaSeconds));
    entity.speedKnots = clampStep(
        entity.speedKnots,
        entity.currentTask.targetSpeedKnots,
        kAccelerationKnotsPerSecond * deltaSeconds);
    const double targetAltitude = static_cast<double>(entity.currentTask.targetAltitudeMeters);
    const double altitudeDelta = targetAltitude - static_cast<double>(entity.altitude);
    entity.verticalSpeedMetersPerSecond = clampStep(
        0.0,
        altitudeDelta,
        kClimbRateMetersPerSecond);
    entity.currentTask.status = QStringLiteral("Running");
    return;
  }

  if (entity.currentTask.taskType == QStringLiteral("MoveToLocation")) {
    entity.currentTask.targetHeadingDegrees = bearingDegrees(
        entity.latitude,
        entity.longitude,
        entity.currentTask.targetLatitude,
        entity.currentTask.targetLongitude);
    const double rangeMeters = distanceMeters(
        entity.latitude,
        entity.longitude,
        entity.currentTask.targetLatitude,
        entity.currentTask.targetLongitude);
    const double headingDelta = shortestSignedAngle(
        entity.headingDegrees,
        entity.currentTask.targetHeadingDegrees);
    entity.headingDegrees = normalizeDegrees360(
        entity.headingDegrees +
        clampStep(0.0, headingDelta, kHeadingRateDegreesPerSecond * deltaSeconds));
    entity.speedKnots = clampStep(
        entity.speedKnots,
        entity.currentTask.targetSpeedKnots,
        kAccelerationKnotsPerSecond * deltaSeconds);
    const double altitudeDelta =
        static_cast<double>(entity.currentTask.targetAltitudeMeters - entity.altitude);
    entity.verticalSpeedMetersPerSecond = clampStep(
        0.0,
        altitudeDelta,
        kClimbRateMetersPerSecond);
    entity.currentTask.status =
        rangeMeters < 200.0 ? QStringLiteral("On target") : QStringLiteral("Running");
    if (rangeMeters < 200.0) {
      entity.speedKnots = 0.0;
      entity.verticalSpeedMetersPerSecond = 0.0;
    }
    return;
  }

  if (entity.currentTask.taskType == QStringLiteral("FollowEntity")) {
    for (const Entity& target : snapshot) {
      if (target.name != entity.currentTask.targetEntityName) {
        continue;
      }

      entity.currentTask.targetLatitude = target.latitude;
      entity.currentTask.targetLongitude = target.longitude;
      entity.currentTask.targetHeadingDegrees = bearingDegrees(
          entity.latitude,
          entity.longitude,
          target.latitude,
          target.longitude);
      const double rangeMeters = distanceMeters(
          entity.latitude,
          entity.longitude,
          target.latitude,
          target.longitude);
      const double headingDelta = shortestSignedAngle(
          entity.headingDegrees,
          entity.currentTask.targetHeadingDegrees);
      entity.headingDegrees = normalizeDegrees360(
          entity.headingDegrees +
          clampStep(0.0, headingDelta, kHeadingRateDegreesPerSecond * deltaSeconds));
      const double targetSpeed = qMax(target.speedKnots, entity.currentTask.targetSpeedKnots);
      entity.speedKnots = clampStep(
          entity.speedKnots,
          rangeMeters > 1500.0 ? targetSpeed + 40.0 : targetSpeed,
          kAccelerationKnotsPerSecond * deltaSeconds);
      const double altitudeTarget = entity.currentTask.targetAltitudeMeters > 0
          ? entity.currentTask.targetAltitudeMeters
          : target.altitude;
      entity.verticalSpeedMetersPerSecond = clampStep(
          0.0,
          altitudeTarget - entity.altitude,
          kClimbRateMetersPerSecond);
      entity.currentTask.status = QStringLiteral("Following");
      return;
    }

    entity.currentTask.status = QStringLiteral("Target unavailable");
  }
}

void updateTaskCompletion(Entity& entity) {
  if (!entity.currentTask.enabled) {
    return;
  }

  if (entity.currentTask.taskType == QStringLiteral("FlyHeadingAltitudeSpeed")) {
    const bool headingReached =
        qAbs(shortestSignedAngle(entity.headingDegrees, entity.currentTask.targetHeadingDegrees)) < 1.0;
    const bool speedReached =
        qAbs(entity.speedKnots - entity.currentTask.targetSpeedKnots) < 2.0;
    const bool altitudeReached =
        qAbs(entity.altitude - entity.currentTask.targetAltitudeMeters) < 25.0;
    if (headingReached && speedReached && altitudeReached) {
      entity.currentTask.status = QStringLiteral("On target");
      entity.verticalSpeedMetersPerSecond = 0.0;
    }
  }
}

#if defined(QTTEST_HAS_JSBSIM)
struct JsbsimSession {
  std::unique_ptr<JSBSim::FGFDMExec> exec;
  QString modelName;
};

QHash<QString, JsbsimSession*>& jsbsimSessions() {
  static QHash<QString, JsbsimSession*> sessions;
  return sessions;
}

bool ensureJsbsimSession(Entity& entity, double deltaSeconds) {
  const QString modelName = defaultJsbsimAircraftModel(entity);
  entity.jsbsimAircraftModel = modelName;

  auto& sessions = jsbsimSessions();
  JsbsimSession*& session = sessions[entity.name];
  if (!session) {
    session = new JsbsimSession();
  }
  if (session->exec && session->modelName == modelName) {
    session->exec->Setdt(deltaSeconds);
    return true;
  }

#ifdef QTTEST_SOURCE_DIR
  const QString rootPath = QDir(QString::fromUtf8(QTTEST_SOURCE_DIR))
                               .absoluteFilePath(QStringLiteral("Dependencies/jsbsim"));
#else
  const QString rootPath = QDir(QDir::currentPath()).absoluteFilePath(QStringLiteral("Dependencies/jsbsim"));
#endif

  auto exec = std::make_unique<JSBSim::FGFDMExec>();
  exec->SetRootDir(SGPath(rootPath.toStdString()));
  exec->Setdt(deltaSeconds);
  if (!exec->LoadModel(modelName.toStdString(), true)) {
    return false;
  }

  exec->SetPropertyValue("ic/lat-geod-deg", entity.latitude);
  exec->SetPropertyValue("ic/long-gc-deg", entity.longitude);
  exec->SetPropertyValue("ic/h-sl-ft", entity.altitude * kMetersToFeet);
  exec->SetPropertyValue("ic/psi-true-deg", entity.headingDegrees);
  exec->SetPropertyValue("ic/vc-kts", qMax(0.0, entity.speedKnots));
  exec->SetPropertyValue("ic/gamma-deg", 0.0);
  if (!exec->RunIC()) {
    return false;
  }

  session->modelName = modelName;
  session->exec = std::move(exec);
  return true;
}

bool applyJsbsimStep(Entity& entity, double deltaSeconds) {
  if (!ensureJsbsimSession(entity, deltaSeconds)) {
    return false;
  }

  auto& sessions = jsbsimSessions();
  auto sessionIt = sessions.find(entity.name);
  if (sessionIt == sessions.end() || !sessionIt.value() || !sessionIt.value()->exec) {
    return false;
  }
  JsbsimSession* session = sessionIt.value();

  session->exec->Setdt(deltaSeconds);
  session->exec->SetPropertyValue("ic/lat-geod-deg", entity.latitude);
  session->exec->SetPropertyValue("ic/long-gc-deg", entity.longitude);
  session->exec->SetPropertyValue("ic/h-sl-ft", entity.altitude * kMetersToFeet);
  session->exec->SetPropertyValue("ic/psi-true-deg", entity.headingDegrees);
  session->exec->SetPropertyValue("ic/vc-kts", qMax(0.0, entity.speedKnots));
  session->exec->SetPropertyValue(
      "ic/gamma-deg",
      qRadiansToDegrees(qAtan2(entity.verticalSpeedMetersPerSecond,
                               qMax(1.0, entity.speedKnots * kKnotsToMetersPerSecond))));
  if (!session->exec->RunIC()) {
    return false;
  }
  if (!session->exec->Run()) {
    return false;
  }

  entity.latitude = session->exec->GetPropertyValue("position/lat-geod-deg");
  entity.longitude = session->exec->GetPropertyValue("position/long-gc-deg");
  entity.altitude = qMax(
      0,
      static_cast<int>(qRound(session->exec->GetPropertyValue("position/h-sl-ft") * kFeetToMeters)));
  entity.headingDegrees = normalizeDegrees360(session->exec->GetPropertyValue("attitude/psi-deg"));
  entity.speedKnots = qMax(
      0.0,
      session->exec->GetPropertyValue("velocities/vtrue-kts"));
  entity.verticalSpeedMetersPerSecond =
      session->exec->GetPropertyValue("velocities/h-dot-fps") * kFeetToMeters;
  return true;
}
#endif

} // namespace

void FlightDynamicsEngine::advanceEntities(QVector<Entity>& entities, double deltaSeconds) {
  if (deltaSeconds <= 0.0) {
    return;
  }

  const QVector<Entity> snapshot = entities;
  for (Entity& entity : entities) {
    advanceEntity(entity, snapshot, deltaSeconds);
  }
}

void FlightDynamicsEngine::advanceEntity(
    Entity& entity,
    const QVector<Entity>& snapshot,
    double deltaSeconds) {
  if (!entity.flightDynamicsEnabled && !entity.currentTask.enabled) {
    return;
  }

  resolveTaskTargets(entity, snapshot, deltaSeconds);

#if defined(QTTEST_HAS_JSBSIM)
  if (entity.flightDynamicsMode == QStringLiteral("jsbsim") &&
      applyJsbsimStep(entity, deltaSeconds)) {
    updateTaskCompletion(entity);
    return;
  }
#endif

  applyKinematicStep(entity, deltaSeconds);
  updateTaskCompletion(entity);
}
