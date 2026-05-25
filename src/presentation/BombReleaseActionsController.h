#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <functional>

#include "domain/Entity.h"

class ScenarioState;

namespace presentation {

class BombReleaseController;

/// Handles the three user-initiated bomb-release commands:
///   - releaseBombAtSurfaceEntity  (target picker dialog)
///   - releaseBombAtCustomCoordinates (map coordinate pick)
///   - cancelPendingBombRelease    (undo pending release)
///
/// Delegates the actual queuing/processing/pick-mode state to the
/// injected BombReleaseController. All UI interactions (QInputDialog,
/// status bar, log, syncUi) are injected as callbacks.
class BombReleaseActionsController : public QObject {
  Q_OBJECT
public:
  using SelectedNameFn     = std::function<QString()>;
  using FindEntityFn       = std::function<const Entity*(const QString&)>;
  using IsRunningFn        = std::function<bool()>;
  using IsPickPendingFn    = std::function<bool()>;
  using BeginCoordPickFn   = std::function<void()>;
  using StatusFn           = std::function<void(const QString&)>;
  using LogFn              = std::function<void(const QString&)>;
  using SyncUiFn           = std::function<void()>;
  using ValidTargetsFn     = std::function<QVector<const Entity*>(const Entity&)>;
  using TargetLabelFn      = std::function<QString(const Entity&)>;
  using PickItemFn         = std::function<QString(
      const QString& title, const QString& label,
      const QStringList& items, bool& ok)>;

  explicit BombReleaseActionsController(
      BombReleaseController* bombController,
      SelectedNameFn         selectedName,
      FindEntityFn           findEntity,
      IsRunningFn            isRunning,
      IsPickPendingFn        isPickPending,
      BeginCoordPickFn       beginCoordPick,
      ValidTargetsFn         validTargets,
      TargetLabelFn          targetLabel,
      PickItemFn             pickItem,
      StatusFn               setStatus,
      LogFn                  log,
      SyncUiFn               syncUi,
      QObject*               parent = nullptr);

  void releaseBombAtSurfaceEntity();
  void releaseBombAtCustomCoordinates();
  void cancelPendingBombRelease();

private:
  void queueBombReleaseAtEntity(const QString& launcherName, const Entity& target);

  BombReleaseController* _bombController;
  SelectedNameFn         _selectedName;
  FindEntityFn           _findEntity;
  IsRunningFn            _isRunning;
  IsPickPendingFn        _isPickPending;
  BeginCoordPickFn       _beginCoordPick;
  ValidTargetsFn         _validTargets;
  TargetLabelFn          _targetLabel;
  PickItemFn             _pickItem;
  StatusFn               _setStatus;
  LogFn                  _log;
  SyncUiFn               _syncUi;
};

} // namespace presentation
