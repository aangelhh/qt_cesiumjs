#include "infrastructure/ModelOrientation.h"

#include <gtest/gtest.h>

TEST(ModelOrientation, InfersXForwardProfileFromAssetUri) {
  EXPECT_EQ(
      ModelOrientation::inferCesiumAxes(
          QStringLiteral("file:///models/Air/Fighter/F-22_RAPTOR.GLB?cache=1")),
      QStringLiteral("x-forward-y-up"));
}

TEST(ModelOrientation, InfersZForwardProfileFromAssetUri) {
  EXPECT_EQ(
      ModelOrientation::inferCesiumAxes(
          QStringLiteral("/models/Air/Fighter/a-10_thunderbolt_ii.glb")),
      QStringLiteral("z-forward-y-up"));
}

TEST(ModelOrientation, ExplicitConfigurationTakesPrecedence) {
  EXPECT_EQ(
      ModelOrientation::resolveCesiumAxes(
          QStringLiteral(" custom-profile "),
          QStringLiteral("/models/Air/Fighter/f-16_fighting_falcon.glb")),
      QStringLiteral("custom-profile"));
}

TEST(ModelOrientation, UnknownAssetPreservesLegacyOrientation) {
  EXPECT_TRUE(ModelOrientation::inferCesiumAxes(
      QStringLiteral("/models/custom/unknown.glb")).isEmpty());
}
