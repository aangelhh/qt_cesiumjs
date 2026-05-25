#include <gtest/gtest.h>
#include "../../src/domain/BombReleaseGate.h"
#include "../../src/domain/Entity.h"

using namespace domain;

static Entity makeLauncher(double lat, double lon, int alt,
                           double heading, double speed,
                           double pitch = 0.0, double vspeed = 0.0) {
  Entity e;
  e.latitude = lat;
  e.longitude = lon;
  e.altitude = alt;
  e.headingDegrees = heading;
  e.speedKnots = speed;
  e.pitchDegrees = pitch;
  e.verticalSpeedMetersPerSecond = vspeed;
  return e;
}

TEST(BombReleaseGate, NotValidIfOnGround) {
  Entity launcher = makeLauncher(40.0, -3.0, 0, 0.0, 200.0);
  auto result = evaluateBombReleaseGate(launcher, 40.01, -3.0, 0.0);
  EXPECT_FALSE(result.valid);
}

TEST(BombReleaseGate, NotValidIfTooSlow) {
  Entity launcher = makeLauncher(40.0, -3.0, 3000, 0.0, 0.5);
  auto result = evaluateBombReleaseGate(launcher, 40.01, -3.0, 0.0);
  EXPECT_FALSE(result.valid);
}

TEST(BombReleaseGate, ValidWhenAboveAndMoving) {
  Entity launcher = makeLauncher(40.0, -3.0, 3000, 0.0, 300.0);
  auto result = evaluateBombReleaseGate(launcher, 40.01, -3.0, 0.0);
  EXPECT_TRUE(result.valid);
}

TEST(BombReleaseGate, TargetBehindNotAhead) {
  // Target behind the launcher (heading north, target to south)
  Entity launcher = makeLauncher(40.0, -3.0, 3000, 0.0, 300.0);
  auto result = evaluateBombReleaseGate(launcher, 39.9, -3.0, 0.0);
  EXPECT_TRUE(result.valid);
  EXPECT_FALSE(result.targetAhead);
}

TEST(BombReleaseGate, ReadyToReleaseRequiresAll) {
  BombReleaseGateEvaluation eval;
  eval.valid = true;
  eval.targetAhead = true;
  eval.withinHeadingCone = true;
  eval.withinReleaseWindow = false;
  EXPECT_FALSE(eval.readyToRelease());

  eval.withinReleaseWindow = true;
  EXPECT_TRUE(eval.readyToRelease());
}

TEST(BombReleaseGate, StateLabelValues) {
  BombReleaseGateEvaluation eval;
  EXPECT_EQ(eval.stateLabel(), QStringLiteral("Armed"));
  eval.valid = true;
  eval.targetAhead = true;
  eval.withinHeadingCone = true;
  eval.withinReleaseWindow = true;
  EXPECT_EQ(eval.stateLabel(), QStringLiteral("In Release Window"));
}

TEST(WeaponQuantity, FindsExistingWeapon) {
  Entity e;
  WeaponInventoryItem bomb;
  bomb.weaponType = QStringLiteral("Bomb");
  bomb.quantity = 4;
  e.weapons.append(bomb);
  EXPECT_EQ(domain::weaponQuantity(e, QStringLiteral("Bomb")), 4);
  EXPECT_EQ(domain::weaponQuantity(e, QStringLiteral("Missile")), 0);
}

TEST(WeaponQuantity, CaseInsensitive) {
  Entity e;
  WeaponInventoryItem m;
  m.weaponType = QStringLiteral("Missile");
  m.quantity = 2;
  e.weapons.append(m);
  EXPECT_EQ(domain::weaponQuantity(e, QStringLiteral("missile")), 2);
}
