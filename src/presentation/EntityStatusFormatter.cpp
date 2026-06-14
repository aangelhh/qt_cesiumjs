#include "presentation/EntityStatusFormatter.h"
#include "application/ScenarioState.h"
#include "domain/BombReleaseGate.h"
#include "domain/CombatRules.h"
#include "domain/GeoMath.h"
#include "presentation/EntityPlanExecutor.h"

#include <QStringList>
#include <algorithm>

namespace presentation {

namespace {

const Entity* findEntity(const QVector<Entity>& entities, const QString& name) {
  for (const Entity& e : entities) {
    if (e.name == name) {
      return &e;
    }
  }
  return nullptr;
}

} // namespace

QString buildEntityOperationalStatus(
    const QVariantMap& summary,
    const Entity* entity,
    const EntityStatusContext& ctx) {

  const auto value = [&summary](const char* key, const QString& fallback = QStringLiteral("-")) {
    const QString text = summary.value(QString::fromLatin1(key)).toString().trimmed();
    return text.isEmpty() ? fallback : text;
  };
  const auto displayTaskType = [](const QString& taskType) {
    return taskType == QStringLiteral("InterceptEntity") ||
           taskType == QStringLiteral("InterceptEntity2D") ||
           taskType == QStringLiteral("InterceptEntity3D")
        ? QStringLiteral("Intercept Entity")
        : (taskType == QStringLiteral("FollowRoute") ||
           taskType == QStringLiteral("MoveAlongRoute")
           ? QStringLiteral("Follow Route")
           : taskType == QStringLiteral("HoldRacetrack")
             ? QStringLiteral("Hold Racetrack")
           : taskType == QStringLiteral("WaitOnLocation")
             ? QStringLiteral("Wait on Location")
           : taskType);
  };

  const QString taskType = value("taskType", QStringLiteral("No current tasks"));
  const QString taskStatus = value("taskStatus", QStringLiteral("-"));
  const auto routeStatus = [&summary](const QString& statusText) {
    const int current = summary.value(QStringLiteral("taskRouteCurrentWaypointIndex")).toInt(0);
    const int total = summary.value(QStringLiteral("taskRouteTotalWaypoints")).toInt(0);
    if (total <= 0) {
      return statusText;
    }
    return QStringLiteral("%1 WP %2/%3").arg(statusText).arg(current).arg(total);
  };
  const QString status = value("status");
  QString operationalState =
      taskType == QStringLiteral("-") || taskType == QStringLiteral("No current tasks")
          ? status
          : QStringLiteral("%1 (%2)").arg(
              displayTaskType(taskType),
              taskType == QStringLiteral("FollowRoute") || taskType == QStringLiteral("MoveAlongRoute")
                  ? routeStatus(taskStatus)
                  : taskStatus);

  if (!entity) {
    return operationalState;
  }

  const QString liveTaskType = entity->currentTask.taskType.trimmed();
  const QString liveTaskStatus = entity->currentTask.status.trimmed();
  if (entity->currentTask.enabled && !liveTaskType.isEmpty()) {
    const QString liveStatus = liveTaskStatus.isEmpty() ? QStringLiteral("-") : liveTaskStatus;
    const QString progressStatus =
        (liveTaskType == QStringLiteral("FollowRoute") || liveTaskType == QStringLiteral("MoveAlongRoute")) &&
            entity->currentTask.routeTotalWaypoints > 0
        ? QStringLiteral("%1 WP %2/%3")
              .arg(liveStatus)
              .arg(entity->currentTask.routeCurrentWaypointIndex)
              .arg(entity->currentTask.routeTotalWaypoints)
        : liveStatus;
    operationalState = QStringLiteral("%1 (%2)")
        .arg(displayTaskType(liveTaskType), progressStatus);
  }

  const QString entityName = entity->name;
  const int missileCount = domain::weaponQuantity(*entity, QStringLiteral("Missile"));
  const int bombCount = domain::weaponQuantity(*entity, QStringLiteral("Bomb"));
  const QString behaviorMode = entity->behaviorMode.trimmed().isEmpty()
      ? QStringLiteral("Manual")
      : entity->behaviorMode.trimmed();
  const QString behaviorTargetName = entity->behaviorTargetEntityName.trimmed();
  const Entity* behaviorTarget = findEntity(ctx.entities, behaviorTargetName);

  bool behaviorTargetDetected = false;
  double behaviorTargetRangeMeters = -1.0;
  for (const SensorContact& contact : entity->sensorContacts) {
    if (contact.targetEntityName.compare(behaviorTargetName, Qt::CaseInsensitive) != 0) {
      continue;
    }
    if (contact.detected) {
      behaviorTargetDetected = true;
      if (behaviorTargetRangeMeters < 0.0 ||
          contact.rangeMeters < behaviorTargetRangeMeters) {
        behaviorTargetRangeMeters = contact.rangeMeters;
      }
    }
  }

  const QString behaviorTargetStatus = behaviorTargetName.isEmpty()
      ? QStringLiteral("-")
      : QStringLiteral("%1 | %2 | %3%4")
            .arg(
                behaviorTargetDetected
                    ? QStringLiteral("detected")
                    : QStringLiteral("not detected"),
                behaviorTarget
                    ? (behaviorTarget->forceIdentifier == entity->forceIdentifier
                           ? QStringLiteral("friendly")
                           : QStringLiteral("enemy"))
                    : QStringLiteral("unknown side"),
                behaviorTarget
                    ? (behaviorTarget->destroyed
                           ? QStringLiteral("destroyed")
                           : QStringLiteral("alive"))
                    : QStringLiteral("missing"),
                behaviorTargetRangeMeters >= 0.0
                    ? QStringLiteral(" | %1 km")
                          .arg(behaviorTargetRangeMeters / 1000.0, 0, 'f', 1)
                    : QString());

  struct ContactDebugLine {
    double rangeMeters = 0.0;
    QString text;
  };
  QVector<ContactDebugLine> contactLines;
  for (const SensorContact& contact : entity->sensorContacts) {
    const QString targetName = contact.targetEntityName.trimmed();
    const Entity* target = findEntity(ctx.entities, targetName);
    const bool friendly = target && target->forceIdentifier == entity->forceIdentifier;
    const bool destroyed = target && target->destroyed;
    const bool validBehaviorTarget =
        contact.detected &&
        target &&
        !destroyed &&
        target->name != entityName &&
        !friendly;

    contactLines.push_back(ContactDebugLine{
        contact.rangeMeters,
        QStringLiteral("%1 | %2 | %3 km | brg %4 deg | %5 | %6 | %7")
            .arg(targetName.isEmpty() ? QStringLiteral("<unknown>") : targetName)
            .arg(contact.detected ? QStringLiteral("detected") : QStringLiteral("not detected"))
            .arg(contact.rangeMeters / 1000.0, 0, 'f', 1)
            .arg(contact.bearingDegrees, 0, 'f', 1)
            .arg(target
                     ? (friendly ? QStringLiteral("friendly") : QStringLiteral("enemy"))
                     : QStringLiteral("unknown side"))
            .arg(target
                     ? (destroyed ? QStringLiteral("destroyed") : QStringLiteral("alive"))
                     : QStringLiteral("unknown state"))
            .arg(validBehaviorTarget
                     ? QStringLiteral("behavior valid")
                     : QStringLiteral("behavior blocked")),
    });
  }
  std::sort(
      contactLines.begin(),
      contactLines.end(),
      [](const ContactDebugLine& left, const ContactDebugLine& right) {
        return left.rangeMeters < right.rangeMeters;
      });

  QStringList contactTextLines;
  for (int index = 0; index < contactLines.size() && index < 10; ++index) {
    contactTextLines.push_back(QStringLiteral("  %1").arg(contactLines.at(index).text));
  }
  const QString contactsText = contactTextLines.isEmpty()
      ? QStringLiteral("  No contacts detected")
      : contactTextLines.join(QStringLiteral("\n"));

  QString bombReleaseState = QStringLiteral("None");
  QString bombTargetText = QStringLiteral("-");
  QString bombDistanceText = QStringLiteral("-");
  if (ctx.pendingRelease.pending &&
      ctx.pendingRelease.launcherEntityName.compare(entityName, Qt::CaseInsensitive) == 0) {
    const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
        *entity,
        ctx.pendingRelease.targetLatitude,
        ctx.pendingRelease.targetLongitude,
        ctx.pendingRelease.targetAltitudeMeters);
    bombReleaseState = evaluation.stateLabel();
    bombTargetText = ctx.pendingRelease.targetLabel.trimmed().isEmpty()
        ? domain::attackPointLabel(
              ctx.pendingRelease.targetLatitude,
              ctx.pendingRelease.targetLongitude)
        : ctx.pendingRelease.targetLabel.trimmed();
    bombDistanceText = QStringLiteral("%1 km")
        .arg(
            domain::distanceMeters(
                entity->latitude,
                entity->longitude,
                ctx.pendingRelease.targetLatitude,
                ctx.pendingRelease.targetLongitude) / 1000.0,
            0,
            'f',
            1);
  } else {
    for (const ActiveMunition& munition : ctx.activeMunitions) {
      if (munition.launcherEntityName.compare(entityName, Qt::CaseInsensitive) == 0 &&
          munition.munitionType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0) {
        bombReleaseState = QStringLiteral("Released");
        bombTargetText = munition.id;
        break;
      }
    }
  }

