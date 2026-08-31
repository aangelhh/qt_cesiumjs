#pragma once

#include "geospatial/IGeospatialService.h"

#include <QString>

namespace domain {

using GeoCoordinate = geospatial::GeodeticCoordinate;
using EcefCoordinate = geospatial::EcefCoordinate;
using EnuCoordinate = geospatial::EnuCoordinate;
using NedCoordinate = geospatial::NedCoordinate;
using GeodesicInverseResult = geospatial::GeodesicInverseResult;

GeodesicInverseResult inverseGeodesic(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

double distanceMeters(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

double normalizeDegrees360(double degrees);

double shortestSignedAngle(double currentHeading, double targetHeading);

double bearingDegrees(
    double latitude1, double longitude1,
    double latitude2, double longitude2);

GeoCoordinate destinationPoint(
    double latitude,
    double longitude,
    double bearingDegrees,
    double distanceMeters,
    double altitudeMeters = 0.0);

double slantDistanceMeters(
    double latitude1,
    double longitude1,
    double altitudeMeters1,
    double latitude2,
    double longitude2,
    double altitudeMeters2);

EcefCoordinate geodeticToEcef(const GeoCoordinate& coordinate);
GeoCoordinate ecefToGeodetic(const EcefCoordinate& coordinate);
EnuCoordinate geodeticToEnu(
    const GeoCoordinate& coordinate,
    const GeoCoordinate& origin);
GeoCoordinate enuToGeodetic(
    const EnuCoordinate& coordinate,
    const GeoCoordinate& origin);
NedCoordinate geodeticToNed(
    const GeoCoordinate& coordinate,
    const GeoCoordinate& origin);
GeoCoordinate nedToGeodetic(
    const NedCoordinate& coordinate,
    const GeoCoordinate& origin);

/// Formats lat/lon as "lat, lon" with 4 decimal places.
QString formatPosition(double latitude, double longitude);

/// Formats a coordinate pair as an attack point label.
QString attackPointLabel(double latitude, double longitude);

} // namespace domain
