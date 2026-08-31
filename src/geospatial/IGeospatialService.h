#pragma once

namespace geospatial {

// Canonical geodetic coordinates use WGS84 degrees and ellipsoidal meters.
struct GeodeticCoordinate {
  double latitude = 0.0;
  double longitude = 0.0;
  double altitudeMeters = 0.0;
};

struct EcefCoordinate {
  double xMeters = 0.0;
  double yMeters = 0.0;
  double zMeters = 0.0;
};

struct EnuCoordinate {
  double eastMeters = 0.0;
  double northMeters = 0.0;
  double upMeters = 0.0;
};

struct NedCoordinate {
  double northMeters = 0.0;
  double eastMeters = 0.0;
  double downMeters = 0.0;
};

struct GeodesicInverseResult {
  double distanceMeters = 0.0;
  double initialBearingDegrees = 0.0;
  double finalBearingDegrees = 0.0;
};

class IGeospatialService {
public:
  virtual ~IGeospatialService() = default;

  virtual GeodesicInverseResult inverseGeodesic(
      const GeodeticCoordinate& from,
      const GeodeticCoordinate& to) const = 0;
  virtual GeodeticCoordinate directGeodesic(
      const GeodeticCoordinate& from,
      double initialBearingDegrees,
      double distanceMeters) const = 0;

  virtual EcefCoordinate geodeticToEcef(
      const GeodeticCoordinate& coordinate) const = 0;
  virtual GeodeticCoordinate ecefToGeodetic(
      const EcefCoordinate& coordinate) const = 0;

  virtual EnuCoordinate geodeticToEnu(
      const GeodeticCoordinate& coordinate,
      const GeodeticCoordinate& origin) const = 0;
  virtual GeodeticCoordinate enuToGeodetic(
      const EnuCoordinate& coordinate,
      const GeodeticCoordinate& origin) const = 0;

  virtual NedCoordinate geodeticToNed(
      const GeodeticCoordinate& coordinate,
      const GeodeticCoordinate& origin) const = 0;
  virtual GeodeticCoordinate nedToGeodetic(
      const NedCoordinate& coordinate,
      const GeodeticCoordinate& origin) const = 0;
};

} // namespace geospatial
