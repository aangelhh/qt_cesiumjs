#pragma once

#include <QVariantMap>
#include <QVector>
#include <QString>
#include <functional>

class ScenarioState;

namespace presentation {

/// Owns the state-machine for "pick a coordinate on the map to create a
/// graphic" (Waypoint, Route, Circle/Ellipse Area, Polygon Area).
///
/// MainWindow wires the callbacks in its constructor and delegates all
/// pick-flow state to this class.
class GraphicPickCoordinator {
public:
  using StatusFn    = std::function<void(const QString&)>;
  using SyncUiFn    = std::function<void()>;
  using SendDraftFn = std::function<void(const QVariantMap&)>;
  using ClearDraftFn= std::function<void(const QString&)>;
  using BeginPickFn = std::function<void()>;

  GraphicPickCoordinator(
      ScenarioState*  state,
      StatusFn        status,
      SyncUiFn        syncUi,
      SendDraftFn     sendDraft,
      ClearDraftFn    clearDraft,
      BeginPickFn     beginPick);

  // ── Setup methods (called from the "Add …" dialog handlers) ──────────
  void beginWaypointPick(const QString& name);
  void beginWaypointPick(const QString& name, double altitudeMeters);
  void beginRoutePick(const QString& name);
  void beginRoutePick(const QString& name, double firstAltitudeMeters, double secondAltitudeMeters);

  struct AreaCircleParams  { double altitudeMeters; double radiusMeters; };
  struct AreaEllipseParams { double altitudeMeters; double semiMajorMeters;
                             double semiMinorMeters; double rotationDegrees; };
  void beginAreaCirclePick  (const QString& name, const AreaCircleParams&  p);
  void beginAreaEllipsePick (const QString& name, const AreaEllipseParams& p);
  void beginAreaPolygonPick (const QString& name, double altitudeMeters);

  // ── Query ─────────────────────────────────────────────────────────────
  bool isPending() const;

  /// Consume one map coordinate. Returns true if the coordinate was
  /// consumed by an active pick-flow (caller should not handle it further).
  bool handleCoordinate(double longitude, double latitude, double height);

private:
  // state
  QString              _mode;
  QString              _name;
  QVector<QVariantMap> _routePoints;
  QVector<QVariantMap> _areaPoints;
  QString              _areaType;
  bool                 _waypointAltitudeConfigured{false};
  double               _waypointAltitudeMeters{0.0};
  QVector<double>      _routePointAltitudesMeters;
  double               _areaAltitudeMeters{0.0};
  double               _areaRadiusMeters{0.0};
  double               _areaSemiMajorMeters{0.0};
  double               _areaSemiMinorMeters{0.0};
  double               _areaRotationDegrees{0.0};

  // dependencies
  ScenarioState* _state;
  StatusFn       _status;
  SyncUiFn       _syncUi;
  SendDraftFn    _sendDraft;
  ClearDraftFn   _clearDraft;
  BeginPickFn    _beginPick;

  // helpers
  void  clearState();
  QVariantMap makeDraftSummary(
      const QString& label, const QString& type, const QString& altLabel,
      const QString& posLabel, const QString& status,
      double lat, double lon) const;
};

} // namespace presentation