  const bool canUseWeapons = domain::entityCanUseMissileActions(*entity);

  bool hasAirTarget = false;
  bool hasDetectedAirTarget = false;
  bool hasMissileTargetInRange = false;
  const double missileMaxRangeMeters = ScenarioState::missileMaxRangeMeters();
  for (const Entity& candidate : ctx.entities) {
    if (candidate.name == entityName ||
        candidate.destroyed ||
        candidate.forceIdentifier == entity->forceIdentifier ||
        candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
      continue;
    }
    hasAirTarget = true;
    for (const SensorContact& contact : entity->sensorContacts) {
      if (!contact.detected ||
          contact.targetEntityName.compare(candidate.name, Qt::CaseInsensitive) != 0) {
        continue;
      }
      hasDetectedAirTarget = true;
      if (contact.rangeMeters > 0.0 && contact.rangeMeters <= missileMaxRangeMeters) {
        hasMissileTargetInRange = true;
      }
    }
  }

  const auto availabilityText = [](bool available, const QString& reason) {
    return available
        ? QStringLiteral("\u2714 Available")
        : QStringLiteral("\u2716 Blocked (%1)").arg(reason);
  };
  const auto fieldLine = [](const QString& label, const QString& text) {
    return QStringLiteral("  %1 %2")
        .arg(label + QStringLiteral(":"), -24, QLatin1Char(' '))
        .arg(text);
  };

