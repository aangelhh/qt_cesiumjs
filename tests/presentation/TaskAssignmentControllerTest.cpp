#include <gtest/gtest.h>
#include "application/ScenarioState.h"
#include "application/ScenarioQueries.h"
#include "presentation/TaskAssignmentController.h"
#include "presentation/EntityHomePositionTracker.h"

using namespace presentation;

namespace {

struct Fixture {
  ScenarioState             state;
  EntityHomePositionTracker homeTracker;
  QString   lastStatus;
  QString   openedDialog;
  QString   selectedName;
  bool      isOperable     = true;
  bool      flyTargetsOk   = true;
  double    flyHeading     = 45.0;
  int       flyAlt         = 3000;
  double    flySpeed       = 250.0;
  QString   pickItemReturn;
  bool      pickItemOk     = true;
  double    askDoubleReturn = 0.0;
  bool      askDoubleOk    = true;
  QString   lastAppliedEntity;
  EntityTask lastAppliedTask;
  bool      applyTaskResult = true;
  QVariantMap summaryMap;

  TaskAssignmentController* makeController() {
    return new TaskAssignmentController(
        [this]() { return selectedName; },
        [this]() { return isOperable; },
        [this]() { return summaryMap; },
        [this](const QString& name) {
          return homeTracker.positionFor(name);
        },
        [this](bool requirePoints) -> QStringList {
          return application::availableRouteNames(&state, requirePoints);
        },
        [this](double& h, int& a, double& s) -> bool {
          h = flyHeading; a = flyAlt; s = flySpeed;
          return flyTargetsOk;
        },
        [this](const QString& name, const EntityTask& task) -> bool {
          lastAppliedEntity = name;
          lastAppliedTask   = task;
          return applyTaskResult;
        },
        [this](const QString& taskType) { openedDialog = taskType; },
        [this](const QString&, const QString&,
               const QStringList&, int, bool& ok) -> QString {
          ok = pickItemOk;
          return pickItemReturn;
        },
        [this](const QString&, const QString&,
               double, double, double, int, bool& ok) -> double {
          ok = askDoubleOk;
          return askDoubleReturn;
        },
        [this](const QString& msg) { lastStatus = msg; });
  }

  void addRoute(const QString& routeName) {
    RouteGraphic r;
    r.name = routeName;
    r.points.append({10.0, 20.0, 100.0});
    state.addRoute(r);
  }

  Entity addEntity(const QString& name) {
    Entity e;
    e.name   = name;
    e.domain = QStringLiteral("Air");
    e.category = QStringLiteral("Fighter");
    e.forceIdentifier = 1;
    state.addEntity(e);
    return e;
  }
};

} // namespace

// ── Dialog-based 1-liners ────────────────────────────────────────────────────

