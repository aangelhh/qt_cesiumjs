#include "application/AttackTaskProcessor.h"
#include "application/ScenarioQueries.h"
#include "application/ScenarioState.h"
#include "domain/BombReleaseGate.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/EntityIdentity.h"
#include "domain/GeoMath.h"
#include "presentation/BombReleaseController.h"

namespace application {

namespace {

bool isAttackPursuitTask(const QString& taskType) {
  return taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackUntilDestroyed");
}

void clearAttackPursuitTargets(Entity& entity) {
  entity.currentTask.targetLatitude = entity.latitude;
  entity.currentTask.targetLongitude = entity.longitude;
  entity.currentTask.targetAltitudeMeters = entity.altitude;
  entity.currentTask.targetHeadingDegrees = entity.headingDegrees;
  entity.currentTask.targetSpeedKnots = 0.0;
}

} // namespace

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
    const QString entityKey = domain::entityKey(entity);
    if (entity.destroyed) {
      _attackAirElapsedSeconds.remove(entityKey);
      _attackAirMissileCooldownSeconds.remove(entityKey);
    }
    if ((taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackUntilDestroyed") ||
         taskType == QStringLiteral("FireInDirection") ||
         taskType == QStringLiteral("AttackOnce")) &&
        domain::attackTaskStatusIsTerminal(taskStatus)) {
      _attackAirElapsedSeconds.remove(entityKey);
      _attackAirMissileCooldownSeconds.remove(entityKey);
    }
    if (!entity.currentTask.enabled ||
        entity.destroyed ||
        (taskType != QStringLiteral("AttackOnce") &&
         taskType != QStringLiteral("AttackUntilDestroyed") &&
         taskType != QStringLiteral("FireOnPosition") &&
         taskType != QStringLiteral("FireInDirection") &&
         taskType != QStringLiteral("StopWeaponsTask") &&
         taskType != QStringLiteral("AttackAir") &&
         taskType != QStringLiteral("AttackSurface")) ||
        domain::attackTaskStatusIsTerminal(taskStatus)) {
      continue;
    }
    taskEntityNames.push_back(entityKey);
  }

  for (const QString& entityName : taskEntityNames) {
    const Entity* entity = nullptr;
    for (const Entity& e : _state->entities()) {
      if (domain::entityMatchesReference(e, entityName)) { entity = &e; break; }
    }
    if (!entity || entity->destroyed) {
      setEntityTaskStatus(entityName, QStringLiteral("Target unavailable"));
      continue;
    }

    const QString taskType = entity->currentTask.taskType.trimmed();
    if (taskType == QStringLiteral("AttackOnce")) {
      processAttackOnceTask(entityName, deltaSeconds);
    } else if (taskType == QStringLiteral("AttackUntilDestroyed")) {
      processAttackUntilDestroyedTask(entityName, deltaSeconds);
    } else if (taskType == QStringLiteral("FireOnPosition")) {
      processFireOnPositionTask(entityName);
    } else if (taskType == QStringLiteral("FireInDirection")) {
      processFireInDirectionTask(entityName, deltaSeconds);
    } else if (taskType == QStringLiteral("StopWeaponsTask")) {
      processStopWeaponsTask(entityName);
    } else if (taskType == QStringLiteral("AttackAir")) {
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
    const QString launcherKey = domain::entityKey(launcher);
    const QString taskType = launcher.currentTask.taskType.trimmed();
    if (launcher.currentTask.enabled &&
        (taskType == QStringLiteral("AttackOnce") ||
         taskType == QStringLiteral("AttackUntilDestroyed") ||
         taskType == QStringLiteral("FireOnPosition") ||
         taskType == QStringLiteral("FireInDirection") ||
         taskType == QStringLiteral("StopWeaponsTask") ||
         taskType == QStringLiteral("AttackAir") ||
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
      _autoBehaviorDamageReactionLevel.remove(launcherKey);
      continue;
    }

    const int damageReactionLevel = domain::autoBehaviorDamageReactionLevel(launcher);
    const int previousReactionLevel =
        _autoBehaviorDamageReactionLevel.value(launcherKey, -1);
    if (damageReactionLevel != previousReactionLevel) {
      _autoBehaviorDamageReactionLevel.insert(launcherKey, damageReactionLevel);
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

    const auto cooldownIt = _autoBombReleaseCooldownSeconds.constFind(launcherKey);
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
        launcherKey,
        target->latitude,
        target->longitude,
        static_cast<double>(target->altitude),
        target->name,
        QStringLiteral("Auto Behavior"),
        domain::entityKey(*target),
        false,
        false);
    _autoBombReleaseCooldownSeconds.insert(
        launcherKey, kAutoBombReleaseCooldownSeconds);
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

bool AttackTaskProcessor::processAttackOnceTask(
    const QString& entityName, double deltaSeconds) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  _attackAirElapsedSeconds[entityName] =
      _attackAirElapsedSeconds.value(entityName, 0.0) + qMax(0.0, deltaSeconds);
  const double timeoutSeconds = launcher->currentTask.timeoutSeconds > 0.0
      ? launcher->currentTask.timeoutSeconds
      : kAttackAirTimeoutSeconds;
  if (_attackAirElapsedSeconds.value(entityName) >= timeoutSeconds) {
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Once task failed for %1: timeout.")
             .arg(entityName));
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString targetName = domain::targetEntityReference(launcher->currentTask);
  if (targetName.isEmpty()) {
    _attackAirElapsedSeconds.remove(entityName);
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const Entity* target = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, targetName)) { target = &e; break; }
  }
  if (!target || target->destroyed ||
      domain::entityKey(*target) == domain::entityKey(*launcher) ||
      target->forceIdentifier == launcher->forceIdentifier) {
    _attackAirElapsedSeconds.remove(entityName);
    _log(QStringLiteral("Attack Once task failed for %1: target no longer valid.")
             .arg(entityName));
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  QString weaponType = launcher->currentTask.weaponType.trimmed();
  if (weaponType.isEmpty() ||
      weaponType.compare(QStringLiteral("Auto"), Qt::CaseInsensitive) == 0) {
    weaponType = target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
        ? QStringLiteral("Missile")
        : QStringLiteral("Bomb");
  }

  if (weaponType.compare(QStringLiteral("Missile"), Qt::CaseInsensitive) == 0) {
    if (target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0 ||
        domain::weaponQuantity(*launcher, QStringLiteral("Missile")) <= 0) {
      _attackAirElapsedSeconds.remove(entityName);
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }
    if (!_state->launchMissileAt(entityName, targetName)) {
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Once task launched missile from %1 at %2")
             .arg(entityName, targetName));
    _setStatus(QStringLiteral("Attack Once completado para %1.").arg(entityName));
    return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
  }

  if (weaponType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0) {
    if (target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0 ||
        domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0 ||
        _bombCtrl->isPending()) {
      _attackAirElapsedSeconds.remove(entityName);
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }

    _queueBomb(
        entityName,
        target->latitude,
        target->longitude,
        static_cast<double>(target->altitude),
        target->name,
        QStringLiteral("Attack Once Task"),
        domain::entityKey(*target),
        false,
        false);
    _attackAirElapsedSeconds.remove(entityName);
    _log(QStringLiteral("Attack Once task armed bomb release for %1 at %2")
             .arg(entityName, target->name));
    _setStatus(QStringLiteral("Attack Once armado para %1.").arg(entityName));
    return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
  }

  _attackAirElapsedSeconds.remove(entityName);
  return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
}

bool AttackTaskProcessor::processAttackUntilDestroyedTask(
    const QString& entityName, double deltaSeconds) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString targetName = domain::targetEntityReference(launcher->currentTask);
  if (targetName.isEmpty()) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const Entity* target = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, targetName)) { target = &e; break; }
  }
  if (target && target->destroyed) {
    setEntityTaskStatus(entityName, QStringLiteral("Completed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Until Destroyed task completed for %1: target %2 destroyed.")
             .arg(entityName, targetName));
    _setStatus(QStringLiteral("Attack Until Destroyed completado para %1: target destruido.")
                   .arg(entityName));
    return true;
  }
  if (!target || domain::entityKey(*target) == domain::entityKey(*launcher) ||
      target->forceIdentifier == launcher->forceIdentifier) {
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Until Destroyed task failed for %1: target no longer valid.")
             .arg(entityName));
    return true;
  }

  _attackAirElapsedSeconds[entityName] =
      _attackAirElapsedSeconds.value(entityName, 0.0) + qMax(0.0, deltaSeconds);
  const double maxEngagementTimeSeconds =
      launcher->currentTask.maxEngagementTimeSeconds > 0.0
          ? launcher->currentTask.maxEngagementTimeSeconds
          : kAttackAirTimeoutSeconds;
  if (_attackAirElapsedSeconds.value(entityName) >= maxEngagementTimeSeconds) {
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Attack Until Destroyed task failed for %1: timeout against %2.")
             .arg(entityName, targetName));
    return true;
  }

  QString weaponType = launcher->currentTask.weaponType.trimmed();
  if (weaponType.isEmpty() ||
      weaponType.compare(QStringLiteral("Auto"), Qt::CaseInsensitive) == 0) {
    weaponType = target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
        ? QStringLiteral("Missile")
        : QStringLiteral("Bomb");
  }
  const double shotCooldownSeconds = launcher->currentTask.shotCooldownSeconds > 0.0
      ? launcher->currentTask.shotCooldownSeconds
      : kAttackAirMissileCooldownSeconds;

  if (weaponType.compare(QStringLiteral("Missile"), Qt::CaseInsensitive) == 0) {
    if (target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }

    Entity* mutableLauncher = nullptr;
    for (Entity& e : _state->entitiesMutable()) {
      if (domain::entityMatchesReference(e, entityName)) { mutableLauncher = &e; break; }
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
        _log(QStringLiteral("Attack Until Destroyed task failed for %1: no missiles remaining.")
                 .arg(entityName));
        return true;
      }
      return false;
    }
    if (activeMissile ||
        _attackAirMissileCooldownSeconds.value(entityName, 0.0) > 0.0) {
      return false;
    }
    if (_state->launchMissileAt(entityName, targetName)) {
      _attackAirMissileCooldownSeconds[entityName] = shotCooldownSeconds;
      _log(QStringLiteral("Attack Until Destroyed task launched missile from %1 at %2")
               .arg(entityName, targetName));
      _setStatus(QStringLiteral("Attack Until Destroyed: %1 atacando %2.")
                     .arg(entityName, targetName));
      return true;
    }
    return false;
  }

  if (weaponType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0) {
    if (target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }
    setEntityTaskStatus(entityName, QStringLiteral("Running"));
    if (_bombCtrl->isPending()) {
      return false;
    }
    if (domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0) {
      setEntityTaskStatus(entityName, QStringLiteral("Failed"));
      _attackAirElapsedSeconds.remove(entityName);
      _attackAirMissileCooldownSeconds.remove(entityName);
      _log(QStringLiteral("Attack Until Destroyed task failed for %1: no bombs remaining.")
               .arg(entityName));
      return true;
    }
    if (_attackAirMissileCooldownSeconds.value(entityName, 0.0) > 0.0) {
      return false;
    }

    _queueBomb(
        entityName,
        target->latitude,
        target->longitude,
        static_cast<double>(target->altitude),
        target->name,
        QStringLiteral("Attack Until Destroyed Task"),
        domain::entityKey(*target),
        false,
        false);
    _attackAirMissileCooldownSeconds[entityName] = shotCooldownSeconds;
    _log(QStringLiteral("Attack Until Destroyed task armed bomb release for %1 at %2")
             .arg(entityName, target->name));
    _setStatus(QStringLiteral("Attack Until Destroyed: %1 atacando %2.")
                   .arg(entityName, target->name));
    return true;
  }

  return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
}

