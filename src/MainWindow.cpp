#include "MainWindow.h"

#include "AddEntityDialog.h"
#include "AssignTaskDialog.h"
#include "EntityDetailsDialog.h"
#include "application/ScenarioState.h"
#include "application/SimulationEngine.h"
#include "application/Command.h"
#include "application/Event.h"
#include "application/EventBus.h"
#include "application/KinematicsTelemetry.h"
#include "infrastructure/Ros2TelemetryPublisher.h"
#include "application/ScenarioQueries.h"
#include "application/TaskApplicator.h"
#include "domain/BombReleaseGate.h"
#include "presentation/BombReleaseController.h"
#include "presentation/DetectedContactsPresenter.h"
#include "presentation/EntityDefaultsResolver.h"
#include "presentation/EntityPlanExecutor.h"
#include "application/HlaCombatDemoScenario.h"
#include "presentation/BombTargetMapSync.h"
#include "presentation/MapBridgeScripts.h"
#include "presentation/KinematicsCockpitWidget.h"
#include "presentation/Ros2TelemetryDialog.h"
#include "presentation/EntityContextMenuBuilder.h"
#include "presentation/EntityContextMenuStateBuilder.h"
#include "presentation/TrackSetSyncer.h"
#include "presentation/EntityPlanDialog.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/EntityIdentity.h"
#include "domain/GeoMath.h"
#include "infrastructure/CesiumScenePage.h"
#include "infrastructure/MapBridge.h"
#include "infrastructure/ModelCatalog.h"
#include "infrastructure/interoperability/hla/RprFomEncoding.h"
#include "infrastructure/SensorModelProviderCatalog.h"
#include "infrastructure/SensorModelProviderBootstrap.h"
#include "presentation/EntityTextFormatter.h"
#include "presentation/EntityHomePositionTracker.h"
#include "presentation/EntityStatusFormatter.h"
#include "presentation/EntityVisualStateManager.h"
#include "presentation/PlanStepConfigurator.h"
#include "presentation/TrackIconProvider.h"
#include "presentation/TrackSummaryBuilder.h"
#include "ui_MainWindow.h"

#include <QAbstractItemView>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QColor>
#include <QDialog>
#include <QDockWidget>
#include <QEventLoop>
#include <QFrame>
#include <QHeaderView>
#include <QSettings>
#include <QHBoxLayout>
#include <QIcon>
#include <QListWidget>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QItemSelectionModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QInputDialog>
#include <QSizePolicy>
#include <QStandardItem>
#include <cmath>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QSet>
#include <QToolButton>
#include <QVariantList>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QTimer>

#include <algorithm>
#include <cmath>
#include <functional>
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

