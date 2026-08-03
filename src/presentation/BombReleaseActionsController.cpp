#include "presentation/BombReleaseActionsController.h"
#include "presentation/BombReleaseController.h"
#include "domain/BombReleaseGate.h"
#include "domain/EntityIdentity.h"
#include <QHash>

namespace presentation {

BombReleaseActionsController::BombReleaseActionsController(
    BombReleaseController* bombController,
    SelectedNameFn         selectedName,
    FindEntityFn           findEntity,
    IsRunningFn            isRunning,
    IsPickPendingFn        isPickPending,
    BeginCoordPickFn       beginCoordPick,
    ValidTargetsFn         validTargets,
    TargetLabelFn          targetLabel,
    PickItemFn             pickItem,
    StatusFn               setStatus,
    LogFn                  log,
    SyncUiFn               syncUi,
    QObject*               parent)
    : QObject(parent)
    , _bombController(bombController)
    , _selectedName(std::move(selectedName))
    , _findEntity(std::move(findEntity))
    , _isRunning(std::move(isRunning))
    , _isPickPending(std::move(isPickPending))
    , _beginCoordPick(std::move(beginCoordPick))
    , _validTargets(std::move(validTargets))
    , _targetLabel(std::move(targetLabel))
    , _pickItem(std::move(pickItem))
    , _setStatus(std::move(setStatus))
    , _log(std::move(log))
    , _syncUi(std::move(syncUi)) {}

void BombReleaseActionsController::releaseBombAtSurfaceEntity() {
  _bombController->clear();

  const QString launcherName = _selectedName();
  if (launcherName.isEmpty()) {
    return;
  }

  const Entity* launcher = _findEntity(launcherName);
  const int bombCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) : 0;
  if (!launcher || bombCount <= 0) {
    _setStatus(QStringLiteral("No hay bombas disponibles en %1.").arg(launcherName));
    return;
  }

  if (!_isRunning()) {
    _setStatus(QStringLiteral("Arranca la simulacion para programar el release de la bomba."));
    return;
  }

  const QVector<const Entity*> targets = _validTargets(*launcher);
  if (targets.isEmpty()) {
    _setStatus(QStringLiteral("No hay surface targets validos para %1.").arg(launcherName));
    return;
  }

  QStringList options;
  QHash<QString, const Entity*> targetByOption;
  for (const Entity* target : targets) {
    if (!target) {
      continue;
    }
    QString option = _targetLabel(*target);
    if (targetByOption.contains(option)) {
      option = QStringLiteral("%1 [%2]")
          .arg(option, domain::entityKey(*target).left(8));
    }
    options.push_back(option);
    targetByOption.insert(option, target);
  }

  bool ok = false;
  const QString selectedOption = _pickItem(
      QStringLiteral("Release Bomb At"),
      QStringLiteral("Surface Target"),
      options, ok);
  if (!ok || selectedOption.trimmed().isEmpty()) {
    _bombController->clear();
    return;
  }

  const Entity* target = targetByOption.value(selectedOption, nullptr);
  if (!target) {
    _bombController->clear();
    return;
  }

  queueBombReleaseAtEntity(launcherName, *target);
}

bool BombReleaseActionsController::validateLauncherCanQueueBombTarget(
    const QString& launcherName) {
  if (launcherName.isEmpty()) {
    return false;
  }

  const Entity* launcher = _findEntity(launcherName);
  const int bombCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) : 0;
  if (!launcher || bombCount <= 0) {
    _setStatus(QStringLiteral("No hay bombas disponibles en %1.").arg(launcherName));
    return false;
  }

  if (!_isRunning()) {
    _setStatus(QStringLiteral("Arranca la simulacion para programar targets de bomba."));
    return false;
  }

  return true;
}

