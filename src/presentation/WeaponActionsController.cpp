#include "presentation/WeaponActionsController.h"
#include "application/ScenarioQueries.h"
#include "application/ScenarioState.h"
#include "domain/BombReleaseGate.h"
#include "presentation/TrackSummaryBuilder.h"

#include <QHash>
#include <QtMath>

namespace presentation {

WeaponActionsController::WeaponActionsController(
    ScenarioState*  state,
    SelectedNameFn  selectedName,
    FindEntityFn    findEntity,
    IsRunningFn     isRunning,
    LogFn           log,
    StatusFn        setStatus,
    SyncUiFn        syncUi,
    PickItemFn      pickTarget,
    QObject*        parent)
    : QObject(parent)
    , _state(state)
    , _selectedName(std::move(selectedName))
    , _findEntity(std::move(findEntity))
    , _isRunning(std::move(isRunning))
    , _log(std::move(log))
    , _setStatus(std::move(setStatus))
    , _syncUi(std::move(syncUi))
    , _pickTarget(std::move(pickTarget)) {}

void WeaponActionsController::addMissileToSelected() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  if (!_state->addMissileToEntity(entityName, 1)) {
    _setStatus(QStringLiteral("No se pudo anadir un misil a %1.").arg(entityName));
    return;
  }
  _log(QStringLiteral("Missile added to %1").arg(entityName));
  _syncUi();
  _setStatus(QStringLiteral("Misil anadido a %1.").arg(entityName));
}

void WeaponActionsController::addBombToSelected() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  if (!_state->addBombToEntity(entityName, 1)) {
    _setStatus(QStringLiteral("No se pudo anadir una bomba a %1.").arg(entityName));
    return;
  }
  _log(QStringLiteral("Bomb added to %1").arg(entityName));
  _syncUi();
  _setStatus(QStringLiteral("Bomba anadida a %1.").arg(entityName));
}

void WeaponActionsController::launchMissileFromSelected() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  const Entity* entity = _findEntity(entityName);
  const int missileCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Missile")) : 0;
  if (missileCount <= 0) {
    _setStatus(QStringLiteral("No hay misiles disponibles en %1.").arg(entityName));
    return;
  }
  if (!_isRunning()) {
    _setStatus(QStringLiteral("Arranca la simulacion para lanzar el misil."));
    return;
  }
  if (!_state->launchMissile(entityName)) {
    _setStatus(QStringLiteral("No se pudo lanzar un misil desde %1.").arg(entityName));
    return;
  }
  _log(QStringLiteral("Missile launched from %1").arg(entityName));
  _syncUi();
  _setStatus(QStringLiteral("Misil lanzado desde %1. Quedan %2.")
      .arg(entityName)
      .arg(qMax(0, missileCount - 1)));
}

void WeaponActionsController::releaseBombFromSelected() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  const Entity* entity = _findEntity(entityName);
  const int bombCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Bomb")) : 0;
  if (bombCount <= 0) {
    _setStatus(QStringLiteral("No hay bombas disponibles en %1.").arg(entityName));
    return;
  }
  if (!_isRunning()) {
    _setStatus(QStringLiteral("Arranca la simulacion para soltar la bomba."));
    return;
  }
  if (!_state->releaseBomb(entityName)) {
    _setStatus(QStringLiteral("No se pudo soltar una bomba desde %1.").arg(entityName));
    return;
  }
  _log(QStringLiteral("Bomb released from %1").arg(entityName));
  _syncUi();
  _setStatus(QStringLiteral("Bomba soltada desde %1. Quedan %2.")
      .arg(entityName)
      .arg(qMax(0, bombCount - 1)));
}

void WeaponActionsController::launchMissileAtSelected() {
  const QString launcherName = _selectedName();
  if (launcherName.isEmpty()) {
    return;
  }
  const Entity* launcher = _findEntity(launcherName);
  const int missileCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Missile")) : 0;
  if (!launcher || missileCount <= 0) {
    _setStatus(QStringLiteral("No hay misiles disponibles en %1.").arg(launcherName));
    return;
  }
  if (!_isRunning()) {
    _setStatus(QStringLiteral("Arranca la simulacion para lanzar el misil."));
    return;
  }

  const QVector<application::MissileTargetCandidate> candidates =
      application::detectedMissileTargetsInRange(_state, *launcher);
  if (candidates.isEmpty()) {
    _setStatus(QStringLiteral("No detected air targets in missile range for %1.")
        .arg(launcherName));
    return;
  }

  QStringList options;
  QHash<QString, QString> nameByOption;
  for (const application::MissileTargetCandidate& c : candidates) {
    if (!c.entity) {
      continue;
    }
    const QString label = presentation::missileTargetDisplayLabel(*c.entity, c.rangeMeters);
    options.push_back(label);
    nameByOption.insert(label, c.entity->name);
  }

  const QString selected = _pickTarget(options);
  if (selected.trimmed().isEmpty()) {
    return;
  }
  const QString targetName = nameByOption.value(selected).trimmed();
  if (targetName.isEmpty()) {
    return;
  }

  this->executeLaunchAt(launcherName, targetName, missileCount);
}

void WeaponActionsController::executeLaunchAt(
    const QString& launcherName,
    const QString& targetName,
    int previousMissileCount) {
  if (!_state->launchMissileAt(launcherName, targetName)) {
    _setStatus(QStringLiteral("Target out of missile range for %1.").arg(launcherName));
    return;
  }
  _log(QStringLiteral("Missile launched from %1 at %2").arg(launcherName, targetName));
  _syncUi();
  _setStatus(QStringLiteral("Misil lanzado desde %1 hacia %2. Quedan %3.")
      .arg(launcherName, targetName)
      .arg(qMax(0, previousMissileCount - 1)));
}

} // namespace presentation
