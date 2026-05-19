#pragma once

#include <QVariantMap>
#include <QString>

struct Entity;
struct ActiveMunition;
struct TransientEffect;
struct PendingBombRelease;

namespace presentation {

/// Creates a QVariantMap with all track fields pre-populated with defaults.
QVariantMap makeTrackSummary(
    const QString& name,
    const QString& type,
    const QString& team,
    const QString& altitudeText,
    const QString& positionText,
    const QString& status,
    double latitude,
    double longitude);

/// Track summary for an active munition (missile or bomb).
QVariantMap makeMunitionTrackSummary(const ActiveMunition& munition);

/// Track summary for a transient visual effect (explosion, smoke, etc.)
QVariantMap makeTransientEffectTrackSummary(const TransientEffect& effect);

/// Track summary for the pending bomb target marker.
QVariantMap makePendingBombTargetTrackSummary(
    const QString& targetLabel,
    double latitude,
    double longitude,
    double targetAltitudeMeters,
    const QString& teamLabel,
    const QString& releaseStateLabel,
    double distanceMetersToTarget);

/// Track summary for the line connecting launcher to pending bomb target.
QVariantMap makePendingBombTargetLineTrackSummary(
    const Entity& launcher,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters,
    const QString& teamLabel,
    const QString& releaseStateLabel);

/// One-line display labels for entities used as weapon targets.
QString missileTargetDisplayLabel(const Entity& entity, double rangeMeters);
QString bombTargetDisplayLabel(const Entity& entity);

} // namespace presentation
