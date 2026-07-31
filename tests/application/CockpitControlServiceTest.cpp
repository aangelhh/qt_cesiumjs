#include <gtest/gtest.h>

#include "application/CockpitControlService.h"
#include "application/ScenarioState.h"

namespace {

Entity makeAirEntity(const QString& name = QStringLiteral("Viper 1")) {
  Entity entity;
  entity.name = name;
  entity.domain = QStringLiteral("Air");
  entity.altitude = 3000;
  entity.headingDegrees = 90.0;
  entity.speedKnots = 300.0;
  return entity;
}

application::CockpitControlService makeService(
    ScenarioState& state,
    int& stoppedPlans) {
  return application::CockpitControlService(
      &state,
      [&state](const QString& entityName, const EntityTask& task, bool) {
        return state.assignTask(entityName, task);
      },
      [&stoppedPlans](const QString&, bool clearTask) {
        EXPECT_FALSE(clearTask);
        ++stoppedPlans;
      });
}

} // namespace

TEST(CockpitControlService, TakesAuthorityAndAppliesAutopilotSetpoints) {
  ScenarioState state;
  state.addEntity(makeAirEntity());
  int stoppedPlans = 0;
  auto service = makeService(state, stoppedPlans);

  application::FlightControlCommand command;
  command.entityName = QStringLiteral("Viper 1");
  command.headingDegrees = 370.0;
  command.altitudeMeters = 5000;
  command.speedKnots = 420.0;

  ASSERT_TRUE(service.takeControl(command));
  EXPECT_EQ(stoppedPlans, 1);
  EXPECT_TRUE(service.hasControl(command.entityName));
  EXPECT_EQ(
      service.authorityFor(command.entityName),
      application::EntityControlAuthority::CockpitAutopilot);

  const Entity& entity = state.entities().front();
  EXPECT_EQ(entity.currentTask.taskType, QStringLiteral("FlyHeadingAltitudeSpeed"));
  EXPECT_EQ(entity.currentTask.status, QStringLiteral("Running"));
  EXPECT_DOUBLE_EQ(entity.currentTask.targetHeadingDegrees, 10.0);
  EXPECT_EQ(entity.currentTask.targetAltitudeMeters, 5000);
  EXPECT_DOUBLE_EQ(entity.currentTask.targetSpeedKnots, 420.0);
}

TEST(CockpitControlService, RejectsGroundAndDestroyedEntities) {
  ScenarioState state;
  Entity ground = makeAirEntity(QStringLiteral("Ground 1"));
  ground.domain = QStringLiteral("Ground");
  state.addEntity(ground);
  Entity destroyed = makeAirEntity(QStringLiteral("Destroyed 1"));
  destroyed.destroyed = true;
  state.addEntity(destroyed);
  int stoppedPlans = 0;
  auto service = makeService(state, stoppedPlans);

  EXPECT_FALSE(service.takeControl({QStringLiteral("Ground 1"), 0.0, 0, 0.0}));
  EXPECT_FALSE(service.takeControl({QStringLiteral("Destroyed 1"), 0.0, 0, 0.0}));
  EXPECT_EQ(stoppedPlans, 0);
}

TEST(CockpitControlService, OnlyUpdatesSetpointsWhileOwningAuthority) {
  ScenarioState state;
  state.addEntity(makeAirEntity());
  int stoppedPlans = 0;
  auto service = makeService(state, stoppedPlans);

  application::FlightControlCommand command{
      QStringLiteral("Viper 1"), 180.0, 6000, 450.0};
  EXPECT_FALSE(service.updateSetpoints(command));
  ASSERT_TRUE(service.takeControl(command));

  command.headingDegrees = 240.0;
  command.altitudeMeters = 7000;
  ASSERT_TRUE(service.updateSetpoints(command));
  EXPECT_DOUBLE_EQ(state.entities().front().currentTask.targetHeadingDegrees, 240.0);
  EXPECT_EQ(state.entities().front().currentTask.targetAltitudeMeters, 7000);
}

TEST(CockpitControlService, ReleaseClearsTaskAndStopsResidualMovement) {
  ScenarioState state;
  state.addEntity(makeAirEntity());
  int stoppedPlans = 0;
  auto service = makeService(state, stoppedPlans);
  const application::FlightControlCommand command{
      QStringLiteral("Viper 1"), 180.0, 6000, 450.0};
  ASSERT_TRUE(service.takeControl(command));

  ASSERT_TRUE(service.releaseControl(command.entityName));
  EXPECT_FALSE(service.hasControl(command.entityName));
  const Entity& entity = state.entities().front();
  EXPECT_FALSE(entity.currentTask.enabled);
  EXPECT_EQ(entity.currentTask.status, QStringLiteral("Idle"));
  EXPECT_DOUBLE_EQ(entity.speedKnots, 0.0);
  EXPECT_DOUBLE_EQ(entity.verticalSpeedMetersPerSecond, 0.0);
}

TEST(CockpitControlService, ReconcileDropsAuthorityAfterTaskOverride) {
  ScenarioState state;
  state.addEntity(makeAirEntity());
  int stoppedPlans = 0;
  auto service = makeService(state, stoppedPlans);
  ASSERT_TRUE(service.takeControl(
      {QStringLiteral("Viper 1"), 180.0, 6000, 450.0}));

  EntityTask overrideTask;
  overrideTask.enabled = true;
  overrideTask.taskType = QStringLiteral("ReturnToBase");
  service.reconcile(QStringLiteral("Viper 1"), overrideTask);

  EXPECT_FALSE(service.hasControl(QStringLiteral("Viper 1")));
}