bool AttackTaskProcessor::processFireOnPositionTask(const QString& entityName) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const EntityTask task = launcher->currentTask;
  QString weaponType = task.weaponType.trimmed();
  if (weaponType.isEmpty() ||
      weaponType.compare(QStringLiteral("Auto"), Qt::CaseInsensitive) == 0) {
    weaponType = QStringLiteral("Bomb");
  }
  if (weaponType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) != 0) {
    _log(QStringLiteral("Fire on Position task failed for %1: unsupported weapon %2.")
             .arg(entityName, weaponType));
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }
  if (!domain::attackSurfaceCoordinatesAreUsable(
          task.targetLatitude, task.targetLongitude) ||
      domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0 ||
      _bombCtrl->isPending()) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString targetLabel =
      domain::attackPointLabel(task.targetLatitude, task.targetLongitude);
  _queueBomb(
      entityName,
      task.targetLatitude,
      task.targetLongitude,
      static_cast<double>(task.targetAltitudeMeters),
      targetLabel,
      QStringLiteral("Fire on Position Task"),
      QString(),
      false,
      false);
  _attackAirElapsedSeconds.remove(entityName);
  _attackAirMissileCooldownSeconds.remove(entityName);
  _log(QStringLiteral("Fire on Position task armed bomb release for %1 at %2")
           .arg(entityName, targetLabel));
  _setStatus(QStringLiteral("Fire on Position armado para %1.").arg(entityName));
  return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
}

