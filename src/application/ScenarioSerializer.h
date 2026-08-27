#pragma once

#include <QString>
#include <QtGlobal>
#include <QVector>

#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"

namespace application {

/// Lightweight value type holding the four persisted collections.
struct ScenarioSnapshot {
  static constexpr quint32 kDefaultSensorRandomSeed = 0x5eed1234U;

  QVector<Entity>          entities;
  QVector<Waypoint>        waypoints;
  QVector<RouteGraphic>    routes;
  QVector<AreaDefinition>  areas;
  quint32                  sensorRandomSeed = kDefaultSensorRandomSeed;
};

/// Serializes the snapshot to JSON at the given file path.
/// Creates parent directories as needed.
/// @returns true on success.
bool saveScenario(const QString& filePath, const ScenarioSnapshot& snapshot);

/// Deserializes a snapshot from JSON at the given file path.
/// Entity tasks and flight state are reset on load (matches ScenarioState::load() contract).
/// @returns The loaded snapshot, or an empty snapshot if the file cannot be read.
ScenarioSnapshot loadScenario(const QString& filePath);

} // namespace application
