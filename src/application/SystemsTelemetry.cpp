#include "application/SystemsTelemetry.h"

#include <QtGlobal>

namespace application {
namespace {

bool containsAny(const QString& value, const QStringList& candidates) {
  for (const QString& candidate : candidates) {
    if (value.contains(candidate)) {
      return true;
    }
  }
  return false;
}

int inferredEngineCount(const Entity& entity) {
  const QString model = (entity.modelName + QLatin1Char(' ') +
                         entity.jsbsimAircraftModel)
                            .trimmed()
                            .toLower();
  if (containsAny(model, {
          QStringLiteral("b17"),
          QStringLiteral("b-17"),
          QStringLiteral("b747"),
          QStringLiteral("boeing 747"),
          QStringLiteral("c-130"),
          QStringLiteral("c130")})) {
    return 4;
  }
  if (containsAny(model, {
          QStringLiteral("typhoon"),
          QStringLiteral("rafale"),
          QStringLiteral("f/a-18"),
          QStringLiteral("f-18"),
          QStringLiteral("ef-18"),
          QStringLiteral("cf-18"),
          QStringLiteral("f-15"),
          QStringLiteral("a-10"),
          QStringLiteral("a10"),
          QStringLiteral("dhc6"),
          QStringLiteral("dhc-6"),
          QStringLiteral("c310")})) {
    return 2;
  }
  return entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0
      ? 1
      : 0;
}

} // namespace

QString systemsDisplayProfileForEntity(const Entity& entity) {
  if (!entity.systemsDisplayProfileId.trimmed().isEmpty()) {
    return entity.systemsDisplayProfileId.trimmed();
  }
  const int count = engineCountForEntity(entity);
  if (count <= 0) {
    return QStringLiteral("systems-none");
  }
  if (entity.category.compare(
          QStringLiteral("Helicopter"), Qt::CaseInsensitive) == 0) {
    return QStringLiteral("rotorcraft-%1-engine").arg(count);
  }
  return QStringLiteral("air-turbine-%1-engine").arg(count);
}

int engineCountForEntity(const Entity& entity) {
  return entity.engineCount > 0
      ? qBound(1, entity.engineCount, 8)
      : inferredEngineCount(entity);
}

SystemsTelemetrySnapshot makeEstimatedSystemsTelemetrySnapshot(
    const Entity& entity,
    double maximumSpeedKnots) {
  SystemsTelemetrySnapshot snapshot;
  snapshot.entityName = entity.name;
  snapshot.profileId = systemsDisplayProfileForEntity(entity);
  snapshot.dataSource = QStringLiteral("Kinematic estimate");

  const int engineCount = engineCountForEntity(entity);
  snapshot.engines.reserve(engineCount);
  const double speedRatio = maximumSpeedKnots > 0.0
      ? qBound(0.0, entity.speedKnots / maximumSpeedKnots, 1.0)
      : 0.0;
  const bool active = !entity.destroyed && entity.speedKnots > 0.5;
  for (int index = 0; index < engineCount; ++index) {
    EngineTelemetry engine;
    engine.engineId = QStringLiteral("ENG %1").arg(index + 1);
    engine.state = entity.destroyed
        ? QStringLiteral("FAILED")
        : (active ? QStringLiteral("RUNNING") : QStringLiteral("OFF"));
    engine.n1Percent = active ? 35.0 + speedRatio * 65.0 : 0.0;
    engine.n2Percent = active ? 52.0 + speedRatio * 48.0 : 0.0;
    engine.exhaustTemperatureCelsius = active
        ? 320.0 + speedRatio * 520.0
        : 20.0;
    engine.fuelFlowKilogramsPerHour = active
        ? 250.0 + speedRatio * 1750.0
        : 0.0;
    engine.thrustKilonewtons = active ? speedRatio * 80.0 : 0.0;
    engine.n1Available = true;
    engine.n2Available = true;
    engine.exhaustTemperatureAvailable = true;
    engine.fuelFlowAvailable = true;
    engine.thrustAvailable = true;
    engine.available = true;
    engine.estimated = true;
    snapshot.engines.push_back(engine);
  }
  return snapshot;
}

} // namespace application
