#pragma once

#include "infrastructure/SensorModelProviderCatalog.h"

#include <QStringList>

class ScenarioState;

namespace infrastructure {

class SensorModelProviderBootstrap {
public:
  static QStringList registerEnabledProviders(
      ScenarioState& scenarioState,
      const QVector<SensorModelProviderEntry>& providers);
};

} // namespace infrastructure
