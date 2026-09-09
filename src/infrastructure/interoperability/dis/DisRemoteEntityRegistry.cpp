#include "infrastructure/interoperability/dis/DisRemoteEntityRegistry.h"

namespace tactical::dis {

void DisRemoteEntityRegistry::configure(
    std::uint8_t exerciseId,
    std::uint16_t localSiteId,
    std::uint16_t localApplicationId,
    qint64 timeoutMilliseconds) {
  clear();
  _exerciseId = exerciseId;
  _localSiteId = localSiteId;
  _localApplicationId = localApplicationId;
  _timeoutMilliseconds = timeoutMilliseconds;
}

void DisRemoteEntityRegistry::clear() {
  _lastSeenMilliseconds.clear();
  _states.clear();
  _changes.clear();
}

void DisRemoteEntityRegistry::accept(
    const EntityState& state,
    qint64 nowMilliseconds) {
  if (state.exerciseId != _exerciseId) return;
  if (state.identifier.site == _localSiteId &&
      state.identifier.application == _localApplicationId) {
    return;
  }
  const QString key = state.identifier.key();
  _states.insert(key, state);
  _lastSeenMilliseconds.insert(key, nowMilliseconds);
  _changes.push_back({state, false});
}

void DisRemoteEntityRegistry::expire(qint64 nowMilliseconds) {
  for (auto iterator = _lastSeenMilliseconds.begin();
       iterator != _lastSeenMilliseconds.end();) {
    if (nowMilliseconds - iterator.value() <= _timeoutMilliseconds) {
      ++iterator;
      continue;
    }
    const QString key = iterator.key();
    _changes.push_back({_states.value(key), true});
    _states.remove(key);
    iterator = _lastSeenMilliseconds.erase(iterator);
  }
}

std::vector<RemoteEntityChange> DisRemoteEntityRegistry::takeChanges() {
  std::vector<RemoteEntityChange> result;
  result.swap(_changes);
  return result;
}

qsizetype DisRemoteEntityRegistry::size() const { return _states.size(); }

} // namespace tactical::dis