bool AttackTaskProcessor::processFireInDirectionTask(
    const QString& entityName, double deltaSeconds) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString weaponType = launcher->currentTask.weaponType.trimmed();
  if (!weaponType.isEmpty() &&
      weaponType.compare(QStringLiteral("Auto"), Qt::CaseInsensitive) != 0) {
    _log(QStringLiteral("Fire in Direction task failed for %1: no compatible %2 weapon.")
             .arg(entityName, weaponType));
    _attackAirElapsedSeconds.remove(entityName);
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  _attackAirElapsedSeconds[entityName] =
      _attackAirElapsedSeconds.value(entityName, 0.0) + qMax(0.0, deltaSeconds);
  const double durationSeconds = launcher->currentTask.durationSeconds;
  if (durationSeconds <= 0.0 ||
      _attackAirElapsedSeconds.value(entityName) >= durationSeconds) {
    _attackAirElapsedSeconds.remove(entityName);
    _attackAirMissileCooldownSeconds.remove(entityName);
    _log(QStringLiteral("Fire in Direction task completed for %1: placeholder executed.")
             .arg(entityName));
    _setStatus(QStringLiteral("Fire in Direction completado para %1.").arg(entityName));
    return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
  }

  Entity* mutableLauncher = nullptr;
  for (Entity& e : _state->entitiesMutable()) {
    if (domain::entityMatchesReference(e, entityName)) { mutableLauncher = &e; break; }
  }
  if (mutableLauncher) {
    mutableLauncher->currentTask.status = QStringLiteral("Running");
    mutableLauncher->currentTask.targetHeadingDegrees =
        launcher->currentTask.targetHeadingDegrees;
  }
  return true;
}

