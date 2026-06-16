#include "presentation/TaskAssignmentController.h"

namespace {
  constexpr double kOrbitHoldDefaultRadiusMeters = 1500.0;
} // namespace

namespace presentation {

TaskAssignmentController::TaskAssignmentController(
    SelectedNameFn    selectedName,
    IsOperableFn      isOperable,
    SummaryFn         summary,
    HomePositionFn    homePosition,
    AvailableRoutesFn availableRoutes,
    FlyTargetsFn      flyTargets,
    ApplyTaskFn       applyTask,
    OpenDialogFn      openDialog,
    PickItemFn        pickItem,
    AskDoubleFn       askDouble,
    StatusFn          setStatus,
    QObject*          parent)
    : QObject(parent)
    , _selectedName(std::move(selectedName))
    , _isOperable(std::move(isOperable))
    , _summary(std::move(summary))
    , _homePosition(std::move(homePosition))
    , _availableRoutes(std::move(availableRoutes))
    , _flyTargets(std::move(flyTargets))
    , _applyTask(std::move(applyTask))
    , _openDialog(std::move(openDialog))
    , _pickItem(std::move(pickItem))
    , _askDouble(std::move(askDouble))
    , _setStatus(std::move(setStatus)) {}

void TaskAssignmentController::assignReturnToBase() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty() || !_isOperable()) {
    return;
  }

  const QVariantMap summary = _summary();
  const EntityHomePosition homePos = _homePosition(entityName);

  double headingDegrees = 0.0;
  int currentAltitudeMeters = 0;
  double speedKnots = summary.value(QStringLiteral("speedKnots")).toDouble();
  _flyTargets(headingDegrees, currentAltitudeMeters, speedKnots);

  EntityTask task;
  task.taskType = QStringLiteral("MoveToLocation");
  task.enabled  = true;
  task.status   = QStringLiteral("Running");
  task.targetLatitude = homePos.valid
      ? homePos.latitude
      : summary.value(QStringLiteral("latitude")).toDouble();
  task.targetLongitude = homePos.valid
      ? homePos.longitude
      : summary.value(QStringLiteral("longitude")).toDouble();
  task.targetAltitudeMeters = homePos.valid
      ? homePos.altitudeMeters
      : currentAltitudeMeters;
  task.targetSpeedKnots = speedKnots;

  if (!_applyTask(entityName, task)) {
    return;
  }
  _setStatus(QStringLiteral("RTB asignado a %1.").arg(entityName));
}

void TaskAssignmentController::assignPatrolRoute() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty() || !_isOperable()) {
    return;
  }

  const QStringList availableRoutes = _availableRoutes(true);
  if (availableRoutes.isEmpty()) {
    _setStatus(QStringLiteral("No hay rutas disponibles para Patrol Route."));
    return;
  }

  const QVariantMap summary = _summary();
  const QString currentRouteName = summary.value(QStringLiteral("taskTargetRouteName")).toString();
  int routeIndex = availableRoutes.indexOf(currentRouteName);
  if (routeIndex < 0) {
    routeIndex = 0;
  }

  bool ok = false;
  const QString routeName = _pickItem(
      QStringLiteral("Patrol Route"), QStringLiteral("Route"),
      availableRoutes, routeIndex, ok);
  if (!ok || routeName.trimmed().isEmpty()) {
    return;
  }

  double headingDegrees = 0.0;
  int altitudeMeters    = 0;
  double speedKnots     = summary.value(QStringLiteral("speedKnots")).toDouble();
  _flyTargets(headingDegrees, altitudeMeters, speedKnots);

  EntityTask task;
  task.taskType          = QStringLiteral("FollowRoute");
  task.enabled           = true;
  task.status            = QStringLiteral("Running");
  task.targetRouteName   = routeName;
  task.targetAltitudeMeters = altitudeMeters;
  task.targetSpeedKnots  = speedKnots;
  task.arrivalToleranceMeters =
      summary.value(QStringLiteral("domain")).toString().compare(
          QStringLiteral("Ground"), Qt::CaseInsensitive) == 0
      ? 500.0
      : 1000.0;

  if (!_applyTask(entityName, task)) {
    return;
  }
  _setStatus(QStringLiteral("Patrol Route asignado a %1 sobre %2.")
      .arg(entityName, routeName));
}

