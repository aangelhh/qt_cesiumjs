#pragma once

#include "presentation/PlanTypes.h"

#include <QVariantMap>
#include <functional>

struct Entity;

namespace presentation {

/// Sends the pending bomb target track(s) to the map, or removes them if no
/// release is pending. The launcher entity is resolved via the findEntity
/// callback so the function has no dependency on ScenarioState or any widget.
///
/// \param pendingRelease  Current pending bomb release state.
/// \param findEntity      Returns a pointer to an Entity given its name
///                        (nullptr when not found).
/// \param sendTrack       Pushes a track summary to the map.
/// \param removeTrack     Removes a track from the map by ID/name.
void syncPendingBombTargetToMap(
    const PendingBombRelease&                          pendingRelease,
    std::function<const Entity*(const QString& name)>  findEntity,
    std::function<void(const QVariantMap&, bool)>      sendTrack,
    std::function<void(const QString&)>                removeTrack);

} // namespace presentation
