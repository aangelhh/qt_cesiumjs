#include <gtest/gtest.h>
#include "application/BehaviorEngine.h"
#include "domain/Entity.h"

using namespace application;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Entity makeAirFighter(
    const QString& name,
    int force = 1,
    const QString& behavior = QStringLiteral("Aggressive")) {
  Entity e;
  e.name = name;
  e.domain = QStringLiteral("Air");
  e.category = QStringLiteral("Fighter");
  e.forceIdentifier = force;
  e.behaviorMode = behavior;
  e.destroyed = false;
  e.damagePercent = 0.0;
  WeaponInventoryItem missile;
  missile.weaponType = QStringLiteral("Missile");
  missile.quantity = 4;
  e.weapons.push_back(missile);
  return e;
}

static SensorContact makeContact(const QString& target, double range, bool detected = true) {
  SensorContact c;
  c.targetEntityName = target;
  c.rangeMeters = range;
  c.detected = detected;
  return c;
}

// ── advanceBehaviors — manual mode ────────────────────────────────────────────

TEST(BehaviorEngine, ManualModeDoesNotEngage) {
  QVector<Entity> entities;
  Entity a = makeAirFighter(QStringLiteral("A"), 1, QStringLiteral("Manual"));
  Entity b = makeAirFighter(QStringLiteral("B"), 2);
  b.behaviorMode = QStringLiteral("Manual");
  entities.push_back(a);
  entities.push_back(b);

  BehaviorCooldownMap cooldowns;
  BehaviorReactionMap reactions;
  bool launched = false;
  const QStringList logs = advanceBehaviors(entities, cooldowns, reactions, 1.0,
    [&](const QString&, const QString&) { launched = true; return true; });

  EXPECT_FALSE(launched);
  EXPECT_TRUE(logs.isEmpty());
}

// ── advanceBehaviors — destroyed entity is cleared ───────────────────────────

TEST(BehaviorEngine, DestroyedEntityClearedFromMaps) {
  QVector<Entity> entities;
  Entity a = makeAirFighter(QStringLiteral("A"), 1);
  a.destroyed = true;
  a.behaviorTargetEntityName = QStringLiteral("B");
  entities.push_back(a);

  BehaviorCooldownMap cooldowns;
  cooldowns[QStringLiteral("A")] = 5.0;
  BehaviorReactionMap reactions;
  reactions[QStringLiteral("A")] = 1;

  advanceBehaviors(entities, cooldowns, reactions, 1.0,
    [](const QString&, const QString&) { return false; });

  EXPECT_TRUE(entities.at(0).behaviorTargetEntityName.isEmpty());
  EXPECT_EQ(cooldowns.count(QStringLiteral("A")), 0u);
  EXPECT_EQ(reactions.count(QStringLiteral("A")), 0u);
}

// ── advanceBehaviors — target selection via sensor contacts ───────────────────

TEST(BehaviorEngine, SelectsClosestDetectedEnemy) {
  QVector<Entity> entities;
  Entity shooter = makeAirFighter(QStringLiteral("Shooter"), 1);
  shooter.sensorContacts.push_back(makeContact(QStringLiteral("FarEnemy"), 80000.0));
  shooter.sensorContacts.push_back(makeContact(QStringLiteral("NearEnemy"), 15000.0));
  entities.push_back(shooter);

  Entity far = makeAirFighter(QStringLiteral("FarEnemy"), 2);
  Entity near = makeAirFighter(QStringLiteral("NearEnemy"), 2);
  entities.push_back(far);
  entities.push_back(near);

  BehaviorCooldownMap cooldowns;
  BehaviorReactionMap reactions;
  advanceBehaviors(entities, cooldowns, reactions, 0.0,
    [](const QString&, const QString&) { return false; });

  EXPECT_EQ(entities.at(0).behaviorTargetEntityName, QStringLiteral("NearEnemy"));
}

// ── advanceBehaviors — auto launch on Aggressive mode ─────────────────────────

