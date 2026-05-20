#include <gtest/gtest.h>
#include "application/ScenarioState.h"
#include "presentation/ScenarioObjectEditorController.h"
#include "domain/Entity.h"

using namespace presentation;

namespace {

struct Fixture {
  ScenarioState state;
  QString lastLog;
  QString lastStatus;
  bool synced = false;
  QString selectedEntity;
  QString selectedObject;
  bool isEntity = true;
  bool isDestroyed = false;
  QVariantMap currentSummary;
  QString cleanupReturn;
  QString removedTrack;

  ScenarioObjectEditorController* makeCtrl() {
    return new ScenarioObjectEditorController(
        &state,
        [this]() { return selectedEntity; },
        [this]() { return selectedObject; },
        [this]() { return isEntity; },
        [this]() { return isDestroyed; },
        [this]() { return currentSummary; },
        [this](const QString& name) { return cleanupReturn; },
        [this](const QString& name) { removedTrack = name; },
        [this](const QString& msg) { lastLog = msg; },
        [this](const QString& msg) { lastStatus = msg; },
        [this]() { synced = true; },
        nullptr);
  }

  Entity makeEntity(const QString& name) {
    Entity e;
    e.name = name;
    e.latitude  = 0.0;
    e.longitude = 0.0;
    e.altitude  = 1000.0;
    e.headingDegrees = 0.0;
    e.speedKnots = 100.0;
    e.forceIdentifier = 1;
    return e;
  }
};

} // namespace

// ─── deleteSelected – entity ───────────────────────────────────────────────

