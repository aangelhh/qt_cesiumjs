#pragma once

#include "infrastructure/interoperability/hla/HlaRuntime.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace tactical::hla {

struct RprSensorState {
  std::string entityStableId;
  std::string sensorId;
  bool enabled = true;
  bool emitting = true;
  double azimuthCenterDegrees = 0.0;
  double azimuthWidthDegrees = 360.0;
  double elevationCenterDegrees = 0.0;
  double elevationWidthDegrees = 90.0;
  double frequencyHertz = 0.0;
  double bandwidthHertz = 0.0;
  double peakPowerWatts = 0.0;
  bool hasTracks = false;
};

class HlaSensorPublisher {
public:
  explicit HlaSensorPublisher(HlaRuntime& runtime);

  Result synchronize(const std::vector<RprSensorState>& sensors);
  Result removeAll();
  std::size_t registeredSensorCount() const;

private:
  struct RegisteredSensor {
    ObjectInstanceId systemInstanceId = 0;
    ObjectInstanceId beamInstanceId = 0;
    std::uint16_t entityNumber = 0;
  };

  Result ensurePublished();
  std::vector<NamedValue> encodeSystem(
      const RprSensorState& sensor,
      const RegisteredSensor& registered) const;
  std::vector<NamedValue> encodeBeam(
      const RprSensorState& sensor,
      const RegisteredSensor& registered) const;

  HlaRuntime& _runtime;
  bool _published = false;
  std::uint16_t _nextEntityNumber = 1;
  std::unordered_map<std::string, RegisteredSensor> _registeredSensors;
};

} // namespace tactical::hla
