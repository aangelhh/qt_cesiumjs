#pragma once

#include "infrastructure/interoperability/hla/IHlaBackend.h"

#include <memory>
#include <string>

namespace tactical::hla {

enum class BackendAdapter {
  Mock,
  SharedLibrary
};

struct BackendConfiguration {
  BackendAdapter adapter = BackendAdapter::Mock;
  std::string expectedBackendId;
  std::string libraryPath;
};

struct BackendCreationResult {
  std::unique_ptr<IHlaBackend> backend;
  std::string error;

  explicit operator bool() const { return backend != nullptr; }
};

class HlaBackendFactory {
public:
  static BackendCreationResult create(const BackendConfiguration& configuration);
};

} // namespace tactical::hla
