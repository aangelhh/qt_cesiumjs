#include <gtest/gtest.h>

#include "application/Event.h"
#include "application/EventBus.h"
#include "application/KinematicsTelemetry.h"
#include "application/ScenarioState.h"

namespace {

Entity makeTelemetryEntity() {
  Entity entity;
  entity.name = QStringLiteral("Viper 1");
  entity.domain = QStringLiteral("Air");
  entity.category = QStringLiteral("Fighter");
  entity.forceIdentifier = 1;
  entity.latitude = 40.4168;
  entity.longitude = -3.7038;
  entity.altitude = 3200;
  entity.headingDegrees = 350.0;
  entity.pitchDegrees = 4.0;
  entity.rollDegrees = -12.0;
  entity.speedKnots = 310.0;
  entity.verticalSpeedMetersPerSecond = 15.0;
  entity.flightDynamicsMode = QStringLiteral("kinematic");
  entity.currentTask.taskType = QStringLiteral("InterceptEntity");
  entity.currentTask.status = QStringLiteral("Running");
  entity.currentTask.enabled = true;
  entity.currentTask.targetHeadingDegrees = 10.0;
  entity.currentTask.targetAltitudeMeters = 5000;
  entity.currentTask.targetSpeedKnots = 400.0;
  return entity;
}

} // namespace

TEST(KinematicsTelemetry, CapturesEntityStateSetpointsAndUnits) {
  const Entity entity = makeTelemetryEntity();

  const application::KinematicsTelemetrySnapshot snapshot =
      application::makeKinematicsTelemetrySnapshot(entity, 12.5, 0.033);

  EXPECT_EQ(snapshot.entityId, entity.entityId);
  EXPECT_EQ(snapshot.entityName, QStringLiteral("Viper 1"));
  EXPECT_EQ(snapshot.domain, QStringLiteral("Air"));
  EXPECT_DOUBLE_EQ(snapshot.simulationTimeSeconds, 12.5);
  EXPECT_DOUBLE_EQ(snapshot.deltaTimeSeconds, 0.033);
  EXPECT_DOUBLE_EQ(snapshot.latitudeDegrees, 40.4168);
  EXPECT_DOUBLE_EQ(snapshot.longitudeDegrees, -3.7038);
  EXPECT_DOUBLE_EQ(snapshot.altitudeMeters, 3200.0);
  EXPECT_DOUBLE_EQ(snapshot.headingDegrees, 350.0);
  EXPECT_DOUBLE_EQ(snapshot.pitchDegrees, 4.0);
  EXPECT_DOUBLE_EQ(snapshot.rollDegrees, -12.0);
  EXPECT_DOUBLE_EQ(snapshot.speedKnots, 310.0);
  EXPECT_DOUBLE_EQ(snapshot.verticalSpeedMetersPerSecond, 15.0);
  EXPECT_DOUBLE_EQ(snapshot.targetAltitudeMeters, 5000.0);
  EXPECT_EQ(snapshot.taskType, QStringLiteral("InterceptEntity"));
  EXPECT_EQ(snapshot.taskStatus, QStringLiteral("Running"));
  EXPECT_TRUE(snapshot.taskEnabled);
}

TEST(KinematicsTelemetry, ComputesSignedTrackingErrorsAndControllerEnvelope) {
  const Entity entity = makeTelemetryEntity();

  const application::KinematicsTelemetrySnapshot snapshot =
      application::makeKinematicsTelemetrySnapshot(entity, 1.0, 0.1);

  EXPECT_DOUBLE_EQ(snapshot.headingErrorDegrees, 20.0);
  EXPECT_DOUBLE_EQ(snapshot.altitudeErrorMeters, 1800.0);
  EXPECT_DOUBLE_EQ(snapshot.speedErrorKnots, 90.0);
  EXPECT_DOUBLE_EQ(snapshot.maxHeadingRateDegreesPerSecond, 6.0);
  EXPECT_DOUBLE_EQ(snapshot.maxAccelerationKnotsPerSecond, 18.0);
  EXPECT_DOUBLE_EQ(snapshot.maxClimbRateMetersPerSecond, 35.0);
  EXPECT_DOUBLE_EQ(snapshot.maxSpeedKnots, 900.0);
  EXPECT_DOUBLE_EQ(snapshot.maxAltitudeMeters, 16000.0);
}

TEST(KinematicsTelemetry, FallsBackToKinematicDynamicsModel) {
  Entity entity = makeTelemetryEntity();
  entity.flightDynamicsMode = QStringLiteral("   ");

  const application::KinematicsTelemetrySnapshot snapshot =
      application::makeKinematicsTelemetrySnapshot(entity, 0.0, 0.1);

  EXPECT_EQ(snapshot.dynamicsModel, QStringLiteral("kinematic"));
}

TEST(KinematicsTelemetry, CapturesDynamicsFallbackDiagnostics) {
  Entity entity = makeTelemetryEntity();
  entity.activeDynamicsBackend = QStringLiteral("kinematic-fallback");
  entity.dynamicsFallbackReason = QStringLiteral("Step budget exceeded");
  entity.dynamicsStepDurationMilliseconds = 12.5;

  const application::KinematicsTelemetrySnapshot snapshot =
      application::makeKinematicsTelemetrySnapshot(entity, 0.0, 0.1);

  EXPECT_EQ(snapshot.dynamicsModel, QStringLiteral("kinematic-fallback"));
  EXPECT_EQ(
      snapshot.dynamicsFallbackReason,
      QStringLiteral("Step budget exceeded"));
  EXPECT_DOUBLE_EQ(snapshot.dynamicsStepDurationMilliseconds, 12.5);
}

