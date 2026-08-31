#include "GeoMath.h"
#include "geospatial/GeographicLibGeospatialService.h"

#include <cmath>
#include <QString>

namespace domain {

GeodesicInverseResult inverseGeodesic(
    double latitude1, double longitude1,
    double latitude2, double longitude2) {
  return geospatial::wgs84GeospatialService().inverseGeodesic(
      {latitude1, longitude1, 0.0},
      {latitude2, longitude2, 0.0});
}

double distanceMeters(
    double latitude1, double longitude1,
    double latitude2, double longitude2) {
  return inverseGeodesic(
      latitude1, longitude1, latitude2, longitude2).distanceMeters;
}

double normalizeDegrees360(double degrees) {
  while (degrees < 0.0) {
    degrees += 360.0;
  }
  while (degrees >= 360.0) {
    degrees -= 360.0;
  }
  return degrees;
}

double shortestSignedAngle(double currentHeading, double targetHeading) {
  double delta = normalizeDegrees360(targetHeading) -
                 normalizeDegrees360(currentHeading);
  while (delta > 180.0) {
    delta -= 360.0;
  }
  while (delta < -180.0) {
    delta += 360.0;
  }
  return delta;
}

double bearingDegrees(
    double latitude1, double longitude1,
    double latitude2, double longitude2) {
  return inverseGeodesic(
      latitude1, longitude1, latitude2, longitude2)
      .initialBearingDegrees;
}

GeoCoordinate destinationPoint(
    double latitude,
    double longitude,
    double bearingDegreesValue,
    double distanceMetersValue,
    double altitudeMeters) {
  return geospatial::wgs84GeospatialService().directGeodesic(
      {latitude, longitude, altitudeMeters},
      bearingDegreesValue,
      distanceMetersValue);
}

double slantDistanceMeters(
    double latitude1,
    double longitude1,
    double altitudeMeters1,
    double latitude2,
    double longitude2,
    double altitudeMeters2) {
  const double horizontal = distanceMeters(
      latitude1, longitude1, latitude2, longitude2);
  return std::hypot(horizontal, altitudeMeters2 - altitudeMeters1);
}

EcefCoordinate geodeticToEcef(const GeoCoordinate& coordinate) {
  return geospatial::wgs84GeospatialService().geodeticToEcef(coordinate);
}

GeoCoordinate ecefToGeodetic(const EcefCoordinate& coordinate) {
  return geospatial::wgs84GeospatialService().ecefToGeodetic(coordinate);
}

EnuCoordinate geodeticToEnu(
    const GeoCoordinate& coordinate,
    const GeoCoordinate& origin) {
  return geospatial::wgs84GeospatialService().geodeticToEnu(
      coordinate, origin);
}

GeoCoordinate enuToGeodetic(
    const EnuCoordinate& coordinate,
    const GeoCoordinate& origin) {
  return geospatial::wgs84GeospatialService().enuToGeodetic(
      coordinate, origin);
}

NedCoordinate geodeticToNed(
    const GeoCoordinate& coordinate,
    const GeoCoordinate& origin) {
  return geospatial::wgs84GeospatialService().geodeticToNed(
      coordinate, origin);
}

GeoCoordinate nedToGeodetic(
    const NedCoordinate& coordinate,
    const GeoCoordinate& origin) {
  return geospatial::wgs84GeospatialService().nedToGeodetic(
      coordinate, origin);
}

QString formatPosition(double latitude, double longitude) {
  return QStringLiteral("%1, %2")
      .arg(latitude, 0, 'f', 4)
      .arg(longitude, 0, 'f', 4);
}

QString attackPointLabel(double latitude, double longitude) {
  return formatPosition(latitude, longitude);
}

} // namespace domain
