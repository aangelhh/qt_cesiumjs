#include "presentation/KinematicsCockpitModel.h"

#include <QtGlobal>

namespace presentation {
namespace {

constexpr double kMetersToFeet = 3.28083989501312;
constexpr double kMetersPerSecondToThousandsFeetPerMinute =
    kMetersToFeet * 60.0 / 1000.0;

} // namespace

KinematicsCockpitData makeKinematicsCockpitData(
    const application::KinematicsTelemetrySnapshot& snapshot) {
  KinematicsCockpitData data;
  data.entityName = snapshot.entityName;
  data.rollDegrees = snapshot.rollDegrees;
  data.pitchDegrees = snapshot.pitchDegrees;
  data.headingDegrees = snapshot.headingDegrees;
  data.airspeedKnots = qMax(0.0, snapshot.speedKnots);
  data.altitudeFeet = snapshot.altitudeMeters * kMetersToFeet;
  data.climbRateThousandsFeetPerMinute =
      snapshot.verticalSpeedMetersPerSecond *
      kMetersPerSecondToThousandsFeetPerMinute;
  data.maximumAirspeedKnots = qMax(0.0, snapshot.maxSpeedKnots);
  data.flightDirectorActive =
      snapshot.taskEnabled &&
      snapshot.taskStatus.compare(
          QStringLiteral("Running"),
          Qt::CaseInsensitive) == 0;
  data.selectedHeadingDegrees = data.flightDirectorActive
      ? snapshot.targetHeadingDegrees
      : snapshot.headingDegrees;
  data.selectedAirspeedKnots = data.flightDirectorActive
      ? qMax(0.0, snapshot.targetSpeedKnots)
      : data.airspeedKnots;
  data.selectedAltitudeFeet = data.flightDirectorActive
      ? snapshot.targetAltitudeMeters * kMetersToFeet
      : data.altitudeFeet;
  data.systemsProfileId = snapshot.systems.profileId;
  data.systemsDataSource = snapshot.systems.dataSource;
  for (const application::EngineTelemetry& engine : snapshot.systems.engines) {
    QVariantMap values;
    values.insert(QStringLiteral("engineId"), engine.engineId);
    values.insert(QStringLiteral("state"), engine.state);
    values.insert(QStringLiteral("n1Percent"), engine.n1Percent);
    values.insert(QStringLiteral("n2Percent"), engine.n2Percent);
    values.insert(
        QStringLiteral("exhaustTemperatureCelsius"),
        engine.exhaustTemperatureCelsius);
    values.insert(
        QStringLiteral("fuelFlowKilogramsPerHour"),
        engine.fuelFlowKilogramsPerHour);
    values.insert(
        QStringLiteral("thrustKilonewtons"),
        engine.thrustKilonewtons);
    values.insert(QStringLiteral("n1Available"), engine.n1Available);
    values.insert(QStringLiteral("n2Available"), engine.n2Available);
    values.insert(
        QStringLiteral("exhaustTemperatureAvailable"),
        engine.exhaustTemperatureAvailable);
    values.insert(
        QStringLiteral("fuelFlowAvailable"),
        engine.fuelFlowAvailable);
    values.insert(
        QStringLiteral("thrustAvailable"),
        engine.thrustAvailable);
    values.insert(QStringLiteral("available"), engine.available);
    values.insert(QStringLiteral("estimated"), engine.estimated);
    data.engines.push_back(values);
  }
  data.available =
      !snapshot.destroyed &&
      snapshot.domain.compare(
          QStringLiteral("Air"),
          Qt::CaseInsensitive) == 0;
  data.statusText = data.available
      ? QStringLiteral("%1 | %2 | %3")
            .arg(snapshot.dynamicsModel,
                 snapshot.taskType.isEmpty()
                     ? QStringLiteral("No active task")
                     : snapshot.taskType,
                 snapshot.taskStatus)
      : (snapshot.destroyed
             ? QStringLiteral("Entity destroyed")
             : QStringLiteral("Air entity required"));
  return data;
}

} // namespace presentation
