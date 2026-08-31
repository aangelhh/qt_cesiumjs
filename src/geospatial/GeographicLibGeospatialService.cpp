#include "geospatial/GeographicLibGeospatialService.h"

#include <geodesic.h>

#include <algorithm>
#include <cmath>
#include <limits>

namespace geospatial {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kWgs84SemiMajorAxisMeters = 6378137.0;
constexpr double kWgs84Flattening = 1.0 / 298.257223563;
constexpr double kWgs84SemiMinorAxisMeters =
    kWgs84SemiMajorAxisMeters * (1.0 - kWgs84Flattening);
constexpr double kWgs84FirstEccentricitySquared =
    kWgs84Flattening * (2.0 - kWgs84Flattening);
constexpr double kWgs84SecondEccentricitySquared =
    (kWgs84SemiMajorAxisMeters * kWgs84SemiMajorAxisMeters -
     kWgs84SemiMinorAxisMeters * kWgs84SemiMinorAxisMeters) /
    (kWgs84SemiMinorAxisMeters * kWgs84SemiMinorAxisMeters);

double toRadians(double degrees) {
  return degrees * kPi / 180.0;
}

double toDegrees(double radians) {
  return radians * 180.0 / kPi;
}

double normalizeDegrees360(double degrees) {
  double normalized = std::fmod(degrees, 360.0);
  if (normalized < 0.0) {
    normalized += 360.0;
  }
  return normalized;
}

double normalizeLongitude(double degrees) {
  double normalized = std::fmod(degrees + 180.0, 360.0);
  if (normalized < 0.0) {
    normalized += 360.0;
  }
  return normalized - 180.0;
}

const geod_geodesic& wgs84Geodesic() {
  static const geod_geodesic geodesic = []() {
    geod_geodesic value;
    geod_init(
        &value,
        kWgs84SemiMajorAxisMeters,
        kWgs84Flattening);
    return value;
  }();
  return geodesic;
}

EcefCoordinate enuToEcefDelta(
    const EnuCoordinate& coordinate,
    const GeodeticCoordinate& origin) {
  const double latitude = toRadians(origin.latitude);
  const double longitude = toRadians(origin.longitude);
  const double sinLatitude = std::sin(latitude);
  const double cosLatitude = std::cos(latitude);
  const double sinLongitude = std::sin(longitude);
  const double cosLongitude = std::cos(longitude);

  return {
      -sinLongitude * coordinate.eastMeters -
          sinLatitude * cosLongitude * coordinate.northMeters +
          cosLatitude * cosLongitude * coordinate.upMeters,
      cosLongitude * coordinate.eastMeters -
          sinLatitude * sinLongitude * coordinate.northMeters +
          cosLatitude * sinLongitude * coordinate.upMeters,
      cosLatitude * coordinate.northMeters +
          sinLatitude * coordinate.upMeters,
  };
}

} // namespace

GeodesicInverseResult GeographicLibGeospatialService::inverseGeodesic(
    const GeodeticCoordinate& from,
    const GeodeticCoordinate& to) const {
  GeodesicInverseResult result;
  geod_inverse(
      &wgs84Geodesic(),
      from.latitude,
      from.longitude,
      to.latitude,
      to.longitude,
      &result.distanceMeters,
      &result.initialBearingDegrees,
      &result.finalBearingDegrees);
  result.initialBearingDegrees =
      normalizeDegrees360(result.initialBearingDegrees);
  result.finalBearingDegrees =
      normalizeDegrees360(result.finalBearingDegrees);
  return result;
}

GeodeticCoordinate GeographicLibGeospatialService::directGeodesic(
    const GeodeticCoordinate& from,
    double initialBearingDegrees,
    double distanceMeters) const {
  GeodeticCoordinate result = from;
  geod_direct(
      &wgs84Geodesic(),
      from.latitude,
      from.longitude,
      normalizeDegrees360(initialBearingDegrees),
      distanceMeters,
      &result.latitude,
      &result.longitude,
      nullptr);
  result.longitude = normalizeLongitude(result.longitude);
  return result;
}

EcefCoordinate GeographicLibGeospatialService::geodeticToEcef(
    const GeodeticCoordinate& coordinate) const {
  const double latitude = toRadians(coordinate.latitude);
  const double longitude = toRadians(coordinate.longitude);
  const double sinLatitude = std::sin(latitude);
  const double cosLatitude = std::cos(latitude);
  const double sinLongitude = std::sin(longitude);
  const double cosLongitude = std::cos(longitude);
  const double primeVerticalRadius = kWgs84SemiMajorAxisMeters /
      std::sqrt(
          1.0 - kWgs84FirstEccentricitySquared *
                    sinLatitude * sinLatitude);

  return {
      (primeVerticalRadius + coordinate.altitudeMeters) *
          cosLatitude * cosLongitude,
      (primeVerticalRadius + coordinate.altitudeMeters) *
          cosLatitude * sinLongitude,
      (primeVerticalRadius *
           (1.0 - kWgs84FirstEccentricitySquared) +
       coordinate.altitudeMeters) * sinLatitude,
  };
}

