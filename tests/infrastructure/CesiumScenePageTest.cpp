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