TEST(ScenarioObjectEditorController, deleteSelectedEntity_removesEntityAndLogsStatus) {
  Fixture f;
  f.state.addEntity(f.makeEntity(QStringLiteral("Alpha")));
  f.selectedObject = QStringLiteral("Alpha");
  f.selectedEntity = QStringLiteral("Alpha");
  f.isEntity = true;
  QVariantMap sum;
  sum[QStringLiteral("type")] = QStringLiteral("Entity");
  f.currentSummary = sum;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_TRUE(f.state.entities().isEmpty());
  EXPECT_EQ(f.removedTrack, QStringLiteral("Alpha"));
  EXPECT_TRUE(f.synced);
  EXPECT_TRUE(f.lastLog.contains(QStringLiteral("Alpha")));
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(ScenarioObjectEditorController, deleteSelectedEntity_emptyName_doesNothing) {
  Fixture f;
  f.selectedObject = QString{};
  f.isEntity = true;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_TRUE(f.removedTrack.isEmpty());
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(ScenarioObjectEditorController, deleteSelectedEntity_unknownEntity_doesNotLog) {
  Fixture f;
  f.selectedObject = QStringLiteral("Unknown");
  f.isEntity = true;
  QVariantMap sum;
  sum[QStringLiteral("type")] = QStringLiteral("Entity");
  f.currentSummary = sum;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_TRUE(f.lastLog.isEmpty());
  EXPECT_FALSE(f.synced);
  delete ctrl;
}

// ─── deleteSelected – waypoint ─────────────────────────────────────────────

TEST(ScenarioObjectEditorController, deleteSelectedWaypoint_removesWaypointAndLogs) {
  Fixture f;
  Waypoint wp;
  wp.name = QStringLiteral("WP1");
  wp.latitude = 0.0;
  wp.longitude = 0.0;
  wp.altitudeMeters = 0.0;
  f.state.addWaypoint(wp);
  f.selectedObject = QStringLiteral("WP1");
  f.isEntity = false;
  QVariantMap sum;
  sum[QStringLiteral("type")] = QStringLiteral("Waypoint");
  f.currentSummary = sum;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_TRUE(f.state.waypoints().isEmpty());
  EXPECT_EQ(f.removedTrack, QStringLiteral("WP1"));
  EXPECT_TRUE(f.synced);
  EXPECT_TRUE(f.lastLog.contains(QStringLiteral("WP1")));
  delete ctrl;
}

// ─── deleteSelected – route ────────────────────────────────────────────────

TEST(ScenarioObjectEditorController, deleteSelectedRoute_removesRouteAndLogs) {
  Fixture f;
  RouteGraphic rt;
  rt.name = QStringLiteral("Route1");
  f.state.addRoute(rt);
  f.selectedObject = QStringLiteral("Route1");
  f.isEntity = false;
  QVariantMap sum;
  sum[QStringLiteral("type")] = QStringLiteral("Route");
  f.currentSummary = sum;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_TRUE(f.state.routes().isEmpty());
  EXPECT_TRUE(f.synced);
  delete ctrl;
}

// ─── deleteSelected – area ─────────────────────────────────────────────────

TEST(ScenarioObjectEditorController, deleteSelectedArea_removesAreaAndLogs) {
  Fixture f;
  AreaDefinition area;
  area.name = QStringLiteral("Area1");
  f.state.addArea(area);
  f.selectedObject = QStringLiteral("Area1");
  f.isEntity = false;
  QVariantMap sum;
  sum[QStringLiteral("type")] = QStringLiteral("Area");
  f.currentSummary = sum;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_TRUE(f.state.areas().isEmpty());
  EXPECT_TRUE(f.synced);
  delete ctrl;
}

// ─── deleteSelected – cleanup message propagated ───────────────────────────

TEST(ScenarioObjectEditorController, deleteSelected_withCleanupMessage_usesCleanupStatus) {
  Fixture f;
  f.state.addEntity(f.makeEntity(QStringLiteral("Bravo")));
  f.selectedObject = QStringLiteral("Bravo");
  f.selectedEntity = QStringLiteral("Bravo");
  f.isEntity = true;
  f.cleanupReturn = QStringLiteral("Cleanup: bomb target cleared");
  QVariantMap sum;
  sum[QStringLiteral("type")] = QStringLiteral("Entity");
  f.currentSummary = sum;

  auto* ctrl = f.makeCtrl();
  ctrl->deleteSelected();

  EXPECT_EQ(f.lastStatus, QStringLiteral("Cleanup: bomb target cleared"));
  delete ctrl;
}

// ─── clearSelectedTask ──────────────────────────────────────────────────────

TEST(ScenarioObjectEditorController, clearSelectedTask_clearsTaskAndLogs) {
  Fixture f;
  f.state.addEntity(f.makeEntity(QStringLiteral("Charlie")));
  EntityTask task;
  task.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  f.state.assignTask(QStringLiteral("Charlie"), task);
  f.selectedEntity = QStringLiteral("Charlie");
  f.isDestroyed = false;

  auto* ctrl = f.makeCtrl();
  ctrl->clearSelectedTask();

  EXPECT_TRUE(f.synced);
  EXPECT_TRUE(f.lastLog.contains(QStringLiteral("Charlie")));
  delete ctrl;
}

TEST(ScenarioObjectEditorController, clearSelectedTask_emptyName_doesNothing) {
  Fixture f;
  f.selectedEntity = QString{};

  auto* ctrl = f.makeCtrl();
  ctrl->clearSelectedTask();

  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(ScenarioObjectEditorController, clearSelectedTask_destroyedEntity_doesNothing) {
  Fixture f;
  f.state.addEntity(f.makeEntity(QStringLiteral("Delta")));
  f.selectedEntity = QStringLiteral("Delta");
  f.isDestroyed = true;

  auto* ctrl = f.makeCtrl();
  ctrl->clearSelectedTask();

  EXPECT_FALSE(f.synced);
  delete ctrl;
}

TEST(ScenarioObjectEditorController, clearSelectedTask_noTaskAssigned_stillSyncs) {
  Fixture f;
  f.state.addEntity(f.makeEntity(QStringLiteral("Echo")));
  f.selectedEntity = QStringLiteral("Echo");
  f.isDestroyed = false;

  auto* ctrl = f.makeCtrl();
  ctrl->clearSelectedTask();

  // clearTask returns true (sets status to Idle) so sync and log happen
  EXPECT_TRUE(f.synced);
  EXPECT_TRUE(f.lastLog.contains(QStringLiteral("Echo")));
  delete ctrl;
}
