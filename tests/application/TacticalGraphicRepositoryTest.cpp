#include <gtest/gtest.h>
#include "application/TacticalGraphicRepository.h"

using namespace application;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Waypoint makeWaypoint(const QString& name, double lat = 0.0, double lon = 0.0) {
  Waypoint wp;
  wp.name = name;
  wp.latitude = lat;
  wp.longitude = lon;
  wp.altitudeMeters = 1000.0;
  return wp;
}

static RouteGraphic makeRoute(const QString& name, int numPoints = 2) {
  RouteGraphic route;
  route.name = name;
  for (int i = 0; i < numPoints; ++i) {
    RoutePoint p;
    p.latitude = i * 1.0;
    p.longitude = i * 1.0;
    p.altitudeMeters = 3000.0;
    route.points.push_back(p);
  }
  return route;
}

static AreaDefinition makeArea(const QString& id, const QString& name) {
  AreaDefinition area;
  area.id = id;
  area.name = name;
  area.areaType = QStringLiteral("Circle");
  area.radiusMeters = 5000.0;
  return area;
}

// ── Waypoints ─────────────────────────────────────────────────────────────────

TEST(TacticalGraphicRepository, UpsertWaypointAddsNew) {
  QVector<Waypoint> waypoints;
  upsertWaypoint(waypoints, makeWaypoint(QStringLiteral("Alpha"), 10.0, 20.0));
  ASSERT_EQ(waypoints.size(), 1);
  EXPECT_EQ(waypoints.at(0).name, QStringLiteral("Alpha"));
}

TEST(TacticalGraphicRepository, UpsertWaypointReplacesExisting) {
  QVector<Waypoint> waypoints;
  upsertWaypoint(waypoints, makeWaypoint(QStringLiteral("Alpha"), 10.0, 20.0));
  upsertWaypoint(waypoints, makeWaypoint(QStringLiteral("Alpha"), 99.0, 99.0));
  ASSERT_EQ(waypoints.size(), 1);
  EXPECT_NEAR(waypoints.at(0).latitude, 99.0, 0.001);
}

TEST(TacticalGraphicRepository, RemoveWaypointFound) {
  QVector<Waypoint> waypoints;
  upsertWaypoint(waypoints, makeWaypoint(QStringLiteral("Alpha")));
  upsertWaypoint(waypoints, makeWaypoint(QStringLiteral("Bravo")));
  EXPECT_TRUE(removeWaypoint(waypoints, QStringLiteral("Alpha")));
  ASSERT_EQ(waypoints.size(), 1);
  EXPECT_EQ(waypoints.at(0).name, QStringLiteral("Bravo"));
}

TEST(TacticalGraphicRepository, RemoveWaypointNotFound) {
  QVector<Waypoint> waypoints;
  upsertWaypoint(waypoints, makeWaypoint(QStringLiteral("Alpha")));
  EXPECT_FALSE(removeWaypoint(waypoints, QStringLiteral("X")));
  EXPECT_EQ(waypoints.size(), 1);
}

// ── Routes ────────────────────────────────────────────────────────────────────

TEST(TacticalGraphicRepository, UpsertRouteAddsNew) {
  QVector<RouteGraphic> routes;
  upsertRoute(routes, makeRoute(QStringLiteral("Route-1")));
  ASSERT_EQ(routes.size(), 1);
  EXPECT_EQ(routes.at(0).name, QStringLiteral("Route-1"));
}

TEST(TacticalGraphicRepository, UpsertRouteReplacesExisting) {
  QVector<RouteGraphic> routes;
  upsertRoute(routes, makeRoute(QStringLiteral("Route-1"), 2));
  upsertRoute(routes, makeRoute(QStringLiteral("Route-1"), 5));
  ASSERT_EQ(routes.size(), 1);
  EXPECT_EQ(routes.at(0).points.size(), 5);
}

TEST(TacticalGraphicRepository, RemoveRouteFound) {
  QVector<RouteGraphic> routes;
  upsertRoute(routes, makeRoute(QStringLiteral("R1")));
  upsertRoute(routes, makeRoute(QStringLiteral("R2")));
  EXPECT_TRUE(removeRoute(routes, QStringLiteral("R1")));
  ASSERT_EQ(routes.size(), 1);
  EXPECT_EQ(routes.at(0).name, QStringLiteral("R2"));
}

