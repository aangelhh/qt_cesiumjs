#pragma once

#include "domain/Entity.h"

#include <QtGlobal>
#include <QVector>

namespace application::sensors {
class SensorModelRegistry;
}

class SensorEngine {
public:
  static constexpr quint32 kDefaultRandomSeed = 0x5eed1234U;

  static void updateEntityContacts(
      QVector<Entity>& entities,
      double simulationTimeSeconds = 0.0,
      quint32 scenarioSeed = kDefaultRandomSeed,
      const application::sensors::SensorModelRegistry* modelRegistry = nullptr);
};
