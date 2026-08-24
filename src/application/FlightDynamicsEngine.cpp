#include "application/FlightDynamicsEngine.h"
#include "application/MovementIntent.h"
#include "application/dynamics/KinematicDynamicsModel.h"
#include "domain/EntityIdentity.h"

#if defined(QTTEST_HAS_JSBSIM)
#include "application/dynamics/JSBSimDynamicsModel.h"
#endif

#include <QtMath>

#include <cmath>
#include <memory>

namespace {

constexpr double kEarthRadiusMeters = 6371000.0;
constexpr double kKnotsToMetersPerSecond = 0.514444;
constexpr double kClimbRateMetersPerSecond = 20.0;
constexpr double kMinimumAttitudeSpeedMetersPerSecond = 5.0;
constexpr double kPitchResponseDegreesPerSecond = 18.0;
constexpr double kRollResponseDegreesPerSecond = 45.0;
constexpr double kRollLevelResponseDegreesPerSecond = 120.0;
constexpr double kMaxPitchDegrees = 10.0;
constexpr double kMaxRollDegrees = 30.0;
constexpr double kGravityMetersPerSecondSquared = 9.81;
constexpr double kClimbPitchBiasDegrees = 1.5;
constexpr double kSettledYawRateDegreesPerSecond = 0.5;
constexpr double kSettledHeadingErrorDegrees = 1.0;
constexpr int kAltitudeCaptureToleranceMeters = 50;

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

double smoothAttitudeDegrees(
    double currentDegrees,
    double targetDegrees,
    double maxAbsDegrees,
    double responseDegreesPerSecond,
    double deltaSeconds) {
  const double limitedTargetDegrees = qBound(-maxAbsDegrees, targetDegrees, maxAbsDegrees);
  return clampStep(
      currentDegrees,
      limitedTargetDegrees,
      responseDegreesPerSecond * deltaSeconds);
}

void relaxDerivedAttitude(Entity& entity, double deltaSeconds) {
  entity.pitchDegrees = smoothAttitudeDegrees(
      entity.pitchDegrees,
      0.0,
      kMaxPitchDegrees,
      kPitchResponseDegreesPerSecond,
      deltaSeconds);
  entity.rollDegrees = smoothAttitudeDegrees(
      entity.rollDegrees,
      0.0,
      kMaxRollDegrees,
      kRollResponseDegreesPerSecond,
      deltaSeconds);
}

bool entityIsGround(const Entity& entity) {
  return entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
}

bool isInterceptEntityTaskType(const QString& taskType) {
  return taskType == QStringLiteral("InterceptEntity") ||
         taskType == QStringLiteral("InterceptEntity2D") ||
         taskType == QStringLiteral("InterceptEntity3D");
}

bool isMovementTaskType(const QString& taskType) {
  return taskType == QStringLiteral("MoveToLocation") ||
         taskType == QStringLiteral("WaitOnLocation") ||
         taskType == QStringLiteral("MoveToWaypoint") ||
         taskType == QStringLiteral("MoveAlongRoute") ||
         taskType == QStringLiteral("FollowRoute") ||
         taskType == QStringLiteral("PatrolArea") ||
         taskType == QStringLiteral("OrbitArea") ||
         taskType == QStringLiteral("HoldRacetrack") ||
         taskType == QStringLiteral("FollowEntity") ||
         isInterceptEntityTaskType(taskType) ||
         taskType == QStringLiteral("FlyHeadingAltitudeSpeed") ||
         taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackUntilDestroyed");
}

bool isGroundMovementTaskType(const QString& taskType) {
  return taskType == QStringLiteral("MoveToLocation") ||
         taskType == QStringLiteral("WaitOnLocation") ||
         taskType == QStringLiteral("MoveToWaypoint") ||
         taskType == QStringLiteral("MoveAlongRoute") ||
         taskType == QStringLiteral("FollowRoute") ||
         taskType == QStringLiteral("PatrolArea") ||
         taskType == QStringLiteral("OrbitArea") ||
         taskType == QStringLiteral("HoldRacetrack") ||
         taskType == QStringLiteral("FollowEntity") ||
         isInterceptEntityTaskType(taskType);
}

bool isRouteTaskType(const QString& taskType) {
  return taskType == QStringLiteral("MoveAlongRoute") ||
         taskType == QStringLiteral("FollowRoute");
}

bool taskStatusIsTerminal(const QString& status) {
  return status == QStringLiteral("Completed") ||
         status == QStringLiteral("Failed") ||
         status == QStringLiteral("Target unavailable");
}

void stopForFuelExhaustion(Entity& entity, double deltaSeconds) {
  entity.fuelRemainingKilograms = 0.0;
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 0.0;
  entity.activeDynamicsBackend = QStringLiteral("fuel-exhausted");
  if (entity.currentTask.enabled &&
      !taskStatusIsTerminal(entity.currentTask.status)) {
    entity.currentTask.status = QStringLiteral("Failed");
  }
  relaxDerivedAttitude(entity, deltaSeconds);
}

bool applyKinematicStep(
    Entity& entity,
    double simulationTimeSeconds,
    double deltaSeconds);

void normalizeGroundKinematics(Entity& entity) {
  entity.verticalSpeedMetersPerSecond = 0.0;
  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
}

void settleCapturedVerticalIntent(Entity& entity) {
  if (qAbs(entity.currentTask.targetAltitudeMeters - entity.altitude) <=
      kAltitudeCaptureToleranceMeters) {
    entity.verticalSpeedMetersPerSecond = 0.0;
  }
}

void updateDerivedKinematicAttitude(
    Entity& entity,
    double previousHeadingDegrees,
    double deltaSeconds) {
  if (deltaSeconds <= 0.0) {
    return;
  }

  if (entityIsGround(entity)) {
    normalizeGroundKinematics(entity);
    return;
  }

  const double horizontalSpeedMetersPerSecond =
      qMax(0.0, entity.speedKnots * kKnotsToMetersPerSecond);
  if (horizontalSpeedMetersPerSecond < kMinimumAttitudeSpeedMetersPerSecond) {
    relaxDerivedAttitude(entity, deltaSeconds);
    return;
  }

  const double effectiveVerticalSpeedMetersPerSecond =
      qAbs(entity.currentTask.targetAltitudeMeters - entity.altitude) <=
              kAltitudeCaptureToleranceMeters
          ? 0.0
          : entity.verticalSpeedMetersPerSecond;
  const double flightPathPitchDegrees = qRadiansToDegrees(qAtan2(
      effectiveVerticalSpeedMetersPerSecond,
      horizontalSpeedMetersPerSecond));
  const double headingStepDegrees =
      shortestSignedAngle(previousHeadingDegrees, entity.headingDegrees);
  const double headingErrorDegrees = shortestSignedAngle(
      entity.headingDegrees,
      entity.currentTask.targetHeadingDegrees);
  const double yawRateRadiansPerSecond =
      qDegreesToRadians(headingStepDegrees) / deltaSeconds;
  const double yawRateDegreesPerSecond = headingStepDegrees / deltaSeconds;
  const double rawRollDegrees = qRadiansToDegrees(qAtan(
      (horizontalSpeedMetersPerSecond * yawRateRadiansPerSecond) /
      kGravityMetersPerSecondSquared));
  const double climbBiasDegrees = qBound(
      -kClimbPitchBiasDegrees,
      (effectiveVerticalSpeedMetersPerSecond / kClimbRateMetersPerSecond) *
          kClimbPitchBiasDegrees,
      kClimbPitchBiasDegrees);
  const double rawPitchDegrees = flightPathPitchDegrees + climbBiasDegrees;
  const bool onTarget = entity.currentTask.status == QStringLiteral("On target");

  double safePitchDegrees = std::isfinite(rawPitchDegrees) ? rawPitchDegrees : 0.0;
  if (qAbs(effectiveVerticalSpeedMetersPerSecond) < 0.5) {
    safePitchDegrees = 0.0;
  }
  double safeRollDegrees = std::isfinite(rawRollDegrees) ? rawRollDegrees : 0.0;
  const bool headingSettled =
      qAbs(yawRateDegreesPerSecond) <= kSettledYawRateDegreesPerSecond ||
      qAbs(headingErrorDegrees) <= kSettledHeadingErrorDegrees;
  if (onTarget || headingSettled) {
    safeRollDegrees = 0.0;
  }
  entity.pitchDegrees = smoothAttitudeDegrees(
      entity.pitchDegrees,
      safePitchDegrees,
      kMaxPitchDegrees,
      kPitchResponseDegreesPerSecond,
      deltaSeconds);
  entity.rollDegrees = smoothAttitudeDegrees(
      entity.rollDegrees,
      safeRollDegrees,
      kMaxRollDegrees,
      qFuzzyIsNull(safeRollDegrees)
          ? kRollLevelResponseDegreesPerSecond
          : kRollResponseDegreesPerSecond,
      deltaSeconds);
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

#if defined(QTTEST_HAS_JSBSIM)
QString defaultJsbsimAircraftModel(const Entity& entity) {
  if (!entity.jsbsimAircraftModel.trimmed().isEmpty()) {
    return entity.jsbsimAircraftModel.trimmed();
  }
  return QStringLiteral("c172x");
}

QString defaultJsbsimControlProfile(const QString& modelName) {
  const QString normalized = modelName.trimmed().toLower();
  if (normalized == QStringLiteral("f16") ||
      normalized == QStringLiteral("f22") ||
      normalized == QStringLiteral("f15") ||
      normalized == QStringLiteral("a4") ||
      normalized == QStringLiteral("t38")) {
    return QStringLiteral("fighter-generic");
  }
  return QStringLiteral("aircraft-generic");
}
#endif

bool applyKinematicStep(
    Entity& entity,
    double simulationTimeSeconds,
    double deltaSeconds) {
  application::dynamics::KinematicDynamicsModel model;
  const application::dynamics::DynamicsModelConfiguration configuration{
      QStringLiteral("kinematic"),
      entity.type,
      entityIsGround(entity),
  };
  const application::dynamics::DynamicsState initialState{
      entity.latitude,
      entity.longitude,
      static_cast<double>(entity.altitude),
      entity.headingDegrees,
      entity.pitchDegrees,
      entity.rollDegrees,
      entity.speedKnots,
      entity.verticalSpeedMetersPerSecond,
  };
  if (!model.configure(configuration) || !model.initialize(initialState)) {
    return false;
  }
  const application::dynamics::DynamicsStepResult result = model.step({
      simulationTimeSeconds,
      deltaSeconds,
      {},
  });
  if (!result) {
    return false;
  }

  const application::dynamics::DynamicsState nextState = model.state();
  entity.latitude = nextState.latitudeDegrees;
  entity.longitude = nextState.longitudeDegrees;
  entity.altitude = qMax(0, static_cast<int>(qRound(nextState.altitudeMeters)));
  entity.headingDegrees = nextState.headingDegrees;
  entity.pitchDegrees = nextState.pitchDegrees;
  entity.rollDegrees = nextState.rollDegrees;
  entity.speedKnots = nextState.speedKnots;
  entity.verticalSpeedMetersPerSecond =
      nextState.verticalSpeedMetersPerSecond;
  return true;
}

void resolveTaskTargets(Entity& entity, std::unordered_map<QString, domain::TaskStack>& taskStacks, const QVector<Entity>& snapshot, double deltaSeconds) {
  if (!entity.currentTask.enabled) {
    return;
  }

  if (taskStatusIsTerminal(entity.currentTask.status)) {
    return;
  }

  if (!isMovementTaskType(entity.currentTask.taskType)) {
    return;
  }
  
  auto it = taskStacks.find(domain::entityKey(entity));
  if (it == taskStacks.end() && domain::entityKey(entity) != entity.name) {
    it = taskStacks.find(entity.name);
  }
  if (it != taskStacks.end() && !it->second.isEmpty()) {
      // Feed external world data into specific task types before evaluation
      domain::ITask* topTask = it->second.top();
      auto findActiveTarget = [&]() -> const Entity* {
          const QString targetReference =
              domain::targetEntityReference(entity.currentTask);
          for (const Entity& target : snapshot) {
              if (domain::entityMatchesReference(target, targetReference) && !target.destroyed) {
                  return &target;
              }
          }
          return nullptr;
      };
      if (entity.currentTask.taskType == QStringLiteral("FollowEntity")) {
          auto* followTask = static_cast<domain::FollowEntityTask*>(topTask);
          const Entity* targetEntity = findActiveTarget();
          if (!targetEntity) {
              entity.currentTask.status = QStringLiteral("Target unavailable");
              entity.speedKnots = 0.0;
              entity.verticalSpeedMetersPerSecond = 0.0;
              return;
          }
          entity.currentTask.targetLatitude = targetEntity->latitude;
          entity.currentTask.targetLongitude = targetEntity->longitude;
          followTask->updateTargetLocation(
              targetEntity->latitude,
              targetEntity->longitude,
              static_cast<double>(targetEntity->altitude),
              targetEntity->speedKnots);
      }
      if (isInterceptEntityTaskType(entity.currentTask.taskType)) {
          auto* interceptTask = static_cast<domain::InterceptEntity3DTask*>(topTask);
          const Entity* targetEntity = findActiveTarget();
          if (!targetEntity) {
              entity.currentTask.status = QStringLiteral("Target unavailable");
              entity.speedKnots = 0.0;
              entity.verticalSpeedMetersPerSecond = 0.0;
              return;
          }
          const bool altitudeApplies =
              !entityIsGround(entity) && !entityIsGround(*targetEntity);
          const double interceptAltitude = altitudeApplies
              ? static_cast<double>(targetEntity->altitude)
              : static_cast<double>(entity.altitude);
          entity.currentTask.targetLatitude = targetEntity->latitude;
          entity.currentTask.targetLongitude = targetEntity->longitude;
          entity.currentTask.targetAltitudeMeters = static_cast<int>(interceptAltitude);
          interceptTask->updateTargetLocation(
              targetEntity->latitude,
              targetEntity->longitude,
              interceptAltitude);
      }
  
      domain::ITask::State evaluatedState = domain::ITask::State::Running;
      domain::DesiredState desired = it->second.evaluateTop(
          entity.latitude,
          entity.longitude,
          static_cast<double>(entity.altitude),
          entity.headingDegrees,
          deltaSeconds,
          &evaluatedState);
      
      entity.currentTask.targetHeadingDegrees = desired.targetHeadingDegrees;
      entity.currentTask.targetAltitudeMeters = static_cast<int>(desired.targetAltitudeMeters);
      entity.currentTask.targetSpeedKnots = desired.targetSpeedKnots;
      if (isRouteTaskType(entity.currentTask.taskType)) {
          auto* routeTask = static_cast<domain::RouteTask*>(topTask);
          const RoutePoint targetPoint = routeTask->currentTargetPoint();
          entity.currentTask.targetLatitude = targetPoint.latitude;
          entity.currentTask.targetLongitude = targetPoint.longitude;
          entity.currentTask.routeTotalWaypoints = routeTask->totalPoints();
          entity.currentTask.routeCurrentWaypointIndex = routeTask->totalPoints() > 0
              ? qMin(routeTask->currentPointIndex() + 1, routeTask->totalPoints())
              : 0;
      }
      if (entity.currentTask.taskType == QStringLiteral("WaitOnLocation")) {
          auto* waitTask = static_cast<domain::WaitOnLocationTask*>(topTask);
          if (waitTask->hasArrived() && entity.currentTask.durationSeconds > 0.0) {
              entity.currentTask.elapsedSeconds += qMax(0.0, deltaSeconds);
              if (entity.currentTask.elapsedSeconds >= entity.currentTask.durationSeconds) {
                  evaluatedState = domain::ITask::State::Completed;
              }
          }
      }
      
      if (entity.currentTask.taskType == QStringLiteral("FollowEntity") &&
          entity.currentTask.durationSeconds > 0.0) {
          entity.currentTask.elapsedSeconds += qMax(0.0, deltaSeconds);
          if (entity.currentTask.elapsedSeconds >= entity.currentTask.durationSeconds) {
              evaluatedState = domain::ITask::State::Completed;
          }
      }
      if (isInterceptEntityTaskType(entity.currentTask.taskType) &&
          entity.currentTask.timeoutSeconds > 0.0 &&
          evaluatedState != domain::ITask::State::Completed) {
          entity.currentTask.elapsedSeconds += qMax(0.0, deltaSeconds);
          if (entity.currentTask.elapsedSeconds >= entity.currentTask.timeoutSeconds) {
              evaluatedState = domain::ITask::State::Failed;
          }
      }
      if (isRouteTaskType(entity.currentTask.taskType) &&
          entity.currentTask.timeoutSeconds > 0.0 &&
          evaluatedState != domain::ITask::State::Completed) {
          entity.currentTask.elapsedSeconds += qMax(0.0, deltaSeconds);
          if (entity.currentTask.elapsedSeconds >= entity.currentTask.timeoutSeconds) {
              evaluatedState = domain::ITask::State::Failed;
          }
      }
      if (entity.currentTask.taskType == QStringLiteral("HoldRacetrack") &&
          entity.currentTask.durationSeconds > 0.0) {
          entity.currentTask.elapsedSeconds += qMax(0.0, deltaSeconds);
          if (entity.currentTask.elapsedSeconds >= entity.currentTask.durationSeconds) {
              evaluatedState = domain::ITask::State::Completed;
          }
      }

      if (evaluatedState == domain::ITask::State::Completed) {
          entity.currentTask.status =
              (entity.currentTask.taskType == QStringLiteral("FollowEntity") ||
               isInterceptEntityTaskType(entity.currentTask.taskType) ||
               isRouteTaskType(entity.currentTask.taskType) ||
               entity.currentTask.taskType == QStringLiteral("HoldRacetrack") ||
               entity.currentTask.taskType == QStringLiteral("WaitOnLocation"))
              ? QStringLiteral("Completed")
              : QStringLiteral("On target");
      } else if (evaluatedState == domain::ITask::State::Failed) {
          entity.currentTask.status =
              (isInterceptEntityTaskType(entity.currentTask.taskType) ||
               isRouteTaskType(entity.currentTask.taskType))
              ? QStringLiteral("Failed")
              : QStringLiteral("Target unavailable");
      } else {
          entity.currentTask.status = QStringLiteral("Running");
      }

      application::applyMovementIntent(
          entity,
          application::movementIntentFromTask(entity.currentTask),
          deltaSeconds,
          application::movementControllerLimitsForEntity(entity));
          
      if (entity.currentTask.status == QStringLiteral("On target") ||
          entity.currentTask.status == QStringLiteral("Completed")) {
          application::stopMovementIntent(entity);
      }
      return;
  }

  const bool isAttackAirTask =
      entity.currentTask.taskType == QStringLiteral("AttackAir") ||
      entity.currentTask.taskType == QStringLiteral("AttackUntilDestroyed");
  if (entity.currentTask.taskType == QStringLiteral("MoveToLocation") ||
      entity.currentTask.taskType == QStringLiteral("WaitOnLocation") ||
      entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") ||
      entity.currentTask.taskType == QStringLiteral("MoveAlongRoute") ||
      entity.currentTask.taskType == QStringLiteral("FollowRoute") ||
      isAttackAirTask) {
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

    application::applyMovementIntent(
        entity,
        application::movementIntentFromTask(entity.currentTask),
        deltaSeconds,
        application::movementControllerLimitsForEntity(entity));

    entity.currentTask.status = QStringLiteral("Running");
    if (!isAttackAirTask && distance < 200.0) {
      entity.currentTask.status = QStringLiteral("On target");
      application::stopMovementIntent(entity);
    }
    return;
  }
}

#if defined(QTTEST_HAS_JSBSIM)
using JsbsimModelRegistry = std::unordered_map<
    QString,
    std::unique_ptr<application::dynamics::JSBSimDynamicsModel>>;

std::unordered_map<
    QString,
    std::unique_ptr<application::dynamics::JSBSimDynamicsModel>>&
jsbsimModels() {
  // JSBSim owns process-level state whose static teardown order is not under
  // our control. Runtime entries are explicitly released by ScenarioState;
  // the registry shell intentionally remains alive until process exit.
  static auto* models = new JsbsimModelRegistry();
  return *models;
}

void releaseJsbsimModel(const QString& entityId) {
  auto& models = jsbsimModels();
  const auto it = models.find(entityId);
  if (it == models.end()) {
    return;
  }
  if (it->second) {
    it->second->shutdown();
  }
  models.erase(it);
}

void clearJsbsimModels() {
  auto& models = jsbsimModels();
  for (auto& [entityId, model] : models) {
    Q_UNUSED(entityId);
    if (model) {
      model->shutdown();
    }
  }
  models.clear();
}

application::dynamics::JSBSimDynamicsModel* ensureJsbsimModel(Entity& entity) {
  const QString modelName = defaultJsbsimAircraftModel(entity);
  entity.jsbsimAircraftModel = modelName;
  if (entity.controlProfileId.trimmed().isEmpty()) {
    entity.controlProfileId = defaultJsbsimControlProfile(modelName);
  }
  application::ensureFuelConfiguration(entity);

  auto& modelPtr = jsbsimModels()[domain::entityKey(entity)];
  const bool needsRebuild =
      !modelPtr || !modelPtr->isInitialized() || modelPtr->loadedModelName() != modelName;
  if (!needsRebuild) {
    return modelPtr.get();
  }

  modelPtr = std::make_unique<application::dynamics::JSBSimDynamicsModel>();
  const application::dynamics::DynamicsModelConfiguration configuration{
      modelName,
      entity.type,
      /*groundConstrained=*/false,
      entity.fuelCapacityKilograms,
  };
  if (!modelPtr->configure(configuration)) {
    modelPtr.reset();
    return nullptr;
  }

  const application::dynamics::DynamicsState initialState{
      entity.latitude,
      entity.longitude,
      static_cast<double>(entity.altitude),
      entity.headingDegrees,
      entity.pitchDegrees,
      entity.rollDegrees,
      entity.speedKnots,
      entity.verticalSpeedMetersPerSecond,
      entity.fuelRemainingKilograms,
      -1.0,
  };
  if (!modelPtr->initialize(initialState)) {
    modelPtr.reset();
    return nullptr;
  }

  const auto initialized = modelPtr->state();
  if (initialized.fuelCapacityKilograms >= 0.0) {
    entity.fuelCapacityKilograms = initialized.fuelCapacityKilograms;
  }
  if (initialized.fuelRemainingKilograms >= 0.0) {
    entity.fuelRemainingKilograms = initialized.fuelRemainingKilograms;
  }
  return modelPtr.get();
}

bool applyJsbsimStep(Entity& entity, double simulationTimeSeconds, double deltaSeconds) {
  auto* model = ensureJsbsimModel(entity);
  if (!model) {
    return false;
  }

  const bool preferDirectFcs =
      entity.currentTask.taskType == QStringLiteral("MoveToLocation") ||
      entity.currentTask.taskType == QStringLiteral("WaitOnLocation") ||
      entity.currentTask.taskType == QStringLiteral("MoveToWaypoint") ||
      entity.currentTask.taskType == QStringLiteral("FlyHeadingAltitudeSpeed");

  application::dynamics::DynamicsStepContext context;
  context.simulationTimeSeconds = simulationTimeSeconds;
  context.deltaTimeSeconds = deltaSeconds;
  context.controlSetpoint.valid =
      entity.currentTask.enabled && entity.currentTask.status == QStringLiteral("Running");
  context.controlSetpoint.preferDirectControl = preferDirectFcs;
  context.controlSetpoint.targetHeadingDegrees = entity.currentTask.targetHeadingDegrees;
  context.controlSetpoint.targetAltitudeMeters = entity.currentTask.targetAltitudeMeters;
  context.controlSetpoint.targetSpeedKnots = entity.currentTask.targetSpeedKnots;
  context.controlSetpoint.controlProfileId = entity.controlProfileId;

  const auto result = model->step(context);
  if (!result) {
    return false;
  }

  const auto next = model->state();
  entity.latitude = next.latitudeDegrees;
  entity.longitude = next.longitudeDegrees;
  entity.altitude = qMax(0, static_cast<int>(qRound(next.altitudeMeters)));
  entity.headingDegrees = next.headingDegrees;
  entity.pitchDegrees = next.pitchDegrees;
  entity.rollDegrees = next.rollDegrees;
  entity.speedKnots = qMax(0.0, next.speedKnots);
  entity.verticalSpeedMetersPerSecond = next.verticalSpeedMetersPerSecond;
  if (next.fuelRemainingKilograms >= 0.0) {
    entity.fuelRemainingKilograms =
        qBound(0.0, next.fuelRemainingKilograms, entity.fuelCapacityKilograms);
  }
  return true;
}
#endif

} // namespace

void FlightDynamicsEngine::advanceEntities(QVector<Entity>& entities, std::unordered_map<QString, domain::TaskStack>& taskStacks, double deltaSeconds) {
  advanceEntities(entities, taskStacks, 0.0, deltaSeconds);
}

void FlightDynamicsEngine::advanceEntities(
    QVector<Entity>& entities,
    std::unordered_map<QString, domain::TaskStack>& taskStacks,
    double simulationTimeSeconds,
    double deltaSeconds) {
  if (deltaSeconds <= 0.0) {
    return;
  }

  const QVector<Entity> snapshot = entities;
  for (Entity& entity : entities) {
    advanceEntity(
        entity,
        taskStacks,
        snapshot,
        simulationTimeSeconds,
        deltaSeconds);
  }
}

void FlightDynamicsEngine::releaseDynamicsModel(const QString& entityId) {
#if defined(QTTEST_HAS_JSBSIM)
  releaseJsbsimModel(entityId);
#else
  Q_UNUSED(entityId);
#endif
}

void FlightDynamicsEngine::clearDynamicsModels() {
#if defined(QTTEST_HAS_JSBSIM)
  clearJsbsimModels();
#endif
}

application::SystemsTelemetrySnapshot
FlightDynamicsEngine::systemsTelemetryForEntity(
    const Entity& entity,
    double maximumSpeedKnots) {
  application::SystemsTelemetrySnapshot snapshot =
      application::makeEstimatedSystemsTelemetrySnapshot(
          entity,
          maximumSpeedKnots);
  const bool configuredForJsbsim =
      entity.flightDynamicsEnabled &&
      entity.flightDynamicsMode.compare(
          QStringLiteral("jsbsim"), Qt::CaseInsensitive) == 0;
  if (configuredForJsbsim &&
      entity.activeDynamicsBackend.compare(
          QStringLiteral("jsbsim"), Qt::CaseInsensitive) != 0) {
    snapshot.dataSource = entity.activeDynamicsBackend.compare(
                              QStringLiteral("kinematic-fallback"),
                              Qt::CaseInsensitive) == 0
        ? QStringLiteral("Kinematic estimate | JSBSim fallback")
        : (entity.activeDynamicsBackend.compare(
               QStringLiteral("fuel-exhausted"),
               Qt::CaseInsensitive) == 0
               ? QStringLiteral("Fuel exhausted")
               : QStringLiteral("Kinematic estimate | JSBSim inactive"));
  }
#if defined(QTTEST_HAS_JSBSIM)
  if (entity.activeDynamicsBackend.compare(
          QStringLiteral("jsbsim"), Qt::CaseInsensitive) != 0) {
    return snapshot;
  }
  const auto modelIt = jsbsimModels().find(domain::entityKey(entity));
  if (modelIt == jsbsimModels().end() || !modelIt->second ||
      !modelIt->second->isInitialized()) {
    return snapshot;
  }

  const auto* model = modelIt->second.get();
  const QVector<application::EngineTelemetry> engines =
      model->engineTelemetry(entity.destroyed);
  if (engines.isEmpty()) {
    return snapshot;
  }

  snapshot.dataSource = QStringLiteral("JSBSim | %1")
                            .arg(model->loadedModelName());
  snapshot.engines = engines;
  application::updateFuelTelemetrySummary(
      snapshot,
      entity.fuelCapacityKilograms,
      entity.fuelRemainingKilograms);
#endif
  return snapshot;
}

bool FlightDynamicsEngine::setFuelRemaining(
    Entity& entity,
    double kilograms) {
  if (entity.domain.compare(
          QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
    return false;
  }
  application::ensureFuelConfiguration(entity);
  if (entity.fuelCapacityKilograms <= 0.0) {
    return false;
  }
  entity.fuelRemainingKilograms = qBound(
      0.0,
      kilograms,
      entity.fuelCapacityKilograms);
#if defined(QTTEST_HAS_JSBSIM)
  const auto modelIt = jsbsimModels().find(domain::entityKey(entity));
  if (modelIt != jsbsimModels().end() && modelIt->second &&
      modelIt->second->isInitialized() &&
      modelIt->second->applyExternalFuelOverride(entity.fuelRemainingKilograms)) {
    const application::dynamics::DynamicsState modelState =
        modelIt->second->state();
    entity.fuelCapacityKilograms = modelState.fuelCapacityKilograms;
    entity.fuelRemainingKilograms = modelState.fuelRemainingKilograms;
  }
#endif
  return true;
}

void FlightDynamicsEngine::advanceEntity(
    Entity& entity,
    std::unordered_map<QString, domain::TaskStack>& taskStacks,
    const QVector<Entity>& snapshot,
    double simulationTimeSeconds,
    double deltaSeconds) {
  application::ensureFuelConfiguration(entity);
  // An entity must only move when it has an active task.
  // flightDynamicsEnabled / flightDynamicsMode only control how movement is simulated,
  // not whether the entity should move at all.
  if (entityIsGround(entity)) {
    entity.activeDynamicsBackend = QStringLiteral("kinematic-ground");
    normalizeGroundKinematics(entity);
    if (!entity.currentTask.enabled) {
      entity.speedKnots = 0.0;
      return;
    }
    if (taskStatusIsTerminal(entity.currentTask.status)) {
      entity.speedKnots = 0.0;
      return;
    }
    if (!isGroundMovementTaskType(entity.currentTask.taskType)) {
      entity.speedKnots = 0.0;
      return;
    }

    resolveTaskTargets(entity, taskStacks, snapshot, deltaSeconds);
    entity.currentTask.targetAltitudeMeters = entity.altitude;
    normalizeGroundKinematics(entity);
    if (entity.speedKnots <= 0.0) {
      return;
    }

    applyKinematicStep(entity, simulationTimeSeconds, deltaSeconds);
    normalizeGroundKinematics(entity);
    return;
  }

  if (!entity.currentTask.enabled) {
    entity.activeDynamicsBackend = QStringLiteral("inactive");
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    relaxDerivedAttitude(entity, deltaSeconds);
    return;
  }
  if (taskStatusIsTerminal(entity.currentTask.status)) {
    entity.activeDynamicsBackend = QStringLiteral("inactive");
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    relaxDerivedAttitude(entity, deltaSeconds);
    return;
  }

  if (entity.fuelCapacityKilograms > 0.0 &&
      entity.fuelRemainingKilograms <= 0.0) {
    stopForFuelExhaustion(entity, deltaSeconds);
    return;
  }

  const double previousHeadingDegrees = entity.headingDegrees;
  if (!isMovementTaskType(entity.currentTask.taskType)) {
    entity.activeDynamicsBackend = QStringLiteral("kinematic");
    entity.verticalSpeedMetersPerSecond = 0.0;
    applyKinematicStep(entity, simulationTimeSeconds, deltaSeconds);
    application::consumeEstimatedFuel(entity, deltaSeconds);
    if (entity.fuelRemainingKilograms <= 0.0) {
      stopForFuelExhaustion(entity, deltaSeconds);
      return;
    }
    updateDerivedKinematicAttitude(entity, previousHeadingDegrees, deltaSeconds);
    return;
  }

  resolveTaskTargets(entity, taskStacks, snapshot, deltaSeconds);

#if defined(QTTEST_HAS_JSBSIM)
  if (entity.flightDynamicsEnabled &&
      entity.flightDynamicsMode.compare(
          QStringLiteral("jsbsim"), Qt::CaseInsensitive) == 0) {
    if (applyJsbsimStep(entity, simulationTimeSeconds, deltaSeconds)) {
      if (entity.fuelRemainingKilograms <= 0.0) {
        stopForFuelExhaustion(entity, deltaSeconds);
        return;
      }
      entity.activeDynamicsBackend = QStringLiteral("jsbsim");
      return;
    }
    entity.activeDynamicsBackend = QStringLiteral("kinematic-fallback");
  } else {
    entity.activeDynamicsBackend = QStringLiteral("kinematic");
  }
#else
  entity.activeDynamicsBackend = QStringLiteral("kinematic");
#endif

  applyKinematicStep(entity, simulationTimeSeconds, deltaSeconds);
  application::consumeEstimatedFuel(entity, deltaSeconds);
  if (entity.fuelRemainingKilograms <= 0.0) {
    stopForFuelExhaustion(entity, deltaSeconds);
    return;
  }
  settleCapturedVerticalIntent(entity);
  updateDerivedKinematicAttitude(entity, previousHeadingDegrees, deltaSeconds);
}
