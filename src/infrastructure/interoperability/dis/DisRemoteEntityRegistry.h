#pragma once

#include "infrastructure/interoperability/dis/DisTypes.h"

#include <QHash>

#include <vector>

namespace tactical::dis {

class DisRemoteEntityRegistry {
public:
  void configure(
      std::uint8_t exerciseId,
      std::uint16_t localSiteId,
      std::uint16_t localApplicationId,
      qint64 timeoutMilliseconds);
  void clear();
  void accept(const EntityState& state, qint64 nowMilliseconds);
  void expire(qint64 nowMilliseconds);
  std::vector<RemoteEntityChange> takeChanges();
  qsizetype size() const;

private:
  std::uint8_t _exerciseId = 1;
  std::uint16_t _localSiteId = 1;
  std::uint16_t _localApplicationId = 1;
  qint64 _timeoutMilliseconds = 12000;
  QHash<QString, qint64> _lastSeenMilliseconds;
  QHash<QString, EntityState> _states;
  std::vector<RemoteEntityChange> _changes;
};

} // namespace tactical::dis