void BombReleaseActionsController::addBombTargetToQueue() {
  const QString launcherName = _selectedName();
  if (!validateLauncherCanQueueBombTarget(launcherName)) {
    return;
  }

  const Entity* launcher = _findEntity(launcherName);
  if (!launcher) {
    return;
  }
  const QVector<const Entity*> targets = _validTargets(*launcher);
  if (targets.isEmpty()) {
    _setStatus(QStringLiteral("No hay surface targets validos para %1.").arg(launcherName));
    return;
  }

  QStringList options;
  QHash<QString, const Entity*> targetByOption;
  for (const Entity* target : targets) {
    if (!target) {
      continue;
    }
    QString option = _targetLabel(*target);
    if (targetByOption.contains(option)) {
      option = QStringLiteral("%1 [%2]")
          .arg(option, domain::entityKey(*target).left(8));
    }
    options.push_back(option);
    targetByOption.insert(option, target);
  }

  bool ok = false;
  const QString selectedOption = _pickItem(
      QStringLiteral("Add Bomb Target to Queue"),
      QStringLiteral("Surface Target"),
      options, ok);
  if (!ok || selectedOption.trimmed().isEmpty()) {
    return;
  }

  const Entity* target = targetByOption.value(selectedOption, nullptr);
  if (!target) {
    return;
  }

  addBombTargetToQueueAtEntity(launcherName, *target);
}

void BombReleaseActionsController::addCustomBombTargetToQueue() {
  const QString launcherName = _selectedName();
  if (!validateLauncherCanQueueBombTarget(launcherName)) {
    return;
  }

  if (_isPickPending()) {
    _setStatus(QStringLiteral("Termina antes la captura de coordenadas que ya esta activa."));
    return;
  }

  _bombController->beginQueuePickMode(launcherName);
  _beginCoordPick();
  _setStatus(QStringLiteral("Haz clic en el mapa para encolar un target de bomba para %1.")
      .arg(launcherName));
}

void BombReleaseActionsController::clearBombTargetQueue() {
  _bombController->clearTargetQueue();
  _syncUi();
}

void BombReleaseActionsController::releaseBombAtCustomCoordinates() {
  _bombController->clear();
  _bombController->cancelPickMode();

  const QString launcherName = _selectedName();
  if (launcherName.isEmpty()) {
    return;
  }

  const Entity* launcher = _findEntity(launcherName);
  const int bombCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) : 0;
  if (!launcher || bombCount <= 0) {
    _setStatus(QStringLiteral("No hay bombas disponibles en %1.").arg(launcherName));
    return;
  }

  if (!_isRunning()) {
    _setStatus(QStringLiteral("Arranca la simulacion para programar el release de la bomba."));
    return;
  }

  if (_isPickPending()) {
    _setStatus(QStringLiteral("Termina antes la captura de coordenadas que ya esta activa."));
    return;
  }

  _bombController->beginPickMode(launcherName);
  _beginCoordPick();
  _setStatus(QStringLiteral("Haz clic en el mapa para fijar el punto de ataque de %1.")
      .arg(launcherName));
}

void BombReleaseActionsController::cancelPendingBombRelease() {
  if (!_bombController->pendingRelease().pending) {
    _setStatus(QStringLiteral("No hay release de bomba pendiente."));
    return;
  }

  const QString launcherName = _bombController->pendingRelease().launcherEntityName;
  const QString targetLabel  = _bombController->pendingRelease().targetLabel;
  _bombController->cancelPickMode();
  _bombController->cancelActiveAndArmNext();
  _log(QStringLiteral("Bomb release canceled for %1 on %2")
      .arg(launcherName, targetLabel));
  _syncUi();
  _setStatus(QStringLiteral("Release de bomba cancelado para %1.").arg(launcherName));
}

void BombReleaseActionsController::queueBombReleaseAtEntity(
    const QString& launcherName, const Entity& target) {
  _bombController->queue(
      launcherName,
      target.latitude,
      target.longitude,
      static_cast<double>(target.altitude),
      target.name,
      QStringLiteral("Surface Entity"),
      domain::entityKey(target));
}

void BombReleaseActionsController::addBombTargetToQueueAtEntity(
    const QString& launcherName, const Entity& target) {
  _bombController->addTargetToQueue(
      launcherName,
      target.latitude,
      target.longitude,
      static_cast<double>(target.altitude),
      target.name,
      QStringLiteral("Surface Entity"),
      domain::entityKey(target));
}

} // namespace presentation
