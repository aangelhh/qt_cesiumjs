#include "presentation/EntityVisualStateManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

namespace presentation {

namespace {
constexpr auto kRootKey = "entities";
constexpr auto kHiddenKey = "hidden";
constexpr auto kRadarKey = "radarCoverageVisible";
constexpr auto kHistoryKey = "trackHistoryVisible";
} // namespace

EntityVisualStateManager::EntityVisualStateManager(QString jsonFilePath)
    : m_jsonFilePath(std::move(jsonFilePath)) {}

EntityVisualState EntityVisualStateManager::stateFor(const QString& entityName) const {
    return m_states.value(entityName);
}

EntityVisualState& EntityVisualStateManager::ensureState(const QString& entityName) {
    return m_states[entityName];
}

void EntityVisualStateManager::remove(const QString& entityName) {
    m_states.remove(entityName);
}

bool EntityVisualStateManager::contains(const QString& entityName) const {
    return m_states.contains(entityName);
}

void EntityVisualStateManager::load() {
    m_states.clear();

    QFile file(m_jsonFilePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        return;
    }

    const QJsonObject entitiesObject =
        document.object().value(QLatin1String(kRootKey)).toObject();
    for (auto it = entitiesObject.begin(); it != entitiesObject.end(); ++it) {
        const QJsonObject stateObject = it.value().toObject();
        EntityVisualState visualState;
        visualState.hidden = stateObject.value(QLatin1String(kHiddenKey)).toBool(false);
        visualState.radarCoverageVisible =
            stateObject.value(QLatin1String(kRadarKey)).toBool(false);
        visualState.trackHistoryVisible =
            stateObject.value(QLatin1String(kHistoryKey)).toBool(false);
        m_states.insert(it.key(), visualState);
    }
}

void EntityVisualStateManager::save() const {
    QJsonObject entitiesObject;
    for (auto it = m_states.constBegin(); it != m_states.constEnd(); ++it) {
        entitiesObject.insert(it.key(), QJsonObject{
                                            {QLatin1String(kHiddenKey), it.value().hidden},
                                            {QLatin1String(kRadarKey), it.value().radarCoverageVisible},
                                            {QLatin1String(kHistoryKey), it.value().trackHistoryVisible},
                                        });
    }

    QFile file(m_jsonFilePath);
    const QFileInfo info(file);
    QDir().mkpath(info.absolutePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }

    const QJsonDocument document(QJsonObject{
        {QLatin1String(kRootKey), entitiesObject},
    });
    file.write(document.toJson(QJsonDocument::Indented));
}

void EntityVisualStateManager::pruneTo(const QSet<QString>& validEntityNames) {
    bool removed = false;
    for (auto it = m_states.begin(); it != m_states.end();) {
        if (!validEntityNames.contains(it.key())) {
            it = m_states.erase(it);
            removed = true;
            continue;
        }
        ++it;
    }

    if (removed) {
        save();
    }
}

} // namespace presentation