TEST(BehaviorEngine, AggressiveLaunchesAndLogsMissile) {
  QVector<Entity> entities;
  Entity shooter = makeAirFighter(QStringLiteral("Shooter"), 1);
  shooter.sensorContacts.push_back(makeContact(QStringLiteral("Target"), 20000.0));
  entities.push_back(shooter);

  Entity target = makeAirFighter(QStringLiteral("Target"), 2);
  entities.push_back(target);

  BehaviorCooldownMap cooldowns;
  BehaviorReactionMap reactions;
  QString launchedFrom, launchedAt;
  const QStringList logs = advanceBehaviors(entities, cooldowns, reactions, 0.0,
    [&](const QString& l, const QString& t) {
      launchedFrom = l;
      launchedAt = t;
      return true;
    });

  EXPECT_EQ(launchedFrom, QStringLiteral("Shooter"));
  EXPECT_EQ(launchedAt, QStringLiteral("Target"));
  EXPECT_FALSE(logs.isEmpty());
  EXPECT_GT(cooldowns[QStringLiteral("Shooter")], 0.0);
}

// ── advanceBehaviors — cooldown prevents repeated launch ──────────────────────

TEST(BehaviorEngine, CooldownPreventsDuplicateLaunch) {
  QVector<Entity> entities;
  Entity shooter = makeAirFighter(QStringLiteral("Shooter"), 1);
  shooter.sensorContacts.push_back(makeContact(QStringLiteral("Target"), 20000.0));
  entities.push_back(shooter);
  entities.push_back(makeAirFighter(QStringLiteral("Target"), 2));

  BehaviorCooldownMap cooldowns;
  cooldowns[QStringLiteral("Shooter")] = 10.0; // still on cooldown

  BehaviorReactionMap reactions;
  int launchCount = 0;
  advanceBehaviors(entities, cooldowns, reactions, 0.0,
    [&](const QString&, const QString&) { ++launchCount; return true; });

  EXPECT_EQ(launchCount, 0);
}

// ── advanceBehaviors — cooldown ticks down ────────────────────────────────────

TEST(BehaviorEngine, CooldownTicksDown) {
  QVector<Entity> entities; // empty — just testing cooldown decay
  BehaviorCooldownMap cooldowns;
  cooldowns[QStringLiteral("A")] = 5.0;
  BehaviorReactionMap reactions;
  advanceBehaviors(entities, cooldowns, reactions, 3.0,
    [](const QString&, const QString&) { return false; });

  EXPECT_NEAR(cooldowns.at(QStringLiteral("A")), 2.0, 0.001);
}

TEST(BehaviorEngine, CooldownEntryRemovedWhenExpired) {
  QVector<Entity> entities;
  BehaviorCooldownMap cooldowns;
  cooldowns[QStringLiteral("A")] = 1.0;
  BehaviorReactionMap reactions;
  advanceBehaviors(entities, cooldowns, reactions, 2.0, // ticks past 0
    [](const QString&, const QString&) { return false; });

  EXPECT_EQ(cooldowns.count(QStringLiteral("A")), 0u);
}

// ── advanceBehaviors — critical damage blocks auto-engage ─────────────────────

TEST(BehaviorEngine, CriticalDamageBlocksEngagement) {
  QVector<Entity> entities;
  Entity shooter = makeAirFighter(QStringLiteral("Shooter"), 1);
  shooter.damagePercent = 85.0; // critical
  shooter.sensorContacts.push_back(makeContact(QStringLiteral("Target"), 20000.0));
  entities.push_back(shooter);
  entities.push_back(makeAirFighter(QStringLiteral("Target"), 2));

  BehaviorCooldownMap cooldowns;
  BehaviorReactionMap reactions;
  bool launched = false;
  advanceBehaviors(entities, cooldowns, reactions, 0.0,
    [&](const QString&, const QString&) { launched = true; return true; });

  EXPECT_FALSE(launched);
}

// ── advanceBehaviors — non-air entity skipped ─────────────────────────────────

TEST(BehaviorEngine, GroundEntitySkipped) {
  QVector<Entity> entities;
  Entity ground;
  ground.name = QStringLiteral("Tank");
  ground.domain = QStringLiteral("Ground");
  ground.forceIdentifier = 1;
  ground.behaviorMode = QStringLiteral("Aggressive");
  ground.destroyed = false;
  ground.sensorContacts.push_back(makeContact(QStringLiteral("Target"), 5000.0));
  entities.push_back(ground);
  entities.push_back(makeAirFighter(QStringLiteral("Target"), 2));

  BehaviorCooldownMap cooldowns;
  BehaviorReactionMap reactions;
  bool launched = false;
  advanceBehaviors(entities, cooldowns, reactions, 0.0,
    [&](const QString&, const QString&) { launched = true; return true; });

  EXPECT_FALSE(launched);
  EXPECT_TRUE(entities.at(0).behaviorTargetEntityName.isEmpty());
}
