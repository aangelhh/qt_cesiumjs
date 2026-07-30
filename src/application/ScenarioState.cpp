#include "application/ScenarioState.h"

#include "application/BehaviorEngine.h"
#include "application/Event.h"
#include "application/EventBus.h"
#include "application/FlightDynamicsEngine.h"
#include "application/MunitionSimulator.h"
#include "application/ScenarioSerializer.h"
#include "application/SensorEngine.h"
#include "application/TacticalGraphicRepository.h"
#include "application/TaskApplicator.h"

#include <QDir>
#include <QtMath>
#include <cmath>
#include <utility>

namespace {

constexpr double kDefaultMissileMaxRangeMeters = 60000.0;
constexpr double kLaunchFlashTtlSeconds = 0.25;

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

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

bool isMovementTaskType(const QString& taskType) {
  return taskType == QStringLiteral("MoveToLocation") ||
         taskType == QStringLiteral("WaitOnLocation") ||
         taskType == QStringLiteral("MoveToWaypoint") ||
         taskType == QStringLiteral("MoveAlongRoute") ||
         taskType == QStringLiteral("FollowRoute") ||
         taskType == QStringLiteral("PatrolArea") ||
         taskType == QStringLiteral("OrbitArea") ||
         taskType == QStringLiteral("HoldRacetrack") ||
         taskType == QStringLiteral("FollowEntity") ||
         taskType == QStringLiteral("InterceptEntity") ||
         taskType == QStringLiteral("InterceptEntity2D") ||
         taskType == QStringLiteral("InterceptEntity3D") ||
         taskType == QStringLiteral("FlyHeadingAltitudeSpeed") ||
         taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackUntilDestroyed");
}


bool entityCanCarryMissiles(const Entity& entity) {
  return !entity.destroyed &&
         entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0 &&
         entity.category.compare(QStringLiteral("Fighter"), Qt::CaseInsensitive) == 0;
}

bool entityCanCarryBombs(const Entity& entity) {
  return entityCanCarryMissiles(entity);
}

WeaponInventoryItem* findWeaponInventoryItem(
    QVector<WeaponInventoryItem>& weapons,
    const QString& weaponType) {
  for (WeaponInventoryItem& item : weapons) {
    if (item.weaponType.compare(weaponType, Qt::CaseInsensitive) == 0) {
      return &item;
    }
  }
  return nullptr;
}

bool entityIsGroundDomain(const Entity& entity) {
  return entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
}

void normalizeGroundEntity(Entity& entity) {
  if (!entityIsGroundDomain(entity)) {
    return;
  }

  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
  entity.flightDynamicsEnabled = false;
  entity.flightDynamicsMode = QStringLiteral("kinematic");
  entity.jsbsimAircraftModel.clear();
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 0.0;
  entity.currentTask = EntityTask{};
}

bool entityIsValidMissileTarget(
    const Entity& launcher,
    const Entity& target) {
  return !target.destroyed &&
         target.name != launcher.name &&
         target.forceIdentifier != launcher.forceIdentifier &&
         target.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0;
}

double detectedTargetRangeMeters(
    const Entity& launcher,
    const QString& targetName) {
  const QString trimmedTargetName = targetName.trimmed();
  if (trimmedTargetName.isEmpty()) {
    return -1.0;
  }

  double closestRangeMeters = -1.0;
  for (const SensorContact& contact : launcher.sensorContacts) {
    if (!contact.detected ||
        contact.targetEntityName.trimmed().compare(
            trimmedTargetName,
            Qt::CaseInsensitive) != 0) {
      continue;
    }

    if (closestRangeMeters < 0.0 || contact.rangeMeters < closestRangeMeters) {
      closestRangeMeters = contact.rangeMeters;
    }
  }

  return closestRangeMeters;
}

TransientEffect makeTransientEffect(
    const QString& id,
    const QString& effectType,
    int forceIdentifier,
    double latitude,
    double longitude,
    double altitudeMeters,
    double ttlSeconds) {
  TransientEffect effect;
  effect.id = id;
  effect.effectType = effectType;
  effect.forceIdentifier = forceIdentifier;
  effect.latitude = latitude;
  effect.longitude = longitude;
  effect.altitudeMeters = altitudeMeters;
  effect.ttlSeconds = ttlSeconds;
  return effect;
}

} // namespace

