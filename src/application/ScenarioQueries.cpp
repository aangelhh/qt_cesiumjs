#include "application/ScenarioQueries.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
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

    const QString targetName = contact.targetEntityName.trimmed();
    const QString targetKey = targetName.toCaseFolded();
    if (targetName.isEmpty() || addedTargetNames.contains(targetKey)) {
      continue;
    }

    for (const Entity& candidate : scenarioState->entities()) {
      if (candidate.name.compare(targetName, Qt::CaseInsensitive) != 0 ||
          candidate.name == launcher.name ||
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

  for (const ActiveMunition& munition : scenarioState->activeMunitions()) {
    if (!munition.active ||
        munition.munitionType.compare(QStringLiteral("Missile"), Qt::CaseInsensitive) != 0) {
      continue;
    }
    if (munition.launcherEntityName.compare(launcherName, Qt::CaseInsensitive) == 0 &&
        munition.targetEntityName.compare(targetName, Qt::CaseInsensitive) == 0) {
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
    if (entity.name == entityName) {
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
    if (candidate.name == launcher.name ||
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
    if (!contact.detected || contact.targetEntityName.trimmed().isEmpty()) {
      continue;
    }

    const QString targetName = contact.targetEntityName.trimmed();
    for (const Entity& candidate : scenarioState->entities()) {
      if (candidate.name.compare(targetName, Qt::CaseInsensitive) != 0 ||
          candidate.name == launcher.name ||
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
