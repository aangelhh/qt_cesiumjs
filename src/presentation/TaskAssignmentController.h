#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <functional>

#include "domain/Task.h"
#include "presentation/EntityHomePositionTracker.h"

/// Handles all "assign task" commands for the currently selected entity:
/// ReturnToBase, PatrolRoute, OrbitHoldLocation, and the simple dialog-based
/// assignments (FlyHeadingAltitudeSpeed, MoveToWaypoint, etc.).
///
/// All side-effects and UI interactions are injected as callbacks so the class
/// has no dependency on QWidget or MainWindow.
namespace presentation {

class TaskAssignmentController : public QObject {
  Q_OBJECT
public:
  using StatusFn          = std::function<void(const QString&)>;
  using SelectedNameFn    = std::function<QString()>;
  using IsOperableFn      = std::function<bool()>;
  using SummaryFn         = std::function<QVariantMap()>;
  using HomePositionFn    = std::function<EntityHomePosition(const QString&)>;
  using AvailableRoutesFn = std::function<QStringList(bool requirePoints)>;
  using FlyTargetsFn      = std::function<bool(double&, int&, double&)>;
  using ApplyTaskFn       = std::function<bool(const QString&, const EntityTask&)>;
  using OpenDialogFn      = std::function<void(const QString& taskType)>;
  using PickItemFn        = std::function<QString(
      const QString& title, const QString& label,
      const QStringList& items, int current, bool& ok)>;
  using AskDoubleFn       = std::function<double(
      const QString& title, const QString& label,
      double def, double min, double max, int decimals, bool& ok)>;

  explicit TaskAssignmentController(
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
      QObject*          parent = nullptr);

  // Substantive quick-assign commands
  void assignReturnToBase();
  void assignPatrolRoute();
  void assignOrbitHoldLocation();
  void assignHoldRacetrack();

  // Dialog-based assignments (delegates to openDialog callback)
  void assignFlyHeadingAltitudeSpeed();
  void assignMoveToLocation();
  void assignWaitOnLocation();
  void assignMoveToWaypoint();
  void assignMoveAlongRoute();
  void assignPatrolArea();
  void assignOrbitArea();
  void assignFollowEntity();
  void assignInterceptEntity();
  void assignInterceptEntity2D();
  void assignInterceptEntity3D();
  void assignAttackOnce();
  void assignAttackUntilDestroyed();
  void assignAttackAir();
  void assignAttackSurface();
  void assignWaitUntilTargetDetected();
  void assignWaitUntilTargetDestroyed();
  void assignWaitUntilDamaged();
  void assignWaitUntilTime();
  void assignWaitUntilInRange();

private:
  bool resolveOrbitCenter(double& outLatitude, double& outLongitude);

  SelectedNameFn    _selectedName;
  IsOperableFn      _isOperable;
  SummaryFn         _summary;
  HomePositionFn    _homePosition;
  AvailableRoutesFn _availableRoutes;
  FlyTargetsFn      _flyTargets;
  ApplyTaskFn       _applyTask;
  OpenDialogFn      _openDialog;
  PickItemFn        _pickItem;
  AskDoubleFn       _askDouble;
  StatusFn          _setStatus;
};

} // namespace presentation
