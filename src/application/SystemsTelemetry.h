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
  QVector<EngineTelemetry> engines;
};

QString systemsDisplayProfileForEntity(const Entity& entity);
int engineCountForEntity(const Entity& entity);
SystemsTelemetrySnapshot makeEstimatedSystemsTelemetrySnapshot(
    const Entity& entity,
    double maximumSpeedKnots);

} // namespace application