namespace {
constexpr int kTrackSummaryRole = Qt::UserRole + 1;
constexpr int kDetectedContactObserverRole = Qt::UserRole + 2;
constexpr int kDetectedContactTargetRole = Qt::UserRole + 3;
// kGraphicAltitudeOffsetMeters and kPolygonCloseDistanceMeters moved to presentation/GraphicPickCoordinator.cpp
constexpr int kTaskQuickBarMarginPixels = 14;
constexpr int kTaskQuickBarButtonPixels = 30;
constexpr int kTaskQuickBarIconPixels = 18;
constexpr double kDefaultTelemetryPeriodSeconds = 0.1;
// Bomb release constants now in domain/BombReleaseGate.h
// Attack timing constants now in application/AttackTaskProcessor.h

// planStatusDisplayLabel moved to domain/CombatRules.h

// attackTaskStatusIsTerminal, attackSurfaceCoordinatesAreUsable,
// autoBehaviorDamageReactionLevel, autoBehaviorCanEngageByDamage
// moved to domain/CombatRules.h


// activeMissileInFlightForTarget moved to application/ScenarioQueries.h


// autoBehaviorDamageReactionLevel, autoBehaviorCanEngageByDamage
// moved to domain/CombatRules.h

// behaviorModeOptions moved to presentation/EntityContextMenuStateBuilder.h

double telemetrySourcePeriod(
    const infrastructure::Ros2TelemetryConfiguration& configuration) {
  if ((!configuration.ros2Enabled && !configuration.csvEnabled) ||
      !std::isfinite(configuration.frequencyHertz) ||
      configuration.frequencyHertz <= 0.0) {
    return kDefaultTelemetryPeriodSeconds;
  }
  return std::min(
      kDefaultTelemetryPeriodSeconds,
      1.0 / configuration.frequencyHertz);
}

QString projectRootPath() {
#ifdef QTTEST_SOURCE_DIR
  return QString::fromUtf8(QTTEST_SOURCE_DIR);
#else
  return QDir::currentPath();
#endif
}

QString taskQuickBarIconPath(const QString& fileName) {
  return QDir(projectRootPath())
      .absoluteFilePath(QStringLiteral("Data/icons/task-quickbar/%1").arg(fileName));
}

#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
void clearQtTrackSelectionInMap(QWebEngineView* webView) {
  if (!webView) {
    return;
  }

  webView->page()->runJavaScript(
      QStringLiteral("window.clearQtTrackSelection && window.clearQtTrackSelection();"));
}
#endif

// makeTaskQuickFallbackIcon, loadTaskQuickBarIcon moved to presentation/TrackIconProvider.h

// makeTrackSummary, makeMunitionTrackSummary, makeTransientEffectTrackSummary,
// makePendingBombTargetTrackSummary, makePendingBombTargetLineTrackSummary
// moved to presentation/TrackSummaryBuilder.h

// forceIdentifierLabel, entityCanUseMissileActions moved to domain/CombatRules.h

// weaponQuantity moved to domain/BombReleaseGate.h

// MissileTargetCandidate, detectedMissileTargetsInRange, activeMissileInFlightForTarget,
// entityAltitudeMeters, validBombReleaseTargets, bestDetectedSurfaceBombTarget
// moved to application/ScenarioQueries.h

// bombTargetDisplayLabel moved to presentation/TrackSummaryBuilder.h

void setTrackData(QStandardItem* item, const QVariantMap& summary) {
  item->setData(summary, kTrackSummaryRole);
}

QString entityTreeLabel(const Entity& entity, const QVector<Entity>& entities) {
  int matchingNames = 0;
  for (const Entity& candidate : entities) {
    if (candidate.name.compare(entity.name, Qt::CaseInsensitive) == 0) {
      ++matchingNames;
    }
  }
  return matchingNames > 1
      ? QStringLiteral("%1 [%2]").arg(entity.name, domain::entityKey(entity).left(8))
      : entity.name;
}

// forceColorFromLabel, categoryGlyph, makeTacticalGraphicIcon, makeTrackIcon
// moved to presentation/TrackIconProvider.h

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _contentWidget(nullptr),
      _taskQuickBar(nullptr),
      _kinematicsCockpitDock(nullptr),
      _kinematicsCockpitWidget(nullptr),
      _qflightCockpitDock(nullptr),
      _qflightCockpitWidget(nullptr),
      _ecamCockpitDock(nullptr),
      _ecamCockpitWidget(nullptr),
      _ros2TelemetryPublisher(nullptr),
      _kinematicsTelemetrySubscriptionId(0),
      _mapBridge(new MapBridge(this)),
      _scenarioState(new ScenarioState()),
      _objectsModel(new QStandardItemModel(this)),
      _detectedContactsModel(new QStandardItemModel(this)),
      _friendlyRootItem(nullptr),
      _opposingRootItem(nullptr),
      _neutralRootItem(nullptr),
      _tacticalGraphicsRootItem(nullptr),
      _entityDialog(nullptr),
      _taskDialog(nullptr),
      _entityCoordinatePickPending(false),
      _taskCoordinatePickPending(false),
      _addWaypointAction(new QAction(QStringLiteral("Add Waypoint"), this)),
      _addRouteAction(new QAction(QStringLiteral("Add Route"), this)),
      _addAreaAction(new QAction(QStringLiteral("Add Area"), this)),
      _simulationTimer(new QTimer(this)),
      _applyingMapSelection(false),
      _simulationRunning(false),
      m_simulationEngine(new application::SimulationEngine(_scenarioState, this)),
      _bombReleaseController(std::make_unique<presentation::BombReleaseController>(
          _scenarioState,
          [this](const QString& msg) { this->appendLogMessage(msg); },
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          [this]() { this->syncScenarioStateToUi(); },
          [this](const QString& name, bool notify) { this->selectObjectByName(name, notify); },
          [this](const QString& track) { this->removeTrackFromMap(track); },
          this)),
      _attackTaskProcessor(std::make_unique<application::AttackTaskProcessor>(
          _scenarioState,
          _bombReleaseController.get(),
          [this](const QString& msg) { this->appendLogMessage(msg); },
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          [this](const QString& launcher, double lat, double lon, double alt,
                 const QString& label, const QString& source,
                 const QString& targetEntity, bool log, bool focus) {
            this->queuePendingBombRelease(launcher, lat, lon, alt, label, source, targetEntity, log, focus);
          },
          this)),
      _planExecutor(std::make_unique<presentation::EntityPlanExecutor>(
          _scenarioState,
          [this](const QString& name, const EntityTask& task, bool sync) {
            return this->applyEntityTask(name, task, sync);
          },
          [this](const QString& msg) { this->appendLogMessage(msg); },
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          this)),
      _cockpitControlService(std::make_unique<application::CockpitControlService>(
          _scenarioState,
          [this](const QString& name, const EntityTask& task, bool sync) {
            return this->applyEntityTask(name, task, sync);
          },
          [this](const QString& name, bool clearTask) {
            this->stopEntityPlan(name, clearTask);
          },
          [this](const QString& msg) { this->appendLogMessage(msg); })),
      _entityVisualStateManager(std::make_unique<presentation::EntityVisualStateManager>(
          QDir(projectRootPath()).absoluteFilePath(QStringLiteral("Data/entity_visual_state.json")))),
      _entityHomePositionTracker(std::make_unique<presentation::EntityHomePositionTracker>()),
      _graphicPickCoordinator(std::make_unique<presentation::GraphicPickCoordinator>(
          _scenarioState,
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          [this]() { this->syncScenarioStateToUi(); },
          [this](const QVariantMap& draft) { this->sendDraftGraphicToMap(draft); },
          [this](const QString& name) { this->clearDraftGraphicFromMap(name); },
          [this]() { this->beginGraphicCoordinatePick(); })),
      _planStepConfigurator(std::make_unique<presentation::PlanStepConfigurator>(
          [this](const QString& en, const EntityTask& it, const QString& itype, EntityTask& out) {
            return this->captureTaskConfiguration(en, it, itype, out);
          },
          [this](const QString& nameOrId) -> const AreaDefinition* {
            return this->findAreaByNameOrId(nameOrId);
          },
          [this](const QString& en) {
            return this->_entityHomePositionTracker->positionFor(en);
          },
          [this](const QString& title, const QString& label,
                 const QStringList& items, bool& ok) -> QString {
            return QInputDialog::getItem(this, title, label, items, 0, false, &ok);
          },
          [this](const QString& title, const QString& label,
                 double def, double mn, double mx, bool& ok) -> double {
            return QInputDialog::getDouble(this, title, label, def, mn, mx, 1, &ok);
          }))
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
      , _webView(nullptr)
#endif
{
  this->_ui->setupUi(this);
  const QStringList sensorProviderDiagnostics =
      infrastructure::SensorModelProviderBootstrap::registerEnabledProviders(
          *_scenarioState,
          infrastructure::SensorModelProviderCatalog::loadProviders());
  for (const QString& diagnostic : sensorProviderDiagnostics) {
    this->appendLogMessage(diagnostic);
  }
  this->_ui->viewerHost->setMinimumSize(960, 640);
  this->_ui->viewerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->initializeKinematicsCockpit();
  this->initializeRos2Telemetry();
  this->initializeModels();
  this->populateTaskCommands();
  this->_entityVisualStateManager->load();

  _weaponActionsController = std::make_unique<presentation::WeaponActionsController>(
      this->_scenarioState,
      [this]() { return this->selectedEntityName(); },
      [this](const QString& name) { return this->findEntityByName(name); },
      [this]() { return this->_simulationRunning; },
      [this](const QString& msg) { this->appendLogMessage(msg); },
      [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
      [this]() { this->syncScenarioStateToUi(); },
      [this](const QStringList& options) -> QString {
        bool ok = false;
        const QString result = QInputDialog::getItem(
            this,
            QStringLiteral("Launch Missile At"),
            QStringLiteral("Target"),
            options, 0, false, &ok);
        return ok ? result : QString{};
      },
      this);

  _entityStateActionsController = std::make_unique<presentation::EntityStateActionsController>(
      this->_scenarioState,
      this->_entityVisualStateManager.get(),
      [this]() { return this->selectedEntityName(); },
      [this]() { return this->selectedEntityIsDestroyed(); },
      [this]() { return this->currentSelectionIsOperableEntity(); },
      [this](const QString& name, const EntityTask& task, bool sync) {
        return this->applyEntityTask(name, task, sync);
      },
      [this](double& h, int& a, double& s) {
        return this->resolveSelectedEntityFlyTargets(h, a, s);
      },
      [this](const QString& title, const QString& label,
             double def, double mn, double mx, bool& ok) -> double {
        return QInputDialog::getDouble(this, title, label, def, mn, mx, 1, &ok);
      },
      [this]() {
        if (this->_taskDialog) {
          this->_taskDialog->close();
        }
      },
      [this](const QString& msg) { this->appendLogMessage(msg); },
      [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
      [this]() { this->syncScenarioStateToUi(); },
      this);

  _taskAssignmentController = std::make_unique<presentation::TaskAssignmentController>(
      [this]() { return this->selectedEntityName(); },
      [this]() { return this->currentSelectionIsOperableEntity(); },
      [this]() {
        return this->_ui->objectsTreeView->currentIndex()
            .data(kTrackSummaryRole).toMap();
      },
      [this](const QString& name) {
        return this->_entityHomePositionTracker->positionFor(name);
      },
      [this](bool requirePoints) {
        return application::availableRouteNames(this->_scenarioState, requirePoints);
      },
      [this](double& h, int& a, double& s) {
        return this->resolveSelectedEntityFlyTargets(h, a, s);
      },
      [this](const QString& name, const EntityTask& task) {
        return this->applyEntityTask(name, task);
      },
      [this](const QString& taskType) {
        this->openAssignTaskDialog(taskType);
      },
      [this](const QString& title, const QString& label,
             const QStringList& items, int current, bool& ok) -> QString {
        return QInputDialog::getItem(this, title, label, items, current, false, &ok);
      },
      [this](const QString& title, const QString& label,
             double def, double mn, double mx, int decimals, bool& ok) -> double {
        return QInputDialog::getDouble(this, title, label, def, mn, mx, decimals, &ok);
      },
      [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
      this);

  _tacticalGraphicsEditorController = std::make_unique<presentation::TacticalGraphicsEditorController>(
      this->_graphicPickCoordinator.get(),
      [this]() { return static_cast<int>(this->_scenarioState->waypoints().size()); },
      [this]() { return static_cast<int>(this->_scenarioState->routes().size()); },
      [this]() { return static_cast<int>(this->_scenarioState->areas().size()); },
      [this]() -> double {
        if (const Entity* entity = this->findEntityByName(this->selectedEntityName())) {
          if (entity->domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0) {
            return 0.0;
          }
          return static_cast<double>(entity->altitude);
        }
        const QVector<Waypoint>& waypoints = this->_scenarioState->waypoints();
        return waypoints.isEmpty() ? 0.0 : waypoints.last().altitudeMeters;
      },
      [this]() -> bool {
        if (const Entity* entity = this->findEntityByName(this->selectedEntityName())) {
          return entity->domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
        }
        return false;
      },
      [this](const QString& title, const QString& label,
             const QString& def, bool& ok) -> QString {
        return QInputDialog::getText(this, title, label, QLineEdit::Normal, def, &ok);
      },
      [this](const QString& title, const QString& label,
             const QStringList& items, int current, bool& ok) -> QString {
        return QInputDialog::getItem(this, title, label, items, current, false, &ok);
      },
      [this](const QString& title, const QString& label,
             double def, double mn, double mx, int decimals, bool& ok) -> double {
        return QInputDialog::getDouble(this, title, label, def, mn, mx, decimals, &ok);
      },
      this);

  _bombReleaseActionsController = std::make_unique<presentation::BombReleaseActionsController>(
      this->_bombReleaseController.get(),
      [this]() { return this->selectedEntityName(); },
      [this](const QString& name) -> const Entity* {
        return this->findEntityByName(name);
      },
      [this]() { return this->_simulationRunning; },
      [this]() { return this->_graphicPickCoordinator->isPending(); },
      [this]() { this->beginBombCoordinatePick(); },
      [this](const Entity& launcher) {
        return application::validBombReleaseTargets(this->_scenarioState, launcher);
      },
      [](const Entity& e) { return presentation::bombTargetDisplayLabel(e); },
      [this](const QString& title, const QString& label,
             const QStringList& items, bool& ok) -> QString {
        return QInputDialog::getItem(this, title, label, items, 0, false, &ok);
      },
      [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
      [this](const QString& msg) { this->appendLogMessage(msg); },
      [this]() { this->syncScenarioStateToUi(); },
      this);
  _scenarioObjectEditorController =
      std::make_unique<presentation::ScenarioObjectEditorController>(
          this->_scenarioState,
          [this]() { return this->selectedEntityName(); },
          [this]() { return this->selectedObjectName(); },
          [this]() { return this->currentSelectionIsEntity(); },
          [this]() { return this->selectedEntityIsDestroyed(); },
          [this]() {
            return this->_ui->objectsTreeView->currentIndex()
                .data(kTrackSummaryRole).toMap();
          },
          [this](const QString& name) {
            return this->cleanupRuntimeReferencesForRemovedEntity(name);
          },
          [this](const QString& name) { this->removeTrackFromMap(name); },
          [this](const QString& msg) { this->appendLogMessage(msg); },
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          [this]() { this->syncScenarioStateToUi(); },
          this);
  _simulationLifecycleController =
      std::make_unique<presentation::SimulationLifecycleController>(
          this->_scenarioState,
          [this]() { return this->_simulationRunning; },
          [this](bool v) { this->_simulationRunning = v; },
          [this]() { this->_simulationTimer->start(); },
          [this]() { this->_simulationTimer->stop(); },
          [this]() {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
            if (this->_webView) {
              this->_webView->page()->runJavaScript(
                  QStringLiteral("window.refreshQtTrackedEntity && window.refreshQtTrackedEntity();"));
            }
#endif
          },
          [this]() { this->_bombReleaseController->cancelPickMode(); },
          [this]() {
            this->_attackTaskProcessor->autoBombReleaseCooldownSeconds().clear();
          },
          [this]() { this->clearPendingBombRelease(); },
          [this]() { this->syncScenarioStateToUi(); },
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          [this](const QString& msg) { this->appendLogMessage(msg); },
          [this]() { this->updateSimulationControls(); },
          this);
  _assignTaskController = std::make_unique<presentation::AssignTaskController>(
      [this]() { return this->selectedEntityName(); },
      [this]() { return this->currentSelectionIsOperableEntity(); },
      [this](const QString& name) {
        const auto it = this->_planExecutor->plans().constFind(name);
        return it != this->_planExecutor->plans().constEnd() && it->running;
      },
      [this]() {
        return this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
      },
      [this](const QString& en, const EntityTask& init, const QString& type, EntityTask& out) {
        return this->captureTaskConfiguration(en, init, type, out);
      },
      [this](const QString& en, const EntityTask& task) {
        return this->applyEntityTask(en, task);
      },
      [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
      this);
  {
    QSet<QString> validNames;
    for (const Entity& entity : this->_scenarioState->entities()) {
      validNames.insert(domain::entityKey(entity));
    }
    this->_entityVisualStateManager->pruneTo(validNames);
  }
  for (const Entity& entity : this->_scenarioState->entities()) {
    this->appendEntityToUi(entity);
  }
  this->syncDetectedContactsToUi();
  _simulationTimer->setInterval(33);
  QObject::connect(_simulationTimer, &QTimer::timeout, this, [this]() {
    constexpr double deltaSeconds = 0.033;
    if (_hlaTimeManagementActive) {
      if (!_hlaTimeAdvancePending) {
        _hlaTimeAdvancePending = true;
        emit hlaTimeAdvanceRequested(
            _scenarioState->simulationTimeSeconds() + deltaSeconds);
      }
      return;
    }
    this->advanceSimulationTick(deltaSeconds);
  });

  QObject::connect(this->_ui->actionQuit, &QAction::triggered, this, &QWidget::close);
  QObject::connect(
      this->_ui->actionAddEntity,
      &QAction::triggered,
      this,
      &MainWindow::openAddEntityDialog);
  QObject::connect(_addWaypointAction, &QAction::triggered, this, &MainWindow::openAddWaypointDialog);
  QObject::connect(_addRouteAction, &QAction::triggered, this, &MainWindow::openAddRouteDialog);
  QObject::connect(_addAreaAction, &QAction::triggered, this, &MainWindow::openAddAreaDialog);
  this->_ui->menuInsert->addAction(_addWaypointAction);
  this->_ui->menuInsert->addAction(_addRouteAction);
  this->_ui->menuInsert->addAction(_addAreaAction);
  QObject::connect(
      this->_ui->actionToggleOverlays,
      &QAction::toggled,
      this,
      &MainWindow::toggleTacticalOverlays);
  QObject::connect(
      this->_ui->actionToggleEntitiesPanel,
      &QAction::triggered,
      this,
      [this]() {
        this->_ui->objectsDockWidget->setVisible(!this->_ui->objectsDockWidget->isVisible());
      });
  QObject::connect(
      this->_ui->actionToggleContactsPanel,
      &QAction::triggered,
      this,
      [this]() {
        this->_ui->contactsDockWidget->setVisible(!this->_ui->contactsDockWidget->isVisible());
      });
  QObject::connect(
      this->_ui->actionStartSimulation,
      &QAction::triggered,
      this,
      &MainWindow::startSimulation);
  QObject::connect(
      this->_ui->actionPauseSimulation,
      &QAction::triggered,
      this,
      &MainWindow::pauseSimulation);
  QObject::connect(
      this->_ui->actionStopSimulation,
      &QAction::triggered,
      this,
      &MainWindow::stopSimulation);
  QObject::connect(
      this->_ui->actionDelete,
      &QAction::triggered,
      this,
      &MainWindow::deleteSelectedEntity);
  QObject::connect(
      this->_mapBridge,
      &MapBridge::pickedCoordinate,
      this,
      &MainWindow::reportPickedCoordinate);
  QObject::connect(
      this->_mapBridge,
      &MapBridge::mapStatus,
      this,
      &MainWindow::reportMapStatus);
  QObject::connect(
      this->_mapBridge,
      &MapBridge::selectedTrack,
      this,
      &MainWindow::handleMapTrackSelection);
  QObject::connect(
      this->_mapBridge,
      &MapBridge::entityContextMenuRequested,
      this,
      &MainWindow::openMapEntityContextMenu);

  QObject::connect(
      this->_ui->objectsTreeView,
      &QTreeView::doubleClicked,
      this,
      [this](const QModelIndex&) { this->openSelectedEntityDetails(); });
  this->_ui->objectsTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(
      this->_ui->objectsTreeView,
      &QWidget::customContextMenuRequested,
      this,
      &MainWindow::openObjectsContextMenu);
  QObject::connect(
      this->_ui->tasksListWidget,
      &QListWidget::itemDoubleClicked,
      this,
      [this](QListWidgetItem* item) {
        if (!item) {
          return;
        }
        const QString taskType = item->data(Qt::UserRole).toString();
        if (taskType == QStringLiteral("FlyHeadingAltitudeSpeed")) {
          this->assignFlyHeadingAltitudeSpeedTask();
        } else if (taskType == QStringLiteral("MoveToLocation")) {
          this->assignMoveToLocationTask();
        } else if (taskType == QStringLiteral("WaitOnLocation")) {
          this->assignWaitOnLocationTask();
        } else if (taskType == QStringLiteral("MoveToWaypoint")) {
          this->assignMoveToWaypointTask();
        } else if (taskType == QStringLiteral("FollowRoute") ||
                   taskType == QStringLiteral("MoveAlongRoute")) {
          this->assignMoveAlongRouteTask();
        } else if (taskType == QStringLiteral("PatrolArea")) {
          this->assignPatrolAreaTask();
        } else if (taskType == QStringLiteral("OrbitArea")) {
          this->assignOrbitAreaTask();
        } else if (taskType == QStringLiteral("HoldRacetrack")) {
          this->assignHoldRacetrackTask();
        } else if (taskType == QStringLiteral("FollowEntity")) {
          this->assignFollowEntityTask();
        } else if (taskType == QStringLiteral("InterceptEntity") ||
                   taskType == QStringLiteral("InterceptEntity2D") ||
                   taskType == QStringLiteral("InterceptEntity3D")) {
          this->assignInterceptEntityTask();
        } else if (taskType == QStringLiteral("AttackOnce")) {
          this->assignAttackOnceTask();
        } else if (taskType == QStringLiteral("AttackUntilDestroyed")) {
          this->assignAttackUntilDestroyedTask();
        } else if (taskType == QStringLiteral("FireOnPosition")) {
          this->assignFireOnPositionTask();
        } else if (taskType == QStringLiteral("FireInDirection")) {
          this->assignFireInDirectionTask();
        } else if (taskType == QStringLiteral("StopWeaponsTask")) {
          this->assignStopWeaponsTask();
        } else if (taskType == QStringLiteral("AttackAir")) {
          this->assignAttackAirTask();
        } else if (taskType == QStringLiteral("AttackSurface")) {
          this->assignAttackSurfaceTask();
        } else if (taskType == QStringLiteral("WaitUntilTargetDetected")) {
          this->assignWaitUntilTargetDetectedTask();
        } else if (taskType == QStringLiteral("WaitUntilTargetDestroyed")) {
          this->assignWaitUntilTargetDestroyedTask();
        } else if (taskType == QStringLiteral("WaitUntilDamaged")) {
          this->assignWaitUntilDamagedTask();
        } else if (taskType == QStringLiteral("WaitUntilTime")) {
          this->assignWaitUntilTimeTask();
        } else if (taskType == QStringLiteral("WaitUntilInRange")) {
          this->assignWaitUntilInRangeTask();
        } else if (taskType == QStringLiteral("ClearTask")) {
          this->clearSelectedTask();
        }
      });

  const QString accessToken = CesiumScenePage::defaultAccessToken();
  const QString assetId = []() {
    const QString configured =
        CesiumScenePage::readConfigValue(QStringLiteral("ion_asset_id"));
    return configured.isEmpty() ? CesiumScenePage::defaultAssetId()
                                : configured;
  }();

  this->_ui->statusLabel->setText(
      QStringLiteral("Preparando CesiumJS local. Asset configurado: %1.")
          .arg(assetId));

#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  const QUrl localCesiumBaseUrl = CesiumScenePage::cesiumSourceBaseUrl();
  const QString cesiumScriptPath =
      localCesiumBaseUrl.toLocalFile() + QStringLiteral("Cesium/Cesium.js");

  if (!QFileInfo::exists(cesiumScriptPath)) {
    auto* fallback = new QLabel(
        QStringLiteral(
            "No se ha encontrado la copia local de CesiumJS.\n\n"
            "Se esperaba en vendor/cesiumjs/Cesium/Cesium.js."),
        this->_ui->viewerHost);
    fallback->setWordWrap(true);
    fallback->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->_contentWidget = fallback;
    this->_ui->statusLabel->setText(
        QStringLiteral("Falta la libreria local de CesiumJS en vendor/cesiumjs."));
  } else {
    auto* webView = new QWebEngineView(this->_ui->viewerHost);
    this->_webView = webView;
    webView->settings()->setAttribute(
        QWebEngineSettings::LocalContentCanAccessRemoteUrls,
        true);
    webView->settings()->setAttribute(
        QWebEngineSettings::LocalContentCanAccessFileUrls,
        true);
    webView->settings()->setAttribute(
        QWebEngineSettings::JavascriptEnabled,
        true);

    auto* channel = new QWebChannel(webView->page());
    channel->registerObject(QStringLiteral("qtBridge"), this->_mapBridge);
    webView->page()->setWebChannel(channel);

    QObject::connect(
        webView,
        &QWebEngineView::loadStarted,
        this,
        [this]() {
          this->_ui->statusLabel->setText(
              QStringLiteral("Cargando la pagina local de CesiumJS..."));
        });
    QObject::connect(
        webView,
        &QWebEngineView::loadFinished,
        this,
        [this](bool ok) {
          this->_ui->statusLabel->setText(
              ok
                  ? QStringLiteral(
                        "Pagina local de CesiumJS cargada. Ya puedes seleccionar tracks en el mapa.")
                  : QStringLiteral(
                        "Qt WebEngine no pudo terminar de cargar la pagina local de CesiumJS."));
          if (ok) {
            this->syncTracksToMap();
            this->toggleTacticalOverlays(this->_ui->actionToggleOverlays->isChecked());
            const auto firstEntitySummary = [this]() -> QVariantMap {
              const QList<QStandardItem*> roots = {
                  this->_friendlyRootItem,
                  this->_opposingRootItem,
                  this->_neutralRootItem,
              };
              for (QStandardItem* root : roots) {
                if (!root) {
                  continue;
                }
                for (int row = 0; row < root->rowCount(); ++row) {
                  QStandardItem* categoryItem = root->child(row);
                  if (!categoryItem) {
                    continue;
                  }
                  for (int childRow = 0; childRow < categoryItem->rowCount(); ++childRow) {
                    QStandardItem* entityItem = categoryItem->child(childRow);
                    if (!entityItem) {
                      continue;
                    }
                    const QVariantMap summary =
                        entityItem->data(kTrackSummaryRole).toMap();
                    if (!summary.isEmpty()) {
                      return summary;
                    }
                  }
                }
              }
              return {};
            }();
            if (!firstEntitySummary.isEmpty()) {
              this->sendTrackToMap(firstEntitySummary, true);
            }
          }
        });

    const QUrl localPageUrl = CesiumScenePage::writeLocalHtmlPage(accessToken);
    if (!localPageUrl.isValid()) {
      auto* fallback = new QLabel(
          QStringLiteral("No se pudo generar la pagina local temporal para CesiumJS."),
          this->_ui->viewerHost);
      fallback->setWordWrap(true);
      fallback->setAlignment(Qt::AlignTop | Qt::AlignLeft);
      this->_contentWidget = fallback;
      this->_ui->statusLabel->setText(
          QStringLiteral("Fallo al preparar la pagina local de CesiumJS."));
      delete webView;
      this->_webView = nullptr;
    } else {
      webView->load(localPageUrl);
      this->_contentWidget = webView;
    }
  }
#else
  auto* fallback = new QLabel(
      QStringLiteral(
          "CesiumJS no puede activarse todavia porque tu instalacion de Qt no "
          "tiene completa la cadena de WebEngine."),
      this->_ui->viewerHost);
  fallback->setWordWrap(true);
  fallback->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  this->_contentWidget = fallback;
  this->_ui->statusLabel->setText(
      QStringLiteral("Qt WebEngine no esta disponible en este build."));
#endif

  auto* hostLayout = new QVBoxLayout(this->_ui->viewerHost);
  hostLayout->setContentsMargins(0, 0, 0, 0);
  hostLayout->addWidget(this->_contentWidget);
  this->_ui->viewerHost->installEventFilter(this);
  this->createTaskQuickBar();
  this->updateTaskQuickBarState();
  this->positionTaskQuickBar();
  this->updateSimulationControls();
}

MainWindow::~MainWindow() {
  if (this->_kinematicsTelemetrySubscriptionId != 0) {
    application::EventBus::instance()
        .unsubscribe<application::EventKinematicsTelemetryUpdated>(
            this->_kinematicsTelemetrySubscriptionId);
  }
  delete this->_ros2TelemetryPublisher;
  this->_ros2TelemetryPublisher = nullptr;
  delete this->_scenarioState;
  delete this->_ui;
}

QVector<Entity> MainWindow::entitySnapshot() const {
  const auto lock = _scenarioState->lock();
  return _scenarioState->entities();
}

void MainWindow::startHlaCombatDemo() {
  const application::HlaCombatDemoScenario demo =
      application::makeHlaCombatDemoScenario();

  this->_scenarioState->reset();
  this->_planExecutor->plans().clear();
  this->_scenarioState->addEntity(demo.friendly);
  this->_scenarioState->addEntity(demo.opposing);

  if (!this->applyEntityTask(
          demo.opposing.entityId,
          demo.opposingMovementTask,
          /*syncUi=*/false)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo iniciar el movimiento del rival HLA."));
    return;
  }

  EntityPlan& plan = this->ensureEntityPlan(demo.friendly.entityId);
  PlanStep followStep;
  followStep.kind = PlanStepKind::FollowEntity;
  followStep.task = demo.friendlyPlanTasks.at(0);
  followStep.label = QStringLiteral("Follow %1").arg(demo.opposing.name);
  PlanStep attackStep;
  attackStep.kind = PlanStepKind::AttackUntilDestroyed;
  attackStep.task = demo.friendlyPlanTasks.at(1);
  attackStep.label = QStringLiteral("Attack %1 Until Destroyed")
      .arg(demo.opposing.name);
  plan.steps = {followStep, attackStep};

  this->syncScenarioStateToUi();
  if (!this->startEntityPlan(demo.friendly.entityId)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo iniciar el plan de combate HLA."));
    return;
  }
  this->startSimulation();
  this->_ui->statusLabel->setText(
      QStringLiteral("Demo HLA: %1 sigue y ataca a %2 hasta destruirlo.")
          .arg(demo.friendly.name, demo.opposing.name));
  this->appendLogMessage(QStringLiteral(
      "HLA combat demo started: Follow Entity -> Attack Until Destroyed."));
}

QVector<ActiveMunition> MainWindow::activeMunitionSnapshot() const {
  const auto lock = _scenarioState->lock();
  return _scenarioState->activeMunitions();
}

QVector<TransientEffect> MainWindow::transientEffectSnapshot() const {
  const auto lock = _scenarioState->lock();
  return _scenarioState->transientEffects();
}

void MainWindow::applyHlaRemoteEntityChanges(
    const std::vector<tactical::hla::RemoteEntityChange>& changes) {
  if (changes.empty()) return;
  for (const tactical::hla::RemoteEntityChange& change : changes) {
    if (change.removed) {
      _scenarioState->removeExternalEntity(
          QString::fromStdString(change.state.stableId));
      continue;
    }
    const tactical::hla::RprEntityState& state = change.state;
    Entity entity;
    entity.entityId = QString::fromStdString(state.stableId);
    entity.name = QString::fromStdString(state.name);
    entity.domain = QString::fromStdString(state.domain);
    entity.type = entity.domain == QStringLiteral("Air")
        ? QStringLiteral("Aircraft")
        : entity.domain;
    entity.category = QStringLiteral("HLA Remote");
    entity.entityKind = state.entityKind;
    entity.entityDomain = state.entityDomain;
    entity.entityCountry = state.countryCode;
    entity.entityCategory = state.category;
    entity.entitySubcategory = state.subcategory;
    entity.entitySpecific = state.specific;
    entity.entityExtra = state.extra;
    entity.refreshEntityTypeCode();
    entity.forceIdentifier = state.forceIdentifier;
    entity.latitude = state.latitudeDegrees;
    entity.longitude = state.longitudeDegrees;
    entity.altitude = qMax(0, qRound(state.altitudeMeters));
    entity.headingDegrees = state.headingDegrees;
    entity.pitchDegrees = state.pitchDegrees;
    entity.rollDegrees = state.rollDegrees;
    entity.speedKnots = state.speedKnots;
    entity.verticalSpeedMetersPerSecond =
        state.verticalSpeedMetersPerSecond;
    entity.damagePercent = state.damagePercent;
    entity.destroyed = state.destroyed;
    entity.externallyControlled = true;
    _scenarioState->upsertExternalEntity(entity);
  }
  this->syncScenarioStateToUi();
}

void MainWindow::applyHlaRemoteMunitionChanges(
    const std::vector<tactical::hla::RemoteMunitionChange>& changes) {
  for (const tactical::hla::RemoteMunitionChange& change : changes) {
    const QString trackId = QString::fromStdString(change.state.stableId);
    if (change.removed) {
      this->removeTrackFromMap(trackId);
      continue;
    }
    ActiveMunition munition;
    munition.id = trackId;
    munition.munitionType = change.state.category == 2
        ? QStringLiteral("Bomb")
        : QStringLiteral("Missile");
    munition.status = QStringLiteral("HLA Remote");
    munition.latitude = change.state.latitudeDegrees;
    munition.longitude = change.state.longitudeDegrees;
    munition.altitudeMeters = change.state.altitudeMeters;
    munition.headingDegrees = change.state.headingDegrees;
    munition.pitchDegrees = change.state.pitchDegrees;
    munition.rollDegrees = change.state.rollDegrees;
    munition.speedMetersPerSecond = change.state.speedKnots * 0.514444;
    this->sendTrackToMap(
        presentation::makeMunitionTrackSummary(munition), false);
  }
}

void MainWindow::applyHlaRemoteSensorChanges(
    const std::vector<tactical::hla::RemoteSensorChange>& changes) {
  if (changes.empty()) return;
  for (const tactical::hla::RemoteSensorChange& change : changes) {
    const QString entityId = QString::fromStdString(change.hostEntityId);
    const QString sensorId = QString::fromStdString(change.sensorId);
    if (change.removed) {
      _scenarioState->removeExternalSensor(entityId, sensorId);
      continue;
    }
    SensorDefinition sensor;
    sensor.id = sensorId;
    sensor.name = QStringLiteral("HLA Radar");
    sensor.modelProviderId = QStringLiteral("hla-rpr");
    sensor.sensorType = QStringLiteral("radar");
    sensor.sensorSubType = QStringLiteral("airborne-radar");
    sensor.enabled = true;
    sensor.emitting = change.emitting;
    sensor.azimuthCenterDegrees = change.azimuthCenterDegrees;
    sensor.azimuthWidthDegrees = change.azimuthWidthDegrees;
    sensor.elevationCenterDegrees = change.elevationCenterDegrees;
    sensor.elevationWidthDegrees = change.elevationWidthDegrees;
    sensor.radarProfile.frequencyHertz = change.frequencyHertz;
    sensor.radarProfile.bandwidthHertz = change.bandwidthHertz;
    sensor.radarProfile.peakPowerWatts =
        change.effectiveRadiatedPowerDbm > 0.0
            ? std::pow(10.0, change.effectiveRadiatedPowerDbm / 10.0) / 1000.0
            : 0.0;
    _scenarioState->upsertExternalSensor(entityId, sensor);

    const QVector<Entity> entities = this->entitySnapshot();
    const auto host = std::find_if(
        entities.cbegin(), entities.cend(), [&entityId](const Entity& entity) {
          return entity.entityId.compare(entityId, Qt::CaseInsensitive) == 0;
        });
    SensorContacts contacts;
    if (host != entities.cend()) {
      for (const std::string& trackedName :
           change.trackedObjectInstanceNames) {
        const auto target = std::find_if(
            entities.cbegin(), entities.cend(),
            [&trackedName](const Entity& entity) {
              if (entity.externallyControlled) {
                return entity.entityId ==
                    QStringLiteral("hla:%1")
                        .arg(QString::fromStdString(trackedName));
              }
              return tactical::hla::RprFomEncoding::objectInstanceName(
                         domain::entityKey(entity).toStdString()) == trackedName;
            });
        if (target == entities.cend()) continue;
        SensorContact contact;
        contact.sensorId = sensorId;
        contact.sensorModelProviderId = QStringLiteral("hla-rpr");
        contact.sensorType = QStringLiteral("radar");
        contact.sensorSubType = QStringLiteral("airborne-radar");
        contact.targetEntityId = domain::entityKey(*target);
        contact.targetEntityName = target->name;
        contact.rangeMeters = domain::distanceMeters(
            host->latitude, host->longitude,
            target->latitude, target->longitude);
        contact.bearingDegrees = domain::bearingDegrees(
            host->latitude, host->longitude,
            target->latitude, target->longitude);
        contact.detected = true;
        contact.confidence = 1.0;
        contact.lastSeenSimulationSeconds =
            _scenarioState->simulationTimeSeconds();
        contact.trackState = QStringLiteral("Tracked");
        contacts.push_back(std::move(contact));
      }
    }
    _scenarioState->replaceExternalSensorContacts(
        entityId, sensorId, contacts);
  }
  this->syncScenarioStateToUi();
}

void MainWindow::applyHlaRemoteWarfareEvents(
    const std::vector<tactical::hla::RemoteWarfareEvent>& events) {
  if (events.empty()) return;
  for (const tactical::hla::RemoteWarfareEvent& event : events) {
    TransientEffect effect;
    effect.id = QStringLiteral("hla-event-%1")
                    .arg(QString::fromStdString(event.eventId));
    effect.effectType =
        event.kind == tactical::hla::RemoteWarfareEventKind::WeaponFire
            ? QStringLiteral("LaunchFlash")
            : QStringLiteral("ImpactFlash");
    effect.latitude = event.latitudeDegrees;
    effect.longitude = event.longitudeDegrees;
    effect.altitudeMeters = event.altitudeMeters;
    effect.ttlSeconds =
        event.kind == tactical::hla::RemoteWarfareEventKind::WeaponFire
            ? 0.5
            : 0.8;
    _scenarioState->appendExternalEffect(effect);
    QString correlation;
    if (!event.firingObjectInstanceName.empty()) {
      correlation += QStringLiteral(" firing=%1").arg(
          QString::fromStdString(event.firingObjectInstanceName));
    }
    if (!event.munitionObjectInstanceName.empty()) {
      correlation += QStringLiteral(" munition=%1").arg(
          QString::fromStdString(event.munitionObjectInstanceName));
    }
    if (!event.targetObjectInstanceName.empty()) {
      correlation += QStringLiteral(" target=%1").arg(
          QString::fromStdString(event.targetObjectInstanceName));
    }
    this->appendLogMessage(
        QStringLiteral("HLA %1: %2 at %3, %4, %5 m%6")
            .arg(
                event.kind == tactical::hla::RemoteWarfareEventKind::WeaponFire
                    ? QStringLiteral("WeaponFire")
                    : QStringLiteral("MunitionDetonation"),
                QString::fromStdString(event.munitionType))
            .arg(event.latitudeDegrees, 0, 'f', 5)
            .arg(event.longitudeDegrees, 0, 'f', 5)
            .arg(event.altitudeMeters, 0, 'f', 0)
            .arg(correlation));
  }
  this->syncScenarioStateToUi();
}

void MainWindow::applyHlaRemoteSimulationControl(
    tactical::hla::RemoteSimulationControl control) {
  _applyingHlaSimulationControl = true;
  switch (control) {
    case tactical::hla::RemoteSimulationControl::StartResume:
      this->startSimulation();
      break;
    case tactical::hla::RemoteSimulationControl::Pause:
      this->pauseSimulation();
      break;
    case tactical::hla::RemoteSimulationControl::Stop:
      this->stopSimulation();
      break;
  }
  _applyingHlaSimulationControl = false;
}

void MainWindow::reportHlaSynchronizationStatus(const QString& message) {
  this->appendLogMessage(QStringLiteral("HLA synchronization: %1").arg(message));
}

void MainWindow::setHlaTimeManagementActive(bool active) {
  _hlaTimeManagementActive = active;
  _hlaTimeAdvancePending = false;
  this->appendLogMessage(
      active
          ? QStringLiteral("HLA time management active: simulation ticks require RTI grants.")
          : QStringLiteral("HLA time management disabled: using local simulation clock."));
}

void MainWindow::applyHlaTimeAdvanceGrant(double logicalTimeSeconds) {
  _hlaTimeAdvancePending = false;
  const double currentTime = _scenarioState->simulationTimeSeconds();
  if (logicalTimeSeconds <= currentTime) return;
  this->advanceSimulationTick(logicalTimeSeconds - currentTime);
}

void MainWindow::initializeKinematicsCockpit() {
  this->_kinematicsCockpitDock = new QDockWidget(
      QStringLiteral("Modern PFD"),
      this);
  this->_kinematicsCockpitDock->setObjectName(
      QStringLiteral("kinematicsCockpitDockWidget"));
  this->_kinematicsCockpitDock->setAllowedAreas(
      Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  this->_kinematicsCockpitWidget =
      new presentation::KinematicsCockpitWidget(
          this->_kinematicsCockpitDock,
          presentation::KinematicsCockpitWidget::PanelMode::ModernPfd);
  this->_kinematicsCockpitDock->setWidget(this->_kinematicsCockpitWidget);
  connect(
      this->_kinematicsCockpitWidget,
      &presentation::KinematicsCockpitWidget::takeControlRequested,
      this,
      &MainWindow::takeCockpitControl);
  connect(
      this->_kinematicsCockpitWidget,
      &presentation::KinematicsCockpitWidget::setpointsRequested,
      this,
      &MainWindow::updateCockpitSetpoints);
  connect(
      this->_kinematicsCockpitWidget,
      &presentation::KinematicsCockpitWidget::releaseControlRequested,
      this,
      &MainWindow::releaseCockpitControl);
  this->_kinematicsCockpitDock->setFeatures(
      QDockWidget::DockWidgetClosable |
      QDockWidget::DockWidgetMovable |
      QDockWidget::DockWidgetFloatable);
  this->addDockWidget(Qt::BottomDockWidgetArea, this->_kinematicsCockpitDock);
  this->_ui->menuView->addAction(
      this->_kinematicsCockpitDock->toggleViewAction());

  this->_qflightCockpitDock = new QDockWidget(
      QStringLiteral("QFlight EADI"),
      this);
  this->_qflightCockpitDock->setObjectName(
      QStringLiteral("qflightCockpitDockWidget"));
  this->_qflightCockpitWidget =
      new presentation::KinematicsCockpitWidget(
          this->_qflightCockpitDock,
          presentation::KinematicsCockpitWidget::PanelMode::QFlightEadi);
  this->_qflightCockpitDock->setWidget(this->_qflightCockpitWidget);
  this->_qflightCockpitDock->setFeatures(
      QDockWidget::DockWidgetClosable |
      QDockWidget::DockWidgetMovable |
      QDockWidget::DockWidgetFloatable);
  this->addDockWidget(Qt::BottomDockWidgetArea, this->_qflightCockpitDock);
  this->_ui->menuView->addAction(
      this->_qflightCockpitDock->toggleViewAction());

  this->_ecamCockpitDock = new QDockWidget(
      QStringLiteral("ECAM Engine Display"),
      this);
  this->_ecamCockpitDock->setObjectName(
      QStringLiteral("ecamCockpitDockWidget"));
  this->_ecamCockpitWidget =
      new presentation::KinematicsCockpitWidget(
          this->_ecamCockpitDock,
          presentation::KinematicsCockpitWidget::PanelMode::EcamEngine);
  this->_ecamCockpitDock->setWidget(this->_ecamCockpitWidget);
  this->_ecamCockpitDock->setFeatures(
      QDockWidget::DockWidgetClosable |
      QDockWidget::DockWidgetMovable |
      QDockWidget::DockWidgetFloatable);
  this->addDockWidget(Qt::BottomDockWidgetArea, this->_ecamCockpitDock);
  this->_ui->menuView->addAction(
      this->_ecamCockpitDock->toggleViewAction());
  this->tabifyDockWidget(
      this->_kinematicsCockpitDock,
      this->_qflightCockpitDock);
  this->tabifyDockWidget(
      this->_qflightCockpitDock,
      this->_ecamCockpitDock);
  this->_kinematicsCockpitDock->raise();

  this->_kinematicsTelemetrySubscriptionId =
      application::EventBus::instance()
          .subscribe<application::EventKinematicsTelemetryUpdated>(
              [this](const application::EventKinematicsTelemetryUpdated& event) {
                const application::KinematicsTelemetrySnapshot snapshot =
                    event.snapshot;
                QMetaObject::invokeMethod(
                    this,
                    [this, snapshot]() {
                      const QString entityReference = snapshot.entityId.trimmed().isEmpty()
                          ? snapshot.entityName
                          : snapshot.entityId;
                      if (entityReference == this->selectedEntityName()) {
                        EntityTask activeTask;
                        activeTask.enabled = snapshot.taskEnabled;
                        activeTask.taskType = snapshot.taskType;
                        this->_cockpitControlService->reconcile(
                            entityReference,
                            activeTask);
                        this->_kinematicsCockpitWidget->setControlActive(
                            this->_cockpitControlService->hasControl(
                                entityReference));
                        this->_kinematicsCockpitWidget->applySnapshot(snapshot);
                        this->_qflightCockpitWidget->applySnapshot(snapshot);
                        this->_ecamCockpitWidget->applySnapshot(snapshot);
                      }
                    },
                    Qt::QueuedConnection);
              });
}

void MainWindow::initializeRos2Telemetry() {
  this->_ros2TelemetryPublisher =
      new infrastructure::Ros2TelemetryPublisher(this);

  QSettings settings(QStringLiteral("qttest"), QStringLiteral("qttest"));
  settings.beginGroup(QStringLiteral("Ros2Telemetry"));
  infrastructure::Ros2TelemetryConfiguration configuration;
  configuration.ros2Enabled = settings.value(
      QStringLiteral("ros2Enabled"), false).toBool();
  configuration.topicPrefix = settings.value(
      QStringLiteral("topicPrefix"),
      QStringLiteral("/qttest/entities")).toString();
  configuration.entityReference = settings.value(
      QStringLiteral("entityReference")).toString();
  configuration.frequencyHertz = settings.value(
      QStringLiteral("frequencyHertz"), 10.0).toDouble();
  configuration.csvEnabled = settings.value(
      QStringLiteral("csvEnabled"), false).toBool();
  configuration.csvPath = settings.value(
      QStringLiteral("csvPath")).toString();
  settings.endGroup();

  this->_ros2TelemetryPublisher->applyConfiguration(configuration);
  this->_scenarioState->setKinematicsTelemetryPublicationPeriod(
      telemetrySourcePeriod(configuration));
  connect(
      this->_ros2TelemetryPublisher,
      &infrastructure::Ros2TelemetryPublisher::publicationStateChanged,
      this,
      [this](const QString& state) {
        if (this->_ros2TelemetryPublisher->configuration().ros2Enabled) {
          this->_ui->statusLabel->setText(
              QStringLiteral("ROS 2 telemetry: %1").arg(state));
        }
      });

  QAction* action = this->_ui->menuView->addAction(
      QStringLiteral("ROS 2 Telemetry..."));
  connect(
      action,
      &QAction::triggered,
      this,
      &MainWindow::configureRos2Telemetry);
}

void MainWindow::configureRos2Telemetry() {
  QVector<Entity> entities;
  {
    auto lock = this->_scenarioState->lock();
    entities = this->_scenarioState->entities();
  }
  presentation::Ros2TelemetryDialog dialog(
      this->_ros2TelemetryPublisher->configuration(),
      entities,
      this->_ros2TelemetryPublisher->ros2Available(),
      this->_ros2TelemetryPublisher->availabilityMessage(),
      this);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  const infrastructure::Ros2TelemetryConfiguration configuration =
      dialog.configuration();
  this->_ros2TelemetryPublisher->applyConfiguration(configuration);
  this->_scenarioState->setKinematicsTelemetryPublicationPeriod(
      telemetrySourcePeriod(configuration));

  QSettings settings(QStringLiteral("qttest"), QStringLiteral("qttest"));
  settings.beginGroup(QStringLiteral("Ros2Telemetry"));
  settings.setValue(
      QStringLiteral("ros2Enabled"),
      configuration.ros2Enabled);
  settings.setValue(
      QStringLiteral("topicPrefix"),
      configuration.topicPrefix);
  settings.setValue(
      QStringLiteral("entityReference"),
      configuration.entityReference);
  settings.setValue(
      QStringLiteral("frequencyHertz"),
      configuration.frequencyHertz);
  settings.setValue(QStringLiteral("csvEnabled"), configuration.csvEnabled);
  settings.setValue(QStringLiteral("csvPath"), configuration.csvPath);
  settings.endGroup();

  this->_ui->statusLabel->setText(
      configuration.ros2Enabled
      ? QStringLiteral("ROS 2 telemetry enabled.")
      : QStringLiteral("ROS 2 telemetry disabled."));
}

void MainWindow::refreshKinematicsCockpitForEntity(const Entity* entity) {
  if (!entity) {
    this->_kinematicsCockpitWidget->clear();
    this->_qflightCockpitWidget->clear();
    this->_ecamCockpitWidget->clear();
    return;
  }

  const application::KinematicsTelemetrySnapshot snapshot =
      application::makeKinematicsTelemetrySnapshot(
          *entity,
          this->_scenarioState->simulationTimeSeconds(),
          0.0);
  const QString entityReference = domain::entityKey(*entity);
  this->_cockpitControlService->reconcile(entityReference, entity->currentTask);
  this->_kinematicsCockpitWidget->setControlActive(
      this->_cockpitControlService->hasControl(entityReference));
  this->_kinematicsCockpitWidget->applySnapshot(snapshot);
  this->_qflightCockpitWidget->applySnapshot(snapshot);
  this->_ecamCockpitWidget->applySnapshot(snapshot);
}

void MainWindow::takeCockpitControl(
    const QString& entityName,
    double headingDegrees,
    int altitudeMeters,
    double speedKnots) {
  const application::FlightControlCommand command{
      entityName, headingDegrees, altitudeMeters, speedKnots};
  const bool acquired = this->_cockpitControlService->takeControl(command);
  this->_kinematicsCockpitWidget->setControlActive(acquired);
  this->_ui->statusLabel->setText(
      acquired
          ? QStringLiteral("Control de cockpit activo para %1.").arg(entityName)
          : QStringLiteral("No se pudo tomar control de %1.").arg(entityName));
}

void MainWindow::updateCockpitSetpoints(
    const QString& entityName,
    double headingDegrees,
    int altitudeMeters,
    double speedKnots) {
  this->_cockpitControlService->updateSetpoints(
      {entityName, headingDegrees, altitudeMeters, speedKnots});
}

void MainWindow::releaseCockpitControl(const QString& entityName) {
  const bool released = this->_cockpitControlService->releaseControl(entityName);
  this->_kinematicsCockpitWidget->setControlActive(false);
  if (released) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Control de cockpit liberado para %1.").arg(entityName));
    this->syncScenarioStateToUi();
  }
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
  if (watched == this->_ui->viewerHost &&
      (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
    this->positionTaskQuickBar();
  }
  return QMainWindow::eventFilter(watched, event);
}

void MainWindow::appendLogMessage(const QString& message) {
  if (message.trimmed().isEmpty()) {
    return;
  }

  this->_ui->eventLogPlainTextEdit->appendPlainText(message);
}

void MainWindow::setSelectedTrackDetails(const QVariantMap& summary) {
  if (summary.isEmpty() && this->_kinematicsCockpitWidget) {
    this->refreshKinematicsCockpitForEntity(nullptr);
  }

  const auto value = [&summary](const char* key, const QString& fallback = QStringLiteral("-")) {
    const QString text = summary.value(QString::fromLatin1(key)).toString().trimmed();
    return text.isEmpty() ? fallback : text;
  };

  const QString name = value("name", QStringLiteral("No selection"));
  const QString type = value("type");
  const QString team = value("team");
  const QString altitude = value("altitude");
  const QString position = value("position");
  const QString status = value("status");

  this->_ui->selectedTrackNameValueLabel->setText(name);
  this->_ui->selectedTrackTypeValueLabel->setText(type);
  this->_ui->selectedTrackTeamValueLabel->setText(team);
  this->_ui->selectedTrackAltitudeValueLabel->setText(altitude);
  this->_ui->selectedTrackPositionValueLabel->setText(position);

  this->_ui->selectionNameValueLabel->setText(name);
  this->_ui->selectionTypeValueLabel->setText(type);
  const QString taskType = value("taskType", QStringLiteral("No current tasks"));
  const QString taskStatus = value("taskStatus", QStringLiteral("-"));
  const auto displayTaskType = [](const QString& rawTaskType) {
    return rawTaskType == QStringLiteral("InterceptEntity") ||
           rawTaskType == QStringLiteral("InterceptEntity2D") ||
           rawTaskType == QStringLiteral("InterceptEntity3D")
        ? QStringLiteral("Intercept Entity")
        : (rawTaskType == QStringLiteral("FollowRoute") ||
           rawTaskType == QStringLiteral("MoveAlongRoute")
           ? QStringLiteral("Follow Route")
           : rawTaskType == QStringLiteral("HoldRacetrack")
             ? QStringLiteral("Hold Racetrack")
           : rawTaskType == QStringLiteral("WaitOnLocation")
             ? QStringLiteral("Wait on Location")
           : rawTaskType == QStringLiteral("AttackOnce")
             ? QStringLiteral("Attack Once")
           : rawTaskType == QStringLiteral("AttackUntilDestroyed")
             ? QStringLiteral("Attack Until Destroyed")
           : rawTaskType == QStringLiteral("FireOnPosition")
             ? QStringLiteral("Fire on Position")
           : rawTaskType == QStringLiteral("FireInDirection")
             ? QStringLiteral("Fire in Direction")
           : rawTaskType == QStringLiteral("StopWeaponsTask")
             ? QStringLiteral("Stop Weapons Task")
           : rawTaskType == QStringLiteral("WaitUntilTargetDetected")
             ? QStringLiteral("Wait Until Target Detected")
           : rawTaskType == QStringLiteral("WaitUntilTargetDestroyed")
             ? QStringLiteral("Wait Until Target Destroyed")
           : rawTaskType == QStringLiteral("WaitUntilDamaged")
             ? QStringLiteral("Wait Until Damaged")
           : rawTaskType == QStringLiteral("WaitUntilTime")
             ? QStringLiteral("Wait Until Time")
           : rawTaskType == QStringLiteral("WaitUntilInRange")
             ? QStringLiteral("Wait Until In Range")
           : rawTaskType);
  };
  const auto displayTaskStatus = [&summary](const QString& rawTaskType, const QString& rawStatus) {
    const int current = summary.value(QStringLiteral("taskRouteCurrentWaypointIndex")).toInt(0);
    const int total = summary.value(QStringLiteral("taskRouteTotalWaypoints")).toInt(0);
    if ((rawTaskType == QStringLiteral("FollowRoute") ||
         rawTaskType == QStringLiteral("MoveAlongRoute")) &&
        total > 0) {
      return QStringLiteral("%1 WP %2/%3").arg(rawStatus).arg(current).arg(total);
    }
    return rawStatus;
  };
  const QString operationalState =
      taskType == QStringLiteral("-") || taskType == QStringLiteral("No current tasks")
          ? status
          : QStringLiteral("%1 (%2)").arg(displayTaskType(taskType), displayTaskStatus(taskType, taskStatus));
  const QString selectedEntityReference =
      summary.value(QStringLiteral("entityId")).toString().trimmed().isEmpty()
          ? name
          : summary.value(QStringLiteral("entityId")).toString();
  const Entity* selectedEntity = this->findEntityByName(selectedEntityReference);
  this->_ui->selectionStateValueLabel->setWordWrap(false);
  this->_ui->selectionStateValueLabel->setStyleSheet(QString());
  this->_ui->selectionStateValueLabel->setText(operationalState);
  this->_ui->selectionPositionValueLabel->setText(position);
  const QString operationalStatus =
      this->buildSelectedEntityOperationalStatus(summary, selectedEntity);
  if (this->_ui->operationalStatusPlainTextEdit->toPlainText() !=
      operationalStatus) {
    QScrollBar* verticalScrollBar =
        this->_ui->operationalStatusPlainTextEdit->verticalScrollBar();
    QScrollBar* horizontalScrollBar =
        this->_ui->operationalStatusPlainTextEdit->horizontalScrollBar();
    const int verticalScrollPosition = verticalScrollBar->value();
    const int horizontalScrollPosition = horizontalScrollBar->value();

    this->_ui->operationalStatusPlainTextEdit->setPlainText(operationalStatus);
    verticalScrollBar->setValue(verticalScrollPosition);
    horizontalScrollBar->setValue(horizontalScrollPosition);
  }
}

QString MainWindow::buildSelectedEntityOperationalStatus(
    const QVariantMap& summary,
    const Entity* entity) const {
  const presentation::EntityStatusContext ctx{
      this->_scenarioState->entities(),
      this->_scenarioState->activeMunitions(),
      this->_bombReleaseController->pendingRelease(),
      this->_planExecutor->plans(),
      this->_simulationRunning,
      this->_bombReleaseController->queuedTargetCount(),
      this->_bombReleaseController->nextQueuedTargetLabel()};
  return presentation::buildEntityOperationalStatus(summary, entity, ctx);
}

void MainWindow::initializeModels() {
  this->initializeObjectTreeModel();
  this->initializeContactsTableModel();

  this->_ui->eventLogPlainTextEdit->clear();
  this->appendLogMessage(QStringLiteral("Operational log ready."));
  this->appendLogMessage(QStringLiteral("Cesium map connected."));
  this->appendLogMessage(QStringLiteral("Awaiting commands..."));

  this->_ui->objectsTreeView->clearSelection();
  this->setSelectedTrackDetails(QVariantMap{});
  this->rebuildTacticalGraphicsTree();
}

void MainWindow::initializeObjectTreeModel() {
  this->_objectsModel->setHorizontalHeaderLabels({QStringLiteral("Name")});

  this->_friendlyRootItem = new QStandardItem(QStringLiteral("Friendly"));
  this->_friendlyRootItem->setIcon(presentation::makeTrackIcon(QStringLiteral("Friendly"), QStringLiteral("Side"), true));
  this->_opposingRootItem = new QStandardItem(QStringLiteral("Opposing"));
  this->_opposingRootItem->setIcon(presentation::makeTrackIcon(QStringLiteral("Opposing"), QStringLiteral("Side"), true));
  this->_neutralRootItem = new QStandardItem(QStringLiteral("Neutral"));
  this->_neutralRootItem->setIcon(presentation::makeTrackIcon(QStringLiteral("Neutral"), QStringLiteral("Side"), true));
  this->_tacticalGraphicsRootItem = new QStandardItem(QStringLiteral("Tactical Graphics"));
  this->_tacticalGraphicsRootItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Graphic")));

  setTrackData(
      this->_friendlyRootItem,
      presentation::makeTrackSummary(
          QStringLiteral("Friendly"),
          QStringLiteral("Side"),
          QStringLiteral("Friendly"),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("0 tracks"),
          0.0,
          0.0));
  setTrackData(
      this->_opposingRootItem,
      presentation::makeTrackSummary(
          QStringLiteral("Opposing"),
          QStringLiteral("Side"),
          QStringLiteral("Opposing"),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("0 tracks"),
          0.0,
          0.0));
  setTrackData(
      this->_neutralRootItem,
      presentation::makeTrackSummary(
          QStringLiteral("Neutral"),
          QStringLiteral("Side"),
          QStringLiteral("Neutral"),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("0 tracks"),
          0.0,
          0.0));
  setTrackData(
      this->_tacticalGraphicsRootItem,
      presentation::makeTrackSummary(
          QStringLiteral("Tactical Graphics"),
          QStringLiteral("Graphic"),
          QStringLiteral("Overlay"),
          QStringLiteral("-"),
          QStringLiteral("Mission overlays"),
          QStringLiteral("Graphics"),
          0.0,
          0.0));

  this->_objectsModel->appendRow(this->_friendlyRootItem);
  this->_objectsModel->appendRow(this->_opposingRootItem);
  this->_objectsModel->appendRow(this->_neutralRootItem);
  this->_objectsModel->appendRow(this->_tacticalGraphicsRootItem);

  this->_ui->objectsTreeView->setModel(this->_objectsModel);
  this->_ui->objectsTreeView->expandAll();
  this->_ui->objectsTreeView->header()->setStretchLastSection(true);
  this->_ui->objectsTreeView->setHeaderHidden(false);
  this->_ui->objectsTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

  QObject::connect(
      this->_ui->objectsTreeView->selectionModel(),
      &QItemSelectionModel::currentChanged,
      this,
      &MainWindow::updateSelectedTrackPanel);
}

void MainWindow::initializeContactsTableModel() {
  this->_detectedContactsModel->setHorizontalHeaderLabels({
      QStringLiteral("Observer"),
      QStringLiteral("Contact"),
      QStringLiteral("Side"),
      QStringLiteral("Type"),
      QStringLiteral("Range"),
      QStringLiteral("Bearing"),
      QStringLiteral("Altitude"),
      QStringLiteral("Last Seen"),
  });
  this->_ui->contactsTableView->setModel(this->_detectedContactsModel);
  this->_ui->contactsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->_ui->contactsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  this->_ui->contactsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  this->_ui->contactsTableView->setSortingEnabled(false);
  this->_ui->contactsTableView->horizontalHeader()->setStretchLastSection(true);
  this->_ui->contactsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  this->_ui->contactsTableView->verticalHeader()->setVisible(false);
  QObject::connect(
      this->_ui->contactsTableView->selectionModel(),
      &QItemSelectionModel::currentChanged,
      this,
      &MainWindow::handleDetectedContactSelection);
}

void MainWindow::appendEntityToUi(const Entity& entity) {
  this->_entityHomePositionTracker->remember(entity);

  QStandardItem* rootItem = this->rootItemForForceIdentifier(entity.forceIdentifier);
  if (!rootItem) {
    return;
  }

  const QString category = presentation::normalizeEntityCategory(entity.category);

  const QVariantMap summary = this->makeEntityTrackSummary(entity);
  QStandardItem* categoryItem = this->ensureGroupItem(
      rootItem,
      category,
      presentation::makeTrackSummary(
          category,
          QStringLiteral("Category"),
          domain::forceIdentifierLabel(entity.forceIdentifier),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("Category"),
          0.0,
          0.0));

  auto* item = new QStandardItem(
      entityTreeLabel(entity, this->_scenarioState->entities()));
  item->setIcon(presentation::makeTrackIcon(domain::forceIdentifierLabel(entity.forceIdentifier), category, false));
  setTrackData(item, summary);
  categoryItem->appendRow(item);
  this->_ui->objectsTreeView->expand(rootItem->index());
  this->_ui->objectsTreeView->expand(categoryItem->index());
  this->_ui->objectsTreeView->setCurrentIndex(item->index());

  this->appendLogMessage(EntityTextFormatter::listLabel(entity));
  this->sendTrackToMap(summary, true);
  this->syncDetectedContactsToUi();
}

QVariantMap MainWindow::makeEntityTrackSummary(const Entity& entity) const {
  const presentation::EntityVisualState visualState =
      this->_entityVisualStateManager->stateFor(domain::entityKey(entity));
  return presentation::makeEntityTrackSummary(entity, visualState);
}

const Waypoint* MainWindow::findWaypointByName(const QString& waypointName) const {
  if (waypointName.trimmed().isEmpty()) {
    return nullptr;
  }

  for (const Waypoint& waypoint : this->_scenarioState->waypoints()) {
    if (waypoint.name == waypointName) {
      return &waypoint;
    }
  }

  return nullptr;
}

const RouteGraphic* MainWindow::findRouteByName(const QString& routeName) const {
  if (routeName.trimmed().isEmpty()) {
    return nullptr;
  }

  for (const RouteGraphic& route : this->_scenarioState->routes()) {
    if (route.name == routeName) {
      return &route;
    }
  }

  return nullptr;
}

const AreaDefinition* MainWindow::findAreaByNameOrId(const QString& areaNameOrId) const {
  if (areaNameOrId.trimmed().isEmpty()) {
    return nullptr;
  }

  for (const AreaDefinition& area : this->_scenarioState->areas()) {
    if (area.name == areaNameOrId || area.id == areaNameOrId) {
      return &area;
    }
  }

  return nullptr;
}

QStringList MainWindow::availableWaypointNames() const {
  return application::availableWaypointNames(this->_scenarioState);
}

QStringList MainWindow::availableRouteNames(bool requirePoints) const {
  return application::availableRouteNames(this->_scenarioState, requirePoints);
}

QStringList MainWindow::availableAreaNames() const {
  return application::availableAreaNames(this->_scenarioState);
}

EntityPlan& MainWindow::ensureEntityPlan(const QString& entityName) {
  return this->_planExecutor->ensurePlan(entityName);
}

void MainWindow::pruneEntityPlans() {
  this->_planExecutor->prunePlans();
}

bool MainWindow::captureTaskConfiguration(
    const QString& entityName,
    const EntityTask& initialTask,
    const QString& initialTaskType,
    EntityTask& outTask) {
  if (this->_taskDialog) {
    this->_taskDialog->close();
    this->_taskDialog = nullptr;
  }

  QHash<QString, int> duplicateCounts;
  for (const Entity& candidate : this->_scenarioState->entities()) {
    ++duplicateCounts[candidate.name.toCaseFolded()];
  }

  QVector<EntityTargetOption> availableTargets;
  for (const Entity& entity : this->_scenarioState->entities()) {
    if (!domain::entityMatchesReference(entity, entityName)) {
      const QString id = domain::entityKey(entity);
      const bool duplicateName = duplicateCounts.value(entity.name.toCaseFolded()) > 1;
      availableTargets.push_back(EntityTargetOption{
          id,
          entity.name,
          duplicateName
              ? QStringLiteral("%1 [%2]").arg(entity.name, id.left(8))
              : entity.name,
      });
    }
  }

  const QStringList availableWaypoints = this->availableWaypointNames();
  const QStringList availableRoutes = this->availableRouteNames(false);
  const QStringList availableAreas = this->availableAreaNames();

  QWidget* dialogParent = QApplication::activeModalWidget();
  if (!dialogParent) {
    dialogParent = this;
  }

  QPointer<AssignTaskDialog> dialog = new AssignTaskDialog(
      this->findEntityByName(entityName)
          ? this->findEntityByName(entityName)->name
          : entityName,
      availableTargets,
      availableWaypoints,
      availableRoutes,
      availableAreas,
      initialTask,
      initialTaskType,
      dialogParent);
  dialog->setAttribute(Qt::WA_DeleteOnClose, false);
  dialog->setWindowModality(dialogParent == this ? Qt::NonModal : Qt::WindowModal);
  this->_taskDialog = dialog;

  QEventLoop loop;
  bool accepted = false;

  QObject::connect(
      dialog,
      &QObject::destroyed,
      &loop,
      [this, &loop]() {
        this->_taskDialog = nullptr;
        this->_taskCoordinatePickPending = false;
        if (loop.isRunning()) {
          loop.quit();
        }
      });
  QObject::connect(
      dialog,
      &AssignTaskDialog::pickOnMapRequested,
      this,
      &MainWindow::beginTaskCoordinatePick);
  QObject::connect(
      dialog,
      &QDialog::finished,
      &loop,
      [this, dialog, &accepted, &outTask, &loop](int result) {
        this->_taskCoordinatePickPending = false;
        accepted = result == QDialog::Accepted;
        if (accepted && dialog) {
          outTask = dialog->task();
        }
        if (loop.isRunning()) {
          loop.quit();
        }
      });

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
  loop.exec();

  if (dialog) {
    dialog->deleteLater();
  }

  return accepted;
}

QStandardItem* MainWindow::rootItemForForceIdentifier(int forceIdentifier) const {
  switch (forceIdentifier) {
    case 1:
      return this->_friendlyRootItem;
    case 2:
      return this->_opposingRootItem;
    case 3:
      return this->_neutralRootItem;
    default:
      return this->_neutralRootItem ? this->_neutralRootItem : this->_friendlyRootItem;
  }
}

QStandardItem* MainWindow::ensureGroupItem(
    QStandardItem* parent,
    const QString& label,
    const QVariantMap& summary) {
  if (!parent) {
    return nullptr;
  }

  for (int row = 0; row < parent->rowCount(); ++row) {
    QStandardItem* child = parent->child(row);
    if (child && child->text() == label) {
      return child;
    }
  }

  auto* item = new QStandardItem(label);
  item->setIcon(presentation::makeTrackIcon(summary.value(QStringLiteral("team")).toString(), label, true));
  setTrackData(item, summary);
  parent->appendRow(item);
  return item;
}

void MainWindow::openAddEntityDialog() {
  if (this->_entityDialog) {
    this->_entityDialog->show();
    this->_entityDialog->raise();
    this->_entityDialog->activateWindow();
    return;
  }

  const QVector<ModelCatalogEntry> modelCatalog = ModelCatalog::loadModels();
  QVector<infrastructure::SensorModelProviderEntry> sensorModelProviders =
      infrastructure::SensorModelProviderCatalog::loadProviders();
  const QStringList registeredProviderIds = _scenarioState->sensorModelIds();
  for (infrastructure::SensorModelProviderEntry& provider : sensorModelProviders) {
    provider.available = registeredProviderIds.contains(
        provider.id,
        Qt::CaseInsensitive);
  }
  auto* dialog = new AddEntityDialog(
      modelCatalog,
      sensorModelProviders,
      this);
  this->_entityDialog = dialog;

  QObject::connect(
      dialog,
      &QObject::destroyed,
      this,
      [this]() {
        this->_entityDialog = nullptr;
        this->_entityCoordinatePickPending = false;
      });
  QObject::connect(
      dialog,
      &AddEntityDialog::pickOnMapRequested,
      this,
      &MainWindow::beginEntityCoordinatePick);
  QObject::connect(
      dialog,
      &QDialog::accepted,
      this,
      [this, dialog]() { this->onEntityDialogAccepted(dialog->entity()); });

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void MainWindow::onEntityDialogAccepted(const Entity& entity) {
  const bool hasRadarSensor = std::any_of(
      entity.sensors.cbegin(),
      entity.sensors.cend(),
      [](const SensorDefinition& sensor) {
        return sensor.sensorType.compare(QStringLiteral("radar"), Qt::CaseInsensitive) == 0;
      });
  if (hasRadarSensor && !entity.name.trimmed().isEmpty() &&
      !this->_entityVisualStateManager->contains(domain::entityKey(entity))) {
    presentation::EntityVisualState& visualState =
        this->_entityVisualStateManager->ensureState(domain::entityKey(entity));
    visualState.radarCoverageVisible = true;
    visualState.trackHistoryVisible = false;
    this->_entityVisualStateManager->save();
  }
  this->_scenarioState->addEntity(entity);
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(EntityTextFormatter::statusMessage(entity));
}

void MainWindow::beginMapCoordinatePick() {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    this->_ui->statusLabel->setText(
        QStringLiteral("El visor de CesiumJS no esta disponible ahora mismo."));
    return;
  }

  if (this->_entityDialog) {
    this->_entityDialog->hide();
  }
  if (this->_taskDialog) {
    this->_taskDialog->hide();
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("Haz clic en el mapa para rellenar latitud y longitud."));
  this->_webView->page()->runJavaScript(
      QStringLiteral("window.beginQtCoordinatePick && window.beginQtCoordinatePick();"));
#else
  this->_ui->statusLabel->setText(
      QStringLiteral("Qt WebEngine no esta disponible en este build."));
#endif
}

void MainWindow::beginEntityCoordinatePick() {
  if (!this->_entityDialog) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay una entidad esperando coordenadas."));
    return;
  }
  this->_entityCoordinatePickPending = true;
  this->_taskCoordinatePickPending = false;
  this->_bombReleaseController->cancelPickMode();
  this->beginMapCoordinatePick();
}

void MainWindow::reportPickedCoordinate(double longitude, double latitude, double height) {
  if (this->_taskCoordinatePickPending) {
    this->_taskCoordinatePickPending = false;
    if (!this->_taskDialog) {
      this->_ui->statusLabel->setText(
          QStringLiteral("La task ya no esta esperando coordenadas."));
      return;
    }
    QPointer<AssignTaskDialog> taskDialog = this->_taskDialog;
    taskDialog->setPickedCoordinate(longitude, latitude, height);
    QTimer::singleShot(0, this, [taskDialog]() {
      if (!taskDialog) {
        return;
      }
      taskDialog->show();
      taskDialog->raise();
      taskDialog->activateWindow();
    });
    this->_ui->statusLabel->setText(
        QStringLiteral("Coordenadas capturadas: lat %1, lon %2")
            .arg(latitude, 0, 'f', 6)
            .arg(longitude, 0, 'f', 6));
    return;
  }

  if (this->_entityCoordinatePickPending) {
    this->_entityCoordinatePickPending = false;
    if (!this->_entityDialog) {
      this->_ui->statusLabel->setText(
          QStringLiteral("La entidad ya no esta esperando coordenadas."));
      return;
    }
    this->_entityDialog->setPickedCoordinate(longitude, latitude, height);
    this->_entityDialog->show();
    this->_entityDialog->raise();
    this->_entityDialog->activateWindow();
    this->_ui->statusLabel->setText(
        QStringLiteral("Coordenadas capturadas: lat %1, lon %2")
            .arg(latitude, 0, 'f', 6)
            .arg(longitude, 0, 'f', 6));
    return;
  }

  if (this->_graphicPickCoordinator->handleCoordinate(longitude, latitude, height)) {
    return;
  }

  if (this->_bombReleaseController->isPickingMode()) {
    this->handleBombPickCoordinate(longitude, latitude);
    return;
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("Coordenadas recibidas sin operacion pendiente: lat %1, lon %2")
          .arg(latitude,  0, 'f', 5)
          .arg(longitude, 0, 'f', 5));
}

void MainWindow::handleBombPickCoordinate(double longitude, double latitude) {
  const QString launcherName = this->_bombReleaseController->pickingLauncherName();
  const bool queueTarget = this->_bombReleaseController->isQueuePickingMode();
  this->_bombReleaseController->cancelPickMode();

  const Entity* launcher = this->findEntityByName(launcherName);
  const int bombCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) : 0;
  if (!launcher || launcher->destroyed || bombCount <= 0) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo programar el release de bomba para %1.")
            .arg(launcherName));
    return;
  }

  const QString targetLabel = domain::attackPointLabel(latitude, longitude);
  if (queueTarget) {
    this->_bombReleaseController->addTargetToQueue(
        launcherName,
        latitude,
        longitude,
        0.0,
        targetLabel,
        QStringLiteral("Pick on map"));
  } else {
    this->queuePendingBombRelease(
        launcherName,
        latitude,
        longitude,
        0.0,
        targetLabel,
        QStringLiteral("Pick on map"));
  }
}

void MainWindow::openSelectedEntityDetails() {
  const QModelIndex currentIndex = this->_ui->objectsTreeView->currentIndex();
  if (!currentIndex.isValid()) {
    return;
  }

  const QVariantMap summary = currentIndex.data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    return;
  }

  const QString entityReference =
      summary.value(QStringLiteral("entityId")).toString().trimmed().isEmpty()
          ? summary.value(QStringLiteral("name")).toString()
          : summary.value(QStringLiteral("entityId")).toString();
  auto* dialog = new EntityDetailsDialog(
      summary,
      [this, entityReference]() -> QVariantMap {
        const Entity* entity = this->findEntityByName(entityReference);
        return entity ? this->makeEntityTrackSummary(*entity) : QVariantMap{};
      },
      this);
  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void MainWindow::reportMapStatus(const QString& message) {
  if (!message.trimmed().isEmpty()) {
    this->_ui->statusLabel->setText(message);
  }
}

void MainWindow::startSimulation() {
  if (_simulationRunning) return;
  this->_simulationLifecycleController->start();
  _simulationStopped = false;
  if (!_applyingHlaSimulationControl) {
    emit hlaSimulationControlRequested(
        tactical::hla::RemoteSimulationControl::StartResume,
        _scenarioState->simulationTimeSeconds());
  }
}

void MainWindow::pauseSimulation() {
  if (!_simulationRunning) return;
  this->_simulationLifecycleController->pause();
  _simulationStopped = false;
  if (!_applyingHlaSimulationControl) {
    emit hlaSimulationControlRequested(
        tactical::hla::RemoteSimulationControl::Pause,
        _scenarioState->simulationTimeSeconds());
  }
}

void MainWindow::stopSimulation() {
  const bool publishStop = !_simulationStopped;
  const double simulationTimeSeconds =
      _scenarioState->simulationTimeSeconds();
  this->_simulationLifecycleController->stop();
  _simulationStopped = true;
  if (publishStop && !_applyingHlaSimulationControl) {
    emit hlaSimulationControlRequested(
        tactical::hla::RemoteSimulationControl::Stop,
        simulationTimeSeconds);
  }
}

void MainWindow::updateSelectedTrackPanel(const QModelIndex& current, const QModelIndex&) {
  const QVariantMap summary = current.data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    this->setSelectedTrackDetails(QVariantMap{});
    this->refreshKinematicsCockpitForEntity(nullptr);
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
    if (!this->_applyingMapSelection) {
      clearQtTrackSelectionInMap(this->_webView);
    }
#endif
    this->updateTaskQuickBarState();
    return;
  }

  this->setSelectedTrackDetails(summary);

  const QString selectedName = summary.value(QStringLiteral("name")).toString();
  if (!selectedName.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Track seleccionado: %1").arg(selectedName));
  }

  if (!this->currentSelectionIsEntity()) {
    this->refreshKinematicsCockpitForEntity(nullptr);
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
    if (!this->_applyingMapSelection) {
      clearQtTrackSelectionInMap(this->_webView);
    }
#endif
    this->updateTaskQuickBarState();
    return;
  }

  const QString selectedReference =
      summary.value(QStringLiteral("entityId")).toString().trimmed().isEmpty()
          ? selectedName
          : summary.value(QStringLiteral("entityId")).toString();
  this->refreshKinematicsCockpitForEntity(
      this->findEntityByName(selectedReference));

  if (!this->_applyingMapSelection) {
    this->sendTrackToMap(summary, true);
  }
  this->updateTaskQuickBarState();
}

void MainWindow::handleDetectedContactSelection(const QModelIndex& current, const QModelIndex&) {
  if (!current.isValid()) {
    return;
  }

  const QModelIndex rowIndex = current.sibling(current.row(), 0);
  const QString observerName = rowIndex.data(kDetectedContactObserverRole).toString();
  const QString contactName = rowIndex.data(kDetectedContactTargetRole).toString();
  if (observerName.isEmpty() || contactName.isEmpty()) {
    return;
  }

  const Entity* observer = this->findEntityByName(observerName);
  const Entity* contact = this->findEntityByName(contactName);
  this->_ui->statusLabel->setText(
      QStringLiteral("Contacto seleccionado: %1 detecta a %2.")
          .arg(observer ? observer->name : observerName,
               contact ? contact->name : contactName));
}

void MainWindow::handleMapTrackSelection(const QString& trackName) {
  const auto clearQtObjectSelection = [this]() {
    if (QItemSelectionModel* selectionModel = this->_ui->objectsTreeView->selectionModel()) {
      selectionModel->clearSelection();
      selectionModel->clearCurrentIndex();
    }
  };

  if (trackName.trimmed().isEmpty()) {
    this->_applyingMapSelection = true;
    clearQtObjectSelection();
    this->_applyingMapSelection = false;
    this->setSelectedTrackDetails(QVariantMap{});
    this->updateTaskQuickBarState();
    this->_ui->statusLabel->setText(QStringLiteral("No hay entidad seleccionada."));
    return;
  }

  this->_applyingMapSelection = true;
  this->selectObjectByName(trackName, false);
  this->_applyingMapSelection = false;

  if (this->selectedObjectName() != trackName) {
    this->_applyingMapSelection = true;
    clearQtObjectSelection();
    this->_applyingMapSelection = false;
    this->setSelectedTrackDetails(QVariantMap{});
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
    clearQtTrackSelectionInMap(this->_webView);
#endif
    this->updateTaskQuickBarState();
    this->_ui->statusLabel->setText(QStringLiteral("No hay entidad seleccionada."));
    return;
  }

  if (this->currentSelectionIsEntity()) {
    const QVariantMap summary =
        this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
    this->sendTrackToMap(summary, true);
  }
  this->updateTaskQuickBarState();
}

void MainWindow::openMapEntityContextMenu(const QString& trackName, int viewX, int viewY) {
  if (trackName.trimmed().isEmpty()) {
    this->handleMapTrackSelection(QString());
    return;
  }

  this->handleMapTrackSelection(trackName);
  if (this->selectedEntityName() != trackName) {
    return;
  }

  QWidget* anchor = this->_ui->viewerHost;
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (this->_webView) {
    anchor = this->_webView;
  }
#endif
  if (!anchor) {
    return;
  }

  QMenu menu(this);
  this->populateEntityContextMenu(menu);
  menu.exec(anchor->mapToGlobal(QPoint(viewX, viewY)));
}

void MainWindow::toggleTacticalOverlays(bool enabled) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    return;
  }

