#include "presentation/EntityPlanExecutor.h"
#include "application/ScenarioState.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/EntityIdentity.h"
#include "domain/GeoMath.h"

#include <QSet>
#include <QVector>

namespace presentation {

EntityPlanExecutor::EntityPlanExecutor(
    ScenarioState* state,
    ApplyTaskFn    applyTask,
    LogFn          log,
    StatusFn       setStatus,
    QObject*       parent)
    : QObject(parent)
    , _state(state)
    , _applyTask(std::move(applyTask))
    , _log(std::move(log))
    , _setStatus(std::move(setStatus)) {}

QHash<QString, EntityPlan>& EntityPlanExecutor::plans() {
  return _entityPlans;
}

const QHash<QString, EntityPlan>& EntityPlanExecutor::plans() const {
  return _entityPlans;
}

EntityPlan& EntityPlanExecutor::ensurePlan(const QString& entityName) {
  return _entityPlans[entityName];
}

// ── Static display helper ────────────────────────────────────────────────────

QString EntityPlanExecutor::planStepDisplayLabel(const PlanStep& step) {
  if (!step.label.trimmed().isEmpty()) {
    QString label = step.label;
    if (step.kind == PlanStepKind::InterceptEntity ||
        step.kind == PlanStepKind::InterceptEntity2D ||
        step.kind == PlanStepKind::InterceptEntity3D) {
      label.replace(QStringLiteral("Intercept Entity 2D"), QStringLiteral("Intercept Entity"));
      label.replace(QStringLiteral("Intercept Entity 3D"), QStringLiteral("Intercept Entity"));
    }
    return label;
  }

  switch (step.kind) {
    case PlanStepKind::MoveToLocation:       return QStringLiteral("Move To Location");
    case PlanStepKind::WaitOnLocation:       return QStringLiteral("Wait on Location");
    case PlanStepKind::MoveToWaypoint:       return QStringLiteral("Move To Waypoint");
    case PlanStepKind::FollowRoute:
    case PlanStepKind::MoveAlongRoute:       return QStringLiteral("Follow Route");
    case PlanStepKind::PatrolArea:           return QStringLiteral("Patrol Area");
    case PlanStepKind::FlyHeadingAltitudeSpeed: return QStringLiteral("Fly Heading / Altitude / Speed");
    case PlanStepKind::OrbitHoldLocation:    return QStringLiteral("Orbit / Hold (Location)");
    case PlanStepKind::HoldRacetrack:        return QStringLiteral("Hold Racetrack");
    case PlanStepKind::FollowEntity:         return QStringLiteral("Follow Entity");
    case PlanStepKind::InterceptEntity:
    case PlanStepKind::InterceptEntity2D:
    case PlanStepKind::InterceptEntity3D:    return QStringLiteral("Intercept Entity");
    case PlanStepKind::ReturnToBase:         return QStringLiteral("Return To Base");
    case PlanStepKind::AttackOnce:           return QStringLiteral("Attack Once");
    case PlanStepKind::AttackUntilDestroyed: return QStringLiteral("Attack Until Destroyed");
    case PlanStepKind::FireOnPosition:       return QStringLiteral("Fire on Position");
    case PlanStepKind::FireInDirection:      return QStringLiteral("Fire in Direction");
    case PlanStepKind::StopWeaponsTask:      return QStringLiteral("Stop Weapons Task");
    case PlanStepKind::AttackAir:            return QStringLiteral("Attack Air");
    case PlanStepKind::AttackSurface:        return QStringLiteral("Attack Surface");
    case PlanStepKind::WaitUntilTargetDetected: return QStringLiteral("Wait Until Target Detected");
    case PlanStepKind::WaitUntilTargetDestroyed: return QStringLiteral("Wait Until Target Destroyed");
    case PlanStepKind::WaitUntilDamaged:     return QStringLiteral("Wait Until Damaged");
    case PlanStepKind::WaitUntilTime:        return QStringLiteral("Wait Until Time");
    case PlanStepKind::WaitUntilInRange:     return QStringLiteral("Wait Until In Range");
  }
  return QStringLiteral("Plan Step");
}

// ── Public plan lifecycle ─────────────────────────────────────────────────────

bool EntityPlanExecutor::startPlan(const QString& entityName) {
  const Entity* entity = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { entity = &e; break; }
  }
  if (!entity || entity->destroyed) {
    return false;
  }

