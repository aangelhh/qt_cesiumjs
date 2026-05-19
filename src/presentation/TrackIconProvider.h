#pragma once

#include <QColor>
#include <QIcon>
#include <QString>

namespace presentation {

/// Returns the accent color associated with a force label (Friendly/Opposing/etc.)
QColor forceColorFromLabel(const QString& team);

/// Returns the single-letter glyph used in track icons for a given entity category.
QString categoryGlyph(const QString& category);

/// Builds a small colored icon for a tactical graphic (route, waypoint, area, etc.)
QIcon makeTacticalGraphicIcon(const QString& graphicType);

/// Builds a small colored track icon for an entity (16x16 rounded rect with glyph).
QIcon makeTrackIcon(const QString& team, const QString& category, bool isGroup);

/// Builds a text-glyph fallback icon for the task quickbar.
QIcon makeTaskQuickFallbackIcon(const QString& glyph, const QColor& accent);

/// Loads a task quickbar icon from disk; falls back to makeTaskQuickFallbackIcon.
QIcon loadTaskQuickBarIcon(
    const QString& iconFilePath,
    const QString& fallbackGlyph,
    const QColor& accent);

} // namespace presentation
