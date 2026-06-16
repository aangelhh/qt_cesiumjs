#include <gtest/gtest.h>
#include "presentation/PlanStepConfigurator.h"
#include "presentation/EntityPlanExecutor.h"
#include "presentation/EntityHomePositionTracker.h"
#include "domain/Entity.h"
#include "domain/GeoMath.h"

#include <QCoreApplication>

namespace {

// Default stubs — typed to match PlanStepConfigurator callback signatures
presentation::PlanStepConfigurator::CaptureFn alwaysAcceptCapture =
    [](const QString&, const EntityTask& init, const QString&, EntityTask& out) -> bool {
  out = init;
  return true;
};
presentation::PlanStepConfigurator::CaptureFn alwaysCancelCapture =
    [](const QString&, const EntityTask&, const QString&, EntityTask&) -> bool {
  return false;
};
presentation::PlanStepConfigurator::FindAreaFn noArea =
    [](const QString&) -> const AreaDefinition* { return nullptr; };
presentation::PlanStepConfigurator::HomePosFn noHome =
    [](const QString&) -> presentation::EntityHomePosition {
  return {0.0, 0.0, 0, false};
};
presentation::PlanStepConfigurator::AskItemFn acceptItem =
    [](const QString&, const QString&, const QStringList& items, bool& ok) -> QString {
  ok = true;
  return items.isEmpty() ? QString() : items.first();
};
presentation::PlanStepConfigurator::AskItemFn cancelItem =
    [](const QString&, const QString&, const QStringList&, bool& ok) -> QString {
  ok = false;
  return {};
};
presentation::PlanStepConfigurator::AskDoubleFn acceptDouble =
    [](const QString&, const QString&, double def, double, double, bool& ok) -> double {
  ok = true;
  return def;
};

Entity makeAirEntity(const QString& name) {
  Entity e;
  e.name         = name;
  e.category     = QStringLiteral("Air");
  e.domain       = QStringLiteral("Air");
  e.latitude     = 40.0;
  e.longitude    = -3.0;
  e.altitude     = 3000;
  e.speedKnots   = 300.0;
  e.headingDegrees = 90.0;
  return e;
}

class PlanStepConfiguratorTest : public ::testing::Test {};

// ── Cancellation ─────────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, MoveToLocation_CancelReturnsFalse) {
  presentation::PlanStepConfigurator cfg(
      alwaysCancelCapture, noArea, noHome, acceptItem, acceptDouble);
  Entity entity = makeAirEntity("Alpha");
  PlanStep step;
  EXPECT_FALSE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::MoveToLocation, step));
}

TEST_F(PlanStepConfiguratorTest, OrbitHoldLocation_CancelItemReturnsFalse) {
  presentation::PlanStepConfigurator cfg(
      alwaysAcceptCapture, noArea, noHome, cancelItem, acceptDouble);
  Entity entity = makeAirEntity("Beta");
  PlanStep step;
  EXPECT_FALSE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::OrbitHoldLocation, step));
}

// ── MoveToLocation ────────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, MoveToLocation_Label) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString&, EntityTask& out) {
        out = init;
        out.targetLatitude  = 41.5;
        out.targetLongitude = -4.2;
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Gamma");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::MoveToLocation, step));
  EXPECT_EQ(step.kind, PlanStepKind::MoveToLocation);
  EXPECT_TRUE(step.label.startsWith(QStringLiteral("Move To")));
  EXPECT_TRUE(step.task.enabled);
}

// ── MoveToWaypoint ────────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, MoveToWaypoint_Label) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString&, EntityTask& out) {
        out = init;
        out.targetWaypointName = QStringLiteral("WP1");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Delta");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::MoveToWaypoint, step));
  EXPECT_EQ(step.label, QStringLiteral("Move To Waypoint: WP1"));
}

// ── FollowRoute ───────────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, FollowRoute_LabelAndDefaults) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString& initialType, EntityTask& out) {
        EXPECT_EQ(initialType, QStringLiteral("FollowRoute"));
        out = init;
        out.targetRouteName = QStringLiteral("Route1");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("RouteRunner");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::FollowRoute, step));
  EXPECT_EQ(step.kind, PlanStepKind::FollowRoute);
  EXPECT_EQ(step.task.taskType, QStringLiteral("FollowRoute"));
  EXPECT_DOUBLE_EQ(step.task.arrivalToleranceMeters, 1000.0);
  EXPECT_EQ(step.label, QStringLiteral("Follow Route: Route1"));
}

// ── FlyHeadingAltitudeSpeed ───────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, FlyHeadingAltitudeSpeed_Label) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString&, EntityTask& out) {
        out = init;
        out.targetHeadingDegrees = 270.0;
        out.targetAltitudeMeters = 5000;
        out.targetSpeedKnots     = 450.0;
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Epsilon");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 90.0, 3000, 300.0, PlanStepKind::FlyHeadingAltitudeSpeed, step));
  EXPECT_TRUE(step.label.contains(QStringLiteral("270")));
  EXPECT_TRUE(step.label.contains(QStringLiteral("5000")));
  EXPECT_TRUE(step.label.contains(QStringLiteral("450")));
}

