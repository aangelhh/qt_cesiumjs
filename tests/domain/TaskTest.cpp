// Unit tests for the domain task hierarchy. These tests exercise the
// pure-geometry behaviour of ITask implementations without involving Qt
// signals, the scenario state, the simulation thread, or the UI.

#include "domain/Task.h"

#include <gtest/gtest.h>

#include <cmath>

namespace {

// Reference coordinate used as origin for most tests (somewhere over central
// Spain — pick any value; only relative motion matters).
constexpr double kOriginLat = 40.0;
constexpr double kOriginLon = -3.5;

// One degree of latitude is ~111 km. To place a target ~5 km away we offset
// latitude by 0.045 degrees.
constexpr double kFiveKmNorthDeltaLat = 0.045;

double approxBearingNorth() { return 0.0; }

} // namespace

// --- MoveToLocationTask -----------------------------------------------------

TEST(MoveToLocationTask, StartsNotStarted) {
    domain::MoveToLocationTask task(kOriginLat + kFiveKmNorthDeltaLat,
                                    kOriginLon, 5000.0, 250.0);
    EXPECT_EQ(task.getState(), domain::ITask::State::NotStarted);
}

TEST(MoveToLocationTask, TransitionsToRunningOnEvaluate) {
    domain::MoveToLocationTask task(kOriginLat + kFiveKmNorthDeltaLat,
                                    kOriginLon, 5000.0, 250.0);
    task.evaluate(kOriginLat, kOriginLon, 4000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Running);
}

TEST(MoveToLocationTask, EmitsBearingTowardsTarget) {
    // Target directly north of the entity. Bearing should be ~0 degrees.
    domain::MoveToLocationTask task(kOriginLat + kFiveKmNorthDeltaLat,
                                    kOriginLon, 5000.0, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 4000.0, 180.0, 0.1);
    EXPECT_NEAR(desired.targetHeadingDegrees, approxBearingNorth(), 1.0);
    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 5000.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 250.0);
}

TEST(MoveToLocationTask, CompletesWhenWithinArrivalThreshold) {
    // Target equals current position -> distance ~0 -> task completes and
    // commands speed 0 (stop).
    domain::MoveToLocationTask task(kOriginLat, kOriginLon, 5000.0, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 5000.0, 90.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Completed);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}

// --- RouteTask --------------------------------------------------------------

TEST(RouteTask, FailsImmediatelyWhenRouteIsEmpty) {
    domain::RouteTask task(QVector<RoutePoint>{}, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 4000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Failed);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}

TEST(RouteTask, AdvancesThroughWaypointsAsItPassesEach) {
    QVector<RoutePoint> route;
    RoutePoint a; a.latitude = kOriginLat; a.longitude = kOriginLon; a.altitudeMeters = 5000.0;
    RoutePoint b; b.latitude = kOriginLat + kFiveKmNorthDeltaLat; b.longitude = kOriginLon; b.altitudeMeters = 5000.0;
    route.push_back(a);
    route.push_back(b);

    domain::RouteTask task(route, 250.0);

    // Start at the first waypoint -> should immediately advance and target b.
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Running);
    // Bearing towards b is roughly north.
    EXPECT_NEAR(desired.targetHeadingDegrees, 0.0, 1.0);
}

TEST(RouteTask, CompletesAfterPassingFinalWaypoint) {
    QVector<RoutePoint> route;
    RoutePoint a; a.latitude = kOriginLat; a.longitude = kOriginLon; a.altitudeMeters = 5000.0;
    route.push_back(a);

    domain::RouteTask task(route, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Completed);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}

// --- FlyHeadingAltitudeSpeedTask -------------------------------------------

TEST(FlyHeadingAltitudeSpeedTask, EmitsConstantSetpoint) {
    domain::FlyHeadingAltitudeSpeedTask task(270.0, 8000.0, 300.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 4000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Running);
    EXPECT_DOUBLE_EQ(desired.targetHeadingDegrees, 270.0);
    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 8000.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 300.0);
}

