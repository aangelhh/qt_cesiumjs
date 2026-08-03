#include "presentation/BombReleaseController.h"
#include "application/ScenarioState.h"
#include "domain/BombReleaseGate.h"
#include "domain/Entity.h"
#include "domain/EntityIdentity.h"

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

BombTargetQueueItem BombReleaseController::makeQueueItem(
    const QString& launcherEntityName,
    double         targetLat,
    double         targetLon,
    double         targetAltMeters,
    const QString& targetLabel,
    const QString& sourceDesc,
    const QString& targetEntityName) const {
  BombTargetQueueItem item;
  const QString launcherReference = launcherEntityName.trimmed();
  for (const Entity& entity : _state->entities()) {
    if (domain::entityMatchesReference(entity, launcherReference)) {
      item.launcherEntityId = domain::entityKey(entity);
      item.launcherEntityName = entity.name;
      break;
    }
  }
  if (item.launcherEntityId.isEmpty()) {
    item.launcherEntityName = launcherReference;
  }
  const QString targetReference = targetEntityName.trimmed();
  for (const Entity& entity : _state->entities()) {
    if (domain::entityMatchesReference(entity, targetReference)) {
      item.targetEntityId = domain::entityKey(entity);
      item.targetEntityName = entity.name;
      break;
    }
  }
  if (item.targetEntityId.isEmpty()) {
    item.targetEntityName = targetReference;
  }
  item.targetLatitude = targetLat;
  item.targetLongitude = targetLon;
  item.targetAltitudeMeters = targetAltMeters;
  item.targetLabel = targetLabel.trimmed();
  item.sourceDescription = sourceDesc.trimmed();
  return item;
}

void BombReleaseController::armTarget(
    const BombTargetQueueItem& item,
    bool logQueued,
    bool focusLauncher) {
  _pendingBombRelease.launcherEntityId    = item.launcherEntityId;
  _pendingBombRelease.launcherEntityName  = item.launcherEntityName;
  _pendingBombRelease.targetEntityId      = item.targetEntityId;
  _pendingBombRelease.targetEntityName    = item.targetEntityName;
  _pendingBombRelease.targetLatitude      = item.targetLatitude;
  _pendingBombRelease.targetLongitude     = item.targetLongitude;
  _pendingBombRelease.targetAltitudeMeters = item.targetAltitudeMeters;
  _pendingBombRelease.targetLabel         = item.targetLabel;
  _pendingBombRelease.sourceDescription   = item.sourceDescription;
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
    _selectObject(_pendingBombRelease.launcherReference(), true);
  }
}

bool BombReleaseController::queuedTargetIsUsable(const BombTargetQueueItem& item) const {
  if (item.launcherReference().isEmpty()) {
    return false;
  }
  const Entity* launcher = nullptr;
  for (const Entity& entity : _state->entities()) {
    if (domain::entityMatchesReference(entity, item.launcherReference())) {
      launcher = &entity;
      break;
    }
  }
  if (!launcher || launcher->destroyed ||
      domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0) {
    return false;
  }
  if (item.targetReference().isEmpty()) {
    return true;
  }
  for (const Entity& entity : _state->entities()) {
    if (!domain::entityMatchesReference(entity, item.targetReference())) {
      continue;
    }
    return !entity.destroyed;
  }
  return false;
}

bool BombReleaseController::armNextQueuedTarget() {
  while (!_targetQueue.isEmpty()) {
    const BombTargetQueueItem item = _targetQueue.takeFirst();
    if (!queuedTargetIsUsable(item)) {
      _log(QStringLiteral("Skipped queued bomb target %1: target unavailable or no bombs.")
               .arg(item.targetLabel.trimmed().isEmpty()
                    ? item.targetEntityName
                    : item.targetLabel));
      continue;
    }
    armTarget(item, true, false);
    return true;
  }
  return false;
}

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
  armTarget(
      makeQueueItem(
          launcherEntityName, targetLat, targetLon, targetAltMeters,
          targetLabel, sourceDesc, targetEntityName),
      logQueued,
      focusLauncher);
}

void BombReleaseController::addTargetToQueue(
    const QString& launcherEntityName,
    double         targetLat,
    double         targetLon,
    double         targetAltMeters,
    const QString& targetLabel,
    const QString& sourceDesc,
    const QString& targetEntityName,
    bool           logQueued,
    bool           focusLauncher) {
  const BombTargetQueueItem item = makeQueueItem(
      launcherEntityName, targetLat, targetLon, targetAltMeters,
      targetLabel, sourceDesc, targetEntityName);
  if (!_pendingBombRelease.pending) {
    armTarget(item, logQueued, focusLauncher);
    return;
  }
  _targetQueue.push_back(item);
  if (logQueued) {
    _log(QStringLiteral("Bomb target queued for %1 on %2 (%3)")
             .arg(item.launcherEntityName, item.targetLabel, item.sourceDescription));
  }
  _setStatus(QStringLiteral("Bomb target queued for %1. Queue: %2.")
                 .arg(item.launcherEntityName)
                 .arg(_targetQueue.size()));
}

