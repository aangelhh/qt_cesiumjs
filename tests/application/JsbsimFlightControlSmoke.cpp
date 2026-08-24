#include <gtest/gtest.h>

#include "application/FlightDynamicsEngine.h"
#include "domain/EntityIdentity.h"
#include "domain/GeoMath.h"

#include <cmath>
#include <unordered_map>

TEST(JsbsimFlightControl, FighterSetpointsRemainStable) {
  Entity fighter;
  fighter.name = QStringLiteral("JSBSim smoke fighter");
  fighter.domain = QStringLiteral("Air");
  fighter.category = QStringLiteral("Fighter");
  fighter.latitude = 40.0;
  fighter.longitude = -3.0;
  fighter.altitude = 3000;
  fighter.headingDegrees = 90.0;
  fighter.speedKnots = 320.0;
  fighter.flightDynamicsEnabled = true;
  fighter.flightDynamicsMode = QStringLiteral("jsbsim");
  fighter.jsbsimAircraftModel = QStringLiteral("f16");
  // Legacy scenarios may contain a JSBSim model without controlProfileId.
  fighter.controlProfileId.clear();
  fighter.currentTask.enabled = true;
  fighter.currentTask.status = QStringLiteral("Running");
  fighter.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  fighter.currentTask.targetHeadingDegrees = 110.0;
  fighter.currentTask.targetAltitudeMeters = 5200;
  fighter.currentTask.targetSpeedKnots = 210.0;

  QVector<Entity> entities = {fighter};
  std::unordered_map<QString, domain::TaskStack> taskStacks;
  const double initialHeadingError = qAbs(domain::shortestSignedAngle(
      fighter.headingDegrees,
      fighter.currentTask.targetHeadingDegrees));

  for (int tick = 0; tick < 600; ++tick) {
    FlightDynamicsEngine::advanceEntities(
        entities,
        taskStacks,
        1.0 / 60.0);
    ASSERT_EQ(entities.size(), 1);
    const Entity& current = entities.first();
    ASSERT_TRUE(std::isfinite(current.latitude));
    ASSERT_TRUE(std::isfinite(current.longitude));
    ASSERT_TRUE(std::isfinite(current.headingDegrees));
    ASSERT_TRUE(std::isfinite(current.pitchDegrees));
    ASSERT_TRUE(std::isfinite(current.rollDegrees));
    ASSERT_TRUE(std::isfinite(current.speedKnots));
    ASSERT_GT(current.altitude, 0);
    ASSERT_LT(qAbs(current.pitchDegrees), 45.0);
    ASSERT_LT(qAbs(current.rollDegrees), 70.0);
    ASSERT_GT(current.speedKnots, 120.0);
    ASSERT_LT(current.speedKnots, 900.0);
  }

  const Entity& result = entities.first();
  EXPECT_EQ(result.activeDynamicsBackend, QStringLiteral("jsbsim"));
  EXPECT_EQ(result.controlProfileId, QStringLiteral("fighter-generic"));
  const double finalHeadingError = qAbs(domain::shortestSignedAngle(
      result.headingDegrees,
      result.currentTask.targetHeadingDegrees));
  EXPECT_LT(finalHeadingError, initialHeadingError);
  EXPECT_GT(result.altitude, fighter.altitude);
  EXPECT_GT(result.fuelCapacityKilograms, 0.0);
  EXPECT_GT(result.fuelRemainingKilograms, 0.0);
  EXPECT_LT(result.fuelRemainingKilograms, 3200.0);

  const application::SystemsTelemetrySnapshot systems =
      FlightDynamicsEngine::systemsTelemetryForEntity(result, 900.0);
  EXPECT_EQ(systems.dataSource, QStringLiteral("JSBSim | f16"));
  ASSERT_EQ(systems.engines.size(), 1);
  const application::EngineTelemetry& engine = systems.engines.front();
  EXPECT_EQ(engine.state, QStringLiteral("RUNNING"));
  EXPECT_TRUE(engine.n1Available);
  EXPECT_TRUE(engine.n2Available);
  EXPECT_TRUE(engine.fuelFlowAvailable);
  EXPECT_TRUE(engine.thrustAvailable);
  EXPECT_GT(engine.n1Percent, 0.0);
  EXPECT_GT(engine.n2Percent, 0.0);
  EXPECT_GT(engine.fuelFlowKilogramsPerHour, 0.0);
  EXPECT_GT(engine.thrustKilonewtons, 0.0);
  EXPECT_TRUE(systems.fuelAvailable);
  EXPECT_GT(systems.fuelPercent, 0.0);
  EXPECT_LT(systems.fuelPercent, 100.0);
  EXPECT_TRUE(systems.enduranceAvailable);
}

