#pragma once

#include "domain/Entity.h"

#include <QVector>

class ScenarioState {
public:
  void addEntity(const Entity& entity);
  const QVector<Entity>& entities() const;

private:
  QVector<Entity> _entities;
};
