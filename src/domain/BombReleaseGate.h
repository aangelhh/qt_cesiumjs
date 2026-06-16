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
  double relativeAltitudeMeters = 0.0;
  double horizontalSpeedMetersPerSecond = 0.0;
  double timeToImpactSeconds = -1.0;
  double releaseDistanceMeters = -1.0;
  double distanceToTargetMeters = -1.0;
  double desiredHeadingDegrees = 0.0;
  double headingErrorDegrees = 0.0;
  double distanceErrorMeters = 0.0;
  double secondsToReleaseWindow = 0.0;

  bool readyToRelease() const {
    return valid && targetAhead && withinHeadingCone && withinReleaseWindow;
  }

  QString stateLabel() const {
    return readyToRelease() ? QStringLiteral("In Release Window")
                           : QStringLiteral("Armed");
  }

  QString ccrpCueLabel() const {
    if (!valid) {
      return QStringLiteral("Invalid");
    }
    if (!targetAhead) {
      return QStringLiteral("Target Behind");
    }
    if (!withinHeadingCone) {
      return QStringLiteral("Steer");
    }
    return withinReleaseWindow ? QStringLiteral("Release")
                               : QStringLiteral("Hold");
  }
};

BombReleaseGateEvaluation evaluateBombReleaseGate(
    const Entity& launcher,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters);

int weaponQuantity(const Entity& entity, const QString& weaponType);

} // namespace domain
