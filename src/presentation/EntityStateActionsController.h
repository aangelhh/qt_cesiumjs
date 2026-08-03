#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <functional>

#include "domain/Entity.h"
#include "domain/Task.h"

class ScenarioState;

namespace presentation {

class EntityVisualStateManager;

/// Handles entity state-change commands for the currently selected entity:
/// destroy/restore, behavior mode, visual flags (hidden/radar/track-history),
/// and FlyHeadingAltitudeSpeed adjustments.
///
/// All side-effects (log, status, UI sync, task application) are injected
/// as callbacks — no dependency on any Qt widget type.
class EntityStateActionsController : public QObject {
  Q_OBJECT
public:
  using LogFn              = std::function<void(const QString&)>;
  using StatusFn           = std::function<void(const QString&)>;
  using SyncUiFn           = std::function<void()>;
  using SelectedNameFn     = std::function<QString()>;
  using IsDestroyedFn      = std::function<bool()>;
  using IsOperableFn       = std::function<bool()>;
  using ApplyTaskFn        = std::function<bool(const QString&, const EntityTask&, bool)>;
  /// Returns current fly targets (heading, altitude, speed) for the selected entity.
  using FlyTargetsFn       = std::function<bool(double&, int&, double&)>;
  /// Prompts user for a double value. Returns false (via ok) if cancelled.
  using AskDoubleFn        = std::function<double(
      const QString& title, const QString& label,
      double def, double min, double max, bool& ok)>;
  /// Called when a task dialog must be closed (entity destroyed).
  using CloseTaskDialogFn  = std::function<void()>;

  explicit EntityStateActionsController(
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
      QObject*                    parent = nullptr);

  void destroySelected();
  void restoreSelected();
  void setSelectedDestroyed(bool destroyed);
  void setSelectedBehaviorMode(const QString& mode);
  void setSelectedHidden(bool hidden);
  void setSelectedRadarCoverageVisible(bool visible);
  void setSelectedTrackHistoryVisible(bool visible);
  void setSelectedHeading();
  void setSelectedAltitude();
  void setSelectedSpeed();
  void setSelectedFuel();

private:
  void applyFlyTask(double headingDeg, int altMeters, double speedKnots);

  ScenarioState*            _state;
  EntityVisualStateManager* _visualState;
  SelectedNameFn            _selectedName;
  IsDestroyedFn             _isDestroyed;
  IsOperableFn              _isOperable;
  ApplyTaskFn               _applyTask;
  FlyTargetsFn              _flyTargets;
  AskDoubleFn               _askDouble;
  CloseTaskDialogFn         _closeTaskDialog;
  LogFn                     _log;
  StatusFn                  _setStatus;
  SyncUiFn                  _syncUi;
};

} // namespace presentation
