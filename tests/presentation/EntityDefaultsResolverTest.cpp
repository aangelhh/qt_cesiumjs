#include <gtest/gtest.h>
#include "presentation/EntityDefaultsResolver.h"
#include "domain/Entity.h"

using namespace presentation;

// ─── summaryHasRunningTaskTargets ───────────────────────────────────────────

TEST(EntityDefaultsResolver, summaryHasRunningTaskTargets_trueWhenTaskRunning) {
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = true;
  m[QStringLiteral("taskStatus")]  = QStringLiteral("Running");
  m[QStringLiteral("taskType")]    = QStringLiteral("MoveToLocation");
  EXPECT_TRUE(summaryHasRunningTaskTargets(m));
}

TEST(EntityDefaultsResolver, summaryHasRunningTaskTargets_falseWhenNotEnabled) {
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = false;
  m[QStringLiteral("taskStatus")]  = QStringLiteral("Running");
  m[QStringLiteral("taskType")]    = QStringLiteral("MoveToLocation");
  EXPECT_FALSE(summaryHasRunningTaskTargets(m));
}

TEST(EntityDefaultsResolver, summaryHasRunningTaskTargets_falseWhenNotRunning) {
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = true;
  m[QStringLiteral("taskStatus")]  = QStringLiteral("Idle");
  m[QStringLiteral("taskType")]    = QStringLiteral("MoveToLocation");
  EXPECT_FALSE(summaryHasRunningTaskTargets(m));
}

TEST(EntityDefaultsResolver, summaryHasRunningTaskTargets_falseWhenEmptyType) {
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = true;
  m[QStringLiteral("taskStatus")]  = QStringLiteral("Running");
  m[QStringLiteral("taskType")]    = QStringLiteral("   "); // whitespace only
  EXPECT_FALSE(summaryHasRunningTaskTargets(m));
}

// ─── resolveFlyTargetsFromSummary ────────────────────────────────────────────

TEST(EntityDefaultsResolver, resolveFlyTargets_emptyMap_returnsFalse) {
  double h; int a; double s;
  EXPECT_FALSE(resolveFlyTargetsFromSummary({}, 5000, h, a, s));
}

TEST(EntityDefaultsResolver, resolveFlyTargets_noRunningTask_usesCurrentTelemetry) {
  QVariantMap m;
  m[QStringLiteral("taskEnabled")]  = false;
  m[QStringLiteral("headingDegrees")] = 90.0;
  m[QStringLiteral("speedKnots")]     = 250.0;

  double h; int a; double s;
  EXPECT_TRUE(resolveFlyTargetsFromSummary(m, 8000, h, a, s));
  EXPECT_DOUBLE_EQ(h, 90.0);
  EXPECT_EQ(a, 8000);
  EXPECT_DOUBLE_EQ(s, 250.0);
}

TEST(EntityDefaultsResolver, resolveFlyTargets_runningTask_usesTaskTargets) {
  QVariantMap m;
  m[QStringLiteral("taskEnabled")]               = true;
  m[QStringLiteral("taskStatus")]                = QStringLiteral("Running");
  m[QStringLiteral("taskType")]                  = QStringLiteral("FlyHeadingAltitudeSpeed");
  m[QStringLiteral("taskTargetHeadingDegrees")]  = 270.0;
  m[QStringLiteral("taskTargetAltitudeMeters")]  = 12000;
  m[QStringLiteral("taskTargetSpeedKnots")]      = 400.0;
  m[QStringLiteral("headingDegrees")]            = 45.0;  // ignored
  m[QStringLiteral("speedKnots")]                = 100.0; // ignored

  double h; int a; double s;
  EXPECT_TRUE(resolveFlyTargetsFromSummary(m, 5000, h, a, s));
  EXPECT_DOUBLE_EQ(h, 270.0);
  EXPECT_EQ(a, 12000);
  EXPECT_DOUBLE_EQ(s, 400.0);
}

// ─── resolvePlanStepDefaults ─────────────────────────────────────────────────

namespace {
Entity makeAirEntity(double heading, int altitude, double speed) {
  Entity e;
  e.headingDegrees = heading;
  e.altitude       = altitude;
  e.speedKnots     = speed;
  e.domain         = QStringLiteral("Air");
  return e;
}
Entity makeSurfaceEntity(double heading, int altitude, double speed) {
  Entity e;
  e.headingDegrees = heading;
  e.altitude       = altitude;
  e.speedKnots     = speed;
  e.domain         = QStringLiteral("Surface");
  return e;
}
}

TEST(EntityDefaultsResolver, planStepDefaults_noRunningTask_usesEntityTelemetry) {
  Entity e = makeAirEntity(45.0, 5000, 300.0);
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = false;

  double h; int a; double s;
  resolvePlanStepDefaults(e, m, h, a, s);
  EXPECT_DOUBLE_EQ(h, 45.0);
  EXPECT_EQ(a, 5000);
  EXPECT_DOUBLE_EQ(s, 300.0);
}

TEST(EntityDefaultsResolver, planStepDefaults_runningTask_usesTaskTargets) {
  Entity e = makeAirEntity(45.0, 5000, 300.0);
  QVariantMap m;
  m[QStringLiteral("taskEnabled")]              = true;
  m[QStringLiteral("taskStatus")]               = QStringLiteral("Running");
  m[QStringLiteral("taskType")]                 = QStringLiteral("MoveToLocation");
  m[QStringLiteral("taskTargetHeadingDegrees")] = 180.0;
  m[QStringLiteral("taskTargetAltitudeMeters")] = 10000;
  m[QStringLiteral("taskTargetSpeedKnots")]     = 500.0;

  double h; int a; double s;
  resolvePlanStepDefaults(e, m, h, a, s);
  EXPECT_DOUBLE_EQ(h, 180.0);
  EXPECT_EQ(a, 10000);
  EXPECT_DOUBLE_EQ(s, 500.0);
}

TEST(EntityDefaultsResolver, planStepDefaults_zeroSpeed_airUsesDefault220) {
  Entity e = makeAirEntity(0.0, 5000, 0.0);
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = false;

  double h; int a; double s;
  resolvePlanStepDefaults(e, m, h, a, s);
  EXPECT_DOUBLE_EQ(s, 220.0);
}

TEST(EntityDefaultsResolver, planStepDefaults_zeroSpeed_surfaceUsesDefault12) {
  Entity e = makeSurfaceEntity(0.0, 0, 0.0);
  QVariantMap m;
  m[QStringLiteral("taskEnabled")] = false;

  double h; int a; double s;
  resolvePlanStepDefaults(e, m, h, a, s);
  EXPECT_DOUBLE_EQ(s, 12.0);
}
