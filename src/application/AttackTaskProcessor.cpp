#include "application/AttackTaskProcessor.h"
#include "application/ScenarioQueries.h"
#include "application/ScenarioState.h"
#include "domain/BombReleaseGate.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/GeoMath.h"
#include "presentation/BombReleaseController.h"

namespace application {

AttackTaskProcessor::AttackTaskProcessor(
    ScenarioState* state,
    presentation::BombReleaseController* bombCtrl,
    LogFn         log,
    StatusFn      setStatus,
    QueueBombFn   queueBomb,
    QObject*      parent)
    : QObject(parent)
    , _state(state)
    , _bombCtrl(bombCtrl)
    , _log(std::move(log))
    , _setStatus(std::move(setStatus))
    , _queueBomb(std::move(queueBomb)) {}

// ── Public API ────────────────────────────────────────────────────────────────

void AttackTaskProcessor::processAttackTasks(double deltaSeconds, bool simulationRunning) {
  if (!simulationRunning) {
    return;
  }

  if (deltaSeconds > 0.0) {
    for (auto it = _attackAirMissileCooldownSeconds.begin();
         it != _attackAirMissileCooldownSeconds.end();) {
      it.value() = qMax(0.0, it.value() - deltaSeconds);
      if (it.value() <= 0.0) {
        it = _attackAirMissileCooldownSeconds.erase(it);
        continue;
      }
      ++it;
    }
  }

  QStringList taskEntityNames;
  for (const Entity& entity : _state->entities()) {
    const QString taskType = entity.currentTask.taskType.trimmed();
    const QString taskStatus = entity.currentTask.status.trimmed();
    if (entity.destroyed) {
      _attackAirElapsedSeconds.remove(entity.name);
      _attackAirMissileCooldownSeconds.remove(entity.name);
    }
    if (taskType == QStringLiteral("AttackAir") &&
        domain::attackTaskStatusIsTerminal(taskStatus)) {
      _attackAirElapsedSeconds.remove(entity.name);
      _attackAirMissileCooldownSeconds.remove(entity.name);
    }
    if (!entity.currentTask.enabled ||
        entity.destroyed ||
        (taskType != QStringLiteral("AttackAir") &&
         taskType != QStringLiteral("AttackSurface")) ||
        domain::attackTaskStatusIsTerminal(taskStatus)) {
      continue;
    }
    taskEntityNames.push_back(entity.name);
  }

  for (const QString& entityName : taskEntityNames) {
    const Entity* entity = nullptr;
    for (const Entity& e : _state->entities()) {
      if (e.name == entityName) { entity = &e; break; }
    }
    if (!entity || entity->destroyed) {
      setEntityTaskStatus(entityName, QStringLiteral("Target unavailable"));
      continue;
    }

    const QString taskType = entity->currentTask.taskType.trimmed();
    if (taskType == QStringLiteral("AttackAir")) {
      processAttackAirTask(entityName, deltaSeconds);
    } else if (taskType == QStringLiteral("AttackSurface")) {
      processAttackSurfaceTask(entityName);
    }
  }
}

void AttackTaskProcessor::processAutoBombing(double deltaSeconds, bool simulationRunning) {
  if (deltaSeconds > 0.0) {
    for (auto it = _autoBombReleaseCooldownSeconds.begin();
         it != _autoBombReleaseCooldownSeconds.end();) {
      it.value() = qMax(0.0, it.value() - deltaSeconds);
      if (it.value() <= 0.0) {
        it = _autoBombReleaseCooldownSeconds.erase(it);
        continue;
      }
      ++it;
    }
  }

  if (!simulationRunning || _bombCtrl->isPending()) {
    return;
  }

  for (const Entity& launcher : _state->entities()) {
    const QString taskType = launcher.currentTask.taskType.trimmed();
    if (launcher.currentTask.enabled &&
        (taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackSurface"))) {
      continue;
    }

    const QString behaviorMode = launcher.behaviorMode.trimmed().isEmpty()
        ? QStringLiteral("Manual")
        : launcher.behaviorMode.trimmed();
    if (behaviorMode.compare(QStringLiteral("Aggressive"), Qt::CaseInsensitive) != 0 ||
        launcher.destroyed ||
        launcher.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0 ||
        domain::weaponQuantity(launcher, QStringLiteral("Bomb")) <= 0) {
      _autoBehaviorDamageReactionLevel.remove(launcher.name);
      continue;
    }

    const int damageReactionLevel = domain::autoBehaviorDamageReactionLevel(launcher);
    const int previousReactionLevel =
        _autoBehaviorDamageReactionLevel.value(launcher.name, -1);
    if (damageReactionLevel != previousReactionLevel) {
      _autoBehaviorDamageReactionLevel.insert(launcher.name, damageReactionLevel);
      if (damageReactionLevel >= 2) {
        _log(QStringLiteral("%1 auto bombing blocked: critical damage (%2%).")
                 .arg(launcher.name).arg(launcher.damagePercent, 0, 'f', 0));
      } else if (damageReactionLevel >= 1) {
        _log(QStringLiteral("%1 auto bombing blocked: damage threshold reached (%2%).")
                 .arg(launcher.name).arg(launcher.damagePercent, 0, 'f', 0));
      } else {
        _log(QStringLiteral("%1 auto bombing re-enabled by damage state (%2%).")
                 .arg(launcher.name).arg(launcher.damagePercent, 0, 'f', 0));
      }
    }
    if (!domain::autoBehaviorCanEngageByDamage(launcher)) {
      continue;
    }

    const auto cooldownIt = _autoBombReleaseCooldownSeconds.constFind(launcher.name);
    if (cooldownIt != _autoBombReleaseCooldownSeconds.constEnd() &&
        cooldownIt.value() > 0.0) {
      continue;
    }

    const Entity* target =
        application::bestDetectedSurfaceBombTarget(_state, launcher);
    if (!target) {
      continue;
    }

    _queueBomb(
        launcher.name,
        target->latitude,
        target->longitude,
        static_cast<double>(target->altitude),
        target->name,
        QStringLiteral("Auto Behavior"),
        target->name,
        false,
        false);
    _autoBombReleaseCooldownSeconds.insert(
        launcher.name, kAutoBombReleaseCooldownSeconds);
    _log(QStringLiteral("%1 auto-armed bomb release at %2")
             .arg(launcher.name, target->name));
    _setStatus(QStringLiteral("%1 auto-armed bomb release at %2.")
                   .arg(launcher.name, target->name));
    return;
  }
}

void AttackTaskProcessor::removeEntity(const QString& entityName) {
  _attackAirElapsedSeconds.remove(entityName);
  _attackAirMissileCooldownSeconds.remove(entityName);
  _autoBombReleaseCooldownSeconds.remove(entityName);
  _autoBehaviorDamageReactionLevel.remove(entityName);
}

QHash<QString, double>& AttackTaskProcessor::attackAirElapsedSeconds() {
  return _attackAirElapsedSeconds;
}
QHash<QString, double>& AttackTaskProcessor::attackAirMissileCooldownSeconds() {
  return _attackAirMissileCooldownSeconds;
}
QHash<QString, double>& AttackTaskProcessor::autoBombReleaseCooldownSeconds() {
  return _autoBombReleaseCooldownSeconds;
}
QHash<QString, int>& AttackTaskProcessor::autoBehaviorDamageReactionLevel() {
  return _autoBehaviorDamageReactionLevel;
}

// ── Private helpers ───────────────────────────────────────────────────────────

bool AttackTaskProcessor::processAttackAirTask(
    const QString& entityName, double deltaSeconds) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (e.name == entityName) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString targetName = launcher->currentTask.targetEntityName.trimmed();
  if (targetName.isEmpty()) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const Entity* target = nullptr;
  for (const Entity& e : _state->entities()) {
    if (e.name == targetName) { target = &e; break; }
  }
  if (target && target->destroyed) {
    setEntityTaskStatus(entityName, QStringLiteral("Completed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Air task completed for %1: target %2 destroyed.")
             .arg(entityName, targetName));
    _setStatus(
        QStringLiteral("Attack Air completado para %1: target destruido.").arg(entityName));
    return true;
  }
  if (!target || target->name == launcher->name) {
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Air task failed for %1: target no longer valid.")
             .arg(entityName));
    return true;
  }
  if (target->forceIdentifier == launcher->forceIdentifier ||
      target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    return true;
  }

