#include <gtest/gtest.h>

#include "presentation/KinematicsCockpitModel.h"

namespace {

application::KinematicsTelemetrySnapshot makeSnapshot() {
  application::KinematicsTelemetrySnapshot snapshot;
  snapshot.entityName = QStringLiteral("Falcon 1");
  snapshot.domain = QStringLiteral("Air");
  snapshot.altitudeMeters = 3000.0;
  snapshot.verticalSpeedMetersPerSecond = 10.0;
  snapshot.headingDegrees = 275.0;
  snapshot.pitchDegrees = 5.0;
  snapshot.rollDegrees = -20.0;
  snapshot.speedKnots = 320.0;
  snapshot.targetHeadingDegrees = 290.0;
  snapshot.targetAltitudeMeters = 5000.0;
  snapshot.targetSpeedKnots = 400.0;
  snapshot.maxSpeedKnots = 900.0;
  snapshot.taskEnabled = true;
  snapshot.taskType = QStringLiteral("InterceptEntity");
  snapshot.taskStatus = QStringLiteral("Running");
  snapshot.dynamicsModel = QStringLiteral("kinematic");
  snapshot.systems.profileId = QStringLiteral("air-turbine-1-engine");
  snapshot.systems.dataSource = QStringLiteral("Kinematic estimate");
  application::EngineTelemetry engine;
  engine.engineId = QStringLiteral("ENG 1");
  engine.state = QStringLiteral("RUNNING");
  engine.n1Percent = 73.0;
  engine.n1Available = true;
  engine.estimated = true;
  engine.available = true;
  snapshot.systems.engines.push_back(engine);
  return snapshot;
}

} // namespace

TEST(KinematicsCockpitModel, ConvertsInternalAltitudeAndVerticalSpeedUnits) {
  const auto data = presentation::makeKinematicsCockpitData(makeSnapshot());

  EXPECT_NEAR(data.altitudeFeet, 9842.519685, 1.0e-5);
  EXPECT_NEAR(data.selectedAltitudeFeet, 16404.199475, 1.0e-5);
  EXPECT_NEAR(data.climbRateThousandsFeetPerMinute, 1.968503937, 1.0e-9);
}

TEST(KinematicsCockpitModel, MapsActualAndSelectedFlightValues) {
  const auto data = presentation::makeKinematicsCockpitData(makeSnapshot());

  EXPECT_EQ(data.entityName, QStringLiteral("Falcon 1"));
  EXPECT_DOUBLE_EQ(data.headingDegrees, 275.0);
  EXPECT_DOUBLE_EQ(data.pitchDegrees, 5.0);
  EXPECT_DOUBLE_EQ(data.rollDegrees, -20.0);
  EXPECT_DOUBLE_EQ(data.airspeedKnots, 320.0);
  EXPECT_DOUBLE_EQ(data.selectedHeadingDegrees, 290.0);
  EXPECT_DOUBLE_EQ(data.selectedAirspeedKnots, 400.0);
  EXPECT_DOUBLE_EQ(data.maximumAirspeedKnots, 900.0);
  EXPECT_TRUE(data.flightDirectorActive);
  EXPECT_TRUE(data.available);
  EXPECT_EQ(data.systemsProfileId, QStringLiteral("air-turbine-1-engine"));
  ASSERT_EQ(data.engines.size(), 1);
  EXPECT_DOUBLE_EQ(
      data.engines.front().toMap().value(QStringLiteral("n1Percent")).toDouble(),
      73.0);
}

TEST(KinematicsCockpitModel, RejectsGroundAndDestroyedEntities) {
  auto snapshot = makeSnapshot();
  snapshot.domain = QStringLiteral("Ground");
  EXPECT_FALSE(presentation::makeKinematicsCockpitData(snapshot).available);

  snapshot.domain = QStringLiteral("Air");
  snapshot.destroyed = true;
  const auto destroyed = presentation::makeKinematicsCockpitData(snapshot);
  EXPECT_FALSE(destroyed.available);
  EXPECT_EQ(destroyed.statusText, QStringLiteral("Entity destroyed"));
}

TEST(KinematicsCockpitModel, FlightDirectorRequiresRunningEnabledTask) {
  auto snapshot = makeSnapshot();
  snapshot.taskStatus = QStringLiteral("Completed");
  EXPECT_FALSE(
      presentation::makeKinematicsCockpitData(snapshot).flightDirectorActive);

  snapshot.taskStatus = QStringLiteral("Running");
  snapshot.taskEnabled = false;
  EXPECT_FALSE(
      presentation::makeKinematicsCockpitData(snapshot).flightDirectorActive);
}