  QString launchMissileReason;
  if (!canUseWeapons || entity->destroyed) {
    launchMissileReason = QStringLiteral("platform not eligible");
  } else if (missileCount <= 0) {
    launchMissileReason = QStringLiteral("no missiles");
  } else if (!ctx.simulationRunning) {
    launchMissileReason = QStringLiteral("simulation stopped");
  } else if (!hasAirTarget) {
    launchMissileReason = QStringLiteral("no target");
  } else if (!hasDetectedAirTarget) {
    launchMissileReason = QStringLiteral("target not detected");
  } else if (!hasMissileTargetInRange) {
    launchMissileReason = QStringLiteral("out of range");
  }
  const bool launchMissileAvailable = launchMissileReason.isEmpty();

  QString releaseBombReason;
  if (!canUseWeapons || entity->destroyed) {
    releaseBombReason = QStringLiteral("platform not eligible");
  } else if (bombCount <= 0) {
    releaseBombReason = QStringLiteral("no bombs");
  } else if (!ctx.simulationRunning) {
    releaseBombReason = QStringLiteral("simulation stopped");
  }
  const bool releaseBombAvailable = releaseBombReason.isEmpty();

  const bool cancelBombAvailable =
      ctx.pendingRelease.pending &&
      ctx.pendingRelease.launcherEntityName.compare(entityName, Qt::CaseInsensitive) == 0;
  const auto activePlanIt = ctx.plans.constFind(entityName);
  const EntityPlan* activePlan = (activePlanIt != ctx.plans.constEnd())
      ? &activePlanIt.value()
      : nullptr;
  const QString planStatus = (!activePlan || activePlan->status.trimmed().isEmpty())
      ? QString(plan_status::NotStarted)
      : domain::planStatusDisplayLabel(activePlan->status.trimmed());
  QString currentPlanStep = QStringLiteral("-");
  if (activePlan &&
      activePlan->running &&
      activePlan->currentStepIndex >= 0 &&
      activePlan->currentStepIndex < activePlan->steps.size()) {
    const PlanStep& step = activePlan->steps.at(activePlan->currentStepIndex);
    const QString stepStatus = step.status.trimmed().isEmpty()
        ? QString(plan_status::NotStarted)
        : step.status.trimmed();
    currentPlanStep = QStringLiteral("%1 [%2]")
        .arg(EntityPlanExecutor::planStepDisplayLabel(step), stepStatus);
  }

  QStringList lines;
  lines << QStringLiteral("[ENTITY]")
        << fieldLine(QStringLiteral("State"), operationalState)
        << fieldLine(
               QStringLiteral("Damage"),
               QStringLiteral("%1 (%2%)")
                   .arg(
                       value("damageState", QStringLiteral("Intact")),
                       QString::number(
                           summary.value(QStringLiteral("damagePercent"), 0.0).toDouble(),
                           'f',
                           0)))
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[WEAPONS]")
        << fieldLine(QStringLiteral("Missiles"), QString::number(missileCount))
        << fieldLine(QStringLiteral("Bombs"), QString::number(bombCount))
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[BEHAVIOR]")
        << fieldLine(QStringLiteral("Mode"), behaviorMode)
        << fieldLine(
               QStringLiteral("Target"),
               behaviorTargetName.isEmpty() ? QStringLiteral("-") : behaviorTargetName)
        << fieldLine(QStringLiteral("Target status"), behaviorTargetStatus)
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[SENSORS]")
        << contactsText
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[BOMBING]")
        << fieldLine(QStringLiteral("Pending Release"), bombReleaseState)
        << fieldLine(QStringLiteral("Bomb Target"), bombTargetText)
        << fieldLine(QStringLiteral("Distance"), bombDistanceText)
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[PLAN]")
        << fieldLine(QStringLiteral("Status"), planStatus)
        << fieldLine(QStringLiteral("Current Step"), currentPlanStep)
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[ACTIONS]")
        << fieldLine(
               QStringLiteral("Launch Missile At"),
               availabilityText(launchMissileAvailable, launchMissileReason))
        << fieldLine(
               QStringLiteral("Release Bomb At"),
               availabilityText(releaseBombAvailable, releaseBombReason))
        << fieldLine(
               QStringLiteral("Cancel Bomb Release"),
               availabilityText(
                   cancelBombAvailable,
                   QStringLiteral("no pending release")));

  return lines.join(QStringLiteral("\n"));
}

} // namespace presentation
