#include "presentation/EntityHomePositionTracker.h"

#include "domain/Entity.h"

namespace presentation {

void EntityHomePositionTracker::remember(const Entity& entity) {
    if (entity.name.trimmed().isEmpty() || m_positions.contains(entity.name)) {
        return;
    }

    m_positions.insert(entity.name, EntityHomePosition{
                                        entity.latitude,
                                        entity.longitude,
                                        entity.altitude,
                                        true,
                                    });
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
