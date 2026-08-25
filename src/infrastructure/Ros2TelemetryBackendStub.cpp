#include "infrastructure/Ros2TelemetryBackend.h"

namespace infrastructure {
namespace {

class UnavailableRos2TelemetryBackend final : public IRos2TelemetryBackend {
public:
  bool available() const override { return false; }

  QString availabilityMessage() const override {
    return QStringLiteral(
        "ROS 2 support is not included in this build. Reconfigure with "
        "QTTEST_ENABLE_ROS2_TELEMETRY=ON after sourcing ROS 2.");
  }

  bool start(
      const Ros2TelemetryBackendConfiguration&,
      QString* errorMessage) override {
    if (errorMessage) {
      *errorMessage = this->availabilityMessage();
    }
    return false;
  }

  void stop() override {}

  bool enqueue(const Ros2KinematicsTelemetryMessage&) override {
    return false;
  }

  std::uint64_t droppedMessageCount() const override { return 0; }
};

} // namespace

std::unique_ptr<IRos2TelemetryBackend> createRos2TelemetryBackend() {
  return std::make_unique<UnavailableRos2TelemetryBackend>();
}

} // namespace infrastructure
