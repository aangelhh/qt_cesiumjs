#pragma once

#include <QMainWindow>
#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"
#ifdef QTTEST_HAS_DIS
#include "infrastructure/interoperability/dis/DisTypes.h"
#endif
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

#include <cstdint>
#include <memory>

#include "application/AttackTaskProcessor.h"
#include "application/CockpitControlService.h"
#include "application/SimulationEngine.h"
#include "application/StartupConfiguration.h"
#include "presentation/BombReleaseController.h"
#include "presentation/EntityPlanExecutor.h"
#include "presentation/GraphicPickCoordinator.h"
#include "presentation/PlanStepConfigurator.h"
#include "presentation/PlanTypes.h"
#include "presentation/AssignTaskController.h"
#include "presentation/BombReleaseActionsController.h"
#include "presentation/EntityStateActionsController.h"
#include "presentation/ScenarioObjectEditorController.h"
#include "presentation/SimulationLifecycleController.h"
#include "presentation/TacticalGraphicsEditorController.h"
#include "presentation/TaskAssignmentController.h"
#include "presentation/WeaponActionsController.h"

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
class QDockWidget;
class QToolButton;
class QWidget;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
class QWebEngineView;
#endif
struct AreaDefinition;
struct ActiveMunition;
struct TransientEffect;
struct Entity;
struct RouteGraphic;
struct Waypoint;

namespace presentation {
class BombReleaseController;
class EntityHomePositionTracker;
class EntityPlanDialog;
class EntityPlanExecutor;
class EntityVisualStateManager;
class GraphicPickCoordinator;
class DisConnectionPanel;
class HlaConnectionPanel;
class KinematicsCockpitWidget;
class PlanStepConfigurator;
}

