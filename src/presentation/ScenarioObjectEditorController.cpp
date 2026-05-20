#include "presentation/ScenarioObjectEditorController.h"
#include "application/ScenarioState.h"

namespace presentation {

ScenarioObjectEditorController::ScenarioObjectEditorController(
    ScenarioState*    state,
    SelectedNameFn    selectedEntityName,
    SelectedObjectFn  selectedObjectName,
    IsEntityFn        isEntity,
    IsDestroyedFn     isDestroyed,
    SummaryFn         summary,
    CleanupEntityFn   cleanupEntity,
    RemoveTrackFn     removeTrack,
    LogFn             log,
    StatusFn          setStatus,
    SyncUiFn          syncUi,
    QObject*          parent)
    : QObject(parent)
    , _state(state)
    , _selectedEntityName(std::move(selectedEntityName))
    , _selectedObjectName(std::move(selectedObjectName))
    , _isEntity(std::move(isEntity))
    , _isDestroyed(std::move(isDestroyed))
    , _summary(std::move(summary))
    , _cleanupEntity(std::move(cleanupEntity))
    , _removeTrack(std::move(removeTrack))
    , _log(std::move(log))
    , _setStatus(std::move(setStatus))
    , _syncUi(std::move(syncUi)) {}

void ScenarioObjectEditorController::deleteSelected() {
  const QString objectName = _selectedObjectName();
  if (objectName.isEmpty()) {
    return;
  }

  const QVariantMap sum = _summary();
  const QString type = sum.value(QStringLiteral("type")).toString();

  bool removed = false;
  QString label = QStringLiteral("Object");
  if (_isEntity()) {
    removed = _state->removeEntity(objectName);
    label = QStringLiteral("Entity");
  } else if (type == QStringLiteral("Waypoint")) {
    removed = _state->removeWaypoint(objectName);
    label = QStringLiteral("Waypoint");
  } else if (type == QStringLiteral("Route")) {
    removed = _state->removeRoute(objectName);
    label = QStringLiteral("Route");
  } else if (type == QStringLiteral("Area")) {
    removed = _state->removeArea(objectName);
    label = QStringLiteral("Area");
  }

  if (!removed) {
    return;
  }

  QString cleanupStatus;
  if (label == QStringLiteral("Entity")) {
    cleanupStatus = _cleanupEntity(objectName);
  }
  _log(QStringLiteral("%1 deleted: %2").arg(label, objectName));
  _removeTrack(objectName);
  _syncUi();
  _setStatus(cleanupStatus.isEmpty()
      ? QStringLiteral("%1 eliminado: %2").arg(label, objectName)
      : cleanupStatus);
}

void ScenarioObjectEditorController::clearSelectedTask() {
  const QString entityName = _selectedEntityName();
  if (entityName.isEmpty() || _isDestroyed()) {
    return;
  }

  if (_state->clearTask(entityName)) {
    _log(QStringLiteral("Task cleared for %1").arg(entityName));
    _syncUi();
  }
}

} // namespace presentation
