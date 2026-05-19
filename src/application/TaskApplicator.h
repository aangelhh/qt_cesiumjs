#pragma once

#include <QString>
#include <functional>

#include "domain/Task.h"

class ScenarioState;

namespace application {

class SimulationEngine;

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
