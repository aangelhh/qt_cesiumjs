#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QPointer>
#include <QVariantMap>

#include "application/SimulationEngine.h"

class QAction;
class AddEntityDialog;
class AssignTaskDialog;
class MapBridge;
class ScenarioState;
class QStandardItem;
class QStandardItemModel;
class QTimer;
class QPoint;
class QWidget;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
class QWebEngineView;
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  void openAddEntityDialog();
  void beginEntityCoordinatePick();
  void reportPickedCoordinate(double longitude, double latitude, double height);
  void reportMapStatus(const QString& message);
  void updateSelectedTrackPanel(const QModelIndex& current, const QModelIndex& previous);
  void handleMapTrackSelection(const QString& trackName);
  void toggleTacticalOverlays(bool enabled);
  void openSelectedEntityDetails();
  void openObjectsContextMenu(const QPoint& position);
  void assignFlyHeadingAltitudeSpeedTask();
  void assignMoveToLocationTask();
  void assignMoveToWaypointTask();
  void assignMoveAlongRouteTask();
  void assignPatrolAreaTask();
  void assignOrbitAreaTask();
  void assignFollowEntityTask();
  void clearSelectedTask();
  void deleteSelectedEntity();
  void openAddWaypointDialog();
  void openAddRouteDialog();
  void openAddAreaDialog();
  void startSimulation();
  void pauseSimulation();
  void stopSimulation();

private:
  void initializeModels();
  void appendEntityToUi(const class Entity& entity);
  QStandardItem* rootItemForForceIdentifier(int forceIdentifier) const;
  QStandardItem* ensureGroupItem(QStandardItem* parent, const QString& label, const QVariantMap& summary);
  void rebuildTacticalGraphicsTree();
  void appendLogMessage(const QString& message);
  void setSelectedTrackDetails(const QVariantMap& summary);
  void sendTrackToMap(const QVariantMap& summary, bool focus = false);
  void removeTrackFromMap(const QString& trackName);
  void sendDraftGraphicToMap(const QVariantMap& summary);
  void clearDraftGraphicFromMap(const QString& name);
  void syncTacticalGraphicsToMap();
  void syncTracksToMap();
  void syncScenarioStateToUi();
  void selectObjectByName(const QString& trackName, bool notifyMap);
  QStandardItem* findTrackItemByName(QStandardItem* parent, const QString& trackName) const;
  QString selectedEntityName() const;
  QString selectedObjectName() const;
  bool currentSelectionIsEntity() const;
  bool currentSelectionIsTacticalGraphic() const;
  void openAssignTaskDialog(const QString& initialTaskType);
  void populateTaskCommands();
  void beginTaskCoordinatePick();
  void beginGraphicCoordinatePick();
  void updateSimulationControls();

  Ui::MainWindow* _ui;
  QWidget* _contentWidget;
  MapBridge* _mapBridge;
  ScenarioState* _scenarioState;
  QStandardItemModel* _objectsModel;
  QStandardItem* _friendlyRootItem;
  QStandardItem* _opposingRootItem;
  QStandardItem* _neutralRootItem;
  QStandardItem* _tacticalGraphicsRootItem;
  QPointer<AddEntityDialog> _entityDialog;
  QPointer<AssignTaskDialog> _taskDialog;
  QAction* _addWaypointAction;
  QAction* _addRouteAction;
  QAction* _addAreaAction;
  QTimer* _simulationTimer;
  bool _applyingMapSelection;
  bool _simulationRunning;
  QString _pendingGraphicMode;
  QString _pendingGraphicName;
  QVector<QVariantMap> _pendingRoutePoints;
  QString _pendingAreaType;
  double _pendingAreaRadiusMeters;
  double _pendingAreaAltitudeMeters;
  double _pendingAreaSemiMajorMeters;
  double _pendingAreaSemiMinorMeters;
  double _pendingAreaRotationDegrees;
  QVector<QVariantMap> _pendingAreaPoints;
  application::SimulationEngine* m_simulationEngine;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  QWebEngineView* _webView;
#endif
};
