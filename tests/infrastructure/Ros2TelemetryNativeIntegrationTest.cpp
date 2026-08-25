#include "infrastructure/Ros2TelemetryBackend.h"
#include "infrastructure/Ros2TelemetryContract.h"

#include <qttest_interfaces/msg/kinematics_telemetry.hpp>
#include <rclcpp/rclcpp.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace {

int fail(const std::string& message) {
  std::cerr << "ROS 2 native telemetry integration failed: " << message
            << '\n';
  return 1;
}

} // namespace

int main(int argc, char** argv) {
  auto context = std::make_shared<rclcpp::Context>();
  context->init(argc, argv);
  rclcpp::NodeOptions nodeOptions;
  nodeOptions.context(context);
  auto subscriberNode = std::make_shared<rclcpp::Node>(
      "qttest_kinematics_test_subscriber",
      nodeOptions);
  rclcpp::ExecutorOptions executorOptions;
  executorOptions.context = context;
  rclcpp::executors::SingleThreadedExecutor executor(executorOptions);
  executor.add_node(subscriberNode);

  const QString entityId = QStringLiteral("integration-entity-42");
  const QString topic = infrastructure::ros2KinematicsTopic(
      QStringLiteral("/qttest/test/entities"),
      entityId);
  qttest_interfaces::msg::KinematicsTelemetry::SharedPtr received;
  const rclcpp::QoS qos = rclcpp::QoS(rclcpp::KeepLast(5))
      .best_effort()
      .durability_volatile();
  auto subscription = subscriberNode->create_subscription<
      qttest_interfaces::msg::KinematicsTelemetry>(
      topic.toStdString(),
      qos,
      [&received](
          qttest_interfaces::msg::KinematicsTelemetry::SharedPtr message) {
        received = std::move(message);
      });

  std::unique_ptr<infrastructure::IRos2TelemetryBackend> backend =
      infrastructure::createRos2TelemetryBackend();
  if (!backend->available()) {
    context->shutdown("native telemetry backend unavailable");
    return fail(backend->availabilityMessage().toStdString());
  }
  infrastructure::Ros2TelemetryBackendConfiguration configuration;
  configuration.topicPrefix = QStringLiteral("/qttest/test/entities");
  QString errorMessage;
  if (!backend->start(configuration, &errorMessage)) {
    context->shutdown("native telemetry backend failed to start");
    return fail(errorMessage.toStdString());
  }

  infrastructure::Ros2KinematicsTelemetryMessage outgoing;
  outgoing.schemaVersion = QStringLiteral("qttest.kinematics.v1");
  outgoing.entityId = entityId;
  outgoing.entityName = QStringLiteral("Integration Fighter");
  outgoing.altitudeMeters = 4321.0;
  outgoing.headingDegrees = 123.0;

  const auto deadline = std::chrono::steady_clock::now() +
      std::chrono::seconds(8);
  while (!received && std::chrono::steady_clock::now() < deadline) {
    if (!backend->enqueue(outgoing)) {
      backend->stop();
      context->shutdown("native telemetry enqueue failed");
      return fail("backend rejected an outgoing message");
    }
    executor.spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  backend->stop();
  executor.remove_node(subscriberNode);
  context->shutdown("native telemetry integration test finished");
  if (!received) {
    return fail("no typed DDS message was received before timeout");
  }
  if (received->entity_id != "integration-entity-42" ||
      received->entity_name != "Integration Fighter" ||
      received->altitude_meters != 4321.0 ||
      received->heading_degrees != 123.0) {
    return fail("received message fields do not match the published values");
  }

  std::cout << "Received typed telemetry on " << topic.toStdString()
            << " through ROS 2/DDS\n";
  return 0;
}
