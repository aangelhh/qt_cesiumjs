#pragma once

#include <QMainWindow>
#include <QHash>
#include <QHBoxLayout>
#include <QFrame>
#include <QList>
#include <QModelIndex>
#include <QPointer>
#include <QSet>
#include <QStringList>
#include <QVariantMap>
#include <QVector>

#include <memory>

#include "application/AttackTaskProcessor.h"
#include "application/SimulationEngine.h"
#include "presentation/BombReleaseController.h"
#include "presentation/EntityPlanExecutor.h"
#include "presentation/GraphicPickCoordinator.h"
#include "presentation/PlanStepConfigurator.h"
#include "presentation/PlanTypes.h"

class QAction;
class AddEntityDialog;
class AssignTaskDialog;
class MapBridge;
class QMenu;
class ScenarioState;
class QStandardItem;
class QStandardItemModel;
class QTimer;
class QPoint;
class QEvent;
class QToolButton;
class QWidget;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
class QWebEngineView;
#endif
struct AreaDefinition;
struct RouteGraphic;
struct Waypoint;

namespace presentation {
class BombReleaseController;
class EntityHomePositionTracker;
class EntityPlanExecutor;
class EntityVisualStateManager;
class GraphicPickCoordinator;
class PlanStepConfigurator;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
  void openAddEntityDialog();
  void onEntityDialogAccepted(const struct Entity& entity);
  void beginEntityCoordinatePick();
  void reportPickedCoordinate(double longitude, double latitude, double height);
  void handleBombPickCoordinate(double longitude, double latitude);
  void reportMapStatus(const QString& message);
  void updateSelectedTrackPanel(const QModelIndex& current, const QModelIndex& previous);
  void handleDetectedContactSelection(const QModelIndex& current, const QModelIndex& previous);
  void handleMapTrackSelection(const QString& trackName);
  void openMapEntityContextMenu(const QString& trackName, int viewX, int viewY);
  void toggleTacticalOverlays(bool enabled);
  void openSelectedEntityDetails();
  void openObjectsContextMenu(const QPoint& position);
  void assignFlyHeadingAltitudeSpeedTask();
  void assignMoveToLocationTask();
  void assignMoveToWaypointTask();
  void assignMoveAlongRouteTask();
  void assignPatrolAreaTask();
  void assignOrbitAreaTask();
  void assignReturnToBaseTask();
  void assignPatrolRouteTask();
  void assignOrbitHoldLocationTask();
  bool resolveOrbitCenter(double& outLatitude, double& outLongitude);
  void assignFollowEntityTask();
  void assignAttackAirTask();
  void assignAttackSurfaceTask();
  void openEntityPlanDialog();
  void setSelectedEntityHeading();
  void setSelectedEntityAltitude();
  void setSelectedEntitySpeed();
  void setSelectedEntityBehaviorMode(const QString& behaviorMode);
  void destroySelectedEntity();
  void restoreSelectedEntity();
  void addMissileToSelectedEntity();
  void addBombToSelectedEntity();
  void launchMissileFromSelectedEntity();
  void launchMissileAtSelectedEntity();
  void executeMissileLaunch(const QString& launcherName, const QString& targetName, int previousMissileCount);
  void releaseBombFromSelectedEntity();
  void releaseBombAtSurfaceEntity();
  void queueBombReleaseAtEntity(const QString& launcherName, const struct Entity& target);
  void releaseBombAtCustomCoordinates();
  void cancelPendingBombRelease();
  void clearSelectedTask();
  void deleteSelectedEntity();
  void openAddWaypointDialog();
  void openAddRouteDialog();
  void openAddAreaDialog();
  void openAddCircleAreaDialog(const QString& name, double altitudeMeters);
  void openAddEllipseAreaDialog(const QString& name, double altitudeMeters);
  void startSimulation();
  void pauseSimulation();
  void stopSimulation();

private:
  // PendingBombRelease, PlanStepKind, PlanStep, EntityPlan now in
  // presentation/PlanTypes.h