  EntityPlan& plan = ensurePlan(entityName);
  if (plan.steps.isEmpty()) {
    _setStatus(QStringLiteral("El plan esta vacio para %1.").arg(entityName));
    return false;
  }

  for (int index = 0; index < plan.steps.size(); ++index) {
    QString invalidReason;
    if (!validatePlanStep(plan.steps.at(index), &invalidReason)) {
      plan.running = false;
      plan.currentStepIndex = -1;
      plan.currentStableTicks = 0;
      const QString stepLabel = planStepDisplayLabel(plan.steps.at(index));
      _log(QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
               .arg(entityName, stepLabel, invalidReason));
      _setStatus(QStringLiteral("Plan detenido para %1: step invalido (%2).")
                     .arg(entityName, stepLabel));
      return false;
    }
  }

  plan.running = true;
  plan.status = QString(plan_status::Running);
  plan.currentStepIndex = 0;
  plan.currentStableTicks = 0;
  for (PlanStep& step : plan.steps) {
    step.status = QString(plan_status::NotStarted);
  }
  if (!startPlanStepTask(entityName, plan)) {
    return false;
  }

  _log(QStringLiteral("Plan started for %1").arg(entityName));
  _setStatus(QStringLiteral("Plan en ejecucion para %1.").arg(entityName));
  return true;
}

void EntityPlanExecutor::stopPlan(const QString& entityName, bool clearCurrentTask) {
  auto it = _entityPlans.find(entityName);
  if (it == _entityPlans.end()) {
    return;
  }

  it->running = false;
  it->currentStepIndex = -1;
  it->currentStableTicks = 0;
  if (it->status == QString(plan_status::Running)) {
    it->status = QString(plan_status::NotStarted);
  }

  if (clearCurrentTask) {
    _state->clearTask(entityName);
    // Caller is responsible for syncScenarioStateToUi() after this call.
  }
}

void EntityPlanExecutor::prunePlans() {
  for (auto it = _entityPlans.begin(); it != _entityPlans.end();) {
    bool entityExists = false;
    for (const Entity& entity : _state->entities()) {
      if (domain::entityMatchesReference(entity, it.key())) {
        entityExists = true;
        break;
      }
    }
    if (!entityExists) {
      it = _entityPlans.erase(it);
      continue;
    }
    ++it;
  }
}

