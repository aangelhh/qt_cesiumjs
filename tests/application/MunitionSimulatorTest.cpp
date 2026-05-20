#include <gtest/gtest.h>
#include "application/MunitionSimulator.h"
#include "domain/Entity.h"
#include "domain/Munition.h"

using namespace application;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Entity makeAirFighter(
    const QString& name,
    int force = 1,
    double lat = 0.0,
    double lon = 0.0,
    int alt = 5000) {
  Entity e;
  e.name = name;
  e.forceIdentifier = force;
  e.domain = QStringLiteral("Air");
  e.category = QStringLiteral("Fighter");
  e.latitude = lat;
  e.longitude = lon;
  e.altitude = alt;
  e.speedKnots = 300.0;
  e.pitchDegrees = 0.0;
  e.verticalSpeedMetersPerSecond = 0.0;
  e.headingDegrees = 0.0;
  e.destroyed = false;
  WeaponInventoryItem m;
  m.weaponType = QStringLiteral("Missile");
  m.quantity = 4;
  e.weapons.push_back(m);
  WeaponInventoryItem b;
  b.weaponType = QStringLiteral("Bomb");
  b.quantity = 2;
  e.weapons.push_back(b);
  return e;
}

static ActiveMunition makeTestMissile(
    const QString& id,
    double lat = 0.0,
    double lon = 0.0,
    double alt = 5000.0,
    double heading = 0.0) {
  ActiveMunition m;
  m.id = id;
  m.active = true;
  m.munitionType = QStringLiteral("Missile");
  m.forceIdentifier = 1;
  m.launcherEntityName = QStringLiteral("launcher");
  m.latitude = lat;
  m.longitude = lon;
  m.altitudeMeters = alt;
  m.headingDegrees = heading;
  m.pitchDegrees = 0.0;
  m.speedMetersPerSecond = 500.0;
  m.ttlSeconds = 12.0;
  m.hitRadiusMeters = 120.0;
  m.ageSeconds = 0.0;
  return m;
}

static ActiveMunition makeTestBomb(
    const QString& id,
    double lat = 0.0,
    double lon = 0.0,
    double alt = 5000.0) {
  ActiveMunition m;
  m.id = id;
  m.active = true;
  m.munitionType = QStringLiteral("Bomb");
  m.forceIdentifier = 1;
  m.launcherEntityName = QStringLiteral("launcher");
  m.latitude = lat;
  m.longitude = lon;
  m.altitudeMeters = alt;
  m.headingDegrees = 0.0;
  m.pitchDegrees = 0.0;
  m.speedMetersPerSecond = 150.0;
  m.verticalSpeedMetersPerSecond = 0.0;
  m.ttlSeconds = 45.0;
  m.hitRadiusMeters = 120.0;
  m.blastRadiusMeters = 200.0;
  m.baseDamage = 100.0;
  m.ageSeconds = 0.0;
  return m;
}

// ── munitionIsBomb ────────────────────────────────────────────────────────────

TEST(MunitionSimulator_munitionIsBomb, ReturnsTrueForBomb) {
  auto b = makeTestBomb(QStringLiteral("b1"));
  EXPECT_TRUE(munitionIsBomb(b));
}

TEST(MunitionSimulator_munitionIsBomb, ReturnsFalseForMissile) {
  auto m = makeTestMissile(QStringLiteral("m1"));
  EXPECT_FALSE(munitionIsBomb(m));
}

TEST(MunitionSimulator_munitionIsBomb, CaseInsensitive) {
  ActiveMunition m;
  m.munitionType = QStringLiteral("BOMB");
  EXPECT_TRUE(munitionIsBomb(m));
}

// ── makeMissileMunition ───────────────────────────────────────────────────────

TEST(MunitionSimulator_makeMissile, IdContainsEntityNameAndSerial) {
  const Entity e = makeAirFighter(QStringLiteral("Eagle-1"));
  const ActiveMunition m = makeMissileMunition(e, 7);
  EXPECT_TRUE(m.id.contains(QStringLiteral("Eagle-1")));
  EXPECT_TRUE(m.id.contains(QStringLiteral("7")));
}

