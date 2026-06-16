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
  task.routeCurrentWaypointIndex =
      summary.value(QStringLiteral("taskRouteCurrentWaypointIndex")).toInt();
  task.routeTotalWaypoints =
      summary.value(QStringLiteral("taskRouteTotalWaypoints")).toInt();
  task.interceptDistanceMeters =
      summary.value(QStringLiteral("taskInterceptDistanceMeters"), 500.0).toDouble();
  task.altitudeToleranceMeters =
      summary.value(QStringLiteral("taskAltitudeToleranceMeters"), 100.0).toDouble();
  task.timeoutSeconds         = summary.value(QStringLiteral("taskTimeoutSeconds"), 120.0).toDouble();
  task.racetrackLegLengthMeters =
      summary.value(QStringLiteral("taskRacetrackLegLengthMeters"), 10000.0).toDouble();
  task.weaponType              = summary.value(QStringLiteral("taskWeaponType")).toString();
  task.targetDomain            = summary.value(QStringLiteral("taskTargetDomain")).toString();
  task.enemyOnly               = summary.value(QStringLiteral("taskEnemyOnly"), true).toBool();
  task.damageThresholdPercent  =
      summary.value(QStringLiteral("taskDamageThresholdPercent"), 50.0).toDouble();
  task.rangeMeters             = summary.value(QStringLiteral("taskRangeMeters"), 1000.0).toDouble();
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

  const QVariantMap summary = _currentSummary();
  EntityTask currentTask = taskFromSummary(summary);
  if (initialTaskType == QStringLiteral("HoldRacetrack") &&
      currentTask.taskType != QStringLiteral("HoldRacetrack")) {
    currentTask.targetLatitude = summary.value(QStringLiteral("latitude")).toDouble();
    currentTask.targetLongitude = summary.value(QStringLiteral("longitude")).toDouble();
    currentTask.targetHeadingDegrees = summary.value(QStringLiteral("headingDegrees")).toDouble();
    currentTask.targetAltitudeMeters = summary.value(QStringLiteral("altitude")).toInt();
    currentTask.targetSpeedKnots = summary.value(QStringLiteral("speedKnots")).toDouble();
    currentTask.racetrackLegLengthMeters = 10000.0;
    currentTask.durationSeconds = 0.0;
  }
  if (initialTaskType == QStringLiteral("WaitOnLocation") &&
      currentTask.taskType != QStringLiteral("WaitOnLocation")) {
    currentTask.targetLatitude = summary.value(QStringLiteral("latitude")).toDouble();
    currentTask.targetLongitude = summary.value(QStringLiteral("longitude")).toDouble();
    currentTask.targetAltitudeMeters = summary.value(QStringLiteral("altitude")).toInt();
    currentTask.targetSpeedKnots = summary.value(QStringLiteral("speedKnots")).toDouble();
    currentTask.arrivalToleranceMeters = 200.0;
    currentTask.durationSeconds = 0.0;
  }
  if (initialTaskType == QStringLiteral("AttackOnce") &&
      currentTask.taskType != QStringLiteral("AttackOnce")) {
    currentTask.weaponType = QStringLiteral("Auto");
    currentTask.timeoutSeconds = 120.0;
  }

  EntityTask configuredTask;
  if (!_captureConfig(entityName, currentTask, initialTaskType, configuredTask)) {
    return;
  }

  _applyTask(entityName, configuredTask);
}

} // namespace presentation
