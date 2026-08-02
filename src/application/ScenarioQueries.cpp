#include "application/ScenarioQueries.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "domain/EntityIdentity.h"
#include "domain/Munition.h"
#include "domain/TacticalGraphic.h"
#include <QSet>
#include <QString>
#include <QStringList>

namespace application {

QVector<MissileTargetCandidate> detectedMissileTargetsInRange(
    const ScenarioState* scenarioState,
    const Entity& launcher) {
  QVector<MissileTargetCandidate> targets;
  if (!scenarioState) {
    return targets;
  }

  const double maxRangeMeters = ScenarioState::missileMaxRangeMeters();
  QSet<QString> addedTargetNames;
  for (const SensorContact& contact : launcher.sensorContacts) {
    if (!contact.detected || contact.rangeMeters <= 0.0 ||
        contact.rangeMeters > maxRangeMeters) {
      continue;
    }

    const QString targetReference = contact.targetEntityId.trimmed().isEmpty()
        ? contact.targetEntityName.trimmed()
        : contact.targetEntityId.trimmed();
    const QString targetKey = targetReference.toCaseFolded();
    if (targetReference.isEmpty() || addedTargetNames.contains(targetKey)) {
      continue;
    }

    for (const Entity& candidate : scenarioState->entities()) {
      if (!domain::entityMatchesReference(candidate, targetReference) ||
          domain::entityKey(candidate) == domain::entityKey(launcher) ||
          candidate.destroyed ||
          candidate.forceIdentifier == launcher.forceIdentifier ||
          candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
        continue;
      }
      targets.push_back({&candidate, contact.rangeMeters});
      addedTargetNames.insert(targetKey);
      break;
    }
  }

  return targets;
}

bool activeMissileInFlightForTarget(
    const ScenarioState* scenarioState,
    const QString& launcherName,
    const QString& targetName) {
  if (!scenarioState) {
    return false;
  }

  const Entity* launcher = nullptr;
  const Entity* target = nullptr;
  for (const Entity& entity : scenarioState->entities()) {
    if (!launcher && domain::entityMatchesReference(entity, launcherName)) {
      launcher = &entity;
    }
    if (!target && domain::entityMatchesReference(entity, targetName)) {
      target = &entity;
    }
  }
  if (!launcher || !target) {
    return false;
  }

  for (const ActiveMunition& munition : scenarioState->activeMunitions()) {
    if (!munition.active ||
        munition.munitionType.compare(QStringLiteral("Missile"), Qt::CaseInsensitive) != 0) {
      continue;
    }
    const QString launcherReference = munition.launcherEntityId.trimmed().isEmpty()
        ? munition.launcherEntityName
        : munition.launcherEntityId;
    const QString targetReference = munition.targetEntityId.trimmed().isEmpty()
        ? munition.targetEntityName
        : munition.targetEntityId;
    if (domain::entityMatchesReference(*launcher, launcherReference) &&
        domain::entityMatchesReference(*target, targetReference)) {
      return true;
    }
  }
  return false;
}

int entityAltitudeMeters(
    const ScenarioState* scenarioState,
    const QString& entityName) {
  if (!scenarioState || entityName.trimmed().isEmpty()) {
    return 0;
  }

  for (const Entity& entity : scenarioState->entities()) {
    if (domain::entityMatchesReference(entity, entityName)) {
      return entity.altitude;
    }
  }
  return 0;
}

QVector<const Entity*> validBombReleaseTargets(
    const ScenarioState* scenarioState,
    const Entity& launcher) {
  QVector<const Entity*> targets;
  if (!scenarioState) {
    return targets;
  }

  for (const Entity& candidate : scenarioState->entities()) {
    if (domain::entityKey(candidate) == domain::entityKey(launcher) ||
        candidate.destroyed ||
        candidate.forceIdentifier == launcher.forceIdentifier ||
        candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
      continue;
    }
    targets.push_back(&candidate);
  }
  return targets;
}

const Entity* bestDetectedSurfaceBombTarget(
    const ScenarioState* scenarioState,
    const Entity& launcher) {
  if (!scenarioState) {
    return nullptr;
  }

  const Entity* selectedTarget = nullptr;
  double selectedRangeMeters = -1.0;
  for (const SensorContact& contact : launcher.sensorContacts) {
    const QString targetReference = contact.targetEntityId.trimmed().isEmpty()
        ? contact.targetEntityName.trimmed()
        : contact.targetEntityId.trimmed();
    if (!contact.detected || targetReference.isEmpty()) {
      continue;
    }

    for (const Entity& candidate : scenarioState->entities()) {
      if (!domain::entityMatchesReference(candidate, targetReference) ||
          domain::entityKey(candidate) == domain::entityKey(launcher) ||
          candidate.destroyed ||
          candidate.forceIdentifier == launcher.forceIdentifier ||
          candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
        continue;
      }

      if (selectedRangeMeters < 0.0 || contact.rangeMeters < selectedRangeMeters) {
        selectedTarget = &candidate;
        selectedRangeMeters = contact.rangeMeters;
      }
      break;
    }
  }

  return selectedTarget;
}

QStringList availableWaypointNames(const ScenarioState* scenarioState) {
  if (!scenarioState) {
    return {};
  }
  QStringList names;
  QSet<QString> seen;
  for (const Waypoint& waypoint : scenarioState->waypoints()) {
    const QString name = waypoint.name.trimmed();
    if (name.isEmpty() || seen.contains(name)) {
      continue;
    }
    seen.insert(name);
    names.append(name);
  }
  return names;
}

QStringList availableRouteNames(const ScenarioState* scenarioState, bool requirePoints) {
  if (!scenarioState) {
    return {};
  }
  QStringList names;
  QSet<QString> seen;
  for (const RouteGraphic& route : scenarioState->routes()) {
    const QString name = route.name.trimmed();
    if (name.isEmpty() || seen.contains(name)) {
      continue;
    }
    if (requirePoints && route.points.isEmpty()) {
      continue;
    }
    seen.insert(name);
    names.append(name);
  }
  return names;
}

QStringList availableAreaNames(const ScenarioState* scenarioState) {
  if (!scenarioState) {
    return {};
  }
  QStringList names;
  QSet<QString> seen;
  for (const AreaDefinition& area : scenarioState->areas()) {
    QString name = area.name.trimmed();
    if (name.isEmpty()) {
      name = area.id.trimmed();
    }
    if (name.isEmpty() || seen.contains(name)) {
      continue;
    }
    seen.insert(name);
    names.append(name);
  }
  return names;
}

} // namespace application
