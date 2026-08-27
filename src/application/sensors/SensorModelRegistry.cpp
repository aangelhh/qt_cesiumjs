#include "application/sensors/SensorModelRegistry.h"

#include "application/sensors/NativeSensorModel.h"

#include <algorithm>

namespace application::sensors {

SensorModelRegistry::SensorModelRegistry() {
  this->registerModel(std::make_shared<NativeSensorModel>());
}

bool SensorModelRegistry::registerModel(
    std::shared_ptr<const ISensorModel> model) {
  if (!model) {
    return false;
  }

  const QString id = normalizedModelId(model->modelId());
  if (id.isEmpty()) {
    return false;
  }
  _models.insert(id, std::move(model));
  return true;
}

const ISensorModel& SensorModelRegistry::resolve(
    const QString& requestedModelId) const {
  const QString requested = normalizedModelId(requestedModelId);
  const auto requestedIt = _models.constFind(requested);
  if (requestedIt != _models.cend()) {
    return *requestedIt.value();
  }

  const auto nativeIt = _models.constFind(NativeSensorModel::staticModelId());
  return *nativeIt.value();
}

QStringList SensorModelRegistry::modelIds() const {
  QStringList ids = _models.keys();
  std::sort(ids.begin(), ids.end());
  return ids;
}

const SensorModelRegistry& SensorModelRegistry::defaultRegistry() {
  static const SensorModelRegistry registry;
  return registry;
}

QString SensorModelRegistry::normalizedModelId(const QString& modelId) {
  return modelId.trimmed().toLower();
}

} // namespace application::sensors
