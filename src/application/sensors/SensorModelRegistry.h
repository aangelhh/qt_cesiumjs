#pragma once

#include "application/sensors/ISensorModel.h"

#include <QHash>
#include <QStringList>

#include <memory>

namespace application::sensors {

class SensorModelRegistry {
public:
  SensorModelRegistry();

  bool registerModel(std::shared_ptr<const ISensorModel> model);
  const ISensorModel& resolve(const QString& requestedModelId) const;
  QStringList modelIds() const;

  static const SensorModelRegistry& defaultRegistry();

private:
  static QString normalizedModelId(const QString& modelId);

  QHash<QString, std::shared_ptr<const ISensorModel>> _models;
};

} // namespace application::sensors
