#pragma once

#include "presentation/PlanTypes.h"
#include <QObject>
#include <QString>
#include <functional>

class ScenarioState;

namespace presentation {

/// Owns the pending-bomb-release state and the interactive target-pick state.
/// Side effects (logging, status, UI sync, track removal) are injected
/// as callbacks to keep this class decoupled from Qt UI types.
class BombReleaseController : public QObject {
  Q_OBJECT
public:
  using LogFn          = std::function<void(const QString&)>;
  using StatusFn       = std::function<void(const QString&)>;
  using SyncUiFn       = std::function<void()>;
  using SelectObjectFn = std::function<void(const QString&, bool)>;
  using RemoveTrackFn  = std::function<void(const QString&)>;

  explicit BombReleaseController(
      ScenarioState*  state,
      LogFn           log,
      StatusFn        setStatus,
      SyncUiFn        syncUi,
      SelectObjectFn  selectObject,
      RemoveTrackFn   removeTrack,
      QObject*        parent = nullptr);

  /// Queue a pending bomb release for launcherEntityName.
  void queue(
      const QString& launcherEntityName,
      double         targetLat,
      double         targetLon,
      double         targetAltMeters,
      const QString& targetLabel,
      const QString& sourceDesc,
      const QString& targetEntityName = {},
      bool           logQueued        = true,
      bool           focusLauncher    = false);

  /// Clear the pending release unconditionally.
  void clear();

  /// Validate that the launcher still exists; clear if not.
  void validate();

  /// Process one simulation tick — check the release gate and fire if ready.
  void process();

  // Interactive map-pick mode (user clicks a point on the map as bomb target).
  void    beginPickMode(const QString& launcherName);
  void    cancelPickMode();
  bool    isPickingMode() const;
  QString pickingLauncherName() const;

  /// Must be called when an entity is removed from the scenario.
  /// Returns a non-empty status message if the pending release was cancelled.
  QString handleRemovedEntity(const QString& entityName);

  bool                      isPending() const;
  const PendingBombRelease& pendingRelease() const;

private:
  ScenarioState*     _state;
  LogFn              _log;
  StatusFn           _setStatus;
  SyncUiFn           _syncUi;
  SelectObjectFn     _selectObject;
  RemoveTrackFn      _removeTrack;
  PendingBombRelease _pendingBombRelease;
  bool               _isPickingMode       = false;
  QString            _pickingLauncherName;
};

} // namespace presentation
