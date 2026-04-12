#pragma once

namespace application {

/**
 * Base interface for all simulation commands.
 * Commands represent intentions that alter the simulation state.
 */
struct ICommand {
    virtual ~ICommand() = default;
    
    // In the future, we could add methods like virtual void Execute(...) = 0;
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
};

struct CmdAssignFlyHeadingTask : public ICommand {
    QString targetEntityName;
    double targetHeadingDegrees;
    double targetAltitudeMeters;
    double targetSpeedKnots;

    CmdAssignFlyHeadingTask(const QString& name, double heading, double alt, double speed)
        : targetEntityName(name), targetHeadingDegrees(heading), targetAltitudeMeters(alt), targetSpeedKnots(speed) {}
};

struct CmdAssignFollowTask : public ICommand {
    QString targetEntityName;
    QString followEntityName;
    double targetAltitudeMeters;
    double targetSpeedKnots;

    CmdAssignFollowTask(const QString& name, const QString& followName, double alt, double speed)
        : targetEntityName(name), followEntityName(followName), targetAltitudeMeters(alt), targetSpeedKnots(speed) {}
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
};

} // namespace application
