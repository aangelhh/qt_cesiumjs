#include <gtest/gtest.h>
#include "presentation/GraphicPickCoordinator.h"
#include "application/ScenarioState.h"

using namespace presentation;

class GraphicPickCoordinatorTest : public ::testing::Test {
protected:
  void SetUp() override {
    state = std::make_unique<ScenarioState>();
    statusMessages.clear();
    syncUiCount = 0;
    drafts.clear();
    clearedDrafts.clear();
    beginPickCount = 0;

    coordinator = std::make_unique<GraphicPickCoordinator>(
        state.get(),
        [this](const QString& msg) { statusMessages.append(msg); },
        [this]() { ++syncUiCount; },
        [this](const QVariantMap& draft) { drafts.append(draft); },
        [this](const QString& name) { clearedDrafts.append(name); },
        [this]() { ++beginPickCount; });
  }

  std::unique_ptr<ScenarioState> state;
  std::unique_ptr<GraphicPickCoordinator> coordinator;
  QStringList statusMessages;
  int syncUiCount{0};
  QVector<QVariantMap> drafts;
  QStringList clearedDrafts;
  int beginPickCount{0};
};

TEST_F(GraphicPickCoordinatorTest, InitiallyNotPending) {
  EXPECT_FALSE(coordinator->isPending());
}

TEST_F(GraphicPickCoordinatorTest, HandleCoordinateReturnsFalseWhenNotPending) {
  EXPECT_FALSE(coordinator->handleCoordinate(10.0, 20.0, 100.0));
}

TEST_F(GraphicPickCoordinatorTest, BeginWaypointPickSetsPending) {
  coordinator->beginWaypointPick(QStringLiteral("WP1"));
  EXPECT_TRUE(coordinator->isPending());
  EXPECT_EQ(beginPickCount, 1);
  EXPECT_EQ(statusMessages.size(), 1);
}

TEST_F(GraphicPickCoordinatorTest, WaypointPickCreatesWaypoint) {
  coordinator->beginWaypointPick(QStringLiteral("WP1"));
  EXPECT_TRUE(coordinator->handleCoordinate(5.0, 40.0, 200.0));

  EXPECT_FALSE(coordinator->isPending());
  EXPECT_EQ(syncUiCount, 1);
  EXPECT_EQ(state->waypoints().size(), 1);
  EXPECT_EQ(state->waypoints().first().name, QStringLiteral("WP1"));
  EXPECT_DOUBLE_EQ(state->waypoints().first().longitude, 5.0);
  EXPECT_DOUBLE_EQ(state->waypoints().first().latitude, 40.0);
  EXPECT_DOUBLE_EQ(state->waypoints().first().altitudeMeters, 250.0);  // 200 + 50 offset
  EXPECT_FALSE(state->waypoints().first().altitudeMetersSet);
}

TEST_F(GraphicPickCoordinatorTest, WaypointPickUsesConfiguredAltitude) {
  coordinator->beginWaypointPick(QStringLiteral("WP1"), 5000.0);
  EXPECT_TRUE(coordinator->handleCoordinate(5.0, 40.0, 200.0));

  ASSERT_EQ(state->waypoints().size(), 1);
  EXPECT_DOUBLE_EQ(state->waypoints().first().altitudeMeters, 5000.0);
  EXPECT_TRUE(state->waypoints().first().altitudeMetersSet);
}

TEST_F(GraphicPickCoordinatorTest, RoutePickNeedsTwoPoints) {
  coordinator->beginRoutePick(QStringLiteral("R1"));
  EXPECT_TRUE(coordinator->isPending());

  // First point — stays pending, sends draft
  EXPECT_TRUE(coordinator->handleCoordinate(1.0, 10.0, 100.0));
  EXPECT_TRUE(coordinator->isPending());
  EXPECT_EQ(syncUiCount, 0);
  EXPECT_EQ(drafts.size(), 1);

  // Second point — finishes
  EXPECT_TRUE(coordinator->handleCoordinate(2.0, 11.0, 200.0));
  EXPECT_FALSE(coordinator->isPending());
  EXPECT_EQ(syncUiCount, 1);
  EXPECT_EQ(state->routes().size(), 1);
  EXPECT_EQ(state->routes().first().name, QStringLiteral("R1"));
  EXPECT_EQ(state->routes().first().points.size(), 2);
}

TEST_F(GraphicPickCoordinatorTest, RoutePickUsesConfiguredPointAltitudes) {
  coordinator->beginRoutePick(QStringLiteral("R1"), 5000.0, 8000.0);

  EXPECT_TRUE(coordinator->handleCoordinate(1.0, 10.0, 100.0));
  EXPECT_TRUE(coordinator->handleCoordinate(2.0, 11.0, 200.0));

  ASSERT_EQ(state->routes().size(), 1);
  ASSERT_EQ(state->routes().first().points.size(), 2);
  EXPECT_DOUBLE_EQ(state->routes().first().points.at(0).altitudeMeters, 5000.0);
  EXPECT_TRUE(state->routes().first().points.at(0).altitudeMetersSet);
  EXPECT_DOUBLE_EQ(state->routes().first().points.at(1).altitudeMeters, 8000.0);
  EXPECT_TRUE(state->routes().first().points.at(1).altitudeMetersSet);
}