TEST(TacticalGraphicRepository, RemoveRouteNotFound) {
  QVector<RouteGraphic> routes;
  upsertRoute(routes, makeRoute(QStringLiteral("R1")));
  EXPECT_FALSE(removeRoute(routes, QStringLiteral("X")));
  EXPECT_EQ(routes.size(), 1);
}

// ── Areas ─────────────────────────────────────────────────────────────────────

TEST(TacticalGraphicRepository, UpsertAreaAddsNew) {
  QVector<AreaDefinition> areas;
  upsertArea(areas, makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1")));
  ASSERT_EQ(areas.size(), 1);
  EXPECT_EQ(areas.at(0).name, QStringLiteral("Zone-1"));
}

TEST(TacticalGraphicRepository, UpsertAreaReplacesById) {
  QVector<AreaDefinition> areas;
  auto a = makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1"));
  a.radiusMeters = 1000.0;
  upsertArea(areas, a);
  auto updated = makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1-updated"));
  updated.radiusMeters = 9999.0;
  upsertArea(areas, updated);
  ASSERT_EQ(areas.size(), 1);
  EXPECT_NEAR(areas.at(0).radiusMeters, 9999.0, 0.1);
}

TEST(TacticalGraphicRepository, RemoveAreaByNameFound) {
  QVector<AreaDefinition> areas;
  QVector<Entity> entities;
  upsertArea(areas, makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1")));
  EXPECT_TRUE(removeArea(areas, entities, QStringLiteral("Zone-1")));
  EXPECT_TRUE(areas.isEmpty());
}

TEST(TacticalGraphicRepository, RemoveAreaByIdFound) {
  QVector<AreaDefinition> areas;
  QVector<Entity> entities;
  upsertArea(areas, makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1")));
  EXPECT_TRUE(removeArea(areas, entities, QStringLiteral("a1")));
  EXPECT_TRUE(areas.isEmpty());
}

TEST(TacticalGraphicRepository, RemoveAreaClearsPatrolTask) {
  QVector<AreaDefinition> areas;
  upsertArea(areas, makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1")));

  Entity e;
  e.name = QStringLiteral("Eagle-1");
  e.currentTask.taskType = QStringLiteral("PatrolArea");
  e.currentTask.targetAreaName = QStringLiteral("Zone-1");
  e.currentTask.enabled = true;
  QVector<Entity> entities = {e};

  EXPECT_TRUE(removeArea(areas, entities, QStringLiteral("Zone-1")));
  EXPECT_TRUE(entities.at(0).currentTask.taskType.isEmpty());
  EXPECT_EQ(entities.at(0).currentTask.status, QStringLiteral("Idle"));
}

TEST(TacticalGraphicRepository, RemoveAreaClearsOrbitTask) {
  QVector<AreaDefinition> areas;
  upsertArea(areas, makeArea(QStringLiteral("a2"), QStringLiteral("Zone-2")));

  Entity e;
  e.name = QStringLiteral("Hawk-1");
  e.currentTask.taskType = QStringLiteral("OrbitArea");
  e.currentTask.targetAreaName = QStringLiteral("Zone-2");
  QVector<Entity> entities = {e};

  EXPECT_TRUE(removeArea(areas, entities, QStringLiteral("Zone-2")));
  EXPECT_TRUE(entities.at(0).currentTask.taskType.isEmpty());
}

TEST(TacticalGraphicRepository, RemoveAreaDoesNotClearUnrelatedTask) {
  QVector<AreaDefinition> areas;
  upsertArea(areas, makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1")));
  upsertArea(areas, makeArea(QStringLiteral("a2"), QStringLiteral("Zone-2")));

  Entity e;
  e.name = QStringLiteral("Eagle-1");
  e.currentTask.taskType = QStringLiteral("PatrolArea");
  e.currentTask.targetAreaName = QStringLiteral("Zone-2");
  QVector<Entity> entities = {e};

  EXPECT_TRUE(removeArea(areas, entities, QStringLiteral("Zone-1")));
  EXPECT_EQ(entities.at(0).currentTask.taskType, QStringLiteral("PatrolArea"));
}

TEST(TacticalGraphicRepository, RemoveAreaNotFound) {
  QVector<AreaDefinition> areas;
  QVector<Entity> entities;
  upsertArea(areas, makeArea(QStringLiteral("a1"), QStringLiteral("Zone-1")));
  EXPECT_FALSE(removeArea(areas, entities, QStringLiteral("Nope")));
  EXPECT_EQ(areas.size(), 1);
}
