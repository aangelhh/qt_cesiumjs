#include "presentation/DetectedContactsPresenter.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/EntityIdentity.h"
#include "domain/Sensor.h"
#include <QSet>
#include <QString>

namespace presentation {

QVector<DetectedContactRow> buildDetectedContactRows(const QVector<Entity>& entities) {
  QVector<DetectedContactRow> rows;
  QSet<QString> insertedPairs;

  const auto findEntityByName = [&entities](const QString& name) -> const Entity* {
    for (const Entity& entity : entities) {
      if (domain::entityMatchesReference(entity, name)) {
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

      const QString targetReference = contact.targetEntityId.trimmed().isEmpty()
          ? contact.targetEntityName
          : contact.targetEntityId;
      const QString pairKey =
          domain::entityKey(observer) + QStringLiteral("::") + targetReference;
      if (insertedPairs.contains(pairKey)) {
        continue;
      }

      const Entity* target = findEntityByName(targetReference);
      if (!target) {
        continue;
      }

      insertedPairs.insert(pairKey);

      DetectedContactRow row;
      row.observerEntityId = domain::entityKey(observer);
      row.observerName  = observer.name;
      row.targetEntityId = domain::entityKey(*target);
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
