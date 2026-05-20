#pragma once

#include <QString>
#include <QVector>
#include <functional>

#include "domain/Entity.h"
#include "domain/Munition.h"

namespace application {

/// Creates an unguided missile munition positioned at the launcher.
ActiveMunition makeMissileMunition(const Entity& launcher, int serial);

/// Creates a bomb munition positioned at the launcher with forward/down offset.
ActiveMunition makeBombMunition(const Entity& launcher, int serial);

/// Returns true if the munition is a bomb (vs. missile).
bool munitionIsBomb(const ActiveMunition& munition);

/// A single blast-damage hit produced by a bomb detonation.
struct BombBlastHit {
  QString targetName;
  double  damageAmount = 0.0;
};

/// Returns the set of entities hit by a bomb blast, with falloff damage.
///
/// Only live, non-launcher entities within the blast radius are included.
/// Damage falls off linearly from baseDamage at the centre to 0 at the edge.
/// Returns an empty list if the munition is not a bomb or has no blast radius.
QVector<BombBlastHit> computeBombBlastHits(
    const ActiveMunition& munition,
    const QVector<Entity>& entities);

/// Advances all active munitions by deltaSeconds.
///
/// Handles:
///   - Guided missile homing (heading + pitch)
///   - Bomb ballistics (gravity)
///   - Proximity hit detection (guided + area)
///   - Smoke trail / impact transient effects
///   - TTL / ground expiry
///   - Removal of inactive munitions
///
/// @param activeMunitions  In/out list of active munitions.
/// @param effects          In/out list of transient effects.
/// @param entities         Read-only snapshot of scenario entities (for tracking + hit detection).
/// @param applyDamageFn    Called once per hit: (targetEntityName, damagePercent).
/// @param applyBombBlastFn Called once per bomb ground/proximity detonation: (munition).
/// @param deltaSeconds     Simulation step in seconds (must be > 0).
void advanceActiveMunitions(
    QVector<ActiveMunition>&                        activeMunitions,
    QVector<TransientEffect>&                       effects,
    const QVector<Entity>&                          entities,
    std::function<void(const QString&, double)>     applyDamageFn,
    std::function<void(const ActiveMunition&)>      applyBombBlastFn,
    double                                          deltaSeconds);

} // namespace application
