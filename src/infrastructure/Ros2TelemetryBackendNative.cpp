#include "infrastructure/Ros2TelemetryBackend.h"

#include <qttest_interfaces/msg/engine_telemetry.hpp>
#include <qttest_interfaces/msg/kinematics_telemetry.hpp>
#include <rclcpp/rclcpp.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <exception>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace infrastructure {
namespace {

using TelemetryMessage = qttest_interfaces::msg::KinematicsTelemetry;

std::string utf8(const QString& value) {
  return value.toUtf8().toStdString();
}

qttest_interfaces::msg::EngineTelemetry toRosMessage(
    const Ros2EngineTelemetryMessage& source) {
  qttest_interfaces::msg::EngineTelemetry message;
  message.engine_id = utf8(source.engineId);
  message.state = utf8(source.state);
  message.n1_percent = source.n1Percent;
  message.n2_percent = source.n2Percent;
  message.exhaust_temperature_celsius = source.exhaustTemperatureCelsius;
  message.fuel_flow_kilograms_per_hour = source.fuelFlowKilogramsPerHour;
  message.thrust_kilonewtons = source.thrustKilonewtons;
  message.available = source.available;
  message.estimated = source.estimated;
  return message;
}

TelemetryMessage toRosMessage(const Ros2KinematicsTelemetryMessage& source) {
  TelemetryMessage message;
  message.schema_version = utf8(source.schemaVersion);
  message.entity_id = utf8(source.entityId);
  message.entity_name = utf8(source.entityName);
  message.domain = utf8(source.domain);
  message.category = utf8(source.category);
  message.force_identifier = source.forceIdentifier;
  message.task_type = utf8(source.taskType);
  message.task_status = utf8(source.taskStatus);
  message.dynamics_model = utf8(source.dynamicsModel);
  message.dynamics_fallback_reason = utf8(source.dynamicsFallbackReason);
  message.simulation_time_seconds = source.simulationTimeSeconds;
  message.delta_time_seconds = source.deltaTimeSeconds;
  message.latitude_degrees = source.latitudeDegrees;
  message.longitude_degrees = source.longitudeDegrees;
  message.altitude_meters = source.altitudeMeters;
  message.heading_degrees = source.headingDegrees;
  message.pitch_degrees = source.pitchDegrees;
  message.roll_degrees = source.rollDegrees;
  message.speed_knots = source.speedKnots;
  message.vertical_speed_meters_per_second =
      source.verticalSpeedMetersPerSecond;
  message.target_heading_degrees = source.targetHeadingDegrees;
  message.target_altitude_meters = source.targetAltitudeMeters;
  message.target_speed_knots = source.targetSpeedKnots;
  message.heading_error_degrees = source.headingErrorDegrees;
  message.altitude_error_meters = source.altitudeErrorMeters;
  message.speed_error_knots = source.speedErrorKnots;
  message.max_heading_rate_degrees_per_second =
      source.maxHeadingRateDegreesPerSecond;
  message.max_acceleration_knots_per_second =
      source.maxAccelerationKnotsPerSecond;
  message.max_climb_rate_meters_per_second =
      source.maxClimbRateMetersPerSecond;
  message.max_speed_knots = source.maxSpeedKnots;
  message.max_altitude_meters = source.maxAltitudeMeters;
  message.dynamics_step_duration_milliseconds =
      source.dynamicsStepDurationMilliseconds;
  message.task_enabled = source.taskEnabled;
  message.destroyed = source.destroyed;
  message.fuel_capacity_kilograms = source.fuelCapacityKilograms;
  message.fuel_remaining_kilograms = source.fuelRemainingKilograms;
  message.fuel_percent = source.fuelPercent;
  message.total_fuel_flow_kilograms_per_hour =
      source.totalFuelFlowKilogramsPerHour;
  message.estimated_endurance_seconds = source.estimatedEnduranceSeconds;
  message.engines.reserve(static_cast<std::size_t>(source.engines.size()));
  for (const Ros2EngineTelemetryMessage& engine : source.engines) {
    message.engines.push_back(toRosMessage(engine));
  }
  return message;
}

class NativeRos2TelemetryBackend final : public IRos2TelemetryBackend {
public:
  ~NativeRos2TelemetryBackend() override { this->stop(); }

  bool available() const override { return true; }

