#include "AssignTaskDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QtMath>
#include <QVBoxLayout>

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
      _durationSpin(new QDoubleSpinBox(this)),
      _followTargetCombo(new QComboBox(this)),
      _waypointCombo(new QComboBox(this)),
      _routeCombo(new QComboBox(this)),
      _areaCombo(new QComboBox(this)) {
  this->setWindowTitle(QStringLiteral("Assign Task"));

  auto* layout = new QVBoxLayout(this);
  auto* hintLabel = new QLabel(
      QStringLiteral("Assign a task to %1").arg(entityName),
      this);
  hintLabel->setWordWrap(true);
  layout->addWidget(hintLabel);

  _taskTypeCombo->addItem(QStringLiteral("Fly Heading / Altitude / Speed"), QStringLiteral("FlyHeadingAltitudeSpeed"));
  _taskTypeCombo->addItem(QStringLiteral("Move To Location"), QStringLiteral("MoveToLocation"));
  _taskTypeCombo->addItem(QStringLiteral("Move To Waypoint"), QStringLiteral("MoveToWaypoint"));
  _taskTypeCombo->addItem(QStringLiteral("Move Along Route"), QStringLiteral("MoveAlongRoute"));
  _taskTypeCombo->addItem(QStringLiteral("Patrol Area"), QStringLiteral("PatrolArea"));
  _taskTypeCombo->addItem(QStringLiteral("Orbit Area"), QStringLiteral("OrbitArea"));
  _taskTypeCombo->addItem(QStringLiteral("Follow Entity"), QStringLiteral("FollowEntity"));
  _taskTypeCombo->addItem(QStringLiteral("Intercept Entity 2D"), QStringLiteral("InterceptEntity2D"));
  _taskTypeCombo->addItem(QStringLiteral("Attack Air"), QStringLiteral("AttackAir"));
  _taskTypeCombo->addItem(QStringLiteral("Attack Surface"), QStringLiteral("AttackSurface"));

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
  formLayout->addRow(QStringLiteral("Follow / Intercept Distance"), _followDistanceSpin);
  formLayout->addRow(QStringLiteral("Arrival Tolerance"), _arrivalToleranceSpin);
  formLayout->addRow(QStringLiteral("Duration / Timeout"), _durationSpin);
  layout->addLayout(formLayout);

  const QString initialType = initialTaskType.isEmpty() ? currentTask.taskType : initialTaskType;
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
      currentTask.interceptDistanceMeters > 0.0 && initialType == QStringLiteral("InterceptEntity2D")
      ? currentTask.interceptDistanceMeters
      : (currentTask.followDistanceMeters > 0.0 ? currentTask.followDistanceMeters : 1000.0));
  _arrivalToleranceSpin->setValue(currentTask.arrivalToleranceMeters);
  _durationSpin->setValue(
      initialType == QStringLiteral("InterceptEntity2D")
      ? (currentTask.timeoutSeconds > 0.0 ? currentTask.timeoutSeconds : 120.0)
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
  task.timeoutSeconds = _durationSpin->value();
  task.elapsedSeconds = 0.0;
  return task;
}

void AssignTaskDialog::syncUiForTaskType() {
  const QString taskType = _taskTypeCombo->currentData().toString();
  const bool isFlyTask = taskType == QStringLiteral("FlyHeadingAltitudeSpeed");
  const bool isMoveTask = taskType == QStringLiteral("MoveToLocation");
  const bool isWaypointTask = taskType == QStringLiteral("MoveToWaypoint");
  const bool isRouteTask = taskType == QStringLiteral("MoveAlongRoute");
  const bool isAreaTask =
      taskType == QStringLiteral("PatrolArea") ||
      taskType == QStringLiteral("OrbitArea");
  const bool isFollowTask = taskType == QStringLiteral("FollowEntity");
  const bool isInterceptTask = taskType == QStringLiteral("InterceptEntity2D");
  const bool isAttackAirTask = taskType == QStringLiteral("AttackAir");
  const bool isAttackSurfaceTask = taskType == QStringLiteral("AttackSurface");

  _headingSpin->setEnabled(isFlyTask);
  _latitudeSpin->setEnabled(isMoveTask || isAttackSurfaceTask);
  _longitudeSpin->setEnabled(isMoveTask || isAttackSurfaceTask);
  _altitudeSpin->setEnabled(
      isFlyTask ||
      isMoveTask ||
      isWaypointTask ||
      isAreaTask ||
      isAttackSurfaceTask);
  _waypointCombo->setEnabled(isWaypointTask);
  _routeCombo->setEnabled(isRouteTask);
  _areaCombo->setEnabled(isAreaTask);
  _followTargetCombo->setEnabled(
      isFollowTask || isInterceptTask || isAttackAirTask || isAttackSurfaceTask);
  _followDistanceSpin->setEnabled(isFollowTask || isInterceptTask);
  _arrivalToleranceSpin->setEnabled(isFollowTask);
  _durationSpin->setEnabled(isFollowTask || isInterceptTask);
}

void AssignTaskDialog::setPickedCoordinate(double longitude, double latitude, double height) {
  _longitudeSpin->setValue(longitude);
  _latitudeSpin->setValue(latitude);
  _altitudeSpin->setValue(height);
}