// ── ReturnToBase (no recorded home) ──────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, ReturnToBase_NoHome_FallsBackToEntityPos) {
  presentation::PlanStepConfigurator cfg(
      alwaysAcceptCapture, noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Zeta");
  entity.latitude  = 38.7;
  entity.longitude = -9.1;
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::ReturnToBase, step));
  EXPECT_EQ(step.label, QStringLiteral("Return To Base"));
  EXPECT_DOUBLE_EQ(step.task.targetLatitude,  38.7);
  EXPECT_DOUBLE_EQ(step.task.targetLongitude, -9.1);
}

// ── ReturnToBase (with home position) ────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, ReturnToBase_WithHome_UsesHomePos) {
  auto withHome = [](const QString&) -> presentation::EntityHomePosition {
    return {50.0, 8.0, 100, true};
  };
  presentation::PlanStepConfigurator cfg(
      alwaysAcceptCapture, noArea, withHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Eta");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::ReturnToBase, step));
  EXPECT_DOUBLE_EQ(step.task.targetLatitude,  50.0);
  EXPECT_DOUBLE_EQ(step.task.targetLongitude, 8.0);
  EXPECT_EQ(step.task.targetAltitudeMeters, 100);
}

// ── OrbitHoldLocation ─────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, OrbitHoldLocation_CurrentPosition_UsesEntityPos) {
  presentation::PlanStepConfigurator cfg(
      alwaysAcceptCapture, noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Theta");
  entity.latitude  = 48.8;
  entity.longitude = 2.3;
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::OrbitHoldLocation, step));
  EXPECT_EQ(step.task.taskType, QStringLiteral("OrbitArea"));
  EXPECT_DOUBLE_EQ(step.task.targetLatitude,  48.8);
  EXPECT_DOUBLE_EQ(step.task.targetLongitude, 2.3);
  EXPECT_GT(step.task.targetAreaRadiusMeters, 0.0);
}

TEST_F(PlanStepConfiguratorTest, HoldRacetrack_UsesDialogAndDefaults) {
  Entity entity = makeAirEntity(QStringLiteral("F1"));
  presentation::PlanStepConfigurator::CaptureFn capture =
      [](const QString&, const EntityTask& initial, const QString& type, EntityTask& out) {
    EXPECT_EQ(type, QStringLiteral("HoldRacetrack"));
    out = initial;
    out.racetrackLegLengthMeters = 12000.0;
    out.durationSeconds = 300.0;
    return true;
  };
  presentation::PlanStepConfigurator cfg(capture, noArea, noHome, acceptItem, acceptDouble);

  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 45.0, 3000, 300.0, PlanStepKind::HoldRacetrack, step));

  EXPECT_EQ(step.kind, PlanStepKind::HoldRacetrack);
  EXPECT_EQ(step.task.taskType, QStringLiteral("HoldRacetrack"));
  EXPECT_DOUBLE_EQ(step.task.targetLatitude, entity.latitude);
  EXPECT_DOUBLE_EQ(step.task.targetLongitude, entity.longitude);
  EXPECT_DOUBLE_EQ(step.task.targetHeadingDegrees, 45.0);
  EXPECT_DOUBLE_EQ(step.task.racetrackLegLengthMeters, 12000.0);
  EXPECT_DOUBLE_EQ(step.task.durationSeconds, 300.0);
  EXPECT_TRUE(step.label.startsWith(QStringLiteral("Hold Racetrack")));
}

TEST_F(PlanStepConfiguratorTest, WaitOnLocation_UsesDialogAndDefaults) {
  Entity entity = makeAirEntity(QStringLiteral("F1"));
  presentation::PlanStepConfigurator::CaptureFn capture =
      [](const QString&, const EntityTask& initial, const QString& type, EntityTask& out) {
    EXPECT_EQ(type, QStringLiteral("WaitOnLocation"));
    EXPECT_EQ(initial.taskType, QStringLiteral("WaitOnLocation"));
    EXPECT_DOUBLE_EQ(initial.arrivalToleranceMeters, 200.0);
    out = initial;
    out.durationSeconds = 180.0;
    return true;
  };
  presentation::PlanStepConfigurator cfg(capture, noArea, noHome, acceptItem, acceptDouble);

  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 45.0, 3000, 300.0, PlanStepKind::WaitOnLocation, step));

  EXPECT_EQ(step.kind, PlanStepKind::WaitOnLocation);
  EXPECT_EQ(step.task.taskType, QStringLiteral("WaitOnLocation"));
  EXPECT_DOUBLE_EQ(step.task.targetLatitude, entity.latitude);
  EXPECT_DOUBLE_EQ(step.task.targetLongitude, entity.longitude);
  EXPECT_DOUBLE_EQ(step.task.durationSeconds, 180.0);
  EXPECT_TRUE(step.label.startsWith(QStringLiteral("Wait on Location")));
}

