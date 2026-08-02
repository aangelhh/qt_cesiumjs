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
      QStringLiteral("const pitchRadians = Cesium.Math.toRadians(visualPitchDegrees(attitude))")));
  EXPECT_TRUE(html.contains(
      QStringLiteral("Cesium.Math.toRadians(-Number(attitude.rollDegrees || 0.0))")));
}

TEST(CesiumScenePageTest, BuildHtmlCorrectsXForwardAircraftAxesBeforeAttitude) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("axes === 'x-forward-y-up'")));
  EXPECT_TRUE(html.contains(QStringLiteral("attitude.headingDegrees || 0.0) - 90.0")));
  EXPECT_TRUE(html.contains(QStringLiteral("xForwardModelCorrection")));
  EXPECT_TRUE(html.contains(QStringLiteral("Cesium.Quaternion.multiply")));
}

TEST(CesiumScenePageTest, BuildHtmlSupportsStandardZForwardAircraftAxes) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("axes === 'z-forward-y-up'")));
  EXPECT_TRUE(html.contains(QStringLiteral(
      "Cesium already converts standard glTF +Z-forward/+Y-up assets")));
}

TEST(CesiumScenePageTest, BuildHtmlKeepsPitchIndependentFromRoll) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("function visualPitchDegrees")));
  EXPECT_TRUE(html.contains(
      QStringLiteral("Cesium.Math.clamp(pitchDegrees, -12.0, 12.0)")));
  EXPECT_FALSE(html.contains(QStringLiteral("turnPitchGain")));
}

TEST(CesiumScenePageTest, BuildHtmlInterpolatesEntityAttitude) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("function updateInterpolatedAttitude")));
  EXPECT_TRUE(html.contains(QStringLiteral("function lerpAngleDegrees")));
  EXPECT_TRUE(html.contains(QStringLiteral("entity._qtAttitudeState")));
  EXPECT_TRUE(html.contains(QStringLiteral("durationMs: 50.0")));
  EXPECT_FALSE(html.contains(QStringLiteral("durationMs: 140.0")));
}

TEST(CesiumScenePageTest, BuildHtmlKeysTracksByStableEntityId) {
  const QString html = CesiumScenePage::buildHtml(QStringLiteral("token"));

  EXPECT_TRUE(html.contains(QStringLiteral("function trackIdentity(track)")));
  EXPECT_TRUE(html.contains(QStringLiteral("track.entityId || track.name")));
  EXPECT_TRUE(html.contains(QStringLiteral("qtEntitiesByName.get(trackId)")));
  EXPECT_TRUE(html.contains(QStringLiteral("qtTrackName: trackId")));
}