void TaskAssignmentController::assignOrbitHoldLocation() {
  const QString entityName = _selectedName();
  if (entityName.isEmpty() || !_isOperable()) {
    return;
  }

  const QVariantMap summary = _summary();
  double centerLatitude  = summary.value(QStringLiteral("latitude")).toDouble();
  double centerLongitude = summary.value(QStringLiteral("longitude")).toDouble();

  if (!resolveOrbitCenter(centerLatitude, centerLongitude)) {
    return;
  }

  double headingDegrees = 0.0;
  int altitudeMeters    = 0;
  double speedKnots     = summary.value(QStringLiteral("speedKnots")).toDouble();
  _flyTargets(headingDegrees, altitudeMeters, speedKnots);

  EntityTask task;
  task.taskType              = QStringLiteral("OrbitArea");
  task.enabled               = true;
  task.status                = QStringLiteral("Running");
  task.targetLatitude        = centerLatitude;
  task.targetLongitude       = centerLongitude;
  task.targetAltitudeMeters  = altitudeMeters;
  task.targetSpeedKnots      = speedKnots;
  task.targetAreaRadiusMeters = kOrbitHoldDefaultRadiusMeters;

  if (!_applyTask(entityName, task)) {
    return;
  }
  _setStatus(QStringLiteral("Orbit / Hold asignado a %1 alrededor de %2, %3.")
      .arg(entityName)
      .arg(centerLatitude,  0, 'f', 4)
      .arg(centerLongitude, 0, 'f', 4));
}

void TaskAssignmentController::assignHoldRacetrack() {
  _openDialog(QStringLiteral("HoldRacetrack"));
}

bool TaskAssignmentController::resolveOrbitCenter(
    double& outLatitude, double& outLongitude) {
  bool ok = false;
  const QString centerMode = _pickItem(
      QStringLiteral("Orbit / Hold (Location)"),
      QStringLiteral("Center"),
      QStringList{
          QStringLiteral("Current Position"),
          QStringLiteral("Custom Coordinates"),
      },
      0, ok);
  if (!ok) {
    return false;
  }

  if (centerMode != QStringLiteral("Custom Coordinates")) {
    return true;
  }

  outLatitude = _askDouble(
      QStringLiteral("Orbit / Hold (Location)"), QStringLiteral("Latitude"),
      outLatitude, -90.0, 90.0, 6, ok);
  if (!ok) {
    return false;
  }

  outLongitude = _askDouble(
      QStringLiteral("Orbit / Hold (Location)"), QStringLiteral("Longitude"),
      outLongitude, -180.0, 180.0, 6, ok);
  return ok;
}

void TaskAssignmentController::assignFlyHeadingAltitudeSpeed() {
  _openDialog(QStringLiteral("FlyHeadingAltitudeSpeed"));
}

void TaskAssignmentController::assignMoveToLocation() {
  _openDialog(QStringLiteral("MoveToLocation"));
}

void TaskAssignmentController::assignWaitOnLocation() {
  _openDialog(QStringLiteral("WaitOnLocation"));
}

void TaskAssignmentController::assignMoveToWaypoint() {
  _openDialog(QStringLiteral("MoveToWaypoint"));
}

void TaskAssignmentController::assignMoveAlongRoute() {
  _openDialog(QStringLiteral("FollowRoute"));
}

void TaskAssignmentController::assignPatrolArea() {
  _openDialog(QStringLiteral("PatrolArea"));
}

void TaskAssignmentController::assignOrbitArea() {
  _openDialog(QStringLiteral("OrbitArea"));
}

void TaskAssignmentController::assignFollowEntity() {
  _openDialog(QStringLiteral("FollowEntity"));
}

void TaskAssignmentController::assignInterceptEntity() {
  _openDialog(QStringLiteral("InterceptEntity"));
}

void TaskAssignmentController::assignInterceptEntity2D() {
  assignInterceptEntity();
}

void TaskAssignmentController::assignInterceptEntity3D() {
  assignInterceptEntity();
}

void TaskAssignmentController::assignAttackOnce() {
  _openDialog(QStringLiteral("AttackOnce"));
}

void TaskAssignmentController::assignAttackUntilDestroyed() {
  _openDialog(QStringLiteral("AttackUntilDestroyed"));
}

void TaskAssignmentController::assignFireOnPosition() {
  _openDialog(QStringLiteral("FireOnPosition"));
}

void TaskAssignmentController::assignFireInDirection() {
  _openDialog(QStringLiteral("FireInDirection"));
}

void TaskAssignmentController::assignStopWeaponsTask() {
  _openDialog(QStringLiteral("StopWeaponsTask"));
}

void TaskAssignmentController::assignAttackAir() {
  _openDialog(QStringLiteral("AttackAir"));
}

void TaskAssignmentController::assignAttackSurface() {
  _openDialog(QStringLiteral("AttackSurface"));
}

void TaskAssignmentController::assignWaitUntilTargetDetected() {
  _openDialog(QStringLiteral("WaitUntilTargetDetected"));
}

void TaskAssignmentController::assignWaitUntilTargetDestroyed() {
  _openDialog(QStringLiteral("WaitUntilTargetDestroyed"));
}

void TaskAssignmentController::assignWaitUntilDamaged() {
  _openDialog(QStringLiteral("WaitUntilDamaged"));
}

void TaskAssignmentController::assignWaitUntilTime() {
  _openDialog(QStringLiteral("WaitUntilTime"));
}

void TaskAssignmentController::assignWaitUntilInRange() {
  _openDialog(QStringLiteral("WaitUntilInRange"));
}

} // namespace presentation
