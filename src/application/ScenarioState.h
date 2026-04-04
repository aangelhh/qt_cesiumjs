#pragma once

#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"

#include <QString>
#include <QVector>

class ScenarioState {
public:
  ScenarioState();

  void addEntity(const Entity& entity);
  const QVector<Entity>& entities() const;
  bool removeEntity(const QString& entityName);
  void addWaypoint(const Waypoint& waypoint);
  const QVector<Waypoint>& waypoints() const;
  bool removeWaypoint(const QString& waypointName);
  void addRoute(const RouteGraphic& route);
  const QVector<RouteGraphic>& routes() const;
  bool removeRoute(const QString& routeName);
  void addArea(const AreaDefinition& area);
  const QVector<AreaDefinition>& areas() const;
  bool removeArea(const QString& areaName);
  bool assignTask(const QString& entityName, const EntityTask& task);
  bool clearTask(const QString& entityName);
  void refreshSensors();
  void advanceSimulation(double deltaSeconds);
  void stopMission();
  bool save() const;
  bool load();
  void reset();
  QString storagePath() const;

private:
  QVector<Entity> _entities;
  QVector<Waypoint> _waypoints;
  QVector<RouteGraphic> _routes;
  QVector<AreaDefinition> _areas;
};
