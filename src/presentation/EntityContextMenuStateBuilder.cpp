#include "presentation/EntityContextMenuStateBuilder.h"

#include "application/ScenarioQueries.h"
#include "domain/CombatRules.h"
#include "domain/BombReleaseGate.h"
#include "domain/Entity.h"

#include <QVariantMap>

namespace presentation {

QStringList entityBehaviorModeOptions() {
  return {
      QStringLiteral("Manual"),
      QStringLiteral("Aggressive"),
      QStringLiteral("Defensive"),
      QStringLiteral("Patrol"),
  };
}

EntityContextMenuState buildEntityContextMenuState(
    const Entity*              entity,
    const ScenarioState*       state,
    const PendingBombRelease&  pendingRelease,
    bool                       simulationRunning,
    bool                       entityDestroyed,
    const QVariantMap&         summary)
{
  EntityContextMenuState result;
  result.entityDestroyed   = entityDestroyed;
  result.simulationRunning = simulationRunning;
  result.entityName        = entity ? entity->name : QString{};

  result.canUseWeapons = entity && domain::entityCanUseMissileActions(*entity);
  result.missileCount  = entity ? domain::weaponQuantity(*entity, QStringLiteral("Missile")) : 0;
  result.bombCount     = entity ? domain::weaponQuantity(*entity, QStringLiteral("Bomb"))    : 0;
  result.detectedMissileTargetCount =
      entity ? application::detectedMissileTargetsInRange(state, *entity).size() : 0;

  result.bombReleasePendingForThisEntity =
      pendingRelease.pending &&
      entity &&
      pendingRelease.launcherEntityName.compare(entity->name, Qt::CaseInsensitive) == 0;

  result.currentBehaviorMode = entity && !entity->behaviorMode.trimmed().isEmpty()
      ? entity->behaviorMode.trimmed()
      : QStringLiteral("Manual");
  result.behaviorModeOptions = entityBehaviorModeOptions();

  result.hidden               = summary.value(QStringLiteral("hidden")).toBool();
  result.radarCoverageVisible = summary.value(QStringLiteral("radarCoverageVisible")).toBool();
  result.trackHistoryVisible  = summary.value(QStringLiteral("trackHistoryVisible")).toBool();

  return result;
}

} // namespace presentation
