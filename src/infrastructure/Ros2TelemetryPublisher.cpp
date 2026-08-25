#include "infrastructure/Ros2TelemetryPublisher.h"

#include "application/Event.h"

#include <QMetaObject>

#include <algorithm>
#include <cmath>

namespace infrastructure {
namespace {

constexpr double kDefaultFrequencyHertz = 10.0;
constexpr double kMinimumFrequencyHertz = 0.1;
constexpr double kMaximumFrequencyHertz = 60.0;

QByteArray csvText(const QString& text) {
  QString escaped = text;
  escaped.replace(QLatin1Char('"'), QStringLiteral("\"\""));
  return QByteArrayLiteral("\"") + escaped.toUtf8() + QByteArrayLiteral("\"");
}

QByteArray csvNumber(double value) {
  return QByteArray::number(value, 'g', 17);
}

Ros2TelemetryConfiguration normalizedConfiguration(
    Ros2TelemetryConfiguration configuration) {
  configuration.topicPrefix = configuration.topicPrefix.trimmed();
  if (configuration.topicPrefix.isEmpty()) {
    configuration.topicPrefix = QStringLiteral("/qttest/entities");
  }
  if (!configuration.topicPrefix.startsWith(QLatin1Char('/'))) {
    configuration.topicPrefix.prepend(QLatin1Char('/'));
  }
  while (configuration.topicPrefix.endsWith(QLatin1Char('/'))) {
    configuration.topicPrefix.chop(1);
  }
  if (!std::isfinite(configuration.frequencyHertz)) {
    configuration.frequencyHertz = kDefaultFrequencyHertz;
  }
  configuration.frequencyHertz = std::clamp(
      configuration.frequencyHertz,
      kMinimumFrequencyHertz,
      kMaximumFrequencyHertz);
  configuration.entityReference = configuration.entityReference.trimmed();
  configuration.csvPath = configuration.csvPath.trimmed();
  return configuration;
}

} // namespace

QByteArray ros2TelemetryCsvHeader() {
  return QByteArrayLiteral(
      "simulation_time_s,delta_time_s,entity_id,entity_name,latitude_deg,"
      "longitude_deg,altitude_m,heading_deg,pitch_deg,roll_deg,speed_kt,"
      "vertical_speed_mps,target_heading_deg,target_altitude_m,"
      "target_speed_kt,heading_error_deg,altitude_error_m,speed_error_kt,"
      "max_heading_rate_dps,max_acceleration_ktps,max_climb_rate_mps,"
      "max_speed_kt,max_altitude_m,dynamics_step_ms,fuel_remaining_kg,"
      "fuel_percent\n");
}

QByteArray serializeRos2TelemetryCsvRow(
    const application::KinematicsTelemetrySnapshot& snapshot) {
  const QList<QByteArray> values{
      csvNumber(snapshot.simulationTimeSeconds),
      csvNumber(snapshot.deltaTimeSeconds),
      csvText(snapshot.entityId),
      csvText(snapshot.entityName),
      csvNumber(snapshot.latitudeDegrees),
      csvNumber(snapshot.longitudeDegrees),
      csvNumber(snapshot.altitudeMeters),
      csvNumber(snapshot.headingDegrees),
      csvNumber(snapshot.pitchDegrees),
      csvNumber(snapshot.rollDegrees),
      csvNumber(snapshot.speedKnots),
      csvNumber(snapshot.verticalSpeedMetersPerSecond),
      csvNumber(snapshot.targetHeadingDegrees),
      csvNumber(snapshot.targetAltitudeMeters),
      csvNumber(snapshot.targetSpeedKnots),
      csvNumber(snapshot.headingErrorDegrees),
      csvNumber(snapshot.altitudeErrorMeters),
      csvNumber(snapshot.speedErrorKnots),
      csvNumber(snapshot.maxHeadingRateDegreesPerSecond),
      csvNumber(snapshot.maxAccelerationKnotsPerSecond),
      csvNumber(snapshot.maxClimbRateMetersPerSecond),
      csvNumber(snapshot.maxSpeedKnots),
      csvNumber(snapshot.maxAltitudeMeters),
      csvNumber(snapshot.dynamicsStepDurationMilliseconds),
      csvNumber(snapshot.systems.fuelRemainingKilograms),
      csvNumber(snapshot.systems.fuelPercent),
  };
  return values.join(',') + '\n';
}

Ros2TelemetryPublisher::Ros2TelemetryPublisher(QObject* parent)
    : Ros2TelemetryPublisher(createRos2TelemetryBackend(), parent) {}

Ros2TelemetryPublisher::Ros2TelemetryPublisher(
    std::unique_ptr<IRos2TelemetryBackend> backend,
    QObject* parent)
    : QObject(parent),
      _backend(std::move(backend)) {
  _subscriptionId = application::EventBus::instance()
      .subscribe<application::EventKinematicsTelemetryUpdated>(
          [this](const application::EventKinematicsTelemetryUpdated& event) {
            const application::KinematicsTelemetrySnapshot snapshot =
                event.snapshot;
            QMetaObject::invokeMethod(
                this,
                [this, snapshot]() { this->submitSnapshot(snapshot); },
                Qt::QueuedConnection);
          });
}

Ros2TelemetryPublisher::~Ros2TelemetryPublisher() {
  this->blockSignals(true);
  if (_subscriptionId != 0) {
    application::EventBus::instance()
        .unsubscribe<application::EventKinematicsTelemetryUpdated>(
            _subscriptionId);
  }
  _backend->stop();
  this->closeCsv();
}

Ros2TelemetryConfiguration Ros2TelemetryPublisher::configuration() const {
  return _configuration;
}

void Ros2TelemetryPublisher::applyConfiguration(
    const Ros2TelemetryConfiguration& configuration) {
  const Ros2TelemetryConfiguration normalized =
      normalizedConfiguration(configuration);
  const bool backendConfigurationChanged =
      normalized.ros2Enabled != _configuration.ros2Enabled ||
      normalized.topicPrefix != _configuration.topicPrefix;
  const bool backendNeedsStart =
      backendConfigurationChanged ||
      _publicationState != QStringLiteral("Publishing");
  const bool csvChanged =
      normalized.csvEnabled != _configuration.csvEnabled ||
      normalized.csvPath != _configuration.csvPath;
  _configuration = normalized;
  _lastPublicationTimeByEntity.clear();

  if (csvChanged) {
    this->closeCsv();
  }
  if (_configuration.csvEnabled &&
      !_csvFile.isOpen()) {
    this->openCsv();
  }

  if (!_configuration.ros2Enabled) {
    _backend->stop();
    this->setPublicationState(QStringLiteral("Disabled"));
    return;
  }
  if (!_backend->available()) {
    this->setPublicationState(QStringLiteral("Unavailable"));
    return;
  }
  if (backendNeedsStart) {
    _backend->stop();
    Ros2TelemetryBackendConfiguration backendConfiguration;
    backendConfiguration.topicPrefix = _configuration.topicPrefix;
    QString errorMessage;
    if (!_backend->start(backendConfiguration, &errorMessage)) {
      this->setPublicationState(
          errorMessage.isEmpty()
          ? QStringLiteral("Failed")
          : QStringLiteral("Failed: %1").arg(errorMessage));
      return;
    }
  }
  this->setPublicationState(QStringLiteral("Publishing"));
}

bool Ros2TelemetryPublisher::ros2Available() const {
  return _backend->available();
}

QString Ros2TelemetryPublisher::availabilityMessage() const {
  return _backend->availabilityMessage();
}

QString Ros2TelemetryPublisher::publicationState() const {
  return _publicationState;
}

std::uint64_t Ros2TelemetryPublisher::publishedMessageCount() const {
  return _publishedMessageCount;
}

std::uint64_t Ros2TelemetryPublisher::droppedMessageCount() const {
  return _droppedMessageCount + _backend->droppedMessageCount();
}

void Ros2TelemetryPublisher::submitSnapshot(
    const application::KinematicsTelemetrySnapshot& snapshot) {
  if (!this->acceptsSnapshot(snapshot)) {
    return;
  }

  if (_configuration.csvEnabled) {
    if (!_csvFile.isOpen()) {
      this->openCsv();
    }
    if (_csvFile.isOpen()) {
      _csvFile.write(serializeRos2TelemetryCsvRow(snapshot));
    }
  }

  if (_configuration.ros2Enabled) {
    if (!_backend->available() ||
        !_backend->enqueue(makeRos2TelemetryMessage(snapshot))) {
      ++_droppedMessageCount;
      return;
    }
    ++_publishedMessageCount;
  }
}

void Ros2TelemetryPublisher::setPublicationState(const QString& state) {
  if (_publicationState == state) {
    return;
  }
  _publicationState = state;
  emit publicationStateChanged(state);
}

void Ros2TelemetryPublisher::openCsv() {
  if (!_configuration.csvEnabled || _configuration.csvPath.isEmpty()) {
    return;
  }
  _csvFile.setFileName(_configuration.csvPath);
  if (_csvFile.open(
          QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    _csvFile.write(ros2TelemetryCsvHeader());
  }
}

void Ros2TelemetryPublisher::closeCsv() {
  if (_csvFile.isOpen()) {
    _csvFile.flush();
    _csvFile.close();
  }
}

bool Ros2TelemetryPublisher::acceptsSnapshot(
    const application::KinematicsTelemetrySnapshot& snapshot) {
  if (!_configuration.ros2Enabled && !_configuration.csvEnabled) {
    return false;
  }
  const QString filter = _configuration.entityReference;
  if (!filter.isEmpty() &&
      filter.compare(snapshot.entityId, Qt::CaseInsensitive) != 0 &&
      filter.compare(snapshot.entityName, Qt::CaseInsensitive) != 0) {
    return false;
  }

  const QString entityKey = snapshot.entityId.trimmed().isEmpty()
      ? snapshot.entityName
      : snapshot.entityId;
  const double periodSeconds = 1.0 / _configuration.frequencyHertz;
  const auto previous = _lastPublicationTimeByEntity.constFind(entityKey);
  if (previous != _lastPublicationTimeByEntity.cend() &&
      snapshot.simulationTimeSeconds >= previous.value() &&
      snapshot.simulationTimeSeconds - previous.value() + 1.0e-12 <
          periodSeconds) {
    return false;
  }
  _lastPublicationTimeByEntity.insert(
      entityKey,
      snapshot.simulationTimeSeconds);
  return true;
}

} // namespace infrastructure
