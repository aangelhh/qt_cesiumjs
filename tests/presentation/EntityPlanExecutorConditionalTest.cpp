#include <gtest/gtest.h>

#include "application/ScenarioState.h"
#include "presentation/EntityPlanExecutor.h"

namespace {

Entity makeEntity(const QString& name, int force = 1) {
  Entity entity;
  entity.name = name;
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.forceIdentifier = force;
  entity.latitude = 40.0;
  entity.longitude = -3.0;
  entity.altitude = 3000;
  return entity;
}

PlanStep makeWaitStep(PlanStepKind kind, const QString& taskType) {
  PlanStep step;
  step.kind = kind;
  step.task.enabled = true;
  step.task.taskType = taskType;
  step.task.status = QStringLiteral("NotStarted");
  return step;
}

struct ExecutorFixture {
  ScenarioState state;
  QStringList logMessages;
  QString lastStatus;

  presentation::EntityPlanExecutor makeExecutor() {
    return presentation::EntityPlanExecutor(
        &state,
        [this](const QString& entityName, const EntityTask& task, bool) {
          return state.assignTask(entityName, task);
        },
        [this](const QString& message) { logMessages.append(message); },
        [this](const QString& status) { lastStatus = status; });
  }

  EntityPlan& preparePlan(
      presentation::EntityPlanExecutor& executor,
      const QString& entityName,
      const PlanStep& step) {
    EntityPlan& plan = executor.ensurePlan(entityName);
    plan.steps.clear();
    plan.steps.append(step);
    return plan;
  }
};

const Entity* findEntity(const ScenarioState& state, const QString& name) {
  for (const Entity& entity : state.entities()) {
    if (entity.name == name) {
      return &entity;
    }
  }
  return nullptr;
}

} // namespace

TEST(EntityPlanExecutorConditional, WaitUntilTargetDetectedCompletesOnMatchingContact) {
  ExecutorFixture fixture;
  fixture.state.addEntity(makeEntity(QStringLiteral("Own"), 1));
  Entity bandit = makeEntity(QStringLiteral("Bandit"), 2);
  fixture.state.addEntity(bandit);

  SensorContact contact;
  contact.targetEntityName = QStringLiteral("Bandit");
  contact.detected = true;
  for (Entity& entity : fixture.state.entitiesMutable()) {
    if (entity.name == QStringLiteral("Own")) {
      entity.sensorContacts.append(contact);
      break;
    }
  }

  auto executor = fixture.makeExecutor();
  PlanStep step = makeWaitStep(
      PlanStepKind::WaitUntilTargetDetected,
      QStringLiteral("WaitUntilTargetDetected"));
  step.task.targetDomain = QStringLiteral("Air");
  step.task.enemyOnly = true;
  step.task.timeoutSeconds = 10.0;
  EntityPlan& plan = fixture.preparePlan(executor, QStringLiteral("Own"), step);

  ASSERT_TRUE(executor.startPlan(QStringLiteral("Own")));
  executor.advancePlans();

  EXPECT_FALSE(plan.running);
  EXPECT_EQ(plan.status, QStringLiteral("Completed"));
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Completed"));
  ASSERT_NE(findEntity(fixture.state, QStringLiteral("Own")), nullptr);
  EXPECT_EQ(findEntity(fixture.state, QStringLiteral("Own"))->currentTask.status,
            QStringLiteral("Completed"));
}

TEST(EntityPlanExecutorConditional, WaitUntilTargetDestroyedCompletesWhenTargetDestroyed) {
  ExecutorFixture fixture;
  fixture.state.addEntity(makeEntity(QStringLiteral("Own"), 1));
  Entity target = makeEntity(QStringLiteral("Target"), 2);
  target.destroyed = true;
  fixture.state.addEntity(target);

  auto executor = fixture.makeExecutor();
  PlanStep step = makeWaitStep(
      PlanStepKind::WaitUntilTargetDestroyed,
      QStringLiteral("WaitUntilTargetDestroyed"));
  step.task.targetEntityName = QStringLiteral("Target");
  step.task.timeoutSeconds = 10.0;
  EntityPlan& plan = fixture.preparePlan(executor, QStringLiteral("Own"), step);

  ASSERT_TRUE(executor.startPlan(QStringLiteral("Own")));
  executor.advancePlans();

  EXPECT_EQ(plan.status, QStringLiteral("Completed"));
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Completed"));
}

