#include "presentation/DetectedContactsPresenter.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/Sensor.h"
#include <QSet>
#include <QString>

namespace presentation {

QVector<DetectedContactRow> buildDetectedContactRows(const QVector<Entity>& entities) {
  QVector<DetectedContactRow> rows;
  QSet<QString> insertedPairs;

  const auto findEntityByName = [&entities](const QString& name) -> const Entity* {
    for (const Entity& entity : entities) {
      if (entity.name == name) {
        return &entity;
      }
    }
    return nullptr;
  };

  for (const Entity& observer : entities) {
    for (const SensorContact& contact : observer.sensorContacts) {
      if (!contact.detected) {
        continue;
      }

      const QString pairKey =
          observer.name + QStringLiteral("::") + contact.targetEntityName;
      if (insertedPairs.contains(pairKey)) {
        continue;
      }

      const Entity* target = findEntityByName(contact.targetEntityName);
      if (!target) {
        continue;
      }

      insertedPairs.insert(pairKey);

      DetectedContactRow row;
      row.observerName  = observer.name;
      row.targetName    = target->name;
      row.forceLabel    = domain::forceIdentifierLabel(target->forceIdentifier);
      row.typeLabel     = target->type.trimmed().isEmpty() ? target->category : target->type;
      row.rangeText     = QStringLiteral("%1 km").arg(contact.rangeMeters / 1000.0, 0, 'f', 1);
      row.bearingText   = QStringLiteral("%1 deg").arg(contact.bearingDegrees, 0, 'f', 1);
      row.altitudeText  = QStringLiteral("%1 m").arg(target->altitude);
      rows.append(row);
    }
  }

  return rows;
}

} // namespace presentation
