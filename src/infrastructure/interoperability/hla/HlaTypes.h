#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace tactical::hla {

enum class BackendState {
  Unavailable,
  Disconnected,
  Connected,
  Joined,
  Error
};

struct Result {
  bool success = false;
  std::string message;

  static Result ok() { return {true, {}}; }
  static Result failure(std::string message) {
    return {false, std::move(message)};
  }
};

struct SessionConfiguration {
  std::string localSettingsDesignator;
  std::string federationName;
  std::string federateName;
  std::string federateType;
  std::vector<std::string> fomModules;
  bool createFederationIfMissing = true;
};

using ObjectInstanceId = std::uint64_t;
using ByteBuffer = std::vector<std::uint8_t>;

struct NamedValue {
  std::string name;
  ByteBuffer value;
};

} // namespace tactical::hla
