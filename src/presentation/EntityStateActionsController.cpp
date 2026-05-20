#include "presentation/EntityStateActionsController.h"
#include "application/ScenarioState.h"
#include "presentation/EntityVisualStateManager.h"

namespace presentation {

EntityStateActionsController::EntityStateActionsController(
    ScenarioState*              state,
    EntityVisualStateManager*   visualState,
    SelectedNameFn              selectedName,
    IsDestroyedFn               isDestroyed,
    IsOperableFn                isOperable,
    ApplyTaskFn                 applyTask,
    FlyTargetsFn                flyTargets,
    AskDoubleFn                 askDouble,
    CloseTaskDialogFn           closeTaskDialog,
    LogFn                       log,
    StatusFn                    setStatus,
    SyncUiFn                    syncUi,
    QObject*                    parent)
    : QObject(parent)
    , _state(state)
    , _visualState(visualState)
    , _selectedName(std::move(selectedName))
    , _isDestroyed(std::move(isDestroyed))
    , _isOperable(std::move(isOperable))
    , _applyTask(std::move(applyTask))
    , _flyTargets(std::move(flyTargets))
    , _askDouble(std::move(askDouble))
    , _closeTaskDialog(std::move(closeTaskDialog))
    , _log(std::move(log))
    , _setStatus(std::move(setStatus))
    , _syncUi(std::move(syncUi)) {}

void EntityStateActionsController::destroySelected() {
  this->setSelectedDestroyed(true);
}

void EntityStateActionsController::restoreSelected() {
  this->setSelectedDestroyed(false);
}

void EntityStateActionsController::setSelectedDestroyed(bool destroyed) {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  if (!_state->setEntityDestroyed(entityName, destroyed)) {
    return;
  }
  if (destroyed) {
    _closeTaskDialog();
  }
  _log(QStringLiteral("Entity %1 %2")
      .arg(entityName, destroyed ? QStringLiteral("marked as destroyed")
                                 : QStringLiteral("restored")));
  _syncUi();
  _setStatus(destroyed
      ? QStringLiteral("Entidad destruida: %1").arg(entityName)
      : QStringLiteral("Entidad restaurada: %1").arg(entityName));
}

void EntityStateActionsController::setSelectedBehaviorMode(const QString& mode) {
  const QString entityName = _selectedName();
  if (entityName.isEmpty() || _isDestroyed()) {
    return;
  }
  if (!_state->setEntityBehaviorMode(entityName, mode)) {
    _setStatus(QStringLiteral("No se pudo cambiar el behavior mode de %1.")
        .arg(entityName));
    return;
  }
  _syncUi();
  _setStatus(QStringLiteral("%1 behavior mode: %2.").arg(entityName, mode));
}

void EntityStateActionsController::setSelectedHidden(bool hidden) {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  if (!_visualState->setFlag(
          entityName,
          EntityVisualStateManager::Flag::Hidden,
          hidden)) {
    return;
  }
  _visualState->save();
  _log(QStringLiteral("Entity %1 %2")
      .arg(entityName, hidden ? QStringLiteral("hidden") : QStringLiteral("shown")));
  _syncUi();
  _setStatus(hidden
      ? QStringLiteral("Entidad oculta: %1").arg(entityName)
      : QStringLiteral("Entidad visible: %1").arg(entityName));
}

void EntityStateActionsController::setSelectedRadarCoverageVisible(bool visible) {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  if (!_visualState->setFlag(
          entityName,
          EntityVisualStateManager::Flag::RadarCoverageVisible,
          visible)) {
    return;
  }
  _visualState->save();
  _log(QStringLiteral("Radar coverage %1 for %2")
      .arg(visible ? QStringLiteral("enabled") : QStringLiteral("disabled"),
           entityName));
  _syncUi();
  _setStatus(visible
      ? QStringLiteral("Cobertura radar visible para %1").arg(entityName)
      : QStringLiteral("Cobertura radar oculta para %1").arg(entityName));
}

void EntityStateActionsController::setSelectedTrackHistoryVisible(bool visible) {
  const QString entityName = _selectedName();
  if (entityName.isEmpty()) {
    return;
  }
  if (!_visualState->setFlag(
          entityName,
          EntityVisualStateManager::Flag::TrackHistoryVisible,
          visible)) {
    return;
  }
  _visualState->save();
  _log(QStringLiteral("Track history %1 for %2")
      .arg(visible ? QStringLiteral("enabled") : QStringLiteral("disabled"),
           entityName));
  _syncUi();
  _setStatus(visible
      ? QStringLiteral("Historial de trayectoria visible para %1").arg(entityName)
      : QStringLiteral("Historial de trayectoria oculto para %1").arg(entityName));
}

void EntityStateActionsController::setSelectedHeading() {
  double heading = 0.0;
  int alt = 0;
  double speed = 0.0;
  if (!_flyTargets(heading, alt, speed)) {
    return;
  }
  bool ok = false;
  const double newHeading = _askDouble(
      QStringLiteral("Set Heading"), QStringLiteral("Heading (deg)"),
      heading, 0.0, 360.0, ok);
  if (!ok) {
    return;
  }
  this->applyFlyTask(newHeading, alt, speed);
}

void EntityStateActionsController::setSelectedAltitude() {
  double heading = 0.0;
  int alt = 0;
  double speed = 0.0;
  if (!_flyTargets(heading, alt, speed)) {
    return;
  }
  bool ok = false;
  const double newAlt = _askDouble(
      QStringLiteral("Set Altitude"), QStringLiteral("Altitude (m)"),
      static_cast<double>(alt), 0.0, 60000.0, ok);
  if (!ok) {
    return;
  }
  this->applyFlyTask(heading, static_cast<int>(std::lround(newAlt)), speed);
}

void EntityStateActionsController::setSelectedSpeed() {
  double heading = 0.0;
  int alt = 0;
  double speed = 0.0;
  if (!_flyTargets(heading, alt, speed)) {
    return;
  }
  bool ok = false;
  const double newSpeed = _askDouble(
      QStringLiteral("Set Speed"), QStringLiteral("Speed (kts)"),
      speed, 0.0, 2000.0, ok);
  if (!ok) {
    return;
  }
  this->applyFlyTask(heading, alt, newSpeed);
}

void EntityStateActionsController::applyFlyTask(
    double headingDeg, int altMeters, double speedKnots) {
  const QString entityName = _selectedName();
  if (entityName.isEmpty() || _isDestroyed()) {
    return;
  }
  EntityTask task;
  task.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  task.enabled  = true;
  task.status   = QStringLiteral("Running");
  task.targetHeadingDegrees = headingDeg;
  task.targetAltitudeMeters = altMeters;
  task.targetSpeedKnots     = speedKnots;
  _applyTask(entityName, task, /*syncUi=*/true);
}

} // namespace presentation