  _attackAirElapsedSeconds[entityName] =
      _attackAirElapsedSeconds.value(entityName, 0.0) + qMax(0.0, deltaSeconds);
  if (_attackAirElapsedSeconds.value(entityName) >= kAttackAirTimeoutSeconds) {
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Air task failed for %1: timeout against %2.")
             .arg(entityName, targetName));
    return true;
  }

  Entity* mutableLauncher = nullptr;
  for (Entity& e : _state->entitiesMutable()) {
    if (e.name == entityName) { mutableLauncher = &e; break; }
  }
  if (!mutableLauncher) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const double desiredHeadingDegrees = domain::bearingDegrees(
      launcher->latitude, launcher->longitude,
      target->latitude, target->longitude);
  const double desiredSpeedKnots = qMax(
      kAttackAirMinimumPursuitSpeedKnots,
      target->speedKnots + kAttackAirTargetSpeedMarginKnots);
  mutableLauncher->currentTask.targetLatitude = target->latitude;
  mutableLauncher->currentTask.targetLongitude = target->longitude;
  mutableLauncher->currentTask.targetAltitudeMeters = target->altitude;
  mutableLauncher->currentTask.targetHeadingDegrees = desiredHeadingDegrees;
  mutableLauncher->currentTask.targetSpeedKnots = desiredSpeedKnots;
  mutableLauncher->currentTask.status = QStringLiteral("Running");

  const bool activeMissile =
      application::activeMissileInFlightForTarget(_state, entityName, targetName);
  if (domain::weaponQuantity(*launcher, QStringLiteral("Missile")) <= 0) {
    if (!activeMissile) {
      setEntityTaskStatus(entityName, QStringLiteral("Failed"));
      _attackAirElapsedSeconds.remove(entityName);
      _attackAirMissileCooldownSeconds.remove(entityName);
      _log(QStringLiteral("Attack Air task failed for %1: no missiles remaining.")
               .arg(entityName));
      return true;
    }
    return false;
  }

  if (activeMissile) {
    return false;
  }

  if (_attackAirMissileCooldownSeconds.value(entityName, 0.0) > 0.0) {
    return false;
  }

  if (_state->launchMissileAt(entityName, targetName)) {
    _attackAirMissileCooldownSeconds[entityName] = kAttackAirMissileCooldownSeconds;
    _log(QStringLiteral("Attack Air task launched missile from %1 at %2")
             .arg(entityName, targetName));
    _setStatus(QStringLiteral("Attack Air: %1 persiguiendo y atacando %2.")
                   .arg(entityName, targetName));
    return true;
  }

  return false;
}

