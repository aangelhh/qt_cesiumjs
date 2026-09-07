#include "infrastructure/interoperability/hla/HlaEntityPublisher.h"

#include <unordered_set>

namespace tactical::hla {

HlaEntityPublisher::HlaEntityPublisher(HlaRuntime& runtime)
    : _runtime(runtime) {}

Result HlaEntityPublisher::synchronize(
    const std::vector<RprEntityState>& entities,
    std::optional<double> logicalTimeSeconds) {
  std::unordered_set<std::string> activeEntityIds;
  activeEntityIds.reserve(entities.size());

  for (const RprEntityState& entity : entities) {
    if (entity.stableId.empty()) {
      return Result::failure("Cannot publish an entity without a stable id");
    }
    activeEntityIds.insert(entity.stableId);
    const std::string objectClassName =
        RprFomEncoding::objectClassName(entity.domain);
    Result result = this->ensureClassPublished(objectClassName);
    if (!result.success) return result;

    auto iterator = _registeredEntities.find(entity.stableId);
    if (iterator == _registeredEntities.end()) {
      ObjectInstanceId instanceId = 0;
      result = _runtime.registerObjectInstance(
          objectClassName,
          RprFomEncoding::objectInstanceName(entity.stableId),
          instanceId);
      if (!result.success) return result;

      std::uint16_t entityNumber = _nextEntityNumber++;
      if (_nextEntityNumber == 0 || _nextEntityNumber >= 0xfffeU) {
        _nextEntityNumber = 1;
      }
      iterator = _registeredEntities.emplace(
          entity.stableId,
          RegisteredEntity{instanceId, entityNumber, objectClassName}).first;
    } else if (iterator->second.objectClassName != objectClassName) {
      return Result::failure(
          "Changing an HLA object's RPR class at runtime is unsupported");
    }

    const std::vector<NamedValue> attributes = RprFomEncoding::encodeAttributes(
        entity, 1, 1, iterator->second.entityNumber);
    result = logicalTimeSeconds
        ? _runtime.updateObjectAttributesAtTime(
              iterator->second.instanceId, attributes, *logicalTimeSeconds)
        : _runtime.updateObjectAttributes(
              iterator->second.instanceId, attributes);
    if (!result.success) return result;
  }

  for (auto iterator = _registeredEntities.begin();
       iterator != _registeredEntities.end();) {
    if (activeEntityIds.count(iterator->first) != 0) {
      ++iterator;
      continue;
    }
    const Result result = logicalTimeSeconds
        ? _runtime.deleteObjectInstanceAtTime(
              iterator->second.instanceId, *logicalTimeSeconds)
        : _runtime.deleteObjectInstance(iterator->second.instanceId);
    if (!result.success) return result;
    iterator = _registeredEntities.erase(iterator);
  }
  return Result::ok();
}

Result HlaEntityPublisher::removeAll() {
  for (const auto& item : _registeredEntities) {
    const Result result = _runtime.deleteObjectInstance(item.second.instanceId);
    if (!result.success) return result;
  }
  _registeredEntities.clear();
  return Result::ok();
}

std::size_t HlaEntityPublisher::registeredObjectCount() const {
  return _registeredEntities.size();
}

Result HlaEntityPublisher::ensureClassPublished(
    const std::string& objectClassName) {
  if (_publishedClasses.count(objectClassName) != 0) {
    return Result::ok();
  }
  const Result result = _runtime.publishObjectClass(
      objectClassName, RprFomEncoding::publishedAttributeNames());
  if (result.success) {
    _publishedClasses.insert(objectClassName);
  }
  return result;
}

} // namespace tactical::hla
