#pragma once

#include "domain/Entity.h"

#include <QString>
#include <QtGlobal>

namespace application::sensors {

struct SensorEvaluationContext {
  quint32 scenarioSeed = 0;
  double simulationTimeSeconds = 0.0;
  qint64 evaluationIndex = 0;
  double rangeMeters = 0.0;
  const Entity& observer;
  const SensorDefinition& sensor;
  const Entity& target;
};

struct SensorEvaluationResult {
  double probability = 0.0;
  double sample = 0.0;
  bool detected = false;
  QString effectiveModelId;
};

class ISensorModel {
public:
  virtual ~ISensorModel() = default;

  virtual QString modelId() const = 0;
  virtual SensorEvaluationResult evaluate(
      const SensorEvaluationContext& context) const = 0;
};

} // namespace application::sensors
