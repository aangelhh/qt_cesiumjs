#pragma once

#include <QString>

class ScenarioState;

namespace application {

/**
 * Base interface for all simulation commands.
 * Commands represent intentions that alter the simulation state.
 *
 * Each concrete command knows how to apply itself to ScenarioState, which
 * removes the need for the dispatcher (SimulationEngine) to switch on the
 * concrete type via dynamic_cast.
 */
struct ICommand {
    virtual ~ICommand() = default;
    virtual void apply(ScenarioState& scenario) const = 0;
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

    void apply(ScenarioState& scenario) const override;
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

    void apply(ScenarioState& scenario) const override;
};

struct CmdAssignWaitOnLocationTask : public ICommand {
    QString targetEntityName;
    double targetLat;
    double targetLon;
    double targetAlt;
    double targetSpeed;
    double arrivalToleranceMeters;
    double durationSeconds;

    CmdAssignWaitOnLocationTask(
        const QString& name,
        double lat,
        double lon,
        double alt,
        double speed,
        double tolerance,
        double duration)
        : targetEntityName(name),
          targetLat(lat),
          targetLon(lon),
          targetAlt(alt),
          targetSpeed(speed),
          arrivalToleranceMeters(tolerance),
          durationSeconds(duration) {}

    void apply(ScenarioState& scenario) const override;
};

struct CmdAssignFlyHeadingTask : public ICommand {
    QString targetEntityName;
    double targetHeadingDegrees;
    double targetAltitudeMeters;
    double targetSpeedKnots;

    CmdAssignFlyHeadingTask(const QString& name, double heading, double alt, double speed)
        : targetEntityName(name), targetHeadingDegrees(heading), targetAltitudeMeters(alt), targetSpeedKnots(speed) {}

    void apply(ScenarioState& scenario) const override;
};

struct CmdAssignFollowTask : public ICommand {
    QString targetEntityName;
    QString followEntityName;
    double targetAltitudeMeters;
    double targetSpeedKnots;
    double followDistanceMeters;
    double arrivalToleranceMeters;
    double durationSeconds;

    CmdAssignFollowTask(
        const QString& name,
        const QString& followName,
        double alt,
        double speed,
        double distance,
        double tolerance,
        double duration)
        : targetEntityName(name),
          followEntityName(followName),
          targetAltitudeMeters(alt),
          targetSpeedKnots(speed),
          followDistanceMeters(distance),
          arrivalToleranceMeters(tolerance),
          durationSeconds(duration) {}

    void apply(ScenarioState& scenario) const override;
};

struct CmdAssignInterceptEntity2DTask : public ICommand {
    QString targetEntityName;
    QString interceptEntityName;
    double targetSpeedKnots;
    double interceptDistanceMeters;
    double timeoutSeconds;

    CmdAssignInterceptEntity2DTask(
        const QString& name,
        const QString& interceptName,
        double speed,
        double distance,
        double timeout)
        : targetEntityName(name),
          interceptEntityName(interceptName),
          targetSpeedKnots(speed),
          interceptDistanceMeters(distance),
          timeoutSeconds(timeout) {}

    void apply(ScenarioState& scenario) const override;
};

struct CmdAssignInterceptEntity3DTask : public ICommand {
    QString targetEntityName;
    QString interceptEntityName;
    double targetSpeedKnots;
    double interceptDistanceMeters;
    double altitudeToleranceMeters;
    double timeoutSeconds;

    CmdAssignInterceptEntity3DTask(
        const QString& name,
        const QString& interceptName,
        double speed,
        double distance,
        double altitudeTolerance,
        double timeout)
        : targetEntityName(name),
          interceptEntityName(interceptName),
          targetSpeedKnots(speed),
          interceptDistanceMeters(distance),
          altitudeToleranceMeters(altitudeTolerance),
          timeoutSeconds(timeout) {}

    void apply(ScenarioState& scenario) const override;
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

    void apply(ScenarioState& scenario) const override;
};

struct CmdAssignHoldRacetrackTask : public ICommand {
    QString targetEntityName;
    double centerLatitude;
    double centerLongitude;
    double headingDegrees;
    double legLengthMeters;
    double targetAltitudeMeters;
    double targetSpeedKnots;
    double durationSeconds;

    CmdAssignHoldRacetrackTask(
        const QString& name,
        double lat,
        double lon,
        double heading,
        double legLength,
        double alt,
        double speed,
        double duration)
        : targetEntityName(name),
          centerLatitude(lat),
          centerLongitude(lon),
          headingDegrees(heading),
          legLengthMeters(legLength),
          targetAltitudeMeters(alt),
          targetSpeedKnots(speed),
          durationSeconds(duration) {}

    void apply(ScenarioState& scenario) const override;
};

} // namespace application
