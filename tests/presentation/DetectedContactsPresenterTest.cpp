#include <gtest/gtest.h>
#include "presentation/DetectedContactsPresenter.h"
#include "domain/Entity.h"
#include "domain/Sensor.h"

using namespace presentation;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Entity makeEntity(const QString& name, int force = 1,
                         const QString& type = QStringLiteral("Fighter")) {
  Entity e;
  e.name = name;
  e.forceIdentifier = force;
  e.type = type;
  e.category = QStringLiteral("Air");
  e.altitude = 5000;
  return e;
}

static SensorContact makeContact(const QString& target, double rangeMeters,
                                  double bearingDeg, bool detected = true) {
  SensorContact c;
  c.targetEntityName = target;
  c.rangeMeters = rangeMeters;
  c.bearingDegrees = bearingDeg;
  c.detected = detected;
  return c;
}

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST(DetectedContactsPresenterTest, EmptyEntities_ReturnsNoRows) {
  EXPECT_TRUE(buildDetectedContactRows({}).isEmpty());
}

TEST(DetectedContactsPresenterTest, UndetectedContactsAreSkipped) {
  Entity observer = makeEntity(QStringLiteral("Observer"));
  Entity target   = makeEntity(QStringLiteral("Target"));
  observer.sensorContacts.append(makeContact(QStringLiteral("Target"), 50000, 90, false));
  const QVector<Entity> entities{observer, target};
  EXPECT_TRUE(buildDetectedContactRows(entities).isEmpty());
}

TEST(DetectedContactsPresenterTest, DetectedContactProducesOneRow) {
  Entity observer = makeEntity(QStringLiteral("Alpha"));
  Entity target   = makeEntity(QStringLiteral("Bravo"), 2);
  observer.sensorContacts.append(makeContact(QStringLiteral("Bravo"), 10000, 45));
  const QVector<Entity> entities{observer, target};
  const auto rows = buildDetectedContactRows(entities);
  ASSERT_EQ(rows.size(), 1);
  EXPECT_EQ(rows[0].observerName, QStringLiteral("Alpha"));
  EXPECT_EQ(rows[0].targetName,   QStringLiteral("Bravo"));
}

TEST(DetectedContactsPresenterTest, RangeFormattedInKm) {
  Entity observer = makeEntity(QStringLiteral("O1"));
  Entity target   = makeEntity(QStringLiteral("T1"));
  observer.sensorContacts.append(makeContact(QStringLiteral("T1"), 12500, 0));
  const auto rows = buildDetectedContactRows({observer, target});
  ASSERT_EQ(rows.size(), 1);
  EXPECT_EQ(rows[0].rangeText, QStringLiteral("12.5 km"));
}

TEST(DetectedContactsPresenterTest, BearingFormattedWithDeg) {
  Entity observer = makeEntity(QStringLiteral("O1"));
  Entity target   = makeEntity(QStringLiteral("T1"));
  observer.sensorContacts.append(makeContact(QStringLiteral("T1"), 1000, 270));
  const auto rows = buildDetectedContactRows({observer, target});
  ASSERT_EQ(rows.size(), 1);
  EXPECT_EQ(rows[0].bearingText, QStringLiteral("270.0 deg"));
}

TEST(DetectedContactsPresenterTest, DuplicatePairsAreDeduped) {
  Entity observer = makeEntity(QStringLiteral("O1"));
  Entity target   = makeEntity(QStringLiteral("T1"));
  observer.sensorContacts.append(makeContact(QStringLiteral("T1"), 1000, 0));
  observer.sensorContacts.append(makeContact(QStringLiteral("T1"), 2000, 10));
  const auto rows = buildDetectedContactRows({observer, target});
  EXPECT_EQ(rows.size(), 1);
}

TEST(DetectedContactsPresenterTest, MissingTargetEntityIsSkipped) {
  Entity observer = makeEntity(QStringLiteral("O1"));
  observer.sensorContacts.append(makeContact(QStringLiteral("Ghost"), 5000, 0));
  // Ghost entity not in the list
  const auto rows = buildDetectedContactRows({observer});
  EXPECT_TRUE(rows.isEmpty());
}

TEST(DetectedContactsPresenterTest, TypeFallsBackToCategory) {
  Entity observer = makeEntity(QStringLiteral("O1"));
  Entity target   = makeEntity(QStringLiteral("T1"), 2, QStringLiteral(""));
  target.category = QStringLiteral("Ground");
  observer.sensorContacts.append(makeContact(QStringLiteral("T1"), 1000, 0));
  const auto rows = buildDetectedContactRows({observer, target});
  ASSERT_EQ(rows.size(), 1);
  EXPECT_EQ(rows[0].typeLabel, QStringLiteral("Ground"));
}

TEST(DetectedContactsPresenterTest, MultipleObserversEachProduceRows) {
  Entity o1 = makeEntity(QStringLiteral("O1"));
  Entity o2 = makeEntity(QStringLiteral("O2"));
  Entity t  = makeEntity(QStringLiteral("T1"), 2);
  o1.sensorContacts.append(makeContact(QStringLiteral("T1"), 1000, 0));
  o2.sensorContacts.append(makeContact(QStringLiteral("T1"), 2000, 180));
  const auto rows = buildDetectedContactRows({o1, o2, t});
  EXPECT_EQ(rows.size(), 2);
}

TEST(DetectedContactsPresenterTest, ForceLabelFriendly) {
  Entity observer = makeEntity(QStringLiteral("O1"));
  Entity target   = makeEntity(QStringLiteral("T1"), 1);
  observer.sensorContacts.append(makeContact(QStringLiteral("T1"), 1000, 0));
  const auto rows = buildDetectedContactRows({observer, target});
  ASSERT_EQ(rows.size(), 1);
  EXPECT_FALSE(rows[0].forceLabel.isEmpty());
}
