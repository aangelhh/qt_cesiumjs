#pragma once

#include <QHash>
#include <QSet>
#include <QString>

namespace presentation {

// Per-entity, presentation-only visual flags. Persisted as JSON outside the
// scenario so it survives reloads but never appears in the domain model.
struct EntityVisualState {
    bool hidden = false;
    bool radarCoverageVisible = false;
    bool trackHistoryVisible = false;
};

// Owns the in-memory map of EntityVisualState records and the JSON file they
// are persisted to. Extracted from MainWindow as part of the controller-split
// refactor. Pure value semantics — no Qt widgets involved, no dependency on
// the scenario or application layer.
class EntityVisualStateManager {
public:
    // jsonFilePath is the absolute path to the persistence file
    // (e.g. <projectRoot>/Data/entity_visual_state.json).
    explicit EntityVisualStateManager(QString jsonFilePath);

    // Snapshot lookup: returns a default-constructed state if unknown.
    EntityVisualState stateFor(const QString& entityName) const;

    // Mutable accessor; inserts a default record if absent.
    EntityVisualState& ensureState(const QString& entityName);

    // Remove a single entity's record (no-op if absent).
    void remove(const QString& entityName);

    bool contains(const QString& entityName) const;

    // Replace the entire backing map by reloading from disk. Silently no-ops
    // if the file is missing or malformed (matches the legacy behaviour).
    void load();

    // Persist the current map atomically (truncates and rewrites the file).
    void save() const;

    // Drop any record whose entity name is not in the supplied set. Auto-saves
    // only when something was actually removed.
    void pruneTo(const QSet<QString>& validEntityNames);

private:
    QString m_jsonFilePath;
    QHash<QString, EntityVisualState> m_states;
};

} // namespace presentation
