#include "AssignTaskDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QtMath>
#include <QVBoxLayout>

namespace {

bool isInterceptEntityTaskType(const QString& taskType) {
  return taskType == QStringLiteral("InterceptEntity") ||
         taskType == QStringLiteral("InterceptEntity2D") ||
         taskType == QStringLiteral("InterceptEntity3D");
}

} // namespace

AssignTaskDialog::AssignTaskDialog(
    const QString& entityName,
    const QStringList& availableTargets,
    const QStringList& availableWaypoints,
    const QStringList& availableRoutes,
    const QStringList& availableAreas,
    const EntityTask& currentTask,
    const QString& initialTaskType,
    QWidget* parent)
    : QDialog(parent),
      _taskTypeCombo(new QComboBox(this)),
      _headingSpin(new QDoubleSpinBox(this)),
      _altitudeSpin(new QDoubleSpinBox(this)),
      _speedSpin(new QDoubleSpinBox(this)),
      _latitudeSpin(new QDoubleSpinBox(this)),
      _longitudeSpin(new QDoubleSpinBox(this)),
      _followDistanceSpin(new QDoubleSpinBox(this)),
      _arrivalToleranceSpin(new QDoubleSpinBox(this)),
      _altitudeToleranceSpin(new QDoubleSpinBox(this)),
      _durationSpin(new QDoubleSpinBox(this)),
      _followTargetCombo(new QComboBox(this)),
      _waypointCombo(new QComboBox(this)),
      _routeCombo(new QComboBox(this)),
      _areaCombo(new QComboBox(this)),
      _weaponTypeCombo(new QComboBox(this)),
      _targetDomainCombo(new QComboBox(this)),
      _enemyOnlyCheck(new QCheckBox(this)) {
  this->setWindowTitle(QStringLiteral("Assign Task"));

  auto* layout = new QVBoxLayout(this);
  auto* hintLabel = new QLabel(
      QStringLiteral("Assign a task to %1").arg(entityName),
      this);
  hintLabel->setWordWrap(true);
  layout->addWidget(hintLabel);

  _taskTypeCombo->addItem(QStringLiteral("Fly Heading / Altitude / Speed"), QStringLiteral("FlyHeadingAltitudeSpeed"));
  _taskTypeCombo->addItem(QStringLiteral("Move To Location"), QStringLiteral("MoveToLocation"));
  _taskTypeCombo->addItem(QStringLiteral("Wait on Location"), QStringLiteral("WaitOnLocation"));
  _taskTypeCombo->addItem(QStringLiteral("Move To Waypoint"), QStringLiteral("MoveToWaypoint"));
  _taskTypeCombo->addItem(QStringLiteral("Follow Route"), QStringLiteral("FollowRoute"));
  _taskTypeCombo->addItem(QStringLiteral("Patrol Area"), QStringLiteral("PatrolArea"));
  _taskTypeCombo->addItem(QStringLiteral("Orbit Area"), QStringLiteral("OrbitArea"));
  _taskTypeCombo->addItem(QStringLiteral("Hold Racetrack"), QStringLiteral("HoldRacetrack"));
  _taskTypeCombo->addItem(QStringLiteral("Follow Entity"), QStringLiteral("FollowEntity"));
  _taskTypeCombo->addItem(QStringLiteral("Intercept Entity"), QStringLiteral("InterceptEntity"));
  _taskTypeCombo->addItem(QStringLiteral("Attack Once"), QStringLiteral("AttackOnce"));
  _taskTypeCombo->addItem(QStringLiteral("Attack Air"), QStringLiteral("AttackAir"));
  _taskTypeCombo->addItem(QStringLiteral("Attack Surface"), QStringLiteral("AttackSurface"));
  _taskTypeCombo->addItem(QStringLiteral("Wait Until Target Detected"), QStringLiteral("WaitUntilTargetDetected"));
  _taskTypeCombo->addItem(QStringLiteral("Wait Until Target Destroyed"), QStringLiteral("WaitUntilTargetDestroyed"));
  _taskTypeCombo->addItem(QStringLiteral("Wait Until Damaged"), QStringLiteral("WaitUntilDamaged"));
  _taskTypeCombo->addItem(QStringLiteral("Wait Until Time"), QStringLiteral("WaitUntilTime"));
  _taskTypeCombo->addItem(QStringLiteral("Wait Until In Range"), QStringLiteral("WaitUntilInRange"));

  _headingSpin->setRange(0.0, 359.0);
  _headingSpin->setDecimals(1);
  _headingSpin->setSingleStep(5.0);
  _headingSpin->setSuffix(QStringLiteral(" deg"));

  _altitudeSpin->setRange(-1000.0, 80000.0);
  _altitudeSpin->setDecimals(1);
  _altitudeSpin->setSuffix(QStringLiteral(" m"));

  _speedSpin->setRange(0.0, 1200.0);
  _speedSpin->setDecimals(1);
  _speedSpin->setSingleStep(10.0);
  _speedSpin->setSuffix(QStringLiteral(" kts"));

  _latitudeSpin->setRange(-90.0, 90.0);
  _latitudeSpin->setDecimals(6);
  _latitudeSpin->setSingleStep(0.001);
  _latitudeSpin->setSuffix(QStringLiteral(" deg"));

  _longitudeSpin->setRange(-180.0, 180.0);
  _longitudeSpin->setDecimals(6);
  _longitudeSpin->setSingleStep(0.001);
  _longitudeSpin->setSuffix(QStringLiteral(" deg"));

  _followDistanceSpin->setRange(1.0, 100000.0);
  _followDistanceSpin->setDecimals(1);
  _followDistanceSpin->setSingleStep(100.0);
  _followDistanceSpin->setSuffix(QStringLiteral(" m"));

  _arrivalToleranceSpin->setRange(0.0, 10000.0);
  _arrivalToleranceSpin->setDecimals(1);
  _arrivalToleranceSpin->setSingleStep(50.0);
  _arrivalToleranceSpin->setSuffix(QStringLiteral(" m"));

  _altitudeToleranceSpin->setRange(0.0, 20000.0);
  _altitudeToleranceSpin->setDecimals(1);
  _altitudeToleranceSpin->setSingleStep(50.0);
  _altitudeToleranceSpin->setSuffix(QStringLiteral(" m"));

  _durationSpin->setRange(0.0, 86400.0);
  _durationSpin->setDecimals(1);
  _durationSpin->setSingleStep(60.0);
  _durationSpin->setSpecialValueText(QStringLiteral("Until cancelled"));
  _durationSpin->setSuffix(QStringLiteral(" s"));

  for (const QString& targetName : availableTargets) {
    _followTargetCombo->addItem(targetName, targetName);
  }
  _followTargetCombo->addItem(QStringLiteral("Coordinates / None"), QString());
  _waypointCombo->addItems(availableWaypoints);
  _routeCombo->addItems(availableRoutes);
  _areaCombo->addItems(availableAreas);
  _weaponTypeCombo->addItem(QStringLiteral("Auto"), QStringLiteral("Auto"));
  _weaponTypeCombo->addItem(QStringLiteral("Missile"), QStringLiteral("Missile"));
  _weaponTypeCombo->addItem(QStringLiteral("Bomb"), QStringLiteral("Bomb"));
  _targetDomainCombo->addItem(QStringLiteral("Any"), QStringLiteral("Any"));
  _targetDomainCombo->addItem(QStringLiteral("Air"), QStringLiteral("Air"));
  _targetDomainCombo->addItem(QStringLiteral("Ground"), QStringLiteral("Ground"));
  _targetDomainCombo->addItem(QStringLiteral("Land"), QStringLiteral("Land"));
  _targetDomainCombo->addItem(QStringLiteral("Surface"), QStringLiteral("Surface"));
  _enemyOnlyCheck->setText(QStringLiteral("Enemy only"));

  auto* formLayout = new QFormLayout();
  formLayout->addRow(QStringLiteral("Task"), _taskTypeCombo);
  formLayout->addRow(QStringLiteral("Heading"), _headingSpin);
  formLayout->addRow(QStringLiteral("Speed"), _speedSpin);
  formLayout->addRow(QStringLiteral("Latitude"), _latitudeSpin);
  formLayout->addRow(QStringLiteral("Longitude"), _longitudeSpin);
  formLayout->addRow(QStringLiteral("Altitude"), _altitudeSpin);
  formLayout->addRow(QStringLiteral("Target Waypoint"), _waypointCombo);
  formLayout->addRow(QStringLiteral("Target Route"), _routeCombo);
  formLayout->addRow(QStringLiteral("Target Area"), _areaCombo);
  formLayout->addRow(QStringLiteral("Target Entity"), _followTargetCombo);
  formLayout->addRow(QStringLiteral("Target Domain"), _targetDomainCombo);
  formLayout->addRow(QStringLiteral("Enemy Filter"), _enemyOnlyCheck);
  formLayout->addRow(QStringLiteral("Weapon"), _weaponTypeCombo);
  formLayout->addRow(QStringLiteral("Follow / Intercept Distance"), _followDistanceSpin);
  formLayout->addRow(QStringLiteral("Arrival Tolerance"), _arrivalToleranceSpin);
  formLayout->addRow(QStringLiteral("Altitude Tolerance"), _altitudeToleranceSpin);
  formLayout->addRow(QStringLiteral("Duration / Timeout"), _durationSpin);
  layout->addLayout(formLayout);

  const QString rawInitialType = initialTaskType.isEmpty() ? currentTask.taskType : initialTaskType;
  const QString initialType = isInterceptEntityTaskType(rawInitialType)
      ? QStringLiteral("InterceptEntity")
      : (rawInitialType == QStringLiteral("MoveAlongRoute")
         ? QStringLiteral("FollowRoute")
         : rawInitialType);
  const int taskIndex = _taskTypeCombo->findData(initialType);
  if (taskIndex >= 0) {
    _taskTypeCombo->setCurrentIndex(taskIndex);
  }

  _headingSpin->setValue(currentTask.targetHeadingDegrees);
  _speedSpin->setValue(currentTask.targetSpeedKnots > 0.0 ? currentTask.targetSpeedKnots : 320.0);
  _latitudeSpin->setValue(currentTask.targetLatitude);
  _longitudeSpin->setValue(currentTask.targetLongitude);
  _altitudeSpin->setValue(currentTask.targetAltitudeMeters);
  _followDistanceSpin->setValue(
      isInterceptEntityTaskType(rawInitialType)
      ? (currentTask.interceptDistanceMeters > 0.0 ? currentTask.interceptDistanceMeters : 500.0)
      : (initialType == QStringLiteral("WaitUntilInRange")
         ? (currentTask.rangeMeters > 0.0 ? currentTask.rangeMeters : 1000.0)
      : (initialType == QStringLiteral("HoldRacetrack")
         ? (currentTask.racetrackLegLengthMeters > 0.0 ? currentTask.racetrackLegLengthMeters : 10000.0)
         : (currentTask.followDistanceMeters > 0.0 ? currentTask.followDistanceMeters : 1000.0))));
  _arrivalToleranceSpin->setValue(
      initialType == QStringLiteral("FollowRoute") || initialType == QStringLiteral("MoveAlongRoute")
      ? (currentTask.arrivalToleranceMeters > 0.0 ? currentTask.arrivalToleranceMeters : 1000.0)
      : currentTask.arrivalToleranceMeters);
  _altitudeToleranceSpin->setValue(
      initialType == QStringLiteral("WaitUntilDamaged")
      ? (currentTask.damageThresholdPercent > 0.0 ? currentTask.damageThresholdPercent : 50.0)
      : (currentTask.altitudeToleranceMeters > 0.0 ? currentTask.altitudeToleranceMeters : 100.0));
  _durationSpin->setValue(
      isInterceptEntityTaskType(rawInitialType) ||
          rawInitialType == QStringLiteral("AttackOnce") ||
          rawInitialType == QStringLiteral("FollowRoute") ||
          rawInitialType == QStringLiteral("MoveAlongRoute")
      ? (currentTask.timeoutSeconds > 0.0 ? currentTask.timeoutSeconds : 120.0)
      : rawInitialType == QStringLiteral("HoldRacetrack")
        ? currentTask.durationSeconds
      : currentTask.durationSeconds);
  if (!currentTask.targetEntityName.trimmed().isEmpty()) {
    const int followIndex = _followTargetCombo->findData(currentTask.targetEntityName);
    if (followIndex >= 0) {
      _followTargetCombo->setCurrentIndex(followIndex);
    }
  }
  const int waypointIndex = _waypointCombo->findText(currentTask.targetWaypointName);
  if (waypointIndex >= 0) {
    _waypointCombo->setCurrentIndex(waypointIndex);
  }
  const int routeIndex = _routeCombo->findText(currentTask.targetRouteName);
  if (routeIndex >= 0) {
    _routeCombo->setCurrentIndex(routeIndex);
  }
  const int areaIndex = _areaCombo->findText(currentTask.targetAreaName);
  if (areaIndex >= 0) {
    _areaCombo->setCurrentIndex(areaIndex);
  }
  const int weaponIndex = _weaponTypeCombo->findData(
      currentTask.weaponType.trimmed().isEmpty()
      ? QStringLiteral("Auto")
      : currentTask.weaponType.trimmed());
  if (weaponIndex >= 0) {
    _weaponTypeCombo->setCurrentIndex(weaponIndex);
  }
  const int domainIndex = _targetDomainCombo->findData(
      currentTask.targetDomain.trimmed().isEmpty()
      ? QStringLiteral("Any")
      : currentTask.targetDomain.trimmed());
  if (domainIndex >= 0) {
    _targetDomainCombo->setCurrentIndex(domainIndex);
  }
  _enemyOnlyCheck->setChecked(currentTask.enemyOnly);

  QObject::connect(
      _taskTypeCombo,
      &QComboBox::currentIndexChanged,
      this,
      [this](int) { this->syncUiForTaskType(); });
  this->syncUiForTaskType();

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal,
      this);
  auto* pickOnMapButton = buttons->addButton(
      QStringLiteral("Pick on map"),
      QDialogButtonBox::ActionRole);
  QObject::connect(pickOnMapButton, &QPushButton::clicked, this, [this]() {
    emit pickOnMapRequested();
  });
  QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(buttons);
}

