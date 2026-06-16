#pragma once

#include <QVariantMap>
#include <QString>

struct Entity;
struct ActiveMunition;
struct TransientEffect;
struct PendingBombRelease;
struct Waypoint;
struct RouteGraphic;
struct AreaDefinition;

namespace presentation {

struct EntityVisualState;

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

/// Full entity track summary including all task, sensor, and visual fields.
/// Pure function — depends only on entity data and visual state.
QVariantMap makeEntityTrackSummary(
    const Entity& entity,
    const EntityVisualState& visualState);

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
    double distanceMetersToTarget,
    const QString& ccrpCueLabel = {},
    double releaseDistanceMeters = -1.0,
    double distanceErrorMeters = 0.0,
    double timeToImpactSeconds = -1.0);

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

/// Normalizes an entity category string to one of the known display categories,
/// returning "Other" for unrecognised values.
QString normalizeEntityCategory(const QString& category);

/// Track summaries for tactical graphics (pure functions).
QVariantMap makeWaypointTrackSummary(const Waypoint& waypoint);
QVariantMap makeRouteTrackSummary(const RouteGraphic& route);
QVariantMap makeAreaTrackSummary(const AreaDefinition& area);

} // namespace presentation
