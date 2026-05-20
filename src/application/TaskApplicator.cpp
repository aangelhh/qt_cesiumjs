#include "application/TaskApplicator.h"
#include "application/Command.h"
#include "application/ScenarioState.h"
#include "application/SimulationEngine.h"
#include "domain/Entity.h"
#include "domain/Task.h"
#include "domain/TacticalGraphic.h"

#include <QtMath>

namespace {

bool isMovementTaskType(const QString& taskType) {
  return taskType == QStringLiteral("MoveToLocation") ||
         taskType == QStringLiteral("MoveToWaypoint") ||
         taskType == QStringLiteral("MoveAlongRoute") ||
         taskType == QStringLiteral("PatrolArea") ||
         taskType == QStringLiteral("OrbitArea") ||
         taskType == QStringLiteral("FollowEntity") ||
         taskType == QStringLiteral("FlyHeadingAltitudeSpeed") ||
         taskType == QStringLiteral("AttackAir");
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
        task.targetAltitudeMeters = static_cast<int>(qRound(wp.altitudeMeters));
        break;
      }
    }
  }

  if (task.taskType == QStringLiteral("MoveAlongRoute") &&
      !task.targetRouteName.trimmed().isEmpty()) {
    for (const RouteGraphic& route : routes) {
      if (route.name != task.targetRouteName || route.points.isEmpty()) {
        continue;
      }
      const RoutePoint& endPoint = route.points.last();
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

  if (task.enabled && isMovementTaskType(task.taskType)) {
    entity.flightDynamicsEnabled = true;
    if (entity.flightDynamicsMode.trimmed().isEmpty()) {
      entity.flightDynamicsMode = QStringLiteral("kinematic");
    }
    const bool isSpatialMovement =
        task.taskType == QStringLiteral("MoveToLocation") ||
        task.taskType == QStringLiteral("MoveToWaypoint") ||
        task.taskType == QStringLiteral("MoveAlongRoute") ||
        task.taskType == QStringLiteral("PatrolArea") ||
        task.taskType == QStringLiteral("OrbitArea") ||
        task.taskType == QStringLiteral("FollowEntity");
    if (isSpatialMovement && task.targetSpeedKnots <= 0.0) {
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

  if (!state->assignTask(entityName, task)) {
    return false;
  }

  log(QStringLiteral("Task %1 assigned to %2").arg(task.taskType, entityName));

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

  if (domain::TaskStack* stack = state->getTaskStack(entityName)) {
    while (!stack->isEmpty()) {
      stack->pop();
    }
    if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint") {
      double targetLat = resolvedEntity.currentTask.targetLatitude;
      double targetLon = resolvedEntity.currentTask.targetLongitude;
      double targetAlt = resolvedEntity.currentTask.targetAltitudeMeters;
      double targetSpeed = resolvedEntity.currentTask.targetSpeedKnots;
      if (task.taskType == "MoveToWaypoint" && !task.targetWaypointName.trimmed().isEmpty()) {
        for (const Waypoint& waypoint : state->waypoints()) {
          if (waypoint.name == task.targetWaypointName) {
            targetLat = waypoint.latitude;
            targetLon = waypoint.longitude;
            targetAlt = waypoint.altitudeMeters;
            break;
          }
        }
      }
      stack->push(std::make_unique<domain::MoveToLocationTask>(
          targetLat, targetLon, targetAlt, targetSpeed));
    } else if (task.taskType == "MoveAlongRoute") {
      bool createdRouteTask = false;
      for (const RouteGraphic& route : state->routes()) {
        if (route.name != resolvedEntity.currentTask.targetRouteName ||
            route.points.isEmpty()) {
          continue;
        }
        stack->push(std::make_unique<domain::RouteTask>(
            route.points, resolvedEntity.currentTask.targetSpeedKnots));
        createdRouteTask = true;
        break;
      }
      if (!createdRouteTask) {
        stack->push(std::make_unique<domain::MoveToLocationTask>(
            resolvedEntity.currentTask.targetLatitude,
            resolvedEntity.currentTask.targetLongitude,
            resolvedEntity.currentTask.targetAltitudeMeters,
            resolvedEntity.currentTask.targetSpeedKnots));
      }
    } else if (task.taskType == "FlyHeadingAltitudeSpeed") {
      stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
          task.targetHeadingDegrees,
          static_cast<double>(task.targetAltitudeMeters),
          task.targetSpeedKnots));
    } else if (task.taskType == "FollowEntity") {
      stack->push(std::make_unique<domain::FollowEntityTask>(
          static_cast<double>(task.targetAltitudeMeters),
          task.targetSpeedKnots));
    } else if (task.taskType == "PatrolArea") {
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
    } else if (task.taskType == "OrbitArea") {
      stack->push(std::make_unique<domain::OrbitAreaTask>(
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAreaRadiusMeters,
          static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
          resolvedEntity.currentTask.targetSpeedKnots,
          false));
    }
  }

  if (simulationEngine) {
    if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignMoveTask>(
          0,
          entityName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots));
    } else if (task.taskType == "FlyHeadingAltitudeSpeed") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignFlyHeadingTask>(
          entityName,
          task.targetHeadingDegrees,
          task.targetAltitudeMeters,
          task.targetSpeedKnots));
    } else if (task.taskType == "FollowEntity") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignFollowTask>(
          entityName,
          task.targetEntityName,
          task.targetAltitudeMeters,
          task.targetSpeedKnots));
    } else if (task.taskType == "PatrolArea" || task.taskType == "OrbitArea") {
      simulationEngine->enqueueCommand(std::make_unique<CmdAssignOrbitTask>(
          entityName,
          resolvedEntity.currentTask.targetAreaName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAreaRadiusMeters,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots,
          (task.taskType == "PatrolArea")));
    }
  }

  if (syncUi) {
    syncUi_fn();
  }
  return true;
}

} // namespace application
