#include <gtest/gtest.h>
#include "../../src/domain/GeoMath.h"
#include <cmath>

using namespace domain;

TEST(GeoMath, DistanceMetersZeroForSamePoint) {
  EXPECT_DOUBLE_EQ(distanceMeters(40.0, -3.0, 40.0, -3.0), 0.0);
}

TEST(GeoMath, DistanceMetersKnownValues) {
  // Madrid (40.4168, -3.7038) to Barcelona (41.3851, 2.1734) ~505 km.
  const double d = distanceMeters(40.4168, -3.7038, 41.3851, 2.1734);
  EXPECT_NEAR(d, 504000.0, 5000.0);  // within 5 km tolerance
}

TEST(GeoMath, InverseGeodesicMatchesWgs84Reference) {
  const GeodesicInverseResult result = inverseGeodesic(
      40.7128, -74.0060,
      1.3521, 103.8198);

  EXPECT_NEAR(result.distanceMeters, 15340582.979642, 0.001);
  EXPECT_NEAR(result.initialBearingDegrees, 3.219194, 0.000001);
}

TEST(GeoMath, AntimeridianDistanceAndBearingAreStable) {
  const GeodesicInverseResult result = inverseGeodesic(
      10.0, 179.9,
      10.0, -179.9);

  EXPECT_NEAR(result.distanceMeters, 21927.9, 1.0);
  EXPECT_NEAR(result.initialBearingDegrees, 89.9826, 0.001);
}

TEST(GeoMath, DestinationPointFollowsWgs84Geodesic) {
  const GeoCoordinate destination = destinationPoint(0.0, 0.0, 90.0, 111319.490793);

  EXPECT_NEAR(destination.latitude, 0.0, 0.000001);
  EXPECT_NEAR(destination.longitude, 1.0, 0.000001);
}

TEST(GeoMath, SlantDistanceIncludesAltitudeDifference) {
  const double horizontal = distanceMeters(40.0, -3.0, 40.01, -3.0);
  const double slant = slantDistanceMeters(
      40.0, -3.0, 1000.0,
      40.01, -3.0, 2000.0);

  EXPECT_NEAR(slant, std::hypot(horizontal, 1000.0), 0.0001);
}

TEST(GeoMath, NormalizeDegrees360Basic) {
  EXPECT_DOUBLE_EQ(normalizeDegrees360(0.0), 0.0);
  EXPECT_DOUBLE_EQ(normalizeDegrees360(360.0), 0.0);
  EXPECT_DOUBLE_EQ(normalizeDegrees360(-90.0), 270.0);
  EXPECT_DOUBLE_EQ(normalizeDegrees360(450.0), 90.0);
}

TEST(GeoMath, ShortestSignedAngleBasic) {
  EXPECT_NEAR(shortestSignedAngle(10.0, 20.0), 10.0, 1e-9);
  EXPECT_NEAR(shortestSignedAngle(350.0, 10.0), 20.0, 1e-9);
  EXPECT_NEAR(shortestSignedAngle(10.0, 350.0), -20.0, 1e-9);
}

TEST(GeoMath, BearingDegreesNorthSouth) {
  // Due north: same longitude, higher latitude
  const double b = bearingDegrees(40.0, -3.0, 41.0, -3.0);
  EXPECT_NEAR(b, 0.0, 0.5);  // approximately north

  // Due south
  const double bs = bearingDegrees(41.0, -3.0, 40.0, -3.0);
  EXPECT_NEAR(bs, 180.0, 0.5);
}

TEST(GeoMath, BearingDegreesEastWest) {
  // Due east at equator
  const double be = bearingDegrees(0.0, 0.0, 0.0, 1.0);
  EXPECT_NEAR(be, 90.0, 0.1);
}
