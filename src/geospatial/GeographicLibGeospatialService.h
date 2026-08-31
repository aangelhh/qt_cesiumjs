#pragma once

#include "geospatial/IGeospatialService.h"

namespace geospatial {

class GeographicLibGeospatialService final : public IGeospatialService {
public:
  GeodesicInverseResult inverseGeodesic(
      const GeodeticCoordinate& from,
      const GeodeticCoordinate& to) const override;
  GeodeticCoordinate directGeodesic(
      const GeodeticCoordinate& from,
      double initialBearingDegrees,
      double distanceMeters) const override;

  EcefCoordinate geodeticToEcef(
      const GeodeticCoordinate& coordinate) const override;
  GeodeticCoordinate ecefToGeodetic(
      const EcefCoordinate& coordinate) const override;

  EnuCoordinate geodeticToEnu(
      const GeodeticCoordinate& coordinate,
      const GeodeticCoordinate& origin) const override;
  GeodeticCoordinate enuToGeodetic(
      const EnuCoordinate& coordinate,
      const GeodeticCoordinate& origin) const override;

  NedCoordinate geodeticToNed(
      const GeodeticCoordinate& coordinate,
      const GeodeticCoordinate& origin) const override;
  GeodeticCoordinate nedToGeodetic(
      const NedCoordinate& coordinate,
      const GeodeticCoordinate& origin) const override;
};

const IGeospatialService& wgs84GeospatialService();

} // namespace geospatial