  const QString script = QStringLiteral(
      "window.setQtOverlayVisibility && window.setQtOverlayVisibility(%1);")
                             .arg(enabled ? QStringLiteral("true") : QStringLiteral("false"));
  this->_webView->page()->runJavaScript(script);
  this->_ui->statusLabel->setText(
      enabled ? QStringLiteral("Overlays tacticos activados.")
              : QStringLiteral("Overlays tacticos ocultos."));
#else
  Q_UNUSED(enabled)
#endif
}

void MainWindow::sendTrackToMap(const QVariantMap& summary, bool focus) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    return;
  }
  const QString script = presentation::MapBridgeScripts::buildAddOrUpdateScript(summary, focus);
  if (!script.isEmpty()) {
    this->_webView->page()->runJavaScript(script);
  }
#else
  Q_UNUSED(summary)
  Q_UNUSED(focus)
#endif
}

void MainWindow::removeTrackFromMap(const QString& trackName) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    return;
  }
  const QString script = presentation::MapBridgeScripts::buildRemoveScript(trackName);
  if (!script.isEmpty()) {
    this->_webView->page()->runJavaScript(script);
  }
#else
  Q_UNUSED(trackName)
#endif
}

void MainWindow::sendDraftGraphicToMap(const QVariantMap& summary) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    return;
  }
  const QString script = presentation::MapBridgeScripts::buildAddOrUpdateDraftScript(summary);
  if (!script.isEmpty()) {
    this->_webView->page()->runJavaScript(script);
  }
