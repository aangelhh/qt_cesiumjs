#pragma once

#include <QObject>
#include <QString>
#include <functional>

class ScenarioState;

namespace presentation {

/// Controls simulation lifecycle: start, pause, stop.
/// All side-effects are injected as callbacks so this class is Qt-widget-free
/// and fully unit-testable without a QApplication.
class SimulationLifecycleController : public QObject {
  Q_OBJECT
public:
  using BoolGetterFn   = std::function<bool()>;
  using BoolSetterFn   = std::function<void(bool)>;
  using VoidFn         = std::function<void()>;
  using StringFn       = std::function<void(const QString&)>;

  explicit SimulationLifecycleController(
      ScenarioState* state,
      BoolGetterFn   isRunning,
      BoolSetterFn   setRunning,
      VoidFn         startTimer,
      VoidFn         stopTimer,
      VoidFn         notifyJsRefresh,       // JS call on start (may be no-op)
      VoidFn         cancelBombPickMode,
      VoidFn         clearAutoBombCooldown,
      VoidFn         clearPendingBomb,
      VoidFn         syncUi,
      StringFn       setStatus,
      StringFn       log,
      VoidFn         updateControls,
      QObject*       parent = nullptr);

  void start();
  void pause();
  void stop();

private:
  ScenarioState* _state;
  BoolGetterFn   _isRunning;
  BoolSetterFn   _setRunning;
  VoidFn         _startTimer;
  VoidFn         _stopTimer;
  VoidFn         _notifyJsRefresh;
  VoidFn         _cancelBombPickMode;
  VoidFn         _clearAutoBombCooldown;
  VoidFn         _clearPendingBomb;
  VoidFn         _syncUi;
  StringFn       _setStatus;
  StringFn       _log;
  VoidFn         _updateControls;
};

} // namespace presentation
