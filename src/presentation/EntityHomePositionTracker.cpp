#include "presentation/EntityHomePositionTracker.h"

#include "domain/Entity.h"
#include "domain/EntityIdentity.h"

namespace presentation {

void EntityHomePositionTracker::remember(const Entity& entity) {
    const QString entityName = entity.name.trimmed();
    const QString entityKey = domain::entityKey(entity);
    if (entityName.isEmpty() || entityKey.isEmpty()) {
        return;
    }

    const EntityHomePosition position{
                                        entity.latitude,
                                        entity.longitude,
                                        entity.altitude,
                                        true,
                                    };
    if (!m_positions.contains(entityKey)) {
        m_positions.insert(entityKey, position);
    }
    // Legacy callers may still query by display name. The first entity keeps
    // that alias; UUID lookups remain unambiguous for duplicate names.
    if (!m_positions.contains(entityName)) {
        m_positions.insert(entityName, position);
    }
}

EntityHomePosition EntityHomePositionTracker::positionFor(const QString& entityName) const {
    return m_positions.value(entityName);
}

void EntityHomePositionTracker::pruneTo(const QSet<QString>& validEntityNames) {
    for (auto it = m_positions.begin(); it != m_positions.end();) {
        if (!validEntityNames.contains(it.key())) {
            it = m_positions.erase(it);
            continue;
        }
        ++it;
    }
}

} // namespace presentation
