#pragma once

namespace domain {

double distanceMeters(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

double normalizeDegrees360(double degrees);

double shortestSignedAngle(double currentHeading, double targetHeading);

double bearingDegrees(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

} // namespace domain
