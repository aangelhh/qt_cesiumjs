#include <gtest/gtest.h>
#include "application/AttackTaskProcessor.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "presentation/BombReleaseController.h"

namespace {

class AttackTaskProcessorTest : public ::testing::Test {
protected:
  void SetUp() override {
    state = new ScenarioState();
    bombCtrl = new presentation::BombReleaseController(
        state,
        [](const QString&) {},
        [](const QString&) {},
        []() {},
        [](const QString&, bool) {},
        [](const QString&) {},
        nullptr);

    processor = std::make_unique<application::AttackTaskProcessor>(
        state,
        bombCtrl,
        [this](const QString& msg) { logMessages.push_back(msg); },
        [this](const QString& msg) { statusMessages.push_back(msg); },
        [this](const QString& launcher, double lat, double lon, double alt,
               const QString& label, const QString& source,
               const QString& targetEntity, bool log, bool focus) {
          Q_UNUSED(lat); Q_UNUSED(lon); Q_UNUSED(alt); Q_UNUSED(label);
          Q_UNUSED(source); Q_UNUSED(targetEntity); Q_UNUSED(log); Q_UNUSED(focus);
          bombsQueued.push_back(launcher);
        },
        nullptr);
  }

  void TearDown() override {
    processor.reset();
    delete bombCtrl;
    delete state;
  }

  Entity makeAirEntity(const QString& name, int force) {
    Entity e;
    e.name = name;
    e.forceIdentifier = force;
    e.domain = QStringLiteral("Air");
    e.latitude = 40.0;
    e.longitude = -3.0;
    e.altitude = 5000;
    e.speedKnots = 300;
    e.destroyed = false;
    e.damagePercent = 0;
    return e;
  }

  // addEntity resets currentTask, so we must set task after adding
  void setTask(const QString& entityName, const EntityTask& task) {
    for (Entity& e : state->entitiesMutable()) {
      if (e.name == entityName) {
        e.currentTask = task;
        return;
      }
    }
  }

  void addWeapon(const QString& entityName, const QString& type, int qty) {
    for (Entity& e : state->entitiesMutable()) {
      if (e.name == entityName) {
        WeaponInventoryItem w;
        w.weaponType = type;
        w.quantity = qty;
        e.weapons.push_back(w);
        return;
      }
    }
  }

  void addDetectedContact(const QString& launcherName, const QString& targetName, double rangeMeters = 10000.0) {
    for (Entity& e : state->entitiesMutable()) {
      if (e.name == launcherName) {
        SensorContact contact;
        contact.targetEntityName = targetName;
        contact.rangeMeters = rangeMeters;
        contact.detected = true;
        e.sensorContacts.push_back(contact);
        return;
      }
    }
  }

  ScenarioState* state = nullptr;
  presentation::BombReleaseController* bombCtrl = nullptr;
  std::unique_ptr<application::AttackTaskProcessor> processor;
  QStringList logMessages;
  QStringList statusMessages;
  QStringList bombsQueued;
};

TEST_F(AttackTaskProcessorTest, NoOpWhenSimulationNotRunning) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  state->addEntity(makeAirEntity(QStringLiteral("Mig29"), 2));

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackAir");
  task.targetEntityName = QStringLiteral("Mig29");
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, false);
  EXPECT_TRUE(logMessages.isEmpty());
}

TEST_F(AttackTaskProcessorTest, AttackAirFailsWhenTargetEmpty) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackAir");
  task.targetEntityName = QString();
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Failed"));
}

TEST_F(AttackTaskProcessorTest, AttackAirCompletesWhenTargetDestroyed) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Missile"), 2);

  Entity target = makeAirEntity(QStringLiteral("Mig29"), 2);
  target.destroyed = true;
  state->addEntity(target);

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackAir");
  task.targetEntityName = QStringLiteral("Mig29");
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Completed"));
  EXPECT_FALSE(logMessages.isEmpty());
  EXPECT_TRUE(logMessages.first().contains(QStringLiteral("completed")));
}

TEST_F(AttackTaskProcessorTest, AttackAirFailsAgainstFriendly) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Missile"), 2);
  state->addEntity(makeAirEntity(QStringLiteral("Wingman"), 1));

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackAir");
  task.targetEntityName = QStringLiteral("Wingman");
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Failed"));
}

TEST_F(AttackTaskProcessorTest, AttackAirTimesOut) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Missile"), 2);
  state->addEntity(makeAirEntity(QStringLiteral("Mig29"), 2));

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackAir");
  task.targetEntityName = QStringLiteral("Mig29");
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  // Simulate 121 seconds (timeout at 120s): 121/0.033 ≈ 3667 ticks
  for (int i = 0; i < 3667; ++i) {
    processor->processAttackTasks(0.033, true);
  }

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Failed"));
  EXPECT_TRUE(logMessages.last().contains(QStringLiteral("timeout")));
}