TEST(MunitionSimulator_makeMissile, NotBomb) {
  const Entity e = makeAirFighter(QStringLiteral("E"));
  EXPECT_FALSE(munitionIsBomb(makeMissileMunition(e, 0)));
}

TEST(MunitionSimulator_makeMissile, SpeedIsEntitySpeedPlusBoost) {
  Entity e = makeAirFighter(QStringLiteral("E"));
  e.speedKnots = 0.0;
  const ActiveMunition m = makeMissileMunition(e, 0);
  EXPECT_GT(m.speedMetersPerSecond, 200.0); // boost alone is 250 m/s
}

TEST(MunitionSimulator_makeMissile, ForceAndLauncherPreserved) {
  Entity e = makeAirFighter(QStringLiteral("E"), 2);
  const ActiveMunition m = makeMissileMunition(e, 0);
  EXPECT_EQ(m.forceIdentifier, 2);
  EXPECT_EQ(m.launcherEntityName, QStringLiteral("E"));
}

// ── makeBombMunition ──────────────────────────────────────────────────────────

TEST(MunitionSimulator_makeBomb, IsBomb) {
  const Entity e = makeAirFighter(QStringLiteral("B"));
  EXPECT_TRUE(munitionIsBomb(makeBombMunition(e, 0)));
}

TEST(MunitionSimulator_makeBomb, AltitudeIsLessThanEntity) {
  Entity e = makeAirFighter(QStringLiteral("B"), 1, 0.0, 0.0, 6000);
  const ActiveMunition m = makeBombMunition(e, 0);
  EXPECT_LT(m.altitudeMeters, static_cast<double>(e.altitude));
}

// ── advanceActiveMunitions — empty / no-op ────────────────────────────────────

TEST(MunitionSimulator_advance, EmptyListIsNoOp) {
  QVector<ActiveMunition> munitions;
  QVector<TransientEffect> effects;
  QVector<Entity> entities;
  bool called = false;
  advanceActiveMunitions(munitions, effects, entities,
    [&](const QString&, double) { called = true; },
    [&](const ActiveMunition&) { called = true; },
    0.1);
  EXPECT_TRUE(munitions.isEmpty());
  EXPECT_FALSE(called);
}

TEST(MunitionSimulator_advance, ZeroDeltaIsNoOp) {
  QVector<ActiveMunition> munitions;
  munitions.push_back(makeTestMissile(QStringLiteral("m1")));
  QVector<TransientEffect> effects;
  QVector<Entity> entities;
  advanceActiveMunitions(munitions, effects, entities,
    [](const QString&, double) {},
    [](const ActiveMunition&) {},
    0.0);
  EXPECT_EQ(munitions.size(), 1); // still present
}

// ── advanceActiveMunitions — missile moves forward ───────────────────────────

TEST(MunitionSimulator_advance, MissileAdvancesPosition) {
  auto m = makeTestMissile(QStringLiteral("m1"), 0.0, 0.0, 5000.0, 0.0);
  QVector<ActiveMunition> munitions;
  munitions.push_back(m);
  QVector<TransientEffect> effects;
  QVector<Entity> entities;

  advanceActiveMunitions(munitions, effects, entities,
    [](const QString&, double) {},
    [](const ActiveMunition&) {},
    1.0);

  // Missile should have moved north (heading 0)
  ASSERT_EQ(munitions.size(), 1);
  EXPECT_GT(munitions.at(0).latitude, 0.0);
  EXPECT_NEAR(munitions.at(0).longitude, 0.0, 0.0001);
}

// ── advanceActiveMunitions — TTL expiry ──────────────────────────────────────

TEST(MunitionSimulator_advance, MissileRemovedOnTtlExpiry) {
  auto m = makeTestMissile(QStringLiteral("m1"));
  m.ageSeconds = 11.99;  // almost at ttl (12 s)
  QVector<ActiveMunition> munitions;
  munitions.push_back(m);
  QVector<TransientEffect> effects;
  QVector<Entity> entities;

  advanceActiveMunitions(munitions, effects, entities,
    [](const QString&, double) {},
    [](const ActiveMunition&) {},
    0.1); // pushes ageSeconds past ttl

  EXPECT_TRUE(munitions.isEmpty());
}