void EntityPlanExecutor::advancePlans() {
  for (auto it = _entityPlans.begin(); it != _entityPlans.end(); ++it) {
    const QString entityName = it.key();
    EntityPlan& plan = it.value();
    if (!plan.running) {
      continue;
    }

    const Entity* entity = nullptr;
    for (const Entity& e : _state->entities()) {
      if (domain::entityMatchesReference(e, entityName)) { entity = &e; break; }
    }
    if (!entity || entity->destroyed) {
      failRunningPlan(entityName, plan);
      continue;
    }

    if (plan.currentStepIndex < 0 || plan.currentStepIndex >= plan.steps.size()) {
      failRunningPlan(entityName, plan);
      continue;
    }

    PlanStep& activeStep = plan.steps[plan.currentStepIndex];
    activeStep.status = QString(plan_status::Running);
    QString invalidReason;
    if (!validatePlanStep(activeStep, &invalidReason)) {
      failRunningPlan(
          entityName, plan,
          QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
              .arg(entityName, planStepDisplayLabel(activeStep), invalidReason),
          QStringLiteral("Plan detenido para %1: step invalido.").arg(entityName));
      continue;
    }

    if (activePlanStepCompleted(*entity, plan)) {
      const QString completedLabel = planStepDisplayLabel(activeStep);
      activeStep.status = QString(plan_status::Completed);
      ++plan.currentStepIndex;
      plan.currentStableTicks = 0;

      if (plan.currentStepIndex >= plan.steps.size()) {
        completeRunningPlan(entityName, plan, completedLabel);
        continue;
      }

      PlanStep& nextStep = plan.steps[plan.currentStepIndex];
      QString nextInvalidReason;
      if (!validatePlanStep(nextStep, &nextInvalidReason)) {
        failRunningPlan(
            entityName, plan,
            QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
                .arg(entityName, planStepDisplayLabel(nextStep), nextInvalidReason),
            QStringLiteral("Plan detenido para %1: step invalido.").arg(entityName));
        continue;
      }

      const QString nextLabel = planStepDisplayLabel(nextStep);
      if (!startPlanStepTask(entityName, plan)) {
        failRunningPlan(
            entityName, plan,
            QStringLiteral("Plan halted for %1 while starting step %2.").arg(entityName, nextLabel),
            QStringLiteral("Plan fallido para %1: no se pudo arrancar step %2.")
                .arg(entityName, nextLabel));
        continue;
      }

      _log(QStringLiteral("Plan advanced for %1: %2").arg(entityName, nextLabel));
      continue;
    }

    const bool taskFailedForStep =
        entity->currentTask.status == QStringLiteral("Target unavailable") ||
        entity->currentTask.status == QStringLiteral("Failed");
    if (taskFailedForStep) {
      const QString failedLabel = planStepDisplayLabel(activeStep);
      activeStep.status = QString(plan_status::Failed);
      ++plan.currentStepIndex;
      plan.currentStableTicks = 0;

      if (plan.currentStepIndex >= plan.steps.size()) {
        completeRunningPlan(entityName, plan, failedLabel);
        continue;
      }

      PlanStep& nextStep = plan.steps[plan.currentStepIndex];
      QString nextInvalidReason;
      if (!validatePlanStep(nextStep, &nextInvalidReason)) {
        failRunningPlan(
            entityName, plan,
            QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
                .arg(entityName, planStepDisplayLabel(nextStep), nextInvalidReason),
            QStringLiteral("Plan detenido para %1: step invalido.").arg(entityName));
        continue;
      }

      const QString nextLabel = planStepDisplayLabel(nextStep);
      if (!startPlanStepTask(entityName, plan)) {
        failRunningPlan(
            entityName, plan,
            QStringLiteral("Plan halted for %1 while starting step %2.").arg(entityName, nextLabel),
            QStringLiteral("Plan fallido para %1: no se pudo arrancar step %2.")
                .arg(entityName, nextLabel));
        continue;
      }

      _log(QStringLiteral("Plan continued for %1 after failed step %2; next step: %3")
               .arg(entityName, failedLabel, nextLabel));
      continue;
    }

    if (!entity->currentTask.enabled ||
        !activeTaskMatchesPlanStep(*entity, activeStep)) {
      failRunningPlan(
          entityName, plan,
          QStringLiteral("Plan stopped for %1 after task override.").arg(entityName),
          QStringLiteral("Plan detenido para %1: task modificada manualmente.").arg(entityName));
      continue;
    }
  }
}

// ── Private helpers ───────────────────────────────────────────────────────────

bool EntityPlanExecutor::startPlanStepTask(const QString& entityName, EntityPlan& plan) {
  if (plan.currentStepIndex < 0 || plan.currentStepIndex >= plan.steps.size()) {
    return false;
  }
  PlanStep& step = plan.steps[plan.currentStepIndex];
  step.status = QString(plan_status::Running);
  step.task.elapsedSeconds = 0.0;
  if (!_applyTask(entityName, step.task, false)) {
    step.status = QString(plan_status::Failed);
    plan.running = false;
    plan.status = QString(plan_status::Failed);
    plan.currentStepIndex = -1;
    plan.currentStableTicks = 0;
    return false;
  }
  return true;
}

void EntityPlanExecutor::failRunningPlan(
    const QString& entityName,
    EntityPlan&    plan,
    const QString& logMessage,
    const QString& statusMessage) {
  if (!logMessage.isEmpty()) {
    _log(logMessage);
  }
  if (!statusMessage.isEmpty()) {
    _setStatus(statusMessage);
  }
  if (plan.currentStepIndex >= 0 && plan.currentStepIndex < plan.steps.size()) {
    plan.steps[plan.currentStepIndex].status = QString(plan_status::Failed);
  }
  plan.running = false;
  plan.status = QString(plan_status::Failed);
  plan.currentStepIndex = -1;
  plan.currentStableTicks = 0;
}

void EntityPlanExecutor::completeRunningPlan(
    const QString& entityName,
    EntityPlan&    plan,
    const QString& completedLabel) {
  plan.running = false;
  bool hasFailedSteps = false;
  for (const PlanStep& step : plan.steps) {
    if (step.status == plan_status::Failed) {
      hasFailedSteps = true;
      break;
    }
  }
  plan.status = hasFailedSteps
      ? QString(plan_status::CompletedWithFailures)
      : QString(plan_status::Completed);
  plan.currentStepIndex = -1;

  if (hasFailedSteps) {
    _log(QStringLiteral("Plan completed with failures for %1 after %2.")
             .arg(entityName, completedLabel));
    _setStatus(QStringLiteral("Plan completado con fallas para %1.").arg(entityName));
  } else {
    _log(QStringLiteral("Plan completed for %1 after %2.").arg(entityName, completedLabel));
    _setStatus(QStringLiteral("Plan completado para %1.").arg(entityName));
  }
}

