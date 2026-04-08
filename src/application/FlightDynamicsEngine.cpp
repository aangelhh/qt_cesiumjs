#include "application/FlightDynamicsEngine.h"

#if defined(QTTEST_HAS_JSBSIM)
#include <FGFDMExec.h>
#endif

#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QtMath>

#include <cmath>
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

QPair<double, double> destinationPoint(
    double latitude,
    double longitude,
    double bearingDegreesValue,
    double distanceMetersValue) {
  const double angularDistance = distanceMetersValue / kEarthRadiusMeters;
  const double bearing = qDegreesToRadians(bearingDegreesValue);
  const double lat1 = qDegreesToRadians(latitude);
  const double lon1 = qDegreesToRadians(longitude);

  const double sinLat1 = qSin(lat1);
  const double cosLat1 = qCos(lat1);
  const double sinAngular = qSin(angularDistance);
  const double cosAngular = qCos(angularDistance);

  const double lat2 = qAsin(
      sinLat1 * cosAngular +
      cosLat1 * sinAngular * qCos(bearing));
  const double lon2 = lon1 + qAtan2(
      qSin(bearing) * sinAngular * cosLat1,
      cosAngular - sinLat1 * qSin(lat2));

  return {qRadiansToDegrees(lat2), qRadiansToDegrees(lon2)};
}

QString defaultJsbsimAircraftModel(const Entity& entity) {
  if (!entity.jsbsimAircraftModel.trimmed().isEmpty()) {
    return entity.jsbsimAircraftModel.trimmed();
  }
  return QStringLiteral("c172x");
}

