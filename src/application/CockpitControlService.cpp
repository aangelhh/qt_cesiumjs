#include "application/CockpitControlService.h"

#include "application/ScenarioState.h"
#include "domain/EntityIdentity.h"
#include "domain/GeoMath.h"

#include <QtGlobal>

namespace application {

QString controlAuthorityLabel(EntityControlAuthority authority) {
  switch (authority) {
    case EntityControlAuthority::Task: return QStringLiteral("Task");
    case EntityControlAuthority::Plan: return QStringLiteral("Plan");
    case EntityControlAuthority::Behavior: return QStringLiteral("Behavior");
    case EntityControlAuthority::CockpitAutopilot:
      return QStringLiteral("Cockpit autopilot");
    case EntityControlAuthority::CockpitDirect:
      return QStringLiteral("Cockpit direct");
    case EntityControlAuthority::ExternalModel:
      return QStringLiteral("External model");
    case EntityControlAuthority::None:
      return QStringLiteral("None");
  }
  return QStringLiteral("None");
}

CockpitControlService::CockpitControlService(
    ScenarioState* state,
    ApplyTaskFn applyTask,
    StopPlanFn stopPlan,
    LogFn log)
    : _state(state),
      _applyTask(std::move(applyTask)),
      _stopPlan(std::move(stopPlan)),
      _log(std::move(log)) {}

bool CockpitControlService::takeControl(
    const FlightControlCommand& command) {
  if (!commandIsValid(command)) {
    return false;
  }

  if (_stopPlan) {
    _stopPlan(command.entityName, false);
  }
  if (!applyCockpitTask(command)) {
    return false;
  }

  _authorities.insert(
      command.entityName,
      EntityControlAuthority::CockpitAutopilot);
  if (_log) {
    _log(QStringLiteral("Cockpit control acquired for %1.")
             .arg(command.entityName));
  }
  return true;
}

bool CockpitControlService::updateSetpoints(
    const FlightControlCommand& command) {
  if (!hasControl(command.entityName) || !commandIsValid(command)) {
    return false;
  }
  return applyCockpitTask(command);
}

bool CockpitControlService::releaseControl(const QString& entityName) {
  const QString trimmedName = entityName.trimmed();
  if (!hasControl(trimmedName) || !_state) {
    return false;
  }

  // There is no direct/manual pilot authority yet. Releasing the screen must
  // therefore retain the last autopilot task; clearing it would set airspeed
  // to zero and leave an airborne entity without a safe flight intent.
  _authorities.remove(trimmedName);
  if (_log) {
    _log(QStringLiteral(
             "Cockpit control released for %1; last flight setpoints retained.")
             .arg(trimmedName));
  }
  return true;
}

bool CockpitControlService::hasControl(const QString& entityName) const {
  return authorityFor(entityName) == EntityControlAuthority::CockpitAutopilot;
}

EntityControlAuthority CockpitControlService::authorityFor(
    const QString& entityName) const {
  return _authorities.value(
      entityName.trimmed(),
      EntityControlAuthority::None);
}

void CockpitControlService::reconcile(
    const QString& entityName,
    const EntityTask& activeTask) {
  if (!hasControl(entityName)) {
    return;
  }
  if (!activeTask.enabled ||
      activeTask.taskType != QStringLiteral("FlyHeadingAltitudeSpeed")) {
    _authorities.remove(entityName.trimmed());
  }
}

bool CockpitControlService::applyCockpitTask(
    const FlightControlCommand& command) {
  if (!_applyTask) {
    return false;
  }

  EntityTask task;
  task.enabled = true;
  task.status = QStringLiteral("Running");
  task.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  task.targetHeadingDegrees =
      domain::normalizeDegrees360(command.headingDegrees);
  task.targetAltitudeMeters = qMax(0, command.altitudeMeters);
  task.targetSpeedKnots = qMax(0.0, command.speedKnots);
  return _applyTask(command.entityName.trimmed(), task, true);
}

bool CockpitControlService::commandIsValid(
    const FlightControlCommand& command) const {
  const QString entityName = command.entityName.trimmed();
  if (!_state || entityName.isEmpty()) {
    return false;
  }

  auto lock = _state->lock();
  for (const Entity& entity : _state->entities()) {
    if (!domain::entityMatchesReference(entity, entityName)) {
      continue;
    }
    return !entity.destroyed &&
           entity.domain.compare(
               QStringLiteral("Air"),
               Qt::CaseInsensitive) == 0;
  }
  return false;
}

} // namespace application