ScenarioState::ScenarioState() {
  this->reset();
}

void ScenarioState::addEntity(const Entity& entity) {
  ScopedLock lock(_mutex);
  Entity newEntity = entity; // Create a mutable copy
  normalizeGroundEntity(newEntity);
  newEntity.currentTask = EntityTask{}; // CRITICAL: Ensure new entity starts with clean task state
  _entities.push_back(newEntity);
  _taskStacks[newEntity.name] = domain::TaskStack(); // CRITICAL: Initialize empty stack for new entity
  this->refreshSensors();
  this->save();
}

const QVector<Entity>& ScenarioState::entities() const {
  return _entities;
}

const QVector<ActiveMunition>& ScenarioState::activeMunitions() const {
  return _activeMunitions;
}

const QVector<TransientEffect>& ScenarioState::transientEffects() const {
  return _transientEffects;
}

double ScenarioState::missileMaxRangeMeters() {
  return kDefaultMissileMaxRangeMeters;
}

bool ScenarioState::removeEntity(const QString& entityName) {
  ScopedLock lock(_mutex);
  for (qsizetype index = 0; index < _entities.size(); ++index) {
    if (_entities.at(index).name == entityName) {
      const QString removedEntityName = _entities.at(index).name;
      _entities.removeAt(index);
      _taskStacks.erase(removedEntityName); // Clean up stack for removed entity
      _behaviorMissileCooldownSeconds.erase(removedEntityName);

      for (Entity& entity : _entities) {
        if (entity.behaviorTargetEntityName.compare(
                removedEntityName,
                Qt::CaseInsensitive) != 0) {
          continue;
        }

        entity.behaviorTargetEntityName.clear();
        _pendingEventLogMessages.push_back(
            QStringLiteral("Behavior target cleared: target removed (%1 -> %2)")
                .arg(entity.name, removedEntityName));
      }

      this->refreshSensors();
      this->save();
      return true;
    }
  }
  return false;
}

void ScenarioState::addWaypoint(const Waypoint& waypoint) {
  ScopedLock lock(_mutex);
  application::upsertWaypoint(_waypoints, waypoint);
  this->save();
}

const QVector<Waypoint>& ScenarioState::waypoints() const {
  return _waypoints;
}

bool ScenarioState::removeWaypoint(const QString& waypointName) {
  ScopedLock lock(_mutex);
  const bool removed = application::removeWaypoint(_waypoints, waypointName);
  if (removed) this->save();
  return removed;
}

void ScenarioState::addRoute(const RouteGraphic& route) {
  ScopedLock lock(_mutex);
  application::upsertRoute(_routes, route);
  this->save();
}

const QVector<RouteGraphic>& ScenarioState::routes() const {
  return _routes;
}

bool ScenarioState::removeRoute(const QString& routeName) {
  ScopedLock lock(_mutex);
  const bool removed = application::removeRoute(_routes, routeName);
  if (removed) this->save();
  return removed;
}

void ScenarioState::addArea(const AreaDefinition& area) {
  ScopedLock lock(_mutex);
  application::upsertArea(_areas, area);
  this->save();
}

const QVector<AreaDefinition>& ScenarioState::areas() const {
  return _areas;
}

bool ScenarioState::removeArea(const QString& areaName) {
  ScopedLock lock(_mutex);
  const bool removed = application::removeArea(_areas, _entities, areaName);
  if (removed) this->save();
  return removed;
}

bool ScenarioState::assignTask(const QString& entityName, const EntityTask& task) {
  ScopedLock lock(_mutex);
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (entity.destroyed) {
      return false;
    }
    entity.currentTask = task;
    application::resolveTaskCoordinates(entity, _waypoints, _routes, _areas);
    if (!isMovementTaskType(entity.currentTask.taskType)) {
      if (domain::TaskStack* stack = this->getTaskStack(entityName)) {
        while (!stack->isEmpty()) {
          stack->pop();
        }
      }
    }
    this->save();
    return true;
  }
  return false;
}

bool ScenarioState::clearTask(const QString& entityName) {
  EntityTask clearedTask;
  clearedTask.status = QStringLiteral("Idle");
  return this->assignTask(entityName, clearedTask);
}

