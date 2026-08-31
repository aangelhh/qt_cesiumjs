#include "application/MunitionSimulator.h"
#include "domain/EntityIdentity.h"

#include "domain/Entity.h"
#include "domain/GeoMath.h"
#include "domain/Munition.h"

#include <QDir>
#include <QUrl>
#include <QtMath>

namespace {

// ─── Constants ───────────────────────────────────────────────────────────────

constexpr double kKnotsToMetersPerSecond = 0.514444;
constexpr double kDefaultMissileBoostMetersPerSecond = 250.0;
constexpr double kDefaultMissileTtlSeconds = 12.0;
constexpr double kDefaultMissileHitRadiusMeters = 120.0;
constexpr double kDefaultMissileTurnRateDegreesPerSecond = 45.0;
constexpr double kDefaultMissilePitchRateDegreesPerSecond = 30.0;
constexpr double kDefaultMissileMaxPitchDegrees = 60.0;
constexpr double kGravityMetersPerSecondSquared = 9.81;
constexpr double kDefaultBombTtlSeconds = 45.0;
constexpr double kDefaultBombHitRadiusMeters = 120.0;
constexpr double kDefaultBombBlastRadiusMeters = 200.0;
constexpr double kDefaultBombBaseDamage = 100.0;
constexpr double kDefaultBombForwardOffsetMeters = 25.0;
constexpr double kDefaultBombDownOffsetMeters = 5.0;
constexpr double kMinimumMunitionAltitudeMeters = 1.0;
constexpr double kImpactFlashTtlSeconds = 0.45;
constexpr double kBombSmokeTrailIntervalSeconds = 0.25;
constexpr double kBombSmokeTrailTtlSeconds = 1.0;
constexpr double kBombImpactFlashTtlSeconds = 0.35;
constexpr double kBombSmokeTtlSeconds = 3.0;

// ─── Model URI helpers ────────────────────────────────────────────────────────

QString projectRoot() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

QString defaultMissileModelUri() {
  return QUrl::fromLocalFile(
      QDir(projectRoot()).absoluteFilePath(
          QStringLiteral("models/missile/missile.glb")))
      .toString();
}

QString defaultBombModelUri() {
  return QUrl::fromLocalFile(
      QDir(projectRoot()).absoluteFilePath(
          QStringLiteral("models/bomb/bomb.glb")))
      .toString();
}

double clampStep(double current, double target, double maxStep) {
  if (maxStep <= 0.0) {
    return current;
  }
  if (target > current) {
    return qMin(target, current + maxStep);
  }
  return qMax(target, current - maxStep);
}

// ─── Entity lookup ────────────────────────────────────────────────────────────

const Entity* findEntityByName(
    const QVector<Entity>& entities,
    const QString& name) {
  for (const Entity& entity : entities) {
    if (domain::entityMatchesReference(entity, name)) {
      return &entity;
    }
  }
  return nullptr;
}

// ─── Transient effect builders ────────────────────────────────────────────────

TransientEffect makeTransientEffect(
    const QString& id,
    const QString& effectType,
    int forceIdentifier,
    double latitude,
    double longitude,
    double altitudeMeters,
    double ttlSeconds) {
  TransientEffect effect;
  effect.id = id;
  effect.effectType = effectType;
  effect.forceIdentifier = forceIdentifier;
  effect.latitude = latitude;
  effect.longitude = longitude;
  effect.altitudeMeters = altitudeMeters;
  effect.ttlSeconds = ttlSeconds;
  return effect;
}

void appendBombSmokeTrailEffect(
    QVector<TransientEffect>& effects,
    const ActiveMunition& munition,
    int trailIndex) {
  effects.push_back(makeTransientEffect(
      QStringLiteral("%1-bombtrail-%2").arg(munition.id).arg(trailIndex),
      QStringLiteral("BombSmokeTrail"),
      munition.forceIdentifier,
      munition.latitude,
      munition.longitude,
      munition.altitudeMeters,
      kBombSmokeTrailTtlSeconds));
}

void appendBombImpactEffects(
    QVector<TransientEffect>& effects,
    const ActiveMunition& munition) {
  effects.push_back(makeTransientEffect(
      munition.id + QStringLiteral("-bombimpactflash"),
      QStringLiteral("BombImpactFlash"),
      munition.forceIdentifier,
      munition.latitude,
      munition.longitude,
      munition.altitudeMeters,
      kBombImpactFlashTtlSeconds));
  effects.push_back(makeTransientEffect(
      munition.id + QStringLiteral("-bombsmoke"),
      QStringLiteral("BombSmoke"),
      munition.forceIdentifier,
      munition.latitude,
      munition.longitude,
      munition.altitudeMeters,
      kBombSmokeTtlSeconds));
}

} // namespace