EntityTask AssignTaskDialog::task() const {
  EntityTask task;
  task.enabled = true;
  task.status = QStringLiteral("Queued");
  task.taskType = _taskTypeCombo->currentData().toString();
  task.targetHeadingDegrees = _headingSpin->value();
  task.targetAltitudeMeters = static_cast<int>(qRound(_altitudeSpin->value()));
  task.targetSpeedKnots = _speedSpin->value();
  task.targetLatitude = _latitudeSpin->value();
  task.targetLongitude = _longitudeSpin->value();
  task.targetEntityName = _followTargetCombo->currentData().toString().trimmed();
  task.targetWaypointName = _waypointCombo->currentText().trimmed();
  task.targetRouteName = _routeCombo->currentText().trimmed();
  task.targetAreaName = _areaCombo->currentText().trimmed();
  task.followDistanceMeters = _followDistanceSpin->value();
  task.arrivalToleranceMeters = _arrivalToleranceSpin->value();
  task.durationSeconds = _durationSpin->value();
  task.interceptDistanceMeters = _followDistanceSpin->value();
  task.altitudeToleranceMeters = _altitudeToleranceSpin->value();
  task.timeoutSeconds = _durationSpin->value();
  task.racetrackLegLengthMeters = _followDistanceSpin->value();
  task.weaponType = _weaponTypeCombo->currentData().toString();
  task.targetDomain = _targetDomainCombo->currentData().toString();
  task.enemyOnly = _enemyOnlyCheck->isChecked();
  task.damageThresholdPercent = _altitudeToleranceSpin->value();
  task.rangeMeters = _followDistanceSpin->value();
  task.elapsedSeconds = 0.0;
  return task;
}

