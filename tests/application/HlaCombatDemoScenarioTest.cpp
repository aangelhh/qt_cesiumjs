#include <gtest/gtest.h>

#include "application/HlaCombatDemoScenario.h"

TEST(HlaCombatDemoScenario, BuildsFollowThenDestroyPlan) {
  const application::HlaCombatDemoScenario demo =
      application::makeHlaCombatDemoScenario();

  EXPECT_NE(demo.friendly.entityId, demo.opposing.entityId);
  EXPECT_NE(demo.friendly.forceIdentifier, demo.opposing.forceIdentifier);
  EXPECT_EQ(demo.friendly.modelName, QStringLiteral("Mirage 2000"));
  EXPECT_TRUE(demo.friendly.modelUri.endsWith(
      QStringLiteral("dassault_mirage_2000.glb")));
  ASSERT_FALSE(demo.friendly.sensors.isEmpty());
  EXPECT_TRUE(demo.friendly.sensors.first().enabled);
  EXPECT_TRUE(demo.friendly.sensors.first().emitting);
  EXPECT_DOUBLE_EQ(
      demo.friendly.sensors.first().azimuthWidthDegrees,
      120.0);
  EXPECT_DOUBLE_EQ(
      demo.friendly.sensors.first().elevationWidthDegrees,
      60.0);
  ASSERT_EQ(demo.friendlyPlanTasks.size(), 2);
  EXPECT_EQ(demo.friendlyPlanTasks[0].taskType, QStringLiteral("FollowEntity"));
  EXPECT_EQ(
      demo.friendlyPlanTasks[1].taskType,
      QStringLiteral("AttackUntilDestroyed"));
  EXPECT_EQ(
      demo.friendlyPlanTasks[0].targetEntityId,
      demo.opposing.entityId);
  EXPECT_EQ(
      demo.friendlyPlanTasks[1].targetEntityId,
      demo.opposing.entityId);
  ASSERT_FALSE(demo.friendly.weapons.isEmpty());
  EXPECT_GE(demo.friendly.weapons.first().quantity, 12);
}
