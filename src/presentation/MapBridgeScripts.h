#pragma once

#include <QVariantMap>
#include <QString>

namespace presentation {

/// Pure functions that build JavaScript call strings for the CesiumJS
/// map bridge. All functions return empty strings for invalid inputs so the
/// caller can skip the runJavaScript call when the result is empty.
namespace MapBridgeScripts {

/// Script to add/update a track or tactical graphic on the map.
/// Returns an empty string if \p summary is empty.
QString buildAddOrUpdateScript(const QVariantMap& summary, bool focus);

/// Script to remove a track and/or graphic from the map by name.
/// Returns an empty string if \p trackName is empty/whitespace.
QString buildRemoveScript(const QString& trackName);

/// Script to add/update a draft graphic (while the user is drawing).
/// Returns an empty string if \p summary is empty.
QString buildAddOrUpdateDraftScript(const QVariantMap& summary);

/// Script to remove a draft graphic by name.
/// Returns an empty string if \p name is empty/whitespace.
QString buildRemoveDraftScript(const QString& name);

} // namespace MapBridgeScripts
} // namespace presentation
