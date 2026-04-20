#pragma once

#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"
#include "domain/Task.h"

#include <QString>
#include <QVector>
#include <unordered_map>

class ScenarioState {
public:
  ScenarioState();

  void addEntity(const Entity& entity);
  const QVector<Entity>& entities() const;
  QVector<Entity>& entitiesMutable() { return _entities; }
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
  bool setEntityDestroyed(const QString& entityName, bool destroyed);
  domain::TaskStack* getTaskStack(const QString& entityName);
  void refreshSensors();
  void advanceSimulation(double deltaSeconds);
  void stopMission();
  bool save() const;
  bool load();
  void reset();
  QString storagePath() const;

private:
  QVector<Entity> _entities;
  std::unordered_map<QString, domain::TaskStack> _taskStacks;
  QVector<Waypoint> _waypoints;
  QVector<RouteGraphic> _routes;
  QVector<AreaDefinition> _areas;
};
