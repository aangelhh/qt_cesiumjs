#pragma once

#include <QMainWindow>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QPointer>
#include <QSet>
#include <QStringList>
#include <QVariantMap>
#include <QVector>

#include "application/SimulationEngine.h"

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
  void beginEntityCoordinatePick();
  void reportPickedCoordinate(double longitude, double latitude, double height);
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
  void releaseBombFromSelectedEntity();
  void releaseBombAtSurfaceEntity();
  void releaseBombAtCustomCoordinates();
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
  struct EntityVisualState {
    bool hidden = false;
    bool radarCoverageVisible = false;
    bool trackHistoryVisible = false;
  };

  struct EntityHomePosition {
    double latitude = 0.0;
    double longitude = 0.0;
    int altitudeMeters = 0;
    bool valid = false;
  };

  struct PendingBombRelease {
    QString launcherEntityName;
    QString targetEntityName;
    double targetLatitude = 0.0;
    double targetLongitude = 0.0;
    double targetAltitudeMeters = 0.0;
    QString targetLabel;
    QString sourceDescription;
    bool pending = false;
    bool releaseCommandIssued = false;
  };

  enum class PlanStepKind {
    MoveToLocation,
    MoveToWaypoint,
    MoveAlongRoute,
    PatrolArea,
    FlyHeadingAltitudeSpeed,
    OrbitHoldLocation,
    ReturnToBase,
    AttackAir,
    AttackSurface,
  };

  struct PlanStep {
    PlanStepKind kind = PlanStepKind::MoveToLocation;
    EntityTask task;
    QString label;
    QString status = QStringLiteral("NotStarted");
  };

  struct EntityPlan {
    QVector<PlanStep> steps;
    int currentStepIndex = -1;
    bool running = false;
    int currentStableTicks = 0;
    QString status = QStringLiteral("NotStarted");
  };

  void initializeModels();
  void appendEntityToUi(const class Entity& entity);
  QStandardItem* rootItemForForceIdentifier(int forceIdentifier) const;
  QStandardItem* ensureGroupItem(QStandardItem* parent, const QString& label, const QVariantMap& summary);
  void rebuildTacticalGraphicsTree();
  void appendLogMessage(const QString& message);
  void setSelectedTrackDetails(const QVariantMap& summary);
  QString buildSelectedEntityOperationalStatus(
      const QVariantMap& summary,
      const class Entity* entity) const;
  void sendTrackToMap(const QVariantMap& summary, bool focus = false);
  void removeTrackFromMap(const QString& trackName);
  void sendDraftGraphicToMap(const QVariantMap& summary);
  void clearDraftGraphicFromMap(const QString& name);
  void syncTacticalGraphicsToMap();
  void syncTracksToMap();
  void syncDetectedContactsToUi();
  void syncScenarioStateToUi();
  void selectObjectByName(const QString& trackName, bool notifyMap);
  QStandardItem* findTrackItemByName(QStandardItem* parent, const QString& trackName) const;
  const class Entity* findEntityByName(const QString& entityName) const;
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
  bool processAttackAirTask(const QString& entityName, double deltaSeconds);
  bool processAttackSurfaceTask(const QString& entityName);
  bool setEntityTaskStatus(const QString& entityName, const QString& status);
  void processAutoBombingBehaviors(double deltaSeconds);
  void processPendingBombRelease();
  void openAssignTaskDialog(const QString& initialTaskType);
  void populateTaskCommands();
  void beginTaskCoordinatePick();
  void beginGraphicCoordinatePick();
  void updateSimulationControls();
  void createTaskQuickBar();
  void positionTaskQuickBar();
  void updateTaskQuickBarState();
  void showTaskQuickPlaceholder(const QString& actionName);
  void populateEntityContextMenu(QMenu& menu);
  QVariantMap makeEntityTrackSummary(const class Entity& entity) const;
  EntityVisualState entityVisualStateFor(const QString& entityName) const;
  EntityVisualState& ensureEntityVisualState(const QString& entityName);
  EntityHomePosition entityHomePositionFor(const QString& entityName) const;
  const Waypoint* findWaypointByName(const QString& waypointName) const;
  const RouteGraphic* findRouteByName(const QString& routeName) const;
  const AreaDefinition* findAreaByNameOrId(const QString& areaNameOrId) const;
  QStringList availableWaypointNames() const;
  QStringList availableRouteNames(bool requirePoints) const;
  QStringList availableAreaNames() const;
  EntityPlan& ensureEntityPlan(const QString& entityName);
  void rememberEntityHomePosition(const class Entity& entity);
  QString entityVisualStatePath() const;
  void loadEntityVisualStates();
  void saveEntityVisualStates() const;
  void pruneEntityVisualStates();
  void pruneEntityHomePositions();
  void pruneEntityPlans();
  QString planStepDisplayLabel(const PlanStep& step) const;
  bool captureTaskConfiguration(
      const QString& entityName,
      const EntityTask& initialTask,
      const QString& initialTaskType,
      EntityTask& outTask);
  bool configurePlanStep(const QString& entityName, PlanStepKind kind, PlanStep& step);
  bool validatePlanStepForExecution(const PlanStep& step, QString* reason) const;
  bool activeTaskMatchesPlanStep(const class Entity& entity, const PlanStep& step) const;
  bool startEntityPlan(const QString& entityName);
  void stopEntityPlan(const QString& entityName, bool clearCurrentTask);
  void advanceEntityPlans();
  bool activePlanStepCompleted(const class Entity& entity, EntityPlan& plan) const;
  bool startPlanStepTask(const QString& entityName, EntityPlan& plan);
  void failRunningPlan(
      const QString& entityName,
      EntityPlan& plan,
      const QString& logMessage = QString(),
      const QString& statusMessage = QString());
  void completeRunningPlan(const QString& entityName, EntityPlan& plan, const QString& completedLabel);
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
  bool _isPickingBombTarget;
  QString _pendingGraphicMode;
  QString _pendingGraphicName;
  QString _bombTargetPickLauncherName;
  QVector<QVariantMap> _pendingRoutePoints;
  QString _pendingAreaType;
  double _pendingAreaRadiusMeters;
  double _pendingAreaAltitudeMeters;
  double _pendingAreaSemiMajorMeters;
  double _pendingAreaSemiMinorMeters;
  double _pendingAreaRotationDegrees;
  QVector<QVariantMap> _pendingAreaPoints;
  QList<QToolButton*> _taskQuickButtons;
  QSet<QString> _activeMunitionTrackNames;
  QSet<QString> _activeEffectTrackNames;
  QHash<QString, EntityVisualState> _entityVisualStates;
  QHash<QString, EntityHomePosition> _entityHomePositions;
  QHash<QString, double> _autoBombReleaseCooldownSeconds;
  QHash<QString, int> _autoBehaviorDamageReactionLevel;
  QHash<QString, double> _attackAirElapsedSeconds;
  QHash<QString, double> _attackAirMissileCooldownSeconds;
  QHash<QString, EntityPlan> _entityPlans;
  PendingBombRelease _pendingBombRelease;
  application::SimulationEngine* m_simulationEngine;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  QWebEngineView* _webView;
#endif
};
