#include "CombatRules.h"
#include "Entity.h"
#include <cmath>

namespace domain {

bool attackSurfaceCoordinatesAreUsable(double latitude, double longitude) {
  if (!std::isfinite(latitude) || !std::isfinite(longitude)) {
    return false;
  }
  if (latitude < -90.0 || latitude > 90.0 ||
      longitude < -180.0 || longitude > 180.0) {
    return false;
  }
  // Reject (0, 0) as likely uninitialized
  constexpr double kEpsilon = 1e-6;
  return !(std::abs(latitude) < kEpsilon && std::abs(longitude) < kEpsilon);
}

bool attackTaskStatusIsTerminal(const QString& status) {
  return status == QStringLiteral("Completed") ||
         status == QStringLiteral("Failed") ||
         status == QStringLiteral("Target unavailable");
}

int autoBehaviorDamageReactionLevel(const Entity& entity) {
  if (entity.destroyed) {
    return 3;
  }
  if (entity.damagePercent >= 80.0) {
    return 2;
  }
  if (entity.damagePercent >= 50.0) {
    return 1;
  }
  return 0;
}

bool autoBehaviorCanEngageByDamage(const Entity& entity) {
  return autoBehaviorDamageReactionLevel(entity) == 0;
}

QString forceIdentifierLabel(int forceIdentifier) {
  switch (forceIdentifier) {
    case 1: return QStringLiteral("Friendly");
    case 2: return QStringLiteral("Opposing");
    case 3: return QStringLiteral("Neutral");
    default: return QStringLiteral("Unknown");
  }
}

bool entityCanUseMissileActions(const Entity& entity) {
  return !entity.destroyed &&
         entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0 &&
         entity.category.compare(QStringLiteral("Fighter"), Qt::CaseInsensitive) == 0;
}

} // namespace domain