TEST_F(GraphicPickCoordinatorTest, AreaCirclePickCreatesArea) {
  GraphicPickCoordinator::AreaCircleParams params{5000.0, 1000.0};
  coordinator->beginAreaCirclePick(QStringLiteral("A1"), params);
  EXPECT_TRUE(coordinator->isPending());

  EXPECT_TRUE(coordinator->handleCoordinate(3.0, 42.0, 300.0));
  EXPECT_FALSE(coordinator->isPending());
  EXPECT_EQ(syncUiCount, 1);
  EXPECT_EQ(state->areas().size(), 1);
  EXPECT_EQ(state->areas().first().name, QStringLiteral("A1"));
  EXPECT_EQ(state->areas().first().areaType, QStringLiteral("Circle"));
  EXPECT_DOUBLE_EQ(state->areas().first().radiusMeters, 1000.0);
}

TEST_F(GraphicPickCoordinatorTest, AreaEllipsePickCreatesArea) {
  GraphicPickCoordinator::AreaEllipseParams params{3000.0, 2000.0, 1000.0, 45.0};
  coordinator->beginAreaEllipsePick(QStringLiteral("E1"), params);
  EXPECT_TRUE(coordinator->isPending());

  EXPECT_TRUE(coordinator->handleCoordinate(4.0, 43.0, 100.0));
  EXPECT_FALSE(coordinator->isPending());
  EXPECT_EQ(state->areas().size(), 1);
  EXPECT_EQ(state->areas().first().areaType, QStringLiteral("Ellipse"));
  EXPECT_DOUBLE_EQ(state->areas().first().semiMajorAxisMeters, 2000.0);
  EXPECT_DOUBLE_EQ(state->areas().first().semiMinorAxisMeters, 1000.0);
  EXPECT_DOUBLE_EQ(state->areas().first().rotationDegrees, 45.0);
}

TEST_F(GraphicPickCoordinatorTest, PolygonPickNeedsCloseToFirstPoint) {
  coordinator->beginAreaPolygonPick(QStringLiteral("P1"), 1000.0);
  EXPECT_TRUE(coordinator->isPending());

  // Add 3 points, all far from first
  EXPECT_TRUE(coordinator->handleCoordinate(0.0, 0.0, 100.0));
  EXPECT_TRUE(coordinator->isPending());
  EXPECT_TRUE(coordinator->handleCoordinate(1.0, 0.0, 100.0));
  EXPECT_TRUE(coordinator->isPending());
  EXPECT_TRUE(coordinator->handleCoordinate(1.0, 1.0, 100.0));
  EXPECT_TRUE(coordinator->isPending());
  EXPECT_EQ(drafts.size(), 3);

  // Click close to first point (within 500m)
  EXPECT_TRUE(coordinator->handleCoordinate(0.0001, 0.0001, 100.0));
  EXPECT_FALSE(coordinator->isPending());
  EXPECT_EQ(state->areas().size(), 1);
  EXPECT_EQ(state->areas().first().areaType, QStringLiteral("Polygon"));
  EXPECT_EQ(state->areas().first().points.size(), 3);
}

TEST_F(GraphicPickCoordinatorTest, PolygonPickDoesNotCloseWithLessThan3Points) {
  coordinator->beginAreaPolygonPick(QStringLiteral("P2"), 500.0);

  // Only 2 points, then click near first — should NOT close
  EXPECT_TRUE(coordinator->handleCoordinate(0.0, 0.0, 100.0));
  EXPECT_TRUE(coordinator->handleCoordinate(0.001, 0.0, 100.0));
  // Click very near first point — but only 2 points so polygon check not reached
  EXPECT_TRUE(coordinator->handleCoordinate(0.00001, 0.00001, 100.0));
  EXPECT_TRUE(coordinator->isPending());  // still pending — only 3 area points now, needs closing click
  EXPECT_EQ(state->areas().size(), 0);
}

TEST_F(GraphicPickCoordinatorTest, WaypointAltitudeNeverNegative) {
  coordinator->beginWaypointPick(QStringLiteral("WPNeg"));
  // height = -100 → clamped to 0 via qMax(0, -100 + 50) = 0
  EXPECT_TRUE(coordinator->handleCoordinate(1.0, 1.0, -100.0));
  EXPECT_DOUBLE_EQ(state->waypoints().first().altitudeMeters, 0.0);
}