#else
  Q_UNUSED(summary)
#endif
}

void MainWindow::clearDraftGraphicFromMap(const QString& name) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    return;
  }
  const QString script = presentation::MapBridgeScripts::buildRemoveDraftScript(name);
  if (!script.isEmpty()) {
    this->_webView->page()->runJavaScript(script);
  }
#else
  Q_UNUSED(name)
#endif
}

void MainWindow::syncTracksToMap() {
  auto syncBranch = [this](QStandardItem* branch) {
    if (!branch) {
      return;
    }
    for (int row = 0; row < branch->rowCount(); ++row) {
      QStandardItem* child = branch->child(row);
      if (!child) {
        continue;
      }
      const QVariantMap summary = child->data(kTrackSummaryRole).toMap();
      if (!summary.isEmpty()) {
        this->sendTrackToMap(summary, false);
      }
    }
  };

  syncBranch(this->_friendlyRootItem);
  syncBranch(this->_opposingRootItem);
  syncBranch(this->_neutralRootItem);
}

void MainWindow::syncTacticalGraphicsToMap() {
  if (!this->_tacticalGraphicsRootItem) {
    return;
  }
  for (int row = 0; row < this->_tacticalGraphicsRootItem->rowCount(); ++row) {
    QStandardItem* item = this->_tacticalGraphicsRootItem->child(row);
    if (!item) {
      continue;
    }
    const QVariantMap summary = item->data(kTrackSummaryRole).toMap();
    if (!summary.isEmpty()) {
      this->sendTrackToMap(summary, false);
    }
  }
}

