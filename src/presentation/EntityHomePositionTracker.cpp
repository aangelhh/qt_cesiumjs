#include "presentation/EntityHomePositionTracker.h"

#include "application/ScenarioState.h"
#include "domain/Entity.h"

#include <QSet>

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

void EntityHomePositionTracker::pruneAgainst(const ScenarioState& scenario) {
    QSet<QString> validEntityNames;
    for (const Entity& entity : scenario.entities()) {
        validEntityNames.insert(entity.name);
    }

    for (auto it = m_positions.begin(); it != m_positions.end();) {
        if (!validEntityNames.contains(it.key())) {
            it = m_positions.erase(it);
            continue;
        }
        ++it;
    }
}

} // namespace presentation
