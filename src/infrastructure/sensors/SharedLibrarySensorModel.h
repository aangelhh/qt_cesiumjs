#pragma once

#include "application/sensors/ISensorModel.h"
#include "infrastructure/sensors/SensorModelPluginApi.h"

#include <QLibrary>

namespace infrastructure::sensors {

class SharedLibrarySensorModel final
    : public application::sensors::ISensorModel {
public:
  SharedLibrarySensorModel(QString configuredModelId, QString libraryPath);

  QString modelId() const override;
  application::sensors::SensorEvaluationResult evaluate(
      const application::sensors::SensorEvaluationContext& context) const override;

  bool isAvailable() const;
  QString errorString() const;

private:
  QString _modelId;
  QString _libraryPath;
  mutable QLibrary _library;
  QttestSensorPluginEvaluateFn _evaluate = nullptr;
  QString _providerVersion;
  QString _errorString;
};

} // namespace infrastructure::sensors
