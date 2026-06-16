#include <gtest/gtest.h>
#include "presentation/AssignTaskController.h"
#include "domain/Task.h"

using namespace presentation;

namespace {

struct Fixture {
  QString selectedEntity;
  bool operable        = true;
  bool planRunning     = false;
  QVariantMap summary;
  bool captureAccepted = true;
  EntityTask capturedOut;
  QString appliedEntity;
  EntityTask appliedTask;
  QString lastStatus;

  AssignTaskController* makeCtrl() {
    return new AssignTaskController(
        [this]()              { return selectedEntity; },
        [this]()              { return operable; },
        [this](const QString&){ return planRunning; },
        [this]()              { return summary; },
        [this](const QString&, const EntityTask&, const QString&, EntityTask& out) {
          out = capturedOut;
          return captureAccepted;
        },
        [this](const QString& name, const EntityTask& task) {
          appliedEntity = name;
          appliedTask   = task;
          return true;
        },
        [this](const QString& msg) { lastStatus = msg; },
        nullptr);
  }
};

} // namespace

// ─── guards ─────────────────────────────────────────────────────────────────

TEST(AssignTaskController, open_emptyEntity_doesNothing) {
  Fixture f;
  f.selectedEntity = QString{};
  auto* ctrl = f.makeCtrl();
  ctrl->open(QStringLiteral("FlyHeadingAltitudeSpeed"));
  EXPECT_TRUE(f.appliedEntity.isEmpty());
  EXPECT_TRUE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(AssignTaskController, open_notOperable_setsStatus) {
  Fixture f;
  f.selectedEntity = QStringLiteral("Alpha");
  f.operable = false;
  auto* ctrl = f.makeCtrl();
  ctrl->open(QStringLiteral("MoveToLocation"));
  EXPECT_TRUE(f.appliedEntity.isEmpty());
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(AssignTaskController, open_planRunning_setsStatus) {
  Fixture f;
  f.selectedEntity = QStringLiteral("Bravo");
  f.operable    = true;
  f.planRunning = true;
  auto* ctrl = f.makeCtrl();
  ctrl->open(QStringLiteral("PatrolArea"));
  EXPECT_TRUE(f.appliedEntity.isEmpty());
  EXPECT_FALSE(f.lastStatus.isEmpty());
  delete ctrl;
}

TEST(AssignTaskController, open_captureRejected_doesNotApply) {
  Fixture f;
  f.selectedEntity  = QStringLiteral("Charlie");
  f.captureAccepted = false;
  auto* ctrl = f.makeCtrl();
  ctrl->open(QStringLiteral("AttackSurface"));
  EXPECT_TRUE(f.appliedEntity.isEmpty());
  delete ctrl;
}

// ─── happy path ─────────────────────────────────────────────────────────────

TEST(AssignTaskController, open_allGuardsPassed_appliesTask) {
  Fixture f;
  f.selectedEntity  = QStringLiteral("Delta");
  f.captureAccepted = true;
  f.capturedOut.taskType = QStringLiteral("MoveToLocation");
  f.capturedOut.targetLatitude  = 10.0;
  f.capturedOut.targetLongitude = 20.0;
  auto* ctrl = f.makeCtrl();
  ctrl->open(QStringLiteral("MoveToLocation"));
  EXPECT_EQ(f.appliedEntity, QStringLiteral("Delta"));
  EXPECT_EQ(f.appliedTask.taskType, QStringLiteral("MoveToLocation"));
  delete ctrl;
}

// ─── taskFromSummary ────────────────────────────────────────────────────────

TEST(AssignTaskController, taskFromSummary_extractsAllFields) {
  QVariantMap m;
  m[QStringLiteral("taskType")]                    = QStringLiteral("AttackAir");
  m[QStringLiteral("taskEnabled")]                 = true;
  m[QStringLiteral("taskStatus")]                  = QStringLiteral("Active");
  m[QStringLiteral("taskTargetHeadingDegrees")]    = 90.0;
  m[QStringLiteral("taskTargetAltitudeMeters")]    = 5000;
  m[QStringLiteral("taskTargetSpeedKnots")]        = 450.0;
  m[QStringLiteral("taskTargetLatitude")]          = 37.5;
  m[QStringLiteral("taskTargetLongitude")]         = -8.2;
  m[QStringLiteral("taskTargetEntityName")]        = QStringLiteral("Tgt1");
  m[QStringLiteral("taskTargetWaypointName")]      = QStringLiteral("WP2");
  m[QStringLiteral("taskTargetRouteName")]         = QStringLiteral("Rte3");
  m[QStringLiteral("taskTargetAreaName")]          = QStringLiteral("Zone4");
  m[QStringLiteral("taskTargetAreaRadiusMeters")]  = 1500.0;
  m[QStringLiteral("taskMaxEngagementTimeSeconds")] = 180.0;
  m[QStringLiteral("taskShotCooldownSeconds")]     = 5.0;

  const EntityTask task = AssignTaskController::taskFromSummary(m);

  EXPECT_EQ(task.taskType,               QStringLiteral("AttackAir"));
  EXPECT_TRUE(task.enabled);
  EXPECT_EQ(task.status,                 QStringLiteral("Active"));
  EXPECT_DOUBLE_EQ(task.targetHeadingDegrees,  90.0);
  EXPECT_EQ(task.targetAltitudeMeters,   5000);
  EXPECT_DOUBLE_EQ(task.targetSpeedKnots,     450.0);
  EXPECT_DOUBLE_EQ(task.targetLatitude,       37.5);
  EXPECT_DOUBLE_EQ(task.targetLongitude,      -8.2);
  EXPECT_EQ(task.targetEntityName,       QStringLiteral("Tgt1"));
  EXPECT_EQ(task.targetWaypointName,     QStringLiteral("WP2"));
  EXPECT_EQ(task.targetRouteName,        QStringLiteral("Rte3"));
  EXPECT_EQ(task.targetAreaName,         QStringLiteral("Zone4"));
  EXPECT_DOUBLE_EQ(task.targetAreaRadiusMeters, 1500.0);
  EXPECT_DOUBLE_EQ(task.maxEngagementTimeSeconds, 180.0);
  EXPECT_DOUBLE_EQ(task.shotCooldownSeconds, 5.0);
}

TEST(AssignTaskController, taskFromSummary_emptyMap_returnsDefaults) {
  const EntityTask task = AssignTaskController::taskFromSummary({});
  EXPECT_TRUE(task.taskType.isEmpty());
  EXPECT_FALSE(task.enabled);
  EXPECT_DOUBLE_EQ(task.targetLatitude, 0.0);
}
