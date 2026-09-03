#pragma once

#include "infrastructure/interoperability/hla/HlaTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace tactical::hla {

struct RprEntityState {
  std::string stableId;
  std::string name;
  std::string domain;
  std::uint8_t entityKind = 0;
  std::uint8_t entityDomain = 0;
  std::uint16_t countryCode = 0;
  std::uint8_t category = 0;
  std::uint8_t subcategory = 0;
  std::uint8_t specific = 0;
  std::uint8_t extra = 0;
  std::uint8_t forceIdentifier = 0;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;
  double speedKnots = 0.0;
  double damagePercent = 0.0;
  bool destroyed = false;
};

class RprFomEncoding {
public:
  static std::string objectClassName(const std::string& domain);
  static std::string domainFromObjectClassName(
      const std::string& objectClassName);
  static std::string objectInstanceName(const std::string& stableId);
  static std::vector<std::string> publishedAttributeNames();
  static std::vector<NamedValue> encodeAttributes(
      const RprEntityState& entity,
      std::uint16_t siteId,
      std::uint16_t applicationId,
      std::uint16_t entityNumber);
  static Result decodeAttributes(
      const std::vector<NamedValue>& attributes,
      RprEntityState& entity);
};

} // namespace tactical::hla