void MainWindow::syncDetectedContactsToUi() {
  if (!this->_detectedContactsModel) {
    return;
  }

  QScrollBar* verticalScrollBar =
      this->_ui->contactsTableView->verticalScrollBar();
  QScrollBar* horizontalScrollBar =
      this->_ui->contactsTableView->horizontalScrollBar();
  const int verticalScrollPosition = verticalScrollBar->value();
  const int horizontalScrollPosition = horizontalScrollBar->value();

  QString selectedObserverName;
  QString selectedContactName;
  const QModelIndex currentIndex = this->_ui->contactsTableView->currentIndex();
  if (currentIndex.isValid()) {
    const QModelIndex rowIndex = currentIndex.sibling(currentIndex.row(), 0);
    selectedObserverName = rowIndex.data(kDetectedContactObserverRole).toString();
    selectedContactName = rowIndex.data(kDetectedContactTargetRole).toString();
  }

  this->_detectedContactsModel->removeRows(0, this->_detectedContactsModel->rowCount());

  const QString lastSeenText =
      QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
  int restoredRow = -1;

  const QVector<presentation::DetectedContactRow> rows =
      presentation::buildDetectedContactRows(this->_scenarioState->entities());

  for (const presentation::DetectedContactRow& row : rows) {
    auto* observerItem = new QStandardItem(row.observerName);
    observerItem->setData(row.observerEntityId, kDetectedContactObserverRole);
    observerItem->setData(row.targetEntityId, kDetectedContactTargetRole);

    QList<QStandardItem*> rowItems{
        observerItem,
        new QStandardItem(row.targetName),
        new QStandardItem(row.forceLabel),
        new QStandardItem(row.typeLabel),
        new QStandardItem(row.rangeText),
        new QStandardItem(row.bearingText),
        new QStandardItem(row.altitudeText),
        new QStandardItem(lastSeenText),
    };

    for (QStandardItem* item : rowItems) {
      if (item) {
        item->setEditable(false);
      }
    }

    this->_detectedContactsModel->appendRow(rowItems);

    if (row.observerEntityId == selectedObserverName &&
        row.targetEntityId == selectedContactName) {
      restoredRow = this->_detectedContactsModel->rowCount() - 1;
    }
  }

  this->_ui->contactsDockWidget->setWindowTitle(
      QStringLiteral("Detected Contacts (%1)")
          .arg(this->_detectedContactsModel->rowCount()));

  if (restoredRow >= 0) {
    const QModelIndex restoredIndex =
        this->_detectedContactsModel->index(restoredRow, 0);
    this->_ui->contactsTableView->setCurrentIndex(restoredIndex);
    this->_ui->contactsTableView->scrollTo(restoredIndex);
  } else {
    this->_ui->contactsTableView->clearSelection();
  }

  verticalScrollBar->setValue(verticalScrollPosition);
  horizontalScrollBar->setValue(horizontalScrollPosition);
}

