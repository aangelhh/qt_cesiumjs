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
    double targetLat;
    double targetLon;
    double targetAlt;
    double targetSpeed;

    CmdAssignMoveTask(int id, double lat, double lon, double alt, double speed)
        : targetEntityId(id), targetLat(lat), targetLon(lon), targetAlt(alt), targetSpeed(speed) {}
};

} // namespace application
