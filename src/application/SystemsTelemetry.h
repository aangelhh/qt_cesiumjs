#pragma once

#include "domain/Entity.h"

#include <QString>
#include <QVector>

namespace application {

struct EngineTelemetry {
  QString engineId;
  QString state;
  double n1Percent = 0.0;
  double n2Percent = 0.0;
  double exhaustTemperatureCelsius = 0.0;
  double fuelFlowKilogramsPerHour = 0.0;
  double thrustKilonewtons = 0.0;
  bool n1Available = false;
  bool n2Available = false;
  bool exhaustTemperatureAvailable = false;
  bool fuelFlowAvailable = false;
  bool thrustAvailable = false;
  bool available = false;
  bool estimated = false;
};

struct SystemsTelemetrySnapshot {
  QString entityName;
  QString profileId;
  QString dataSource;
  double fuelCapacityKilograms = 0.0;
  double fuelRemainingKilograms = 0.0;
  double fuelPercent = 0.0;
  double totalFuelFlowKilogramsPerHour = 0.0;
  double estimatedEnduranceSeconds = 0.0;
  bool fuelAvailable = false;
  bool enduranceAvailable = false;
  QVector<EngineTelemetry> engines;
};

QString systemsDisplayProfileForEntity(const Entity& entity);
int engineCountForEntity(const Entity& entity);
double defaultFuelCapacityKilograms(const Entity& entity);
double defaultInitialFuelKilograms(const Entity& entity);
void ensureFuelConfiguration(Entity& entity);
void consumeEstimatedFuel(
    Entity& entity,
    double deltaSeconds,
    double maximumSpeedKnots = 900.0);
void updateFuelTelemetrySummary(
    SystemsTelemetrySnapshot& snapshot,
    double capacityKilograms,
    double remainingKilograms);
SystemsTelemetrySnapshot makeEstimatedSystemsTelemetrySnapshot(
    const Entity& entity,
    double maximumSpeedKnots);

} // namespace application