GeodeticCoordinate GeographicLibGeospatialService::ecefToGeodetic(
    const EcefCoordinate& coordinate) const {
  const double horizontal =
      std::hypot(coordinate.xMeters, coordinate.yMeters);
  if (horizontal < 1e-9) {
    return {
        coordinate.zMeters >= 0.0 ? 90.0 : -90.0,
        0.0,
        std::abs(coordinate.zMeters) - kWgs84SemiMinorAxisMeters,
    };
  }

  const double longitude =
      std::atan2(coordinate.yMeters, coordinate.xMeters);
  const double theta = std::atan2(
      coordinate.zMeters * kWgs84SemiMajorAxisMeters,
      horizontal * kWgs84SemiMinorAxisMeters);
  const double sinTheta = std::sin(theta);
  const double cosTheta = std::cos(theta);
  const double latitude = std::atan2(
      coordinate.zMeters +
          kWgs84SecondEccentricitySquared *
              kWgs84SemiMinorAxisMeters *
              sinTheta * sinTheta * sinTheta,
      horizontal -
          kWgs84FirstEccentricitySquared *
              kWgs84SemiMajorAxisMeters *
              cosTheta * cosTheta * cosTheta);
  const double sinLatitude = std::sin(latitude);
  const double primeVerticalRadius = kWgs84SemiMajorAxisMeters /
      std::sqrt(
          1.0 - kWgs84FirstEccentricitySquared *
                    sinLatitude * sinLatitude);
  const double altitude =
      horizontal / std::cos(latitude) - primeVerticalRadius;

  return {
      toDegrees(latitude),
      normalizeLongitude(toDegrees(longitude)),
      altitude,
  };
}

EnuCoordinate GeographicLibGeospatialService::geodeticToEnu(
    const GeodeticCoordinate& coordinate,
    const GeodeticCoordinate& origin) const {
  const EcefCoordinate point = geodeticToEcef(coordinate);
  const EcefCoordinate reference = geodeticToEcef(origin);
  const double dx = point.xMeters - reference.xMeters;
  const double dy = point.yMeters - reference.yMeters;
  const double dz = point.zMeters - reference.zMeters;
  const double latitude = toRadians(origin.latitude);
  const double longitude = toRadians(origin.longitude);
  const double sinLatitude = std::sin(latitude);
  const double cosLatitude = std::cos(latitude);
  const double sinLongitude = std::sin(longitude);
  const double cosLongitude = std::cos(longitude);

  return {
      -sinLongitude * dx + cosLongitude * dy,
      -sinLatitude * cosLongitude * dx -
          sinLatitude * sinLongitude * dy +
          cosLatitude * dz,
      cosLatitude * cosLongitude * dx +
          cosLatitude * sinLongitude * dy +
          sinLatitude * dz,
  };
}

GeodeticCoordinate GeographicLibGeospatialService::enuToGeodetic(
    const EnuCoordinate& coordinate,
    const GeodeticCoordinate& origin) const {
  const EcefCoordinate reference = geodeticToEcef(origin);
  const EcefCoordinate delta = enuToEcefDelta(coordinate, origin);
  return ecefToGeodetic({
      reference.xMeters + delta.xMeters,
      reference.yMeters + delta.yMeters,
      reference.zMeters + delta.zMeters,
  });
}

NedCoordinate GeographicLibGeospatialService::geodeticToNed(
    const GeodeticCoordinate& coordinate,
    const GeodeticCoordinate& origin) const {
  const EnuCoordinate enu = geodeticToEnu(coordinate, origin);
  return {enu.northMeters, enu.eastMeters, -enu.upMeters};
}

GeodeticCoordinate GeographicLibGeospatialService::nedToGeodetic(
    const NedCoordinate& coordinate,
    const GeodeticCoordinate& origin) const {
  return enuToGeodetic(
      {coordinate.eastMeters,
       coordinate.northMeters,
       -coordinate.downMeters},
      origin);
}

const IGeospatialService& wgs84GeospatialService() {
  static const GeographicLibGeospatialService service;
  return service;
}

} // namespace geospatial
