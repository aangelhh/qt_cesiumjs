#include <gtest/gtest.h>
#include "presentation/MapBridgeScripts.h"

using namespace presentation::MapBridgeScripts;

// ─── buildAddOrUpdateScript ──────────────────────────────────────────────────

TEST(MapBridgeScripts, addOrUpdate_emptyMap_returnsEmpty) {
  EXPECT_TRUE(buildAddOrUpdateScript({}, false).isEmpty());
  EXPECT_TRUE(buildAddOrUpdateScript({}, true).isEmpty());
}

TEST(MapBridgeScripts, addOrUpdate_entityType_usesTrackFunction) {
  QVariantMap m;
  m[QStringLiteral("type")] = QStringLiteral("Entity");
  m[QStringLiteral("name")] = QStringLiteral("Alpha");

  const QString script = buildAddOrUpdateScript(m, false);
  EXPECT_TRUE(script.contains(QStringLiteral("addOrUpdateQtTrack")));
  EXPECT_FALSE(script.contains(QStringLiteral("addOrUpdateQtGraphic")));
  EXPECT_TRUE(script.contains(QStringLiteral("false")));
}

TEST(MapBridgeScripts, addOrUpdate_entityFocused_hasTrueFlag) {
  QVariantMap m;
  m[QStringLiteral("type")] = QStringLiteral("Entity");
  m[QStringLiteral("name")] = QStringLiteral("Bravo");

  const QString script = buildAddOrUpdateScript(m, true);
  EXPECT_TRUE(script.contains(QStringLiteral("true")));
}

TEST(MapBridgeScripts, addOrUpdate_waypointType_usesGraphicFunction) {
  QVariantMap m;
  m[QStringLiteral("type")] = QStringLiteral("Waypoint");
  m[QStringLiteral("name")] = QStringLiteral("WP1");

  const QString script = buildAddOrUpdateScript(m, false);
  EXPECT_TRUE(script.contains(QStringLiteral("addOrUpdateQtGraphic")));
  EXPECT_FALSE(script.contains(QStringLiteral("addOrUpdateQtTrack")));
}

TEST(MapBridgeScripts, addOrUpdate_routeType_usesGraphicFunction) {
  QVariantMap m;
  m[QStringLiteral("type")] = QStringLiteral("Route");
  m[QStringLiteral("name")] = QStringLiteral("Rte1");

  EXPECT_TRUE(buildAddOrUpdateScript(m, false).contains(QStringLiteral("addOrUpdateQtGraphic")));
}

TEST(MapBridgeScripts, addOrUpdate_areaType_usesGraphicFunction) {
  QVariantMap m;
  m[QStringLiteral("type")] = QStringLiteral("Area");
  m[QStringLiteral("name")] = QStringLiteral("Zone1");

  EXPECT_TRUE(buildAddOrUpdateScript(m, false).contains(QStringLiteral("addOrUpdateQtGraphic")));
}

TEST(MapBridgeScripts, addOrUpdate_containsSerializedJson) {
  QVariantMap m;
  m[QStringLiteral("type")] = QStringLiteral("Entity");
  m[QStringLiteral("name")] = QStringLiteral("Charlie");

  const QString script = buildAddOrUpdateScript(m, false);
  EXPECT_TRUE(script.contains(QStringLiteral("Charlie")));
}

// ─── buildRemoveScript ───────────────────────────────────────────────────────

TEST(MapBridgeScripts, remove_emptyName_returnsEmpty) {
  EXPECT_TRUE(buildRemoveScript(QString{}).isEmpty());
  EXPECT_TRUE(buildRemoveScript(QStringLiteral("   ")).isEmpty());
}

TEST(MapBridgeScripts, remove_validName_containsBothFunctions) {
  const QString script = buildRemoveScript(QStringLiteral("Delta"));
  EXPECT_TRUE(script.contains(QStringLiteral("removeQtTrack")));
  EXPECT_TRUE(script.contains(QStringLiteral("removeQtGraphic")));
  EXPECT_TRUE(script.contains(QStringLiteral("Delta")));
}

TEST(MapBridgeScripts, remove_nameWithSpecialChars_isJsonEncoded) {
  const QString script = buildRemoveScript(QStringLiteral("Track \"One\""));
  EXPECT_FALSE(script.isEmpty());
  EXPECT_TRUE(script.contains(QStringLiteral("removeQtTrack")));
}

// ─── buildAddOrUpdateDraftScript ─────────────────────────────────────────────

TEST(MapBridgeScripts, addOrUpdateDraft_emptyMap_returnsEmpty) {
  EXPECT_TRUE(buildAddOrUpdateDraftScript({}).isEmpty());
}

TEST(MapBridgeScripts, addOrUpdateDraft_validMap_usesDraftFunction) {
  QVariantMap m;
  m[QStringLiteral("name")] = QStringLiteral("Draft1");

  const QString script = buildAddOrUpdateDraftScript(m);
  EXPECT_TRUE(script.contains(QStringLiteral("addOrUpdateQtDraftGraphic")));
  EXPECT_TRUE(script.contains(QStringLiteral("Draft1")));
}

// ─── buildRemoveDraftScript ──────────────────────────────────────────────────

TEST(MapBridgeScripts, removeDraft_emptyName_returnsEmpty) {
  EXPECT_TRUE(buildRemoveDraftScript(QString{}).isEmpty());
  EXPECT_TRUE(buildRemoveDraftScript(QStringLiteral("  ")).isEmpty());
}

TEST(MapBridgeScripts, removeDraft_validName_usesRemoveFunction) {
  const QString script = buildRemoveDraftScript(QStringLiteral("DraftCircle"));
  EXPECT_TRUE(script.contains(QStringLiteral("removeQtDraftGraphic")));
  EXPECT_TRUE(script.contains(QStringLiteral("DraftCircle")));
}
