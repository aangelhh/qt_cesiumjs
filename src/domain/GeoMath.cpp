#include "GeoMath.h"
#include <cmath>
#include <QString>

namespace domain {

static constexpr double kEarthRadiusMeters = 6371000.0;
static constexpr double kPi = 3.14159265358979323846;

static double toRadians(double degrees) { return degrees * kPi / 180.0; }
static double toDegrees(double radians) { return radians * 180.0 / kPi; }

double distanceMeters(
    double latitude1, double longitude1,
    double latitude2, double longitude2) {
  const double lat1 = toRadians(latitude1);
  const double lon1 = toRadians(longitude1);
  const double lat2 = toRadians(latitude2);
  const double lon2 = toRadians(longitude2);
  const double deltaLat = lat2 - lat1;
  const double deltaLon = lon2 - lon1;
  const double a = std::pow(std::sin(deltaLat / 2.0), 2.0) +
                   std::cos(lat1) * std::cos(lat2) *
                       std::pow(std::sin(deltaLon / 2.0), 2.0);
  const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
  return kEarthRadiusMeters * c;
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
  const double lat1 = toRadians(latitude1);
  const double lon1 = toRadians(longitude1);
  const double lat2 = toRadians(latitude2);
  const double lon2 = toRadians(longitude2);
  const double deltaLon = lon2 - lon1;
  const double y = std::sin(deltaLon) * std::cos(lat2);
  const double x = std::cos(lat1) * std::sin(lat2) -
                   std::sin(lat1) * std::cos(lat2) * std::cos(deltaLon);
  return normalizeDegrees360(toDegrees(std::atan2(y, x)));
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
