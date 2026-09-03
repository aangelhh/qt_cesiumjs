#include <gtest/gtest.h>

#include "application/ScenarioState.h"

namespace {

Entity entity(const QString& id, bool externallyControlled) {
  Entity result;
  result.entityId = id;
  result.name = id;
  result.externallyControlled = externallyControlled;
  return result;
}

SensorDefinition remoteRadar() {
  SensorDefinition sensor;
  sensor.id = QStringLiteral("hla:radar-1");
  sensor.name = QStringLiteral("HLA Radar");
  sensor.modelProviderId = QStringLiteral("hla-rpr");
  sensor.azimuthWidthDegrees = 120.0;
  return sensor;
}

} // namespace

TEST(HlaExternalState, RemoteSensorOnlyUpdatesExternallyControlledEntity) {
  ScenarioState state;
  state.reset();
  state.addEntity(entity(QStringLiteral("local"), false));
  state.upsertExternalEntity(entity(QStringLiteral("hla:remote"), true));

  state.upsertExternalSensor(QStringLiteral("local"), remoteRadar());
  state.upsertExternalSensor(QStringLiteral("hla:remote"), remoteRadar());

  const auto lock = state.lock();
  ASSERT_EQ(state.entities().size(), 2);
  EXPECT_TRUE(state.entities().at(0).sensors.isEmpty());
  ASSERT_EQ(state.entities().at(1).sensors.size(), 1);
  EXPECT_EQ(
      state.entities().at(1).sensors.first().modelProviderId,
      QStringLiteral("hla-rpr"));
}

TEST(HlaExternalState, ExternalEffectsAreDeduplicatedByEventId) {
  ScenarioState state;
  state.reset();
  TransientEffect effect;
  effect.id = QStringLiteral("hla-event-1");
  effect.effectType = QStringLiteral("ImpactFlash");

  state.appendExternalEffect(effect);
  state.appendExternalEffect(effect);

  const auto lock = state.lock();
  ASSERT_EQ(state.transientEffects().size(), 1);
  EXPECT_EQ(state.transientEffects().first().id, effect.id);
}

TEST(HlaExternalState, PlatformReflectionPreservesRemoteSensorObjects) {
  ScenarioState state;
  state.reset();
  Entity remote = entity(QStringLiteral("hla:remote"), true);
  state.upsertExternalEntity(remote);
  state.upsertExternalSensor(remote.entityId, remoteRadar());

  remote.latitude = 41.25;
  remote.longitude = -2.75;
  state.upsertExternalEntity(remote);

  const auto lock = state.lock();
  ASSERT_EQ(state.entities().size(), 1);
  EXPECT_DOUBLE_EQ(state.entities().first().latitude, 41.25);
  EXPECT_DOUBLE_EQ(state.entities().first().longitude, -2.75);
  ASSERT_EQ(state.entities().first().sensors.size(), 1);
  EXPECT_EQ(
      state.entities().first().sensors.first().id,
      QStringLiteral("hla:radar-1"));
}
