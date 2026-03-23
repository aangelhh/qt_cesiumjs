#pragma once

#include "domain/Entity.h"

#include <QString>

class EntityTextFormatter {
public:
  static QString listLabel(const Entity& entity);
  static QString statusMessage(const Entity& entity);
};
