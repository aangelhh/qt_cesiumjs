#include "presentation/BombReleaseController.h"
#include "application/ScenarioState.h"
#include "domain/BombReleaseGate.h"
#include "domain/Entity.h"

#include <QTimer>

namespace presentation {

BombReleaseController::BombReleaseController(
    ScenarioState*  state,
    LogFn           log,
    StatusFn        setStatus,
    SyncUiFn        syncUi,
    SelectObjectFn  selectObject,
    RemoveTrackFn   removeTrack,
    QObject*        parent)
    : QObject(parent)
    , _state(state)
    , _log(std::move(log))
    , _setStatus(std::move(setStatus))
    , _syncUi(std::move(syncUi))
    , _selectObject(std::move(selectObject))
    , _removeTrack(std::move(removeTrack)) {}

void BombReleaseController::queue(
    const QString& launcherEntityName,
    double         targetLat,
    double         targetLon,
    double         targetAltMeters,
    const QString& targetLabel,
    const QString& sourceDesc,
    const QString& targetEntityName,
    bool           logQueued,
    bool           focusLauncher) {
  _pendingBombRelease.launcherEntityName  = launcherEntityName.trimmed();
  _pendingBombRelease.targetEntityName    = targetEntityName.trimmed();
  _pendingBombRelease.targetLatitude      = targetLat;
  _pendingBombRelease.targetLongitude     = targetLon;
  _pendingBombRelease.targetAltitudeMeters = targetAltMeters;
  _pendingBombRelease.targetLabel         = targetLabel.trimmed();
  _pendingBombRelease.sourceDescription   = sourceDesc.trimmed();
  _pendingBombRelease.pending             = true;
  _pendingBombRelease.releaseCommandIssued = false;

  if (logQueued) {
    _log(QStringLiteral("Bomb release queued for %1 on %2 (%3)")
             .arg(_pendingBombRelease.launcherEntityName,
                  _pendingBombRelease.targetLabel,
                  _pendingBombRelease.sourceDescription));
  }
  _setStatus(QStringLiteral("Bomb release armed for %1 on %2.")
                 .arg(_pendingBombRelease.launcherEntityName,
                      _pendingBombRelease.targetLabel));
  if (focusLauncher) {
    _selectObject(_pendingBombRelease.launcherEntityName, true);
  }
}

void BombReleaseController::clear() {
  _pendingBombRelease = PendingBombRelease{};
}

void BombReleaseController::validate() {
  if (!_pendingBombRelease.pending) {
    return;
  }

  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (e.name == _pendingBombRelease.launcherEntityName) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    const QString launcherName = _pendingBombRelease.launcherEntityName;
    clear();
    if (!launcherName.trimmed().isEmpty()) {
      _setStatus(
          QStringLiteral("Bomb release cleared for %1.").arg(launcherName));
    }
  }
}

void BombReleaseController::process() {
  if (!_pendingBombRelease.pending) {
    return;
  }

  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (e.name == _pendingBombRelease.launcherEntityName) { launcher = &e; break; }
  }
  if (!launcher || launcher->destroyed) {
    validate();
    return;
  }

  const int bombCount = domain::weaponQuantity(*launcher, QStringLiteral("Bomb"));
  if (bombCount <= 0) {
    const QString launcherName = _pendingBombRelease.launcherEntityName;
    clear();
    _setStatus(
        QStringLiteral("Bomb release cleared for %1. No bombs available.").arg(launcherName));
    return;
  }

  const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
      *launcher,
      _pendingBombRelease.targetLatitude,
      _pendingBombRelease.targetLongitude,
      _pendingBombRelease.targetAltitudeMeters);
  if (!evaluation.readyToRelease()) {
    return;
  }
  if (_pendingBombRelease.releaseCommandIssued) {
    return;
  }
  _pendingBombRelease.releaseCommandIssued = true;

  const QString launcherName  = _pendingBombRelease.launcherEntityName;
  const QString targetLabel   = _pendingBombRelease.targetLabel;
  const QString sourceDesc    = _pendingBombRelease.sourceDescription;
  _setStatus(QStringLiteral("Bomb release window reached for %1 on %2.")
                 .arg(launcherName, targetLabel));

  QTimer::singleShot(0, this, [this, launcherName, targetLabel, sourceDesc]() {
    if (!_pendingBombRelease.pending ||
        _pendingBombRelease.launcherEntityName != launcherName) {
      return;
    }
    if (!_state->releaseBomb(launcherName)) {
      clear();
      _setStatus(
          QStringLiteral("No se pudo soltar una bomba desde %1.").arg(launcherName));
      _syncUi();
      return;
    }
    _log(QStringLiteral("Bomb released from %1 at %2 (%3)")
             .arg(launcherName, targetLabel, sourceDesc));
    clear();
    _setStatus(
        QStringLiteral("Bomba soltada desde %1 sobre %2.").arg(launcherName, targetLabel));
    _syncUi();
  });
}

void BombReleaseController::beginPickMode(const QString& launcherName) {
  _isPickingMode       = true;
  _pickingLauncherName = launcherName;
}

void BombReleaseController::cancelPickMode() {
  _isPickingMode = false;
  _pickingLauncherName.clear();
}

bool BombReleaseController::isPickingMode() const {
  return _isPickingMode;
}

QString BombReleaseController::pickingLauncherName() const {
  return _pickingLauncherName;
}

QString BombReleaseController::handleRemovedEntity(const QString& entityName) {
  const QString removedEntityName = entityName.trimmed();

  if (_isPickingMode &&
      _pickingLauncherName.compare(removedEntityName, Qt::CaseInsensitive) == 0) {
    cancelPickMode();
  }

  if (!_pendingBombRelease.pending) {
    return QString();
  }

  const bool launcherRemoved =
      _pendingBombRelease.launcherEntityName.compare(
          removedEntityName, Qt::CaseInsensitive) == 0;
  const bool targetRemoved =
      !_pendingBombRelease.targetEntityName.trimmed().isEmpty() &&
      _pendingBombRelease.targetEntityName.compare(
          removedEntityName, Qt::CaseInsensitive) == 0;

  if (!launcherRemoved && !targetRemoved) {
    return QString();
  }

  const QString reason = targetRemoved
      ? QStringLiteral("target removed")
      : QStringLiteral("launcher removed");
  clear();
  _removeTrack(QStringLiteral("Bomb Target"));
  _removeTrack(QStringLiteral("Bomb Target Line"));
  const QString message =
      QStringLiteral("Pending bomb release cancelled: %1").arg(reason);
  _log(message);
  return message;
}

bool BombReleaseController::isPending() const {
  return _pendingBombRelease.pending;
}

const PendingBombRelease& BombReleaseController::pendingRelease() const {
  return _pendingBombRelease;
}

} // namespace presentation
