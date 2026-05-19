#include "presentation/GraphicPickCoordinator.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "presentation/TrackSummaryBuilder.h"
#include "domain/GeoMath.h"

#include <QtMath>

static constexpr double kGraphicAltitudeOffsetMeters = 50.0;
static constexpr double kPolygonCloseDistanceMeters  = 500.0;

namespace presentation {

GraphicPickCoordinator::GraphicPickCoordinator(
    ScenarioState*  state,
    StatusFn        status,
    SyncUiFn        syncUi,
    SendDraftFn     sendDraft,
    ClearDraftFn    clearDraft,
    BeginPickFn     beginPick)
  : _state(state)
  , _status(std::move(status))
  , _syncUi(std::move(syncUi))
  , _sendDraft(std::move(sendDraft))
  , _clearDraft(std::move(clearDraft))
  , _beginPick(std::move(beginPick)) {}

void GraphicPickCoordinator::beginWaypointPick(const QString& name) {
  _mode = QStringLiteral("Waypoint");
  _name = name;
  _status(QStringLiteral("Creando waypoint %1. Haz clic una vez en el mapa para fijar su posicion.").arg(name));
  _beginPick();
}

void GraphicPickCoordinator::beginRoutePick(const QString& name) {
  _mode = QStringLiteral("Route");
  _name = name;
  _routePoints.clear();
  _clearDraft(name + QStringLiteral(" (draft)"));
  _status(QStringLiteral("Creando route %1. Haz clic en el primer punto de la ruta en el mapa.").arg(name));
  _beginPick();
}

void GraphicPickCoordinator::beginAreaCirclePick(const QString& name, const AreaCircleParams& p) {
  _mode = QStringLiteral("Area");
  _name = name;
  _areaType = QStringLiteral("Circle");
  _areaAltitudeMeters = p.altitudeMeters;
  _areaRadiusMeters   = p.radiusMeters;
  _areaPoints.clear();
  _clearDraft(name + QStringLiteral(" (draft)"));
  _status(QStringLiteral("Creando area circular %1. Haz clic en el mapa para fijar el centro.").arg(name));
  _beginPick();
}

void GraphicPickCoordinator::beginAreaEllipsePick(const QString& name, const AreaEllipseParams& p) {
  _mode = QStringLiteral("Area");
  _name = name;
  _areaType              = QStringLiteral("Ellipse");
  _areaAltitudeMeters    = p.altitudeMeters;
  _areaSemiMajorMeters   = p.semiMajorMeters;
  _areaSemiMinorMeters   = p.semiMinorMeters;
  _areaRotationDegrees   = p.rotationDegrees;
  _areaPoints.clear();
  _clearDraft(name + QStringLiteral(" (draft)"));
  _status(QStringLiteral("Creando area eliptica %1. Haz clic en el mapa para fijar el centro.").arg(name));
  _beginPick();
}

void GraphicPickCoordinator::beginAreaPolygonPick(const QString& name, double altitudeMeters) {
  _mode = QStringLiteral("AreaPolygon");
  _name = name;
  _areaAltitudeMeters = altitudeMeters;
  _areaPoints.clear();
  _clearDraft(name + QStringLiteral(" (draft)"));
  _status(QStringLiteral("Creando polygon %1. Anade puntos en el mapa; para cerrar, pincha cerca del primer punto.")
      .arg(name));
  _beginPick();
}

bool GraphicPickCoordinator::isPending() const {
  return !_mode.trimmed().isEmpty();
}

bool GraphicPickCoordinator::handleCoordinate(double longitude, double latitude, double height) {
  if (!isPending()) {
    return false;
  }

  const double graphicAltitude = qMax(0.0, height + kGraphicAltitudeOffsetMeters);

  if (_mode == QStringLiteral("Waypoint")) {
    Waypoint waypoint;
    waypoint.name          = _name;
    waypoint.longitude     = longitude;
    waypoint.latitude      = latitude;
    waypoint.altitudeMeters= graphicAltitude;
    _state->addWaypoint(waypoint);
    _status(QStringLiteral("Waypoint %1 creado en lat %2, lon %3, alt %4 m.")
        .arg(_name)
        .arg(latitude,      0, 'f', 5)
        .arg(longitude,     0, 'f', 5)
        .arg(graphicAltitude,0,'f', 0));
    clearState();
    _syncUi();
    return true;
  }

  if (_mode == QStringLiteral("Route")) {
    _routePoints.push_back(QVariantMap{
        {QStringLiteral("longitude"),     longitude},
        {QStringLiteral("latitude"),      latitude},
        {QStringLiteral("altitudeMeters"),graphicAltitude},
    });
    if (_routePoints.size() >= 2) {
      RouteGraphic route;
      route.name = _name;
      for (const QVariantMap& pt : _routePoints) {
        RoutePoint rp;
        rp.longitude     = pt.value(QStringLiteral("longitude")).toDouble();
        rp.latitude      = pt.value(QStringLiteral("latitude")).toDouble();
        rp.altitudeMeters= pt.value(QStringLiteral("altitudeMeters")).toDouble();
        route.points.push_back(rp);
      }
      _state->addRoute(route);
      _status(QStringLiteral("Route %1 creada con %2 puntos. Ajustada a +%3 m sobre el terreno.")
          .arg(_name)
          .arg(route.points.size())
          .arg(kGraphicAltitudeOffsetMeters, 0, 'f', 0));
      _clearDraft(_name + QStringLiteral(" (draft)"));
      clearState();
      _syncUi();
    } else {
      QVariantMap draft = makeTrackSummary(
          _name + QStringLiteral(" (draft)"),
          QStringLiteral("Route"),
          QStringLiteral("Graphic"),
          QStringLiteral("%1 m").arg(graphicAltitude, 0, 'f', 0),
          domain::formatPosition(latitude, longitude),
          QStringLiteral("Route draft"),
          latitude, longitude);
      draft.insert(QStringLiteral("type"), QStringLiteral("Route"));
      QVariantList pts;
      for (const QVariantMap& pt : _routePoints) {
        pts.push_back(pt);
      }
      draft.insert(QStringLiteral("routePoints"), pts);
      _sendDraft(draft);
      _status(QStringLiteral("Primer punto de la ruta capturado. Selecciona ahora el segundo punto en el mapa."));
      _beginPick();
    }
    return true;
  }

  if (_mode == QStringLiteral("Area")) {
    AreaDefinition area;
    area.id                   = _name;
    area.name                 = _name;
    area.areaType             = _areaType;
    area.centerLongitude      = longitude;
    area.centerLatitude       = latitude;
    area.centerAltitudeMeters = _areaAltitudeMeters;
    area.radiusMeters         = _areaRadiusMeters;
    area.semiMajorAxisMeters  = _areaSemiMajorMeters;
    area.semiMinorAxisMeters  = _areaSemiMinorMeters;
    area.rotationDegrees      = _areaRotationDegrees;
    _state->addArea(area);
    _status(QStringLiteral("Area %1 (%2) creada en lat %3, lon %4.")
        .arg(_name)
        .arg(_areaType)
        .arg(latitude,  0, 'f', 5)
        .arg(longitude, 0, 'f', 5));
    clearState();
    _syncUi();
    return true;
  }

  if (_mode == QStringLiteral("AreaPolygon")) {
    const QVariantMap capturedPoint = {
        {QStringLiteral("longitude"),     longitude},
        {QStringLiteral("latitude"),      latitude},
        {QStringLiteral("altitudeMeters"),graphicAltitude},
    };

    if (_areaPoints.size() >= 3) {
      const QVariantMap& first = _areaPoints.first();
      const double closeDist = domain::distanceMeters(
          latitude, longitude,
          first.value(QStringLiteral("latitude")).toDouble(),
          first.value(QStringLiteral("longitude")).toDouble());

      if (closeDist <= kPolygonCloseDistanceMeters) {
        AreaDefinition area;
        area.id                   = _name;
        area.name                 = _name;
        area.areaType             = QStringLiteral("Polygon");
        area.centerLongitude      = first.value(QStringLiteral("longitude")).toDouble();
        area.centerLatitude       = first.value(QStringLiteral("latitude")).toDouble();
        area.centerAltitudeMeters = first.value(QStringLiteral("altitudeMeters")).toDouble();
        for (const QVariantMap& pt : _areaPoints) {
          RoutePoint rp;
          rp.longitude     = pt.value(QStringLiteral("longitude")).toDouble();
          rp.latitude      = pt.value(QStringLiteral("latitude")).toDouble();
          rp.altitudeMeters= pt.value(QStringLiteral("altitudeMeters")).toDouble();
          area.points.push_back(rp);
        }
        _state->addArea(area);
        _clearDraft(_name + QStringLiteral(" (draft)"));
        _status(QStringLiteral("Polygon area %1 creada con %2 puntos.")
            .arg(_name)
            .arg(area.points.size()));
        clearState();
        _syncUi();
        return true;
      }
    }

    // Not yet closing — accumulate the point and continue
    _areaPoints.push_back(capturedPoint);
    QVariantMap draft = makeTrackSummary(
        _name + QStringLiteral(" (draft)"),
        QStringLiteral("Area"),
        QStringLiteral("Graphic"),
        QStringLiteral("%1 m").arg(graphicAltitude, 0, 'f', 0),
        domain::formatPosition(latitude, longitude),
        QStringLiteral("Polygon draft"),
        latitude, longitude);
    draft.insert(QStringLiteral("type"),             QStringLiteral("Area"));
    draft.insert(QStringLiteral("areaType"),         QStringLiteral("Polygon"));
    draft.insert(QStringLiteral("radiusMeters"),     0.0);
    draft.insert(QStringLiteral("semiMajorAxisMeters"), 0.0);
    draft.insert(QStringLiteral("semiMinorAxisMeters"), 0.0);
    draft.insert(QStringLiteral("rotationDegrees"),  0.0);
    QVariantList pts;
    for (const QVariantMap& pt : _areaPoints) {
      pts.push_back(pt);
    }
    draft.insert(QStringLiteral("areaPoints"), pts);
    _sendDraft(draft);

    if (_areaPoints.size() >= 3) {
      _status(QStringLiteral("Punto %1 del polygon capturado. Para cerrar, pincha cerca del primer punto.")
          .arg(_areaPoints.size()));
    } else {
      _status(QStringLiteral("Punto %1 del polygon capturado. Sigue anadiendo puntos; para cerrar, pincha cerca del primero.")
          .arg(_areaPoints.size()));
    }
    _beginPick();
    return true;
  }

  return false;
}

void GraphicPickCoordinator::clearState() {
  _mode.clear();
  _name.clear();
  _routePoints.clear();
  _areaPoints.clear();
  _areaType.clear();
  _areaAltitudeMeters  = 0.0;
  _areaRadiusMeters    = 0.0;
  _areaSemiMajorMeters = 0.0;
  _areaSemiMinorMeters = 0.0;
  _areaRotationDegrees = 0.0;
}

QVariantMap GraphicPickCoordinator::makeDraftSummary(
    const QString& label, const QString& type, const QString& altLabel,
    const QString& posLabel, const QString& statusStr,
    double lat, double lon) const {
  return makeTrackSummary(label, type, QStringLiteral("Graphic"),
                          altLabel, posLabel, statusStr, lat, lon);
}

} // namespace presentation
