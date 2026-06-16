#pragma once

#include "domain/Task.h"

#include <QDialog>
#include <QStringList>

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;

class AssignTaskDialog : public QDialog {
  Q_OBJECT

public:
  explicit AssignTaskDialog(
      const QString& entityName,
      const QStringList& availableTargets,
      const QStringList& availableWaypoints,
      const QStringList& availableRoutes,
      const QStringList& availableAreas,
      const EntityTask& currentTask,
      const QString& initialTaskType,
      QWidget* parent = nullptr);

  EntityTask task() const;
  void setPickedCoordinate(double longitude, double latitude, double height);

signals:
  void pickOnMapRequested();

private:
  void syncUiForTaskType();

  QComboBox* _taskTypeCombo;
  QDoubleSpinBox* _headingSpin;
  QDoubleSpinBox* _altitudeSpin;
  QDoubleSpinBox* _speedSpin;
  QDoubleSpinBox* _latitudeSpin;
  QDoubleSpinBox* _longitudeSpin;
  QDoubleSpinBox* _followDistanceSpin;
  QDoubleSpinBox* _arrivalToleranceSpin;
  QDoubleSpinBox* _altitudeToleranceSpin;
  QDoubleSpinBox* _durationSpin;
  QComboBox* _followTargetCombo;
  QComboBox* _waypointCombo;
  QComboBox* _routeCombo;
  QComboBox* _areaCombo;
  QComboBox* _weaponTypeCombo;
  QComboBox* _targetDomainCombo;
  QCheckBox* _enemyOnlyCheck;
};
