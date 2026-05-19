#pragma once

#include <QString>

struct Entity;

namespace domain {

/// Whether a coordinate pair is valid for surface attack targeting.
bool attackSurfaceCoordinatesAreUsable(double latitude, double longitude);

/// Whether an attack task status string represents a terminal state.
bool attackTaskStatusIsTerminal(const QString& status);

/// Returns damage reaction level for AI behavior decisions:
/// 0 = nominal, 1 = light damage (50%+), 2 = heavy (80%+), 3 = destroyed
int autoBehaviorDamageReactionLevel(const Entity& entity);

/// Whether the entity is healthy enough to engage targets.
bool autoBehaviorCanEngageByDamage(const Entity& entity);

/// Human-readable label for a plan/attack task status string.
QString planStatusDisplayLabel(const QString& status);

/// Human-readable label for force identifier codes (1=Friendly, 2=Opposing, etc.)
QString forceIdentifierLabel(int forceIdentifier);

/// Whether the entity is eligible for missile actions (air fighter, not destroyed).
bool entityCanUseMissileActions(const Entity& entity);

} // namespace domain
