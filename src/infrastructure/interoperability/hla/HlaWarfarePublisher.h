#pragma once

#include "infrastructure/interoperability/hla/HlaRuntime.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tactical::hla {

struct RprWeaponFireState {
  std::string stableId;
  std::string munitionType;
  std::string firingObjectInstanceName;
  std::string targetObjectInstanceName;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double speedMetersPerSecond = 0.0;
};

struct RprMunitionDetonationState {
  std::string effectId;
  std::string munitionStableId;
  std::string munitionType;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
};

class HlaWarfarePublisher {
public:
  explicit HlaWarfarePublisher(HlaRuntime& runtime);

  Result synchronize(const std::vector<RprWeaponFireState>& activeMunitions);
  Result synchronizeDetonations(
      const std::vector<RprMunitionDetonationState>& detonations);
  Result removeAll();
  std::size_t sentWeaponFireCount() const;
  std::size_t sentDetonationCount() const;
  std::size_t registeredMunitionCount() const;

private:
  Result ensurePublished();
  std::vector<NamedValue> encodeWeaponFire(
      const RprWeaponFireState& munition,
      std::uint16_t eventNumber) const;
  std::vector<NamedValue> encodeDetonation(
      const RprMunitionDetonationState& detonation,
      std::uint16_t eventNumber) const;

  HlaRuntime& _runtime;
  bool _published = false;
  bool _detonationPublished = false;
  std::uint16_t _nextEventNumber = 1;
  std::unordered_set<std::string> _sentMunitionIds;
  std::unordered_set<std::string> _sentDetonationIds;
  struct FireCorrelation {
    std::uint16_t eventNumber = 0;
    std::string firingObjectInstanceName;
    std::string targetObjectInstanceName;
  };
  std::unordered_map<std::string, FireCorrelation> _fireCorrelations;
  struct RegisteredMunition {
    ObjectInstanceId instanceId = 0;
    std::uint16_t entityNumber = 0;
  };
  std::uint16_t _nextMunitionEntityNumber = 32768;
  std::unordered_map<std::string, RegisteredMunition> _registeredMunitions;
};

} // namespace tactical::hla
