#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

class ScenarioState;
struct Entity;

namespace application {

/// Data returned by detectedMissileTargetsInRange.
struct MissileTargetCandidate {
  const Entity* entity = nullptr;
  double rangeMeters = -1.0;
};

/// Air targets detected by launcher's sensors and within missile range.
QVector<MissileTargetCandidate> detectedMissileTargetsInRange(
    const ScenarioState* scenarioState,
    const Entity& launcher);

/// Whether a missile from launcher is already in flight toward target.
bool activeMissileInFlightForTarget(
    const ScenarioState* scenarioState,
    const QString& launcherName,
    const QString& targetName);

/// Returns the altitude (metres) of an entity by name, or 0 if not found.
int entityAltitudeMeters(
    const ScenarioState* scenarioState,
    const QString& entityName);

/// Returns all surface entities that are valid bomb release targets for launcher
/// (enemy, not destroyed, not air domain).
QVector<const Entity*> validBombReleaseTargets(
    const ScenarioState* scenarioState,
    const Entity& launcher);

/// Returns the nearest detected surface enemy entity, or nullptr.
const Entity* bestDetectedSurfaceBombTarget(
    const ScenarioState* scenarioState,
    const Entity& launcher);

/// Unique, non-empty waypoint names from scenario state.
QStringList availableWaypointNames(const ScenarioState* scenarioState);

/// Unique, non-empty route names. If requirePoints is true, routes with
/// no points are excluded.
QStringList availableRouteNames(const ScenarioState* scenarioState,
                                bool requirePoints = false);

/// Unique, non-empty area names (falls back to area.id if name is empty).
QStringList availableAreaNames(const ScenarioState* scenarioState);

} // namespace application