void AssignTaskDialog::syncUiForTaskType() {
  const QString taskType = _taskTypeCombo->currentData().toString();
  const bool isFlyTask = taskType == QStringLiteral("FlyHeadingAltitudeSpeed");
  const bool isMoveTask = taskType == QStringLiteral("MoveToLocation");
  const bool isWaitTask = taskType == QStringLiteral("WaitOnLocation");
  const bool isWaypointTask = taskType == QStringLiteral("MoveToWaypoint");
  const bool isRouteTask =
      taskType == QStringLiteral("FollowRoute") ||
      taskType == QStringLiteral("MoveAlongRoute");
  const bool isAreaTask =
      taskType == QStringLiteral("PatrolArea") ||
      taskType == QStringLiteral("OrbitArea");
  const bool isRacetrackTask = taskType == QStringLiteral("HoldRacetrack");
  const bool isFollowTask = taskType == QStringLiteral("FollowEntity");
  const bool isInterceptTask = isInterceptEntityTaskType(taskType);
  const bool isAttackOnceTask = taskType == QStringLiteral("AttackOnce");
  const bool isAttackAirTask = taskType == QStringLiteral("AttackAir");
  const bool isAttackSurfaceTask = taskType == QStringLiteral("AttackSurface");
  const bool isWaitDetectedTask = taskType == QStringLiteral("WaitUntilTargetDetected");
  const bool isWaitDestroyedTask = taskType == QStringLiteral("WaitUntilTargetDestroyed");
  const bool isWaitDamagedTask = taskType == QStringLiteral("WaitUntilDamaged");
  const bool isWaitTimeTask = taskType == QStringLiteral("WaitUntilTime");
  const bool isWaitRangeTask = taskType == QStringLiteral("WaitUntilInRange");
  const bool isConditionalTargetTask =
      isWaitDetectedTask || isWaitDestroyedTask || isWaitDamagedTask || isWaitRangeTask;

  _headingSpin->setEnabled(isFlyTask || isRacetrackTask);
  _latitudeSpin->setEnabled(isMoveTask || isWaitTask || isRacetrackTask || isAttackSurfaceTask);
  _longitudeSpin->setEnabled(isMoveTask || isWaitTask || isRacetrackTask || isAttackSurfaceTask);
  _altitudeSpin->setEnabled(
      isFlyTask ||
      isMoveTask ||
      isWaitTask ||
      isWaypointTask ||
      isAreaTask ||
      isRacetrackTask ||
      isAttackSurfaceTask);
  _waypointCombo->setEnabled(isWaypointTask);
  _routeCombo->setEnabled(isRouteTask);
  _areaCombo->setEnabled(isAreaTask);
  _followTargetCombo->setEnabled(
      isFollowTask || isInterceptTask || isAttackOnceTask || isAttackAirTask || isAttackSurfaceTask ||
      isConditionalTargetTask);
  _targetDomainCombo->setEnabled(isWaitDetectedTask);
  _enemyOnlyCheck->setEnabled(isWaitDetectedTask);
  _weaponTypeCombo->setEnabled(isAttackOnceTask || isAttackAirTask || isAttackSurfaceTask);
  _followDistanceSpin->setEnabled(isFollowTask || isInterceptTask || isRacetrackTask || isWaitRangeTask);
  _arrivalToleranceSpin->setEnabled(isFollowTask || isRouteTask || isWaitTask);
  _altitudeToleranceSpin->setEnabled(isInterceptTask || isWaitDamagedTask);
  _durationSpin->setEnabled(isFollowTask || isInterceptTask || isRouteTask || isRacetrackTask || isWaitTask ||
                            isAttackOnceTask || isWaitDetectedTask || isWaitDestroyedTask ||
                            isWaitDamagedTask || isWaitTimeTask || isWaitRangeTask);
}

void AssignTaskDialog::setPickedCoordinate(double longitude, double latitude, double height) {
  _longitudeSpin->setValue(longitude);
  _latitudeSpin->setValue(latitude);
  _altitudeSpin->setValue(height);
}
