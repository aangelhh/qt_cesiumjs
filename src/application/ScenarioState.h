#pragma once

#include "domain/Entity.h"
#include "domain/Munition.h"
#include "domain/TacticalGraphic.h"
#include "domain/Task.h"

#include <QString>
#include <QStringList>
#include <QVector>
#include <shared_mutex>
#include <unordered_map>

class ScenarioState {
public:
  ScenarioState();

  void addEntity(const Entity& entity);
  [[deprecated("Use entitiesSnapshot() for thread-safe reads.")]]
  const QVector<Entity>& entities() const;
  QVector<Entity> entitiesSnapshot() const;
  [[deprecated("Use activeMunitionsSnapshot() for thread-safe reads.")]]
  const QVector<ActiveMunition>& activeMunitions() const;
  [[deprecated("Use transientEffectsSnapshot() for thread-safe reads.")]]
  const QVector<TransientEffect>& transientEffects() const;
  QVector<ActiveMunition> activeMunitionsSnapshot() const;
  QVector<TransientEffect> transientEffectsSnapshot() const;
  QVector<Entity>& entitiesMutable() { return _entities; }
  bool removeEntity(const QString& entityName);
  void addWaypoint(const Waypoint& waypoint);
  [[deprecated("Use waypointsSnapshot() for thread-safe reads.")]]
  const QVector<Waypoint>& waypoints() const;
  QVector<Waypoint> waypointsSnapshot() const;
  bool removeWaypoint(const QString& waypointName);
  void addRoute(const RouteGraphic& route);
  [[deprecated("Use routesSnapshot() for thread-safe reads.")]]
  const QVector<RouteGraphic>& routes() const;
  QVector<RouteGraphic> routesSnapshot() const;
  bool removeRoute(const QString& routeName);
  void addArea(const AreaDefinition& area);
  [[deprecated("Use areasSnapshot() for thread-safe reads.")]]
  const QVector<AreaDefinition>& areas() const;
  QVector<AreaDefinition> areasSnapshot() const;
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
  bool updateEntityTaskStatus(const QString& entityName, const QString& status);
  bool updateEntityAttackTarget(
      const QString& entityName,
      double targetLatitude,
      double targetLongitude,
      int targetAltitudeMeters,
      double targetHeadingDegrees,
      double targetSpeedKnots,
      const QString& status);

private:
  domain::TaskStack* getTaskStackUnlocked(const QString& entityName);
  void refreshSensorsUnlocked();
  bool saveUnlocked() const;
  mutable std::shared_mutex _stateMutex;
  QVector<Entity> _entities;
  QVector<ActiveMunition> _activeMunitions;
  QVector<TransientEffect> _transientEffects;
  QStringList _pendingEventLogMessages;
  std::unordered_map<QString, domain::TaskStack> _taskStacks;
  std::unordered_map<QString, double> _behaviorMissileCooldownSeconds;
  std::unordered_map<QString, int> _behaviorDamageReactionLevel;
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