// ── advanceActiveMunitions — bomb gravity ─────────────────────────────────────

TEST(MunitionSimulator_advance, BombFallsDueToGravity) {
  auto b = makeTestBomb(QStringLiteral("b1"), 0.0, 0.0, 5000.0);
  QVector<ActiveMunition> munitions;
  munitions.push_back(b);
  QVector<TransientEffect> effects;
  QVector<Entity> entities;

  advanceActiveMunitions(munitions, effects, entities,
    [](const QString&, double) {},
    [](const ActiveMunition&) {},
    1.0);

  ASSERT_EQ(munitions.size(), 1);
  EXPECT_LT(munitions.at(0).altitudeMeters, 5000.0); // fell
  EXPECT_LT(munitions.at(0).verticalSpeedMetersPerSecond, 0.0); // now negative
}

// ── advanceActiveMunitions — ground expiry ───────────────────────────────────

TEST(MunitionSimulator_advance, BombDetonatesOnGround) {
  auto b = makeTestBomb(QStringLiteral("b1"), 0.0, 0.0, 2.0); // near ground
  b.verticalSpeedMetersPerSecond = -50.0;
  QVector<ActiveMunition> munitions;
  munitions.push_back(b);
  QVector<TransientEffect> effects;
  QVector<Entity> entities;
  int blastCalls = 0;

  advanceActiveMunitions(munitions, effects, entities,
    [](const QString&, double) {},
    [&](const ActiveMunition&) { ++blastCalls; },
    0.1);

  EXPECT_TRUE(munitions.isEmpty());
  EXPECT_EQ(blastCalls, 1);
  EXPECT_FALSE(effects.isEmpty()); // impact effects added
}

// ── advanceActiveMunitions — proximity hit ───────────────────────────────────

TEST(MunitionSimulator_advance, MissileHitsNearbyEnemy) {
  // Missile at same position as target → instant hit
  auto m = makeTestMissile(QStringLiteral("m1"), 10.0, 20.0, 5000.0);
  QVector<ActiveMunition> munitions;
  munitions.push_back(m);
  QVector<TransientEffect> effects;

  Entity enemy;
  enemy.name = QStringLiteral("Red-1");
  enemy.forceIdentifier = 2; // opposite force
  enemy.destroyed = false;
  enemy.latitude = 10.0;
  enemy.longitude = 20.0;
  enemy.altitude = 5000;
  QVector<Entity> entities;
  entities.push_back(enemy);

  QString damagedName;
  double damagedAmount = 0.0;
  advanceActiveMunitions(munitions, effects, entities,
    [&](const QString& name, double amount) {
      damagedName = name;
      damagedAmount = amount;
    },
    [](const ActiveMunition&) {},
    0.01);

  EXPECT_EQ(damagedName, QStringLiteral("Red-1"));
  EXPECT_GT(damagedAmount, 0.0);
  EXPECT_TRUE(munitions.isEmpty());
  EXPECT_FALSE(effects.isEmpty());
}

// ── advanceActiveMunitions — friendly fire ignored ───────────────────────────

TEST(MunitionSimulator_advance, FriendlyEntityNotHit) {
  auto m = makeTestMissile(QStringLiteral("m1"), 10.0, 20.0, 5000.0);
  QVector<ActiveMunition> munitions;
  munitions.push_back(m);
  QVector<TransientEffect> effects;

  Entity friendly;
  friendly.name = QStringLiteral("Blue-2");
  friendly.forceIdentifier = 1; // same force as munition
  friendly.destroyed = false;
  friendly.latitude = 10.0;
  friendly.longitude = 20.0;
  friendly.altitude = 5000;
  QVector<Entity> entities;
  entities.push_back(friendly);

  bool damaged = false;
  advanceActiveMunitions(munitions, effects, entities,
    [&](const QString&, double) { damaged = true; },
    [](const ActiveMunition&) {},
    0.01);

  EXPECT_FALSE(damaged);
  EXPECT_FALSE(munitions.isEmpty()); // still flying
}