bool EntityPlanExecutor::activePlanStepCompleted(
    const Entity& entity,
    EntityPlan&   plan) const {
  if (plan.currentStepIndex < 0 || plan.currentStepIndex >= plan.steps.size()) {
    return false;
  }

  const PlanStep& step = plan.steps.at(plan.currentStepIndex);
  auto mutableEntityTask = [&]() -> EntityTask* {
    for (Entity& mutableEntity : _state->entitiesMutable()) {
      if (domain::entityKey(mutableEntity) == domain::entityKey(entity)) {
        return &mutableEntity.currentTask;
      }
    }
    return nullptr;
  };
  auto markConditionStatus = [&](const QString& status) {
    if (EntityTask* task = mutableEntityTask()) {
      task->status = status;
    }
  };
  auto tickConditionElapsed = [&]() -> double {
    if (EntityTask* task = mutableEntityTask()) {
      task->elapsedSeconds += 1.0;
      return task->elapsedSeconds;
    }
    return step.task.elapsedSeconds;
  };
  auto targetByName = [&](const QString& targetName) -> const Entity* {
    const QString trimmed = targetName.trimmed();
    if (trimmed.isEmpty()) {
      return nullptr;
    }
    for (const Entity& candidate : _state->entities()) {
      if (domain::entityMatchesReference(candidate, trimmed)) {
        return &candidate;
      }
    }
    return nullptr;
  };
  auto targetDomainMatches = [](const QString& filter, const Entity& target) {
    const QString trimmed = filter.trimmed();
    return trimmed.isEmpty() ||
           trimmed.compare(QStringLiteral("Any"), Qt::CaseInsensitive) == 0 ||
           target.domain.compare(trimmed, Qt::CaseInsensitive) == 0 ||
           target.category.compare(trimmed, Qt::CaseInsensitive) == 0;
  };
  auto timedOut = [&](double timeoutSeconds) {
    return timeoutSeconds > 0.0 && tickConditionElapsed() >= timeoutSeconds;
  };

  switch (step.kind) {
    case PlanStepKind::WaitUntilTargetDetected: {
      const QString configuredTarget = domain::targetEntityReference(step.task);
      for (const SensorContact& contact : entity.sensorContacts) {
        if (!contact.detected) {
          continue;
        }
        const QString contactTarget = contact.targetEntityId.trimmed().isEmpty()
            ? contact.targetEntityName.trimmed()
            : contact.targetEntityId.trimmed();
        if (!configuredTarget.isEmpty() &&
            contactTarget.compare(configuredTarget, Qt::CaseInsensitive) != 0) {
          continue;
        }
        const Entity* target = targetByName(contactTarget);
        if (!target || target->destroyed) {
          continue;
        }
        if (step.task.enemyOnly && target->forceIdentifier == entity.forceIdentifier) {
          continue;
        }
        if (!targetDomainMatches(step.task.targetDomain, *target)) {
          continue;
        }
        markConditionStatus(QStringLiteral("Completed"));
        return true;
      }
      if (timedOut(step.task.timeoutSeconds)) {
        markConditionStatus(QStringLiteral("Failed"));
      } else {
        markConditionStatus(QStringLiteral("Running"));
      }
      return false;
    }

    case PlanStepKind::WaitUntilTargetDestroyed: {
      const Entity* target = targetByName(domain::targetEntityReference(step.task));
      if (!target) {
        markConditionStatus(QStringLiteral("Failed"));
        return false;
      }
      if (target->destroyed) {
        markConditionStatus(QStringLiteral("Completed"));
        return true;
      }
      if (timedOut(step.task.timeoutSeconds)) {
        markConditionStatus(QStringLiteral("Failed"));
      } else {
        markConditionStatus(QStringLiteral("Running"));
      }
      return false;
    }

    case PlanStepKind::WaitUntilDamaged: {
      const Entity* target = targetByName(domain::targetEntityReference(step.task));
      if (!target) {
        markConditionStatus(QStringLiteral("Failed"));
        return false;
      }
      if (target->damagePercent >= step.task.damageThresholdPercent) {
        markConditionStatus(QStringLiteral("Completed"));
        return true;
      }
      if (timedOut(step.task.timeoutSeconds)) {
        markConditionStatus(QStringLiteral("Failed"));
      } else {
        markConditionStatus(QStringLiteral("Running"));
      }
      return false;
    }

    case PlanStepKind::WaitUntilTime: {
      if (tickConditionElapsed() >= step.task.durationSeconds) {
        markConditionStatus(QStringLiteral("Completed"));
        return true;
      }
      markConditionStatus(QStringLiteral("Running"));
      return false;
    }

    case PlanStepKind::WaitUntilInRange: {
      const Entity* target = targetByName(domain::targetEntityReference(step.task));
      if (!target || target->destroyed) {
        markConditionStatus(QStringLiteral("Failed"));
        return false;
      }
      const double distanceMeters = domain::distanceMeters(
          entity.latitude, entity.longitude, target->latitude, target->longitude);
      if (distanceMeters <= step.task.rangeMeters) {
        markConditionStatus(QStringLiteral("Completed"));
        return true;
      }
      if (timedOut(step.task.timeoutSeconds)) {
        markConditionStatus(QStringLiteral("Failed"));
      } else {
        markConditionStatus(QStringLiteral("Running"));
      }
      return false;
    }

    default:
      break;
  }

  switch (step.kind) {
    case PlanStepKind::MoveToLocation:
    case PlanStepKind::WaitOnLocation:
    case PlanStepKind::MoveToWaypoint:
    case PlanStepKind::ReturnToBase:
      plan.currentStableTicks = 0;
      return step.kind == PlanStepKind::WaitOnLocation
          ? entity.currentTask.status == QStringLiteral("Completed")
          : entity.currentTask.status == QStringLiteral("On target");
    case PlanStepKind::FollowRoute:
    case PlanStepKind::MoveAlongRoute:
      plan.currentStableTicks = 0;
      return entity.currentTask.status == QStringLiteral("Completed") ||
             entity.currentTask.status == QStringLiteral("On target");

    case PlanStepKind::PatrolArea: {
      const double distanceToCenterMeters = domain::distanceMeters(
          entity.latitude, entity.longitude,
          step.task.targetLatitude, step.task.targetLongitude);
      const double holdDistanceMeters = qMax(100.0, step.task.targetAreaRadiusMeters * 1.15);
      if (distanceToCenterMeters <= holdDistanceMeters) {
        ++plan.currentStableTicks;
      } else {
        plan.currentStableTicks = 0;
      }
      return plan.currentStableTicks >= 3;
    }

    case PlanStepKind::FlyHeadingAltitudeSpeed: {
      const double headingErrorDegrees = qAbs(domain::shortestSignedAngle(
          entity.headingDegrees, step.task.targetHeadingDegrees));
      const int altitudeErrorMeters = qAbs(entity.altitude - step.task.targetAltitudeMeters);
      const double speedErrorKnots = qAbs(entity.speedKnots - step.task.targetSpeedKnots);
      if (headingErrorDegrees <= 5.0 && altitudeErrorMeters <= 50 && speedErrorKnots <= 10.0) {
        ++plan.currentStableTicks;
      } else {
        plan.currentStableTicks = 0;
      }
      return plan.currentStableTicks >= 3;
    }

    case PlanStepKind::OrbitHoldLocation: {
      const double distanceToCenterMeters = domain::distanceMeters(
          entity.latitude, entity.longitude,
          step.task.targetLatitude, step.task.targetLongitude);
      const double holdDistanceMeters = qMax(100.0, step.task.targetAreaRadiusMeters * 1.15);
      if (distanceToCenterMeters <= holdDistanceMeters) {
        ++plan.currentStableTicks;
      } else {
        plan.currentStableTicks = 0;
      }
      return plan.currentStableTicks >= 3;
    }

    case PlanStepKind::AttackAir:
    case PlanStepKind::AttackOnce:
    case PlanStepKind::AttackUntilDestroyed:
    case PlanStepKind::FireOnPosition:
    case PlanStepKind::FireInDirection:
    case PlanStepKind::StopWeaponsTask:
    case PlanStepKind::AttackSurface:
    case PlanStepKind::HoldRacetrack:
    case PlanStepKind::FollowEntity:
    case PlanStepKind::InterceptEntity:
    case PlanStepKind::InterceptEntity2D:
    case PlanStepKind::InterceptEntity3D:
    case PlanStepKind::WaitUntilTargetDetected:
    case PlanStepKind::WaitUntilTargetDestroyed:
    case PlanStepKind::WaitUntilDamaged:
    case PlanStepKind::WaitUntilTime:
    case PlanStepKind::WaitUntilInRange:
      plan.currentStableTicks = 0;
      return entity.currentTask.status == QStringLiteral("Completed");
  }

  return false;
}