namespace infrastructure {
class Ros2TelemetryPublisher;
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  QVector<Entity> entitySnapshot() const;
  QVector<ActiveMunition> activeMunitionSnapshot() const;
  QVector<TransientEffect> transientEffectSnapshot() const;
  void startHlaCombatDemo();
  void applyHlaRemoteEntityChanges(
      const std::vector<tactical::hla::RemoteEntityChange>& changes);
#ifdef QTTEST_HAS_DIS
  void applyDisRemoteEntityChanges(
      const std::vector<tactical::dis::RemoteEntityChange>& changes);
  void applyDisRemoteWarfareEvents(
      const std::vector<tactical::dis::WarfareEvent>& events);
  void configureDisConnection(
      const application::DisStartupConfiguration& configuration,
      bool backendAvailable,
      bool connected,
      bool blockedByHla);
  void setDisConnectionState(
      bool connected,
      bool connecting,
      const QString& detail = QString());
  void updateDisStatistics(
      std::uint64_t transmittedPdus,
      std::uint64_t receivedPdus,
      qsizetype remoteEntities);
  void setDisBlockedByHla(bool blocked);
#endif
  void applyHlaRemoteMunitionChanges(
      const std::vector<tactical::hla::RemoteMunitionChange>& changes);
  void applyHlaRemoteSensorChanges(
      const std::vector<tactical::hla::RemoteSensorChange>& changes);
  void applyHlaRemoteWarfareEvents(
      const std::vector<tactical::hla::RemoteWarfareEvent>& events);
  void applyHlaRemoteSimulationControl(
      tactical::hla::RemoteSimulationControl control);
#ifdef QTTEST_HAS_DIS
  void applyDisRemoteRadarEmissions(const std::vector<tactical::dis::RadarEmission>& emissions);
  bool applyDisEntityManagement(const tactical::dis::EntityManagementRequest& request);
  void applyDisInteractions(const std::vector<tactical::dis::IffState>& iff,
      const std::vector<tactical::dis::CollisionEvent>& collisions);
#endif
  void reportHlaSynchronizationStatus(const QString& message);
  void setHlaTimeManagementActive(bool active);
  void applyHlaTimeAdvanceGrant(double logicalTimeSeconds);
  void configureHlaConnection(
      const application::HlaStartupConfiguration& configuration,
      bool backendAvailable,
      bool connected);
  void setHlaConnectionState(
      bool connected,
      bool connecting,
      const QString& detail = QString());
  void setHlaBlockedByDis(bool blocked);

signals:
  void hlaSimulationControlRequested(
      tactical::hla::RemoteSimulationControl control,
      double simulationTimeSeconds);
  void hlaTimeAdvanceRequested(double logicalTimeSeconds);
  void hlaConnectRequested(
      const QString& federationName,
      const QString& federateName);
  void hlaDisconnectRequested();
#ifdef QTTEST_HAS_DIS
  void disConnectRequested(
      const application::DisStartupConfiguration& configuration);
  void disDisconnectRequested();
  void disEntityManagementRequested(const tactical::dis::EntityManagementRequest& request);
  void disIffRequested(const tactical::dis::IffState& state);
  void disCollisionRequested(const tactical::dis::CollisionEvent& event);
  void disTestEntityStateRequested(
      const tactical::dis::EntityState& state);
#endif

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
  void assignWaitOnLocationTask();
  void assignMoveToWaypointTask();
  void assignMoveAlongRouteTask();
  void assignPatrolAreaTask();
  void assignOrbitAreaTask();
  void assignReturnToBaseTask();
  void assignPatrolRouteTask();
  void assignOrbitHoldLocationTask();
  void assignHoldRacetrackTask();
  void assignFollowEntityTask();
  void assignInterceptEntityTask();
  void assignAttackOnceTask();
  void assignAttackUntilDestroyedTask();
  void assignFireOnPositionTask();
  void assignFireInDirectionTask();
  void assignStopWeaponsTask();
  void assignAttackAirTask();
  void assignAttackSurfaceTask();
  void assignWaitUntilTargetDetectedTask();
  void assignWaitUntilTargetDestroyedTask();
  void assignWaitUntilDamagedTask();
  void assignWaitUntilTimeTask();
  void assignWaitUntilInRangeTask();
  void openEntityPlanDialog();
  void setSelectedEntityHeading();
  void setSelectedEntityAltitude();
  void setSelectedEntitySpeed();
  void setSelectedEntityFuel();
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
  void addBombTargetToQueue();
  void addCustomBombTargetToQueue();
  void clearBombTargetQueue();
  void cancelPendingBombRelease();
  void clearSelectedTask();
  void deleteSelectedEntity();
  void openAddWaypointDialog();
  void openAddRouteDialog();
  void openAddAreaDialog();
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
  void advanceSimulationTick(double deltaSeconds);
  void processPendingBombRelease();
  void openAssignTaskDialog(const QString& initialTaskType);
  void populateTaskCommands();
  void beginMapCoordinatePick();
  void beginTaskCoordinatePick();
  void beginBombCoordinatePick();
  void beginGraphicCoordinatePick();
  void updateSimulationControls();
  void initializeKinematicsCockpit();
  void initializeHlaConnectionPanel();
#ifdef QTTEST_HAS_DIS
  void initializeDisConnectionPanel();
#endif
  void initializeRos2Telemetry();
  void configureRos2Telemetry();
  void refreshKinematicsCockpitForEntity(const struct Entity* entity);
  void takeCockpitControl(
      const QString& entityName,
      double headingDegrees,
      int altitudeMeters,
      double speedKnots);
  void updateCockpitSetpoints(
      const QString& entityName,
      double headingDegrees,
      int altitudeMeters,
      double speedKnots);
  void releaseCockpitControl(const QString& entityName);
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
  QDockWidget* _kinematicsCockpitDock;
  presentation::KinematicsCockpitWidget* _kinematicsCockpitWidget;
  QDockWidget* _qflightCockpitDock;
  presentation::KinematicsCockpitWidget* _qflightCockpitWidget;
  QDockWidget* _ecamCockpitDock;
  presentation::KinematicsCockpitWidget* _ecamCockpitWidget;
  QDockWidget* _hlaConnectionDock;
  presentation::HlaConnectionPanel* _hlaConnectionPanel;
  QToolButton* _hlaStatusButton;
#ifdef QTTEST_HAS_DIS
  QDockWidget* _disConnectionDock;
  presentation::DisConnectionPanel* _disConnectionPanel;
  QToolButton* _disStatusButton;
#endif
  infrastructure::Ros2TelemetryPublisher* _ros2TelemetryPublisher;
  std::uint64_t _kinematicsTelemetrySubscriptionId;
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
  bool _entityCoordinatePickPending;
  bool _taskCoordinatePickPending;
  QPointer<presentation::EntityPlanDialog> _entityPlanDialog;
  QString _entityPlanDialogEntityName;
  QAction* _addWaypointAction;
  QAction* _addRouteAction;
  QAction* _addAreaAction;
  QTimer* _simulationTimer;
  bool _applyingMapSelection;
  bool _simulationRunning;
  bool _simulationStopped = true;
  bool _applyingHlaSimulationControl = false;
#ifdef QTTEST_HAS_DIS
  QHash<QString, QString> _disManagedOwners;
#endif
  bool _hlaTimeManagementActive = false;
  bool _hlaTimeAdvancePending = false;
  QList<QToolButton*> _taskQuickButtons;
  QSet<QString> _activeMunitionTrackNames;
  QSet<QString> _activeEffectTrackNames;
  application::SimulationEngine* m_simulationEngine;
  std::unique_ptr<presentation::BombReleaseController> _bombReleaseController;
  std::unique_ptr<application::AttackTaskProcessor> _attackTaskProcessor;
  std::unique_ptr<presentation::EntityPlanExecutor> _planExecutor;
  std::unique_ptr<application::CockpitControlService> _cockpitControlService;
  std::unique_ptr<presentation::EntityVisualStateManager> _entityVisualStateManager;
  std::unique_ptr<presentation::EntityHomePositionTracker> _entityHomePositionTracker;
  std::unique_ptr<presentation::GraphicPickCoordinator> _graphicPickCoordinator;
  std::unique_ptr<presentation::PlanStepConfigurator> _planStepConfigurator;
  std::unique_ptr<presentation::WeaponActionsController> _weaponActionsController;
  std::unique_ptr<presentation::EntityStateActionsController> _entityStateActionsController;
  std::unique_ptr<presentation::TaskAssignmentController> _taskAssignmentController;
  std::unique_ptr<presentation::TacticalGraphicsEditorController> _tacticalGraphicsEditorController;
  std::unique_ptr<presentation::BombReleaseActionsController> _bombReleaseActionsController;
  std::unique_ptr<presentation::ScenarioObjectEditorController> _scenarioObjectEditorController;
  std::unique_ptr<presentation::SimulationLifecycleController> _simulationLifecycleController;
  std::unique_ptr<presentation::AssignTaskController> _assignTaskController;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  QWebEngineView* _webView;
#endif
};
