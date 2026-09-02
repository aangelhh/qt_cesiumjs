#pragma once

#include "infrastructure/interoperability/hla/HlaRuntime.h"

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace tactical::hla {

struct RprWeaponFireState {
  std::string stableId;
  std::string munitionType;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double speedMetersPerSecond = 0.0;
};

class HlaWarfarePublisher {
public:
  explicit HlaWarfarePublisher(HlaRuntime& runtime);

  Result synchronize(const std::vector<RprWeaponFireState>& activeMunitions);
  std::size_t sentWeaponFireCount() const;

private:
  Result ensurePublished();
  std::vector<NamedValue> encodeWeaponFire(
      const RprWeaponFireState& munition,
      std::uint16_t eventNumber) const;

  HlaRuntime& _runtime;
  bool _published = false;
  std::uint16_t _nextEventNumber = 1;
  std::unordered_set<std::string> _sentMunitionIds;
};

} // namespace tactical::hla