TEST(EntityPlanExecutorConditional, WaitUntilDamagedCompletesAtThreshold) {
  ExecutorFixture fixture;
  fixture.state.addEntity(makeEntity(QStringLiteral("Own"), 1));
  Entity target = makeEntity(QStringLiteral("Target"), 2);
  target.damagePercent = 55.0;
  fixture.state.addEntity(target);

  auto executor = fixture.makeExecutor();
  PlanStep step = makeWaitStep(
      PlanStepKind::WaitUntilDamaged,
      QStringLiteral("WaitUntilDamaged"));
  step.task.targetEntityName = QStringLiteral("Target");
  step.task.damageThresholdPercent = 50.0;
  step.task.timeoutSeconds = 10.0;
  EntityPlan& plan = fixture.preparePlan(executor, QStringLiteral("Own"), step);

  ASSERT_TRUE(executor.startPlan(QStringLiteral("Own")));
  executor.advancePlans();

  EXPECT_EQ(plan.status, QStringLiteral("Completed"));
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Completed"));
}

TEST(EntityPlanExecutorConditional, WaitUntilTimeCompletesAfterDurationTicks) {
  ExecutorFixture fixture;
  fixture.state.addEntity(makeEntity(QStringLiteral("Own"), 1));

  auto executor = fixture.makeExecutor();
  PlanStep step = makeWaitStep(
      PlanStepKind::WaitUntilTime,
      QStringLiteral("WaitUntilTime"));
  step.task.durationSeconds = 2.0;
  EntityPlan& plan = fixture.preparePlan(executor, QStringLiteral("Own"), step);

  ASSERT_TRUE(executor.startPlan(QStringLiteral("Own")));
  executor.advancePlans();
  EXPECT_TRUE(plan.running);
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Running"));

  executor.advancePlans();
  EXPECT_EQ(plan.status, QStringLiteral("Completed"));
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Completed"));
}

TEST(EntityPlanExecutorConditional, WaitUntilInRangeCompletesWhenTargetIsClose) {
  ExecutorFixture fixture;
  fixture.state.addEntity(makeEntity(QStringLiteral("Own"), 1));
  Entity target = makeEntity(QStringLiteral("Target"), 2);
  target.latitude = 40.0001;
  target.longitude = -3.0001;
  fixture.state.addEntity(target);

  auto executor = fixture.makeExecutor();
  PlanStep step = makeWaitStep(
      PlanStepKind::WaitUntilInRange,
      QStringLiteral("WaitUntilInRange"));
  step.task.targetEntityName = QStringLiteral("Target");
  step.task.rangeMeters = 1000.0;
  step.task.timeoutSeconds = 10.0;
  EntityPlan& plan = fixture.preparePlan(executor, QStringLiteral("Own"), step);

  ASSERT_TRUE(executor.startPlan(QStringLiteral("Own")));
  executor.advancePlans();

  EXPECT_EQ(plan.status, QStringLiteral("Completed"));
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Completed"));
}

TEST(EntityPlanExecutorConditional, WaitUntilTargetDetectedFailsOnTimeout) {
  ExecutorFixture fixture;
  fixture.state.addEntity(makeEntity(QStringLiteral("Own"), 1));

  auto executor = fixture.makeExecutor();
  PlanStep step = makeWaitStep(
      PlanStepKind::WaitUntilTargetDetected,
      QStringLiteral("WaitUntilTargetDetected"));
  step.task.timeoutSeconds = 1.0;
  EntityPlan& plan = fixture.preparePlan(executor, QStringLiteral("Own"), step);

  ASSERT_TRUE(executor.startPlan(QStringLiteral("Own")));
  executor.advancePlans();

  EXPECT_FALSE(plan.running);
  EXPECT_EQ(plan.status, QStringLiteral("CompletedWithFailures"));
  EXPECT_EQ(plan.steps.at(0).status, QStringLiteral("Failed"));
  ASSERT_NE(findEntity(fixture.state, QStringLiteral("Own")), nullptr);
  EXPECT_EQ(findEntity(fixture.state, QStringLiteral("Own"))->currentTask.status,
            QStringLiteral("Failed"));
}
