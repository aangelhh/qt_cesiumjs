#pragma once

#include <QString>
#include <QVector>

struct Entity;

namespace presentation {

/// Data for one row in the detected-contacts table.
struct DetectedContactRow {
  QString observerEntityId;
  QString observerName;
  QString targetEntityId;
  QString targetName;
  QString forceLabel;
  QString typeLabel;   ///< target->type if non-empty, otherwise category
  QString rangeText;   ///< e.g. "12.3 km"
  QString bearingText; ///< e.g. "045.0 deg"
  QString altitudeText;///< e.g. "3500 m"
};

/// Pure function: build all detected-contact rows from the entity list.
/// Deduplicates observer+target pairs and skips undetected contacts.
QVector<DetectedContactRow> buildDetectedContactRows(const QVector<Entity>& entities);

} // namespace presentation
