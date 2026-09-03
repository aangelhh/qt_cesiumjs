#pragma once

#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"
#include "infrastructure/interoperability/hla/HlaRuntime.h"

#include <cstdint>
#include <set>
#include <string>

namespace tactical::hla {

class HlaSimulationControlPublisher {
public:
  explicit HlaSimulationControlPublisher(HlaRuntime& runtime);

  Result publish(RemoteSimulationControl control, double simulationTimeSeconds);

private:
  Result ensurePublished(const std::string& interactionClassName);
  std::vector<NamedValue> encodeStartResume(double simulationTimeSeconds);
  std::vector<NamedValue> encodeStopFreeze(
      RemoteSimulationControl control, double simulationTimeSeconds);

  HlaRuntime& _runtime;
  std::set<std::string> _publishedClasses;
  std::uint32_t _nextRequestId = 1;
};

} // namespace tactical::hla
