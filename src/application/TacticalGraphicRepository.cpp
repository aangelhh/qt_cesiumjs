#include "application/TacticalGraphicRepository.h"

namespace application {

void upsertWaypoint(QVector<Waypoint>& waypoints, const Waypoint& waypoint) {
  for (Waypoint& existing : waypoints) {
    if (existing.name == waypoint.name) {
      existing = waypoint;
      return;
    }
  }
  waypoints.push_back(waypoint);
}

bool removeWaypoint(QVector<Waypoint>& waypoints, const QString& waypointName) {
  for (qsizetype i = 0; i < waypoints.size(); ++i) {
    if (waypoints.at(i).name == waypointName) {
      waypoints.removeAt(i);
      return true;
    }
  }
  return false;
}

void upsertRoute(QVector<RouteGraphic>& routes, const RouteGraphic& route) {
  for (RouteGraphic& existing : routes) {
    if (existing.name == route.name) {
      existing = route;
      return;
    }
  }
  routes.push_back(route);
}

bool removeRoute(QVector<RouteGraphic>& routes, const QString& routeName) {
  for (qsizetype i = 0; i < routes.size(); ++i) {
    if (routes.at(i).name == routeName) {
      routes.removeAt(i);
      return true;
    }
  }
  return false;
}

void upsertArea(QVector<AreaDefinition>& areas, const AreaDefinition& area) {
  for (AreaDefinition& existing : areas) {
    if (existing.id == area.id || existing.name == area.name) {
      existing = area;
      return;
    }
  }
  areas.push_back(area);
}

bool removeArea(
    QVector<AreaDefinition>& areas,
    QVector<Entity>& entities,
    const QString& nameOrId) {
  for (qsizetype i = 0; i < areas.size(); ++i) {
    if (areas.at(i).name == nameOrId || areas.at(i).id == nameOrId) {
      areas.removeAt(i);
      for (Entity& entity : entities) {
        if ((entity.currentTask.taskType == QStringLiteral("PatrolArea") ||
             entity.currentTask.taskType == QStringLiteral("OrbitArea")) &&
            entity.currentTask.targetAreaName == nameOrId) {
          entity.currentTask = EntityTask{};
          entity.currentTask.status = QStringLiteral("Idle");
        }
      }
      return true;
    }
  }
  return false;
}

} // namespace application
