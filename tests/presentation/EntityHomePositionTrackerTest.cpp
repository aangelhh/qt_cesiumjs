// Unit tests for presentation::EntityHomePositionTracker.

#include "presentation/EntityHomePositionTracker.h"

#include "domain/Entity.h"

#include <gtest/gtest.h>

namespace {

Entity makeEntity(const QString& name, double lat, double lon, int alt) {
    Entity e;
    e.name = name;
    e.latitude = lat;
    e.longitude = lon;
    e.altitude = alt;
    return e;
}

} // namespace

TEST(EntityHomePositionTracker, ReturnsInvalidWhenUnknown) {
    presentation::EntityHomePositionTracker tracker;
    const auto pos = tracker.positionFor(QStringLiteral("UNKNOWN"));
    EXPECT_FALSE(pos.valid);
    EXPECT_DOUBLE_EQ(pos.latitude, 0.0);
    EXPECT_DOUBLE_EQ(pos.longitude, 0.0);
    EXPECT_EQ(pos.altitudeMeters, 0);
}

TEST(EntityHomePositionTracker, RememberCapturesInitialKinematics) {
    presentation::EntityHomePositionTracker tracker;
    tracker.remember(makeEntity(QStringLiteral("F-16"), 40.0, -3.5, 5000));

    const auto pos = tracker.positionFor(QStringLiteral("F-16"));
    EXPECT_TRUE(pos.valid);
    EXPECT_DOUBLE_EQ(pos.latitude, 40.0);
    EXPECT_DOUBLE_EQ(pos.longitude, -3.5);
    EXPECT_EQ(pos.altitudeMeters, 5000);
}

TEST(EntityHomePositionTracker, RememberIsStickyAfterFirstObservation) {
    presentation::EntityHomePositionTracker tracker;
    tracker.remember(makeEntity(QStringLiteral("F-16"), 40.0, -3.5, 5000));
    // Subsequent calls with new coordinates must NOT overwrite home.
    tracker.remember(makeEntity(QStringLiteral("F-16"), 45.0, 10.0, 9000));

    const auto pos = tracker.positionFor(QStringLiteral("F-16"));
    EXPECT_DOUBLE_EQ(pos.latitude, 40.0);
    EXPECT_DOUBLE_EQ(pos.longitude, -3.5);
    EXPECT_EQ(pos.altitudeMeters, 5000);
}

TEST(EntityHomePositionTracker, RememberIgnoresEmptyOrWhitespaceNames) {
    presentation::EntityHomePositionTracker tracker;
    tracker.remember(makeEntity(QStringLiteral(""), 40.0, -3.5, 5000));
    tracker.remember(makeEntity(QStringLiteral("   "), 40.0, -3.5, 5000));

    EXPECT_FALSE(tracker.positionFor(QStringLiteral("")).valid);
    EXPECT_FALSE(tracker.positionFor(QStringLiteral("   ")).valid);
}

TEST(EntityHomePositionTracker, IndependentRecordsPerEntity) {
    presentation::EntityHomePositionTracker tracker;
    tracker.remember(makeEntity(QStringLiteral("Alpha"), 1.0, 2.0, 100));
    tracker.remember(makeEntity(QStringLiteral("Bravo"), 3.0, 4.0, 200));

    const auto alpha = tracker.positionFor(QStringLiteral("Alpha"));
    const auto bravo = tracker.positionFor(QStringLiteral("Bravo"));
    EXPECT_DOUBLE_EQ(alpha.latitude, 1.0);
    EXPECT_EQ(alpha.altitudeMeters, 100);
    EXPECT_DOUBLE_EQ(bravo.latitude, 3.0);
    EXPECT_EQ(bravo.altitudeMeters, 200);
}

TEST(EntityHomePositionTracker, PruneToRemovesUnknownNames) {
    presentation::EntityHomePositionTracker tracker;
    tracker.remember(makeEntity(QStringLiteral("Keep"), 1.0, 2.0, 100));
    tracker.remember(makeEntity(QStringLiteral("Drop"), 3.0, 4.0, 200));

    QSet<QString> valid;
    valid.insert(QStringLiteral("Keep"));
    tracker.pruneTo(valid);

    EXPECT_TRUE(tracker.positionFor(QStringLiteral("Keep")).valid);
    EXPECT_FALSE(tracker.positionFor(QStringLiteral("Drop")).valid);
}
