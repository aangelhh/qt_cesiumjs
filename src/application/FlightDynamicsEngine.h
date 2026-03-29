#pragma once

#include "domain/Entity.h"

#include <QVector>

class FlightDynamicsEngine {
public:
  static void advanceEntities(QVector<Entity>& entities, double deltaSeconds);

private:
  static void advanceEntity(Entity& entity, const QVector<Entity>& snapshot, double deltaSeconds);
};
