#include <gtest/gtest.h>
#include "presentation/BombTargetMapSync.h"
#include "domain/Entity.h"
#include <QVariantMap>
#include <QVector>

using namespace presentation;

namespace {

PendingBombRelease makePending(const QString& launcher = "Eagle1") {
  PendingBombRelease r;
  r.pending              = true;
  r.launcherEntityName   = launcher;
  r.targetLabel          = "Target Alpha";
  r.targetLatitude       = 40.0;
  r.targetLongitude      = -3.0;
  r.targetAltitudeMeters = 0.0;
  return r;
}

Entity makeLauncher(const QString& name = "Eagle1", int force = 1) {
  Entity e;
  e.name            = name;
  e.forceIdentifier = force;
  e.latitude        = 40.01;
  e.longitude       = -3.01;
  e.altitude        = 3000;
  e.headingDegrees  = 135.0;
  e.speedKnots      = 300.0;
  return e;
}

struct Spy {
  QVector<QString>    sent;
  QVector<QVariantMap> sentSummaries;
  QVector<QString>    removed;
  const Entity*       resolvedEntity = nullptr;

  std::function<const Entity*(const QString&)> finder() {
    return [this](const QString&) { return resolvedEntity; };
  }
  std::function<void(const QVariantMap&, bool)> sender() {
    return [this](const QVariantMap& s, bool) {
      sent.push_back(s.value(QStringLiteral("name")).toString());
      sentSummaries.push_back(s);
    };
  }
  std::function<void(const QString&)> remover() {
    return [this](const QString& id) { removed.push_back(id); };
  }
};

} // namespace

// ── not pending → removes both tracks ─────────────────────────────────────

TEST(BombTargetMapSync, NotPendingRemovesBothTracks) {
  PendingBombRelease r; // pending = false
  Spy spy;
  syncPendingBombTargetToMap(r, spy.finder(), spy.sender(), spy.remover());
  EXPECT_TRUE(spy.sent.isEmpty());
  EXPECT_EQ(spy.removed.size(), 2);
  EXPECT_TRUE(spy.removed.contains(QStringLiteral("Bomb Target")));
  EXPECT_TRUE(spy.removed.contains(QStringLiteral("Bomb Target Line")));
}

// ── launcher not found → sends target only, removes line ──────────────────

TEST(BombTargetMapSync, LauncherNotFoundRemovesLine) {
  Spy spy;
  spy.resolvedEntity = nullptr;
  syncPendingBombTargetToMap(makePending(), spy.finder(), spy.sender(), spy.remover());
  EXPECT_EQ(spy.sent.size(), 1);
  EXPECT_TRUE(spy.removed.contains(QStringLiteral("Bomb Target Line")));
}

TEST(BombTargetMapSync, LauncherNotFoundSendsBombTargetTrack) {
  Spy spy;
  spy.resolvedEntity = nullptr;
  syncPendingBombTargetToMap(makePending(), spy.finder(), spy.sender(), spy.remover());
  EXPECT_TRUE(spy.sent.contains(QStringLiteral("Bomb Target")));
}

// ── launcher found → sends both tracks ────────────────────────────────────

TEST(BombTargetMapSync, LauncherFoundSendsBothTracks) {
  Entity launcher = makeLauncher();
  Spy spy;
  spy.resolvedEntity = &launcher;
  syncPendingBombTargetToMap(makePending(), spy.finder(), spy.sender(), spy.remover());
  EXPECT_EQ(spy.sent.size(), 2);
  EXPECT_TRUE(spy.sent.contains(QStringLiteral("Bomb Target")));
  EXPECT_TRUE(spy.sent.contains(QStringLiteral("Bomb Target Line")));
  EXPECT_TRUE(spy.removed.isEmpty());
}

TEST(BombTargetMapSync, LauncherFoundNothingRemoved) {
  Entity launcher = makeLauncher();
  Spy spy;
  spy.resolvedEntity = &launcher;
  syncPendingBombTargetToMap(makePending(), spy.finder(), spy.sender(), spy.remover());
  EXPECT_TRUE(spy.removed.isEmpty());
}

TEST(BombTargetMapSync, LauncherFoundSendsCcrpCueFields) {
  Entity launcher = makeLauncher();
  Spy spy;
  spy.resolvedEntity = &launcher;
  syncPendingBombTargetToMap(makePending(), spy.finder(), spy.sender(), spy.remover());

  QVariantMap bombTargetSummary;
  for (const QVariantMap& summary : spy.sentSummaries) {
    if (summary.value(QStringLiteral("name")).toString() == QStringLiteral("Bomb Target")) {
      bombTargetSummary = summary;
      break;
    }
  }

  EXPECT_FALSE(bombTargetSummary.isEmpty());
  EXPECT_FALSE(bombTargetSummary.value(QStringLiteral("pendingBombCcrpCue")).toString().isEmpty());
  EXPECT_GE(
      bombTargetSummary.value(QStringLiteral("pendingBombTimeToImpactSeconds")).toDouble(),
      0.0);
}
