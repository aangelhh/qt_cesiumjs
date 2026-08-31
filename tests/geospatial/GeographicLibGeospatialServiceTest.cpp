#include "geospatial/GeographicLibGeospatialService.h"

#include <gtest/gtest.h>

namespace {

constexpr double kWgs84SemiMajorAxisMeters = 6378137.0;

const geospatial::IGeospatialService& service() {
  return geospatial::wgs84GeospatialService();
}

TEST(GeographicLibGeospatialService, GeodeticEcefUsesWgs84AtEquator) {
  const geospatial::EcefCoordinate ecef = service().geodeticToEcef({0.0, 0.0, 0.0});

  EXPECT_NEAR(ecef.xMeters, kWgs84SemiMajorAxisMeters, 0.001);
  EXPECT_NEAR(ecef.yMeters, 0.0, 0.001);
  EXPECT_NEAR(ecef.zMeters, 0.0, 0.001);
}

TEST(GeographicLibGeospatialService, EcefRoundTripPreservesNegativeAltitude) {
  const geospatial::GeodeticCoordinate source{40.4168, -3.7038, -125.0};
  const geospatial::GeodeticCoordinate restored =
      service().ecefToGeodetic(service().geodeticToEcef(source));

  EXPECT_NEAR(restored.latitude, source.latitude, 0.0000001);
  EXPECT_NEAR(restored.longitude, source.longitude, 0.0000001);
  EXPECT_NEAR(restored.altitudeMeters, source.altitudeMeters, 0.001);
}

TEST(GeographicLibGeospatialService, EnuAndNedRoundTripUseDocumentedAxes) {
  const geospatial::GeodeticCoordinate origin{40.4168, -3.7038, 650.0};
  const geospatial::EnuCoordinate requestedEnu{750.0, -320.0, 125.0};

  const geospatial::GeodeticCoordinate point =
      service().enuToGeodetic(requestedEnu, origin);
  const geospatial::EnuCoordinate restoredEnu =
      service().geodeticToEnu(point, origin);
  const geospatial::NedCoordinate restoredNed =
      service().geodeticToNed(point, origin);

  EXPECT_NEAR(restoredEnu.eastMeters, requestedEnu.eastMeters, 0.001);
  EXPECT_NEAR(restoredEnu.northMeters, requestedEnu.northMeters, 0.001);
  EXPECT_NEAR(restoredEnu.upMeters, requestedEnu.upMeters, 0.001);
  EXPECT_NEAR(restoredNed.northMeters, requestedEnu.northMeters, 0.001);
  EXPECT_NEAR(restoredNed.eastMeters, requestedEnu.eastMeters, 0.001);
  EXPECT_NEAR(restoredNed.downMeters, -requestedEnu.upMeters, 0.001);
}

TEST(GeographicLibGeospatialService, PoleRoundTripIsFiniteAndStable) {
  const geospatial::GeodeticCoordinate source{90.0, 0.0, 50.0};
  const geospatial::GeodeticCoordinate restored =
      service().ecefToGeodetic(service().geodeticToEcef(source));

  EXPECT_NEAR(restored.latitude, source.latitude, 0.0000001);
  EXPECT_NEAR(restored.altitudeMeters, source.altitudeMeters, 0.001);
}

} // namespace
