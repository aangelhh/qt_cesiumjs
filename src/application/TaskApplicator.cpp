#include "application/TaskApplicator.h"
#include "application/Command.h"
#include "application/ScenarioState.h"
#include "application/SimulationEngine.h"
#include "domain/Entity.h"
#include "domain/Task.h"
#include "domain/TacticalGraphic.h"

#include <QtMath>

namespace {

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

bool isRouteTaskType(const QString& taskType) {
  return taskType == QStringLiteral("MoveAlongRoute") ||
         taskType == QStringLiteral("FollowRoute");
}

bool isConditionalWaitTaskType(const QString& taskType) {
  return taskType == QStringLiteral("WaitUntilTargetDetected") ||
         taskType == QStringLiteral("WaitUntilTargetDestroyed") ||
         taskType == QStringLiteral("WaitUntilDamaged") ||
         taskType == QStringLiteral("WaitUntilTime") ||
         taskType == QStringLiteral("WaitUntilInRange");
}

bool entityIsGround(const Entity& entity) {
  return entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
}

double defaultRouteArrivalToleranceMeters(const Entity& entity) {
  return entityIsGround(entity) ? 500.0 : 1000.0;
}

double waypointAltitudeForEntity(const Waypoint& waypoint, const Entity& entity) {
  if (entityIsGround(entity)) {
    return static_cast<double>(entity.altitude);
  }
  return waypoint.altitudeMetersSet
      ? waypoint.altitudeMeters
      : static_cast<double>(entity.altitude);
}

QVector<RoutePoint> routePointsForEntity(const RouteGraphic& route, const Entity& entity) {
  QVector<RoutePoint> points;
  points.reserve(route.points.size());
  double fallbackAltitudeMeters = static_cast<double>(entity.altitude);
  for (RoutePoint point : route.points) {
    if (entityIsGround(entity)) {
      point.altitudeMeters = static_cast<double>(entity.altitude);
      point.altitudeMetersSet = true;
    } else if (!point.altitudeMetersSet) {
      point.altitudeMeters = fallbackAltitudeMeters;
    }
    fallbackAltitudeMeters = point.altitudeMeters;
    points.push_back(point);
  }
  return points;
}

} // namespace

