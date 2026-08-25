#include <gtest/gtest.h>

#include "infrastructure/Ros2TelemetryPublisher.h"

#include <QCoreApplication>

#include <memory>
#include <utility>

namespace {

void ensureQtApplication() {
  if (QCoreApplication::instance()) {
    return;
  }
  static int argc = 1;
  static char applicationName[] = "qttest-ros2-telemetry-test";
  static char* argv[] = {applicationName, nullptr};
  static std::unique_ptr<QCoreApplication> application =
      std::make_unique<QCoreApplication>(argc, argv);
}

application::KinematicsTelemetrySnapshot makeSnapshot(
    double simulationTimeSeconds = 12.5) {
  application::KinematicsTelemetrySnapshot snapshot;
  snapshot.entityId = QStringLiteral("Entity 42/Alpha");
  snapshot.entityName = QStringLiteral("Viper, \"One\"");
  snapshot.domain = QStringLiteral("Air");
  snapshot.category = QStringLiteral("Fighter");
  snapshot.forceIdentifier = 1;
  snapshot.simulationTimeSeconds = simulationTimeSeconds;
  snapshot.deltaTimeSeconds = 1.0 / 60.0;
  snapshot.latitudeDegrees = 40.4168;
  snapshot.longitudeDegrees = -3.7038;
  snapshot.altitudeMeters = 3200.0;
  snapshot.headingDegrees = 350.0;
  snapshot.pitchDegrees = 4.0;
  snapshot.rollDegrees = -12.0;
  snapshot.speedKnots = 310.0;
  snapshot.verticalSpeedMetersPerSecond = 15.0;
  snapshot.targetHeadingDegrees = 10.0;
  snapshot.targetAltitudeMeters = 5000.0;
  snapshot.targetSpeedKnots = 400.0;
  snapshot.headingErrorDegrees = 20.0;
  snapshot.altitudeErrorMeters = 1800.0;
  snapshot.speedErrorKnots = 90.0;
  snapshot.maxHeadingRateDegreesPerSecond = 6.0;
  snapshot.maxAccelerationKnotsPerSecond = 18.0;
  snapshot.maxClimbRateMetersPerSecond = 35.0;
  snapshot.maxSpeedKnots = 900.0;
  snapshot.maxAltitudeMeters = 16000.0;
  snapshot.dynamicsModel = QStringLiteral("jsbsim");
  snapshot.dynamicsStepDurationMilliseconds = 0.15;
  snapshot.taskEnabled = true;
  snapshot.systems.fuelRemainingKilograms = 2150.0;
  snapshot.systems.fuelPercent = 71.6;
  application::EngineTelemetry engine;
  engine.engineId = QStringLiteral("engine_1");
  engine.n1Percent = 82.0;
  engine.available = true;
  snapshot.systems.engines.push_back(engine);
  return snapshot;
}

struct FakeBackendState {
  bool started = false;
  bool startSucceeds = true;
  int startCount = 0;
  QString topicPrefix;
  QVector<infrastructure::Ros2KinematicsTelemetryMessage> messages;
};

class FakeRos2Backend final : public infrastructure::IRos2TelemetryBackend {
public:
  explicit FakeRos2Backend(std::shared_ptr<FakeBackendState> state)
      : _state(std::move(state)) {}

  bool available() const override { return true; }
  QString availabilityMessage() const override {
    return QStringLiteral("available");
  }
  bool start(
      const infrastructure::Ros2TelemetryBackendConfiguration& configuration,
      QString* errorMessage) override {
    ++_state->startCount;
    _state->started = _state->startSucceeds;
    _state->topicPrefix = configuration.topicPrefix;
    if (!_state->startSucceeds && errorMessage) {
      *errorMessage = QStringLiteral("test startup failure");
    }
    return _state->startSucceeds;
  }
  void stop() override { _state->started = false; }
  bool enqueue(
      const infrastructure::Ros2KinematicsTelemetryMessage& message) override {
    if (!_state->started) {
      return false;
    }
    _state->messages.push_back(message);
    return true;
  }
  std::uint64_t droppedMessageCount() const override { return 0; }

private:
  std::shared_ptr<FakeBackendState> _state;
};

} // namespace

