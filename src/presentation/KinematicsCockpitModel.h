#pragma once

#include "application/KinematicsTelemetry.h"

#include <QString>
#include <QVariantList>

namespace presentation {

struct KinematicsCockpitData {
  QString entityId;
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
  double fuelCapacityKilograms = 0.0;
  double fuelRemainingKilograms = 0.0;
  double fuelPercent = 0.0;
  double totalFuelFlowKilogramsPerHour = 0.0;
  double estimatedEnduranceSeconds = 0.0;
  bool fuelAvailable = false;
  bool enduranceAvailable = false;
  QVariantList engines;
};

KinematicsCockpitData makeKinematicsCockpitData(
    const application::KinematicsTelemetrySnapshot& snapshot);

} // namespace presentation
