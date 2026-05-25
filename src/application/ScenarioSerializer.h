#pragma once

#include <QString>
#include <QVector>

#include "domain/Entity.h"
#include "domain/TacticalGraphic.h"

namespace application {

/// Lightweight value type holding the four persisted collections.
struct ScenarioSnapshot {
  QVector<Entity>          entities;
  QVector<Waypoint>        waypoints;
  QVector<RouteGraphic>    routes;
  QVector<AreaDefinition>  areas;
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
