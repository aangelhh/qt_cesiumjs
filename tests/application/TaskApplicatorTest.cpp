#include <gtest/gtest.h>
#include "application/TaskApplicator.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "domain/Task.h"
#include "domain/TacticalGraphic.h"

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

TEST_F(TaskApplicatorTest, AssignsInterceptEntity2DTask) {
  state->addEntity(makeAirEntity("Interceptor"));
  EntityTask task;
  task.taskType = "InterceptEntity2D";
  task.targetEntityName = "Target1";
  task.targetSpeedKnots = 420.0;
  task.interceptDistanceMeters = 500.0;
  task.timeoutSeconds = 120.0;

  const bool result = application::applyEntityTask(
      "Interceptor", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});

  EXPECT_TRUE(result);
  const domain::TaskStack* stack = state->getTaskStack("Interceptor");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
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

// ── resolveTaskCoordinates ────────────────────────────────────────────────────

namespace resolve_tests {

static Entity makeAirEntity(const QString& name) {
  Entity e;
  e.name = name;
  e.domain = QStringLiteral("Air");
  e.category = QStringLiteral("Fighter");
  e.flightDynamicsMode = QStringLiteral("kinematic");
  e.destroyed = false;
  return e;
}

static Entity makeGroundEntity(const QString& name) {
  Entity e;
  e.name = name;
  e.domain = QStringLiteral("Ground");
  e.category = QStringLiteral("Vehicle");
  e.destroyed = false;
  return e;
}

TEST(ResolveTaskCoordinates, MoveToWaypointHydratesCoords) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  EntityTask task;
  task.taskType = QStringLiteral("MoveToWaypoint");
  task.targetWaypointName = QStringLiteral("Alpha");
  task.enabled = true;
  entity.currentTask = task;

  Waypoint wp;
  wp.name = QStringLiteral("Alpha");
  wp.latitude = 48.0; wp.longitude = 2.0; wp.altitudeMeters = 5000.0;
  QVector<Waypoint> waypoints = {wp};

  application::resolveTaskCoordinates(entity, waypoints, {}, {});

  EXPECT_NEAR(entity.currentTask.targetLatitude, 48.0, 0.001);
  EXPECT_NEAR(entity.currentTask.targetLongitude, 2.0, 0.001);
  EXPECT_EQ(entity.currentTask.targetAltitudeMeters, 5000);
}

TEST(ResolveTaskCoordinates, MoveToWaypointMissingWaypointNoChange) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  EntityTask task;
  task.taskType = QStringLiteral("MoveToWaypoint");
  task.targetWaypointName = QStringLiteral("Nonexistent");
  task.targetLatitude = 99.0;
  entity.currentTask = task;

  application::resolveTaskCoordinates(entity, {}, {}, {});

  EXPECT_NEAR(entity.currentTask.targetLatitude, 99.0, 0.001);
}

TEST(ResolveTaskCoordinates, MoveAlongRouteTakesLastPoint) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  EntityTask task;
  task.taskType = QStringLiteral("MoveAlongRoute");
  task.targetRouteName = QStringLiteral("Route-1");
  task.enabled = true;
  entity.currentTask = task;

  RouteGraphic route;
  route.name = QStringLiteral("Route-1");
  RoutePoint p1; p1.latitude = 10.0; p1.longitude = 20.0; p1.altitudeMeters = 3000.0;
  RoutePoint p2; p2.latitude = 15.0; p2.longitude = 25.0; p2.altitudeMeters = 4000.0;
  route.points = {p1, p2};

  application::resolveTaskCoordinates(entity, {}, {route}, {});

  EXPECT_NEAR(entity.currentTask.targetLatitude, 15.0, 0.001);
  EXPECT_NEAR(entity.currentTask.targetLongitude, 25.0, 0.001);
  EXPECT_EQ(entity.currentTask.targetAltitudeMeters, 4000);
}

TEST(ResolveTaskCoordinates, PatrolAreaHydratesCenter) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  EntityTask task;
  task.taskType = QStringLiteral("PatrolArea");
  task.targetAreaName = QStringLiteral("Zone-A");
  task.enabled = true;
  entity.currentTask = task;

  AreaDefinition area;
  area.id = QStringLiteral("a1");
  area.name = QStringLiteral("Zone-A");
  area.centerLatitude = 35.0; area.centerLongitude = 50.0;
  area.centerAltitudeMeters = 6000.0;
  area.radiusMeters = 2000.0;

  application::resolveTaskCoordinates(entity, {}, {}, {area});

  EXPECT_NEAR(entity.currentTask.targetLatitude, 35.0, 0.001);
  EXPECT_NEAR(entity.currentTask.targetLongitude, 50.0, 0.001);
  EXPECT_NEAR(entity.currentTask.targetAreaRadiusMeters, 2000.0, 0.1);
}

TEST(ResolveTaskCoordinates, EnabledMovementTaskEnablesFlightDynamics) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  entity.flightDynamicsEnabled = false;
  EntityTask task;
  task.taskType = QStringLiteral("MoveToLocation");
  task.targetLatitude = 10.0; task.targetLongitude = 20.0;
  task.enabled = true;
  entity.currentTask = task;

  application::resolveTaskCoordinates(entity, {}, {}, {});

  EXPECT_TRUE(entity.flightDynamicsEnabled);
}

TEST(ResolveTaskCoordinates, DefaultSpeedSetForAirEntityWithNoSpeed) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  EntityTask task;
  task.taskType = QStringLiteral("MoveToLocation");
  task.targetSpeedKnots = 0.0;
  task.enabled = true;
  entity.currentTask = task;

  application::resolveTaskCoordinates(entity, {}, {}, {});

  EXPECT_NEAR(entity.currentTask.targetSpeedKnots, 220.0, 0.1);
}

TEST(ResolveTaskCoordinates, DefaultSpeedSetForGroundEntityWithNoSpeed) {
  Entity entity = makeGroundEntity(QStringLiteral("G1"));
  EntityTask task;
  task.taskType = QStringLiteral("MoveToLocation");
  task.targetSpeedKnots = 0.0;
  task.enabled = true;
  entity.currentTask = task;

  application::resolveTaskCoordinates(entity, {}, {}, {});

  EXPECT_NEAR(entity.currentTask.targetSpeedKnots, 12.0, 0.1);
}

TEST(ResolveTaskCoordinates, DisabledTaskDoesNotEnableFlightDynamics) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  entity.flightDynamicsEnabled = false;
  EntityTask task;
  task.taskType = QStringLiteral("MoveToLocation");
  task.enabled = false;
  entity.currentTask = task;

  application::resolveTaskCoordinates(entity, {}, {}, {});

  EXPECT_FALSE(entity.flightDynamicsEnabled);
}

} // namespace resolve_tests
