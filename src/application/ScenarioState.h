#pragma once

#include "domain/Entity.h"

#include <QString>
#include <QVector>

class ScenarioState {
public:
  ScenarioState();

  void addEntity(const Entity& entity);
  const QVector<Entity>& entities() const;
  bool assignTask(const QString& entityName, const EntityTask& task);
  bool clearTask(const QString& entityName);
  void refreshSensors();
  void advanceSimulation(double deltaSeconds);
  void stopMission();
  bool save() const;
  bool load();
  QString storagePath() const;

private:
  QVector<Entity> _entities;
};
