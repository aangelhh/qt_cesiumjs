#pragma once

#include "domain/Entity.h"
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
  bool assignTask(const QString& entityName, const EntityTask& task);
  bool clearTask(const QString& entityName);
  domain::TaskStack* getTaskStack(const QString& entityName);
  void refreshSensors();
  void advanceSimulation(double deltaSeconds);
  void stopMission();
  bool save() const;
  bool load();
  QString storagePath() const;

private:
  QVector<Entity> _entities;
  std::unordered_map<QString, domain::TaskStack> _taskStacks;
};