bool AttackTaskProcessor::processStopWeaponsTask(const QString& entityName) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const bool hadPendingRelease =
      _bombCtrl->isPending() &&
      _bombCtrl->pendingRelease().launcherReference().compare(
          entityName, Qt::CaseInsensitive) == 0;
  if (hadPendingRelease) {
    _bombCtrl->cancelActiveAndArmNext();
  }
  _attackAirElapsedSeconds.remove(entityName);
  _attackAirMissileCooldownSeconds.remove(entityName);
  _autoBombReleaseCooldownSeconds.remove(entityName);
  _autoBehaviorDamageReactionLevel.remove(entityName);
  _log(hadPendingRelease
           ? QStringLiteral("Stop Weapons Task cancelled pending release for %1.")
                 .arg(entityName)
           : QStringLiteral("Stop Weapons Task completed for %1.").arg(entityName));
  _setStatus(QStringLiteral("Stop Weapons Task completado para %1.").arg(entityName));
  return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
}

bool AttackTaskProcessor::processAttackAirTask(
    const QString& entityName, double deltaSeconds) {
  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString targetName = domain::targetEntityReference(launcher->currentTask);
  if (targetName.isEmpty()) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const Entity* target = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, targetName)) { target = &e; break; }
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
  if (!target || domain::entityKey(*target) == domain::entityKey(*launcher)) {
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
    if (domain::entityMatchesReference(e, entityName)) { mutableLauncher = &e; break; }
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
    if (domain::entityMatchesReference(e, entityName)) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  if (domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0) {
    return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  setEntityTaskStatus(entityName, QStringLiteral("Running"));

  if (_bombCtrl->isPending()) {
    if (_bombCtrl->pendingRelease().launcherReference().compare(
            entityName, Qt::CaseInsensitive) == 0) {
      return setEntityTaskStatus(entityName, QStringLiteral("Completed"));
    }
    setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    return false;
  }

  const EntityTask task = launcher->currentTask;
  const QString targetName = domain::targetEntityReference(task);
  QString targetLabel;
  QString targetEntityName;
  double targetLatitude = task.targetLatitude;
  double targetLongitude = task.targetLongitude;
  double targetAltitudeMeters = static_cast<double>(task.targetAltitudeMeters);

  if (!targetName.isEmpty()) {
    const Entity* target = nullptr;
    for (const Entity& e : _state->entities()) {
      if (domain::entityMatchesReference(e, targetName)) { target = &e; break; }
    }
    if (!target || target->destroyed) {
      setEntityTaskStatus(entityName, QStringLiteral("Failed"));
      _log(QStringLiteral("Attack Surface task failed for %1: target no longer valid.")
               .arg(entityName));
      return true;
    }
    if (domain::entityKey(*target) == domain::entityKey(*launcher) ||
        target->forceIdentifier == launcher->forceIdentifier ||
        target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
      return setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }

    targetLatitude = target->latitude;
    targetLongitude = target->longitude;
    targetAltitudeMeters = static_cast<double>(target->altitude);
    targetLabel = target->name;
    targetEntityName = domain::entityKey(*target);
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
    if (!domain::entityMatchesReference(entity, entityName)) {
      continue;
    }
    const bool terminalAttackPursuit =
        isAttackPursuitTask(entity.currentTask.taskType.trimmed()) &&
        domain::attackTaskStatusIsTerminal(status);
    if (entity.currentTask.status == status && !terminalAttackPursuit) {
      return true;
    }
    entity.currentTask.status = status;
    if (terminalAttackPursuit) {
      clearAttackPursuitTargets(entity);
    }
    _state->save();
    return true;
  }
  return false;
}

} // namespace application
