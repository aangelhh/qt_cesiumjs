#include <gtest/gtest.h>
#include "../../src/domain/CombatRules.h"
#include "../../src/domain/Entity.h"
#include <cmath>
#include <limits>

using namespace domain;

TEST(CombatRules, AttackSurfaceCoordinatesRejectsNaN) {
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(
      std::numeric_limits<double>::quiet_NaN(), 10.0));
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(
      40.0, std::numeric_limits<double>::infinity()));
}

TEST(CombatRules, AttackSurfaceCoordinatesRejectsOutOfRange) {
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(91.0, 0.0));
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(-91.0, 0.0));
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(0.0, 181.0));
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(0.0, -181.0));
}

TEST(CombatRules, AttackSurfaceCoordinatesRejectsZeroZero) {
  EXPECT_FALSE(attackSurfaceCoordinatesAreUsable(0.0, 0.0));
}

TEST(CombatRules, AttackSurfaceCoordinatesAcceptsValid) {
  EXPECT_TRUE(attackSurfaceCoordinatesAreUsable(40.4168, -3.7038));
  EXPECT_TRUE(attackSurfaceCoordinatesAreUsable(-33.8688, 151.2093));
}

TEST(CombatRules, AttackTaskStatusTerminal) {
  EXPECT_TRUE(attackTaskStatusIsTerminal(QStringLiteral("Completed")));
  EXPECT_TRUE(attackTaskStatusIsTerminal(QStringLiteral("Failed")));
  EXPECT_TRUE(attackTaskStatusIsTerminal(QStringLiteral("Target unavailable")));
  EXPECT_FALSE(attackTaskStatusIsTerminal(QStringLiteral("Running")));
  EXPECT_FALSE(attackTaskStatusIsTerminal(QStringLiteral("NotStarted")));
}

TEST(CombatRules, DamageReactionLevels) {
  Entity e;
  e.destroyed = false;
  e.damagePercent = 0.0;
  EXPECT_EQ(autoBehaviorDamageReactionLevel(e), 0);

  e.damagePercent = 50.0;
  EXPECT_EQ(autoBehaviorDamageReactionLevel(e), 1);

  e.damagePercent = 80.0;
  EXPECT_EQ(autoBehaviorDamageReactionLevel(e), 2);

  e.destroyed = true;
  EXPECT_EQ(autoBehaviorDamageReactionLevel(e), 3);
}

TEST(CombatRules, CanEngageByDamage) {
  Entity e;
  e.destroyed = false;
  e.damagePercent = 0.0;
  EXPECT_TRUE(autoBehaviorCanEngageByDamage(e));

  e.damagePercent = 50.0;
  EXPECT_FALSE(autoBehaviorCanEngageByDamage(e));
}

TEST(CombatRules, ForceIdentifierLabels) {
  EXPECT_EQ(domain::forceIdentifierLabel(1), QStringLiteral("Friendly"));
  EXPECT_EQ(domain::forceIdentifierLabel(2), QStringLiteral("Opposing"));
  EXPECT_EQ(domain::forceIdentifierLabel(3), QStringLiteral("Neutral"));
  EXPECT_EQ(domain::forceIdentifierLabel(99), QStringLiteral("Unknown"));
}

TEST(CombatRules, EntityCanUseMissileActions) {
  Entity e;
  e.destroyed = false;
  e.domain = QStringLiteral("Air");
  e.category = QStringLiteral("Fighter");
  EXPECT_TRUE(domain::entityCanUseMissileActions(e));

  e.destroyed = true;
  EXPECT_FALSE(domain::entityCanUseMissileActions(e));

  e.destroyed = false;
  e.domain = QStringLiteral("Surface");
  EXPECT_FALSE(domain::entityCanUseMissileActions(e));

  e.domain = QStringLiteral("Air");
  e.category = QStringLiteral("Transport");
  EXPECT_FALSE(domain::entityCanUseMissileActions(e));
}
