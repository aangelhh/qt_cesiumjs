#include "application/BehaviorEngine.h"

#include "domain/Entity.h"
#include "domain/EntityIdentity.h"

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
    if (domain::entityMatchesReference(entity, name)) {
      return &entity;
    }
  }
  return nullptr;
}

bool entityIsValidBehaviorTarget(
    const Entity& observer,
    const Entity& target) {
  return !target.destroyed &&
         domain::entityKey(target) != domain::entityKey(observer) &&
         target.forceIdentifier != observer.forceIdentifier;
}

const Entity* selectBestBehaviorTarget(
    const Entity& observer,
    const QVector<Entity>& entities) {
  const Entity* selectedTarget = nullptr;
  double selectedRangeMeters = -1.0;

  for (const SensorContact& contact : observer.sensorContacts) {
    const QString targetReference = contact.targetEntityId.trimmed().isEmpty()
        ? contact.targetEntityName.trimmed()
        : contact.targetEntityId.trimmed();
    if (!contact.detected || targetReference.isEmpty()) {
      continue;
    }

    const Entity* target =
        findEntityByName(entities, targetReference);
    if (!target || !entityIsValidBehaviorTarget(observer, *target)) {
      continue;
    }

    if (selectedRangeMeters < 0.0 || contact.rangeMeters < selectedRangeMeters) {
      selectedRangeMeters = contact.rangeMeters;
      selectedTarget = target;
    }
  }

  return selectedTarget;
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
         domain::entityKey(launcher) != domain::entityKey(target) &&
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
    const QString entityKey = domain::entityKey(entity);
    const auto cooldownKey = [&]() {
      return cooldowns.find(entityKey) != cooldowns.end() ||
             cooldowns.find(entity.name) == cooldowns.end()
          ? entityKey
          : entity.name;
    }();
    const auto reactionKey = [&]() {
      return reactions.find(entityKey) != reactions.end() ||
             reactions.find(entity.name) == reactions.end()
          ? entityKey
          : entity.name;
    }();
    if (entity.destroyed) {
      entity.behaviorTargetEntityId.clear();
      entity.behaviorTargetEntityName.clear();
      cooldowns.erase(entityKey);
      cooldowns.erase(entity.name);
      reactions.erase(entityKey);
      reactions.erase(entity.name);
      continue;
    }

    const QString behaviorMode = normalizedBehaviorMode(entity.behaviorMode);
    if (behaviorMode == QStringLiteral("Manual")) {
      entity.behaviorTargetEntityId.clear();
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    if (entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
      entity.behaviorTargetEntityId.clear();
      entity.behaviorTargetEntityName.clear();
      continue;
    }

    const Entity* selectedTarget = selectBestBehaviorTarget(entity, entities);
    const QString selectedTargetId = selectedTarget
        ? domain::entityKey(*selectedTarget)
        : QString{};
    const QString selectedTargetName = selectedTarget
        ? selectedTarget->name
        : QString{};
    if (entity.behaviorTargetEntityId != selectedTargetId) {
      const QString previousTargetName = entity.behaviorTargetEntityName;
      entity.behaviorTargetEntityId = selectedTargetId;
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
        entity.behaviorTargetEntityId.trimmed().isEmpty()) {
      reactions.erase(reactionKey);
      continue;
    }

    const int damageReactionLevel = behaviorDamageReactionLevel(entity);
    const auto reactionIt = reactions.find(reactionKey);
    const int previousReactionLevel =
        reactionIt == reactions.end() ? -1 : reactionIt->second;
    if (damageReactionLevel != previousReactionLevel) {
      logMessages.push_back(behaviorDamageReactionMessage(entity, damageReactionLevel));
      reactions[reactionKey] = damageReactionLevel;
    }

    const QString taskType = entity.currentTask.taskType.trimmed();
    if (entity.currentTask.enabled &&
        (taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackUntilDestroyed") ||
         taskType == QStringLiteral("FireOnPosition") ||
         taskType == QStringLiteral("FireInDirection") ||
         taskType == QStringLiteral("StopWeaponsTask") ||
         taskType == QStringLiteral("AttackSurface"))) {
      continue;
    }

    if (!canAutoEngageBasedOnDamage(entity)) {
      continue;
    }

    const auto cooldownIt = cooldowns.find(cooldownKey);
    if (cooldownIt != cooldowns.end() && cooldownIt->second > 0.0) {
      continue;
    }

    const Entity* target =
        findEntityByName(entities, entity.behaviorTargetEntityId.trimmed());
    if (!target || !entityPassesAutoMissileQuickValidation(entity, *target)) {
      continue;
    }

    bool contactUsesStableId = false;
    for (const SensorContact& contact : entity.sensorContacts) {
      if (contact.detected &&
          !contact.targetEntityId.trimmed().isEmpty() &&
          domain::entityMatchesReference(*target, contact.targetEntityId)) {
        contactUsesStableId = true;
        break;
      }
    }
    const QString launcherName = contactUsesStableId ? entityKey : entity.name;
    const QString targetName = contactUsesStableId
        ? domain::entityKey(*target)
        : target->name;
    if (!launchFn(launcherName, targetName)) {
      continue;
    }

    cooldowns[contactUsesStableId ? entityKey : entity.name] =
        kBehaviorAutoMissileCooldownSeconds;
    logMessages.push_back(
        QStringLiteral("%1 auto-launched missile at %2")
            .arg(launcherName, targetName));
  }

  return logMessages;
}

} // namespace application