TEST(Ros2TelemetryContract, MapsSnapshotAndBuildsStableTopic) {
  const infrastructure::Ros2KinematicsTelemetryMessage message =
      infrastructure::makeRos2TelemetryMessage(makeSnapshot());

  EXPECT_EQ(message.schemaVersion, QStringLiteral("qttest.kinematics.v1"));
  EXPECT_EQ(message.entityId, QStringLiteral("Entity 42/Alpha"));
  EXPECT_DOUBLE_EQ(message.altitudeMeters, 3200.0);
  EXPECT_DOUBLE_EQ(message.rollDegrees, -12.0);
  ASSERT_EQ(message.engines.size(), 1);
  EXPECT_DOUBLE_EQ(message.engines.front().n1Percent, 82.0);
  EXPECT_EQ(
      infrastructure::ros2KinematicsTopic(
          QStringLiteral("qttest/entities/"),
          message.entityId),
      QStringLiteral("/qttest/entities/entity_42_alpha/kinematics"));
}

TEST(Ros2TelemetryPublisher, FiltersRateAndPublishesThroughBackend) {
  ensureQtApplication();
  auto state = std::make_shared<FakeBackendState>();
  infrastructure::Ros2TelemetryPublisher publisher(
      std::make_unique<FakeRos2Backend>(state));
  infrastructure::Ros2TelemetryConfiguration configuration;
  configuration.ros2Enabled = true;
  configuration.topicPrefix = QStringLiteral("/exercise/entities");
  configuration.entityReference = QStringLiteral("Entity 42/Alpha");
  configuration.frequencyHertz = 10.0;
  publisher.applyConfiguration(configuration);

  publisher.submitSnapshot(makeSnapshot(1.0));
  publisher.submitSnapshot(makeSnapshot(1.01));
  application::KinematicsTelemetrySnapshot other = makeSnapshot(1.2);
  other.entityId = QStringLiteral("other");
  publisher.submitSnapshot(other);
  publisher.submitSnapshot(makeSnapshot(1.2));

  EXPECT_EQ(publisher.publicationState(), QStringLiteral("Publishing"));
  EXPECT_EQ(state->topicPrefix, QStringLiteral("/exercise/entities"));
  ASSERT_EQ(state->messages.size(), 2);
  EXPECT_DOUBLE_EQ(state->messages.back().simulationTimeSeconds, 1.2);
  EXPECT_EQ(publisher.publishedMessageCount(), 2u);
  EXPECT_EQ(publisher.droppedMessageCount(), 0u);
}

TEST(Ros2TelemetryPublisher, CsvEscapesIdentityAndContainsSignals) {
  const QByteArray header = infrastructure::ros2TelemetryCsvHeader();
  const QByteArray row = infrastructure::serializeRos2TelemetryCsvRow(
      makeSnapshot());

  EXPECT_TRUE(header.startsWith("simulation_time_s,delta_time_s,entity_id"));
  EXPECT_TRUE(header.contains("heading_error_deg"));
  EXPECT_TRUE(row.contains("\"Viper, \"\"One\"\"\""));
  EXPECT_TRUE(row.contains(",350,"));
}

TEST(Ros2TelemetryPublisher, RetriesSameConfigurationAfterStartupFailure) {
  ensureQtApplication();
  auto state = std::make_shared<FakeBackendState>();
  state->startSucceeds = false;
  infrastructure::Ros2TelemetryPublisher publisher(
      std::make_unique<FakeRos2Backend>(state));
  infrastructure::Ros2TelemetryConfiguration configuration;
  configuration.ros2Enabled = true;

  publisher.applyConfiguration(configuration);
  EXPECT_TRUE(publisher.publicationState().startsWith(
      QStringLiteral("Failed:")));
  EXPECT_EQ(state->startCount, 1);

  state->startSucceeds = true;
  publisher.applyConfiguration(configuration);

  EXPECT_EQ(publisher.publicationState(), QStringLiteral("Publishing"));
  EXPECT_EQ(state->startCount, 2);
  EXPECT_TRUE(state->started);
}

TEST(Ros2TelemetryPublisher, BuildWithoutRos2ReportsUnavailableCleanly) {
  ensureQtApplication();
  infrastructure::Ros2TelemetryPublisher publisher;
  infrastructure::Ros2TelemetryConfiguration configuration;
  configuration.ros2Enabled = true;
  publisher.applyConfiguration(configuration);

  EXPECT_FALSE(publisher.ros2Available());
  EXPECT_EQ(publisher.publicationState(), QStringLiteral("Unavailable"));
  publisher.submitSnapshot(makeSnapshot());
  EXPECT_EQ(publisher.publishedMessageCount(), 0u);
  EXPECT_EQ(publisher.droppedMessageCount(), 1u);
}