TEST(JsbsimFlightControl, HeadingSetpointProducesBankAndHorizontalTurn) {
  Entity fighter;
  fighter.name = QStringLiteral("JSBSim heading-only fighter");
  fighter.domain = QStringLiteral("Air");
  fighter.category = QStringLiteral("Fighter");
  fighter.latitude = 40.0;
  fighter.longitude = -3.0;
  fighter.altitude = 3000;
  fighter.headingDegrees = 90.0;
  fighter.speedKnots = 320.0;
  fighter.flightDynamicsEnabled = true;
  fighter.flightDynamicsMode = QStringLiteral("jsbsim");
  fighter.jsbsimAircraftModel = QStringLiteral("f16");
  fighter.controlProfileId = QStringLiteral("fighter-generic");
  fighter.currentTask.enabled = true;
  fighter.currentTask.status = QStringLiteral("Running");
  fighter.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  fighter.currentTask.targetHeadingDegrees = 135.0;
  fighter.currentTask.targetAltitudeMeters = fighter.altitude;
  fighter.currentTask.targetSpeedKnots = fighter.speedKnots;

  QVector<Entity> entities = {fighter};
  std::unordered_map<QString, domain::TaskStack> taskStacks;
  const double initialHeadingError = qAbs(domain::shortestSignedAngle(
      fighter.headingDegrees,
      fighter.currentTask.targetHeadingDegrees));
  double maximumAbsBankDegrees = 0.0;
  double maximumAbsPitchDegrees = 0.0;

  for (int tick = 0; tick < 900; ++tick) {
    FlightDynamicsEngine::advanceEntities(
        entities,
        taskStacks,
        1.0 / 60.0);
    ASSERT_EQ(entities.size(), 1);
    maximumAbsBankDegrees = qMax(
        maximumAbsBankDegrees,
        qAbs(entities.first().rollDegrees));
    maximumAbsPitchDegrees = qMax(
        maximumAbsPitchDegrees,
        qAbs(entities.first().pitchDegrees));
  }

  const Entity& result = entities.first();
  const double finalHeadingError = qAbs(domain::shortestSignedAngle(
      result.headingDegrees,
      result.currentTask.targetHeadingDegrees));
  EXPECT_GT(maximumAbsBankDegrees, 5.0);
  EXPECT_LT(finalHeadingError, initialHeadingError);
  EXPECT_LT(maximumAbsPitchDegrees, maximumAbsBankDegrees);
  EXPECT_LT(qAbs(result.altitude - fighter.altitude), 500);
}

TEST(JsbsimFlightControl, MoveToLocationUsesJsbsimInsteadOfKinematicBypass) {
  Entity fighter;
  fighter.name = QStringLiteral("JSBSim move-to fighter");
  fighter.domain = QStringLiteral("Air");
  fighter.category = QStringLiteral("Fighter");
  fighter.latitude = 40.0;
  fighter.longitude = -3.0;
  fighter.altitude = 3000;
  fighter.headingDegrees = 90.0;
  fighter.speedKnots = 320.0;
  fighter.flightDynamicsEnabled = true;
  fighter.flightDynamicsMode = QStringLiteral("jsbsim");
  fighter.jsbsimAircraftModel = QStringLiteral("f16");
  fighter.currentTask.enabled = true;
  fighter.currentTask.status = QStringLiteral("Running");
  fighter.currentTask.taskType = QStringLiteral("MoveToLocation");
  fighter.currentTask.targetLatitude = 40.05;
  fighter.currentTask.targetLongitude = -2.95;
  fighter.currentTask.targetAltitudeMeters = 3500;
  fighter.currentTask.targetSpeedKnots = 340.0;

  QVector<Entity> entities = {fighter};
  std::unordered_map<QString, domain::TaskStack> taskStacks;
  for (int tick = 0; tick < 120; ++tick) {
    FlightDynamicsEngine::advanceEntities(
        entities,
        taskStacks,
        1.0 / 60.0);
  }

  ASSERT_EQ(entities.size(), 1);
  EXPECT_EQ(
      entities.first().activeDynamicsBackend,
      QStringLiteral("jsbsim"));
  const auto systems = FlightDynamicsEngine::systemsTelemetryForEntity(
      entities.first(),
      900.0);
  EXPECT_EQ(systems.dataSource, QStringLiteral("JSBSim | f16"));
  ASSERT_EQ(systems.engines.size(), 1);
  EXPECT_TRUE(systems.engines.front().available);
}

