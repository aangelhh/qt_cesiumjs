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

// ── computeBombBlastHits ───────────────────────────────────────────────────────

static ActiveMunition makeGroundedBomb(
    double lat, double lon, double altMeters,
    double blastRadius = 200.0, double baseDamage = 100.0) {
  ActiveMunition m;
  m.munitionType = QStringLiteral("Bomb");
  m.id = QStringLiteral("bomb-1");
  m.launcherEntityName = QStringLiteral("Launcher");
  m.latitude = lat;
  m.longitude = lon;
  m.altitudeMeters = altMeters;
  m.blastRadiusMeters = blastRadius;
  m.baseDamage = baseDamage;
  m.active = true;
  return m;
}

TEST(MunitionSimulator, BombBlastHitsEntityInRadius) {
  const auto bomb = makeGroundedBomb(0.0, 0.0, 10.0);
  Entity target;
  target.name = QStringLiteral("T1");
  target.latitude = 0.0; target.longitude = 0.0; target.altitude = 10;
  target.destroyed = false;
  const auto hits = computeBombBlastHits(bomb, {target});
  ASSERT_EQ(hits.size(), 1);
  EXPECT_EQ(hits.at(0).targetName, QStringLiteral("T1"));
  EXPECT_GT(hits.at(0).damageAmount, 0.0);
  EXPECT_LE(hits.at(0).damageAmount, 100.0);
}

TEST(MunitionSimulator, BombBlastMissesBeyondRadius) {
  const auto bomb = makeGroundedBomb(0.0, 0.0, 0.0, 200.0);
  Entity target;
  target.name = QStringLiteral("T1");
  // ~1km away — well outside 200m blast radius
  target.latitude = 0.009; target.longitude = 0.0; target.altitude = 0;
  target.destroyed = false;
  EXPECT_TRUE(computeBombBlastHits(bomb, {target}).isEmpty());
}

TEST(MunitionSimulator, BombBlastSkipsLauncher) {
  auto bomb = makeGroundedBomb(0.0, 0.0, 0.0);
  Entity launcher;
  launcher.name = QStringLiteral("Launcher"); // same as bomb.launcherEntityName
  launcher.latitude = 0.0; launcher.longitude = 0.0; launcher.altitude = 0;
  launcher.destroyed = false;
  EXPECT_TRUE(computeBombBlastHits(bomb, {launcher}).isEmpty());
}

TEST(MunitionSimulator, BombBlastSkipsDestroyedEntity) {
  const auto bomb = makeGroundedBomb(0.0, 0.0, 0.0);
  Entity target;
  target.name = QStringLiteral("T1");
  target.latitude = 0.0; target.longitude = 0.0; target.altitude = 0;
  target.destroyed = true;
  EXPECT_TRUE(computeBombBlastHits(bomb, {target}).isEmpty());
}

TEST(MunitionSimulator, BombBlastDamageFalloffWithDistance) {
  const auto bomb = makeGroundedBomb(0.0, 0.0, 0.0, 500.0, 100.0);
  Entity close; close.name = QStringLiteral("Close");
  close.latitude = 0.0; close.longitude = 0.0; close.altitude = 0; close.destroyed = false;
  Entity mid; mid.name = QStringLiteral("Mid");
  mid.latitude = 0.0; mid.longitude = 0.002; mid.altitude = 0; mid.destroyed = false;
  const auto hits = computeBombBlastHits(bomb, {close, mid});
  ASSERT_EQ(hits.size(), 2);
  const double closeDmg = hits.at(0).damageAmount;
  const double midDmg   = hits.at(1).damageAmount;
  EXPECT_GT(closeDmg, midDmg);
}

TEST(MunitionSimulator, BombBlastReturnsEmptyForMissile) {
  ActiveMunition missile;
  missile.munitionType = QStringLiteral("Missile");
  missile.blastRadiusMeters = 200.0;
  missile.baseDamage = 100.0;
  Entity target;
  target.name = QStringLiteral("T1");
  target.latitude = 0.0; target.longitude = 0.0; target.altitude = 0;
  target.destroyed = false;
  EXPECT_TRUE(computeBombBlastHits(missile, {target}).isEmpty());
}

TEST(MunitionSimulator, BombBlastReturnsEmptyForZeroRadius) {
  auto bomb = makeGroundedBomb(0.0, 0.0, 0.0, 0.0);
  Entity target;
  target.name = QStringLiteral("T1");
  target.latitude = 0.0; target.longitude = 0.0; target.altitude = 0;
  target.destroyed = false;
  EXPECT_TRUE(computeBombBlastHits(bomb, {target}).isEmpty());
}

// ── advanceTransientEffects ───────────────────────────────────────────────────

static TransientEffect makeFx(const QString& id, double ttl) {
  TransientEffect e;
  e.id = id;
  e.effectType = QStringLiteral("Smoke");
  e.ttlSeconds = ttl;
  e.ageSeconds = 0.0;
  e.active = true;
  return e;
}

TEST(MunitionSimulator, TransientEffectAgesOnStep) {
  QVector<TransientEffect> fx = { makeFx(QStringLiteral("e1"), 1.0) };
  advanceTransientEffects(fx, 0.5);
  ASSERT_EQ(fx.size(), 1);
  EXPECT_NEAR(fx.at(0).ageSeconds, 0.5, 0.001);
  EXPECT_TRUE(fx.at(0).active);
}

TEST(MunitionSimulator, TransientEffectRemovedAfterTtl) {
  QVector<TransientEffect> fx = { makeFx(QStringLiteral("e1"), 0.3) };
  advanceTransientEffects(fx, 0.5);
  EXPECT_TRUE(fx.isEmpty());
}

TEST(MunitionSimulator, TransientEffectZeroDeltaIsNoop) {
  QVector<TransientEffect> fx = { makeFx(QStringLiteral("e1"), 1.0) };
  advanceTransientEffects(fx, 0.0);
  ASSERT_EQ(fx.size(), 1);
  EXPECT_NEAR(fx.at(0).ageSeconds, 0.0, 0.001);
}

TEST(MunitionSimulator, TransientEffectMultiplePartialExpiry) {
  QVector<TransientEffect> fx = {
    makeFx(QStringLiteral("short"), 0.1),
    makeFx(QStringLiteral("long"),  5.0),
  };
  advanceTransientEffects(fx, 0.5);
  ASSERT_EQ(fx.size(), 1);
  EXPECT_EQ(fx.at(0).id, QStringLiteral("long"));
}
