#include <gtest/gtest.h>
#include "presentation/EntityStatusFormatter.h"
#include "domain/Entity.h"
#include "domain/Munition.h"
#include "presentation/PlanTypes.h"

using namespace presentation;

// ── Helpers ──────────────────────────────────────────────────────────────────

static Entity makeEntity(const QString& name, int force = 1) {
  Entity e;
  e.name = name;
  e.forceIdentifier = force;
  e.destroyed = false;
  return e;
}

static EntityStatusContext makeCtx(
    const QVector<Entity>& entities = {},
    bool simRunning = false) {
  static QVector<ActiveMunition> noMunitions;
  static PendingBombRelease noBomb;
  static QHash<QString, EntityPlan> noPlans;
  return EntityStatusContext{entities, noMunitions, noBomb, noPlans, simRunning};
}

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST(EntityStatusFormatterTest, NullEntityReturnsSummaryStatus) {
  QVariantMap summary;
  summary[QStringLiteral("status")] = QStringLiteral("Ready");
  const auto ctx = makeCtx();
  const QString result = buildEntityOperationalStatus(summary, nullptr, ctx);
  EXPECT_EQ(result, QStringLiteral("Ready"));
}

TEST(EntityStatusFormatterTest, NullEntityWithTaskType) {
  QVariantMap summary;
  summary[QStringLiteral("taskType")] = QStringLiteral("MoveToLocation");
  summary[QStringLiteral("taskStatus")] = QStringLiteral("Running");
  const auto ctx = makeCtx();
  const QString result = buildEntityOperationalStatus(summary, nullptr, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("MoveToLocation")));
  EXPECT_TRUE(result.contains(QStringLiteral("Running")));
}

TEST(EntityStatusFormatterTest, EntityWithNoTaskShowsSummaryStatus) {
  QVariantMap summary;
  summary[QStringLiteral("status")] = QStringLiteral("Idle");
  Entity entity = makeEntity(QStringLiteral("Alpha"));
  entity.currentTask.enabled = false;
  const auto ctx = makeCtx({entity});
  const QString result = buildEntityOperationalStatus(summary, &entity, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("Idle")));
}

TEST(EntityStatusFormatterTest, EntityWithLiveTaskOverridesSummary) {
  QVariantMap summary;
  summary[QStringLiteral("taskType")] = QStringLiteral("Patrol");
  summary[QStringLiteral("taskStatus")] = QStringLiteral("Running");
  Entity entity = makeEntity(QStringLiteral("Bravo"));
  entity.currentTask.enabled = true;
  entity.currentTask.taskType = QStringLiteral("AttackAir");
  entity.currentTask.status = QStringLiteral("Running");
  const auto ctx = makeCtx({entity});
  const QString result = buildEntityOperationalStatus(summary, &entity, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("AttackAir")));
}

TEST(EntityStatusFormatterTest, MissileAndBombCountsAppearInOutput) {
  QVariantMap summary;
  Entity entity = makeEntity(QStringLiteral("Charlie"));
  WeaponInventoryItem missile;
  missile.weaponType = QStringLiteral("Missile");
  missile.quantity = 3;
  WeaponInventoryItem bomb;
  bomb.weaponType = QStringLiteral("Bomb");
  bomb.quantity = 2;
  entity.weapons.push_back(missile);
  entity.weapons.push_back(bomb);
  const auto ctx = makeCtx({entity});
  const QString result = buildEntityOperationalStatus(summary, &entity, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("3")));
  EXPECT_TRUE(result.contains(QStringLiteral("2")));
}

TEST(EntityStatusFormatterTest, BehaviorModeDefaultsToManual) {
  QVariantMap summary;
  Entity entity = makeEntity(QStringLiteral("Delta"));
  entity.behaviorMode = QStringLiteral("");
  const auto ctx = makeCtx({entity});
  const QString result = buildEntityOperationalStatus(summary, &entity, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("Manual")));
}

TEST(EntityStatusFormatterTest, BehaviorModeUsesEntityValue) {
  QVariantMap summary;
  Entity entity = makeEntity(QStringLiteral("Echo"));
  entity.behaviorMode = QStringLiteral("Aggressive");
  const auto ctx = makeCtx({entity});
  const QString result = buildEntityOperationalStatus(summary, &entity, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("Aggressive")));
}

TEST(EntityStatusFormatterTest, NoContactsMessageWhenEmpty) {
  QVariantMap summary;
  Entity entity = makeEntity(QStringLiteral("Foxtrot"));
  entity.sensorContacts.clear();
  const auto ctx = makeCtx({entity});
  const QString result = buildEntityOperationalStatus(summary, &entity, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("No contacts detected")));
}

TEST(EntityStatusFormatterTest, ContactAppearsInOutput) {
  QVariantMap summary;
  Entity observer = makeEntity(QStringLiteral("Golf"), 1);
  Entity target = makeEntity(QStringLiteral("Hotel"), 2);
  SensorContact contact;
  contact.targetEntityName = QStringLiteral("Hotel");
  contact.detected = true;
  contact.rangeMeters = 25000.0;
  contact.bearingDegrees = 90.0;
  observer.sensorContacts.push_back(contact);
  const auto ctx = makeCtx({observer, target});
  const QString result = buildEntityOperationalStatus(summary, &observer, ctx);
  EXPECT_TRUE(result.contains(QStringLiteral("Hotel")));
  EXPECT_TRUE(result.contains(QStringLiteral("detected")));
}
