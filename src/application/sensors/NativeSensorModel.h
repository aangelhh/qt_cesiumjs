#pragma once

#include "application/sensors/ISensorModel.h"

namespace application::sensors {

class NativeSensorModel final : public ISensorModel {
public:
  static QString staticModelId();

  QString modelId() const override;
  SensorEvaluationResult evaluate(
      const SensorEvaluationContext& context) const override;
};

} // namespace application::sensors
