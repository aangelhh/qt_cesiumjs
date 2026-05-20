#pragma once

#include <QSet>
#include <QString>
#include <QVariantMap>
#include <functional>

namespace presentation {

/// Syncs a collection of objects to the map by sending new/updated tracks
/// and removing stale ones.
///
/// \param items         Current collection to sync.
/// \param extractId     Returns the stable track ID for an item.
/// \param buildSummary  Returns the QVariantMap track summary for an item.
/// \param previousNames Track IDs that were active in the previous sync.
/// \param sendTrack     Callback that pushes a track to the map.
/// \param removeTrack   Callback that removes a track from the map by ID.
/// \return              The set of IDs that are active after this sync.
template<typename T>
QSet<QString> syncCollectionToMap(
    const QVector<T>&                                            items,
    std::function<QString(const T&)>                            extractId,
    std::function<QVariantMap(const T&)>                        buildSummary,
    const QSet<QString>&                                        previousNames,
    std::function<void(const QVariantMap& summary, bool select)> sendTrack,
    std::function<void(const QString& id)>                      removeTrack)
{
  QSet<QString> currentNames;
  for (const T& item : items) {
    currentNames.insert(extractId(item));
    sendTrack(buildSummary(item), false);
  }
  for (const QString& name : previousNames) {
    if (!currentNames.contains(name)) {
      removeTrack(name);
    }
  }
  return currentNames;
}

} // namespace presentation
