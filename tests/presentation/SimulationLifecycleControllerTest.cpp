#include <gtest/gtest.h>
#include "application/ScenarioState.h"
#include "presentation/SimulationLifecycleController.h"

using namespace presentation;

namespace {

struct Fixture {
  ScenarioState state;
  bool running      = false;
  bool timerStarted = false;
  bool timerStopped = false;
  bool jsRefreshed  = false;
  bool bombPickCancelled  = false;
  bool cooldownCleared    = false;
  bool pendingBombCleared = false;
  bool synced        = false;
  bool controlsUpdated = false;
  QString lastStatus;
  QString lastLog;

  SimulationLifecycleController* makeCtrl() {
    return new SimulationLifecycleController(
        &state,
        [this]()      { return running; },
        [this](bool v){ running = v; },
        [this]()      { timerStarted = true; },
        [this]()      { timerStopped = true; },
        [this]()      { jsRefreshed = true; },
        [this]()      { bombPickCancelled = true; },
        [this]()      { cooldownCleared = true; },
        [this]()      { pendingBombCleared = true; },
        [this]()      { synced = true; },
        [this](const QString& m) { lastStatus = m; },
        [this](const QString& m) { lastLog = m; },
        [this]()      { controlsUpdated = true; },
        nullptr);
  }
};

} // namespace

// ─── start ──────────────────────────────────────────────────────────────────

TEST(SimulationLifecycleController, start_whenStopped_startsTimerAndNotifies) {
  Fixture f;
  f.running = false;
  auto* ctrl = f.makeCtrl();
  ctrl->start();

  EXPECT_TRUE(f.running);
  EXPECT_TRUE(f.timerStarted);
  EXPECT_TRUE(f.jsRefreshed);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  EXPECT_FALSE(f.lastLog.isEmpty());
  EXPECT_TRUE(f.controlsUpdated);
  delete ctrl;
}

TEST(SimulationLifecycleController, start_whenAlreadyRunning_doesNothing) {
  Fixture f;
  f.running = true;
  auto* ctrl = f.makeCtrl();
  ctrl->start();

  EXPECT_FALSE(f.timerStarted);
  EXPECT_FALSE(f.controlsUpdated);
  delete ctrl;
}

// ─── pause ──────────────────────────────────────────────────────────────────

TEST(SimulationLifecycleController, pause_whenRunning_stopsTimerAndSyncs) {
  Fixture f;
  f.running = true;
  auto* ctrl = f.makeCtrl();
  ctrl->pause();

  EXPECT_FALSE(f.running);
  EXPECT_TRUE(f.timerStopped);
  EXPECT_TRUE(f.synced);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  EXPECT_FALSE(f.lastLog.isEmpty());
  EXPECT_TRUE(f.controlsUpdated);
  delete ctrl;
}

TEST(SimulationLifecycleController, pause_whenAlreadyStopped_doesNothing) {
  Fixture f;
  f.running = false;
  auto* ctrl = f.makeCtrl();
  ctrl->pause();

  EXPECT_FALSE(f.timerStopped);
  EXPECT_FALSE(f.controlsUpdated);
  delete ctrl;
}

// ─── stop ───────────────────────────────────────────────────────────────────

TEST(SimulationLifecycleController, stop_clearsAllStateAndNotifies) {
  Fixture f;
  f.running = true;
  auto* ctrl = f.makeCtrl();
  ctrl->stop();

  EXPECT_FALSE(f.running);
  EXPECT_TRUE(f.timerStopped);
  EXPECT_TRUE(f.bombPickCancelled);
  EXPECT_TRUE(f.cooldownCleared);
  EXPECT_TRUE(f.pendingBombCleared);
  EXPECT_TRUE(f.synced);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  EXPECT_FALSE(f.lastLog.isEmpty());
  EXPECT_TRUE(f.controlsUpdated);
  delete ctrl;
}

TEST(SimulationLifecycleController, stop_whenAlreadyStopped_stillClearsAndNotifies) {
  Fixture f;
  f.running = false;
  auto* ctrl = f.makeCtrl();
  ctrl->stop();

  // stop() has no early-return guard — always runs full cleanup
  EXPECT_TRUE(f.bombPickCancelled);
  EXPECT_TRUE(f.cooldownCleared);
  EXPECT_TRUE(f.pendingBombCleared);
  EXPECT_TRUE(f.controlsUpdated);
  delete ctrl;
}

TEST(SimulationLifecycleController, stop_doesNotCallJsRefresh) {
  Fixture f;
  f.running = true;
  auto* ctrl = f.makeCtrl();
  ctrl->stop();

  EXPECT_FALSE(f.jsRefreshed); // JS refresh only on start
  delete ctrl;
}

// ─── start then pause ───────────────────────────────────────────────────────

TEST(SimulationLifecycleController, startThenPause_togglesRunningFlag) {
  Fixture f;
  auto* ctrl = f.makeCtrl();

  ctrl->start();
  EXPECT_TRUE(f.running);

  ctrl->pause();
  EXPECT_FALSE(f.running);
  delete ctrl;
}
