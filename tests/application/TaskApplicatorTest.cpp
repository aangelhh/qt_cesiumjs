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

  Entity makeGroundEntity(const QString& name) {
    Entity e;
    e.name = name;
    e.domain = QStringLiteral("Ground");
    e.category = QStringLiteral("Ground");
    e.latitude = 40.02;
    e.longitude = -3.0;
    e.altitude = 0;
    e.speedKnots = 0.0;
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

TEST_F(TaskApplicatorTest, AssignsInterceptEntityTaskWith3DStack) {
  state->addEntity(makeAirEntity("Interceptor"));
  EntityTask task;
  task.taskType = "InterceptEntity";
  task.targetEntityName = "Target1";
  task.targetSpeedKnots = 420.0;
  task.interceptDistanceMeters = 500.0;
  task.altitudeToleranceMeters = 100.0;
  task.timeoutSeconds = 120.0;

  const bool result = application::applyEntityTask(
      "Interceptor", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});

  EXPECT_TRUE(result);
  const domain::TaskStack* stack = state->getTaskStack("Interceptor");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
  EXPECT_NE(dynamic_cast<domain::InterceptEntity3DTask*>(stack->top()), nullptr);

  ASSERT_FALSE(state->entities().isEmpty());
  EXPECT_EQ(state->entities().front().currentTask.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_DOUBLE_EQ(state->entities().front().currentTask.altitudeToleranceMeters, 100.0);
}

TEST_F(TaskApplicatorTest, LegacyInterceptEntity2DTypeNormalizesToUnifiedTask) {
  state->addEntity(makeAirEntity("Interceptor2D"));

  EntityTask task;
  task.taskType = "InterceptEntity2D";
  task.targetEntityName = "Target1";
  task.targetSpeedKnots = 420.0;
  task.interceptDistanceMeters = 500.0;
  task.timeoutSeconds = 120.0;

  EXPECT_TRUE(application::applyEntityTask(
      "Interceptor2D", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {}));
  ASSERT_FALSE(state->entities().isEmpty());
  EXPECT_EQ(state->entities().front().currentTask.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_DOUBLE_EQ(state->entities().front().currentTask.altitudeToleranceMeters, 100.0);
  const domain::TaskStack* stack = state->getTaskStack("Interceptor2D");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
  EXPECT_NE(dynamic_cast<domain::InterceptEntity3DTask*>(stack->top()), nullptr);
}

TEST_F(TaskApplicatorTest, LegacyInterceptEntity3DTypeNormalizesToUnified3DTask) {
  state->addEntity(makeAirEntity("Interceptor3D"));

  EntityTask legacy3d;
  legacy3d.taskType = "InterceptEntity3D";
  legacy3d.targetEntityName = "Target1";
  legacy3d.targetSpeedKnots = 420.0;
  legacy3d.interceptDistanceMeters = 500.0;
  legacy3d.altitudeToleranceMeters = 300.0;
  legacy3d.timeoutSeconds = 120.0;

  EXPECT_TRUE(application::applyEntityTask(
      "Interceptor3D", legacy3d, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {}));

  EXPECT_EQ(state->entities().front().currentTask.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_DOUBLE_EQ(state->entities().front().currentTask.altitudeToleranceMeters, 300.0);
  const domain::TaskStack* stack = state->getTaskStack("Interceptor3D");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
  EXPECT_NE(dynamic_cast<domain::InterceptEntity3DTask*>(stack->top()), nullptr);
}

TEST_F(TaskApplicatorTest, InterceptEntityAgainstGroundTargetMaintainsOwnAltitude) {
  state->addEntity(makeAirEntity("Interceptor"));
  state->addEntity(makeGroundEntity("GroundTarget"));

  EntityTask task;
  task.taskType = "InterceptEntity";
  task.enabled = true;
  task.targetEntityName = "GroundTarget";
  task.targetSpeedKnots = 420.0;
  task.interceptDistanceMeters = 500.0;
  task.altitudeToleranceMeters = 100.0;
  task.timeoutSeconds = 120.0;

  ASSERT_TRUE(application::applyEntityTask(
      "Interceptor", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {}));

  state->advanceSimulation(0.1);

  ASSERT_GE(state->entities().size(), 2);
  EXPECT_EQ(state->entities().front().currentTask.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_EQ(state->entities().front().currentTask.targetAltitudeMeters, 3000);
}

TEST_F(TaskApplicatorTest, AssignsHoldRacetrackTask) {
  state->addEntity(makeAirEntity("Holder"));

  EntityTask task;
  task.taskType = "HoldRacetrack";
  task.targetLatitude = 40.0;
  task.targetLongitude = -3.0;
  task.targetHeadingDegrees = 90.0;
  task.targetAltitudeMeters = 6000;
  task.targetSpeedKnots = 240.0;
  task.racetrackLegLengthMeters = 12000.0;

  const bool result = application::applyEntityTask(
      "Holder", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});

  EXPECT_TRUE(result);
  const domain::TaskStack* stack = state->getTaskStack("Holder");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
  EXPECT_NE(dynamic_cast<domain::HoldRacetrackTask*>(stack->top()), nullptr);
  ASSERT_FALSE(state->entities().isEmpty());
  EXPECT_EQ(state->entities().front().currentTask.taskType, QStringLiteral("HoldRacetrack"));
  EXPECT_DOUBLE_EQ(state->entities().front().currentTask.racetrackLegLengthMeters, 12000.0);
}

TEST_F(TaskApplicatorTest, HoldRacetrackDefaultsLegLength) {
  state->addEntity(makeAirEntity("DefaultHolder"));

  EntityTask task;
  task.taskType = "HoldRacetrack";
  task.targetLatitude = 40.0;
  task.targetLongitude = -3.0;
  task.targetAltitudeMeters = 6000;
  task.targetSpeedKnots = 240.0;

  ASSERT_TRUE(application::applyEntityTask(
      "DefaultHolder", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {}));

  ASSERT_FALSE(state->entities().isEmpty());
  EXPECT_DOUBLE_EQ(state->entities().front().currentTask.racetrackLegLengthMeters, 10000.0);
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

TEST_F(TaskApplicatorTest, AssignsFollowRouteTask) {
  state->addEntity(makeAirEntity("RouteRunner"));
  RouteGraphic route;
  route.name = QStringLiteral("Route1");
  RoutePoint p1; p1.latitude = 40.0; p1.longitude = -3.0; p1.altitudeMeters = 5000.0; p1.altitudeMetersSet = true;
  RoutePoint p2; p2.latitude = 40.1; p2.longitude = -3.1; p2.altitudeMeters = 8000.0; p2.altitudeMetersSet = true;
  route.points = {p1, p2};
  state->addRoute(route);

  EntityTask task;
  task.taskType = "FollowRoute";
  task.targetRouteName = "Route1";
  task.targetSpeedKnots = 300.0;
  task.arrivalToleranceMeters = 500.0;

  const bool result = application::applyEntityTask(
      "RouteRunner", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});

  EXPECT_TRUE(result);
  const domain::TaskStack* stack = state->getTaskStack("RouteRunner");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
  EXPECT_NE(dynamic_cast<domain::RouteTask*>(stack->top()), nullptr);
  ASSERT_FALSE(state->entities().isEmpty());
  EXPECT_EQ(state->entities().front().currentTask.taskType, QStringLiteral("FollowRoute"));
  EXPECT_EQ(state->entities().front().currentTask.routeTotalWaypoints, 2);
}

TEST_F(TaskApplicatorTest, FollowRouteWithMissingRouteCreatesFailingRouteTask) {
  state->addEntity(makeAirEntity("RouteRunner"));
  EntityTask task;
  task.taskType = "FollowRoute";
  task.targetRouteName = "Missing";
  task.targetSpeedKnots = 300.0;

  const bool result = application::applyEntityTask(
      "RouteRunner", task, false, state, nullptr,
      [this](const QString& m) { logMessages << m; },
      []() {});

  EXPECT_TRUE(result);
  const domain::TaskStack* stack = state->getTaskStack("RouteRunner");
  ASSERT_NE(stack, nullptr);
  EXPECT_FALSE(stack->isEmpty());
  EXPECT_NE(dynamic_cast<domain::RouteTask*>(stack->top()), nullptr);
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
  wp.altitudeMetersSet = true;
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
  p1.altitudeMetersSet = true;
  p2.altitudeMetersSet = true;
  route.points = {p1, p2};

  application::resolveTaskCoordinates(entity, {}, {route}, {});

  EXPECT_NEAR(entity.currentTask.targetLatitude, 15.0, 0.001);
  EXPECT_NEAR(entity.currentTask.targetLongitude, 25.0, 0.001);
  EXPECT_EQ(entity.currentTask.targetAltitudeMeters, 4000);
}

TEST(ResolveTaskCoordinates, MoveToWaypointLegacyAirAltitudeFallsBackToCurrentAltitude) {
  Entity entity = makeAirEntity(QStringLiteral("E1"));
  entity.altitude = 3000;
  EntityTask task;
  task.taskType = QStringLiteral("MoveToWaypoint");
  task.targetWaypointName = QStringLiteral("Legacy");
  task.enabled = true;
  entity.currentTask = task;

  Waypoint wp;
  wp.name = QStringLiteral("Legacy");
  wp.latitude = 48.0;
  wp.longitude = 2.0;
  wp.altitudeMeters = 0.0;
  wp.altitudeMetersSet = false;

  application::resolveTaskCoordinates(entity, {wp}, {}, {});

  EXPECT_EQ(entity.currentTask.targetAltitudeMeters, 3000);
}

TEST(ResolveTaskCoordinates, GroundRouteAltitudeIsForcedToZero) {
  Entity entity = makeGroundEntity(QStringLiteral("G1"));
  entity.altitude = 1000;
  EntityTask task;
  task.taskType = QStringLiteral("MoveAlongRoute");
  task.targetRouteName = QStringLiteral("GroundRoute");
  task.enabled = true;
  entity.currentTask = task;

  RouteGraphic route;
  route.name = QStringLiteral("GroundRoute");
  RoutePoint p1; p1.latitude = 10.0; p1.longitude = 20.0; p1.altitudeMeters = 3000.0; p1.altitudeMetersSet = true;
  RoutePoint p2; p2.latitude = 15.0; p2.longitude = 25.0; p2.altitudeMeters = 4000.0; p2.altitudeMetersSet = true;
  route.points = {p1, p2};

  application::resolveTaskCoordinates(entity, {}, {route}, {});

  EXPECT_EQ(entity.currentTask.targetAltitudeMeters, 0);
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
