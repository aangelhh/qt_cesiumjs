#pragma once

#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"

#include <QVector>

namespace application {

// Upserts waypoint by name (replaces if name already exists).
void upsertWaypoint(QVector<Waypoint>& waypoints, const Waypoint& waypoint);

// Removes the waypoint with waypointName. Returns true if found and removed.
bool removeWaypoint(QVector<Waypoint>& waypoints, const QString& waypointName);

// Upserts route by name (replaces if name already exists).
void upsertRoute(QVector<RouteGraphic>& routes, const RouteGraphic& route);

// Removes the route with routeName. Returns true if found and removed.
bool removeRoute(QVector<RouteGraphic>& routes, const QString& routeName);

// Upserts area by id or name (replaces if either matches).
void upsertArea(QVector<AreaDefinition>& areas, const AreaDefinition& area);

// Removes area by name or id. Also clears PatrolArea/OrbitArea tasks on any
// entity that was targeting that area. Returns true if found and removed.
bool removeArea(
    QVector<AreaDefinition>& areas,
    QVector<Entity>& entities,
    const QString& nameOrId);

} // namespace application
