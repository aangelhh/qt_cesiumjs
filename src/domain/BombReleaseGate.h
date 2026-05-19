#pragma once

#include <QString>

struct Entity;

namespace domain {

constexpr double kKnotsToMetersPerSecond = 0.514444;
constexpr double kBombReleaseGravityMps2 = 9.81;
constexpr double kBombReleaseHeadingConeDegrees = 35.0;
constexpr double kBombReleaseDistanceToleranceMeters = 150.0;

struct BombReleaseGateEvaluation {
  bool valid = false;
  bool targetAhead = false;
  bool withinHeadingCone = false;
  bool withinReleaseWindow = false;

  bool readyToRelease() const {
    return valid && targetAhead && withinHeadingCone && withinReleaseWindow;
  }

  QString stateLabel() const {
    return readyToRelease() ? QStringLiteral("In Release Window")
                           : QStringLiteral("Armed");
  }
};

BombReleaseGateEvaluation evaluateBombReleaseGate(
    const Entity& launcher,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters);

int weaponQuantity(const Entity& entity, const QString& weaponType);

} // namespace domain
