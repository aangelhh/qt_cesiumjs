#include "infrastructure/interoperability/hla/HlaBackendFactory.h"

#include "infrastructure/interoperability/hla/MockHlaBackend.h"
#include "infrastructure/interoperability/hla/OpenRtiCompatibilityBackend.h"
#include "infrastructure/interoperability/hla/SharedLibraryHlaBackend.h"

#include <memory>
#include <utility>

namespace tactical::hla {

BackendCreationResult HlaBackendFactory::create(
    const BackendConfiguration& configuration) {
  std::unique_ptr<IHlaBackend> backend;

  switch (configuration.adapter) {
  case BackendAdapter::Mock:
    backend = std::make_unique<MockHlaBackend>();
    break;
  case BackendAdapter::SharedLibrary: {
    if (configuration.libraryPath.empty()) {
      return {nullptr, "HLA backend library path is empty"};
    }
    auto sharedBackend = std::make_unique<SharedLibraryHlaBackend>(
        configuration.libraryPath);
    if (!sharedBackend->isAvailable()) {
      return {nullptr, sharedBackend->loadError()};
    }
    backend = std::move(sharedBackend);
    break;
  }
  }

  if (!configuration.expectedBackendId.empty() &&
      backend->id() != configuration.expectedBackendId) {
    return {nullptr,
            "HLA backend id mismatch: expected " +
                configuration.expectedBackendId + ", loaded " + backend->id()};
  }

  if (backend->id() == "openrti1516e") {
    backend = std::make_unique<OpenRtiCompatibilityBackend>(std::move(backend));
  }

  return {std::move(backend), {}};
}

} // namespace tactical::hla
