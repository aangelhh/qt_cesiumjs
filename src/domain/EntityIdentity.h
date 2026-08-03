#pragma once

#include "domain/Entity.h"

#include <QString>

namespace domain {

inline void ensureEntityId(Entity& entity) {
  if (entity.entityId.trimmed().isEmpty()) {
    entity.entityId = QUuid::createUuid().toString(QUuid::WithoutBraces);
  }
}

inline QString entityKey(const Entity& entity) {
  const QString id = entity.entityId.trimmed();
  return id.isEmpty() ? entity.name.trimmed() : id;
}

inline bool entityMatchesReference(
    const Entity& entity,
    const QString& reference) {
  const QString trimmedReference = reference.trimmed();
  if (trimmedReference.isEmpty()) {
    return false;
  }
  if (!entity.entityId.trimmed().isEmpty() &&
      entity.entityId.compare(trimmedReference, Qt::CaseInsensitive) == 0) {
    return true;
  }
  return entity.name.compare(trimmedReference, Qt::CaseInsensitive) == 0;
}

inline QString targetEntityReference(const EntityTask& task) {
  const QString id = task.targetEntityId.trimmed();
  return id.isEmpty() ? task.targetEntityName.trimmed() : id;
}

} // namespace domain
