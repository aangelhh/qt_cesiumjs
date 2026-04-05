#pragma once
#include <cmath>

namespace SDL_RTI {
    void geoCoordsToXYZ(double latitude, double longitude, double altitude, double& x, double& y, double& z)
    {
        // WGS84 ellipsoid constants
        const double a = 6378137.0; // semi-major axis in meters
        const double f = 1.0 / 298.257223563; // flattening
        const double e2 = f * (2.0 - f); // eccentricity squared

        float pi = 3.14159265358979323;
        double latRad = latitude * pi / 180.0; // latitude in radians
        double lonRad = longitude * pi / 180.0; // longitude in radians

        // Calculate the prime vertical radius of curvature
        double N = a / sqrt(1.0 - e2 * pow(sin(latRad), 2));

        // Calculate ECEF coordinates
        x = (N + altitude) * cos(latRad) * cos(lonRad);
        y = (N + altitude) * cos(latRad) * sin(lonRad);
        z = (N * (1.0 - e2) + altitude) * sin(latRad);
    }

}