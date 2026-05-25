#pragma once

#include <QVariantMap>
#include "domain/Entity.h"

namespace presentation {

/// Returns true if the entity's track summary has a currently-running task
/// whose fly targets (heading/altitude/speed) should be used as defaults
/// instead of the entity's current telemetry.
bool summaryHasRunningTaskTargets(const QVariantMap& summary);

/// Resolves heading/altitude/speed defaults from a track summary.
///
/// If the summary indicates a running task, the task's target values are used.
/// Otherwise the entity's current telemetry values from the summary are used,
/// with \p currentAltitudeMeters supplied externally (ScenarioState query).
///
/// Returns true on success (summary is non-empty).
bool resolveFlyTargetsFromSummary(
    const QVariantMap& summary,
    int                currentAltitudeMeters,
    double&            outHeadingDegrees,
    int&               outAltitudeMeters,
    double&            outSpeedKnots);

/// Resolves plan-step defaults (heading/altitude/speed) for the given entity,
/// preferring running task targets when available, falling back to entity
/// telemetry. Applies a minimum speed for Air vs Surface domains.
void resolvePlanStepDefaults(
    const Entity&      entity,
    const QVariantMap& summary,
    double&            outHeadingDegrees,
    int&               outAltitudeMeters,
    double&            outSpeedKnots);

} // namespace presentation