namespace application {

void resolveTaskCoordinates(
    Entity&                        entity,
    const QVector<Waypoint>&       waypoints,
    const QVector<RouteGraphic>&   routes,
    const QVector<AreaDefinition>& areas) {
  EntityTask& task = entity.currentTask;

  if (task.taskType == QStringLiteral("MoveToWaypoint") &&
      !task.targetWaypointName.trimmed().isEmpty()) {
    for (const Waypoint& wp : waypoints) {
      if (wp.name == task.targetWaypointName) {
        task.targetLatitude = wp.latitude;
        task.targetLongitude = wp.longitude;
        task.targetAltitudeMeters = static_cast<int>(qRound(waypointAltitudeForEntity(wp, entity)));
        break;
      }
    }
  }

  if (isRouteTaskType(task.taskType) &&
      !task.targetRouteName.trimmed().isEmpty()) {
    for (const RouteGraphic& route : routes) {
      if (route.name != task.targetRouteName || route.points.isEmpty()) {
        continue;
      }
      const QVector<RoutePoint> routePoints = routePointsForEntity(route, entity);
      const RoutePoint& endPoint = routePoints.last();
      task.targetLatitude = endPoint.latitude;
      task.targetLongitude = endPoint.longitude;
      task.targetAltitudeMeters = static_cast<int>(qRound(endPoint.altitudeMeters));
      break;
    }
  }

  if ((task.taskType == QStringLiteral("PatrolArea") ||
       task.taskType == QStringLiteral("OrbitArea")) &&
      !task.targetAreaName.trimmed().isEmpty()) {
    for (const AreaDefinition& area : areas) {
      if (area.name != task.targetAreaName && area.id != task.targetAreaName) {
        continue;
      }
      task.targetLatitude = area.centerLatitude;
      task.targetLongitude = area.centerLongitude;
      task.targetAltitudeMeters = static_cast<int>(qRound(area.centerAltitudeMeters));
      double radiusMeters = area.radiusMeters;
      if (radiusMeters <= 0.0) {
        if (area.areaType == QStringLiteral("Ellipse")) {
          radiusMeters = qMax(area.semiMinorAxisMeters, 100.0);
        } else if (!area.points.isEmpty()) {
          radiusMeters = 250.0;
        } else {
          radiusMeters = 500.0;
        }
      }
      task.targetAreaRadiusMeters = radiusMeters;
      break;
    }
  }

  if (entityIsGround(entity) && isMovementTaskType(task.taskType)) {
    task.targetAltitudeMeters = entity.altitude;
  }

  if (task.enabled && isMovementTaskType(task.taskType)) {
    entity.flightDynamicsEnabled = true;
    if (entity.flightDynamicsMode.trimmed().isEmpty()) {
      entity.flightDynamicsMode = QStringLiteral("kinematic");
    }
    const bool isSpatialMovement =
        task.taskType == QStringLiteral("MoveToLocation") ||
        task.taskType == QStringLiteral("WaitOnLocation") ||
        task.taskType == QStringLiteral("MoveToWaypoint") ||
        isRouteTaskType(task.taskType) ||
        task.taskType == QStringLiteral("PatrolArea") ||
        task.taskType == QStringLiteral("OrbitArea") ||
        task.taskType == QStringLiteral("HoldRacetrack") ||
        task.taskType == QStringLiteral("FollowEntity") ||
        isInterceptEntityTaskType(task.taskType);
    if ((task.taskType == QStringLiteral("FollowEntity") ||
         isInterceptEntityTaskType(task.taskType)) &&
        task.targetSpeedKnots <= 0.0) {
      task.targetSpeedKnots = entity.speedKnots > 0.0
          ? entity.speedKnots
          : (entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
             ? 220.0
             : 12.0);
    } else if (isSpatialMovement && task.targetSpeedKnots <= 0.0) {
      task.targetSpeedKnots =
          entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
          ? 220.0
          : 12.0;
    }
  }
}

bool applyEntityTask(
    const QString&    entityName,
    const EntityTask& task,
    bool              syncUi,
    ScenarioState*    state,
    SimulationEngine* simulationEngine,
    std::function<void(const QString&)> log,
    std::function<void()>               syncUi_fn) {

  if (entityName.isEmpty()) {
    return false;
  }

  EntityTask taskToApply = task;
  if (taskToApply.taskType == QStringLiteral("InterceptEntity2D") ||
      taskToApply.taskType == QStringLiteral("InterceptEntity3D")) {
    taskToApply.taskType = QStringLiteral("InterceptEntity");
  }
  if (isInterceptEntityTaskType(taskToApply.taskType)) {
    if (taskToApply.altitudeToleranceMeters <= 0.0) {
      taskToApply.altitudeToleranceMeters = 100.0;
    }
    if (taskToApply.interceptDistanceMeters <= 0.0) {
      taskToApply.interceptDistanceMeters = 500.0;
    }
    if (taskToApply.timeoutSeconds <= 0.0) {
      taskToApply.timeoutSeconds = 120.0;
    }
  }
  if (isRouteTaskType(taskToApply.taskType)) {
    if (taskToApply.arrivalToleranceMeters <= 0.0) {
      for (const Entity& entity : state->entities()) {
        if (entity.name == entityName) {
          taskToApply.arrivalToleranceMeters = defaultRouteArrivalToleranceMeters(entity);
          break;
        }
      }
      if (taskToApply.arrivalToleranceMeters <= 0.0) {
        taskToApply.arrivalToleranceMeters = 1000.0;
      }
    }
  }
  if (taskToApply.taskType == QStringLiteral("HoldRacetrack") &&
      taskToApply.racetrackLegLengthMeters <= 0.0) {
    taskToApply.racetrackLegLengthMeters = 10000.0;
  }
  if (taskToApply.taskType == QStringLiteral("WaitOnLocation") &&
      taskToApply.arrivalToleranceMeters <= 0.0) {
    taskToApply.arrivalToleranceMeters = 200.0;
  }
  if (taskToApply.taskType == QStringLiteral("AttackOnce")) {
    if (taskToApply.timeoutSeconds <= 0.0) {
      taskToApply.timeoutSeconds = 120.0;
    }
    if (taskToApply.weaponType.trimmed().isEmpty()) {
      taskToApply.weaponType = QStringLiteral("Auto");
    }
  }
  if (taskToApply.taskType == QStringLiteral("AttackUntilDestroyed")) {
    if (taskToApply.maxEngagementTimeSeconds <= 0.0) {
      taskToApply.maxEngagementTimeSeconds = 120.0;
    }
    if (taskToApply.timeoutSeconds <= 0.0) {
      taskToApply.timeoutSeconds = taskToApply.maxEngagementTimeSeconds;
    }
    if (taskToApply.shotCooldownSeconds <= 0.0) {
      taskToApply.shotCooldownSeconds = 8.0;
    }
    if (taskToApply.weaponType.trimmed().isEmpty()) {
      taskToApply.weaponType = QStringLiteral("Auto");
    }
  }
  if (taskToApply.taskType == QStringLiteral("FireOnPosition")) {
    if (taskToApply.weaponType.trimmed().isEmpty()) {
      taskToApply.weaponType = QStringLiteral("Auto");
    }
  }
  if (taskToApply.taskType == QStringLiteral("FireInDirection")) {
    if (taskToApply.weaponType.trimmed().isEmpty()) {
      taskToApply.weaponType = QStringLiteral("Auto");
    }
  }
  if (isConditionalWaitTaskType(taskToApply.taskType)) {
    if (taskToApply.taskType == QStringLiteral("WaitUntilTime")) {
      if (taskToApply.durationSeconds <= 0.0) {
        taskToApply.durationSeconds = 30.0;
      }
    } else if (taskToApply.timeoutSeconds <= 0.0) {
      taskToApply.timeoutSeconds = 120.0;
    }
    if (taskToApply.taskType == QStringLiteral("WaitUntilDamaged") &&
        taskToApply.damageThresholdPercent <= 0.0) {
      taskToApply.damageThresholdPercent = 50.0;
    }
    if (taskToApply.taskType == QStringLiteral("WaitUntilInRange") &&
        taskToApply.rangeMeters <= 0.0) {
      taskToApply.rangeMeters = 1000.0;
    }
    if (taskToApply.taskType == QStringLiteral("WaitUntilTargetDetected") &&
        taskToApply.targetDomain.trimmed().isEmpty()) {
      taskToApply.targetDomain = QStringLiteral("Any");
    }
  }

  if (!state->assignTask(entityName, taskToApply)) {
    return false;
  }

  log(QStringLiteral("Task %1 assigned to %2").arg(taskToApply.taskType, entityName));

  Entity resolvedEntity;
  bool foundEntity = false;
  for (const Entity& entity : state->entities()) {
    if (entity.name != entityName) {
      continue;
    }
    resolvedEntity = entity;
    foundEntity = true;
    break;
  }
  if (!foundEntity) {
    return false;
  }

  auto updateAppliedTask = [&](const EntityTask& updatedTask) {
    for (Entity& entity : state->entitiesMutable()) {
      if (entity.name == entityName) {
        entity.currentTask = updatedTask;
        break;
      }
    }
  };

  if (domain::TaskStack* stack = state->getTaskStack(entityName)) {
    while (!stack->isEmpty()) {
      stack->pop();
    }
    if (taskToApply.taskType == "MoveToLocation" || taskToApply.taskType == "MoveToWaypoint") {
      double targetLat = resolvedEntity.currentTask.targetLatitude;
      double targetLon = resolvedEntity.currentTask.targetLongitude;
      double targetAlt = resolvedEntity.currentTask.targetAltitudeMeters;
      double targetSpeed = resolvedEntity.currentTask.targetSpeedKnots;
      if (taskToApply.taskType == "MoveToWaypoint" && !taskToApply.targetWaypointName.trimmed().isEmpty()) {
        for (const Waypoint& waypoint : state->waypoints()) {
          if (waypoint.name == taskToApply.targetWaypointName) {
            targetLat = waypoint.latitude;
            targetLon = waypoint.longitude;
            targetAlt = waypointAltitudeForEntity(waypoint, resolvedEntity);
            break;
          }
        }
      }
      stack->push(std::make_unique<domain::MoveToLocationTask>(
          targetLat, targetLon, targetAlt, targetSpeed));
    } else if (taskToApply.taskType == "WaitOnLocation") {
      stack->push(std::make_unique<domain::WaitOnLocationTask>(
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
          resolvedEntity.currentTask.targetSpeedKnots,
          resolvedEntity.currentTask.arrivalToleranceMeters));
    } else if (isRouteTaskType(taskToApply.taskType)) {
      bool createdRouteTask = false;
      for (const RouteGraphic& route : state->routes()) {
        if (route.name != resolvedEntity.currentTask.targetRouteName ||
            route.points.isEmpty()) {
          continue;
        }
        const QVector<RoutePoint> points = routePointsForEntity(route, resolvedEntity);
        resolvedEntity.currentTask.routeTotalWaypoints = points.size();
        resolvedEntity.currentTask.routeCurrentWaypointIndex = points.isEmpty() ? 0 : 1;
        updateAppliedTask(resolvedEntity.currentTask);
        stack->push(std::make_unique<domain::RouteTask>(
            points,
            resolvedEntity.currentTask.targetSpeedKnots,
            resolvedEntity.currentTask.arrivalToleranceMeters));
        createdRouteTask = true;
        break;
      }
      if (!createdRouteTask && taskToApply.taskType == QStringLiteral("MoveAlongRoute")) {
        stack->push(std::make_unique<domain::MoveToLocationTask>(
            resolvedEntity.currentTask.targetLatitude,
            resolvedEntity.currentTask.targetLongitude,
            resolvedEntity.currentTask.targetAltitudeMeters,
            resolvedEntity.currentTask.targetSpeedKnots));
      } else if (!createdRouteTask) {
        stack->push(std::make_unique<domain::RouteTask>(
            QVector<RoutePoint>{},
            resolvedEntity.currentTask.targetSpeedKnots,
            resolvedEntity.currentTask.arrivalToleranceMeters));
      }
    } else if (taskToApply.taskType == "FlyHeadingAltitudeSpeed") {
      stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
          taskToApply.targetHeadingDegrees,
          static_cast<double>(taskToApply.targetAltitudeMeters),
          taskToApply.targetSpeedKnots));
    } else if (taskToApply.taskType == "FollowEntity") {
      stack->push(std::make_unique<domain::FollowEntityTask>(
          static_cast<double>(taskToApply.targetAltitudeMeters),
          taskToApply.targetSpeedKnots,
          resolvedEntity.currentTask.followDistanceMeters,
          resolvedEntity.currentTask.arrivalToleranceMeters));
    } else if (isInterceptEntityTaskType(taskToApply.taskType)) {
      stack->push(std::make_unique<domain::InterceptEntity3DTask>(
          resolvedEntity.currentTask.targetSpeedKnots,
          resolvedEntity.currentTask.interceptDistanceMeters,
          resolvedEntity.currentTask.altitudeToleranceMeters));
    } else if (taskToApply.taskType == "PatrolArea") {
      for (const AreaDefinition& area : state->areas()) {
        if (area.name != resolvedEntity.currentTask.targetAreaName &&
            area.id != resolvedEntity.currentTask.targetAreaName) {
          continue;
        }
        stack->push(std::make_unique<domain::PatrolAreaTask>(
            domain::buildPatrolRouteFromArea(area),
            static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
            resolvedEntity.currentTask.targetSpeedKnots));
        break;
      }
      if (stack->isEmpty()) {
        stack->push(std::make_unique<domain::OrbitAreaTask>(
            resolvedEntity.currentTask.targetLatitude,
            resolvedEntity.currentTask.targetLongitude,
            resolvedEntity.currentTask.targetAreaRadiusMeters,
            static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
            resolvedEntity.currentTask.targetSpeedKnots,
            true));
      }
    } else if (taskToApply.taskType == "OrbitArea") {
      stack->push(std::make_unique<domain::OrbitAreaTask>(
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAreaRadiusMeters,
          static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
          resolvedEntity.currentTask.targetSpeedKnots,
          false));
    } else if (taskToApply.taskType == "HoldRacetrack") {
      stack->push(std::make_unique<domain::HoldRacetrackTask>(
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetHeadingDegrees,
          resolvedEntity.currentTask.racetrackLegLengthMeters,
          static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
          resolvedEntity.currentTask.targetSpeedKnots));
    }
  }

  if (simulationEngine) {
    if (taskToApply.taskType == "MoveToLocation" || taskToApply.taskType == "MoveToWaypoint") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignMoveTask>(
          0,
          entityName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots));
    } else if (taskToApply.taskType == "WaitOnLocation") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignWaitOnLocationTask>(
          entityName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots,
          resolvedEntity.currentTask.arrivalToleranceMeters,
          resolvedEntity.currentTask.durationSeconds));
    } else if (taskToApply.taskType == "FlyHeadingAltitudeSpeed") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignFlyHeadingTask>(
          entityName,
          taskToApply.targetHeadingDegrees,
          taskToApply.targetAltitudeMeters,
          taskToApply.targetSpeedKnots));
    } else if (taskToApply.taskType == "FollowEntity") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignFollowTask>(
          entityName,
          taskToApply.targetEntityName,
          taskToApply.targetAltitudeMeters,
          taskToApply.targetSpeedKnots,
          taskToApply.followDistanceMeters,
          taskToApply.arrivalToleranceMeters,
          taskToApply.durationSeconds));
    } else if (isInterceptEntityTaskType(taskToApply.taskType)) {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignInterceptEntity3DTask>(
          entityName,
          taskToApply.targetEntityName,
          taskToApply.targetSpeedKnots,
          taskToApply.interceptDistanceMeters,
          taskToApply.altitudeToleranceMeters,
          taskToApply.timeoutSeconds));
    } else if (taskToApply.taskType == "PatrolArea" || taskToApply.taskType == "OrbitArea") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignOrbitTask>(
          entityName,
          resolvedEntity.currentTask.targetAreaName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAreaRadiusMeters,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots,
          (taskToApply.taskType == "PatrolArea")));
    } else if (taskToApply.taskType == "HoldRacetrack") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignHoldRacetrackTask>(
          entityName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetHeadingDegrees,
          resolvedEntity.currentTask.racetrackLegLengthMeters,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots,
          resolvedEntity.currentTask.durationSeconds));
    }
  }

  if (syncUi) {
    syncUi_fn();
  }
  return true;
}

} // namespace application