void BombReleaseController::clear() {
  _pendingBombRelease = PendingBombRelease{};
}

void BombReleaseController::clearAll() {
  clear();
  _targetQueue.clear();
}

void BombReleaseController::cancelActiveAndArmNext() {
  clear();
  armNextQueuedTarget();
}

void BombReleaseController::clearTargetQueue() {
  _targetQueue.clear();
  _setStatus(QStringLiteral("Bomb target queue cleared."));
}

int BombReleaseController::queuedTargetCount() const {
  return _targetQueue.size();
}

QString BombReleaseController::nextQueuedTargetLabel() const {
  return _targetQueue.isEmpty() ? QString() : _targetQueue.first().targetLabel;
}

void BombReleaseController::validate() {
  if (!_pendingBombRelease.pending) {
    return;
  }

  const Entity* launcher = nullptr;
  for (const Entity& e : _state->entities()) {
    if (domain::entityMatchesReference(e, _pendingBombRelease.launcherReference())) {
      launcher = &e;
      break;
    }
  }
  if (!launcher || launcher->destroyed) {
    const QString launcherName = _pendingBombRelease.launcherEntityName;
    clear();
    armNextQueuedTarget();
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
    if (domain::entityMatchesReference(e, _pendingBombRelease.launcherReference())) {
      launcher = &e;
      break;
    }
  }
  if (!launcher || launcher->destroyed) {
    validate();
    return;
  }

  const int bombCount = domain::weaponQuantity(*launcher, QStringLiteral("Bomb"));
  if (bombCount <= 0) {
    const QString launcherName = _pendingBombRelease.launcherEntityName;
    clearAll();
    _setStatus(
        QStringLiteral("Bomb release queue stopped for %1. No bombs available.").arg(launcherName));
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

  const QString launcherReference = _pendingBombRelease.launcherReference();
  const QString launcherName  = _pendingBombRelease.launcherEntityName;
  const QString targetLabel   = _pendingBombRelease.targetLabel;
  const QString sourceDesc    = _pendingBombRelease.sourceDescription;
  _setStatus(QStringLiteral("Bomb release window reached for %1 on %2.")
                 .arg(launcherName, targetLabel));

  QTimer::singleShot(0, this, [this, launcherReference, launcherName, targetLabel, sourceDesc]() {
    if (!_pendingBombRelease.pending ||
        _pendingBombRelease.launcherReference() != launcherReference) {
      return;
    }
    if (!_state->releaseBomb(launcherReference)) {
      clearAll();
      _setStatus(
          QStringLiteral("No se pudo soltar una bomba desde %1. Cola detenida.").arg(launcherName));
      _syncUi();
      return;
    }
    _log(QStringLiteral("Bomb released from %1 at %2 (%3)")
             .arg(launcherName, targetLabel, sourceDesc));
    clear();
    armNextQueuedTarget();
    _setStatus(
        QStringLiteral("Bomba soltada desde %1 sobre %2.").arg(launcherName, targetLabel));
    _syncUi();
  });
}

void BombReleaseController::beginPickMode(const QString& launcherName) {
  _isPickingMode       = true;
  _isQueuePickingMode  = false;
  _pickingLauncherName = launcherName;
}

void BombReleaseController::beginQueuePickMode(const QString& launcherName) {
  _isPickingMode       = true;
  _isQueuePickingMode  = true;
  _pickingLauncherName = launcherName;
}

void BombReleaseController::cancelPickMode() {
  _isPickingMode = false;
  _isQueuePickingMode = false;
  _pickingLauncherName.clear();
}

bool BombReleaseController::isPickingMode() const {
  return _isPickingMode;
}

bool BombReleaseController::isQueuePickingMode() const {
  return _isQueuePickingMode;
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

  for (auto it = _targetQueue.begin(); it != _targetQueue.end();) {
    const bool queuedLauncherRemoved =
        it->launcherReference().compare(removedEntityName, Qt::CaseInsensitive) == 0;
    const bool queuedTargetRemoved =
        !it->targetReference().isEmpty() &&
        it->targetReference().compare(removedEntityName, Qt::CaseInsensitive) == 0;
    if (queuedLauncherRemoved || queuedTargetRemoved) {
      it = _targetQueue.erase(it);
      continue;
    }
    ++it;
  }

  if (!_pendingBombRelease.pending) {
    return QString();
  }

  const bool launcherRemoved =
      _pendingBombRelease.launcherReference().compare(
          removedEntityName, Qt::CaseInsensitive) == 0;
  const bool targetRemoved =
      !_pendingBombRelease.targetReference().isEmpty() &&
      _pendingBombRelease.targetReference().compare(
          removedEntityName, Qt::CaseInsensitive) == 0;

  if (!launcherRemoved && !targetRemoved) {
    return QString();
  }

  const QString reason = targetRemoved
      ? QStringLiteral("target removed")
      : QStringLiteral("launcher removed");
  clear();
  if (!launcherRemoved) {
    armNextQueuedTarget();
  } else {
    _targetQueue.clear();
  }
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
