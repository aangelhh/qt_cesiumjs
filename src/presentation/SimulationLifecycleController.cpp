#include "presentation/SimulationLifecycleController.h"
#include "application/ScenarioState.h"

namespace presentation {

SimulationLifecycleController::SimulationLifecycleController(
    ScenarioState* state,
    BoolGetterFn   isRunning,
    BoolSetterFn   setRunning,
    VoidFn         startTimer,
    VoidFn         stopTimer,
    VoidFn         notifyJsRefresh,
    VoidFn         cancelBombPickMode,
    VoidFn         clearAutoBombCooldown,
    VoidFn         clearPendingBomb,
    VoidFn         syncUi,
    StringFn       setStatus,
    StringFn       log,
    VoidFn         updateControls,
    QObject*       parent)
    : QObject(parent)
    , _state(state)
    , _isRunning(std::move(isRunning))
    , _setRunning(std::move(setRunning))
    , _startTimer(std::move(startTimer))
    , _stopTimer(std::move(stopTimer))
    , _notifyJsRefresh(std::move(notifyJsRefresh))
    , _cancelBombPickMode(std::move(cancelBombPickMode))
    , _clearAutoBombCooldown(std::move(clearAutoBombCooldown))
    , _clearPendingBomb(std::move(clearPendingBomb))
    , _syncUi(std::move(syncUi))
    , _setStatus(std::move(setStatus))
    , _log(std::move(log))
    , _updateControls(std::move(updateControls)) {}

void SimulationLifecycleController::start() {
  if (_isRunning()) {
    return;
  }
  _setRunning(true);
  _startTimer();
  _notifyJsRefresh();
  _setStatus(QStringLiteral("Simulacion en marcha."));
  _log(QStringLiteral("Simulation started."));
  _updateControls();
}

void SimulationLifecycleController::pause() {
  if (!_isRunning()) {
    return;
  }
  _setRunning(false);
  _stopTimer();
  _syncUi();
  _setStatus(QStringLiteral("Simulacion en pausa."));
  _log(QStringLiteral("Simulation paused."));
  _updateControls();
}

void SimulationLifecycleController::stop() {
  _setRunning(false);
  _stopTimer();
  _cancelBombPickMode();
  _clearAutoBombCooldown();
  _clearPendingBomb();
  _state->stopMission();
  _syncUi();
  _setStatus(QStringLiteral("Mision detenida. Todas las tasks han terminado."));
  _log(QStringLiteral("Simulation stopped. Mission state cleared."));
  _updateControls();
}

} // namespace presentation
