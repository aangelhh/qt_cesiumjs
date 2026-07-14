#include "infrastructure/CesiumScenePage.h"

#include <gtest/gtest.h>

TEST(CesiumScenePageTest, BuildHtmlInjectsAccessTokenAsJsonStringLiteral) {
  const QString html =
      CesiumScenePage::buildHtml(QStringLiteral("abc'\"\\</script>123"));

  EXPECT_TRUE(html.contains(
      QStringLiteral("Cesium.Ion.defaultAccessToken = \"abc'\\\"\\\\<\\/script>123\";")));
  EXPECT_FALSE(html.contains(
      QStringLiteral("Cesium.Ion.defaultAccessToken = '';")));
}

TEST(CesiumScenePageTest, BuildHtmlPreservesSimulationPitchSignForCesium) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(
      QStringLiteral("Cesium.Math.toRadians(visualPitchDegrees(attitude))")));
  EXPECT_TRUE(html.contains(
      QStringLiteral("Cesium.Math.toRadians(-Number(attitude.rollDegrees || 0.0))")));
}

TEST(CesiumScenePageTest, BuildHtmlAddsTurnPitchVisualCompensation) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("function visualPitchDegrees")));
  EXPECT_TRUE(html.contains(QStringLiteral("turnPitchGain")));
  EXPECT_TRUE(html.contains(QStringLiteral("pitchDegrees * turnPitchGain")));
}

TEST(CesiumScenePageTest, BuildHtmlInterpolatesEntityAttitude) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("function updateInterpolatedAttitude")));
  EXPECT_TRUE(html.contains(QStringLiteral("function lerpAngleDegrees")));
  EXPECT_TRUE(html.contains(QStringLiteral("entity._qtAttitudeState")));
}
