#include "presentation/AssignTaskController.h"

namespace presentation {

AssignTaskController::AssignTaskController(
    SelectedEntityFn  selectedEntityName,
    IsOperableFn      isOperable,
    IsPlanRunningFn   isPlanRunning,
    CurrentSummaryFn  currentSummary,
    CaptureConfigFn   captureConfig,
    ApplyTaskFn       applyTask,
    StatusFn          setStatus,
    QObject*          parent)
    : QObject(parent)
    , _selectedEntityName(std::move(selectedEntityName))
    , _isOperable(std::move(isOperable))
    , _isPlanRunning(std::move(isPlanRunning))
    , _currentSummary(std::move(currentSummary))
    , _captureConfig(std::move(captureConfig))
    , _applyTask(std::move(applyTask))
    , _setStatus(std::move(setStatus)) {}

// static
EntityTask AssignTaskController::taskFromSummary(const QVariantMap& summary) {
  EntityTask task;
  task.taskType               = summary.value(QStringLiteral("taskType")).toString();
  task.enabled                = summary.value(QStringLiteral("taskEnabled")).toBool();
  task.status                 = summary.value(QStringLiteral("taskStatus")).toString();
  task.targetHeadingDegrees   = summary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble();
  task.targetAltitudeMeters   = summary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt();
  task.targetSpeedKnots       = summary.value(QStringLiteral("taskTargetSpeedKnots")).toDouble();
  task.targetLatitude         = summary.value(QStringLiteral("taskTargetLatitude")).toDouble();
  task.targetLongitude        = summary.value(QStringLiteral("taskTargetLongitude")).toDouble();
  task.targetEntityName       = summary.value(QStringLiteral("taskTargetEntityName")).toString();
  task.targetWaypointName     = summary.value(QStringLiteral("taskTargetWaypointName")).toString();
  task.targetRouteName        = summary.value(QStringLiteral("taskTargetRouteName")).toString();
  task.targetAreaName         = summary.value(QStringLiteral("taskTargetAreaName")).toString();
  task.targetAreaRadiusMeters = summary.value(QStringLiteral("taskTargetAreaRadiusMeters")).toDouble();
  task.followDistanceMeters   = summary.value(QStringLiteral("taskFollowDistanceMeters"), 1000.0).toDouble();
  task.arrivalToleranceMeters = summary.value(QStringLiteral("taskArrivalToleranceMeters"), 100.0).toDouble();
  task.durationSeconds        = summary.value(QStringLiteral("taskDurationSeconds")).toDouble();
  task.elapsedSeconds         = summary.value(QStringLiteral("taskElapsedSeconds")).toDouble();
  task.interceptDistanceMeters =
      summary.value(QStringLiteral("taskInterceptDistanceMeters"), 500.0).toDouble();
  task.timeoutSeconds         = summary.value(QStringLiteral("taskTimeoutSeconds"), 120.0).toDouble();
  return task;
}

void AssignTaskController::open(const QString& initialTaskType) {
  const QString entityName = _selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (!_isOperable()) {
    _setStatus(QStringLiteral("La entidad seleccionada no esta operable."));
    return;
  }

  if (_isPlanRunning(entityName)) {
    _setStatus(QStringLiteral("No puedes editar la task mientras el plan esta en ejecucion."));
    return;
  }

  const EntityTask currentTask = taskFromSummary(_currentSummary());

  EntityTask configuredTask;
  if (!_captureConfig(entityName, currentTask, initialTaskType, configuredTask)) {
    return;
  }

  _applyTask(entityName, configuredTask);
}

} // namespace presentation
