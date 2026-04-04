#pragma once

#include <QString>
#include <QVector>

struct Waypoint {
  QString name;
  double latitude = 0.0;
  double longitude = 0.0;
  double altitudeMeters = 0.0;
};

struct RoutePoint {
  double latitude = 0.0;
  double longitude = 0.0;
  double altitudeMeters = 0.0;
};

struct RouteGraphic {
  QString name;
  QVector<RoutePoint> points;
};

struct AreaDefinition {
  QString id;
  QString name;
  QString areaType = QStringLiteral("Circle");
  int forceIdentifier = 0;
  double centerLatitude = 0.0;
  double centerLongitude = 0.0;
  double centerAltitudeMeters = 0.0;
  double radiusMeters = 1000.0;
  double semiMajorAxisMeters = 1000.0;
  double semiMinorAxisMeters = 600.0;
  double rotationDegrees = 0.0;
  QVector<RoutePoint> points;
  double minAltitudeMeters = 0.0;
  double maxAltitudeMeters = 0.0;
  QString notes;
};