TEST(TaskAssignmentController, assignFlyHeadingOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignFlyHeadingAltitudeSpeed();
  EXPECT_EQ(f.openedDialog, QStringLiteral("FlyHeadingAltitudeSpeed"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignMoveToLocationOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignMoveToLocation();
  EXPECT_EQ(f.openedDialog, QStringLiteral("MoveToLocation"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignWaitOnLocationOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignWaitOnLocation();
  EXPECT_EQ(f.openedDialog, QStringLiteral("WaitOnLocation"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignMoveToWaypointOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignMoveToWaypoint();
  EXPECT_EQ(f.openedDialog, QStringLiteral("MoveToWaypoint"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignPatrolAreaOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignPatrolArea();
  EXPECT_EQ(f.openedDialog, QStringLiteral("PatrolArea"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignHoldRacetrackOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignHoldRacetrack();
  EXPECT_EQ(f.openedDialog, QStringLiteral("HoldRacetrack"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignAttackOnceOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignAttackOnce();
  EXPECT_EQ(f.openedDialog, QStringLiteral("AttackOnce"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignAttackAirOpensDlg) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignAttackAir();
  EXPECT_EQ(f.openedDialog, QStringLiteral("AttackAir"));
  delete ctrl;
}

TEST(TaskAssignmentController, assignInterceptEntityOpensUnifiedDialog) {
  Fixture f;
  auto* ctrl = f.makeController();
  ctrl->assignInterceptEntity();
  EXPECT_EQ(f.openedDialog, QStringLiteral("InterceptEntity"));
  delete ctrl;
}

TEST(TaskAssignmentController, legacyInterceptActionsOpenUnifiedDialog) {
  Fixture f;
  auto* ctrl = f.makeController();

  ctrl->assignInterceptEntity2D();
  EXPECT_EQ(f.openedDialog, QStringLiteral("InterceptEntity"));

  f.openedDialog.clear();
  ctrl->assignInterceptEntity3D();
  EXPECT_EQ(f.openedDialog, QStringLiteral("InterceptEntity"));
  delete ctrl;
}

// ── assignReturnToBase ───────────────────────────────────────────────────────

TEST(TaskAssignmentController, rtbUsesCurrentPositionWhenNoHome) {
  Fixture f;
  f.addEntity(QStringLiteral("Alpha"));
  f.selectedName = QStringLiteral("Alpha");
  f.summaryMap[QStringLiteral("latitude")]  = 12.0;
  f.summaryMap[QStringLiteral("longitude")] = 34.0;
  f.summaryMap[QStringLiteral("speedKnots")] = 200.0;
  auto* ctrl = f.makeController();

  ctrl->assignReturnToBase();

  EXPECT_EQ(f.lastAppliedEntity, QStringLiteral("Alpha"));
  EXPECT_EQ(f.lastAppliedTask.taskType, QStringLiteral("MoveToLocation"));
  EXPECT_DOUBLE_EQ(f.lastAppliedTask.targetLatitude,  12.0);
  EXPECT_DOUBLE_EQ(f.lastAppliedTask.targetLongitude, 34.0);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(TaskAssignmentController, rtbUsesHomePositionWhenAvailable) {
  Fixture f;
  f.addEntity(QStringLiteral("Bravo"));
  f.selectedName = QStringLiteral("Bravo");
  f.summaryMap[QStringLiteral("latitude")]  = 0.0;
  f.summaryMap[QStringLiteral("longitude")] = 0.0;
  f.summaryMap[QStringLiteral("speedKnots")] = 100.0;
  // register a home position via Entity
  Entity homeEntity;
  homeEntity.name = QStringLiteral("Bravo");
  homeEntity.latitude = 55.5; homeEntity.longitude = 37.6; homeEntity.altitude = 200;
  f.homeTracker.remember(homeEntity);
  auto* ctrl = f.makeController();

  ctrl->assignReturnToBase();

  EXPECT_DOUBLE_EQ(f.lastAppliedTask.targetLatitude,  55.5);
  EXPECT_DOUBLE_EQ(f.lastAppliedTask.targetLongitude, 37.6);
  EXPECT_EQ(f.lastAppliedTask.targetAltitudeMeters,   200);
  delete ctrl;
}

TEST(TaskAssignmentController, rtbDoesNothingWithoutSelection) {
  Fixture f;
  f.selectedName = QString{};
  auto* ctrl = f.makeController();
  ctrl->assignReturnToBase();
  EXPECT_TRUE(f.lastAppliedEntity.isEmpty());
  delete ctrl;
}

// ── assignPatrolRoute ────────────────────────────────────────────────────────

TEST(TaskAssignmentController, patrolRouteAssignedSuccessfully) {
  Fixture f;
  f.addEntity(QStringLiteral("Charlie"));
  f.selectedName    = QStringLiteral("Charlie");
  f.addRoute(QStringLiteral("Route1"));
  f.pickItemReturn  = QStringLiteral("Route1");
  f.pickItemOk      = true;
  f.summaryMap[QStringLiteral("speedKnots")] = 300.0;
  auto* ctrl = f.makeController();

  ctrl->assignPatrolRoute();

  EXPECT_EQ(f.lastAppliedTask.taskType,        QStringLiteral("FollowRoute"));
  EXPECT_EQ(f.lastAppliedTask.targetRouteName, QStringLiteral("Route1"));
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(TaskAssignmentController, patrolRouteAbortedWhenUserCancels) {
  Fixture f;
  f.addEntity(QStringLiteral("Delta"));
  f.selectedName   = QStringLiteral("Delta");
  f.addRoute(QStringLiteral("RouteX"));
  f.pickItemOk     = false;
  auto* ctrl = f.makeController();

  ctrl->assignPatrolRoute();

  EXPECT_TRUE(f.lastAppliedEntity.isEmpty());
  delete ctrl;
}

TEST(TaskAssignmentController, patrolRouteNoRoutesAvailable) {
  Fixture f;
  f.addEntity(QStringLiteral("Echo"));
  f.selectedName = QStringLiteral("Echo");
  // no routes added
  auto* ctrl = f.makeController();

  ctrl->assignPatrolRoute();

  EXPECT_TRUE(f.lastAppliedEntity.isEmpty());
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

// ── assignOrbitHoldLocation ──────────────────────────────────────────────────

TEST(TaskAssignmentController, orbitHoldCurrentPosition) {
  Fixture f;
  f.addEntity(QStringLiteral("Fox"));
  f.selectedName = QStringLiteral("Fox");
  f.summaryMap[QStringLiteral("latitude")]   = 40.0;
  f.summaryMap[QStringLiteral("longitude")]  = -3.0;
  f.summaryMap[QStringLiteral("speedKnots")] = 200.0;
  // pickItem returns "Current Position" on first call
  f.pickItemReturn = QStringLiteral("Current Position");
  f.pickItemOk     = true;
  auto* ctrl = f.makeController();

  ctrl->assignOrbitHoldLocation();

  EXPECT_EQ(f.lastAppliedTask.taskType, QStringLiteral("OrbitArea"));
  EXPECT_DOUBLE_EQ(f.lastAppliedTask.targetLatitude,  40.0);
  EXPECT_DOUBLE_EQ(f.lastAppliedTask.targetLongitude, -3.0);
  EXPECT_GT(f.lastAppliedTask.targetAreaRadiusMeters, 0.0);
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(TaskAssignmentController, orbitHoldCancelledByUser) {
  Fixture f;
  f.addEntity(QStringLiteral("Golf"));
  f.selectedName = QStringLiteral("Golf");
  f.pickItemOk   = false;
  auto* ctrl = f.makeController();

  ctrl->assignOrbitHoldLocation();

  EXPECT_TRUE(f.lastAppliedEntity.isEmpty());
  delete ctrl;
}