  QString availabilityMessage() const override {
    return QStringLiteral("ROS 2 publisher available");
  }

  bool start(
      const Ros2TelemetryBackendConfiguration& configuration,
      QString* errorMessage) override {
    this->stop();
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _configuration = configuration;
      _configuration.queueCapacity =
          std::max<std::size_t>(1, _configuration.queueCapacity);
      _running = true;
      _started = false;
      _startupError.clear();
      _droppedMessages = 0;
    }
    _worker = std::thread([this]() { this->run(); });

    std::unique_lock<std::mutex> lock(_mutex);
    _startedCondition.wait_for(
        lock,
        std::chrono::seconds(5),
        [this]() { return _started || !_startupError.empty(); });
    if (!_startupError.empty() || !_started) {
      const QString error = !_startupError.empty()
          ? QString::fromStdString(_startupError)
          : QStringLiteral("Timed out while starting ROS 2 publisher");
      lock.unlock();
      this->stop();
      if (errorMessage) {
        *errorMessage = error;
      }
      return false;
    }
    return true;
  }

  void stop() override {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _running = false;
      _queue.clear();
    }
    _queueCondition.notify_all();
    if (_worker.joinable()) {
      _worker.join();
    }
  }

  bool enqueue(const Ros2KinematicsTelemetryMessage& message) override {
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_running || !_started) {
      return false;
    }
    if (_queue.size() >= _configuration.queueCapacity) {
      _queue.pop_front();
      ++_droppedMessages;
    }
    _queue.push_back(message);
    _queueCondition.notify_one();
    return true;
  }

  std::uint64_t droppedMessageCount() const override {
    return _droppedMessages.load();
  }

private:
  void run() {
    try {
      auto context = std::make_shared<rclcpp::Context>();
      context->init(0, nullptr);
      rclcpp::NodeOptions nodeOptions;
      nodeOptions.context(context);
      auto node = std::make_shared<rclcpp::Node>(
          "qttest_kinematics_publisher",
          nodeOptions);
      rclcpp::ExecutorOptions executorOptions;
      executorOptions.context = context;
      rclcpp::executors::SingleThreadedExecutor executor(executorOptions);
      executor.add_node(node);
      const rclcpp::QoS qos = rclcpp::QoS(rclcpp::KeepLast(5))
          .best_effort()
          .durability_volatile();
      std::unordered_map<
          std::string,
          rclcpp::Publisher<TelemetryMessage>::SharedPtr> publishers;

      {
        std::lock_guard<std::mutex> lock(_mutex);
        _started = true;
      }
      _startedCondition.notify_all();

      while (true) {
        Ros2KinematicsTelemetryMessage source;
        bool hasMessage = false;
        {
          std::unique_lock<std::mutex> lock(_mutex);
          _queueCondition.wait_for(
              lock,
              std::chrono::milliseconds(5),
              [this]() { return !_running || !_queue.empty(); });
          if (!_running && _queue.empty()) {
            break;
          }
          if (!_queue.empty()) {
            source = std::move(_queue.front());
            _queue.pop_front();
            hasMessage = true;
          }
        }

        if (hasMessage) {
          const std::string topic = utf8(ros2KinematicsTopic(
              _configuration.topicPrefix,
              source.entityId));
          auto& publisher = publishers[topic];
          if (!publisher) {
            publisher = node->create_publisher<TelemetryMessage>(topic, qos);
          }
          publisher->publish(toRosMessage(source));
        }
        executor.spin_some();
      }

      executor.remove_node(node);
      context->shutdown("qttest telemetry publisher stopped");
    } catch (const std::exception& error) {
      {
        std::lock_guard<std::mutex> lock(_mutex);
        _startupError = error.what();
        _running = false;
      }
      _startedCondition.notify_all();
    }
  }

  Ros2TelemetryBackendConfiguration _configuration;
  mutable std::mutex _mutex;
  std::condition_variable _queueCondition;
  std::condition_variable _startedCondition;
  std::deque<Ros2KinematicsTelemetryMessage> _queue;
  std::thread _worker;
  bool _running = false;
  bool _started = false;
  std::string _startupError;
  std::atomic<std::uint64_t> _droppedMessages{0};
};

} // namespace

std::unique_ptr<IRos2TelemetryBackend> createRos2TelemetryBackend() {
  return std::make_unique<NativeRos2TelemetryBackend>();
}

} // namespace infrastructure
