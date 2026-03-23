#include "application/ScenarioState.h"

void ScenarioState::addEntity(const Entity& entity) {
  _entities.push_back(entity);
}

const QVector<Entity>& ScenarioState::entities() const {
  return _entities;
}
