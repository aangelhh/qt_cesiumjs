#include "presentation/EntityDefaultsResolver.h"

namespace presentation {

bool summaryHasRunningTaskTargets(const QVariantMap& summary) {
  return summary.value(QStringLiteral("taskEnabled")).toBool() &&
      summary.value(QStringLiteral("taskStatus")).toString() == QStringLiteral("Running") &&
      !summary.value(QStringLiteral("taskType")).toString().trimmed().isEmpty();
}

bool resolveFlyTargetsFromSummary(
    const QVariantMap& summary,
    int                currentAltitudeMeters,
    double&            outHeadingDegrees,
    int&               outAltitudeMeters,
    double&            outSpeedKnots) {
  if (summary.isEmpty()) {
    return false;
  }

  const bool running = summaryHasRunningTaskTargets(summary);

  outHeadingDegrees = running
      ? summary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble()
      : summary.value(QStringLiteral("headingDegrees")).toDouble();

  outAltitudeMeters = running
      ? summary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt()
      : currentAltitudeMeters;

  outSpeedKnots = running
      ? summary.value(QStringLiteral("taskTargetSpeedKnots")).toDouble()
      : summary.value(QStringLiteral("speedKnots")).toDouble();

  return true;
}

void resolvePlanStepDefaults(
    const Entity&      entity,
    const QVariantMap& summary,
    double&            outHeadingDegrees,
    int&               outAltitudeMeters,
    double&            outSpeedKnots) {
  const bool running = summaryHasRunningTaskTargets(summary);

  outHeadingDegrees = running
      ? summary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble()
      : entity.headingDegrees;

  outAltitudeMeters = running
      ? summary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt()
      : static_cast<int>(entity.altitude);

  outSpeedKnots = running
      ? summary.value(QStringLiteral("taskTargetSpeedKnots")).toDouble()
      : entity.speedKnots;

  if (outSpeedKnots <= 0.0) {
    outSpeedKnots =
        entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
            ? 220.0
            : 12.0;
  }
}

} // namespace presentation
