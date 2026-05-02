#pragma once

#include "domain/Entity.h"
#include "domain/Munition.h"
#include "domain/TacticalGraphic.h"
#include "domain/Task.h"

#include <QString>
#include <QStringList>
#include <QVector>
#include <unordered_map>

class ScenarioState {
public:
  ScenarioState();

  void addEntity(const Entity& entity);
  const QVector<Entity>& entities() const;
  const QVector<ActiveMunition>& activeMunitions() const;
  const QVector<TransientEffect>& transientEffects() const;
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
  bool setEntityBehaviorMode(const QString& entityName, const QString& behaviorMode);
  void applyMissileDamage(const QString& targetName, double damageAmount);
  bool addMissileToEntity(const QString& entityName, int quantity = 1);
  bool addBombToEntity(const QString& entityName, int quantity = 1);
  bool launchMissile(const QString& entityName);
  bool launchMissileAt(const QString& launcherName, const QString& targetName);
  bool releaseBomb(const QString& entityName);
  static double missileMaxRangeMeters();
  QStringList takePendingEventLogMessages();
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
  QVector<ActiveMunition> _activeMunitions;
  QVector<TransientEffect> _transientEffects;
  QStringList _pendingEventLogMessages;
  std::unordered_map<QString, domain::TaskStack> _taskStacks;
  std::unordered_map<QString, double> _behaviorMissileCooldownSeconds;
  QVector<Waypoint> _waypoints;
  QVector<RouteGraphic> _routes;
  QVector<AreaDefinition> _areas;
  int _nextMunitionSerial = 1;

  void applyDamageWithSource(
      const QString& targetName,
      double damageAmount,
      const QString& sourceLabel);
  void applyBombBlastDamage(const ActiveMunition& munition);
  void advanceBehaviors(double deltaSeconds);
  void advanceActiveMunitions(double deltaSeconds);
  void advanceTransientEffects(double deltaSeconds);
};
