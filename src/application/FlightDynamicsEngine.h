#pragma once

#include "domain/Entity.h"
#include "domain/Task.h"
#include "application/SystemsTelemetry.h"

#include <QVector>
#include <unordered_map>

class FlightDynamicsEngine {
public:
  static void advanceEntities(QVector<Entity>& entities, std::unordered_map<QString, domain::TaskStack>& taskStacks, double deltaSeconds);
  static application::SystemsTelemetrySnapshot systemsTelemetryForEntity(
      const Entity& entity,
      double maximumSpeedKnots);

private:
  static void advanceEntity(Entity& entity, std::unordered_map<QString, domain::TaskStack>& taskStacks, const QVector<Entity>& snapshot, double deltaSeconds);
};
