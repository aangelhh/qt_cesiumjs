#pragma once
#include <QString>
#include <memory>

#include "ScenarioState.h"
#include "domain/Task.h"

namespace application {

/**
 * Base interface for all simulation commands.
 * Commands represent intentions that alter the simulation state.
 */
struct ICommand {
    virtual ~ICommand() = default;
    virtual void execute(ScenarioState& scenario) const = 0;
};

struct CmdCreateEntity : public ICommand {
    int id;
    QString name;
    double lat;
    double lon;
    double alt;
    double heading;
    double speed;
    QString type;

    CmdCreateEntity(int id, const QString& name, double lat, double lon, double alt, double heading, double speed, const QString& type = "Fighter")
        : id(id), name(name), lat(lat), lon(lon), alt(alt), heading(heading), speed(speed), type(type) {}

    void execute(ScenarioState& scenario) const override {
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
};

struct CmdAssignMoveTask : public ICommand {
    int targetEntityId;
    QString targetEntityName;
    double targetLat;
    double targetLon;
    double targetAlt;
    double targetSpeed;

    CmdAssignMoveTask(int id, const QString& name, double lat, double lon, double alt, double speed)
        : targetEntityId(id), targetEntityName(name), targetLat(lat), targetLon(lon), targetAlt(alt), targetSpeed(speed) {}

    void execute(ScenarioState& scenario) const override {
        EntityTask task;
        task.enabled = true;
        task.taskType = "MoveToLocation";
        task.targetLatitude = targetLat;
        task.targetLongitude = targetLon;
        task.targetAltitudeMeters = static_cast<int>(targetAlt);
        task.targetSpeedKnots = targetSpeed;
        task.status = "Running";

        QString targetName = targetEntityName.trimmed();
        if (targetName.isEmpty() && targetEntityId == 1) {
            targetName = "TestAircraft";
        }

        if (targetName.isEmpty()) {
            return;
        }

        scenario.assignTask(targetName, task);
        domain::TaskStack* stack = scenario.getTaskStack(targetName);
        if (!stack) {
            return;
        }
        while (!stack->isEmpty()) {
            stack->pop();
        }
        stack->push(std::make_unique<domain::MoveToLocationTask>(
            task.targetLatitude, task.targetLongitude,
            task.targetAltitudeMeters, task.targetSpeedKnots
        ));
    }
};

struct CmdAssignFlyHeadingTask : public ICommand {
    QString targetEntityName;
    double targetHeadingDegrees;
    double targetAltitudeMeters;
    double targetSpeedKnots;

    CmdAssignFlyHeadingTask(const QString& name, double heading, double alt, double speed)
        : targetEntityName(name), targetHeadingDegrees(heading), targetAltitudeMeters(alt), targetSpeedKnots(speed) {}

    void execute(ScenarioState& scenario) const override {
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

        domain::TaskStack* stack = scenario.getTaskStack(targetName);
        if (!stack) {
            return;
        }
        while (!stack->isEmpty()) {
            stack->pop();
        }
        stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
            task.targetHeadingDegrees,
            static_cast<double>(task.targetAltitudeMeters),
            task.targetSpeedKnots
        ));
    }
};

struct CmdAssignFollowTask : public ICommand {
    QString targetEntityName;
    QString followEntityName;
    double targetAltitudeMeters;
    double targetSpeedKnots;

    CmdAssignFollowTask(const QString& name, const QString& followName, double alt, double speed)
        : targetEntityName(name), followEntityName(followName), targetAltitudeMeters(alt), targetSpeedKnots(speed) {}

    void execute(ScenarioState& scenario) const override {
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
        task.status = "Running";
        scenario.assignTask(targetName, task);

        domain::TaskStack* stack = scenario.getTaskStack(targetName);
        if (!stack) {
            return;
        }
        while (!stack->isEmpty()) {
            stack->pop();
        }
        stack->push(std::make_unique<domain::FollowEntityTask>(
            static_cast<double>(task.targetAltitudeMeters),
            task.targetSpeedKnots
        ));
    }
};

struct CmdAssignOrbitTask : public ICommand {
    QString targetEntityName;
    QString targetAreaName;
    double targetLatitude;
    double targetLongitude;
    double targetAreaRadiusMeters;
    double targetAltitudeMeters;
    double targetSpeedKnots;
    bool isPatrol;

    CmdAssignOrbitTask(const QString& name, const QString& areaName, double lat, double lon, double radius, double alt, double speed, bool patrol)
        : targetEntityName(name), targetAreaName(areaName), targetLatitude(lat), targetLongitude(lon), targetAreaRadiusMeters(radius), targetAltitudeMeters(alt), targetSpeedKnots(speed), isPatrol(patrol) {}

    void execute(ScenarioState& scenario) const override {
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
        while (!stack->isEmpty()) {
            stack->pop();
        }
        if (isPatrol) {
            bool createdPatrolTask = false;
            for (const AreaDefinition& area : scenario.areasSnapshot()) {
                if (area.name != task.targetAreaName && area.id != task.targetAreaName) {
                    continue;
                }
                stack->push(std::make_unique<domain::PatrolAreaTask>(
                    domain::buildPatrolRouteFromArea(area),
                    static_cast<double>(task.targetAltitudeMeters),
                    task.targetSpeedKnots
                ));
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
                    true
                ));
            }
        } else {
            stack->push(std::make_unique<domain::OrbitAreaTask>(
                task.targetLatitude,
                task.targetLongitude,
                task.targetAreaRadiusMeters,
                static_cast<double>(task.targetAltitudeMeters),
                task.targetSpeedKnots,
                false
            ));
        }
    }
};

} // namespace application