void MainWindow::syncScenarioStateToUi() {
  this->validatePendingBombRelease();
  const QString prevSelected = this->selectedEntityName();

  if (this->syncEntityTreeToUi(prevSelected)) {
    this->_applyingMapSelection = true;
    if (QItemSelectionModel* sel = this->_ui->objectsTreeView->selectionModel()) {
      sel->clearSelection();
      sel->clearCurrentIndex();
    }
    this->_applyingMapSelection = false;
    this->setSelectedTrackDetails(QVariantMap{});
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
    clearQtTrackSelectionInMap(this->_webView);
#endif
    this->_ui->statusLabel->setText(QStringLiteral("No hay entidad seleccionada."));
  }

  this->syncActiveMunitionTracksToMap();
  this->syncTransientEffectsToMap();
  this->syncPendingBombTargetToMap();
  this->syncDetectedContactsToUi();

  for (const QString& message : this->_scenarioState->takePendingEventLogMessages()) {
    this->appendLogMessage(message);
    if (message.contains(QStringLiteral("behavior"), Qt::CaseInsensitive)) {
      this->_ui->statusLabel->setText(message);
    }
  }

  if (!this->_simulationRunning) {
    this->rebuildTacticalGraphicsTree();
    this->syncTacticalGraphicsToMap();
  }

  this->updateTaskQuickBarState();
}

bool MainWindow::syncEntityTreeToUi(const QString& selectedEntityNameBeforeSync) {
  bool selectedEntityRemoved = false;

  std::function<void(QStandardItem*)> removeMissingFromBranch =
      [this, &removeMissingFromBranch, &selectedEntityRemoved, &selectedEntityNameBeforeSync](
          QStandardItem* branch) {
    if (!branch) {
      return;
    }
    for (int row = branch->rowCount() - 1; row >= 0; --row) {
      QStandardItem* child = branch->child(row);
      if (!child) {
        continue;
      }
      if (child->rowCount() > 0) {
        removeMissingFromBranch(child);
        if (child->rowCount() == 0) {
          branch->removeRow(row);
        }
        continue;
      }
      const QVariantMap childSummary = child->data(kTrackSummaryRole).toMap();
      const QString name = childSummary.value(QStringLiteral("name")).toString();
      const QString entityReference =
          childSummary.value(QStringLiteral("entityId")).toString().trimmed().isEmpty()
              ? name
              : childSummary.value(QStringLiteral("entityId")).toString();
      bool exists = false;
      for (const Entity& entity : this->_scenarioState->entities()) {
        if (domain::entityMatchesReference(entity, entityReference)) {
          exists = true;
          break;
        }
      }
      if (!exists) {
        if (!entityReference.isEmpty()) {
          this->removeTrackFromMap(entityReference);
          if (entityReference == selectedEntityNameBeforeSync) {
            selectedEntityRemoved = true;
          }
        }
        branch->removeRow(row);
      }
    }
  };

  removeMissingFromBranch(this->_friendlyRootItem);
  removeMissingFromBranch(this->_opposingRootItem);
  removeMissingFromBranch(this->_neutralRootItem);

  {
    QSet<QString> validNames;
    for (const Entity& entity : this->_scenarioState->entities()) {
      validNames.insert(domain::entityKey(entity));
    }
    this->_entityVisualStateManager->pruneTo(validNames);
    this->_entityHomePositionTracker->pruneTo(validNames);
  }
  this->pruneEntityPlans();

  for (const Entity& entity : this->_scenarioState->entities()) {
    this->_entityHomePositionTracker->remember(entity);

    const QString entityReference = domain::entityKey(entity);
    QStandardItem* item = this->findTrackItemByName(this->_friendlyRootItem, entityReference);
    if (!item) {
      item = this->findTrackItemByName(this->_opposingRootItem, entityReference);
    }
    if (!item) {
      item = this->findTrackItemByName(this->_neutralRootItem, entityReference);
    }

    const QVariantMap summary = this->makeEntityTrackSummary(entity);
    if (!item) {
      this->appendEntityToUi(entity);
      continue;
    }

    item->setText(entityTreeLabel(entity, this->_scenarioState->entities()));
    setTrackData(item, summary);
    this->sendTrackToMap(summary, false);
    if (this->_ui->objectsTreeView->currentIndex() == item->index()) {
      this->setSelectedTrackDetails(summary);
    }
  }

  return selectedEntityRemoved;
}

void MainWindow::syncActiveMunitionTracksToMap() {
  this->_activeMunitionTrackNames = presentation::syncCollectionToMap<ActiveMunition>(
      this->_scenarioState->activeMunitions(),
      [](const ActiveMunition& m) { return m.id; },
      [](const ActiveMunition& m) { return presentation::makeMunitionTrackSummary(m); },
      this->_activeMunitionTrackNames,
      [this](const QVariantMap& s, bool sel) { this->sendTrackToMap(s, sel); },
      [this](const QString& id) { this->removeTrackFromMap(id); });
}

void MainWindow::syncTransientEffectsToMap() {
  this->_activeEffectTrackNames = presentation::syncCollectionToMap<TransientEffect>(
      this->_scenarioState->transientEffects(),
      [](const TransientEffect& e) { return e.id; },
      [](const TransientEffect& e) { return presentation::makeTransientEffectTrackSummary(e); },
      this->_activeEffectTrackNames,
      [this](const QVariantMap& s, bool sel) { this->sendTrackToMap(s, sel); },
      [this](const QString& id) { this->removeTrackFromMap(id); });
}

void MainWindow::syncPendingBombTargetToMap() {
  presentation::syncPendingBombTargetToMap(
      this->_bombReleaseController->pendingRelease(),
      [this](const QString& name) { return this->findEntityByName(name); },
      [this](const QVariantMap& s, bool f) { this->sendTrackToMap(s, f); },
      [this](const QString& id) { this->removeTrackFromMap(id); });
}

static const char* kTaskQuickBarStyleSheet =
    "QFrame#taskQuickBar {"
    "  background-color: rgba(34, 34, 34, 225);"
    "  border: 1px solid rgba(105, 115, 128, 180);"
    "  border-radius: 8px;"
    "}"
    "QToolButton#taskQuickBarButton {"
    "  background-color: transparent;"
    "  border: 1px solid transparent;"
    "  border-radius: 4px;"
    "  padding: 2px;"
    "}"
    "QToolButton#taskQuickBarButton:hover:enabled {"
    "  background-color: rgba(68, 87, 108, 200);"
    "  border-color: rgba(120, 146, 173, 200);"
    "}"
    "QToolButton#taskQuickBarButton:pressed:enabled {"
    "  background-color: rgba(52, 69, 88, 220);"
    "}"
    "QToolButton#taskQuickBarButton:disabled {"
    "  background-color: transparent;"
    "  border-color: transparent;"
    "}";

void MainWindow::createTaskQuickBar() {
  if (this->_taskQuickBar) {
    return;
  }

  auto* panel = new QFrame(this->_ui->viewerHost);
  panel->setObjectName(QStringLiteral("taskQuickBar"));
  panel->setFrameShape(QFrame::StyledPanel);
  panel->setStyleSheet(QString::fromLatin1(kTaskQuickBarStyleSheet));

  auto* layout = new QHBoxLayout(panel);
  layout->setContentsMargins(6, 6, 6, 6);
  layout->setSpacing(3);

  this->populateTaskQuickBarButtons(panel, layout);

  panel->adjustSize();
  panel->show();
  panel->raise();
  this->_taskQuickBar = panel;
}

void MainWindow::populateTaskQuickBarButtons(QFrame* panel, QHBoxLayout* layout) {
  auto addButton = [this, panel, layout](
                       const QString& iconFileName,
                       const QString& fallbackGlyph,
                       const QColor& accent,
                       const QString& tooltip,
                       const std::function<void()>& handler) {
    auto* button = new QToolButton(panel);
    button->setObjectName(QStringLiteral("taskQuickBarButton"));
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setAutoRaise(false);
    button->setFixedSize(kTaskQuickBarButtonPixels, kTaskQuickBarButtonPixels);
    button->setIconSize(QSize(kTaskQuickBarIconPixels, kTaskQuickBarIconPixels));
    button->setIcon(presentation::loadTaskQuickBarIcon(
        taskQuickBarIconPath(iconFileName), fallbackGlyph, accent));
    button->setToolTip(tooltip);
    layout->addWidget(button);
    this->_taskQuickButtons.append(button);
    QObject::connect(button, &QToolButton::clicked, this, [this, handler]() {
      if (!this->currentSelectionIsEntity()) {
        return;
      }
      handler();
    });
  };

  addButton(
      QStringLiteral("move_to_location.xpm"),
      QStringLiteral("ML"),
      QColor(QStringLiteral("#55d3ff")),
      QStringLiteral("Move to Location"),
      [this]() { this->assignMoveToLocationTask(); });
  addButton(
      QStringLiteral("move_along_route.xpm"),
      QStringLiteral("FR"),
      QColor(QStringLiteral("#ff8c52")),
      QStringLiteral("Follow Route"),
      [this]() { this->assignMoveAlongRouteTask(); });
  addButton(
      QStringLiteral("fly_heading.xpm"),
      QStringLiteral("FH"),
      QColor(QStringLiteral("#a2f06a")),
      QStringLiteral("Fly Heading"),
      [this]() { this->assignFlyHeadingAltitudeSpeedTask(); });
  addButton(
      QStringLiteral("fly_altitude.xpm"),
      QStringLiteral("FA"),
      QColor(QStringLiteral("#a2f06a")),
      QStringLiteral("Fly Altitude"),
      [this]() { this->assignFlyHeadingAltitudeSpeedTask(); });
  addButton(
      QStringLiteral("patrol_route.xpm"),
      QStringLiteral("PR"),
      QColor(QStringLiteral("#ffd166")),
      QStringLiteral("Patrol Route\nPlaceholder"),
      [this]() { this->showTaskQuickPlaceholder(QStringLiteral("Patrol Route")); });
  addButton(
      QStringLiteral("pattern_hold_location.xpm"),
      QStringLiteral("PH"),
      QColor(QStringLiteral("#ffd166")),
      QStringLiteral("Hold Racetrack"),
      [this]() { this->assignHoldRacetrackTask(); });
  addButton(
      QStringLiteral("orbit_object.xpm"),
      QStringLiteral("OO"),
      QColor(QStringLiteral("#ffd166")),
      QStringLiteral("Orbit Object\nPlaceholder"),
      [this]() { this->showTaskQuickPlaceholder(QStringLiteral("Orbit Object")); });
  addButton(
      QStringLiteral("return_to_base.xpm"),
      QStringLiteral("RT"),
      QColor(QStringLiteral("#55d3ff")),
      QStringLiteral("Return To Base\nPlaceholder"),
      [this]() { this->showTaskQuickPlaceholder(QStringLiteral("Return To Base")); });
  addButton(
      QStringLiteral("execute_surface_attack.xpm"),
      QStringLiteral("SA"),
      QColor(QStringLiteral("#ff6c52")),
      QStringLiteral("Execute Surface Attack\nPlaceholder"),
      [this]() { this->showTaskQuickPlaceholder(QStringLiteral("Execute Surface Attack")); });
  addButton(
      QStringLiteral("fire_at.xpm"),
      QStringLiteral("FA"),
      QColor(QStringLiteral("#ff6c52")),
      QStringLiteral("Fire At\nPlaceholder"),
      [this]() { this->showTaskQuickPlaceholder(QStringLiteral("Fire At")); });
}

