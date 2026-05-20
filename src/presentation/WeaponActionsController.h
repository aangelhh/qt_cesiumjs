#pragma once

#include <QObject>
#include <QString>
#include <functional>

#include "domain/Entity.h"

class ScenarioState;

namespace presentation {

/// Handles weapon action commands (add/launch missiles, add/release bombs)
/// for the currently selected entity.
///
/// All side-effects (logging, status bar, UI sync) are injected as callbacks
/// so this class is decoupled from any Qt widget type.
class WeaponActionsController : public QObject {
  Q_OBJECT
public:
  using LogFn           = std::function<void(const QString&)>;
  using StatusFn        = std::function<void(const QString&)>;
  using SyncUiFn        = std::function<void()>;
  using SelectedNameFn  = std::function<QString()>;
  using FindEntityFn    = std::function<const Entity*(const QString&)>;
  using IsRunningFn     = std::function<bool()>;
  /// Prompts user to pick one item from a list. Returns empty string if cancelled.
  using PickItemFn      = std::function<QString(const QStringList&)>;

  explicit WeaponActionsController(
      ScenarioState*  state,
      SelectedNameFn  selectedName,
      FindEntityFn    findEntity,
      IsRunningFn     isRunning,
      LogFn           log,
      StatusFn        setStatus,
      SyncUiFn        syncUi,
      PickItemFn      pickTarget,
      QObject*        parent = nullptr);

  void addMissileToSelected();
  void addBombToSelected();
  void launchMissileFromSelected();
  void releaseBombFromSelected();
  void launchMissileAtSelected();

private:
  void executeLaunchAt(
      const QString& launcherName,
      const QString& targetName,
      int            previousMissileCount);

  ScenarioState* _state;
  SelectedNameFn _selectedName;
  FindEntityFn   _findEntity;
  IsRunningFn    _isRunning;
  LogFn          _log;
  StatusFn       _setStatus;
  SyncUiFn       _syncUi;
  PickItemFn     _pickTarget;
};

} // namespace presentation
