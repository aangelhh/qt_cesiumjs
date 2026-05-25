#include "presentation/TacticalGraphicsEditorController.h"

namespace presentation {

TacticalGraphicsEditorController::TacticalGraphicsEditorController(
    GraphicPickCoordinator* coordinator,
    CountFn                 waypointCount,
    CountFn                 routeCount,
    CountFn                 areaCount,
    AskTextFn               askText,
    PickItemFn              pickItem,
    AskDoubleFn             askDouble,
    QObject*                parent)
    : QObject(parent)
    , _coordinator(coordinator)
    , _waypointCount(std::move(waypointCount))
    , _routeCount(std::move(routeCount))
    , _areaCount(std::move(areaCount))
    , _askText(std::move(askText))
    , _pickItem(std::move(pickItem))
    , _askDouble(std::move(askDouble)) {}

void TacticalGraphicsEditorController::openAddWaypointDialog() {
  bool ok = false;
  const QString name = _askText(
      QStringLiteral("Add Waypoint"),
      QStringLiteral("Waypoint name"),
      QStringLiteral("Waypoint %1").arg(_waypointCount() + 1),
      ok).trimmed();
  if (!ok || name.isEmpty()) {
    return;
  }
  _coordinator->beginWaypointPick(name);
}

void TacticalGraphicsEditorController::openAddRouteDialog() {
  bool ok = false;
  const QString name = _askText(
      QStringLiteral("Add Route"),
      QStringLiteral("Route name"),
      QStringLiteral("Route %1").arg(_routeCount() + 1),
      ok).trimmed();
  if (!ok || name.isEmpty()) {
    return;
  }
  _coordinator->beginRoutePick(name);
}

void TacticalGraphicsEditorController::openAddAreaDialog() {
  bool ok = false;
  const QString name = _askText(
      QStringLiteral("Add Area"),
      QStringLiteral("Area name"),
      QStringLiteral("Area %1").arg(_areaCount() + 1),
      ok).trimmed();
  if (!ok || name.isEmpty()) {
    return;
  }

  const QStringList areaTypes = {
      QStringLiteral("Circle"),
      QStringLiteral("Ellipse"),
      QStringLiteral("Polygon"),
  };
  const QString areaType = _pickItem(
      QStringLiteral("Area Type"), QStringLiteral("Type"),
      areaTypes, 0, ok);
  if (!ok) {
    return;
  }

  const double areaAltitudeMeters = _askDouble(
      QStringLiteral("Area Altitude"),
      QStringLiteral("Center altitude (m)"),
      0.0, -1000.0, 80000.0, 1, ok);
  if (!ok) {
    return;
  }

  if (areaType == QStringLiteral("Circle")) {
    openAddCircleAreaDialog(name, areaAltitudeMeters);
  } else if (areaType == QStringLiteral("Ellipse")) {
    openAddEllipseAreaDialog(name, areaAltitudeMeters);
  } else {
    _coordinator->beginAreaPolygonPick(name, areaAltitudeMeters);
  }
}

void TacticalGraphicsEditorController::openAddCircleAreaDialog(
    const QString& name, double altitudeMeters) {
  bool ok = false;
  const double radiusMeters = _askDouble(
      QStringLiteral("Area Radius"),
      QStringLiteral("Radius (m)"),
      5000.0, 50.0, 500000.0, 0, ok);
  if (!ok) {
    return;
  }
  _coordinator->beginAreaCirclePick(name, {altitudeMeters, radiusMeters});
}

void TacticalGraphicsEditorController::openAddEllipseAreaDialog(
    const QString& name, double altitudeMeters) {
  bool ok = false;
  const double semiMajorMeters = _askDouble(
      QStringLiteral("Ellipse Semi-major Axis"),
      QStringLiteral("Semi-major axis (m)"),
      6000.0, 50.0, 500000.0, 0, ok);
  if (!ok) {
    return;
  }
  const double semiMinorMeters = _askDouble(
      QStringLiteral("Ellipse Semi-minor Axis"),
      QStringLiteral("Semi-minor axis (m)"),
      3000.0, 50.0, 500000.0, 0, ok);
  if (!ok) {
    return;
  }
  const double rotationDegrees = _askDouble(
      QStringLiteral("Ellipse Rotation"),
      QStringLiteral("Rotation (deg)"),
      0.0, -360.0, 360.0, 1, ok);
  if (!ok) {
    return;
  }
  _coordinator->beginAreaEllipsePick(
      name, {altitudeMeters, semiMajorMeters, semiMinorMeters, rotationDegrees});
}

} // namespace presentation