  void initializeModels();
  void initializeObjectTreeModel();
  void initializeContactsTableModel();
  void appendEntityToUi(const struct Entity& entity);
  QStandardItem* rootItemForForceIdentifier(int forceIdentifier) const;
  QStandardItem* ensureGroupItem(QStandardItem* parent, const QString& label, const QVariantMap& summary);
  void rebuildTacticalGraphicsTree();
  void appendLogMessage(const QString& message);
  void setSelectedTrackDetails(const QVariantMap& summary);
  QString buildSelectedEntityOperationalStatus(
      const QVariantMap& summary,
      const struct Entity* entity) const;
  void sendTrackToMap(const QVariantMap& summary, bool focus = false);
  void removeTrackFromMap(const QString& trackName);
  void sendDraftGraphicToMap(const QVariantMap& summary);
  void clearDraftGraphicFromMap(const QString& name);
  void syncTacticalGraphicsToMap();
  void syncTracksToMap();
  void syncDetectedContactsToUi();
  void syncScenarioStateToUi();
  /// Helpers called by syncScenarioStateToUi:
  bool syncEntityTreeToUi(const QString& selectedEntityNameBeforeSync);
  void syncActiveMunitionTracksToMap();
  void syncTransientEffectsToMap();
  void syncPendingBombTargetToMap();
  void selectObjectByName(const QString& trackName, bool notifyMap);
  QStandardItem* findTrackItemByName(QStandardItem* parent, const QString& trackName) const;
  const struct Entity* findEntityByName(const QString& entityName) const;
  QString selectedEntityName() const;
  QString selectedObjectName() const;
  bool currentSelectionIsEntity() const;
  bool currentSelectionIsOperableEntity() const;
  bool selectedEntityIsDestroyed() const;
  bool currentSelectionIsTacticalGraphic() const;
  void queuePendingBombRelease(
      const QString& launcherEntityName,
      double targetLatitude,
      double targetLongitude,
      double targetAltitudeMeters,
      const QString& targetLabel,
      const QString& sourceDescription,
      const QString& targetEntityName = QString(),
      bool logQueued = true,
      bool focusLauncher = true);
  void clearPendingBombRelease();
  QString cleanupRuntimeReferencesForRemovedEntity(const QString& entityName);
  void validatePendingBombRelease();
  void processAttackTasks(double deltaSeconds);
  void processAutoBombingBehaviors(double deltaSeconds);
  void processPendingBombRelease();
  void openAssignTaskDialog(const QString& initialTaskType);
  void populateTaskCommands();
  void beginTaskCoordinatePick();
  void beginGraphicCoordinatePick();
  void updateSimulationControls();
  void createTaskQuickBar();
  void populateTaskQuickBarButtons(QFrame* panel, QHBoxLayout* layout);
  void positionTaskQuickBar();
  void updateTaskQuickBarState();
  void showTaskQuickPlaceholder(const QString& actionName);
  void populateEntityContextMenu(QMenu& menu);
  QVariantMap makeEntityTrackSummary(const struct Entity& entity) const;
  const Waypoint* findWaypointByName(const QString& waypointName) const;
  const RouteGraphic* findRouteByName(const QString& routeName) const;
  const AreaDefinition* findAreaByNameOrId(const QString& areaNameOrId) const;
  QStringList availableWaypointNames() const;
  QStringList availableRouteNames(bool requirePoints) const;
  QStringList availableAreaNames() const;
  EntityPlan& ensureEntityPlan(const QString& entityName);
  void pruneEntityPlans();
  bool captureTaskConfiguration(
      const QString& entityName,
      const EntityTask& initialTask,
      const QString& initialTaskType,
      EntityTask& outTask);
  bool configurePlanStep(const QString& entityName, PlanStepKind kind, PlanStep& step);
  bool startEntityPlan(const QString& entityName);
  void stopEntityPlan(const QString& entityName, bool clearCurrentTask);
  void advanceEntityPlans();
  bool applyEntityTask(
      const QString& entityName,
      const EntityTask& task,
      bool syncUi = true);
  bool resolveSelectedEntityFlyTargets(
      double& headingDegrees,
      int& altitudeMeters,
      double& speedKnots) const;
  void applyFlyHeadingAltitudeSpeedTask(
      double headingDegrees,
      int altitudeMeters,
      double speedKnots);
  void setSelectedEntityDestroyed(bool destroyed);
  void setSelectedEntityHidden(bool hidden);
  void setSelectedEntityRadarCoverageVisible(bool visible);
  void setSelectedEntityTrackHistoryVisible(bool visible);
  void focusSelectedEntityInMap();
  void showContextMenuPlaceholder(const QString& actionName);

  Ui::MainWindow* _ui;
  QWidget* _contentWidget;
  QWidget* _taskQuickBar;
  MapBridge* _mapBridge;
  ScenarioState* _scenarioState;
  QStandardItemModel* _objectsModel;
  QStandardItemModel* _detectedContactsModel;
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
  QList<QToolButton*> _taskQuickButtons;
  QSet<QString> _activeMunitionTrackNames;
  QSet<QString> _activeEffectTrackNames;
  application::SimulationEngine* m_simulationEngine;
  std::unique_ptr<presentation::BombReleaseController> _bombReleaseController;
  std::unique_ptr<application::AttackTaskProcessor> _attackTaskProcessor;
  std::unique_ptr<presentation::EntityPlanExecutor> _planExecutor;
  std::unique_ptr<presentation::EntityVisualStateManager> _entityVisualStateManager;
  std::unique_ptr<presentation::EntityHomePositionTracker> _entityHomePositionTracker;
  std::unique_ptr<presentation::GraphicPickCoordinator> _graphicPickCoordinator;
  std::unique_ptr<presentation::PlanStepConfigurator> _planStepConfigurator;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  QWebEngineView* _webView;
#endif
};
