#pragma once

#include "presentation/EntityContextMenuBuilder.h"
#include "presentation/PlanTypes.h"

#include <QVariantMap>

struct Entity;
class ScenarioState;

namespace presentation {

/// Builds the EntityContextMenuState by querying the entity, scenario state,
/// and pending bomb release state. No Qt widget dependency — fully testable.
///
/// \param entity        Selected entity (may be nullptr → all counts zero).
/// \param state         ScenarioState for missile target detection.
/// \param pendingRelease Current pending bomb release.
/// \param simulationRunning Whether the simulation is currently running.
/// \param entityDestroyed   Whether the selected entity is destroyed.
/// \param summary           Track summary QVariantMap (hidden/radar/trackHistory flags).
EntityContextMenuState buildEntityContextMenuState(
    const Entity*              entity,
    const ScenarioState*       state,
    const PendingBombRelease&  pendingRelease,
    bool                       simulationRunning,
    bool                       entityDestroyed,
    const QVariantMap&         summary);

/// Returns the available behavior mode names.
/// Kept here so it can be shared without depending on MainWindow.
QStringList entityBehaviorModeOptions();

} // namespace presentation
