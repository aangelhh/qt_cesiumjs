#pragma once

#include <QObject>
#include <QString>
#include <functional>

#include "presentation/GraphicPickCoordinator.h"

/// Handles "Add Waypoint / Route / Area" commands, collecting names and
/// geometry parameters via injected callbacks, then delegating coordinate
/// picking to GraphicPickCoordinator.
///
/// No Qt widget dependency — all QInputDialog calls are injected.
namespace presentation {

class TacticalGraphicsEditorController : public QObject {
  Q_OBJECT
public:
  using CountFn    = std::function<int()>;
  using AskTextFn  = std::function<QString(
      const QString& title, const QString& label,
      const QString& defaultText, bool& ok)>;
  using PickItemFn = std::function<QString(
      const QString& title, const QString& label,
      const QStringList& items, int current, bool& ok)>;
  using AskDoubleFn = std::function<double(
      const QString& title, const QString& label,
      double def, double min, double max, int decimals, bool& ok)>;

  explicit TacticalGraphicsEditorController(
      GraphicPickCoordinator* coordinator,
      CountFn                 waypointCount,
      CountFn                 routeCount,
      CountFn                 areaCount,
      AskTextFn               askText,
      PickItemFn              pickItem,
      AskDoubleFn             askDouble,
      QObject*                parent = nullptr);

  void openAddWaypointDialog();
  void openAddRouteDialog();
  void openAddAreaDialog();

private:
  void openAddCircleAreaDialog(const QString& name, double altitudeMeters);
  void openAddEllipseAreaDialog(const QString& name, double altitudeMeters);

  GraphicPickCoordinator* _coordinator;
  CountFn                 _waypointCount;
  CountFn                 _routeCount;
  CountFn                 _areaCount;
  AskTextFn               _askText;
  PickItemFn              _pickItem;
  AskDoubleFn             _askDouble;
};

} // namespace presentation
