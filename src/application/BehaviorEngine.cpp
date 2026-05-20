#include "application/BehaviorEngine.h"

#include "domain/Entity.h"

namespace {

constexpr double kBehaviorAutoMissileCooldownSeconds = 12.0;

// ─── Behavior helpers ─────────────────────────────────────────────────────────

QStringList supportedBehaviorModes() {
  return {
      QStringLiteral("Manual"),
      QStringLiteral("Aggressive"),
      QStringLiteral("Defensive"),
      QStringLiteral("Patrol"),
  };
}

QString normalizedBehaviorMode(const QString& behaviorMode) {
  const QString trimmed = behaviorMode.trimmed();
  for (const QString& mode : supportedBehaviorModes()) {
    if (mode.compare(trimmed, Qt::CaseInsensitive) == 0) {
      return mode;
    }
  }
  return QStringLiteral("Manual");
}

int weaponQuantity(const Entity& entity, const QString& weaponType) {
  for (const WeaponInventoryItem& item : entity.weapons) {
    if (item.weaponType.compare(weaponType, Qt::CaseInsensitive) == 0) {
      return item.quantity;
    }
  }
  return 0;
}

const Entity* findEntityByName(
    const QVector<Entity>& entities,
    const QString& name) {
  for (const Entity& entity : entities) {
    if (entity.name == name) {
      return &entity;
    }
  }
  return nullptr;
}

bool entityIsValidBehaviorTarget(
    const Entity& observer,
    const Entity& target) {
  return !target.destroyed &&
         target.name != observer.name &&
         target.forceIdentifier != observer.forceIdentifier;
}

QString selectBestBehaviorTargetName(
    const Entity& observer,
    const QVector<Entity>& entities) {
  QString selectedTargetName;
  double selectedRangeMeters = -1.0;

  for (const SensorContact& contact : observer.sensorContacts) {
    if (!contact.detected || contact.targetEntityName.trimmed().isEmpty()) {
      continue;
    }

    const Entity* target =
        findEntityByName(entities, contact.targetEntityName.trimmed());
    if (!target || !entityIsValidBehaviorTarget(observer, *target)) {
      continue;
    }

    if (selectedRangeMeters < 0.0 || contact.rangeMeters < selectedRangeMeters) {
      selectedRangeMeters = contact.rangeMeters;
      selectedTargetName = target->name;
    }
  }

  return selectedTargetName;
}

int behaviorDamageReactionLevel(const Entity& entity) {
  if (entity.destroyed) {
    return 3;
  }
  if (entity.damagePercent >= 80.0) {
    return 2;
  }
  if (entity.damagePercent >= 50.0) {
    return 1;
  }
  return 0;
}

bool canAutoEngageBasedOnDamage(const Entity& entity) {
  return behaviorDamageReactionLevel(entity) == 0;
}

QString behaviorDamageReactionMessage(const Entity& entity, int level) {
  if (level == 2) {
    return QStringLiteral("%1 behavior auto-engagement blocked: critical damage (%2%).")
        .arg(entity.name)
        .arg(entity.damagePercent, 0, 'f', 0);
  }
  if (level == 1) {
    return QStringLiteral("%1 behavior auto-engagement reduced: damaged (%2%).")
        .arg(entity.name)
        .arg(entity.damagePercent, 0, 'f', 0);
  }
  return QStringLiteral("%1 behavior auto-engagement restored (%2%).")
      .arg(entity.name)
      .arg(entity.damagePercent, 0, 'f', 0);
}

bool entityPassesAutoMissileQuickValidation(
    const Entity& launcher,
    const Entity& target) {
  return !launcher.destroyed &&
         !target.destroyed &&
         !launcher.name.trimmed().isEmpty() &&
         !target.name.trimmed().isEmpty() &&
         launcher.name != target.name &&
         launcher.forceIdentifier != target.forceIdentifier &&
         weaponQuantity(launcher, QStringLiteral("Missile")) > 0;
}

} // namespace

namespace application {

QStringList advanceBehaviors(
    QVector<Entity>&                                    entities,
    BehaviorCooldownMap&                                cooldowns,
    BehaviorReactionMap&                                reactions,
    double                                              deltaSeconds,
    std::function<bool(const QString&, const QString&)> launchFn) {

  QStringList logMessages;

  if (deltaSeconds > 0.0) {
    for (auto it = cooldowns.begin(); it != cooldowns.end();) {
      it->second = qMax(0.0, it->second - deltaSeconds);
      if (it->second <= 0.0) {
        it = cooldowns.erase(it);
        continue;
      }
      ++it;
    }
  }

  for (Entity& entity : entities) {
    if (entity.destroyed) {
      entity.behaviorTargetEntityName.clear();
      cooldowns.erase(entity.name);
      reactions.erase(entity.name);
      continue;
    }

    const QString behaviorMode = normalizedBehaviorMode(entity.behaviorMode);
    if (behaviorMode == QStringLiteral("Manual")) {
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    if (entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    const QString selectedTargetName = selectBestBehaviorTargetName(entity, entities);
    if (entity.behaviorTargetEntityName != selectedTargetName) {
      const QString previousTargetName = entity.behaviorTargetEntityName;
      entity.behaviorTargetEntityName = selectedTargetName;
      if (!selectedTargetName.isEmpty()) {
        logMessages.push_back(
            QStringLiteral("%1 behavior target selected: %2")
                .arg(entity.name, selectedTargetName));
      } else if (!previousTargetName.isEmpty()) {
        logMessages.push_back(
            QStringLiteral("%1 behavior target cleared")
                .arg(entity.name));
      }
    }

    if (behaviorMode != QStringLiteral("Aggressive") ||
        entity.behaviorTargetEntityName.trimmed().isEmpty()) {
      reactions.erase(entity.name);
      continue;
    }

    const int damageReactionLevel = behaviorDamageReactionLevel(entity);
    const auto reactionIt = reactions.find(entity.name);
    const int previousReactionLevel =
        reactionIt == reactions.end() ? -1 : reactionIt->second;
    if (damageReactionLevel != previousReactionLevel) {
      logMessages.push_back(behaviorDamageReactionMessage(entity, damageReactionLevel));
      reactions[entity.name] = damageReactionLevel;
    }

    const QString taskType = entity.currentTask.taskType.trimmed();
    if (entity.currentTask.enabled &&
        (taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackSurface"))) {
      continue;
    }

    if (!canAutoEngageBasedOnDamage(entity)) {
      continue;
    }

    const auto cooldownIt = cooldowns.find(entity.name);
    if (cooldownIt != cooldowns.end() && cooldownIt->second > 0.0) {
      continue;
    }

    const Entity* target =
        findEntityByName(entities, entity.behaviorTargetEntityName.trimmed());
    if (!target || !entityPassesAutoMissileQuickValidation(entity, *target)) {
      continue;
    }

    const QString launcherName = entity.name;
    const QString targetName = target->name;
    if (!launchFn(launcherName, targetName)) {
      continue;
    }

    cooldowns[launcherName] = kBehaviorAutoMissileCooldownSeconds;
    logMessages.push_back(
        QStringLiteral("%1 auto-launched missile at %2")
            .arg(launcherName, targetName));
  }

  return logMessages;
}

} // namespace application