namespace application {

// ─── Public factory functions ─────────────────────────────────────────────────

ActiveMunition makeMissileMunition(const Entity& entity, int serial) {
  ActiveMunition munition;
  munition.id = QStringLiteral("%1-%2-missile-%3")
                    .arg(entity.name, domain::entityKey(entity).left(8))
                    .arg(serial);
  munition.launcherEntityId = domain::entityKey(entity);
  munition.launcherEntityName = entity.name;
  munition.forceIdentifier = entity.forceIdentifier;
  munition.munitionType = QStringLiteral("Missile");
  munition.modelUri = defaultMissileModelUri();
  munition.latitude = entity.latitude;
  munition.longitude = entity.longitude;
  munition.altitudeMeters = static_cast<double>(entity.altitude);
  munition.headingDegrees = domain::normalizeDegrees360(entity.headingDegrees);
  munition.pitchDegrees = entity.pitchDegrees;
  munition.rollDegrees = 0.0;
  munition.speedMetersPerSecond =
      qMax(0.0, entity.speedKnots * kKnotsToMetersPerSecond) +
      kDefaultMissileBoostMetersPerSecond;
  munition.verticalSpeedMetersPerSecond = 0.0;
  munition.ttlSeconds = kDefaultMissileTtlSeconds;
  munition.hitRadiusMeters = kDefaultMissileHitRadiusMeters;
  return munition;
}

ActiveMunition makeBombMunition(const Entity& entity, int serial) {
  ActiveMunition munition;
  munition.id = QStringLiteral("%1-%2-bomb-%3")
                    .arg(entity.name, domain::entityKey(entity).left(8))
                    .arg(serial);
  munition.launcherEntityId = domain::entityKey(entity);
  munition.launcherEntityName = entity.name;
  munition.forceIdentifier = entity.forceIdentifier;
  munition.munitionType = QStringLiteral("Bomb");
  munition.modelUri = defaultBombModelUri();
  munition.status = QStringLiteral("Falling");
  munition.headingDegrees = domain::normalizeDegrees360(entity.headingDegrees);
  munition.pitchDegrees = entity.pitchDegrees;
  munition.rollDegrees = 0.0;
  const double pitchRadians = qDegreesToRadians(entity.pitchDegrees);
  const double baseSpeedMetersPerSecond =
      qMax(0.0, entity.speedKnots * kKnotsToMetersPerSecond);
  munition.speedMetersPerSecond =
      qMax(0.0, baseSpeedMetersPerSecond * qCos(pitchRadians));
  munition.verticalSpeedMetersPerSecond = entity.verticalSpeedMetersPerSecond;
  munition.ttlSeconds = kDefaultBombTtlSeconds;
  munition.hitRadiusMeters = kDefaultBombHitRadiusMeters;
  munition.blastRadiusMeters = kDefaultBombBlastRadiusMeters;
  munition.baseDamage = kDefaultBombBaseDamage;

  const domain::GeoCoordinate offsetPosition = domain::destinationPoint(
      entity.latitude,
      entity.longitude,
      munition.headingDegrees,
      kDefaultBombForwardOffsetMeters,
      entity.altitude);
  munition.latitude = offsetPosition.latitude;
  munition.longitude = offsetPosition.longitude;
  munition.altitudeMeters = qMax(
      kMinimumMunitionAltitudeMeters,
      static_cast<double>(entity.altitude) - kDefaultBombDownOffsetMeters);
  return munition;
}

bool munitionIsBomb(const ActiveMunition& munition) {
  return munition.munitionType.compare(
             QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0;
}

QVector<BombBlastHit> computeBombBlastHits(
    const ActiveMunition& munition,
    const QVector<Entity>& entities) {
  if (!munitionIsBomb(munition) ||
      munition.blastRadiusMeters <= 0.0 ||
      munition.baseDamage <= 0.0) {
    return {};
  }

  QVector<BombBlastHit> hits;
  for (const Entity& entity : entities) {
    const QString launcherReference =
        munition.launcherEntityId.trimmed().isEmpty()
            ? munition.launcherEntityName
            : munition.launcherEntityId;
    if (entity.destroyed ||
        domain::entityMatchesReference(entity, launcherReference)) {
      continue;
    }

    const double horizontalDistanceMeters = domain::distanceMeters(
        munition.latitude, munition.longitude,
        entity.latitude,   entity.longitude);
    const double verticalSeparationMeters = qAbs(
        munition.altitudeMeters - static_cast<double>(entity.altitude));
    const double slantRangeMeters = qSqrt(
        qPow(horizontalDistanceMeters, 2.0) +
        qPow(verticalSeparationMeters, 2.0));
    if (slantRangeMeters >= munition.blastRadiusMeters) {
      continue;
    }

    const double damage = munition.baseDamage *
        (1.0 - slantRangeMeters / munition.blastRadiusMeters);
    if (damage <= 0.0) {
      continue;
    }

    hits.push_back(BombBlastHit{domain::entityKey(entity), damage});
  }
  return hits;
}

// ─── Simulation step ──────────────────────────────────────────────────────────

void advanceActiveMunitions(
    QVector<ActiveMunition>&                    activeMunitions,
    QVector<TransientEffect>&                   effects,
    const QVector<Entity>&                      entities,
    std::function<void(const QString&, double)> applyDamageFn,
    std::function<void(const ActiveMunition&)>  applyBombBlastFn,
    double                                      deltaSeconds) {
  if (deltaSeconds <= 0.0 || activeMunitions.isEmpty()) {
    return;
  }

  const double maxTurnStepDegrees =
      kDefaultMissileTurnRateDegreesPerSecond * deltaSeconds;
  const double maxPitchStepDegrees =
      kDefaultMissilePitchRateDegreesPerSecond * deltaSeconds;

  for (ActiveMunition& munition : activeMunitions) {
    if (!munition.active) {
      continue;
    }

    const double previousAgeSeconds = munition.ageSeconds;
    const bool isBomb = munitionIsBomb(munition);
    const QString targetReference =
        munition.targetEntityId.trimmed().isEmpty()
            ? munition.targetEntityName
            : munition.targetEntityId;
    const bool hasGuidedTarget =
        !isBomb && !targetReference.trimmed().isEmpty();
    const Entity* trackedTarget = nullptr;

    if (hasGuidedTarget && munition.guidanceActive) {
      trackedTarget = findEntityByName(entities, targetReference);
      if (trackedTarget && !trackedTarget->destroyed) {
        const double desiredHeadingDegrees = domain::bearingDegrees(
            munition.latitude, munition.longitude,
            trackedTarget->latitude, trackedTarget->longitude);
        const double headingDeltaDegrees = domain::shortestSignedAngle(
            munition.headingDegrees, desiredHeadingDegrees);
        munition.headingDegrees = domain::normalizeDegrees360(
            munition.headingDegrees +
            clampStep(0.0, headingDeltaDegrees, maxTurnStepDegrees));

        const double horizontalDistanceToTargetMeters = domain::distanceMeters(
            munition.latitude, munition.longitude,
            trackedTarget->latitude, trackedTarget->longitude);
        const double altitudeDeltaMeters =
            static_cast<double>(trackedTarget->altitude) -
            munition.altitudeMeters;
        const double desiredPitchDegrees = qBound(
            -kDefaultMissileMaxPitchDegrees,
            qRadiansToDegrees(qAtan2(
                altitudeDeltaMeters,
                qMax(1.0, horizontalDistanceToTargetMeters))),
            kDefaultMissileMaxPitchDegrees);
        munition.pitchDegrees = qBound(
            -kDefaultMissileMaxPitchDegrees,
            clampStep(
                munition.pitchDegrees,
                desiredPitchDegrees,
                maxPitchStepDegrees),
            kDefaultMissileMaxPitchDegrees);
        munition.status = QStringLiteral("Tracking");
      } else {
        munition.guidanceActive = false;
        munition.status = QStringLiteral("Lost Target");
        trackedTarget = nullptr;
      }
    }

    double horizontalDistanceMeters = 0.0;
    double verticalDistanceMeters = 0.0;
    if (isBomb) {
      munition.verticalSpeedMetersPerSecond -=
          kGravityMetersPerSecondSquared * deltaSeconds;
      horizontalDistanceMeters =
          qMax(0.0, munition.speedMetersPerSecond * deltaSeconds);
      verticalDistanceMeters =
          munition.verticalSpeedMetersPerSecond * deltaSeconds;
      munition.pitchDegrees = qRadiansToDegrees(qAtan2(
          munition.verticalSpeedMetersPerSecond,
          qMax(1.0, munition.speedMetersPerSecond)));
      munition.status = QStringLiteral("Falling");
    } else {
      const double pitchRadians = qDegreesToRadians(munition.pitchDegrees);
      const double totalDistanceMeters =
          munition.speedMetersPerSecond * deltaSeconds;
      horizontalDistanceMeters =
          qMax(0.0, totalDistanceMeters * qCos(pitchRadians));
      verticalDistanceMeters = totalDistanceMeters * qSin(pitchRadians);
    }

    const domain::GeoCoordinate nextPosition = domain::destinationPoint(
        munition.latitude,
        munition.longitude,
        munition.headingDegrees,
        horizontalDistanceMeters,
        munition.altitudeMeters);

    munition.latitude = nextPosition.latitude;
    munition.longitude = nextPosition.longitude;
    munition.altitudeMeters += verticalDistanceMeters;
    munition.ageSeconds += deltaSeconds;

    if (isBomb) {
      const int previousTrailIndex =
          qFloor(previousAgeSeconds / kBombSmokeTrailIntervalSeconds);
      const int currentTrailIndex =
          qFloor(munition.ageSeconds / kBombSmokeTrailIntervalSeconds);
      for (int i = previousTrailIndex + 1; i <= currentTrailIndex; ++i) {
        appendBombSmokeTrailEffect(effects, munition, i);
      }
    }

    // ── Hit detection ───────────────────────────────────────────────────────
    if (hasGuidedTarget) {
      if (munition.guidanceActive && trackedTarget && !trackedTarget->destroyed) {
        const double hz = domain::distanceMeters(
            munition.latitude, munition.longitude,
            trackedTarget->latitude, trackedTarget->longitude);
        const double vz = qAbs(
            munition.altitudeMeters -
            static_cast<double>(trackedTarget->altitude));
        if (qSqrt(qPow(hz, 2.0) + qPow(vz, 2.0)) <= munition.hitRadiusMeters) {
          applyDamageFn(domain::entityKey(*trackedTarget), 25.0);
          if (isBomb) {
            appendBombImpactEffects(effects, munition);
          } else {
            effects.push_back(makeTransientEffect(
                munition.id + QStringLiteral("-impact"),
                QStringLiteral("ImpactFlash"),
                munition.forceIdentifier,
                munition.latitude, munition.longitude,
                munition.altitudeMeters,
                kImpactFlashTtlSeconds));
          }
          munition.active = false;
          continue;
        }
      }
    } else {
      QString impactedEntityName;
      double bestImpactRange = munition.hitRadiusMeters;
      for (const Entity& entity : entities) {
        const QString launcherReference =
            munition.launcherEntityId.trimmed().isEmpty()
                ? munition.launcherEntityName
                : munition.launcherEntityId;
        if (entity.destroyed ||
            domain::entityMatchesReference(entity, launcherReference) ||
            entity.forceIdentifier == munition.forceIdentifier) {
          continue;
        }
        const double hz = domain::distanceMeters(
            munition.latitude, munition.longitude,
            entity.latitude, entity.longitude);
        const double vz = qAbs(
            munition.altitudeMeters - static_cast<double>(entity.altitude));
        const double slant = qSqrt(qPow(hz, 2.0) + qPow(vz, 2.0));
        if (slant <= bestImpactRange) {
          bestImpactRange = slant;
          impactedEntityName = domain::entityKey(entity);
        }
      }

      if (!impactedEntityName.isEmpty()) {
        if (isBomb) {
          applyBombBlastFn(munition);
          appendBombImpactEffects(effects, munition);
        } else {
          applyDamageFn(impactedEntityName, 25.0);
          effects.push_back(makeTransientEffect(
              munition.id + QStringLiteral("-impact"),
              QStringLiteral("ImpactFlash"),
              munition.forceIdentifier,
              munition.latitude, munition.longitude,
              munition.altitudeMeters,
              kImpactFlashTtlSeconds));
        }
        munition.active = false;
        continue;
      }
    }

    // ── Ground / TTL expiry ─────────────────────────────────────────────────
    if (munition.altitudeMeters <= kMinimumMunitionAltitudeMeters) {
      if (isBomb) {
        applyBombBlastFn(munition);
        appendBombImpactEffects(effects, munition);
      }
      munition.active = false;
      continue;
    }

    if (munition.ageSeconds >= munition.ttlSeconds) {
      munition.active = false;
    }
  }

  // Remove inactive munitions in reverse order to preserve indices.
  for (qsizetype i = activeMunitions.size() - 1; i >= 0; --i) {
    if (activeMunitions.at(i).active) {
      continue;
    }
    activeMunitions.removeAt(i);
    if (i == 0) {
      break;
    }
  }
}

void advanceTransientEffects(QVector<TransientEffect>& effects, double deltaSeconds) {
  if (deltaSeconds <= 0.0 || effects.isEmpty()) {
    return;
  }
  for (TransientEffect& effect : effects) {
    if (!effect.active) {
      continue;
    }
    effect.ageSeconds += deltaSeconds;
    if (effect.ageSeconds >= effect.ttlSeconds) {
      effect.active = false;
    }
  }
  for (qsizetype i = effects.size() - 1; i >= 0; --i) {
    if (!effects.at(i).active) {
      effects.removeAt(i);
    }
    if (i == 0) {
      break;
    }
  }
}

} // namespace application