bool EntityPlanExecutor::validatePlanStep(const PlanStep& step, QString* reason) const {
  auto setReason = [reason](const QString& text) {
    if (reason) { *reason = text; }
    return false;
  };

  if (step.task.taskType.trimmed().isEmpty()) {
    return setReason(QStringLiteral("step task type is empty"));
  }
  const QString targetReference = domain::targetEntityReference(step.task);
  const auto targetExists = [&]() {
    for (const Entity& candidate : _state->entities()) {
      if (domain::entityMatchesReference(candidate, targetReference)) {
        return true;
      }
    }
    return false;
  };

  switch (step.kind) {
    case PlanStepKind::MoveToWaypoint: {
      if (step.task.targetWaypointName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("waypoint is not set"));
      }
      bool found = false;
      for (const auto& w : _state->waypoints()) {
        if (w.name == step.task.targetWaypointName) { found = true; break; }
      }
      if (!found) {
        return setReason(
            QStringLiteral("waypoint '%1' no longer exists")
                .arg(step.task.targetWaypointName));
      }
      return true;
    }

    case PlanStepKind::FollowRoute:
    case PlanStepKind::MoveAlongRoute: {
      if (step.task.targetRouteName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("route is not set"));
      }
      const RouteGraphic* route = nullptr;
      for (const auto& r : _state->routes()) {
        if (r.name == step.task.targetRouteName) { route = &r; break; }
      }
      if (!route) {
        return setReason(
            QStringLiteral("route '%1' no longer exists").arg(step.task.targetRouteName));
      }
      if (route->points.size() < 2) {
        return setReason(
            QStringLiteral("route '%1' has fewer than two points").arg(step.task.targetRouteName));
      }
      return true;
    }

    case PlanStepKind::PatrolArea: {
      if (step.task.targetAreaName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("area is not set"));
      }
      bool found = false;
      for (const auto& a : _state->areas()) {
        if (a.name == step.task.targetAreaName || a.id == step.task.targetAreaName) {
          found = true; break;
        }
      }
      if (!found) {
        return setReason(
            QStringLiteral("area '%1' no longer exists").arg(step.task.targetAreaName));
      }
      return true;
    }

    case PlanStepKind::MoveToLocation:
    case PlanStepKind::WaitOnLocation:
    case PlanStepKind::FlyHeadingAltitudeSpeed:
    case PlanStepKind::OrbitHoldLocation:
    case PlanStepKind::HoldRacetrack:
    case PlanStepKind::ReturnToBase:
      return true;

    case PlanStepKind::FollowEntity: {
      if (targetReference.isEmpty()) {
        return setReason(QStringLiteral("follow target is not set"));
      }
      return true;
    }

    case PlanStepKind::InterceptEntity:
    case PlanStepKind::InterceptEntity2D:
    case PlanStepKind::InterceptEntity3D: {
      if (targetReference.isEmpty()) {
        return setReason(QStringLiteral("intercept target is not set"));
      }
      return true;
    }

    case PlanStepKind::AttackAir: {
      if (targetReference.isEmpty()) {
        return setReason(QStringLiteral("air target is not set"));
      }
      if (!targetExists()) {
        return setReason(
            QStringLiteral("air target '%1' no longer exists")
                .arg(step.task.targetEntityName.trimmed()));
      }
      return true;
    }

    case PlanStepKind::AttackOnce: {
      if (targetReference.isEmpty()) {
        return setReason(QStringLiteral("attack target is not set"));
      }
      if (!targetExists()) {
        return setReason(
            QStringLiteral("attack target '%1' no longer exists")
                .arg(step.task.targetEntityName.trimmed()));
      }
      return true;
    }

    case PlanStepKind::AttackUntilDestroyed: {
      if (targetReference.isEmpty()) {
        return setReason(QStringLiteral("attack target is not set"));
      }
      if (!targetExists()) {
        return setReason(
            QStringLiteral("attack target '%1' no longer exists")
                .arg(step.task.targetEntityName.trimmed()));
      }
      if (step.task.maxEngagementTimeSeconds < 0.0) {
        return setReason(QStringLiteral("max engagement time must be non-negative"));
      }
      if (step.task.shotCooldownSeconds < 0.0) {
        return setReason(QStringLiteral("shot cooldown must be non-negative"));
      }
      return true;
    }

    case PlanStepKind::FireOnPosition: {
      if (!domain::attackSurfaceCoordinatesAreUsable(
              step.task.targetLatitude, step.task.targetLongitude)) {
        return setReason(QStringLiteral("fire position coordinates are not set"));
      }
      return true;
    }

    case PlanStepKind::FireInDirection:
      if (step.task.durationSeconds < 0.0) {
        return setReason(QStringLiteral("duration must be non-negative"));
      }
      return true;

    case PlanStepKind::StopWeaponsTask:
      return true;

    case PlanStepKind::AttackSurface: {
      if (!targetReference.isEmpty()) {
        if (!targetExists()) {
          return setReason(
              QStringLiteral("surface target '%1' no longer exists")
                  .arg(step.task.targetEntityName.trimmed()));
        }
        return true;
      }
      if (!domain::attackSurfaceCoordinatesAreUsable(
              step.task.targetLatitude, step.task.targetLongitude)) {
        return setReason(QStringLiteral("surface target coordinates are not set"));
      }
      return true;
    }

    case PlanStepKind::WaitUntilTargetDetected:
      if (step.task.timeoutSeconds < 0.0) {
        return setReason(QStringLiteral("timeout must be non-negative"));
      }
      return true;

    case PlanStepKind::WaitUntilTargetDestroyed:
    case PlanStepKind::WaitUntilDamaged:
    case PlanStepKind::WaitUntilInRange: {
      if (targetReference.isEmpty()) {
        return setReason(QStringLiteral("target entity is not set"));
      }
      if (step.kind == PlanStepKind::WaitUntilDamaged &&
          (step.task.damageThresholdPercent < 0.0 || step.task.damageThresholdPercent > 100.0)) {
        return setReason(QStringLiteral("damage threshold must be between 0 and 100"));
      }
      if (step.kind == PlanStepKind::WaitUntilInRange && step.task.rangeMeters <= 0.0) {
        return setReason(QStringLiteral("range must be greater than zero"));
      }
      if (step.task.timeoutSeconds < 0.0) {
        return setReason(QStringLiteral("timeout must be non-negative"));
      }
      return true;
    }

    case PlanStepKind::WaitUntilTime:
      if (step.task.durationSeconds <= 0.0) {
        return setReason(QStringLiteral("duration must be greater than zero"));
      }
      return true;
  }

  return true;
}