TEST_F(AttackTaskProcessorTest, AttackOnceWithMissileCompletesAfterSingleLaunch) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Missile"), 2);
  state->addEntity(makeAirEntity(QStringLiteral("Mig29"), 2));
  addDetectedContact(QStringLiteral("F16"), QStringLiteral("Mig29"));

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackOnce");
  task.targetEntityName = QStringLiteral("Mig29");
  task.weaponType = QStringLiteral("Missile");
  task.timeoutSeconds = 120.0;
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Completed"));
  ASSERT_FALSE(updated.weapons.isEmpty());
  EXPECT_EQ(updated.weapons.first().quantity, 1);
  EXPECT_EQ(state->activeMunitions().size(), 1);
  EXPECT_EQ(state->activeMunitions().first().targetEntityName, QStringLiteral("Mig29"));
}

TEST_F(AttackTaskProcessorTest, AttackOnceWithBombCompletesAfterQueueingRelease) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Bomb"), 2);

  Entity target;
  target.name = QStringLiteral("Tank1");
  target.forceIdentifier = 2;
  target.domain = QStringLiteral("Land");
  target.latitude = 40.1;
  target.longitude = -3.1;
  target.altitude = 500;
  target.destroyed = false;
  state->addEntity(target);

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackOnce");
  task.targetEntityName = QStringLiteral("Tank1");
  task.weaponType = QStringLiteral("Bomb");
  task.timeoutSeconds = 120.0;
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  EXPECT_EQ(bombsQueued.size(), 1);
  EXPECT_EQ(bombsQueued.first(), QStringLiteral("F16"));
  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Completed"));
}

TEST_F(AttackTaskProcessorTest, AttackOnceFailsWithNoCompatibleAmmo) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  state->addEntity(makeAirEntity(QStringLiteral("Mig29"), 2));
  addDetectedContact(QStringLiteral("F16"), QStringLiteral("Mig29"));

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackOnce");
  task.targetEntityName = QStringLiteral("Mig29");
  task.weaponType = QStringLiteral("Missile");
  task.timeoutSeconds = 120.0;
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Failed"));
  EXPECT_TRUE(state->activeMunitions().isEmpty());
}

TEST_F(AttackTaskProcessorTest, AttackOnceFailsForInvalidTarget) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Missile"), 2);

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackOnce");
  task.targetEntityName = QStringLiteral("MissingTarget");
  task.weaponType = QStringLiteral("Missile");
  task.timeoutSeconds = 120.0;
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Failed"));
}

TEST_F(AttackTaskProcessorTest, AttackSurfaceFailsWithNoBombs) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));

  Entity target;
  target.name = QStringLiteral("Tank1");
  target.forceIdentifier = 2;
  target.domain = QStringLiteral("Land");
  target.latitude = 40.1;
  target.longitude = -3.1;
  target.altitude = 500;
  target.destroyed = false;
  state->addEntity(target);

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackSurface");
  task.targetEntityName = QStringLiteral("Tank1");
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Failed"));
}

TEST_F(AttackTaskProcessorTest, AttackSurfaceQueuesBomb) {
  state->addEntity(makeAirEntity(QStringLiteral("F16"), 1));
  addWeapon(QStringLiteral("F16"), QStringLiteral("Bomb"), 4);

  Entity target;
  target.name = QStringLiteral("Tank1");
  target.forceIdentifier = 2;
  target.domain = QStringLiteral("Land");
  target.latitude = 40.1;
  target.longitude = -3.1;
  target.altitude = 500;
  target.destroyed = false;
  state->addEntity(target);

  EntityTask task;
  task.enabled = true;
  task.taskType = QStringLiteral("AttackSurface");
  task.targetEntityName = QStringLiteral("Tank1");
  task.status = QStringLiteral("Running");
  setTask(QStringLiteral("F16"), task);

  processor->processAttackTasks(0.033, true);

  EXPECT_EQ(bombsQueued.size(), 1);
  EXPECT_EQ(bombsQueued.first(), QStringLiteral("F16"));
  const Entity& updated = state->entities().first();
  EXPECT_EQ(updated.currentTask.status, QStringLiteral("Completed"));
}

TEST_F(AttackTaskProcessorTest, RemoveEntityClearsState) {
  processor->attackAirElapsedSeconds()[QStringLiteral("F16")] = 50.0;
  processor->attackAirMissileCooldownSeconds()[QStringLiteral("F16")] = 3.0;
  processor->autoBombReleaseCooldownSeconds()[QStringLiteral("F16")] = 10.0;
  processor->autoBehaviorDamageReactionLevel()[QStringLiteral("F16")] = 1;

  processor->removeEntity(QStringLiteral("F16"));

  EXPECT_FALSE(processor->attackAirElapsedSeconds().contains(QStringLiteral("F16")));
  EXPECT_FALSE(processor->attackAirMissileCooldownSeconds().contains(QStringLiteral("F16")));
  EXPECT_FALSE(processor->autoBombReleaseCooldownSeconds().contains(QStringLiteral("F16")));
  EXPECT_FALSE(processor->autoBehaviorDamageReactionLevel().contains(QStringLiteral("F16")));
}

TEST_F(AttackTaskProcessorTest, AutoBombingSkipsNonAggressiveEntities) {
  Entity e = makeAirEntity(QStringLiteral("F16"), 1);
  e.behaviorMode = QStringLiteral("Defensive");
  state->addEntity(e);
  addWeapon(QStringLiteral("F16"), QStringLiteral("Bomb"), 4);

  processor->processAutoBombing(0.033, true);

  EXPECT_TRUE(bombsQueued.isEmpty());
}

} // namespace
