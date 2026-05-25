#include "Command.h"

#include "ScenarioState.h"
#include "domain/Task.h"

#include <memory>

namespace application {

namespace {

// Helper: clear a TaskStack so a freshly-pushed task starts from a clean state.
void clearStack(domain::TaskStack& stack) {
    while (!stack.isEmpty()) {
        stack.pop();
    }
}

} // namespace

void CmdCreateEntity::apply(ScenarioState& scenario) const {
    Entity newEntity;
    newEntity.name = name;
    newEntity.latitude = lat;
    newEntity.longitude = lon;
    newEntity.altitude = static_cast<int>(alt);
    newEntity.headingDegrees = heading;
    newEntity.speedKnots = 0.0;
    newEntity.verticalSpeedMetersPerSecond = 0.0;
    newEntity.type = type;
    newEntity.flightDynamicsEnabled = false;
    newEntity.flightDynamicsMode = "kinematic";

    scenario.addEntity(newEntity);
}

void CmdAssignMoveTask::apply(ScenarioState& scenario) const {
    QString targetName = targetEntityName.trimmed();
    if (targetName.isEmpty() && targetEntityId == 1) {
        targetName = "TestAircraft";
    }
    if (targetName.isEmpty()) {
        return;
    }

    EntityTask task;
    task.enabled = true;
    task.taskType = "MoveToLocation";
    task.targetLatitude = targetLat;
    task.targetLongitude = targetLon;
    task.targetAltitudeMeters = static_cast<int>(targetAlt);
    task.targetSpeedKnots = targetSpeed;
    task.status = "Running";
    scenario.assignTask(targetName, task);

    if (domain::TaskStack* stack = scenario.getTaskStack(targetName)) {
        clearStack(*stack);
        stack->push(std::make_unique<domain::MoveToLocationTask>(
            task.targetLatitude, task.targetLongitude,
            task.targetAltitudeMeters, task.targetSpeedKnots));
    }
}

void CmdAssignFlyHeadingTask::apply(ScenarioState& scenario) const {
    const QString targetName = targetEntityName.trimmed();
    if (targetName.isEmpty()) {
        return;
    }

    EntityTask task;
    task.enabled = true;
    task.taskType = "FlyHeadingAltitudeSpeed";
    task.targetHeadingDegrees = targetHeadingDegrees;
    task.targetAltitudeMeters = static_cast<int>(targetAltitudeMeters);
    task.targetSpeedKnots = targetSpeedKnots;
    task.status = "Running";
    scenario.assignTask(targetName, task);

    if (domain::TaskStack* stack = scenario.getTaskStack(targetName)) {
        clearStack(*stack);
        stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
            task.targetHeadingDegrees,
            static_cast<double>(task.targetAltitudeMeters),
            task.targetSpeedKnots));
    }
}

void CmdAssignFollowTask::apply(ScenarioState& scenario) const {
    const QString targetName = targetEntityName.trimmed();
    if (targetName.isEmpty()) {
        return;
    }

    EntityTask task;
    task.enabled = true;
    task.taskType = "FollowEntity";
    task.targetEntityName = followEntityName;
    task.targetAltitudeMeters = static_cast<int>(targetAltitudeMeters);
    task.targetSpeedKnots = targetSpeedKnots;
    task.followDistanceMeters = followDistanceMeters;
    task.arrivalToleranceMeters = arrivalToleranceMeters;
    task.durationSeconds = durationSeconds;
    task.status = "Running";
    scenario.assignTask(targetName, task);

    for (const Entity& entity : scenario.entities()) {
        if (entity.name == targetName) {
            task = entity.currentTask;
            break;
        }
    }

    if (domain::TaskStack* stack = scenario.getTaskStack(targetName)) {
        clearStack(*stack);
        stack->push(std::make_unique<domain::FollowEntityTask>(
            static_cast<double>(task.targetAltitudeMeters),
            task.targetSpeedKnots,
            task.followDistanceMeters,
            task.arrivalToleranceMeters));
    }
}

void CmdAssignOrbitTask::apply(ScenarioState& scenario) const {
    const QString targetName = targetEntityName.trimmed();
    if (targetName.isEmpty()) {
        return;
    }

    EntityTask task;
    task.enabled = true;
    task.taskType = isPatrol ? "PatrolArea" : "OrbitArea";
    task.targetAreaName = targetAreaName;
    task.targetLatitude = targetLatitude;
    task.targetLongitude = targetLongitude;
    task.targetAreaRadiusMeters = targetAreaRadiusMeters;
    task.targetAltitudeMeters = static_cast<int>(targetAltitudeMeters);
    task.targetSpeedKnots = targetSpeedKnots;
    task.status = "Running";
    scenario.assignTask(targetName, task);

    domain::TaskStack* stack = scenario.getTaskStack(targetName);
    if (!stack) {
        return;
    }
    clearStack(*stack);

    if (isPatrol) {
        bool createdPatrolTask = false;
        for (const AreaDefinition& area : scenario.areas()) {
            if (area.name != task.targetAreaName && area.id != task.targetAreaName) {
                continue;
            }
            stack->push(std::make_unique<domain::PatrolAreaTask>(
                domain::buildPatrolRouteFromArea(area),
                static_cast<double>(task.targetAltitudeMeters),
                task.targetSpeedKnots));
            createdPatrolTask = true;
            break;
        }
        if (!createdPatrolTask) {
            stack->push(std::make_unique<domain::OrbitAreaTask>(
                task.targetLatitude,
                task.targetLongitude,
                task.targetAreaRadiusMeters,
                static_cast<double>(task.targetAltitudeMeters),
                task.targetSpeedKnots,
                true));
        }
    } else {
        stack->push(std::make_unique<domain::OrbitAreaTask>(
            task.targetLatitude,
            task.targetLongitude,
            task.targetAreaRadiusMeters,
            static_cast<double>(task.targetAltitudeMeters),
            task.targetSpeedKnots,
            false));
    }
}

} // namespace application
