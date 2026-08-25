#pragma once

#include "application/KinematicsTelemetry.h"
#include "domain/Entity.h"
#include "domain/Munition.h"
#include "domain/TacticalGraphic.h"
#include "domain/Task.h"

#include <QString>
#include <QStringList>
#include <QVector>
#include <mutex>
#include <unordered_map>

class ScenarioState {
public:
  // Recursive so the scenario's own mutating methods can call helpers that
  // also need to lock without self-deadlocking.
  using Mutex = std::recursive_mutex;
  using ScopedLock = std::unique_lock<Mutex>;

  ScenarioState();

  // Callers that hold a reference returned by any of the const getters below
  // (entities(), waypoints(), areas(), ...) must keep this lock alive for the
  // entire time the reference is used; the simulation thread mutates the same
  // containers in advanceSimulation(). For read-after-copy access it is enough
  // to take this lock around the copy.
  ScopedLock lock() const { return ScopedLock(_mutex); }

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
  bool entityFuelState(
      const QString& entityName,
      double& remainingKilograms,
      double& capacityKilograms) const;
  bool setEntityFuelRemaining(const QString& entityName, double kilograms);
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
  double simulationTimeSeconds() const;
  void setKinematicsTelemetryPublicationPeriod(double periodSeconds);
  void stopMission();
  bool save() const;
  bool load();
  void reset();
  QString storagePath() const;

private:
  mutable Mutex _mutex;
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
  double _simulationTimeSeconds = 0.0;
  application::KinematicsTelemetryPublisher _kinematicsTelemetryPublisher;

  void applyDamageWithSource(
      const QString& targetName,
      double damageAmount,
      const QString& sourceLabel);
  void applyBombBlastDamage(const ActiveMunition& munition);
  void advanceBehaviors(double deltaSeconds);
  void advanceActiveMunitions(double deltaSeconds);
  void advanceTransientEffects(double deltaSeconds);
};
