#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <functional>
#include <unordered_map>

#include "domain/Entity.h"

namespace application {

/// Alias matching ScenarioState's internal cooldown/reaction maps.
using BehaviorCooldownMap  = std::unordered_map<QString, double>;
using BehaviorReactionMap  = std::unordered_map<QString, int>;

/// Advances AI behavior for all entities.
///
/// Mutates:
///   - entity.behaviorTargetEntityName — updated to best sensor contact
///   - cooldowns   — missile cooldown timers ticked / expired entries removed
///   - reactions   — damage-reaction level cache
///
/// @param entities     In/out entity list (behaviorTargetEntityName mutated).
/// @param cooldowns    In/out missile cooldown map.
/// @param reactions    In/out damage reaction level map.
/// @param deltaSeconds Simulation step in seconds.
/// @param launchFn     Called when auto-engage fires: (launcherName, targetName)
///                     → returns true on success.
/// @returns            Log messages to append to the scenario event log.
QStringList advanceBehaviors(
    QVector<Entity>&                                    entities,
    BehaviorCooldownMap&                                cooldowns,
    BehaviorReactionMap&                                reactions,
    double                                              deltaSeconds,
    std::function<bool(const QString&, const QString&)> launchFn);

} // namespace application
