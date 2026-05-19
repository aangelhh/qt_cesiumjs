#include <gtest/gtest.h>
#include "application/TaskApplicator.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "domain/Task.h"

namespace {

class TaskApplicatorTest : public ::testing::Test {
protected:
  void SetUp() override {
    state = new ScenarioState();
  }
  void TearDown() override {
    delete state;
  }

  Entity makeAirEntity(const QString& name) {
    Entity e;
    e.name = name;
    e.category = QStringLiteral("Air");
    e.latitude = 40.0;
    e.longitude = -3.0;
    e.altitude = 3000;
    e.speedKnots = 300.0;
    return e;
  }

  ScenarioState* state{nullptr};
  QStringList logMessages;
};

// Returns false for missing entity
TEST_F(TaskApplicatorTest, ReturnsFalseForUnknownEntity) {
  EntityTask task;
  task.taskType = "MoveToLocation";
  const bool result = application::applyEntityTask(
      "ghost", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});
  EXPECT_FALSE(result);
}

// Returns false for empty name
TEST_F(TaskApplicatorTest, ReturnsFalseForEmptyName) {
  EntityTask task;
  task.taskType = "MoveToLocation";
  const bool result = application::applyEntityTask(
      "", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});
  EXPECT_FALSE(result);
}

// Assigns MoveToLocation task and logs it
TEST_F(TaskApplicatorTest, AssignsMoveToLocationTask) {
  state->addEntity(makeAirEntity("Alpha"));
  EntityTask task;
  task.taskType = "MoveToLocation";
  task.targetLatitude = 41.0;
  task.targetLongitude = -4.0;
  task.targetAltitudeMeters = 5000.0;
  task.targetSpeedKnots = 350.0;
  state->entitiesMutable()[0].currentTask = task;

  const bool result = application::applyEntityTask(
      "Alpha", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});
  EXPECT_TRUE(result);
  ASSERT_EQ(logMessages.size(), 1);
  EXPECT_TRUE(logMessages[0].contains("MoveToLocation"));
  EXPECT_TRUE(logMessages[0].contains("Alpha"));

  // TaskStack should have 1 item
  const domain::TaskStack* stack = state->getTaskStack("Alpha");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
}

// Assigns FlyHeadingAltitudeSpeed task
TEST_F(TaskApplicatorTest, AssignsFlyHeadingTask) {
  state->addEntity(makeAirEntity("Beta"));
  EntityTask task;
  task.taskType = "FlyHeadingAltitudeSpeed";
  task.targetHeadingDegrees = 90.0;
  task.targetAltitudeMeters = 4000.0;
  task.targetSpeedKnots = 400.0;
  state->entitiesMutable()[0].currentTask = task;

  const bool result = application::applyEntityTask(
      "Beta", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});
  EXPECT_TRUE(result);

  const domain::TaskStack* stack = state->getTaskStack("Beta");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
}

// Assigns FollowEntity task
TEST_F(TaskApplicatorTest, AssignsFollowEntityTask) {
  state->addEntity(makeAirEntity("Gamma"));
  EntityTask task;
  task.taskType = "FollowEntity";
  task.targetEntityName = "Target1";
  task.targetAltitudeMeters = 3000.0;
  task.targetSpeedKnots = 350.0;
  state->entitiesMutable()[0].currentTask = task;

  const bool result = application::applyEntityTask(
      "Gamma", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});
  EXPECT_TRUE(result);
}

// syncUi callback is called only when syncUi==true
TEST_F(TaskApplicatorTest, SyncUiCallbackCalledOnlyWhenRequested) {
  state->addEntity(makeAirEntity("Delta"));
  EntityTask task;
  task.taskType = "MoveToLocation";
  state->entitiesMutable()[0].currentTask = task;

  int syncCallCount = 0;
  application::applyEntityTask(
      "Delta", task, false, state, nullptr,
      [](const QString&) {},
      [&syncCallCount]() { ++syncCallCount; });
  EXPECT_EQ(syncCallCount, 0);

  application::applyEntityTask(
      "Delta", task, true, state, nullptr,
      [](const QString&) {},
      [&syncCallCount]() { ++syncCallCount; });
  EXPECT_EQ(syncCallCount, 1);
}

// MoveAlongRoute falls back to MoveToLocation when route not found
TEST_F(TaskApplicatorTest, MoveAlongRouteFallsBackToMoveToLocation) {
  state->addEntity(makeAirEntity("Epsilon"));
  EntityTask task;
  task.taskType = "MoveAlongRoute";
  task.targetRouteName = "NonExistentRoute";
  task.targetLatitude = 42.0;
  task.targetLongitude = -5.0;
  task.targetAltitudeMeters = 3000.0;
  task.targetSpeedKnots = 300.0;
  state->entitiesMutable()[0].currentTask = task;

  const bool result = application::applyEntityTask(
      "Epsilon", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});
  EXPECT_TRUE(result);

  // Stack should still have the fallback MoveToLocation task
  const domain::TaskStack* stack = state->getTaskStack("Epsilon");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
}

} // namespace