bool AttackTaskProcessor::processAttackSurfaceTask(const QString& entityName) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (e.name == entityName) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  if (domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  setEntityTaskStatus(entityName, QStringLiteral("Running"));

  if (_bombCtrl->isPending()) {
    if (_bombCtrl->pendingRelease().launcherEntityName.compare(
            entityName, Qt::CaseInsensitive) == 0) {
      return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
    }
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    return false;
  }

  const EntityTask task = launcher->currentTask;
  const QString targetName = task.targetEntityName.trimmed();
  QString targetLabel;
  QString targetEntityName;
  double targetLatitude = task.targetLatitude;
  double targetLongitude = task.targetLongitude;
  double targetAltitudeMeters = static_cast<double>(task.targetAltitudeMeters);

  if (!targetName.isEmpty()) {
    const Entity* target = nullptr;
    for (const Entity& e : _state->entities()) {
      if (e.name == targetName) { target = &e; break; }
    }
    if (!target || target->destroyed) {
      setEntityTaskStatus(entityName, QStringLiteral("Failed"));
      _log(QStringLiteral("Attack Surface task failed for %1: target no longer valid.")
               .arg(entityName));
      return true;
    }
    if (target->name == launcher->name ||
        target->forceIdentifier == launcher->forceIdentifier ||
        target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }

    targetLatitude = target->latitude;
    targetLongitude = target->longitude;
    targetAltitudeMeters = static_cast<double>(target->altitude);
    targetLabel = target->name;
    targetEntityName = target->name;
  } else {
    if (!domain::attackSurfaceCoordinatesAreUsable(targetLatitude, targetLongitude)) {
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }
    targetLabel = domain::attackPointLabel(targetLatitude, targetLongitude);
  }

  _queueBomb(
      entityName, targetLatitude, targetLongitude, targetAltitudeMeters,
      targetLabel, QStringLiteral("Attack Surface Task"),
      targetEntityName, false, false);
  setEntityTaskStatus(entityName, QStringLiteral("Completed"));
  _log(QStringLiteral("Attack Surface task armed bomb release for %1 at %2")
           .arg(entityName, targetLabel));
  _setStatus(QStringLiteral("Attack Surface armado para %1.").arg(entityName));
  return true;
}

bool AttackTaskProcessor::setEntityTaskStatus(
    const QString& entityName, const QString& status) {
  for (Entity& entity : _state->entitiesMutable()) {
    if (entity.name != entityName) {
      continue;
    }
    if (entity.currentTask.status == status) {
      return true;
    }
    entity.currentTask.status = status;
    _state->save();
    return true;
  }
  return false;
}

} // namespace application
