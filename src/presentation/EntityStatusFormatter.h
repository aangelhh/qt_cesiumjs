#pragma once

#include <QString>
#include <QVariantMap>
#include <QVector>

#include "domain/Entity.h"
#include "domain/Munition.h"
#include "presentation/PlanTypes.h"

namespace presentation {

class EntityPlanExecutor;

/// Context needed to format a full operational status string for an entity.
struct EntityStatusContext {
  const QVector<Entity>& entities;
  const QVector<ActiveMunition>& activeMunitions;
  const PendingBombRelease& pendingRelease;
  const QHash<QString, EntityPlan>& plans;
  bool simulationRunning = false;
};

/// Build a multi-line operational status string for display in the detail panel.
/// Pure function — no side effects.
QString buildEntityOperationalStatus(
    const QVariantMap& summary,
    const Entity* entity,
    const EntityStatusContext& ctx);

} // namespace presentation