TEST(KinematicsTelemetry, SnapshotDoesNotAliasMutableEntityState) {
  Entity entity = makeTelemetryEntity();
  const application::KinematicsTelemetrySnapshot snapshot =
      application::makeKinematicsTelemetrySnapshot(entity, 2.0, 0.1);

  entity.name = QStringLiteral("Changed");
  entity.altitude = 9000;
  entity.currentTask.status = QStringLiteral("Completed");

  EXPECT_EQ(snapshot.entityName, QStringLiteral("Viper 1"));
  EXPECT_DOUBLE_EQ(snapshot.altitudeMeters, 3200.0);
  EXPECT_EQ(snapshot.taskStatus, QStringLiteral("Running"));
}

TEST(KinematicsTelemetryPublisher, PublishesAtControlledFrequency) {
  application::KinematicsTelemetryPublisher publisher(0.1);
  QVector<Entity> entities{makeTelemetryEntity()};

  EXPECT_TRUE(publisher.advance(entities, 0.04, 0.04).isEmpty());
  EXPECT_TRUE(publisher.advance(entities, 0.08, 0.04).isEmpty());

  const auto snapshots = publisher.advance(entities, 0.12, 0.04);
  ASSERT_EQ(snapshots.size(), 1);
  EXPECT_DOUBLE_EQ(snapshots.front().simulationTimeSeconds, 0.12);
  EXPECT_DOUBLE_EQ(snapshots.front().deltaTimeSeconds, 0.04);

  EXPECT_TRUE(publisher.advance(entities, 0.16, 0.04).isEmpty());
  EXPECT_EQ(publisher.advance(entities, 0.20, 0.04).size(), 1);
}

TEST(KinematicsTelemetryPublisher, ResetClearsAccumulatedPublicationTime) {
  application::KinematicsTelemetryPublisher publisher(0.1);
  QVector<Entity> entities{makeTelemetryEntity()};

  EXPECT_TRUE(publisher.advance(entities, 0.08, 0.08).isEmpty());
  publisher.reset();
  EXPECT_TRUE(publisher.advance(entities, 0.12, 0.04).isEmpty());
  EXPECT_EQ(publisher.advance(entities, 0.20, 0.08).size(), 1);
}

TEST(KinematicsTelemetryPublisher, InvalidPeriodUsesSafeDefault) {
  application::KinematicsTelemetryPublisher publisher(0.0);

  EXPECT_DOUBLE_EQ(publisher.publicationPeriodSeconds(), 0.1);
  EXPECT_TRUE(publisher.advance({}, 0.0, 0.0).isEmpty());
}

TEST(ScenarioStateTelemetry, PublishesSnapshotsFromAuthoritativeSimulationClock) {
  ScenarioState state;
  {
    auto lock = state.lock();
    state.entitiesMutable().push_back(makeTelemetryEntity());
  }

  QVector<application::KinematicsTelemetrySnapshot> received;
  auto& eventBus = application::EventBus::instance();
  const application::SubscriptionId subscriptionId =
      eventBus.subscribe<application::EventKinematicsTelemetryUpdated>(
          [&received](const application::EventKinematicsTelemetryUpdated& event) {
            received.push_back(event.snapshot);
          });

  state.advanceSimulation(0.05);
  EXPECT_TRUE(received.isEmpty());
  state.advanceSimulation(0.05);

  eventBus.unsubscribe<application::EventKinematicsTelemetryUpdated>(
      subscriptionId);
  ASSERT_EQ(received.size(), 1);
  EXPECT_DOUBLE_EQ(state.simulationTimeSeconds(), 0.1);
  EXPECT_DOUBLE_EQ(received.front().simulationTimeSeconds, 0.1);
  EXPECT_EQ(received.front().entityName, QStringLiteral("Viper 1"));
}

TEST(ScenarioStateTelemetry, PublishesChangingDerivedRollDuringKinematicTurn) {
  ScenarioState state;
  Entity entity = makeTelemetryEntity();
  entity.headingDegrees = 0.0;
  entity.pitchDegrees = 0.0;
  entity.rollDegrees = 0.0;
  entity.speedKnots = 300.0;
  entity.currentTask.taskType = QStringLiteral("MoveToLocation");
  entity.currentTask.targetLatitude = entity.latitude;
  entity.currentTask.targetLongitude = entity.longitude + 1.0;
  entity.currentTask.targetAltitudeMeters = entity.altitude;
  entity.currentTask.targetSpeedKnots = entity.speedKnots;
  {
    auto lock = state.lock();
    state.entitiesMutable().push_back(entity);
  }

  QVector<application::KinematicsTelemetrySnapshot> received;
  auto& eventBus = application::EventBus::instance();
  const application::SubscriptionId subscriptionId =
      eventBus.subscribe<application::EventKinematicsTelemetryUpdated>(
          [&received](const application::EventKinematicsTelemetryUpdated& event) {
            if (event.snapshot.entityName == QStringLiteral("Viper 1")) {
              received.push_back(event.snapshot);
            }
          });

  state.advanceSimulation(0.05);
  state.advanceSimulation(0.05);
  ASSERT_EQ(received.size(), 1);
  const double firstPublishedRollDegrees = received.front().rollDegrees;

  state.advanceSimulation(0.05);
  state.advanceSimulation(0.05);
  eventBus.unsubscribe<application::EventKinematicsTelemetryUpdated>(
      subscriptionId);

  ASSERT_EQ(received.size(), 2);
  EXPECT_GT(firstPublishedRollDegrees, 0.0);
  EXPECT_GT(received.back().rollDegrees, firstPublishedRollDegrees);
  EXPECT_GT(received.back().headingDegrees, received.front().headingDegrees);
  EXPECT_EQ(received.back().dynamicsModel, QStringLiteral("kinematic"));
}
