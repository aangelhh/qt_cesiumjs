// Unit tests for presentation::EntityVisualStateManager. These exercise the
// in-memory hash semantics and the JSON round-trip without involving the
// scenario or the UI.

#include "presentation/EntityVisualStateManager.h"

#include <gtest/gtest.h>

#include <QFile>
#include <QString>
#include <QTemporaryDir>

namespace {

QString tempJsonPath(const QTemporaryDir& dir) {
    return dir.path() + QStringLiteral("/entity_visual_state.json");
}

} // namespace

TEST(EntityVisualStateManager, ReturnsDefaultStateForUnknownEntity) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    presentation::EntityVisualStateManager manager(tempJsonPath(dir));

    const auto state = manager.stateFor(QStringLiteral("UNKNOWN"));
    EXPECT_FALSE(state.hidden);
    EXPECT_FALSE(state.radarCoverageVisible);
    EXPECT_FALSE(state.trackHistoryVisible);
    EXPECT_FALSE(manager.contains(QStringLiteral("UNKNOWN")));
}

TEST(EntityVisualStateManager, EnsureStateInsertsAndPersistsMutation) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    presentation::EntityVisualStateManager manager(tempJsonPath(dir));

    auto& mutated = manager.ensureState(QStringLiteral("F-16"));
    mutated.hidden = true;
    mutated.radarCoverageVisible = true;

    const auto snapshot = manager.stateFor(QStringLiteral("F-16"));
    EXPECT_TRUE(snapshot.hidden);
    EXPECT_TRUE(snapshot.radarCoverageVisible);
    EXPECT_FALSE(snapshot.trackHistoryVisible);
    EXPECT_TRUE(manager.contains(QStringLiteral("F-16")));
}

TEST(EntityVisualStateManager, RemoveDropsTheRecord) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    presentation::EntityVisualStateManager manager(tempJsonPath(dir));

    manager.ensureState(QStringLiteral("F-16")).hidden = true;
    ASSERT_TRUE(manager.contains(QStringLiteral("F-16")));

    manager.remove(QStringLiteral("F-16"));
    EXPECT_FALSE(manager.contains(QStringLiteral("F-16")));
}

TEST(EntityVisualStateManager, SaveAndLoadRoundTripsAllFlags) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = tempJsonPath(dir);

    {
        presentation::EntityVisualStateManager writer(path);
        auto& a = writer.ensureState(QStringLiteral("Alpha"));
        a.hidden = true;
        auto& b = writer.ensureState(QStringLiteral("Bravo"));
        b.radarCoverageVisible = true;
        b.trackHistoryVisible = true;
        writer.save();
    }

    ASSERT_TRUE(QFile::exists(path));

    presentation::EntityVisualStateManager reader(path);
    reader.load();

    const auto a = reader.stateFor(QStringLiteral("Alpha"));
    EXPECT_TRUE(a.hidden);
    EXPECT_FALSE(a.radarCoverageVisible);
    EXPECT_FALSE(a.trackHistoryVisible);

    const auto b = reader.stateFor(QStringLiteral("Bravo"));
    EXPECT_FALSE(b.hidden);
    EXPECT_TRUE(b.radarCoverageVisible);
    EXPECT_TRUE(b.trackHistoryVisible);
}

TEST(EntityVisualStateManager, LoadOnMissingFileLeavesManagerEmpty) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    presentation::EntityVisualStateManager manager(tempJsonPath(dir));
    manager.load();
    EXPECT_FALSE(manager.contains(QStringLiteral("Alpha")));
}

TEST(EntityVisualStateManager, LoadOverwritesPreviousInMemoryState) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = tempJsonPath(dir);

    presentation::EntityVisualStateManager manager(path);
    manager.ensureState(QStringLiteral("Ghost")).hidden = true;
    manager.save();

    // Now mutate without saving, then reload from disk -> mutation gone.
    manager.ensureState(QStringLiteral("ShouldDisappear")).hidden = true;
    manager.load();

    EXPECT_TRUE(manager.contains(QStringLiteral("Ghost")));
    EXPECT_FALSE(manager.contains(QStringLiteral("ShouldDisappear")));
}

TEST(EntityVisualStateManager, PruneToRemovesUnknownNamesAndPersistsRemainder) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = tempJsonPath(dir);

    presentation::EntityVisualStateManager manager(path);
    manager.ensureState(QStringLiteral("Keep")).hidden = true;
    manager.ensureState(QStringLiteral("Drop")).radarCoverageVisible = true;

    QSet<QString> valid;
    valid.insert(QStringLiteral("Keep"));
    manager.pruneTo(valid);

    EXPECT_TRUE(manager.contains(QStringLiteral("Keep")));
    EXPECT_FALSE(manager.contains(QStringLiteral("Drop")));

    // pruneTo() must auto-save when it removed something.
    presentation::EntityVisualStateManager reader(path);
    reader.load();
    EXPECT_TRUE(reader.contains(QStringLiteral("Keep")));
    EXPECT_FALSE(reader.contains(QStringLiteral("Drop")));
}

TEST(EntityVisualStateManager, PruneToWithNoRemovalsDoesNotTouchDisk) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = tempJsonPath(dir);

    presentation::EntityVisualStateManager manager(path);
    manager.ensureState(QStringLiteral("Keep")).hidden = true;

    // File should not exist yet — we never called save().
    ASSERT_FALSE(QFile::exists(path));

    QSet<QString> valid;
    valid.insert(QStringLiteral("Keep"));
    manager.pruneTo(valid);

    // No removal happened, so save() was not invoked -> file still absent.
    EXPECT_FALSE(QFile::exists(path));
}