void MainWindow::positionTaskQuickBar() {
  if (!this->_taskQuickBar || !this->_ui || !this->_ui->viewerHost) {
    return;
  }

  this->_taskQuickBar->adjustSize();
  const QRect hostRect = this->_ui->viewerHost->rect();
  const QSize panelSize = this->_taskQuickBar->sizeHint();
  const int x = qMax(
      kTaskQuickBarMarginPixels,
      hostRect.width() - panelSize.width() - kTaskQuickBarMarginPixels);
  const int y = qMax(
      kTaskQuickBarMarginPixels,
      hostRect.height() - panelSize.height() - kTaskQuickBarMarginPixels);
  this->_taskQuickBar->resize(panelSize);
  this->_taskQuickBar->move(x, y);
  this->_taskQuickBar->raise();
}

void MainWindow::updateTaskQuickBarState() {
  const bool enabled = this->currentSelectionIsOperableEntity();
  for (QToolButton* button : this->_taskQuickButtons) {
    if (button) {
      button->setEnabled(enabled);
    }
  }
}

void MainWindow::showTaskQuickPlaceholder(const QString& actionName) {
  const QString entityName = this->selectedEntityName();
  const QString targetLabel = entityName.isEmpty()
      ? actionName
      : QStringLiteral("%1 for %2").arg(actionName, entityName);
  this->appendLogMessage(
      QStringLiteral("Quick task placeholder requested: %1").arg(targetLabel));
  this->_ui->statusLabel->setText(
      QStringLiteral("%1 aun no esta implementado en esta quick bar.")
          .arg(actionName));
}

void MainWindow::populateEntityContextMenu(QMenu& menu) {
  const bool entityDestroyed = this->selectedEntityIsDestroyed();
  const QString entityName = this->selectedEntityName();
  const Entity* entity = this->findEntityByName(entityName);

  const QVariantMap summaryMap =
      this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();

  presentation::EntityContextMenuState state =
      presentation::buildEntityContextMenuState(
          entity,
          this->_scenarioState,
          this->_bombReleaseController->pendingRelease(),
          this->_simulationRunning,
          entityDestroyed,
          summaryMap);

  presentation::EntityContextMenuSlots actions;
  actions.assignFlyHeadingAltitudeSpeedTask = [this]() { this->assignFlyHeadingAltitudeSpeedTask(); };
  actions.assignMoveToLocationTask          = [this]() { this->assignMoveToLocationTask(); };
  actions.assignWaitOnLocationTask          = [this]() { this->assignWaitOnLocationTask(); };
  actions.assignMoveToWaypointTask          = [this]() { this->assignMoveToWaypointTask(); };
  actions.assignMoveAlongRouteTask          = [this]() { this->assignMoveAlongRouteTask(); };
  actions.assignPatrolRouteTask             = [this]() { this->assignPatrolRouteTask(); };
  actions.assignOrbitHoldLocationTask       = [this]() { this->assignOrbitHoldLocationTask(); };
  actions.assignHoldRacetrackTask           = [this]() { this->assignHoldRacetrackTask(); };
  actions.assignReturnToBaseTask            = [this]() { this->assignReturnToBaseTask(); };
  actions.assignPatrolAreaTask              = [this]() { this->assignPatrolAreaTask(); };
  actions.assignOrbitAreaTask               = [this]() { this->assignOrbitAreaTask(); };
  actions.assignFollowEntityTask            = [this]() { this->assignFollowEntityTask(); };
  actions.assignInterceptEntityTask         = [this]() { this->assignInterceptEntityTask(); };
  actions.assignAttackOnceTask              = [this]() { this->assignAttackOnceTask(); };
  actions.assignAttackUntilDestroyedTask    = [this]() { this->assignAttackUntilDestroyedTask(); };
  actions.assignFireOnPositionTask          = [this]() { this->assignFireOnPositionTask(); };
  actions.assignFireInDirectionTask         = [this]() { this->assignFireInDirectionTask(); };
  actions.assignStopWeaponsTask             = [this]() { this->assignStopWeaponsTask(); };
  actions.assignAttackAirTask               = [this]() { this->assignAttackAirTask(); };
  actions.assignAttackSurfaceTask           = [this]() { this->assignAttackSurfaceTask(); };
  actions.assignWaitUntilTargetDetectedTask  = [this]() { this->assignWaitUntilTargetDetectedTask(); };
  actions.assignWaitUntilTargetDestroyedTask = [this]() { this->assignWaitUntilTargetDestroyedTask(); };
  actions.assignWaitUntilDamagedTask         = [this]() { this->assignWaitUntilDamagedTask(); };
  actions.assignWaitUntilTimeTask            = [this]() { this->assignWaitUntilTimeTask(); };
  actions.assignWaitUntilInRangeTask         = [this]() { this->assignWaitUntilInRangeTask(); };
  actions.clearSelectedTask                 = [this]() { this->clearSelectedTask(); };
  actions.setSelectedEntityHeading          = [this]() { this->setSelectedEntityHeading(); };
  actions.setSelectedEntityAltitude         = [this]() { this->setSelectedEntityAltitude(); };
  actions.setSelectedEntitySpeed            = [this]() { this->setSelectedEntitySpeed(); };
  actions.setSelectedEntityFuel             = [this]() { this->setSelectedEntityFuel(); };
  actions.setSelectedEntityBehaviorMode     = [this](const QString& m) { this->setSelectedEntityBehaviorMode(m); };
  actions.openEntityPlanDialog              = [this]() { this->openEntityPlanDialog(); };
  actions.addMissileToSelectedEntity        = [this]() { this->addMissileToSelectedEntity(); };
  actions.addBombToSelectedEntity           = [this]() { this->addBombToSelectedEntity(); };
  actions.launchMissileFromSelectedEntity   = [this]() { this->launchMissileFromSelectedEntity(); };
  actions.launchMissileAtSelectedEntity     = [this]() { this->launchMissileAtSelectedEntity(); };
  actions.releaseBombFromSelectedEntity     = [this]() { this->releaseBombFromSelectedEntity(); };
  actions.releaseBombAtSurfaceEntity        = [this]() { this->releaseBombAtSurfaceEntity(); };
  actions.releaseBombAtCustomCoordinates    = [this]() { this->releaseBombAtCustomCoordinates(); };
  actions.addBombTargetToQueue              = [this]() { this->addBombTargetToQueue(); };
  actions.addCustomBombTargetToQueue        = [this]() { this->addCustomBombTargetToQueue(); };
  actions.clearBombTargetQueue              = [this]() { this->clearBombTargetQueue(); };
  actions.cancelPendingBombRelease          = [this]() { this->cancelPendingBombRelease(); };
  actions.openSelectedEntityDetails         = [this]() { this->openSelectedEntityDetails(); };
  actions.focusSelectedEntityInMap          = [this]() { this->focusSelectedEntityInMap(); };
  actions.deleteSelectedEntity              = [this]() { this->deleteSelectedEntity(); };
  actions.restoreSelectedEntity             = [this]() { this->restoreSelectedEntity(); };
  actions.destroySelectedEntity             = [this]() { this->destroySelectedEntity(); };
  actions.setSelectedEntityHidden           = [this](bool h) { this->setSelectedEntityHidden(h); };
  actions.setSelectedEntityRadarCoverageVisible = [this](bool v) { this->setSelectedEntityRadarCoverageVisible(v); };
  actions.setSelectedEntityTrackHistoryVisible  = [this](bool v) { this->setSelectedEntityTrackHistoryVisible(v); };

  presentation::populateEntityContextMenu(menu, state, actions);
}

bool MainWindow::applyEntityTask(const QString& entityName, const EntityTask& task, bool syncUi) {
  return application::applyEntityTask(
      entityName,
      task,
      syncUi,
      this->_scenarioState,
      this->m_simulationEngine,
      [this](const QString& msg) { this->appendLogMessage(msg); },
      [this]() { this->syncScenarioStateToUi(); });
}

bool MainWindow::configurePlanStep(const QString& entityName, PlanStepKind kind, PlanStep& step) {
  const Entity* entity = this->findEntityByName(entityName);
  if (!entity) {
    return false;
  }

  const QVariantMap summary = this->makeEntityTrackSummary(*entity);
  double defaultHeading;
  int    defaultAltitudeMeters;
  double defaultSpeedKnots;
  presentation::resolvePlanStepDefaults(
      *entity, summary, defaultHeading, defaultAltitudeMeters, defaultSpeedKnots);

  return this->_planStepConfigurator->configure(
      *entity, defaultHeading, defaultAltitudeMeters, defaultSpeedKnots, kind, step);
}

bool MainWindow::startEntityPlan(const QString& entityName) {
  if (this->_cockpitControlService->hasControl(entityName)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Libera el control de cockpit antes de iniciar el plan de %1.")
            .arg(entityName));
    return false;
  }
  return this->_planExecutor->startPlan(entityName);
}

void MainWindow::stopEntityPlan(const QString& entityName, bool clearCurrentTask) {
  this->_planExecutor->stopPlan(entityName, clearCurrentTask);
  if (clearCurrentTask) {
    this->syncScenarioStateToUi();
  }
}

void MainWindow::advanceEntityPlans() {
  this->_planExecutor->advancePlans();
}


bool MainWindow::resolveSelectedEntityFlyTargets(
    double& headingDegrees,
    int& altitudeMeters,
    double& speedKnots) const {
  if (!this->currentSelectionIsOperableEntity()) {
    return false;
  }

  const QVariantMap summary =
      this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    return false;
  }

  const int currentEntityAltitudeMeters = application::entityAltitudeMeters(
      this->_scenarioState,
      summary.value(QStringLiteral("entityId")).toString().trimmed().isEmpty()
          ? summary.value(QStringLiteral("name")).toString()
          : summary.value(QStringLiteral("entityId")).toString());

  return presentation::resolveFlyTargetsFromSummary(
      summary, currentEntityAltitudeMeters, headingDegrees, altitudeMeters, speedKnots);
}

void MainWindow::applyFlyHeadingAltitudeSpeedTask(
    double headingDegrees,
    int altitudeMeters,
    double speedKnots) {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty() || this->selectedEntityIsDestroyed()) {
    return;
  }

  EntityTask task;
  task.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
  task.enabled = true;
  task.status = QStringLiteral("Running");
  task.targetHeadingDegrees = headingDegrees;
  task.targetAltitudeMeters = altitudeMeters;
  task.targetSpeedKnots = speedKnots;

  this->applyEntityTask(entityName, task, /*syncUi=*/true);
}

void MainWindow::setSelectedEntityHeading() {
  this->_entityStateActionsController->setSelectedHeading();
}

void MainWindow::setSelectedEntityAltitude() {
  this->_entityStateActionsController->setSelectedAltitude();
}

void MainWindow::setSelectedEntitySpeed() {
  this->_entityStateActionsController->setSelectedSpeed();
}

void MainWindow::setSelectedEntityFuel() {
  this->_entityStateActionsController->setSelectedFuel();
}

void MainWindow::setSelectedEntityBehaviorMode(const QString& behaviorMode) {
  this->_entityStateActionsController->setSelectedBehaviorMode(behaviorMode);
}

void MainWindow::focusSelectedEntityInMap() {
  if (!this->currentSelectionIsEntity()) {
    return;
  }

  const QVariantMap summary = this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    return;
  }

  const QString entityName = summary.value(QStringLiteral("name")).toString();
  this->sendTrackToMap(summary, true);
  this->appendLogMessage(QStringLiteral("Camera focused on %1").arg(entityName));
  this->_ui->statusLabel->setText(
      QStringLiteral("Camara siguiendo a %1.").arg(entityName));
}

void MainWindow::setSelectedEntityDestroyed(bool destroyed) {
  this->_entityStateActionsController->setSelectedDestroyed(destroyed);
}

void MainWindow::setSelectedEntityHidden(bool hidden) {
  this->_entityStateActionsController->setSelectedHidden(hidden);
}

void MainWindow::setSelectedEntityRadarCoverageVisible(bool visible) {
  this->_entityStateActionsController->setSelectedRadarCoverageVisible(visible);
}

void MainWindow::setSelectedEntityTrackHistoryVisible(bool visible) {
  this->_entityStateActionsController->setSelectedTrackHistoryVisible(visible);
}

void MainWindow::destroySelectedEntity() {
  this->_entityStateActionsController->destroySelected();
}

void MainWindow::restoreSelectedEntity() {
  this->_entityStateActionsController->restoreSelected();
}

void MainWindow::addMissileToSelectedEntity() {
  this->_weaponActionsController->addMissileToSelected();
}

void MainWindow::addBombToSelectedEntity() {
  this->_weaponActionsController->addBombToSelected();
}

void MainWindow::launchMissileFromSelectedEntity() {
  this->_weaponActionsController->launchMissileFromSelected();
}

void MainWindow::releaseBombFromSelectedEntity() {
  this->_weaponActionsController->releaseBombFromSelected();
}

void MainWindow::launchMissileAtSelectedEntity() {
  this->_weaponActionsController->launchMissileAtSelected();
}

void MainWindow::executeMissileLaunch(
    const QString& launcherName,
    const QString& targetName,
    int previousMissileCount) {
  // Delegated to WeaponActionsController::launchMissileAtSelected
  // Called internally — kept for backward compat with connect() wiring.
  Q_UNUSED(previousMissileCount)
  if (!this->_scenarioState->launchMissileAt(launcherName, targetName)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Target out of missile range for %1.").arg(launcherName));
    return;
  }
  this->appendLogMessage(
      QStringLiteral("Missile launched from %1 at %2").arg(launcherName, targetName));
  this->syncScenarioStateToUi();
}

void MainWindow::showContextMenuPlaceholder(const QString& actionName) {
  const QString entityName = this->selectedEntityName();
  const QString targetLabel = entityName.isEmpty()
      ? actionName
      : QStringLiteral("%1 for %2").arg(actionName, entityName);
  this->appendLogMessage(
      QStringLiteral("Context menu placeholder requested: %1").arg(targetLabel));
  this->_ui->statusLabel->setText(
      QStringLiteral("%1 aun no esta implementado en este menu contextual.")
          .arg(actionName));
}

void MainWindow::openObjectsContextMenu(const QPoint& position) {
  const QModelIndex index = this->_ui->objectsTreeView->indexAt(position);
  if (!index.isValid()) {
    return;
  }

  this->_ui->objectsTreeView->setCurrentIndex(index);
  if (!this->currentSelectionIsEntity() && !this->currentSelectionIsTacticalGraphic()) {
    return;
  }

  QMenu menu(this);
  if (this->currentSelectionIsEntity()) {
    this->populateEntityContextMenu(menu);
  } else {
    menu.addAction(QStringLiteral("Delete Graphic"), this, &MainWindow::deleteSelectedEntity);
  }
  menu.exec(this->_ui->objectsTreeView->viewport()->mapToGlobal(position));
}

void MainWindow::assignFlyHeadingAltitudeSpeedTask() {
  this->_taskAssignmentController->assignFlyHeadingAltitudeSpeed();
}

void MainWindow::assignMoveToLocationTask() {
  this->_taskAssignmentController->assignMoveToLocation();
}

void MainWindow::assignWaitOnLocationTask() {
  this->_taskAssignmentController->assignWaitOnLocation();
}

void MainWindow::assignMoveToWaypointTask() {
  this->_taskAssignmentController->assignMoveToWaypoint();
}