bool ScenarioState::setEntityDestroyed(const QString& entityName, bool destroyed) {
  ScopedLock lock(_mutex);
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }

    if (entity.destroyed == destroyed) {
      return true;
    }

    entity.destroyed = destroyed;
    entity.damagePercent = destroyed
        ? 100.0
        : qMin(entity.damagePercent, 99.0);
    entity.currentTask = EntityTask{};
    entity.currentTask.status = destroyed
        ? QStringLiteral("Destroyed")
        : QStringLiteral("Idle");
    if (destroyed) {
      entity.behaviorTargetEntityName.clear();
      _behaviorMissileCooldownSeconds.erase(entity.name);
    }
    entity.flightDynamicsEnabled = false;
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    entity.sensorContacts.clear();

    if (domain::TaskStack* stack = this->getTaskStack(entityName)) {
      while (!stack->isEmpty()) {
        stack->pop();
      }
    }

    this->refreshSensors();
    this->save();
    return true;
  }
  return false;
}

bool ScenarioState::setEntityBehaviorMode(
    const QString& entityName,
    const QString& behaviorMode) {
  ScopedLock lock(_mutex);
  const QString normalizedMode = normalizedBehaviorMode(behaviorMode);
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }

    if (entity.behaviorMode == normalizedMode) {
      return true;
    }

    entity.behaviorMode = normalizedMode;
    if (normalizedMode == QStringLiteral("Manual")) {
      entity.behaviorTargetEntityName.clear();
    }
    _pendingEventLogMessages.push_back(
        QStringLiteral("%1 behavior mode set to %2")
            .arg(entity.name, entity.behaviorMode));
    this->save();
    return true;
  }
  return false;
}

void ScenarioState::applyDamageWithSource(
    const QString& targetName,
    double damageAmount,
    const QString& sourceLabel) {
  const QString trimmedTargetName = targetName.trimmed();
  const double clampedDamageAmount = qMax(0.0, damageAmount);
  const QString trimmedSourceLabel = sourceLabel.trimmed().isEmpty()
      ? QStringLiteral("Unknown")
      : sourceLabel.trimmed();
  if (trimmedTargetName.isEmpty() || clampedDamageAmount <= 0.0) {
    return;
  }

  for (Entity& entity : _entities) {
    if (entity.name != trimmedTargetName || entity.destroyed) {
      continue;
    }

    entity.damagePercent = qBound(
        0.0,
        entity.damagePercent + clampedDamageAmount,
        100.0);

    if (entity.damagePercent >= 100.0) {
      entity.damagePercent = 100.0;
      _pendingEventLogMessages.push_back(
          QStringLiteral("%1 hit %2: Destroyed")
              .arg(trimmedSourceLabel, entity.name));
      this->setEntityDestroyed(entity.name, true);
      return;
    }

    _pendingEventLogMessages.push_back(
        QStringLiteral("%1 hit %2: %3 (%4%)")
            .arg(trimmedSourceLabel, entity.name)
            .arg(entity.damageStateLabel())
            .arg(qRound(entity.damagePercent)));
    this->save();
    return;
  }
}

void ScenarioState::applyMissileDamage(
    const QString& targetName,
    double damageAmount) {
  ScopedLock lock(_mutex);
  this->applyDamageWithSource(
      targetName,
      damageAmount,
      QStringLiteral("Missile"));
}

void ScenarioState::applyBombBlastDamage(const ActiveMunition& munition) {
  for (const application::BombBlastHit& hit :
       application::computeBombBlastHits(munition, _entities)) {
    this->applyDamageWithSource(hit.targetName, hit.damageAmount, QStringLiteral("Bomb"));
  }
}