TEST(FlyHeadingAltitudeSpeedTask, NeverCompletes) {
    domain::FlyHeadingAltitudeSpeedTask task(90.0, 5000.0, 200.0);
    for (int i = 0; i < 100; ++i) {
        task.evaluate(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1);
    }
    EXPECT_EQ(task.getState(), domain::ITask::State::Running);
}

// --- FollowEntityTask ------------------------------------------------------

TEST(FollowEntityTask, FailsWhenNoTargetEverProvided) {
    domain::FollowEntityTask task(5000.0, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Failed);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}

TEST(FollowEntityTask, ChasesTargetWhenLocationKnown) {
    domain::FollowEntityTask task(5000.0, 250.0);
    task.updateTargetLocation(kOriginLat + kFiveKmNorthDeltaLat, kOriginLon, 5000.0, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Running);
    EXPECT_NEAR(desired.targetHeadingDegrees, 0.0, 1.0);
}

TEST(FollowEntityTask, StopsPursuitInsideFollowDistance) {
    domain::FollowEntityTask task(5000.0, 250.0, 1000.0, 100.0);
    task.updateTargetLocation(kOriginLat + kFiveKmNorthDeltaLat / 10.0, kOriginLon, 5000.0, 250.0);
    const auto desired = task.evaluate(kOriginLat, kOriginLon, 5000.0, 45.0, 0.1);
    EXPECT_EQ(task.getState(), domain::ITask::State::Running);
    EXPECT_DOUBLE_EQ(desired.targetHeadingDegrees, 45.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}

// --- OrbitAreaTask ---------------------------------------------------------

TEST(OrbitAreaTask, DoesNotCompleteWhenOrbiting) {
    domain::OrbitAreaTask task(kOriginLat, kOriginLon, 2000.0, 5000.0, 200.0, false);
    // Place the entity well outside the radius and tick a few times.
    for (int i = 0; i < 50; ++i) {
        task.evaluate(kOriginLat + kFiveKmNorthDeltaLat, kOriginLon, 5000.0, 90.0, 0.1);
    }
    EXPECT_NE(task.getState(), domain::ITask::State::Completed);
}

// --- TaskStack -------------------------------------------------------------

TEST(TaskStack, IsEmptyOnConstruction) {
    domain::TaskStack stack;
    EXPECT_TRUE(stack.isEmpty());
    EXPECT_EQ(stack.top(), nullptr);
}

TEST(TaskStack, EvaluatesTopAndReportsState) {
    domain::TaskStack stack;
    stack.push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(45.0, 6000.0, 200.0));
    ASSERT_FALSE(stack.isEmpty());

    domain::ITask::State state = domain::ITask::State::NotStarted;
    const auto desired = stack.evaluateTop(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1, &state);
    EXPECT_EQ(state, domain::ITask::State::Running);
    EXPECT_DOUBLE_EQ(desired.targetHeadingDegrees, 45.0);
}

TEST(TaskStack, PopRemovesTopTask) {
    domain::TaskStack stack;
    stack.push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(0.0, 5000.0, 200.0));
    stack.push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(90.0, 5000.0, 200.0));
    ASSERT_FALSE(stack.isEmpty());

    domain::ITask::State state = domain::ITask::State::NotStarted;
    const auto firstDesired = stack.evaluateTop(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1, &state);
    EXPECT_DOUBLE_EQ(firstDesired.targetHeadingDegrees, 90.0);

    stack.pop();
    const auto secondDesired = stack.evaluateTop(kOriginLat, kOriginLon, 5000.0, 0.0, 0.1, &state);
    EXPECT_DOUBLE_EQ(secondDesired.targetHeadingDegrees, 0.0);

    stack.pop();
    EXPECT_TRUE(stack.isEmpty());
}
