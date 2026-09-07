#pragma once

#include "infrastructure/interoperability/hla/HlaRuntime.h"
#include "infrastructure/interoperability/hla/RprFomEncoding.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tactical::hla {

class HlaEntityPublisher {
public:
  explicit HlaEntityPublisher(HlaRuntime& runtime);

  Result synchronize(
      const std::vector<RprEntityState>& entities,
      std::optional<double> logicalTimeSeconds = std::nullopt);
  Result removeAll();
  std::size_t registeredObjectCount() const;

private:
  struct RegisteredEntity {
    ObjectInstanceId instanceId = 0;
    std::uint16_t entityNumber = 0;
    std::string objectClassName;
  };

  Result ensureClassPublished(const std::string& objectClassName);

  HlaRuntime& _runtime;
  std::unordered_set<std::string> _publishedClasses;
  std::unordered_map<std::string, RegisteredEntity> _registeredEntities;
  std::uint16_t _nextEntityNumber = 1;
};

} // namespace tactical::hla
