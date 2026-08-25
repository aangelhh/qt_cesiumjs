#pragma once

#include "application/EventBus.h"
#include "application/KinematicsTelemetry.h"
#include "infrastructure/Ros2TelemetryBackend.h"

#include <QFile>
#include <QHash>
#include <QObject>
#include <QString>

#include <cstdint>
#include <memory>

namespace infrastructure {

struct Ros2TelemetryConfiguration {
  bool ros2Enabled = false;
  QString topicPrefix = QStringLiteral("/qttest/entities");
  QString entityReference;
  double frequencyHertz = 10.0;
  bool csvEnabled = false;
  QString csvPath;
};

QByteArray ros2TelemetryCsvHeader();
QByteArray serializeRos2TelemetryCsvRow(
    const application::KinematicsTelemetrySnapshot& snapshot);

class Ros2TelemetryPublisher final : public QObject {
  Q_OBJECT

public:
  explicit Ros2TelemetryPublisher(QObject* parent = nullptr);
  Ros2TelemetryPublisher(
      std::unique_ptr<IRos2TelemetryBackend> backend,
      QObject* parent = nullptr);
  ~Ros2TelemetryPublisher() override;

  Ros2TelemetryConfiguration configuration() const;
  void applyConfiguration(const Ros2TelemetryConfiguration& configuration);
  bool ros2Available() const;
  QString availabilityMessage() const;
  QString publicationState() const;
  std::uint64_t publishedMessageCount() const;
  std::uint64_t droppedMessageCount() const;

  // Public for deterministic tests and alternative telemetry sources.
  void submitSnapshot(
      const application::KinematicsTelemetrySnapshot& snapshot);

signals:
  void publicationStateChanged(const QString& state);

private:
  void setPublicationState(const QString& state);
  void openCsv();
  void closeCsv();
  bool acceptsSnapshot(
      const application::KinematicsTelemetrySnapshot& snapshot);

  Ros2TelemetryConfiguration _configuration;
  std::unique_ptr<IRos2TelemetryBackend> _backend;
  QFile _csvFile;
  QHash<QString, double> _lastPublicationTimeByEntity;
  application::SubscriptionId _subscriptionId = 0;
  QString _publicationState = QStringLiteral("Disabled");
  std::uint64_t _publishedMessageCount = 0;
  std::uint64_t _droppedMessageCount = 0;
};

} // namespace infrastructure
