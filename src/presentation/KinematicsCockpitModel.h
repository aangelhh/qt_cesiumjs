#pragma once

#include "application/KinematicsTelemetry.h"

#include <QString>
#include <QVariantList>

namespace presentation {

struct KinematicsCockpitData {
  QString entityName;
  QString statusText;
  double rollDegrees = 0.0;
  double pitchDegrees = 0.0;
  double headingDegrees = 0.0;
  double airspeedKnots = 0.0;
  double altitudeFeet = 0.0;
  double climbRateThousandsFeetPerMinute = 0.0;
  double selectedHeadingDegrees = 0.0;
  double selectedAirspeedKnots = 0.0;
  double selectedAltitudeFeet = 0.0;
  double maximumAirspeedKnots = 0.0;
  bool flightDirectorActive = false;
  bool available = false;
  QString systemsProfileId;
  QString systemsDataSource;
  QVariantList engines;
};

KinematicsCockpitData makeKinematicsCockpitData(
    const application::KinematicsTelemetrySnapshot& snapshot);

} // namespace presentation
