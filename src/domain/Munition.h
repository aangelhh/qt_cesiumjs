#pragma once

#include <QString>

struct ActiveMunition {
  QString id;
  QString launcherEntityName;
  int forceIdentifier = 0;

  QString munitionType;
  QString modelUri;
  QString targetEntityName;
  bool guidanceActive = false;
  QString status = QStringLiteral("Flying");
  double hitRadiusMeters = 120.0;
  double blastRadiusMeters = 0.0;
  double baseDamage = 0.0;

  double latitude = 0.0;
  double longitude = 0.0;
  double altitudeMeters = 0.0;

  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;

  double speedMetersPerSecond = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
  double ageSeconds = 0.0;
  double ttlSeconds = 12.0;

  bool active = true;
};

struct TransientEffect {
  QString id;
  QString effectType;
  int forceIdentifier = 0;
  double latitude = 0.0;
  double longitude = 0.0;
  double altitudeMeters = 0.0;
  double ageSeconds = 0.0;
  double ttlSeconds = 0.4;
  bool active = true;
};
