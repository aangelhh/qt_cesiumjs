#pragma once

#include "domain/Entity.h"

#include <QVector>

class SensorEngine {
public:
  static void updateEntityContacts(QVector<Entity>& entities);
};