bool ScenarioState::addMissileToEntity(const QString& entityName, int quantity) {
  ScopedLock lock(_mutex);
  if (quantity <= 0) {
    return false;
  }

  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryMissiles(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Missile"));
    if (!item) {
      entity.weapons.push_back(
          WeaponInventoryItem{QStringLiteral("Missile"), quantity});
    } else {
      item->quantity += quantity;
    }

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::addBombToEntity(const QString& entityName, int quantity) {
  ScopedLock lock(_mutex);
  if (quantity <= 0) {
    return false;
  }

  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryBombs(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Bomb"));
    if (!item) {
      entity.weapons.push_back(
          WeaponInventoryItem{QStringLiteral("Bomb"), quantity});
    } else {
      item->quantity += quantity;
    }

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::launchMissile(const QString& entityName) {
  ScopedLock lock(_mutex);
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryMissiles(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Missile"));
    if (!item || item->quantity <= 0) {
      return false;
    }

    --item->quantity;

    ActiveMunition munition = application::makeMissileMunition(entity, _nextMunitionSerial++);
    _activeMunitions.push_back(munition);
    _transientEffects.push_back(makeTransientEffect(
        munition.id + QStringLiteral("-launch"),
        QStringLiteral("LaunchFlash"),
        entity.forceIdentifier,
        munition.latitude,
        munition.longitude,
        munition.altitudeMeters,
        kLaunchFlashTtlSeconds));

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::releaseBomb(const QString& entityName) {
  ScopedLock lock(_mutex);
  for (Entity& entity : _entities) {
    if (entity.name != entityName) {
      continue;
    }
    if (!entityCanCarryBombs(entity)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(entity.weapons, QStringLiteral("Bomb"));
    if (!item || item->quantity <= 0) {
      return false;
    }

    --item->quantity;

    ActiveMunition munition = application::makeBombMunition(entity, _nextMunitionSerial++);
    _activeMunitions.push_back(munition);

    this->save();
    return true;
  }

  return false;
}

bool ScenarioState::launchMissileAt(
    const QString& launcherName,
    const QString& targetName) {
  ScopedLock lock(_mutex);
  const QString trimmedLauncherName = launcherName.trimmed();
  const QString trimmedTargetName = targetName.trimmed();
  if (trimmedLauncherName.isEmpty() || trimmedTargetName.isEmpty()) {
    return false;
  }

  const Entity* validatedTarget = nullptr;
  for (const Entity& entity : _entities) {
    if (entity.name != trimmedTargetName) {
      continue;
    }
    validatedTarget = &entity;
    break;
  }
  if (!validatedTarget) {
    return false;
  }

  for (Entity& launcher : _entities) {
    if (launcher.name != trimmedLauncherName) {
      continue;
    }
    if (!entityCanCarryMissiles(launcher) ||
        !entityIsValidMissileTarget(launcher, *validatedTarget)) {
      return false;
    }

    WeaponInventoryItem* item =
        findWeaponInventoryItem(launcher.weapons, QStringLiteral("Missile"));
    if (!item || item->quantity <= 0) {
      return false;
    }

    const double targetRangeMeters =
        detectedTargetRangeMeters(launcher, validatedTarget->name);
    if (targetRangeMeters < 0.0 ||
        targetRangeMeters > kDefaultMissileMaxRangeMeters) {
      return false;
    }

    --item->quantity;

    ActiveMunition munition = application::makeMissileMunition(launcher, _nextMunitionSerial++);
    munition.targetEntityName = validatedTarget->name;
    munition.guidanceActive = true;
    munition.status = QStringLiteral("Tracking");
    _activeMunitions.push_back(munition);
    _transientEffects.push_back(makeTransientEffect(
        munition.id + QStringLiteral("-launch"),
        QStringLiteral("LaunchFlash"),
        launcher.forceIdentifier,
        munition.latitude,
        munition.longitude,
        munition.altitudeMeters,
        kLaunchFlashTtlSeconds));

    this->save();
    return true;
  }

  return false;
}

QStringList ScenarioState::takePendingEventLogMessages() {
  ScopedLock lock(_mutex);
  const QStringList messages = _pendingEventLogMessages;
  _pendingEventLogMessages.clear();
  return messages;
}

domain::TaskStack* ScenarioState::getTaskStack(const QString& entityName) {
  return &_taskStacks[entityName];
}

void ScenarioState::refreshSensors() {
  SensorEngine::updateEntityContacts(_entities);
}

void ScenarioState::advanceActiveMunitions(double deltaSeconds) {
  application::advanceActiveMunitions(
      _activeMunitions,
      _transientEffects,
      _entities,
      [this](const QString& targetName, double damagePercent) {
        this->applyMissileDamage(targetName, damagePercent);
      },
      [this](const ActiveMunition& munition) {
        this->applyBombBlastDamage(munition);
      },
      deltaSeconds);
}

void ScenarioState::advanceTransientEffects(double deltaSeconds) {
  application::advanceTransientEffects(_transientEffects, deltaSeconds);
}

void ScenarioState::advanceBehaviors(double deltaSeconds) {
  const QStringList messages = application::advanceBehaviors(
      _entities,
      _behaviorMissileCooldownSeconds,
      _behaviorDamageReactionLevel,
      deltaSeconds,
      [this](const QString& launcher, const QString& target) {
        return this->launchMissileAt(launcher, target);
      });
  for (const QString& msg : messages) {
    _pendingEventLogMessages.push_back(msg);
  }
}

void ScenarioState::advanceSimulation(double deltaSeconds) {
  if (!std::isfinite(deltaSeconds) || deltaSeconds <= 0.0) {
    return;
  }

  QVector<application::KinematicsTelemetrySnapshot> telemetrySnapshots;
  {
    ScopedLock lock(_mutex);
    _simulationTimeSeconds += deltaSeconds;
    FlightDynamicsEngine::advanceEntities(_entities, _taskStacks, deltaSeconds);
    this->advanceBehaviors(deltaSeconds);
    this->advanceActiveMunitions(deltaSeconds);
    this->advanceTransientEffects(deltaSeconds);
    this->refreshSensors();
    telemetrySnapshots = _kinematicsTelemetryPublisher.advance(
        _entities,
        _simulationTimeSeconds,
        deltaSeconds);
  }

  for (auto& snapshot : telemetrySnapshots) {
    application::EventBus::instance().publish(
        application::EventKinematicsTelemetryUpdated(std::move(snapshot)));
  }
}

double ScenarioState::simulationTimeSeconds() const {
  ScopedLock lock(_mutex);
  return _simulationTimeSeconds;
}

void ScenarioState::stopMission() {
  ScopedLock lock(_mutex);
  for (Entity& entity : _entities) {
    entity.currentTask = EntityTask{};
    entity.currentTask.status = QStringLiteral("Stopped");
    entity.speedKnots = 0.0;
    entity.verticalSpeedMetersPerSecond = 0.0;
    entity.sensorContacts.clear();
    entity.behaviorTargetEntityName.clear();
  }
  _activeMunitions.clear();
  _transientEffects.clear();
  _pendingEventLogMessages.clear();
  _taskStacks.clear();
  _behaviorMissileCooldownSeconds.clear();
  _behaviorDamageReactionLevel.clear();
  _simulationTimeSeconds = 0.0;
  _kinematicsTelemetryPublisher.reset();
  this->refreshSensors();
  this->save();
}

bool ScenarioState::save() const {
  ScopedLock lock(_mutex);
  return application::saveScenario(this->storagePath(), {
      _entities, _waypoints, _routes, _areas});
}

bool ScenarioState::load() {
  ScopedLock lock(_mutex);
  _entities.clear();
  _activeMunitions.clear();
  _transientEffects.clear();
  _pendingEventLogMessages.clear();
  _waypoints.clear();
  _routes.clear();
  _areas.clear();
  _taskStacks.clear();
  _behaviorMissileCooldownSeconds.clear();
  _behaviorDamageReactionLevel.clear();
  _nextMunitionSerial = 1;
  _simulationTimeSeconds = 0.0;
  _kinematicsTelemetryPublisher.reset();

  const application::ScenarioSnapshot snapshot =
      application::loadScenario(this->storagePath());

  for (const Entity& entity : snapshot.entities) {
    _taskStacks[entity.name] = domain::TaskStack();
    _entities.push_back(entity);
  }
  _waypoints = snapshot.waypoints;
  _routes    = snapshot.routes;
  _areas     = snapshot.areas;

  this->refreshSensors();
  return true;
}

void ScenarioState::reset() {
  ScopedLock lock(_mutex);
  _entities.clear();
  _activeMunitions.clear();
  _transientEffects.clear();
  _pendingEventLogMessages.clear();
  _waypoints.clear();
  _routes.clear();
  _areas.clear();
  _taskStacks.clear(); // Clear all stacks before reset
  _behaviorMissileCooldownSeconds.clear();
  _behaviorDamageReactionLevel.clear();
  _nextMunitionSerial = 1;
  _simulationTimeSeconds = 0.0;
  _kinematicsTelemetryPublisher.reset();
  this->save();
}

QString ScenarioState::storagePath() const {
  return QDir(projectRoot()).absoluteFilePath(QStringLiteral("Data/scenario_state.json"));
}
