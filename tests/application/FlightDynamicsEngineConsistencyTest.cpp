#include <gtest/gtest.h>

#include "application/FlightDynamicsEngine.h"
#include "domain/Task.h"

#include <unordered_map>

namespace {

Entity makeMovingAirEntity(const QString& status) {
  Entity entity;
  entity.name = QStringLiteral("Aircraft");
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.latitude = 40.0;
  entity.longitude = -3.0;
  entity.altitude = 3000;
  entity.headingDegrees = 90.0;
  entity.speedKnots = 300.0;
  entity.verticalSpeedMetersPerSecond = 12.0;
  entity.currentTask.enabled = true;
  entity.currentTask.taskType = QStringLiteral("MoveToLocation");
  entity.currentTask.status = status;
  entity.currentTask.targetLatitude = 40.1;
  entity.currentTask.targetLongitude = -2.9;
  entity.currentTask.targetAltitudeMeters = 6000;
  entity.currentTask.targetHeadingDegrees = 45.0;
  entity.currentTask.targetSpeedKnots = 400.0;
  return entity;
}

void expectPositionUnchanged(
    const Entity& before,
    const Entity& after) {
  EXPECT_DOUBLE_EQ(after.latitude, before.latitude);
  EXPECT_DOUBLE_EQ(after.longitude, before.longitude);
  EXPECT_EQ(after.altitude, before.altitude);
}

} // namespace

TEST(FlightDynamicsEngineConsistency, CompletedTaskDoesNotMoveEntity) {
  QVector<Entity> entities = {makeMovingAirEntity(QStringLiteral("Completed"))};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  expectPositionUnchanged(before, entities.first());
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
}

TEST(FlightDynamicsEngineConsistency, FailedTaskDoesNotMoveEntity) {
  QVector<Entity> entities = {makeMovingAirEntity(QStringLiteral("Failed"))};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  expectPositionUnchanged(before, entities.first());
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
}

TEST(FlightDynamicsEngineConsistency, TargetUnavailableTaskDoesNotMoveEntity) {
  QVector<Entity> entities = {makeMovingAirEntity(QStringLiteral("Target unavailable"))};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  expectPositionUnchanged(before, entities.first());
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
}

TEST(FlightDynamicsEngineConsistency, DisabledTaskDoesNotMoveEntity) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Running"));
  entity.currentTask.enabled = false;
  QVector<Entity> entities = {entity};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  expectPositionUnchanged(before, entities.first());
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
}

TEST(FlightDynamicsEngineConsistency, GroundTerminalTaskKeepsGroundKinematicsSafe) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Completed"));
  entity.domain = QStringLiteral("Ground");
  entity.altitude = 250;
  entity.pitchDegrees = 8.0;
  entity.rollDegrees = -7.0;
  QVector<Entity> entities = {entity};
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  EXPECT_DOUBLE_EQ(entities.first().latitude, entity.latitude);
  EXPECT_DOUBLE_EQ(entities.first().longitude, entity.longitude);
  EXPECT_EQ(entities.first().altitude, entity.altitude);
  EXPECT_DOUBLE_EQ(entities.first().pitchDegrees, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().rollDegrees, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
}

TEST(FlightDynamicsEngineConsistency, GroundNonMovementTaskWithResidualSpeedDoesNotMove) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Running"));
  entity.domain = QStringLiteral("Ground");
  entity.altitude = 410;
  entity.speedKnots = 25.0;
  entity.verticalSpeedMetersPerSecond = 5.0;
  entity.pitchDegrees = 12.0;
  entity.rollDegrees = -9.0;
  entity.currentTask.taskType = QStringLiteral("WaitUntilTime");
  entity.currentTask.targetSpeedKnots = 30.0;
  QVector<Entity> entities = {entity};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  expectPositionUnchanged(before, entities.first());
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().pitchDegrees, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().rollDegrees, 0.0);
}