QString findJsbsimRoot() {
#ifdef QTTEST_SOURCE_DIR
    const QString sourceRoot =
        QDir(QString::fromUtf8(QTTEST_SOURCE_DIR)).absoluteFilePath(QStringLiteral("Dependencies/jsbsim"));
    QDir sourceDir(sourceRoot);
    if (sourceDir.exists(QStringLiteral("aircraft")) &&
        sourceDir.exists(QStringLiteral("engine")) &&
        sourceDir.exists(QStringLiteral("systems"))) {
      return sourceDir.absolutePath();
    }
#else
    Q_UNUSED(0);
#endif

    QDir appDir(QCoreApplication::applicationDirPath());
    QDir cursor = appDir;
    for (int depth = 0; depth < 8; ++depth) {
      const QString candidate = cursor.absoluteFilePath(QStringLiteral("Dependencies/jsbsim"));
      QDir candidateDir(candidate);
      if (candidateDir.exists(QStringLiteral("aircraft")) &&
          candidateDir.exists(QStringLiteral("engine")) &&
          candidateDir.exists(QStringLiteral("systems"))) {
        return candidateDir.absolutePath();
      }
      if (!cursor.cdUp()) {
        break;
      }
    }
    return QString();
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

void resolveTaskTargets(Entity& entity, std::unordered_map<QString, domain::TaskStack>& taskStacks, const QVector<Entity>& snapshot, double deltaSeconds) {
  if (!entity.currentTask.enabled) {
    return;
  }
  
  auto it = taskStacks.find(entity.name);
  if (it != taskStacks.end() && !it->second.isEmpty()) {
      // Feed external world data into specific task types before evaluation
      domain::ITask* topTask = it->second.top();
      if (auto* followTask = dynamic_cast<domain::FollowEntityTask*>(topTask)) {
          for (const Entity& target : snapshot) {
              if (target.name == entity.currentTask.targetEntityName) {
                  followTask->updateTargetLocation(target.latitude, target.longitude, static_cast<double>(target.altitude), target.speedKnots);
                  break;
              }
          }
      }
  
      domain::DesiredState desired = it->second.evaluateTop(
          entity.latitude, entity.longitude, static_cast<double>(entity.altitude), 
          entity.headingDegrees, deltaSeconds);
      
      entity.currentTask.targetHeadingDegrees = desired.targetHeadingDegrees;
      entity.currentTask.targetAltitudeMeters = static_cast<int>(desired.targetAltitudeMeters);
      entity.currentTask.targetSpeedKnots = desired.targetSpeedKnots;
      
      if (it->second.top()->getState() == domain::ITask::State::Completed) {
          entity.currentTask.status = QStringLiteral("On target");
      } else if (it->second.top()->getState() == domain::ITask::State::Failed) {
          entity.currentTask.status = QStringLiteral("Target unavailable");
      } else {
          entity.currentTask.status = QStringLiteral("Running");
      }
      
      // Compute vertical speed for kinematics
      const double targetAltitude = static_cast<double>(entity.currentTask.targetAltitudeMeters);
      const double altitudeDelta = targetAltitude - static_cast<double>(entity.altitude);
      entity.verticalSpeedMetersPerSecond = clampStep(
          0.0, altitudeDelta, kClimbRateMetersPerSecond);
          
      // Compute heading/speed interpolation for kinematics
      const double headingDelta = shortestSignedAngle(
          entity.headingDegrees, entity.currentTask.targetHeadingDegrees);
      entity.headingDegrees = normalizeDegrees360(
          entity.headingDegrees + clampStep(0.0, headingDelta, kHeadingRateDegreesPerSecond * deltaSeconds));
          
      entity.speedKnots = clampStep(
          entity.speedKnots, entity.currentTask.targetSpeedKnots, kAccelerationKnotsPerSecond * deltaSeconds);
          
      if (entity.currentTask.status == QStringLiteral("On target")) {
          entity.speedKnots = 0.0;
          entity.verticalSpeedMetersPerSecond = 0.0;
      }
      return;
  }

  if (entity.currentTask.taskType == QStringLiteral("MoveToLocation") ||
      entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") ||
      entity.currentTask.taskType == QStringLiteral("MoveAlongRoute")) {
    entity.currentTask.targetHeadingDegrees = bearingDegrees(
        entity.latitude,
        entity.longitude,
        entity.currentTask.targetLatitude,
        entity.currentTask.targetLongitude);

    const double distance = distanceMeters(
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
        static_cast<double>(entity.currentTask.targetAltitudeMeters) -
        static_cast<double>(entity.altitude);
    entity.verticalSpeedMetersPerSecond = clampStep(
        0.0,
        altitudeDelta,
        kClimbRateMetersPerSecond);

    entity.currentTask.status = QStringLiteral("Running");
    if (distance < 200.0) {
      entity.currentTask.status = QStringLiteral("On target");
      entity.speedKnots = 0.0;
      entity.verticalSpeedMetersPerSecond = 0.0;
    }
    return;
  }
}

#if defined(QTTEST_HAS_JSBSIM)
enum class JsbsimControlMode {
  NativeAp,
  DirectFcs,
};

struct AircraftState {
  double latitudeDeg = 0.0;
  double longitudeDeg = 0.0;
  double altitudeMeters = 0.0;
  double headingDeg = 0.0;
  double bankRad = 0.0;
  double trueAirspeedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
};

struct ControlCommands {
  double aileronNorm = 0.0;
  double elevatorNorm = 0.0;
  double rudderNorm = 0.0;
  double throttleNorm = 0.5;
};

struct JsbsimSession {
  std::unique_ptr<JSBSim::FGFDMExec> exec;
  QString modelName;
  JsbsimControlMode controlMode = JsbsimControlMode::DirectFcs;
};

QHash<QString, JsbsimSession*>& jsbsimSessions() {
  static QHash<QString, JsbsimSession*> sessions;
  return sessions;
}

bool jsbsimHasProperty(JSBSim::FGFDMExec& exec, const std::string& propertyName) {
  for (const std::string& catalogEntry : exec.GetPropertyCatalog()) {
    if (catalogEntry.rfind(propertyName, 0) == 0) {
      return true;
    }
  }
  return false;
}

JsbsimControlMode detectJsbsimControlMode(JSBSim::FGFDMExec& exec) {
  const bool hasHeadingHold = jsbsimHasProperty(exec, "ap/heading_hold");
  const bool hasHeadingSetpoint = jsbsimHasProperty(exec, "ap/heading_setpoint");
  const bool hasAltitudeHold = jsbsimHasProperty(exec, "ap/altitude_hold");
  const bool hasAltitudeSetpoint = jsbsimHasProperty(exec, "ap/altitude_setpoint");
  if (hasHeadingHold && hasHeadingSetpoint && hasAltitudeHold && hasAltitudeSetpoint) {
    return JsbsimControlMode::NativeAp;
  }
  return JsbsimControlMode::DirectFcs;
}

double clampNormalizedCommand(double value) {
  return qBound(-1.0, value, 1.0);
}

double clampNormalizedThrottle(double value) {
  return qBound(0.0, value, 1.0);
}

bool readJsbsimAircraftState(JSBSim::FGFDMExec& exec, AircraftState& state) {
  const double latitudeDeg = exec.GetPropertyValue("position/lat-geod-deg");
  const double longitudeDeg = exec.GetPropertyValue("position/long-gc-deg");
  const double altitudeFeet = exec.GetPropertyValue("position/h-sl-ft");
  const double headingDeg = exec.GetPropertyValue("attitude/psi-deg");
  const double bankRad = exec.GetPropertyValue("attitude/phi-rad");
  const double trueAirspeedKnots = exec.GetPropertyValue("velocities/vtrue-kts");
  const double verticalSpeedFeetPerSecond = exec.GetPropertyValue("velocities/h-dot-fps");

  if (!std::isfinite(latitudeDeg) ||
      !std::isfinite(longitudeDeg) ||
      !std::isfinite(altitudeFeet) ||
      !std::isfinite(headingDeg) ||
      !std::isfinite(bankRad) ||
      !std::isfinite(trueAirspeedKnots) ||
      !std::isfinite(verticalSpeedFeetPerSecond)) {
    return false;
  }

  state.latitudeDeg = latitudeDeg;
  state.longitudeDeg = longitudeDeg;
  state.altitudeMeters = altitudeFeet * kFeetToMeters;
  state.headingDeg = normalizeDegrees360(headingDeg);
  state.bankRad = bankRad;
  state.trueAirspeedKnots = trueAirspeedKnots;
  state.verticalSpeedMetersPerSecond = verticalSpeedFeetPerSecond * kFeetToMeters;
  return true;
}

ControlCommands computeControlCommands(
    const AircraftState& aircraftState,
    const EntityTask& task) {
  ControlCommands commands;

  const double headingErrorDeg = shortestSignedAngle(
      aircraftState.headingDeg,
      task.targetHeadingDegrees);
  const double bankTargetDeg = qBound(-25.0, headingErrorDeg * 0.35, 25.0);
  const double currentBankDeg = qRadiansToDegrees(aircraftState.bankRad);
  const double bankErrorDeg = bankTargetDeg - currentBankDeg;
  const double altitudeErrorMeters =
      static_cast<double>(task.targetAltitudeMeters) - aircraftState.altitudeMeters;
  const double speedErrorKnots =
      task.targetSpeedKnots - aircraftState.trueAirspeedKnots;

  // Conservative initial gains. Elevator sign may need model-specific tuning.
  constexpr double kBankToAileron = 0.04;
  constexpr double kHeadingToRudder = 0.0015;
  constexpr double kAltitudeToElevator = -0.0008;
  constexpr double kSpeedToThrottle = 0.004;

  commands.aileronNorm = clampNormalizedCommand(bankErrorDeg * kBankToAileron);
  commands.rudderNorm = clampNormalizedCommand(headingErrorDeg * kHeadingToRudder);
  commands.elevatorNorm =
      clampNormalizedCommand(altitudeErrorMeters * kAltitudeToElevator);
  commands.throttleNorm =
      clampNormalizedThrottle(0.5 + speedErrorKnots * kSpeedToThrottle);
  return commands;
}

void applyControlCommands(JSBSim::FGFDMExec& exec, const ControlCommands& commands) {
  exec.SetPropertyValue("fcs/aileron-cmd-norm", commands.aileronNorm);
  exec.SetPropertyValue("fcs/rudder-cmd-norm", commands.rudderNorm);
  exec.SetPropertyValue("fcs/elevator-cmd-norm", commands.elevatorNorm);
  exec.SetPropertyValue("fcs/throttle-cmd-norm", commands.throttleNorm);
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

  const QString rootPath = findJsbsimRoot();
  const QString aircraftPath = QDir(rootPath).absoluteFilePath(QStringLiteral("aircraft"));
  const QString enginePath = QDir(rootPath).absoluteFilePath(QStringLiteral("engine"));
  const QString systemsPath = QDir(rootPath).absoluteFilePath(QStringLiteral("systems"));
  qDebug() << "JSBSim root:" << rootPath << "model:" << modelName;

  auto exec = std::make_unique<JSBSim::FGFDMExec>();
  exec->SetRootDir(SGPath(rootPath.toStdString()));
  exec->Setdt(deltaSeconds);
  if (!exec->LoadModel(
          SGPath(aircraftPath.toStdString()),
          SGPath(enginePath.toStdString()),
          SGPath(systemsPath.toStdString()),
          modelName.toStdString(),
          true)) {
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
  session->controlMode = detectJsbsimControlMode(*exec);
  qDebug() << "JSBSim control mode:"
           << (session->controlMode == JsbsimControlMode::NativeAp ? "NativeAp" : "DirectFcs");
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

  const bool preferDirectFcs =
      entity.currentTask.taskType == QStringLiteral("MoveToLocation") ||
      entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") ||
      entity.currentTask.taskType == QStringLiteral("FlyHeadingAltitudeSpeed");
  const JsbsimControlMode activeControlMode =
      preferDirectFcs ? JsbsimControlMode::DirectFcs : session->controlMode;

  if (activeControlMode == JsbsimControlMode::NativeAp) {
    if (entity.currentTask.enabled && entity.currentTask.status == QStringLiteral("Running")) {
      session->exec->SetPropertyValue("ap/heading_setpoint", entity.currentTask.targetHeadingDegrees);
      session->exec->SetPropertyValue("ap/heading_hold", 1.0);
      session->exec->SetPropertyValue("ap/altitude_setpoint", entity.currentTask.targetAltitudeMeters * kMetersToFeet);
      session->exec->SetPropertyValue("ap/altitude_hold", 1.0);
    } else {
      session->exec->SetPropertyValue("ap/heading_hold", 0.0);
      session->exec->SetPropertyValue("ap/altitude_hold", 0.0);
    }
  } else {
    AircraftState aircraftState;
    if (!readJsbsimAircraftState(*session->exec, aircraftState)) {
      return false;
    }
    const ControlCommands commands =
        computeControlCommands(aircraftState, entity.currentTask);
    applyControlCommands(*session->exec, commands);
  }

  if (!session->exec->Run()) {
    return false;
  }

  AircraftState aircraftState;
  if (!readJsbsimAircraftState(*session->exec, aircraftState)) {
    return false;
  }

  entity.latitude = aircraftState.latitudeDeg;
  entity.longitude = aircraftState.longitudeDeg;
  entity.altitude = qMax(
      0,
      static_cast<int>(qRound(aircraftState.altitudeMeters)));
  entity.headingDegrees = aircraftState.headingDeg;
  entity.speedKnots = qMax(0.0, aircraftState.trueAirspeedKnots);
  entity.verticalSpeedMetersPerSecond =
      aircraftState.verticalSpeedMetersPerSecond;
  return true;
}
#endif

} // namespace

void FlightDynamicsEngine::advanceEntities(QVector<Entity>& entities, std::unordered_map<QString, domain::TaskStack>& taskStacks, double deltaSeconds) {
  if (deltaSeconds <= 0.0) {
    return;
  }

  const QVector<Entity> snapshot = entities;
  for (Entity& entity : entities) {
    advanceEntity(entity, taskStacks, snapshot, deltaSeconds);
  }
}

void FlightDynamicsEngine::advanceEntity(
    Entity& entity,
    std::unordered_map<QString, domain::TaskStack>& taskStacks,
    const QVector<Entity>& snapshot,
    double deltaSeconds) {
  // An entity must only move when it has an active task.
  // flightDynamicsEnabled / flightDynamicsMode only control how movement is simulated,
  // not whether the entity should move at all.
  if (!entity.currentTask.enabled) {
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    return;
  }

  resolveTaskTargets(entity, taskStacks, snapshot, deltaSeconds);

#if defined(QTTEST_HAS_JSBSIM)
  const bool preferKinematicGuidance =
      entity.currentTask.taskType == QStringLiteral("MoveToLocation") ||
      entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") ||
      entity.currentTask.taskType == QStringLiteral("MoveAlongRoute") ||
      entity.currentTask.taskType == QStringLiteral("PatrolArea") ||
      entity.currentTask.taskType == QStringLiteral("OrbitArea") ||
      entity.currentTask.taskType == QStringLiteral("FollowEntity");
  if (!preferKinematicGuidance &&
      entity.flightDynamicsMode == QStringLiteral("jsbsim") &&
      applyJsbsimStep(entity, deltaSeconds)) {
    return;
  }
#endif

  applyKinematicStep(entity, deltaSeconds);
}
