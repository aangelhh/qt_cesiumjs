#pragma once

#include "infrastructure/Ros2TelemetryContract.h"

#include <QString>

#include <cstdint>
#include <memory>

namespace infrastructure {

struct Ros2TelemetryBackendConfiguration {
  QString topicPrefix = QStringLiteral("/qttest/entities");
  std::size_t queueCapacity = 256;
};

class IRos2TelemetryBackend {
public:
  virtual ~IRos2TelemetryBackend() = default;

  virtual bool available() const = 0;
  virtual QString availabilityMessage() const = 0;
  virtual bool start(
      const Ros2TelemetryBackendConfiguration& configuration,
      QString* errorMessage) = 0;
  virtual void stop() = 0;
  virtual bool enqueue(const Ros2KinematicsTelemetryMessage& message) = 0;
  virtual std::uint64_t droppedMessageCount() const = 0;
};

std::unique_ptr<IRos2TelemetryBackend> createRos2TelemetryBackend();

} // namespace infrastructure
