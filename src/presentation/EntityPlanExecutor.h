#pragma once

#include "presentation/PlanTypes.h"
#include <QHash>
#include <QObject>
#include <QString>
#include <functional>

class ScenarioState;
struct Entity;
struct EntityTask;

namespace presentation {

/// Owns the entity plan map and executes plan advancement logic.
/// Side effects (logging, status updates, task application) are injected
/// as callbacks to keep this class decoupled from Qt UI types.
class EntityPlanExecutor : public QObject {
  Q_OBJECT
public:
  using ApplyTaskFn = std::function<bool(const QString&, const EntityTask&, bool)>;
  using LogFn       = std::function<void(const QString&)>;
  using StatusFn    = std::function<void(const QString&)>;

  explicit EntityPlanExecutor(
      ScenarioState* state,
      ApplyTaskFn    applyTask,
      LogFn          log,
      StatusFn       setStatus,
      QObject*       parent = nullptr);

  /// All plans — accessible for UI rendering and serialisation.
  QHash<QString, EntityPlan>&       plans();
  const QHash<QString, EntityPlan>& plans() const;

  /// Return (or create) the plan for an entity.
  EntityPlan& ensurePlan(const QString& entityName);

  /// Start running the plan. Returns false if the plan is empty or invalid.
  bool startPlan(const QString& entityName);

  /// Stop a running plan. When clearCurrentTask is true the entity's current
  /// task is also cleared in ScenarioState (UI sync must be done by caller).
  void stopPlan(const QString& entityName, bool clearCurrentTask = false);

  /// Advance all running plans by one simulation tick.
  void advancePlans();

  /// Remove plans for entities that are no longer in the scenario.
  void prunePlans();

  /// Pure display helper — no state dependency.
  static QString planStepDisplayLabel(const PlanStep& step);

private:
  bool startPlanStepTask(const QString& entityName, EntityPlan& plan);
  void failRunningPlan(
      const QString& entityName,
      EntityPlan&    plan,
      const QString& logMessage    = {},
      const QString& statusMessage = {});
  void completeRunningPlan(
      const QString& entityName,
      EntityPlan&    plan,
      const QString& completedLabel);
  bool activePlanStepCompleted(const Entity& entity, EntityPlan& plan) const;
  bool validatePlanStep(const PlanStep& step, QString* reason) const;
  bool activeTaskMatchesPlanStep(const Entity& entity, const PlanStep& step) const;

  ScenarioState*             _state;
  ApplyTaskFn                _applyTask;
  LogFn                      _log;
  StatusFn                   _setStatus;
  QHash<QString, EntityPlan> _entityPlans;
};

} // namespace presentation
