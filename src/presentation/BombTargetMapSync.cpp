#include "presentation/BombTargetMapSync.h"

#include "domain/BombReleaseGate.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/GeoMath.h"
#include "presentation/TrackSummaryBuilder.h"

namespace presentation {

static const QString kBombTargetTrackName     = QStringLiteral("Bomb Target");
static const QString kBombTargetLineTrackName = QStringLiteral("Bomb Target Line");

void syncPendingBombTargetToMap(
    const PendingBombRelease&                          pendingRelease,
    std::function<const Entity*(const QString& name)>  findEntity,
    std::function<void(const QVariantMap&, bool)>      sendTrack,
    std::function<void(const QString&)>                removeTrack)
{
  if (!pendingRelease.pending) {
    removeTrack(kBombTargetTrackName);
    removeTrack(kBombTargetLineTrackName);
    return;
  }

  QString teamLabel         = QStringLiteral("Friendly");
  QString releaseStateLabel = QStringLiteral("Armed");
  QString ccrpCueLabel;
  double distanceMeters     = -1.0;
  double releaseDistanceMeters = -1.0;
  double distanceErrorMeters = 0.0;
  double timeToImpactSeconds = -1.0;

  if (const Entity* launcher = findEntity(pendingRelease.launcherEntityName)) {
    teamLabel         = domain::forceIdentifierLabel(launcher->forceIdentifier);
    distanceMeters    = domain::distanceMeters(
        launcher->latitude,
        launcher->longitude,
        pendingRelease.targetLatitude,
        pendingRelease.targetLongitude);

    const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
        *launcher,
        pendingRelease.targetLatitude,
        pendingRelease.targetLongitude,
        pendingRelease.targetAltitudeMeters);
    releaseStateLabel = evaluation.stateLabel();
    ccrpCueLabel = evaluation.ccrpCueLabel();
    releaseDistanceMeters = evaluation.releaseDistanceMeters;
    distanceErrorMeters = evaluation.distanceErrorMeters;
    timeToImpactSeconds = evaluation.timeToImpactSeconds;

    sendTrack(
        makePendingBombTargetLineTrackSummary(
            *launcher,
            pendingRelease.targetLatitude,
            pendingRelease.targetLongitude,
            pendingRelease.targetAltitudeMeters,
            teamLabel,
            releaseStateLabel),
        false);
  } else {
    removeTrack(kBombTargetLineTrackName);
  }

  sendTrack(
      makePendingBombTargetTrackSummary(
          pendingRelease.targetLabel,
          pendingRelease.targetLatitude,
          pendingRelease.targetLongitude,
          pendingRelease.targetAltitudeMeters,
          teamLabel,
          releaseStateLabel,
          distanceMeters,
          ccrpCueLabel,
          releaseDistanceMeters,
          distanceErrorMeters,
          timeToImpactSeconds),
      false);
}

} // namespace presentation
