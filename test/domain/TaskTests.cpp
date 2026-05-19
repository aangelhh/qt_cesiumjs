#include <gtest/gtest.h>

#include "domain/Entity.h"
#include "domain/Task.h"

TEST(TaskDomainTests, BuildPatrolRouteFromAreaUsesAreaPoints) {
    AreaDefinition area;
    area.name = "AreaA";
    area.areaType = "Polygon";
    area.points.push_back(RoutePoint{10.0, 20.0, 1000.0});
    area.points.push_back(RoutePoint{11.0, 21.0, 1100.0});
    area.points.push_back(RoutePoint{12.0, 22.0, 1200.0});

    const QVector<RoutePoint> route = domain::buildPatrolRouteFromArea(area);

    ASSERT_EQ(route.size(), 3);
    EXPECT_DOUBLE_EQ(route.at(0).latitude, 10.0);
    EXPECT_DOUBLE_EQ(route.at(1).longitude, 21.0);
    EXPECT_DOUBLE_EQ(route.at(2).altitudeMeters, 1200.0);
}

TEST(TaskDomainTests, TaskStackPushEvaluateAndPop) {
    domain::TaskStack stack;
    stack.push(std::make_unique<domain::MoveToLocationTask>(40.0, -3.0, 3000.0, 220.0));

    const domain::DesiredState state = stack.evaluateTop(
        39.0,
        -4.0,
        1000.0,
        90.0,
        1.0);
    EXPECT_GT(state.targetSpeedKnots, 0.0);
    EXPECT_FALSE(stack.isEmpty());

    stack.pop();
    EXPECT_TRUE(stack.isEmpty());
}

TEST(TaskDomainTests, MoveToLocationTaskReturnsConfiguredTargets) {
    domain::MoveToLocationTask task(41.5, -1.2, 2500.0, 180.0);
    const domain::DesiredState desired = task.evaluate(
        40.0,
        -3.0,
        1200.0,
        90.0,
        1.0);

    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 2500.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 180.0);
}

TEST(TaskDomainTests, FlyHeadingAltitudeSpeedTaskReturnsConfiguredVector) {
    domain::FlyHeadingAltitudeSpeedTask task(275.0, 4200.0, 310.0);
    const domain::DesiredState desired = task.evaluate(
        40.0,
        -3.0,
        2000.0,
        90.0,
        1.0);

    EXPECT_DOUBLE_EQ(desired.targetHeadingDegrees, 275.0);
    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 4200.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 310.0);
}

TEST(TaskDomainTests, OrbitAreaTaskMaintainsConfiguredAltitudeAndSpeed) {
    domain::OrbitAreaTask task(
        41.0,
        -1.0,
        2000.0,
        3500.0,
        250.0,
        false);
    const domain::DesiredState desired = task.evaluate(
        40.5,
        -1.5,
        1800.0,
        45.0,
        1.0);

    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 3500.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 250.0);
}

TEST(TaskDomainTests, FollowEntityTaskUsesUpdatedTargetData) {
    domain::FollowEntityTask task(1500.0, 210.0);
    task.updateTargetLocation(43.0, -5.0, 1700.0, 240.0);

    const domain::DesiredState desired = task.evaluate(
        42.0,
        -4.0,
        1200.0,
        25.0,
        1.0);

    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 1500.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 280.0);
}

TEST(TaskDomainTests, TaskStackTopSwitchesAfterPop) {
    domain::TaskStack stack;
    stack.push(std::make_unique<domain::MoveToLocationTask>(10.0, 10.0, 1000.0, 120.0));
    stack.push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(30.0, 2000.0, 220.0));

    domain::ITask::State evaluatedState = domain::ITask::State::NotStarted;
    const domain::DesiredState topState = stack.evaluateTop(
        0.0,
        0.0,
        0.0,
        0.0,
        1.0,
        &evaluatedState);
    EXPECT_DOUBLE_EQ(topState.targetHeadingDegrees, 30.0);

    stack.pop();
    const domain::DesiredState nextState = stack.evaluateTop(
        0.0,
        0.0,
        0.0,
        0.0,
        1.0,
        &evaluatedState);
    EXPECT_DOUBLE_EQ(nextState.targetAltitudeMeters, 1000.0);
    EXPECT_DOUBLE_EQ(nextState.targetSpeedKnots, 120.0);
}

TEST(TaskDomainTests, FollowEntityTaskWithoutTargetFailsAndStops) {
    domain::FollowEntityTask task(1500.0, 210.0);
    const domain::DesiredState desired = task.evaluate(
        42.0,
        -4.0,
        1200.0,
        25.0,
        1.0);

    EXPECT_EQ(task.getState(), domain::ITask::State::Failed);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}

TEST(TaskDomainTests, EmptyTaskStackReturnsNeutralDesiredState) {
    domain::TaskStack stack;
    domain::ITask::State evaluatedState = domain::ITask::State::Running;
    const domain::DesiredState desired = stack.evaluateTop(
        12.0,
        34.0,
        56.0,
        78.0,
        1.0,
        &evaluatedState);

    EXPECT_EQ(evaluatedState, domain::ITask::State::NotStarted);
    EXPECT_DOUBLE_EQ(desired.targetHeadingDegrees, 78.0);
    EXPECT_DOUBLE_EQ(desired.targetAltitudeMeters, 56.0);
    EXPECT_DOUBLE_EQ(desired.targetSpeedKnots, 0.0);
}