bool EntityPlanExecutor::activeTaskMatchesPlanStep(
    const Entity& entity,
    const PlanStep& step) const {
  const EntityTask& currentTask = entity.currentTask;
  const auto isInterceptType = [](const QString& taskType) {
    return taskType == QStringLiteral("InterceptEntity") ||
           taskType == QStringLiteral("InterceptEntity2D") ||
           taskType == QStringLiteral("InterceptEntity3D");
  };
  const auto isRouteType = [](const QString& taskType) {
    return taskType == QStringLiteral("FollowRoute") ||
           taskType == QStringLiteral("MoveAlongRoute");
  };

  if (currentTask.taskType != step.task.taskType &&
      !(isInterceptType(currentTask.taskType) && isInterceptType(step.task.taskType)) &&
      !(isRouteType(currentTask.taskType) && isRouteType(step.task.taskType))) {
    return false;
  }

  auto nearlyEqual = [](double left, double right, double epsilon) {
    return qAbs(left - right) <= epsilon;
  };
  const bool sameTarget =
      domain::targetEntityReference(currentTask).compare(
          domain::targetEntityReference(step.task),
          Qt::CaseInsensitive) == 0;

  switch (step.kind) {
    case PlanStepKind::MoveToLocation:
    case PlanStepKind::WaitOnLocation:
    case PlanStepKind::ReturnToBase:
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1) &&
             (step.kind != PlanStepKind::WaitOnLocation ||
              (nearlyEqual(currentTask.arrivalToleranceMeters, step.task.arrivalToleranceMeters, 1.0) &&
               nearlyEqual(currentTask.durationSeconds, step.task.durationSeconds, 0.1)));

    case PlanStepKind::MoveToWaypoint:
      return currentTask.targetWaypointName == step.task.targetWaypointName &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::FollowRoute:
    case PlanStepKind::MoveAlongRoute:
      return currentTask.targetRouteName == step.task.targetRouteName &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1) &&
             nearlyEqual(currentTask.arrivalToleranceMeters, step.task.arrivalToleranceMeters, 1.0) &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);

    case PlanStepKind::PatrolArea:
      return currentTask.targetAreaName == step.task.targetAreaName &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::FlyHeadingAltitudeSpeed:
      return nearlyEqual(currentTask.targetHeadingDegrees, step.task.targetHeadingDegrees, 0.1) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::OrbitHoldLocation:
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetAreaRadiusMeters, step.task.targetAreaRadiusMeters, 1.0) &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::HoldRacetrack:
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetHeadingDegrees, step.task.targetHeadingDegrees, 0.1) &&
             nearlyEqual(currentTask.racetrackLegLengthMeters, step.task.racetrackLegLengthMeters, 1.0) &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1) &&
             nearlyEqual(currentTask.durationSeconds, step.task.durationSeconds, 0.1);

    case PlanStepKind::FollowEntity:
      return sameTarget &&
             nearlyEqual(currentTask.followDistanceMeters, step.task.followDistanceMeters, 1.0) &&
             nearlyEqual(currentTask.arrivalToleranceMeters, step.task.arrivalToleranceMeters, 1.0);

    case PlanStepKind::InterceptEntity:
    case PlanStepKind::InterceptEntity2D:
    case PlanStepKind::InterceptEntity3D:
      return sameTarget &&
             nearlyEqual(currentTask.interceptDistanceMeters, step.task.interceptDistanceMeters, 1.0) &&
             nearlyEqual(currentTask.altitudeToleranceMeters, step.task.altitudeToleranceMeters, 1.0) &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);

    case PlanStepKind::AttackAir:
      return sameTarget;

    case PlanStepKind::AttackOnce:
      return sameTarget &&
             currentTask.weaponType == step.task.weaponType &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);

    case PlanStepKind::AttackUntilDestroyed:
      return sameTarget &&
             currentTask.weaponType == step.task.weaponType &&
             nearlyEqual(currentTask.maxEngagementTimeSeconds,
                         step.task.maxEngagementTimeSeconds, 0.1) &&
             nearlyEqual(currentTask.shotCooldownSeconds, step.task.shotCooldownSeconds, 0.1);

    case PlanStepKind::FireOnPosition:
      return currentTask.weaponType == step.task.weaponType &&
             nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters;

    case PlanStepKind::FireInDirection:
      return currentTask.weaponType == step.task.weaponType &&
             nearlyEqual(currentTask.targetHeadingDegrees, step.task.targetHeadingDegrees, 0.1) &&
             nearlyEqual(currentTask.durationSeconds, step.task.durationSeconds, 0.1);

    case PlanStepKind::StopWeaponsTask:
      return true;

    case PlanStepKind::AttackSurface:
      if (!domain::targetEntityReference(step.task).isEmpty()) {
        return sameTarget;
      }
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters;

    case PlanStepKind::WaitUntilTargetDetected:
      return sameTarget &&
             currentTask.targetDomain == step.task.targetDomain &&
             currentTask.enemyOnly == step.task.enemyOnly &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);

    case PlanStepKind::WaitUntilTargetDestroyed:
      return sameTarget &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);

    case PlanStepKind::WaitUntilDamaged:
      return sameTarget &&
             nearlyEqual(currentTask.damageThresholdPercent, step.task.damageThresholdPercent, 0.1) &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);

    case PlanStepKind::WaitUntilTime:
      return nearlyEqual(currentTask.durationSeconds, step.task.durationSeconds, 0.1);

    case PlanStepKind::WaitUntilInRange:
      return sameTarget &&
             nearlyEqual(currentTask.rangeMeters, step.task.rangeMeters, 1.0) &&
             nearlyEqual(currentTask.timeoutSeconds, step.task.timeoutSeconds, 0.1);
  }

  return false;
}

} // namespace presentation