TEST(JsbsimFlightControl, EmptyFuelStopsBeforeJsbsimCanMoveEntity) {
  Entity fighter;
  fighter.name = QStringLiteral("JSBSim empty-fuel fighter");
  fighter.domain = QStringLiteral("Air");
  fighter.category = QStringLiteral("Fighter");
  fighter.latitude = 40.0;
  fighter.longitude = -3.0;
  fighter.altitude = 3000;
  fighter.headingDegrees = 90.0;
  fighter.speedKnots = 320.0;
  fighter.flightDynamicsEnabled = true;
  fighter.flightDynamicsMode = QStringLiteral("jsbsim");
  fighter.jsbsimAircraftModel = QStringLiteral("f16");
  fighter.fuelCapacityKilograms = 5875.0;
  fighter.fuelRemainingKilograms = 0.0;
  fighter.currentTask.enabled = true;
  fighter.currentTask.status = QStringLiteral("Running");
  fighter.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  fighter.currentTask.targetHeadingDegrees = 120.0;
  fighter.currentTask.targetAltitudeMeters = 4000;
  fighter.currentTask.targetSpeedKnots = 400.0;

  QVector<Entity> entities = {fighter};
  std::unordered_map<QString, domain::TaskStack> taskStacks;
  FlightDynamicsEngine::advanceEntities(entities, taskStacks, 1.0);

  ASSERT_EQ(entities.size(), 1);
  const Entity& result = entities.first();
  EXPECT_DOUBLE_EQ(result.latitude, fighter.latitude);
  EXPECT_DOUBLE_EQ(result.longitude, fighter.longitude);
  EXPECT_EQ(result.altitude, fighter.altitude);
  EXPECT_DOUBLE_EQ(result.speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(result.verticalSpeedMetersPerSecond, 0.0);
  EXPECT_EQ(result.currentTask.status, QStringLiteral("Failed"));
  EXPECT_EQ(result.activeDynamicsBackend, QStringLiteral("fuel-exhausted"));
}

TEST(JsbsimFlightControl, RefuelSynchronizesAnActiveJsbsimSession) {
  Entity fighter;
  fighter.name = QStringLiteral("JSBSim refuel fighter");
  fighter.domain = QStringLiteral("Air");
  fighter.category = QStringLiteral("Fighter");
  fighter.latitude = 40.0;
  fighter.longitude = -3.0;
  fighter.altitude = 3000;
  fighter.headingDegrees = 90.0;
  fighter.speedKnots = 320.0;
  fighter.flightDynamicsEnabled = true;
  fighter.flightDynamicsMode = QStringLiteral("jsbsim");
  fighter.jsbsimAircraftModel = QStringLiteral("f16");
  fighter.fuelCapacityKilograms = 5875.0;
  fighter.fuelRemainingKilograms = 2000.0;
  fighter.currentTask.enabled = true;
  fighter.currentTask.status = QStringLiteral("Running");
  fighter.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  fighter.currentTask.targetHeadingDegrees = 90.0;
  fighter.currentTask.targetAltitudeMeters = 3000;
  fighter.currentTask.targetSpeedKnots = 320.0;

  QVector<Entity> entities = {fighter};
  std::unordered_map<QString, domain::TaskStack> taskStacks;
  FlightDynamicsEngine::advanceEntities(entities, taskStacks, 1.0);
  ASSERT_EQ(entities.first().activeDynamicsBackend, QStringLiteral("jsbsim"));

  ASSERT_TRUE(FlightDynamicsEngine::setFuelRemaining(entities.first(), 500.0));
  FlightDynamicsEngine::advanceEntities(entities, taskStacks, 1.0);

  EXPECT_GT(entities.first().fuelRemainingKilograms, 0.0);
  EXPECT_LT(entities.first().fuelRemainingKilograms, 500.0);
}

TEST(JsbsimFlightControl, ReleasedModelReinitializesFromCurrentEntityState) {
  Entity fighter;
  fighter.name = QStringLiteral("JSBSim lifecycle fighter");
  fighter.domain = QStringLiteral("Air");
  fighter.category = QStringLiteral("Fighter");
  fighter.latitude = 40.0;
  fighter.longitude = -3.0;
  fighter.altitude = 3000;
  fighter.headingDegrees = 90.0;
  fighter.speedKnots = 320.0;
  fighter.flightDynamicsEnabled = true;
  fighter.flightDynamicsMode = QStringLiteral("jsbsim");
  fighter.jsbsimAircraftModel = QStringLiteral("f16");
  fighter.currentTask.enabled = true;
  fighter.currentTask.status = QStringLiteral("Running");
  fighter.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  fighter.currentTask.targetHeadingDegrees = 90.0;
  fighter.currentTask.targetAltitudeMeters = 3000;
  fighter.currentTask.targetSpeedKnots = 320.0;

  QVector<Entity> entities = {fighter};
  std::unordered_map<QString, domain::TaskStack> taskStacks;
  FlightDynamicsEngine::advanceEntities(entities, taskStacks, 1.0 / 60.0);
  ASSERT_EQ(entities.first().activeDynamicsBackend, QStringLiteral("jsbsim"));

  FlightDynamicsEngine::releaseDynamicsModel(
      domain::entityKey(entities.first()));
  entities.first().latitude = 41.0;
  entities.first().longitude = -4.0;
  entities.first().altitude = 4000;
  entities.first().currentTask.targetAltitudeMeters = 4000;

  FlightDynamicsEngine::advanceEntities(entities, taskStacks, 1.0 / 60.0);
  EXPECT_EQ(entities.first().activeDynamicsBackend, QStringLiteral("jsbsim"));
  EXPECT_NEAR(entities.first().latitude, 41.0, 0.01);
  EXPECT_NEAR(entities.first().longitude, -4.0, 0.01);
  EXPECT_NEAR(entities.first().altitude, 4000, 10);

  FlightDynamicsEngine::clearDynamicsModels();
}
