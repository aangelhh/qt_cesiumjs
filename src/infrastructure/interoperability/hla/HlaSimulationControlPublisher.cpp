#include "infrastructure/interoperability/hla/HlaSimulationControlPublisher.h"

#include <chrono>
#include <cmath>

namespace tactical::hla {
namespace {

void appendUnsigned16(ByteBuffer& output, std::uint16_t value) {
  output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void appendUnsigned32(ByteBuffer& output, std::uint32_t value) {
  output.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
  output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

ByteBuffer entityIdentifier(
    std::uint16_t site = 1,
    std::uint16_t application = 1,
    std::uint16_t entity = 0) {
  ByteBuffer result;
  appendUnsigned16(result, site);
  appendUnsigned16(result, application);
  appendUnsigned16(result, entity);
  return result;
}

ByteBuffer unsigned32(std::uint32_t value) {
  ByteBuffer result;
  appendUnsigned32(result, value);
  return result;
}

ByteBuffer clockTime(double seconds) {
  const double positiveSeconds = std::max(0.0, seconds);
  const auto hours = static_cast<std::uint32_t>(positiveSeconds / 3600.0);
  const double withinHour = positiveSeconds - static_cast<double>(hours) * 3600.0;
  const auto fraction = static_cast<std::uint32_t>(
      std::llround((withinHour / 3600.0) * 4294967295.0));
  ByteBuffer result;
  appendUnsigned32(result, hours);
  appendUnsigned32(result, fraction);
  return result;
}

double utcEpochSeconds() {
  return std::chrono::duration<double>(
      std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace

HlaSimulationControlPublisher::HlaSimulationControlPublisher(HlaRuntime& runtime)
    : _runtime(runtime) {}

Result HlaSimulationControlPublisher::publish(
    RemoteSimulationControl control,
    double simulationTimeSeconds) {
  const std::string className = control == RemoteSimulationControl::StartResume
      ? "HLAinteractionRoot.StartResume"
      : "HLAinteractionRoot.StopFreeze";
  Result result = this->ensurePublished(className);
  if (!result.success) return result;
  result = _runtime.sendInteraction(
      className,
      control == RemoteSimulationControl::StartResume
          ? this->encodeStartResume(simulationTimeSeconds)
          : this->encodeStopFreeze(control, simulationTimeSeconds));
  if (++_nextRequestId == 0) _nextRequestId = 1;
  return result;
}

Result HlaSimulationControlPublisher::ensurePublished(
    const std::string& interactionClassName) {
  if (_publishedClasses.count(interactionClassName) != 0) return Result::ok();
  const Result result = _runtime.publishInteractionClass(interactionClassName);
  if (result.success) _publishedClasses.insert(interactionClassName);
  return result;
}

std::vector<NamedValue> HlaSimulationControlPublisher::encodeStartResume(
    double simulationTimeSeconds) {
  return {
      {"OriginatingEntity", entityIdentifier()},
      {"ReceivingEntity", entityIdentifier(0, 0, 0)},
      {"RealWorldTime", clockTime(utcEpochSeconds())},
      {"RequestIdentifier", unsigned32(_nextRequestId)},
      {"SimulationTime", clockTime(simulationTimeSeconds)}};
}

std::vector<NamedValue> HlaSimulationControlPublisher::encodeStopFreeze(
    RemoteSimulationControl control,
    double simulationTimeSeconds) {
  static_cast<void>(simulationTimeSeconds);
  const bool stop = control == RemoteSimulationControl::Stop;
  return {
      {"OriginatingEntity", entityIdentifier()},
      {"ReceivingEntity", entityIdentifier(0, 0, 0)},
      {"RequestIdentifier", unsigned32(_nextRequestId)},
      {"RealWorldTime", clockTime(utcEpochSeconds())},
      {"Reason", {static_cast<std::uint8_t>(stop ? 2 : 0)}},
      {"ReflectValues", {static_cast<std::uint8_t>(stop ? 0 : 1)}},
      {"RunInternalSimulationClock", {0}},
      {"UpdateAttributes", {static_cast<std::uint8_t>(stop ? 0 : 1)}}};
}

} // namespace tactical::hla