TEST(FlightDynamicsEngineConsistency, GroundAirOnlyMovementTaskDoesNotMoveOrClimb) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Running"));
  entity.domain = QStringLiteral("Ground");
  entity.altitude = 725;
  entity.speedKnots = 40.0;
  entity.verticalSpeedMetersPerSecond = 7.0;
  entity.pitchDegrees = 15.0;
  entity.rollDegrees = 11.0;
  entity.currentTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  entity.currentTask.targetAltitudeMeters = 5000;
  entity.currentTask.targetSpeedKnots = 180.0;
  QVector<Entity> entities = {entity};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  expectPositionUnchanged(before, entities.first());
  EXPECT_DOUBLE_EQ(entities.first().speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().pitchDegrees, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().rollDegrees, 0.0);
}

TEST(FlightDynamicsEngineConsistency, GroundMoveToLocationMovesHorizontallyOnly) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Running"));
  entity.domain = QStringLiteral("Ground");
  entity.altitude = 530;
  entity.speedKnots = 0.0;
  entity.verticalSpeedMetersPerSecond = 9.0;
  entity.pitchDegrees = 8.0;
  entity.rollDegrees = -5.0;
  entity.currentTask.taskType = QStringLiteral("MoveToLocation");
  entity.currentTask.targetLatitude = 40.05;
  entity.currentTask.targetLongitude = -2.95;
  entity.currentTask.targetAltitudeMeters = 9000;
  entity.currentTask.targetSpeedKnots = 20.0;
  QVector<Entity> entities = {entity};
  const Entity before = entities.first();
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 10.0);

  ASSERT_EQ(entities.size(), 1);
  EXPECT_NE(entities.first().latitude, before.latitude);
  EXPECT_NE(entities.first().longitude, before.longitude);
  EXPECT_EQ(entities.first().altitude, before.altitude);
  EXPECT_DOUBLE_EQ(entities.first().verticalSpeedMetersPerSecond, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().pitchDegrees, 0.0);
  EXPECT_DOUBLE_EQ(entities.first().rollDegrees, 0.0);
  EXPECT_GT(entities.first().speedKnots, 0.0);
}

TEST(FlightDynamicsEngineConsistency, AirTurnAndClimbDeriveStableAttitude) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Running"));
  entity.headingDegrees = 0.0;
  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
  entity.speedKnots = 300.0;
  entity.currentTask.taskType = QStringLiteral("MoveToLocation");
  entity.currentTask.targetLatitude = 40.0;
  entity.currentTask.targetLongitude = -2.0;
  entity.currentTask.targetAltitudeMeters = 5000;
  entity.currentTask.targetSpeedKnots = 300.0;
  QVector<Entity> entities = {entity};
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 1.0);

  ASSERT_EQ(entities.size(), 1);
  EXPECT_GT(entities.first().headingDegrees, entity.headingDegrees);
  EXPECT_GT(entities.first().pitchDegrees, 0.0);
  EXPECT_GT(entities.first().rollDegrees, 0.0);
  EXPECT_GT(entities.first().verticalSpeedMetersPerSecond, 0.0);
}

TEST(FlightDynamicsEngineConsistency, AirTurnAndDescentDeriveStableAttitude) {
  Entity entity = makeMovingAirEntity(QStringLiteral("Running"));
  entity.headingDegrees = 90.0;
  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
  entity.speedKnots = 300.0;
  entity.currentTask.taskType = QStringLiteral("MoveToLocation");
  entity.currentTask.targetLatitude = 41.0;
  entity.currentTask.targetLongitude = -3.0;
  entity.currentTask.targetAltitudeMeters = 1000;
  entity.currentTask.targetSpeedKnots = 300.0;
  QVector<Entity> entities = {entity};
  std::unordered_map<QString, domain::TaskStack> stacks;

  FlightDynamicsEngine::advanceEntities(entities, stacks, 1.0);

  ASSERT_EQ(entities.size(), 1);
  EXPECT_LT(entities.first().headingDegrees, entity.headingDegrees);
  EXPECT_LT(entities.first().pitchDegrees, 0.0);
  EXPECT_LT(entities.first().rollDegrees, 0.0);
  EXPECT_LT(entities.first().verticalSpeedMetersPerSecond, 0.0);
}
