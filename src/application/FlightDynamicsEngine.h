#pragma once

#include "domain/Entity.h"
#include "domain/Task.h"
#include "application/SystemsTelemetry.h"

#include <QVector>
#include <unordered_map>

struct FlightDynamicsExecutionPolicy {
  bool enforceWallClockStepBudget = true;
};

class FlightDynamicsEngine {
public:
  static void advanceEntities(
      QVector<Entity>& entities,
      std::unordered_map<QString, domain::TaskStack>& taskStacks,
      double deltaSeconds);
  static void advanceEntities(
      QVector<Entity>& entities,
      std::unordered_map<QString, domain::TaskStack>& taskStacks,
      double simulationTimeSeconds,
      double deltaSeconds);
  static void advanceEntities(
      QVector<Entity>& entities,
      std::unordered_map<QString, domain::TaskStack>& taskStacks,
      double simulationTimeSeconds,
      double deltaSeconds,
      const FlightDynamicsExecutionPolicy& executionPolicy);
  static application::SystemsTelemetrySnapshot systemsTelemetryForEntity(
      const Entity& entity,
      double maximumSpeedKnots);
  static bool setFuelRemaining(Entity& entity, double kilograms);
  static void releaseDynamicsModel(const QString& entityId);
  static void clearDynamicsModels();

private:
  static void advanceEntity(
      Entity& entity,
      std::unordered_map<QString, domain::TaskStack>& taskStacks,
      const QVector<Entity>& snapshot,
      double simulationTimeSeconds,
      double deltaSeconds,
      const FlightDynamicsExecutionPolicy& executionPolicy);
};
