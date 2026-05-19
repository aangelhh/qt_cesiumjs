#pragma once

#include <QHash>
#include <QSet>
#include <QString>

struct Entity;

namespace presentation {

// Captured initial location of an entity, used as the implicit "Return to Base"
// destination when no explicit base waypoint has been assigned.
struct EntityHomePosition {
    double latitude = 0.0;
    double longitude = 0.0;
    int altitudeMeters = 0;
    bool valid = false;
};

// Records the first observed position of each entity and exposes it back to
// callers as a snapshot. Once an entity is remembered the value is sticky
// (subsequent remember() calls for the same name are ignored). Extracted from
// MainWindow as part of the controller-split refactor.
class EntityHomePositionTracker {
public:
    // Capture entity.{latitude,longitude,altitude} the first time this name is
    // seen. No-op on empty names or repeated calls for the same entity.
    void remember(const Entity& entity);

    // Returns a default-constructed (invalid) record if the entity was never
    // remembered.
    EntityHomePosition positionFor(const QString& entityName) const;

    // Drop any record whose entity name is not in the supplied set.
    void pruneTo(const QSet<QString>& validEntityNames);

private:
    QHash<QString, EntityHomePosition> m_positions;
};

} // namespace presentation
