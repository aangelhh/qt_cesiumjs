#pragma once

#include <QString>

namespace domain {

double distanceMeters(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

double normalizeDegrees360(double degrees);

double shortestSignedAngle(double currentHeading, double targetHeading);

double bearingDegrees(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

/// Formats lat/lon as "lat, lon" with 4 decimal places.
QString formatPosition(double latitude, double longitude);

/// Formats a coordinate pair as an attack point label.
QString attackPointLabel(double latitude, double longitude);

} // namespace domain