void MainWindow::assignMoveAlongRouteTask() {
  this->_taskAssignmentController->assignMoveAlongRoute();
}

void MainWindow::assignReturnToBaseTask() {
  this->_taskAssignmentController->assignReturnToBase();
}

void MainWindow::assignPatrolRouteTask() {
  this->_taskAssignmentController->assignPatrolRoute();
}

void MainWindow::assignOrbitHoldLocationTask() {
  this->_taskAssignmentController->assignOrbitHoldLocation();
}

void MainWindow::assignHoldRacetrackTask() {
  this->_taskAssignmentController->assignHoldRacetrack();
}

void MainWindow::assignPatrolAreaTask() {
  this->_taskAssignmentController->assignPatrolArea();
}

void MainWindow::assignOrbitAreaTask() {
  this->_taskAssignmentController->assignOrbitArea();
}

void MainWindow::assignFollowEntityTask() {
  this->_taskAssignmentController->assignFollowEntity();
}

void MainWindow::assignInterceptEntityTask() {
  this->_taskAssignmentController->assignInterceptEntity();
}

void MainWindow::assignAttackOnceTask() {
  this->_taskAssignmentController->assignAttackOnce();
}

void MainWindow::assignAttackUntilDestroyedTask() {
  this->_taskAssignmentController->assignAttackUntilDestroyed();
}

void MainWindow::assignFireOnPositionTask() {
  this->_taskAssignmentController->assignFireOnPosition();
}

void MainWindow::assignFireInDirectionTask() {
  this->_taskAssignmentController->assignFireInDirection();
}

void MainWindow::assignStopWeaponsTask() {
  this->_taskAssignmentController->assignStopWeaponsTask();
}

void MainWindow::assignAttackAirTask() {
  this->_taskAssignmentController->assignAttackAir();
}

void MainWindow::assignAttackSurfaceTask() {
  this->_taskAssignmentController->assignAttackSurface();
}

void MainWindow::assignWaitUntilTargetDetectedTask() {
  this->_taskAssignmentController->assignWaitUntilTargetDetected();
}

void MainWindow::assignWaitUntilTargetDestroyedTask() {
  this->_taskAssignmentController->assignWaitUntilTargetDestroyed();
}

void MainWindow::assignWaitUntilDamagedTask() {
  this->_taskAssignmentController->assignWaitUntilDamaged();
}

void MainWindow::assignWaitUntilTimeTask() {
  this->_taskAssignmentController->assignWaitUntilTime();
}

void MainWindow::assignWaitUntilInRangeTask() {
  this->_taskAssignmentController->assignWaitUntilInRange();
}

void MainWindow::openEntityPlanDialog() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (this->_entityPlanDialog) {
    if (this->_entityPlanDialogEntityName == entityName) {
      this->_entityPlanDialog->show();
      this->_entityPlanDialog->raise();
      this->_entityPlanDialog->activateWindow();
      return;
    }
    this->_entityPlanDialog->close();
    this->_entityPlanDialog = nullptr;
    this->_entityPlanDialogEntityName.clear();
  }

  auto* dialog = new presentation::EntityPlanDialog(
      entityName,
      this->_scenarioState,
      this->_planExecutor.get(),
      [this](const QString& en, PlanStepKind kind, PlanStep& step) {
        return this->configurePlanStep(en, kind, step);
      },
      [this](const QString& msg) { this->appendLogMessage(msg); },
      [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
      [this]() { this->syncScenarioStateToUi(); },
      this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  this->_entityPlanDialog = dialog;
  this->_entityPlanDialogEntityName = entityName;

  QObject::connect(
      dialog,
      &QObject::destroyed,
      this,
      [this, dialog]() {
        if (this->_entityPlanDialog == dialog) {
          this->_entityPlanDialog = nullptr;
          this->_entityPlanDialogEntityName.clear();
        }
      });

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void MainWindow::clearSelectedTask() {
  this->_scenarioObjectEditorController->clearSelectedTask();
}

void MainWindow::deleteSelectedEntity() {
  this->_scenarioObjectEditorController->deleteSelected();
}

void MainWindow::selectObjectByName(const QString& trackName, bool notifyMap) {
  QStandardItem* item = this->findTrackItemByName(this->_friendlyRootItem, trackName);
  if (!item) {
    item = this->findTrackItemByName(this->_opposingRootItem, trackName);
  }
  if (!item) {
    item = this->findTrackItemByName(this->_neutralRootItem, trackName);
  }
  if (!item) {
    return;
  }

  this->_ui->objectsTreeView->setCurrentIndex(item->index());
  this->_ui->objectsTreeView->scrollTo(item->index());

  if (notifyMap) {
    this->sendTrackToMap(item->data(kTrackSummaryRole).toMap(), true);
  }
}

QStandardItem* MainWindow::findTrackItemByName(QStandardItem* parent, const QString& trackName) const {
  if (!parent) {
    return nullptr;
  }

  for (int row = 0; row < parent->rowCount(); ++row) {
    QStandardItem* child = parent->child(row);
    if (!child) {
      continue;
    }

    const QVariantMap summary = child->data(kTrackSummaryRole).toMap();
    const QString entityReference =
        summary.value(QStringLiteral("entityId")).toString().trimmed().isEmpty()
            ? summary.value(QStringLiteral("name")).toString()
            : summary.value(QStringLiteral("entityId")).toString();
    if (entityReference == trackName) {
      return child;
    }

    if (QStandardItem* nested = this->findTrackItemByName(child, trackName)) {
      return nested;
    }
  }

  return nullptr;
}

const Entity* MainWindow::findEntityByName(const QString& entityName) const {
  for (const Entity& entity : this->_scenarioState->entities()) {
    if (domain::entityMatchesReference(entity, entityName)) {
      return &entity;
    }
  }
  return nullptr;
}

QString MainWindow::selectedEntityName() const {
  if (!this->currentSelectionIsEntity()) {
    return QString();
  }
  const QVariantMap summary =
      this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  const QString entityId = summary.value(QStringLiteral("entityId")).toString();
  return entityId.trimmed().isEmpty()
      ? summary.value(QStringLiteral("name")).toString()
      : entityId;
}

QString MainWindow::selectedObjectName() const {
  const QModelIndex currentIndex = this->_ui->objectsTreeView->currentIndex();
  if (!currentIndex.isValid()) {
    return QString();
  }
  const QVariantMap summary = currentIndex.data(kTrackSummaryRole).toMap();
  if (this->currentSelectionIsEntity()) {
    const QString entityId = summary.value(QStringLiteral("entityId")).toString();
    if (!entityId.trimmed().isEmpty()) {
      return entityId;
    }
  }
  return summary.value(QStringLiteral("name")).toString();
}

bool MainWindow::currentSelectionIsEntity() const {
  const QModelIndex currentIndex = this->_ui->objectsTreeView->currentIndex();
  if (!currentIndex.isValid()) {
    return false;
  }

  QStandardItem* item = this->_objectsModel->itemFromIndex(currentIndex);
  if (!item || item->rowCount() != 0 || !currentIndex.parent().isValid()) {
    return false;
  }
  return !this->_tacticalGraphicsRootItem || currentIndex.parent() != this->_tacticalGraphicsRootItem->index();
}

bool MainWindow::currentSelectionIsOperableEntity() const {
  return this->currentSelectionIsEntity() && !this->selectedEntityIsDestroyed();
}

bool MainWindow::selectedEntityIsDestroyed() const {
  if (!this->currentSelectionIsEntity()) {
    return false;
  }

  return this->_ui->objectsTreeView->currentIndex()
      .data(kTrackSummaryRole)
      .toMap()
      .value(QStringLiteral("destroyed"))
      .toBool();
}

bool MainWindow::currentSelectionIsTacticalGraphic() const {
  const QModelIndex currentIndex = this->_ui->objectsTreeView->currentIndex();
  if (!currentIndex.isValid() || !this->_tacticalGraphicsRootItem) {
    return false;
  }

  QStandardItem* item = this->_objectsModel->itemFromIndex(currentIndex);
  if (!item || item->rowCount() != 0 || !currentIndex.parent().isValid()) {
    return false;
  }
  return currentIndex.parent() == this->_tacticalGraphicsRootItem->index();
}

void MainWindow::queuePendingBombRelease(
    const QString& launcherEntityName,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters,
    const QString& targetLabel,
    const QString& sourceDescription,
    const QString& targetEntityName,
    bool logQueued,
    bool focusLauncher) {
  this->_bombReleaseController->queue(
      launcherEntityName, targetLatitude, targetLongitude, targetAltitudeMeters,
      targetLabel, sourceDescription, targetEntityName, logQueued, focusLauncher);
}

void MainWindow::clearPendingBombRelease() {
  this->_bombReleaseController->clearAll();
}

QString MainWindow::cleanupRuntimeReferencesForRemovedEntity(const QString& entityName) {
  const QString removedEntityName = entityName.trimmed();
  if (removedEntityName.isEmpty()) {
    return QString();
  }

  this->_attackTaskProcessor->removeEntity(removedEntityName);

  return this->_bombReleaseController->handleRemovedEntity(removedEntityName);
}


void MainWindow::validatePendingBombRelease() {
  this->_bombReleaseController->validate();
}

void MainWindow::processAttackTasks(double deltaSeconds) {
  _attackTaskProcessor->processAttackTasks(deltaSeconds, _simulationRunning);
}

void MainWindow::processAutoBombingBehaviors(double deltaSeconds) {
  _attackTaskProcessor->processAutoBombing(deltaSeconds, _simulationRunning);
}

void MainWindow::advanceSimulationTick(double deltaSeconds) {
  this->_scenarioState->advanceSimulation(deltaSeconds);
  this->advanceEntityPlans();
  this->processAttackTasks(deltaSeconds);
  this->processAutoBombingBehaviors(deltaSeconds);
  this->processPendingBombRelease();
  this->syncScenarioStateToUi();
}

void MainWindow::processPendingBombRelease() {
  this->_bombReleaseController->process();
}

void MainWindow::releaseBombAtSurfaceEntity() {
  this->_bombReleaseActionsController->releaseBombAtSurfaceEntity();
}

void MainWindow::queueBombReleaseAtEntity(const QString& launcherName, const Entity& target) {
  this->_bombReleaseController->queue(
      launcherName,
      target.latitude,
      target.longitude,
      static_cast<double>(target.altitude),
      target.name,
      QStringLiteral("Surface Entity"),
      target.name);
}

void MainWindow::releaseBombAtCustomCoordinates() {
  this->_bombReleaseActionsController->releaseBombAtCustomCoordinates();
}

void MainWindow::addBombTargetToQueue() {
  this->_bombReleaseActionsController->addBombTargetToQueue();
}

void MainWindow::addCustomBombTargetToQueue() {
  this->_bombReleaseActionsController->addCustomBombTargetToQueue();
}

void MainWindow::clearBombTargetQueue() {
  this->_bombReleaseActionsController->clearBombTargetQueue();
}

void MainWindow::cancelPendingBombRelease() {
  this->_bombReleaseActionsController->cancelPendingBombRelease();
}

void MainWindow::openAssignTaskDialog(const QString& initialTaskType) {
  this->_assignTaskController->open(initialTaskType);
}

void MainWindow::populateTaskCommands() {
  this->_ui->tasksListWidget->clear();

  static const struct { const char* label; const char* type; } kTaskEntries[] = {
      { "Movement: Fly Heading / Altitude / Speed...", "FlyHeadingAltitudeSpeed" },
      { "Movement: Move To Location...",               "MoveToLocation"          },
      { "Movement: Wait on Location...",               "WaitOnLocation"          },
      { "Movement: Move To Waypoint...",               "MoveToWaypoint"          },
      { "Movement: Follow Route...",                   "FollowRoute"             },
      { "Movement: Patrol Area...",                    "PatrolArea"              },
      { "Movement: Orbit Area...",                     "OrbitArea"               },
      { "Movement: Hold Racetrack...",                 "HoldRacetrack"           },
      { "Movement: Follow Entity...",                  "FollowEntity"            },
      { "Movement: Intercept Entity...",               "InterceptEntity"          },
      { "Attack: Attack Once...",                      "AttackOnce"              },
      { "Attack: Attack Until Destroyed...",           "AttackUntilDestroyed"    },
      { "Attack: Fire on Position...",                 "FireOnPosition"          },
      { "Attack: Fire in Direction...",                "FireInDirection"         },
      { "Attack: Stop Weapons Task",                   "StopWeaponsTask"         },
      { "Attack: Attack Air...",                       "AttackAir"               },
      { "Attack: Attack Surface...",                   "AttackSurface"           },
      { "Conditional: Wait Until Target Detected...",  "WaitUntilTargetDetected" },
      { "Conditional: Wait Until Target Destroyed...", "WaitUntilTargetDestroyed"},
      { "Conditional: Wait Until Damaged...",          "WaitUntilDamaged"        },
      { "Conditional: Wait Until Time...",             "WaitUntilTime"           },
      { "Conditional: Wait Until In Range...",         "WaitUntilInRange"        },
      { "Other: Clear Current Task",                   "ClearTask"               },
  };

  for (const auto& entry : kTaskEntries) {
    auto* item = new QListWidgetItem(QString::fromLatin1(entry.label));
    item->setData(Qt::UserRole, QString::fromLatin1(entry.type));
    this->_ui->tasksListWidget->addItem(item);
  }
}

void MainWindow::rebuildTacticalGraphicsTree() {
  if (!this->_tacticalGraphicsRootItem) {
    return;
  }

  this->_tacticalGraphicsRootItem->removeRows(0, this->_tacticalGraphicsRootItem->rowCount());

  for (const Waypoint& waypoint : this->_scenarioState->waypoints()) {
    auto* waypointItem = new QStandardItem(waypoint.name);
    waypointItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Waypoint")));
    setTrackData(waypointItem, presentation::makeWaypointTrackSummary(waypoint));
    this->_tacticalGraphicsRootItem->appendRow(waypointItem);
  }

  for (const RouteGraphic& route : this->_scenarioState->routes()) {
    auto* routeItem = new QStandardItem(route.name);
    routeItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Route")));
    setTrackData(routeItem, presentation::makeRouteTrackSummary(route));
    this->_tacticalGraphicsRootItem->appendRow(routeItem);
  }

  for (const AreaDefinition& area : this->_scenarioState->areas()) {
    auto* areaItem = new QStandardItem(area.name);
    areaItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Engagement Area")));
    setTrackData(areaItem, presentation::makeAreaTrackSummary(area));
    this->_tacticalGraphicsRootItem->appendRow(areaItem);
  }

  if (this->_tacticalGraphicsRootItem->rowCount() > 0) {
    this->_ui->objectsTreeView->expand(this->_tacticalGraphicsRootItem->index());
  }
}

void MainWindow::beginTaskCoordinatePick() {
  if (!this->_taskDialog) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay una task esperando coordenadas."));
    return;
  }
  this->_entityCoordinatePickPending = false;
  this->_taskCoordinatePickPending = true;
  this->_bombReleaseController->cancelPickMode();
  this->beginMapCoordinatePick();
}

void MainWindow::beginBombCoordinatePick() {
  this->_entityCoordinatePickPending = false;
  this->_taskCoordinatePickPending = false;
  this->beginMapCoordinatePick();
}

void MainWindow::beginGraphicCoordinatePick() {
  this->_entityCoordinatePickPending = false;
  this->_taskCoordinatePickPending = false;
  this->_bombReleaseController->cancelPickMode();
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    this->_ui->statusLabel->setText(
        QStringLiteral("El visor de CesiumJS no esta disponible ahora mismo."));
    return;
  }

  if (this->_entityDialog) {
    this->_entityDialog->hide();
  }
  if (this->_taskDialog) {
    this->_taskDialog->hide();
  }

  this->_webView->page()->runJavaScript(
      QStringLiteral("window.beginQtCoordinatePick && window.beginQtCoordinatePick();"));
#else
  this->_ui->statusLabel->setText(
      QStringLiteral("Qt WebEngine no esta disponible en este build."));
#endif
}

void MainWindow::openAddWaypointDialog() {
  this->_tacticalGraphicsEditorController->openAddWaypointDialog();
}

void MainWindow::openAddRouteDialog() {
  this->_tacticalGraphicsEditorController->openAddRouteDialog();
}

void MainWindow::openAddAreaDialog() {
  this->_tacticalGraphicsEditorController->openAddAreaDialog();
}

void MainWindow::updateSimulationControls() {
  this->_ui->actionStartSimulation->setEnabled(!this->_simulationRunning);
  this->_ui->actionPauseSimulation->setEnabled(this->_simulationRunning);
  this->_ui->actionStopSimulation->setEnabled(this->_simulationRunning || !this->_scenarioState->entities().isEmpty());
  this->_ui->modeBadgeLabel->setText(
      this->_simulationRunning
          ? QStringLiteral("Simulation Running")
          : QStringLiteral("Simulation Paused"));
}
