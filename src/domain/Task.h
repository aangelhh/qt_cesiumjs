#pragma once

#include <QString>

struct EntityTask {
  QString taskType;
  bool enabled = false;
  QString status = QStringLiteral("Idle");
  double targetHeadingDegrees = 0.0;
  int targetAltitudeMeters = 0;
  double targetSpeedKnots = 0.0;
  double targetLatitude = 0.0;
  double targetLongitude = 0.0;
  QString targetEntityName;
  QString targetWaypointName;
  QString targetRouteName;
  QString targetAreaName;
  double targetAreaRadiusMeters = 0.0;
};
