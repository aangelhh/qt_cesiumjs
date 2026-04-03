#pragma once

#include <QString>

namespace application {

// Base interface for all domain events
struct IEvent {
    virtual ~IEvent() = default;
};

// Represents a change in physical position or velocity
struct EventKinematicsUpdated : public IEvent {
    int entityId;
    QString entityName;
    double latitude;
    double longitude;
    double altitudeMeters;
    double headingDegrees;
    double speedKnots;
    QString team;

    EventKinematicsUpdated(int id, const QString& name, double lat, double lon, double alt, double heading, double speed, const QString& team = "blue")
        : entityId(id), entityName(name), latitude(lat), longitude(lon), altitudeMeters(alt), headingDegrees(heading), speedKnots(speed), team(team) {}
};

} // namespace application