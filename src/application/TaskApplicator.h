#pragma once

#include <QString>
#include <QVector>
#include <functional>

#include "domain/Entity.h"
#include "domain/Task.h"
#include "domain/TacticalGraphic.h"

class ScenarioState;

namespace application {

class SimulationEngine;

/// Hydrates the target coordinates of a task from the matching waypoint,
/// route endpoint, or area centre. Also sets default speed and enables
/// flightDynamics when the task is a movement task.
///
/// Call this after setting entity.currentTask = task, before saving.
/// The waypoints/routes/areas vectors are the scenario's current state.
void resolveTaskCoordinates(
    Entity&                         entity,
    const QVector<Waypoint>&        waypoints,
    const QVector<RouteGraphic>&    routes,
    const QVector<AreaDefinition>&  areas);

/// Applies a task assignment to an entity: persists via ScenarioState,
/// rebuilds the TaskStack, and enqueues the matching simulation command.
/// Returns false if the entity does not exist or task assignment fails.
///
/// LogFn is called once on success. SyncUiFn is called only when syncUi==true.
bool applyEntityTask(
    const QString&      entityName,
    const EntityTask&   task,
    bool                syncUi,
    ScenarioState*      state,
    SimulationEngine*   simulationEngine,
    std::function<void(const QString&)> log,
    std::function<void()>               syncUi_fn);

} // namespace application