// ── PatrolArea (area not found) ───────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, PatrolArea_AreaNotFound_StillSucceeds) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString&, EntityTask& out) {
        out = init;
        out.targetAreaName = QStringLiteral("ZoneA");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Iota");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::PatrolArea, step));
  EXPECT_EQ(step.label, QStringLiteral("Patrol Area: ZoneA"));
}

// ── AttackAir ─────────────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, AttackOnce_LabelAndDefaults) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString& initialType, EntityTask& out) {
        EXPECT_EQ(initialType, QStringLiteral("AttackOnce"));
        EXPECT_EQ(init.taskType, QStringLiteral("AttackOnce"));
        EXPECT_EQ(init.weaponType, QStringLiteral("Auto"));
        EXPECT_DOUBLE_EQ(init.timeoutSeconds, 120.0);
        out = init;
        out.targetEntityName = QStringLiteral("Target1");
        out.weaponType = QStringLiteral("Missile");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Kappa");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::AttackOnce, step));
  EXPECT_EQ(step.kind, PlanStepKind::AttackOnce);
  EXPECT_EQ(step.task.taskType, QStringLiteral("AttackOnce"));
  EXPECT_EQ(step.task.weaponType, QStringLiteral("Missile"));
  EXPECT_EQ(step.label, QStringLiteral("Attack Once: Target1"));
}

TEST_F(PlanStepConfiguratorTest, AttackAir_Label) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString&, EntityTask& out) {
        out = init;
        out.targetEntityName = QStringLiteral("Target1");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Kappa");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::AttackAir, step));
  EXPECT_EQ(step.label, QStringLiteral("Attack Air: Target1"));
}

// ── InterceptEntity ───────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, InterceptEntity_UsesUnified3DTaskAndLabel) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString& initialType, EntityTask& out) {
        EXPECT_EQ(initialType, QStringLiteral("InterceptEntity"));
        out = init;
        out.targetEntityName = QStringLiteral("Target1");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Interceptor");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::InterceptEntity, step));
  EXPECT_EQ(step.kind, PlanStepKind::InterceptEntity);
  EXPECT_EQ(step.task.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_DOUBLE_EQ(step.task.interceptDistanceMeters, 500.0);
  EXPECT_DOUBLE_EQ(step.task.altitudeToleranceMeters, 100.0);
  EXPECT_EQ(step.label, QStringLiteral("Intercept Entity: Target1"));
}

TEST_F(PlanStepConfiguratorTest, LegacyInterceptKindsConfigureAsUnifiedInterceptEntity) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString& initialType, EntityTask& out) {
        EXPECT_EQ(initialType, QStringLiteral("InterceptEntity"));
        out = init;
        out.targetEntityName = QStringLiteral("Target1");
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Legacy");
  PlanStep step2d;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::InterceptEntity2D, step2d));
  EXPECT_EQ(step2d.task.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_EQ(step2d.label, QStringLiteral("Intercept Entity: Target1"));

  PlanStep step3d;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::InterceptEntity3D, step3d));
  EXPECT_EQ(step3d.task.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_EQ(step3d.label, QStringLiteral("Intercept Entity: Target1"));
}

TEST_F(PlanStepConfiguratorTest, LegacyInterceptDisplayLabelsAreUnified) {
  PlanStep step2d;
  step2d.kind = PlanStepKind::InterceptEntity2D;
  step2d.label = QStringLiteral("Intercept Entity 2D: Target1");
  EXPECT_EQ(
      presentation::EntityPlanExecutor::planStepDisplayLabel(step2d),
      QStringLiteral("Intercept Entity: Target1"));

  PlanStep step3d;
  step3d.kind = PlanStepKind::InterceptEntity3D;
  step3d.label = QStringLiteral("Intercept Entity 3D: Target1");
  EXPECT_EQ(
      presentation::EntityPlanExecutor::planStepDisplayLabel(step3d),
      QStringLiteral("Intercept Entity: Target1"));
}

// ── AttackSurface ─────────────────────────────────────────────────────────

TEST_F(PlanStepConfiguratorTest, AttackSurface_ByCoordinate_Label) {
  presentation::PlanStepConfigurator cfg(
      [](const QString&, const EntityTask& init, const QString&, EntityTask& out) {
        out = init;
        out.targetEntityName.clear();
        out.targetLatitude  = 36.5;
        out.targetLongitude = -6.2;
        return true;
      },
      noArea, noHome, acceptItem, acceptDouble);

  Entity entity = makeAirEntity("Lambda");
  PlanStep step;
  EXPECT_TRUE(cfg.configure(entity, 0.0, 3000, 300.0, PlanStepKind::AttackSurface, step));
  EXPECT_TRUE(step.label.startsWith(QStringLiteral("Attack Surface:")));
}

} // namespace
