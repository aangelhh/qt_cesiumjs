#include "MainWindow.h"

#include "AddEntityDialog.h"
#include "AssignTaskDialog.h"
#include "EntityDetailsDialog.h"
#include "application/ScenarioState.h"
#include "application/SimulationEngine.h"
#include "application/Command.h"
#include "application/ScenarioQueries.h"
#include "application/TaskApplicator.h"
#include "domain/BombReleaseGate.h"
#include "presentation/BombReleaseController.h"
#include "presentation/EntityPlanExecutor.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/GeoMath.h"
#include "infrastructure/CesiumScenePage.h"
#include "infrastructure/MapBridge.h"
#include "infrastructure/ModelCatalog.h"
#include "presentation/EntityTextFormatter.h"
#include "presentation/EntityHomePositionTracker.h"
#include "presentation/EntityStatusFormatter.h"
#include "presentation/EntityVisualStateManager.h"
#include "presentation/TrackIconProvider.h"
#include "presentation/TrackSummaryBuilder.h"
#include "ui_MainWindow.h"

#include <QAbstractItemView>
#include <QAction>
#include <QActionGroup>
#include <QDateTime>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QColor>
#include <QDialog>
#include <QEventLoop>
#include <QFrame>
#include <QHeaderView>
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
#include <QStandardItemModel>
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
constexpr double kOrbitHoldDefaultRadiusMeters = 1500.0;
// Bomb release constants now in domain/BombReleaseGate.h
// Attack timing constants now in application/AttackTaskProcessor.h

// planStatusDisplayLabel moved to domain/CombatRules.h

// attackTaskStatusIsTerminal, attackSurfaceCoordinatesAreUsable,
// autoBehaviorDamageReactionLevel, autoBehaviorCanEngageByDamage
// moved to domain/CombatRules.h


// activeMissileInFlightForTarget moved to application/ScenarioQueries.h


// autoBehaviorDamageReactionLevel, autoBehaviorCanEngageByDamage
// moved to domain/CombatRules.h

QStringList behaviorModeOptions() {
  return {
      QStringLiteral("Manual"),
      QStringLiteral("Aggressive"),
      QStringLiteral("Defensive"),
      QStringLiteral("Patrol"),
  };
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

QJsonObject mapToJsonObject(const QVariantMap& map) {
  return QJsonObject::fromVariantMap(map);
}
// forceColorFromLabel, categoryGlyph, makeTacticalGraphicIcon, makeTrackIcon
// moved to presentation/TrackIconProvider.h

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _contentWidget(nullptr),
      _taskQuickBar(nullptr),
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
      _entityVisualStateManager(std::make_unique<presentation::EntityVisualStateManager>(
          QDir(projectRootPath()).absoluteFilePath(QStringLiteral("Data/entity_visual_state.json")))),
      _entityHomePositionTracker(std::make_unique<presentation::EntityHomePositionTracker>()),
      _graphicPickCoordinator(std::make_unique<presentation::GraphicPickCoordinator>(
          _scenarioState,
          [this](const QString& msg) { this->_ui->statusLabel->setText(msg); },
          [this]() { this->syncScenarioStateToUi(); },
          [this](const QVariantMap& draft) { this->sendDraftGraphicToMap(draft); },
          [this](const QString& name) { this->clearDraftGraphicFromMap(name); },
          [this]() { this->beginGraphicCoordinatePick(); }))
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
      , _webView(nullptr)
#endif
{
  this->_ui->setupUi(this);
  this->_ui->viewerHost->setMinimumSize(960, 640);
  this->_ui->viewerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->initializeModels();
  this->populateTaskCommands();
  this->_entityVisualStateManager->load();
  {
    QSet<QString> validNames;
    for (const Entity& entity : this->_scenarioState->entities()) {
      validNames.insert(entity.name);
    }
    this->_entityVisualStateManager->pruneTo(validNames);
  }
  for (const Entity& entity : this->_scenarioState->entities()) {
    this->appendEntityToUi(entity);
  }
  this->syncDetectedContactsToUi();
  _simulationTimer->setInterval(33);
  QObject::connect(_simulationTimer, &QTimer::timeout, this, [this]() {
    this->_scenarioState->advanceSimulation(0.033);
    this->advanceEntityPlans();
    this->processAttackTasks(0.033);
    this->processAutoBombingBehaviors(0.033);
    this->processPendingBombRelease();
    this->syncScenarioStateToUi();
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
        } else if (taskType == QStringLiteral("MoveToWaypoint")) {
          this->assignMoveToWaypointTask();
        } else if (taskType == QStringLiteral("MoveAlongRoute")) {
          this->assignMoveAlongRouteTask();
        } else if (taskType == QStringLiteral("PatrolArea")) {
          this->assignPatrolAreaTask();
        } else if (taskType == QStringLiteral("OrbitArea")) {
          this->assignOrbitAreaTask();
        } else if (taskType == QStringLiteral("FollowEntity")) {
          this->assignFollowEntityTask();
        } else if (taskType == QStringLiteral("AttackAir")) {
          this->assignAttackAirTask();
        } else if (taskType == QStringLiteral("AttackSurface")) {
          this->assignAttackSurfaceTask();
        } else if (taskType == QStringLiteral("ClearTask")) {
          this->clearSelectedTask();
        }
      });

  const QString accessToken = []() {
    const QString configured =
        CesiumScenePage::readConfigValue(QStringLiteral("ion_access_token"));
    return configured.isEmpty() ? CesiumScenePage::defaultAccessToken()
                                : configured;
  }();
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
  delete this->_scenarioState;
  delete this->_ui;
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
  const QString operationalState =
      taskType == QStringLiteral("-") || taskType == QStringLiteral("No current tasks")
          ? status
          : QStringLiteral("%1 (%2)").arg(taskType, taskStatus);
  const Entity* selectedEntity = this->findEntityByName(name);
  this->_ui->selectionStateValueLabel->setWordWrap(false);
  this->_ui->selectionStateValueLabel->setStyleSheet(QString());
  this->_ui->selectionStateValueLabel->setText(operationalState);
  this->_ui->selectionPositionValueLabel->setText(position);
  this->_ui->operationalStatusPlainTextEdit->setPlainText(
      this->buildSelectedEntityOperationalStatus(summary, selectedEntity));
}

QString MainWindow::buildSelectedEntityOperationalStatus(
    const QVariantMap& summary,
    const Entity* entity) const {
  const presentation::EntityStatusContext ctx{
      this->_scenarioState->entities(),
      this->_scenarioState->activeMunitions(),
      this->_bombReleaseController->pendingRelease(),
      this->_planExecutor->plans(),
      this->_simulationRunning};
  return presentation::buildEntityOperationalStatus(summary, entity, ctx);
}

void MainWindow::initializeModels() {
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

  this->_ui->eventLogPlainTextEdit->clear();
  this->appendLogMessage(QStringLiteral("Operational log ready."));
  this->appendLogMessage(QStringLiteral("Cesium map connected."));
  this->appendLogMessage(QStringLiteral("Awaiting commands..."));

  this->_ui->objectsTreeView->clearSelection();
  this->setSelectedTrackDetails(QVariantMap{});
  this->rebuildTacticalGraphicsTree();
}

void MainWindow::appendEntityToUi(const Entity& entity) {
  this->_entityHomePositionTracker->remember(entity);

  QStandardItem* rootItem = this->rootItemForForceIdentifier(entity.forceIdentifier);
  if (!rootItem) {
    return;
  }

  QString category = entity.category.trimmed();
  if (category != QStringLiteral("Fighter") &&
      category != QStringLiteral("Bomber") &&
      category != QStringLiteral("Helicopter") &&
      category != QStringLiteral("Transport") &&
      category != QStringLiteral("Tank") &&
      category != QStringLiteral("Truck") &&
      category != QStringLiteral("ArmoredVehicle") &&
      category != QStringLiteral("Armored Vehicle") &&
      category != QStringLiteral("Radar") &&
      category != QStringLiteral("SAMLauncher") &&
      category != QStringLiteral("SAM Launcher")) {
    category = QStringLiteral("Other");
  }

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

  auto* item = new QStandardItem(entity.name);
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
      this->_entityVisualStateManager->stateFor(entity.name);
  const QString damageState = entity.damageStateLabel();

  QVariantMap summary = presentation::makeTrackSummary(
      entity.name,
      entity.type,
      domain::forceIdentifierLabel(entity.forceIdentifier),
      QStringLiteral("%1 m").arg(entity.altitude),
      domain::formatPosition(entity.latitude, entity.longitude),
      damageState,
      entity.latitude,
      entity.longitude);
  summary.insert(QStringLiteral("domain"), entity.domain);
  summary.insert(QStringLiteral("category"), entity.category);
  summary.insert(QStringLiteral("callsign"), entity.callsign);
  summary.insert(QStringLiteral("forceIdentifier"), entity.forceIdentifier);
  summary.insert(QStringLiteral("entityKind"), entity.entityKind);
  summary.insert(QStringLiteral("entityDomain"), entity.entityDomain);
  summary.insert(QStringLiteral("entityCountry"), entity.entityCountry);
  summary.insert(QStringLiteral("entityCategory"), entity.entityCategory);
  summary.insert(QStringLiteral("entitySubcategory"), entity.entitySubcategory);
  summary.insert(QStringLiteral("entitySpecific"), entity.entitySpecific);
  summary.insert(QStringLiteral("entityExtra"), entity.entityExtra);
  summary.insert(QStringLiteral("entityTypeCode"), entity.entityTypeCode);
  summary.insert(QStringLiteral("modelName"), entity.modelName);
  summary.insert(QStringLiteral("modelUri"), entity.modelUri);
  summary.insert(QStringLiteral("headingDegrees"), entity.headingDegrees);
  summary.insert(QStringLiteral("pitchDegrees"), entity.pitchDegrees);
  summary.insert(QStringLiteral("rollDegrees"), entity.rollDegrees);
  summary.insert(QStringLiteral("flightDynamicsEnabled"), entity.flightDynamicsEnabled);
  summary.insert(QStringLiteral("flightDynamicsMode"), entity.flightDynamicsMode);
  summary.insert(QStringLiteral("jsbsimAircraftModel"), entity.jsbsimAircraftModel);
  summary.insert(QStringLiteral("speedKnots"), entity.speedKnots);
  summary.insert(QStringLiteral("verticalSpeedMetersPerSecond"), entity.verticalSpeedMetersPerSecond);
  summary.insert(QStringLiteral("destroyed"), entity.destroyed);
  summary.insert(QStringLiteral("damagePercent"), entity.damagePercent);
  summary.insert(QStringLiteral("damageState"), damageState);
  summary.insert(QStringLiteral("behaviorMode"), entity.behaviorMode.trimmed().isEmpty()
      ? QStringLiteral("Manual")
      : entity.behaviorMode);
  summary.insert(QStringLiteral("behaviorTargetEntityName"), entity.behaviorTargetEntityName);
  summary.insert(QStringLiteral("hidden"), visualState.hidden);
  summary.insert(QStringLiteral("radarCoverageVisible"), visualState.radarCoverageVisible);
  summary.insert(QStringLiteral("trackHistoryVisible"), visualState.trackHistoryVisible);
  summary.insert(QStringLiteral("taskType"), entity.currentTask.taskType);
  summary.insert(QStringLiteral("taskEnabled"), entity.currentTask.enabled);
  summary.insert(QStringLiteral("taskStatus"), entity.currentTask.status);
  summary.insert(QStringLiteral("taskTargetHeadingDegrees"), entity.currentTask.targetHeadingDegrees);
  summary.insert(QStringLiteral("taskTargetAltitudeMeters"), entity.currentTask.targetAltitudeMeters);
  summary.insert(QStringLiteral("taskTargetSpeedKnots"), entity.currentTask.targetSpeedKnots);
  summary.insert(QStringLiteral("taskTargetLatitude"), entity.currentTask.targetLatitude);
  summary.insert(QStringLiteral("taskTargetLongitude"), entity.currentTask.targetLongitude);
  summary.insert(QStringLiteral("taskTargetEntityName"), entity.currentTask.targetEntityName);
  summary.insert(QStringLiteral("taskTargetWaypointName"), entity.currentTask.targetWaypointName);
  summary.insert(QStringLiteral("taskTargetRouteName"), entity.currentTask.targetRouteName);
  summary.insert(QStringLiteral("taskTargetAreaName"), entity.currentTask.targetAreaName);
  summary.insert(QStringLiteral("taskTargetAreaRadiusMeters"), entity.currentTask.targetAreaRadiusMeters);
  summary.insert(QStringLiteral("sensorCount"), entity.sensors.size());
  summary.insert(QStringLiteral("contactCount"), entity.sensorContacts.size());

  QVariantList sensors;
  for (const SensorDefinition& sensor : entity.sensors) {
    sensors.push_back(QVariantMap{
        {QStringLiteral("id"), sensor.id},
        {QStringLiteral("name"), sensor.name},
        {QStringLiteral("sensorType"), sensor.sensorType},
        {QStringLiteral("enabled"), sensor.enabled},
        {QStringLiteral("emitting"), sensor.emitting},
        {QStringLiteral("maxRangeMeters"), sensor.maxRangeMeters},
        {QStringLiteral("azimuthCenterDegrees"), sensor.azimuthCenterDegrees},
        {QStringLiteral("azimuthWidthDegrees"), sensor.azimuthWidthDegrees},
        {QStringLiteral("elevationCenterDegrees"), sensor.elevationCenterDegrees},
        {QStringLiteral("elevationWidthDegrees"), sensor.elevationWidthDegrees},
        {QStringLiteral("maxTracks"), sensor.maxTracks},
    });
  }
  summary.insert(QStringLiteral("sensors"), sensors);

  QVariantList contacts;
  for (const SensorContact& contact : entity.sensorContacts) {
    contacts.push_back(QVariantMap{
        {QStringLiteral("sensorId"), contact.sensorId},
        {QStringLiteral("targetEntityName"), contact.targetEntityName},
        {QStringLiteral("rangeMeters"), contact.rangeMeters},
        {QStringLiteral("bearingDegrees"), contact.bearingDegrees},
        {QStringLiteral("lineOfSight"), contact.lineOfSight},
        {QStringLiteral("detected"), contact.detected},
    });
  }
  summary.insert(QStringLiteral("sensorContacts"), contacts);
  return summary;
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
  QStringList names;
  QSet<QString> seen;
  for (const Waypoint& waypoint : this->_scenarioState->waypoints()) {
    const QString name = waypoint.name.trimmed();
    if (name.isEmpty() || seen.contains(name)) {
      continue;
    }
    seen.insert(name);
    names.append(name);
  }
  return names;
}

QStringList MainWindow::availableRouteNames(bool requirePoints) const {
  QStringList names;
  QSet<QString> seen;
  for (const RouteGraphic& route : this->_scenarioState->routes()) {
    const QString name = route.name.trimmed();
    if (name.isEmpty() || seen.contains(name)) {
      continue;
    }
    if (requirePoints && route.points.isEmpty()) {
      continue;
    }
    seen.insert(name);
    names.append(name);
  }
  return names;
}

QStringList MainWindow::availableAreaNames() const {
  QStringList names;
  QSet<QString> seen;
  for (const AreaDefinition& area : this->_scenarioState->areas()) {
    QString name = area.name.trimmed();
    if (name.isEmpty()) {
      name = area.id.trimmed();
    }
    if (name.isEmpty() || seen.contains(name)) {
      continue;
    }
    seen.insert(name);
    names.append(name);
  }
  return names;
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

  QStringList availableTargets;
  for (const Entity& entity : this->_scenarioState->entities()) {
    if (entity.name != entityName) {
      availableTargets.append(entity.name);
    }
  }

  const QStringList availableWaypoints = this->availableWaypointNames();
  const QStringList availableRoutes = this->availableRouteNames(false);
  const QStringList availableAreas = this->availableAreaNames();

  QPointer<AssignTaskDialog> dialog = new AssignTaskDialog(
      entityName,
      availableTargets,
      availableWaypoints,
      availableRoutes,
      availableAreas,
      initialTask,
      initialTaskType,
      this);
  dialog->setAttribute(Qt::WA_DeleteOnClose, false);
  dialog->setWindowModality(Qt::NonModal);
  this->_taskDialog = dialog;

  QEventLoop loop;
  bool accepted = false;

  QObject::connect(
      dialog,
      &QObject::destroyed,
      &loop,
      [this, &loop]() {
        this->_taskDialog = nullptr;
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
      [dialog, &accepted, &outTask, &loop](int result) {
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
  auto* dialog = new AddEntityDialog(modelCatalog, this);
  this->_entityDialog = dialog;

  QObject::connect(
      dialog,
      &QObject::destroyed,
      this,
      [this]() { this->_entityDialog = nullptr; });
  QObject::connect(
      dialog,
      &AddEntityDialog::pickOnMapRequested,
      this,
      &MainWindow::beginEntityCoordinatePick);
  QObject::connect(
      dialog,
      &QDialog::accepted,
      this,
      [this, dialog]() {
        const Entity entity = dialog->entity();
        const bool hasRadarSensor = std::any_of(
            entity.sensors.cbegin(),
            entity.sensors.cend(),
            [](const SensorDefinition& sensor) {
              return sensor.sensorType.compare(QStringLiteral("radar"), Qt::CaseInsensitive) == 0;
            });
        if (hasRadarSensor && !entity.name.trimmed().isEmpty() &&
            !this->_entityVisualStateManager->contains(entity.name)) {
          presentation::EntityVisualState& visualState =
              this->_entityVisualStateManager->ensureState(entity.name);
          visualState.radarCoverageVisible = true;
          visualState.trackHistoryVisible = false;
          this->_entityVisualStateManager->save();
        }
        this->_scenarioState->addEntity(entity);
        this->appendEntityToUi(entity);
        this->_ui->statusLabel->setText(
            EntityTextFormatter::statusMessage(entity));
      });

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void MainWindow::beginEntityCoordinatePick() {
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

void MainWindow::reportPickedCoordinate(double longitude, double latitude, double height) {
  if (this->_graphicPickCoordinator->handleCoordinate(longitude, latitude, height)) {
    return;
  }

  if (this->_bombReleaseController->isPickingMode()) {
    const QString launcherName = this->_bombReleaseController->pickingLauncherName();
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

    this->queuePendingBombRelease(
        launcherName,
        latitude,
        longitude,
        0.0,
        domain::attackPointLabel(latitude, longitude),
        QStringLiteral("Pick on map"));
    return;
  }

  if (this->_entityDialog) {
    this->_entityDialog->setPickedCoordinate(longitude, latitude, height);
    this->_entityDialog->show();
    this->_entityDialog->raise();
    this->_entityDialog->activateWindow();
  }
  if (this->_taskDialog) {
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
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("Coordenadas capturadas: lat %1, lon %2")
          .arg(latitude,  0, 'f', 5)
          .arg(longitude, 0, 'f', 5));
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

  auto* dialog = new EntityDetailsDialog(summary, this);
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
  if (this->_simulationRunning) {
    return;
  }

  this->_simulationRunning = true;
  this->_simulationTimer->start();
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (this->_webView) {
    this->_webView->page()->runJavaScript(
        QStringLiteral("window.refreshQtTrackedEntity && window.refreshQtTrackedEntity();"));
  }
#endif
  this->_ui->statusLabel->setText(QStringLiteral("Simulacion en marcha."));
  this->appendLogMessage(QStringLiteral("Simulation started."));
  this->updateSimulationControls();
}

void MainWindow::pauseSimulation() {
  if (!this->_simulationRunning) {
    return;
  }

  this->_simulationRunning = false;
  this->_simulationTimer->stop();
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(QStringLiteral("Simulacion en pausa."));
  this->appendLogMessage(QStringLiteral("Simulation paused."));
  this->updateSimulationControls();
}

void MainWindow::stopSimulation() {
  this->_simulationRunning = false;
  this->_simulationTimer->stop();
  this->_bombReleaseController->cancelPickMode();
  this->_attackTaskProcessor->autoBombReleaseCooldownSeconds().clear();
  this->clearPendingBombRelease();
  this->_scenarioState->stopMission();
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(QStringLiteral("Mision detenida. Todas las tasks han terminado."));
  this->appendLogMessage(QStringLiteral("Simulation stopped. Mission state cleared."));
  this->updateSimulationControls();
}

void MainWindow::updateSelectedTrackPanel(const QModelIndex& current, const QModelIndex&) {
  const QVariantMap summary = current.data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    this->setSelectedTrackDetails(QVariantMap{});
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
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
    if (!this->_applyingMapSelection) {
      clearQtTrackSelectionInMap(this->_webView);
    }
#endif
    this->updateTaskQuickBarState();
    return;
  }

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

  this->_ui->statusLabel->setText(
      QStringLiteral("Contacto seleccionado: %1 detecta a %2.")
          .arg(observerName, contactName));
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
  if (!this->_webView || summary.isEmpty()) {
    return;
  }

  const QJsonObject object = mapToJsonObject(summary);
  const QString json = QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
  const QString type = summary.value(QStringLiteral("type")).toString();
  if (type == QStringLiteral("Waypoint") || type == QStringLiteral("Route") ||
      type == QStringLiteral("Area")) {
    const QString script = QStringLiteral(
        "window.addOrUpdateQtGraphic && window.addOrUpdateQtGraphic(%1, %2);")
                               .arg(json, focus ? QStringLiteral("true") : QStringLiteral("false"));
    this->_webView->page()->runJavaScript(script);
    return;
  }
  const QString script = QStringLiteral(
      "window.addOrUpdateQtTrack && window.addOrUpdateQtTrack(%1, %2);")
                             .arg(json, focus ? QStringLiteral("true") : QStringLiteral("false"));
  this->_webView->page()->runJavaScript(script);
#else
  Q_UNUSED(summary)
  Q_UNUSED(focus)
#endif
}

void MainWindow::removeTrackFromMap(const QString& trackName) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView || trackName.trimmed().isEmpty()) {
    return;
  }

  const QString trackJson =
      QString::fromUtf8(QJsonDocument(QJsonArray{trackName}).toJson(QJsonDocument::Compact));
  const QString script = QStringLiteral(
      "(window.removeQtTrack && window.removeQtTrack(%1));"
      "(window.removeQtGraphic && window.removeQtGraphic(%1));")
                             .arg(trackJson.mid(1).chopped(1));
  this->_webView->page()->runJavaScript(script);
#else
  Q_UNUSED(trackName)
#endif
}

void MainWindow::sendDraftGraphicToMap(const QVariantMap& summary) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView || summary.isEmpty()) {
    return;
  }

  const QJsonObject object = mapToJsonObject(summary);
  const QString json = QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
  const QString script = QStringLiteral(
      "window.addOrUpdateQtDraftGraphic && window.addOrUpdateQtDraftGraphic(%1);")
                             .arg(json);
  this->_webView->page()->runJavaScript(script);
#else
  Q_UNUSED(summary)
#endif
}

void MainWindow::clearDraftGraphicFromMap(const QString& name) {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView || name.trimmed().isEmpty()) {
    return;
  }

  const QString json =
      QString::fromUtf8(QJsonDocument(QJsonArray{name}).toJson(QJsonDocument::Compact));
  const QString script = QStringLiteral(
      "window.removeQtDraftGraphic && window.removeQtDraftGraphic(%1);")
                             .arg(json.mid(1).chopped(1));
  this->_webView->page()->runJavaScript(script);
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

  QString selectedObserverName;
  QString selectedContactName;
  const QModelIndex currentIndex = this->_ui->contactsTableView->currentIndex();
  if (currentIndex.isValid()) {
    const QModelIndex rowIndex = currentIndex.sibling(currentIndex.row(), 0);
    selectedObserverName = rowIndex.data(kDetectedContactObserverRole).toString();
    selectedContactName = rowIndex.data(kDetectedContactTargetRole).toString();
  }

  this->_detectedContactsModel->removeRows(0, this->_detectedContactsModel->rowCount());

  const QVector<Entity>& entities = this->_scenarioState->entities();
  const auto findEntityByName = [&entities](const QString& name) -> const Entity* {
    for (const Entity& entity : entities) {
      if (entity.name == name) {
        return &entity;
      }
    }
    return nullptr;
  };

  const QString lastSeenText =
      QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
  QSet<QString> insertedPairs;
  int restoredRow = -1;

  for (const Entity& observer : entities) {
    for (const SensorContact& contact : observer.sensorContacts) {
      if (!contact.detected) {
        continue;
      }

      const QString pairKey =
          observer.name + QStringLiteral("::") + contact.targetEntityName;
      if (insertedPairs.contains(pairKey)) {
        continue;
      }

      const Entity* target = findEntityByName(contact.targetEntityName);
      if (!target) {
        continue;
      }

      insertedPairs.insert(pairKey);

      auto* observerItem = new QStandardItem(observer.name);
      observerItem->setData(observer.name, kDetectedContactObserverRole);
      observerItem->setData(contact.targetEntityName, kDetectedContactTargetRole);

      QList<QStandardItem*> rowItems{
          observerItem,
          new QStandardItem(target->name),
          new QStandardItem(domain::forceIdentifierLabel(target->forceIdentifier)),
          new QStandardItem(target->type.trimmed().isEmpty() ? target->category : target->type),
          new QStandardItem(
              QStringLiteral("%1 km").arg(contact.rangeMeters / 1000.0, 0, 'f', 1)),
          new QStandardItem(
              QStringLiteral("%1 deg").arg(contact.bearingDegrees, 0, 'f', 1)),
          new QStandardItem(QStringLiteral("%1 m").arg(target->altitude)),
          new QStandardItem(lastSeenText),
      };

      for (QStandardItem* item : rowItems) {
        if (item) {
          item->setEditable(false);
        }
      }

      this->_detectedContactsModel->appendRow(rowItems);

      if (observer.name == selectedObserverName &&
          contact.targetEntityName == selectedContactName) {
        restoredRow = this->_detectedContactsModel->rowCount() - 1;
      }
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
}

void MainWindow::syncScenarioStateToUi() {
  this->validatePendingBombRelease();
  const QString selectedEntityNameBeforeSync = this->selectedEntityName();
  bool selectedEntityRemoved = false;

  const auto clearQtObjectSelection = [this]() {
    if (QItemSelectionModel* selectionModel = this->_ui->objectsTreeView->selectionModel()) {
      selectionModel->clearSelection();
      selectionModel->clearCurrentIndex();
    }
  };

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

      const QString name = child->data(kTrackSummaryRole).toMap().value(QStringLiteral("name")).toString();
      bool exists = false;
      for (const Entity& entity : this->_scenarioState->entities()) {
        if (entity.name == name) {
          exists = true;
          break;
        }
      }
      if (!exists) {
        if (!name.isEmpty()) {
          this->removeTrackFromMap(name);
          if (name == selectedEntityNameBeforeSync) {
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

  if (selectedEntityRemoved) {
    this->_applyingMapSelection = true;
    clearQtObjectSelection();
    this->_applyingMapSelection = false;
    this->setSelectedTrackDetails(QVariantMap{});
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
    clearQtTrackSelectionInMap(this->_webView);
#endif
    this->_ui->statusLabel->setText(QStringLiteral("No hay entidad seleccionada."));
  }

  {
    QSet<QString> validNames;
    for (const Entity& entity : this->_scenarioState->entities()) {
      validNames.insert(entity.name);
    }
    this->_entityVisualStateManager->pruneTo(validNames);
    this->_entityHomePositionTracker->pruneTo(validNames);
  }
  this->pruneEntityPlans();

  for (const Entity& entity : this->_scenarioState->entities()) {
    this->_entityHomePositionTracker->remember(entity);

    QStandardItem* item = this->findTrackItemByName(this->_friendlyRootItem, entity.name);
    if (!item) {
      item = this->findTrackItemByName(this->_opposingRootItem, entity.name);
    }
    if (!item) {
      item = this->findTrackItemByName(this->_neutralRootItem, entity.name);
    }

    const QVariantMap summary = this->makeEntityTrackSummary(entity);
    if (!item) {
      this->appendEntityToUi(entity);
      continue;
    }

    setTrackData(item, summary);
    this->sendTrackToMap(summary, false);
    if (this->_ui->objectsTreeView->currentIndex() == item->index()) {
      this->setSelectedTrackDetails(summary);
    }
  }

  QSet<QString> currentMunitionTrackNames;
  for (const ActiveMunition& munition : this->_scenarioState->activeMunitions()) {
    currentMunitionTrackNames.insert(munition.id);
    this->sendTrackToMap(presentation::makeMunitionTrackSummary(munition), false);
  }

  for (const QString& previousName : this->_activeMunitionTrackNames) {
    if (!currentMunitionTrackNames.contains(previousName)) {
      this->removeTrackFromMap(previousName);
    }
  }
  this->_activeMunitionTrackNames = currentMunitionTrackNames;

  QSet<QString> currentEffectTrackNames;
  for (const TransientEffect& effect : this->_scenarioState->transientEffects()) {
    currentEffectTrackNames.insert(effect.id);
    this->sendTrackToMap(presentation::makeTransientEffectTrackSummary(effect), false);
  }

  for (const QString& previousName : this->_activeEffectTrackNames) {
    if (!currentEffectTrackNames.contains(previousName)) {
      this->removeTrackFromMap(previousName);
    }
  }
  this->_activeEffectTrackNames = currentEffectTrackNames;

  const QString pendingBombTargetTrackName = QStringLiteral("Bomb Target");
  const QString pendingBombTargetLineTrackName = QStringLiteral("Bomb Target Line");
  if (this->_bombReleaseController->pendingRelease().pending) {
    QString teamLabel = QStringLiteral("Friendly");
    QString releaseStateLabel = QStringLiteral("Armed");
    double distanceToBombTargetMeters = -1.0;
    if (const Entity* launcher =
            this->findEntityByName(this->_bombReleaseController->pendingRelease().launcherEntityName)) {
      teamLabel = domain::forceIdentifierLabel(launcher->forceIdentifier);
      distanceToBombTargetMeters = domain::distanceMeters(
          launcher->latitude,
          launcher->longitude,
          this->_bombReleaseController->pendingRelease().targetLatitude,
          this->_bombReleaseController->pendingRelease().targetLongitude);
      const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
          *launcher,
          this->_bombReleaseController->pendingRelease().targetLatitude,
          this->_bombReleaseController->pendingRelease().targetLongitude,
          this->_bombReleaseController->pendingRelease().targetAltitudeMeters);
      releaseStateLabel = evaluation.stateLabel();
      this->sendTrackToMap(
          presentation::makePendingBombTargetLineTrackSummary(
              *launcher,
              this->_bombReleaseController->pendingRelease().targetLatitude,
              this->_bombReleaseController->pendingRelease().targetLongitude,
              this->_bombReleaseController->pendingRelease().targetAltitudeMeters,
              teamLabel,
              releaseStateLabel),
          false);
    } else {
      this->removeTrackFromMap(pendingBombTargetLineTrackName);
    }
    this->sendTrackToMap(
        presentation::makePendingBombTargetTrackSummary(
            this->_bombReleaseController->pendingRelease().targetLabel,
            this->_bombReleaseController->pendingRelease().targetLatitude,
            this->_bombReleaseController->pendingRelease().targetLongitude,
            this->_bombReleaseController->pendingRelease().targetAltitudeMeters,
            teamLabel,
            releaseStateLabel,
            distanceToBombTargetMeters),
        false);
  } else {
    this->removeTrackFromMap(pendingBombTargetTrackName);
    this->removeTrackFromMap(pendingBombTargetLineTrackName);
  }

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

void MainWindow::createTaskQuickBar() {
  if (this->_taskQuickBar) {
    return;
  }

  auto* panel = new QFrame(this->_ui->viewerHost);
  panel->setObjectName(QStringLiteral("taskQuickBar"));
  panel->setFrameShape(QFrame::StyledPanel);
  panel->setStyleSheet(QStringLiteral(
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
      "}"
  ));

  auto* layout = new QHBoxLayout(panel);
  layout->setContentsMargins(6, 6, 6, 6);
  layout->setSpacing(3);

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
      QStringLiteral("MR"),
      QColor(QStringLiteral("#ff8c52")),
      QStringLiteral("Move Along Route"),
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
      QStringLiteral("Pattern Hold (Location)\nPlaceholder"),
      [this]() { this->showTaskQuickPlaceholder(QStringLiteral("Pattern Hold (Location)")); });
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

  panel->adjustSize();
  panel->show();
  panel->raise();
  this->_taskQuickBar = panel;
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
  const bool canUseWeapons = entity && domain::entityCanUseMissileActions(*entity);
  const int missileCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Missile")) : 0;
  const int bombCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Bomb")) : 0;
  const int detectedMissileTargetCount =
      entity ? application::detectedMissileTargetsInRange(this->_scenarioState, *entity).size() : 0;
  QMenu* taskMenu = menu.addMenu(QStringLiteral("Task"));
  QMenu* movementMenu = taskMenu->addMenu(QStringLiteral("Movement"));
  movementMenu->addAction(
      QStringLiteral("Fly Heading / Altitude / Speed..."),
      this,
      &MainWindow::assignFlyHeadingAltitudeSpeedTask);
  movementMenu->addAction(
      QStringLiteral("Move To Location..."),
      this,
      &MainWindow::assignMoveToLocationTask);
  movementMenu->addAction(
      QStringLiteral("Move To Waypoint..."),
      this,
      &MainWindow::assignMoveToWaypointTask);
  movementMenu->addAction(
      QStringLiteral("Move Along Route..."),
      this,
      &MainWindow::assignMoveAlongRouteTask);
  movementMenu->addAction(
      QStringLiteral("Patrol Route..."),
      this,
      &MainWindow::assignPatrolRouteTask);
  movementMenu->addAction(
      QStringLiteral("Orbit / Hold (Location)..."),
      this,
      &MainWindow::assignOrbitHoldLocationTask);
  movementMenu->addAction(
      QStringLiteral("Return To Base"),
      this,
      &MainWindow::assignReturnToBaseTask);
  movementMenu->addAction(
      QStringLiteral("Patrol Area..."),
      this,
      &MainWindow::assignPatrolAreaTask);
  movementMenu->addAction(
      QStringLiteral("Orbit Area..."),
      this,
      &MainWindow::assignOrbitAreaTask);
  movementMenu->addAction(
      QStringLiteral("Follow Entity..."),
      this,
      &MainWindow::assignFollowEntityTask);
  QMenu* attackTaskMenu = taskMenu->addMenu(QStringLiteral("Attack"));
  attackTaskMenu->addAction(
      QStringLiteral("Attack Air..."),
      this,
      &MainWindow::assignAttackAirTask);
  attackTaskMenu->addAction(
      QStringLiteral("Attack Surface..."),
      this,
      &MainWindow::assignAttackSurfaceTask);
  taskMenu->addSeparator();
  taskMenu->addAction(QStringLiteral("Clear Current Task"), this, &MainWindow::clearSelectedTask);
  taskMenu->setEnabled(!entityDestroyed);

  QMenu* setMenu = menu.addMenu(QStringLiteral("Set"));
  setMenu->addAction(QStringLiteral("Heading..."), this, &MainWindow::setSelectedEntityHeading);
  setMenu->addAction(QStringLiteral("Altitude..."), this, &MainWindow::setSelectedEntityAltitude);
  setMenu->addAction(QStringLiteral("Speed..."), this, &MainWindow::setSelectedEntitySpeed);
  setMenu->setEnabled(!entityDestroyed);

  QMenu* behaviorMenu = menu.addMenu(QStringLiteral("Behavior"));
  QActionGroup* behaviorGroup = new QActionGroup(behaviorMenu);
  behaviorGroup->setExclusive(true);
  const QString currentBehaviorMode = entity && !entity->behaviorMode.trimmed().isEmpty()
      ? entity->behaviorMode.trimmed()
      : QStringLiteral("Manual");
  for (const QString& behaviorMode : behaviorModeOptions()) {
    QAction* behaviorAction = behaviorMenu->addAction(behaviorMode);
    behaviorAction->setCheckable(true);
    behaviorAction->setChecked(
        behaviorMode.compare(currentBehaviorMode, Qt::CaseInsensitive) == 0);
    behaviorGroup->addAction(behaviorAction);
    QObject::connect(
        behaviorAction,
        &QAction::triggered,
        this,
        [this, behaviorMode]() {
          this->setSelectedEntityBehaviorMode(behaviorMode);
        });
  }
  behaviorMenu->setEnabled(entity && !entityDestroyed);

  QAction* planAction = menu.addAction(QStringLiteral("Plan..."), this, &MainWindow::openEntityPlanDialog);
  planAction->setEnabled(!entityDestroyed);

  QMenu* weaponsMenu = menu.addMenu(QStringLiteral("Weapons"));
  QAction* addMissileAction = weaponsMenu->addAction(
      QStringLiteral("Add Missile"),
      this,
      &MainWindow::addMissileToSelectedEntity);
  QAction* addBombAction = weaponsMenu->addAction(
      QStringLiteral("Add Bomb"),
      this,
      &MainWindow::addBombToSelectedEntity);
  QAction* launchMissileAction = weaponsMenu->addAction(
      QStringLiteral("Launch Missile (%1)").arg(missileCount),
      this,
      &MainWindow::launchMissileFromSelectedEntity);
  QAction* launchMissileAtAction = weaponsMenu->addAction(
      QStringLiteral("Launch Missile At..."),
      this,
      &MainWindow::launchMissileAtSelectedEntity);
  QAction* releaseBombAction = weaponsMenu->addAction(
      QStringLiteral("Release Bomb (%1)").arg(bombCount),
      this,
      &MainWindow::releaseBombFromSelectedEntity);
  QMenu* releaseBombAtMenu = weaponsMenu->addMenu(QStringLiteral("Release Bomb At..."));
  QAction* releaseBombAtSurfaceAction = releaseBombAtMenu->addAction(
      QStringLiteral("Surface Entity..."),
      this,
      &MainWindow::releaseBombAtSurfaceEntity);
  QAction* releaseBombAtCustomAction = releaseBombAtMenu->addAction(
      QStringLiteral("Custom Coordinates..."),
      this,
      &MainWindow::releaseBombAtCustomCoordinates);
  QAction* cancelBombReleaseAction = weaponsMenu->addAction(
      QStringLiteral("Cancel Bomb Release"),
      this,
      &MainWindow::cancelPendingBombRelease);
  addMissileAction->setEnabled(canUseWeapons);
  addBombAction->setEnabled(canUseWeapons);
  launchMissileAction->setEnabled(
      canUseWeapons && missileCount > 0 && this->_simulationRunning);
  launchMissileAtAction->setEnabled(
      canUseWeapons &&
      missileCount > 0 &&
      this->_simulationRunning &&
      detectedMissileTargetCount > 0);
  releaseBombAction->setEnabled(
      canUseWeapons && bombCount > 0 && this->_simulationRunning);
  releaseBombAtMenu->setEnabled(
      canUseWeapons && bombCount > 0 && this->_simulationRunning);
  releaseBombAtSurfaceAction->setEnabled(
      canUseWeapons && bombCount > 0 && this->_simulationRunning);
  releaseBombAtCustomAction->setEnabled(
      canUseWeapons && bombCount > 0 && this->_simulationRunning);
  cancelBombReleaseAction->setEnabled(
      this->_bombReleaseController->pendingRelease().pending &&
      entity &&
      this->_bombReleaseController->pendingRelease().launcherEntityName.compare(entity->name, Qt::CaseInsensitive) == 0);
  if (canUseWeapons && missileCount > 0 && this->_simulationRunning &&
      detectedMissileTargetCount <= 0) {
    const QString message =
        QStringLiteral("No detected air targets in missile range for %1.")
            .arg(entityName);
    launchMissileAtAction->setToolTip(message);
    launchMissileAtAction->setStatusTip(message);
  }

  menu.addSeparator();
  menu.addAction(QStringLiteral("Information..."), this, &MainWindow::openSelectedEntityDetails);
  menu.addAction(QStringLiteral("Focus / Track Camera"), this, &MainWindow::focusSelectedEntityInMap);
  menu.addSeparator();

  QAction* editAction = menu.addAction(QStringLiteral("Edit..."));
  QObject::connect(
      editAction,
      &QAction::triggered,
      this,
      [this]() { this->showContextMenuPlaceholder(QStringLiteral("Edit")); });

  menu.addAction(QStringLiteral("Delete"), this, &MainWindow::deleteSelectedEntity);

  QAction* hideAction = menu.addAction(QStringLiteral("Hide"));
  hideAction->setCheckable(true);
  hideAction->setChecked(
      this->_ui->objectsTreeView->currentIndex()
          .data(kTrackSummaryRole)
          .toMap()
          .value(QStringLiteral("hidden"))
          .toBool());
  QObject::connect(
      hideAction,
      &QAction::toggled,
      this,
      [this](bool hidden) { this->setSelectedEntityHidden(hidden); });

  if (entityDestroyed) {
    menu.addAction(QStringLiteral("Restore"), this, &MainWindow::restoreSelectedEntity);
  } else {
    menu.addAction(QStringLiteral("Destroyed"), this, &MainWindow::destroySelectedEntity);
  }

  menu.addSeparator();

  QAction* radarCoverageAction = menu.addAction(QStringLiteral("Show Radar Coverage"));
  radarCoverageAction->setCheckable(true);
  radarCoverageAction->setChecked(
      this->_ui->objectsTreeView->currentIndex()
          .data(kTrackSummaryRole)
          .toMap()
          .value(QStringLiteral("radarCoverageVisible"))
          .toBool());
  QObject::connect(
      radarCoverageAction,
      &QAction::toggled,
      this,
      [this](bool visible) { this->setSelectedEntityRadarCoverageVisible(visible); });

  QAction* trackHistoryAction = menu.addAction(QStringLiteral("Show Track History"));
  trackHistoryAction->setCheckable(true);
  trackHistoryAction->setChecked(
      this->_ui->objectsTreeView->currentIndex()
          .data(kTrackSummaryRole)
          .toMap()
          .value(QStringLiteral("trackHistoryVisible"))
          .toBool());
  QObject::connect(
      trackHistoryAction,
      &QAction::toggled,
      this,
      [this](bool visible) { this->setSelectedEntityTrackHistoryVisible(visible); });
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
  const bool hasRunningTaskTargets =
      summary.value(QStringLiteral("taskEnabled")).toBool() &&
      summary.value(QStringLiteral("taskStatus")).toString() == QStringLiteral("Running") &&
      !summary.value(QStringLiteral("taskType")).toString().trimmed().isEmpty();

  const double defaultHeading = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble()
      : entity->headingDegrees;
  const int defaultAltitudeMeters = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt()
      : entity->altitude;
  double defaultSpeedKnots = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetSpeedKnots")).toDouble()
      : entity->speedKnots;
  if (defaultSpeedKnots <= 0.0) {
    defaultSpeedKnots =
        entity->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0 ? 220.0 : 12.0;
  }

  step = PlanStep{};
  step.kind = kind;
  step.task.enabled = true;
  step.task.status = QStringLiteral("Queued");
  step.status = QStringLiteral("NotStarted");

  bool ok = false;
  switch (kind) {
    case PlanStepKind::MoveToLocation: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("MoveToLocation");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      initialTask.targetLatitude = entity->latitude;
      initialTask.targetLongitude = entity->longitude;
      initialTask.targetAltitudeMeters = defaultAltitudeMeters;
      initialTask.targetSpeedKnots = defaultSpeedKnots;
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("MoveToLocation"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Move To %1, %2")
                       .arg(step.task.targetLatitude, 0, 'f', 4)
                       .arg(step.task.targetLongitude, 0, 'f', 4);
      return true;
    }

    case PlanStepKind::MoveToWaypoint: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("MoveToWaypoint");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      initialTask.targetWaypointName = summary.value(QStringLiteral("taskTargetWaypointName")).toString();
      initialTask.targetAltitudeMeters = defaultAltitudeMeters;
      initialTask.targetSpeedKnots = defaultSpeedKnots;
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("MoveToWaypoint"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Move To Waypoint: %1").arg(step.task.targetWaypointName);
      return true;
    }

    case PlanStepKind::MoveAlongRoute: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("MoveAlongRoute");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      initialTask.targetRouteName = summary.value(QStringLiteral("taskTargetRouteName")).toString();
      initialTask.targetAltitudeMeters = defaultAltitudeMeters;
      initialTask.targetSpeedKnots = defaultSpeedKnots;
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("MoveAlongRoute"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Move Along Route: %1").arg(step.task.targetRouteName);
      return true;
    }

    case PlanStepKind::PatrolArea: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("PatrolArea");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      initialTask.targetAreaName = summary.value(QStringLiteral("taskTargetAreaName")).toString();
      initialTask.targetAltitudeMeters = defaultAltitudeMeters;
      initialTask.targetSpeedKnots = defaultSpeedKnots;
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("PatrolArea"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      if (const AreaDefinition* area = this->findAreaByNameOrId(step.task.targetAreaName)) {
        step.task.targetLatitude = area->centerLatitude;
        step.task.targetLongitude = area->centerLongitude;
        double radiusMeters = area->radiusMeters;
        if (radiusMeters <= 0.0) {
          if (area->areaType == QStringLiteral("Ellipse")) {
            radiusMeters = qMax(area->semiMinorAxisMeters, 100.0);
          } else if (!area->points.isEmpty()) {
            radiusMeters = 250.0;
          } else {
            radiusMeters = 500.0;
          }
        }
        step.task.targetAreaRadiusMeters = radiusMeters;
      }
      step.label = QStringLiteral("Patrol Area: %1").arg(step.task.targetAreaName);
      return true;
    }

    case PlanStepKind::FlyHeadingAltitudeSpeed: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("FlyHeadingAltitudeSpeed");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      initialTask.targetHeadingDegrees = defaultHeading;
      initialTask.targetAltitudeMeters = defaultAltitudeMeters;
      initialTask.targetSpeedKnots = defaultSpeedKnots;
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("FlyHeadingAltitudeSpeed"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Fly H%1 A%2 S%3")
                       .arg(step.task.targetHeadingDegrees, 0, 'f', 0)
                       .arg(step.task.targetAltitudeMeters)
                       .arg(step.task.targetSpeedKnots, 0, 'f', 0);
      return true;
    }

    case PlanStepKind::OrbitHoldLocation: {
      double centerLatitude = entity->latitude;
      double centerLongitude = entity->longitude;
      const QString centerMode = QInputDialog::getItem(
          this,
          QStringLiteral("Plan Step: Orbit / Hold (Location)"),
          QStringLiteral("Center"),
          QStringList{
              QStringLiteral("Current Position"),
              QStringLiteral("Custom Coordinates"),
          },
          0,
          false,
          &ok);
      if (!ok) {
        return false;
      }

      if (centerMode == QStringLiteral("Custom Coordinates")) {
        centerLatitude = QInputDialog::getDouble(
            this,
            QStringLiteral("Plan Step: Orbit / Hold (Location)"),
            QStringLiteral("Latitude"),
            centerLatitude,
            -90.0,
            90.0,
            6,
            &ok);
        if (!ok) {
          return false;
        }

        centerLongitude = QInputDialog::getDouble(
            this,
            QStringLiteral("Plan Step: Orbit / Hold (Location)"),
            QStringLiteral("Longitude"),
            centerLongitude,
            -180.0,
            180.0,
            6,
            &ok);
        if (!ok) {
          return false;
        }
      }

      step.task.taskType = QStringLiteral("OrbitArea");
      step.task.targetLatitude = centerLatitude;
      step.task.targetLongitude = centerLongitude;
      step.task.targetAltitudeMeters = defaultAltitudeMeters;
      step.task.targetSpeedKnots = defaultSpeedKnots;
      step.task.targetAreaRadiusMeters = kOrbitHoldDefaultRadiusMeters;
      step.label = QStringLiteral("Orbit / Hold at %1, %2")
                       .arg(centerLatitude, 0, 'f', 4)
                       .arg(centerLongitude, 0, 'f', 4);
      return true;
    }

    case PlanStepKind::ReturnToBase: {
      const presentation::EntityHomePosition homePosition = this->_entityHomePositionTracker->positionFor(entityName);
      step.task.taskType = QStringLiteral("MoveToLocation");
      step.task.targetLatitude = homePosition.valid ? homePosition.latitude : entity->latitude;
      step.task.targetLongitude = homePosition.valid ? homePosition.longitude : entity->longitude;
      step.task.targetAltitudeMeters =
          homePosition.valid ? homePosition.altitudeMeters : defaultAltitudeMeters;
      step.task.targetSpeedKnots = defaultSpeedKnots;
      step.label = QStringLiteral("Return To Base");
      return true;
    }

    case PlanStepKind::AttackAir: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("AttackAir");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("AttackAir"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      step.label = QStringLiteral("Attack Air: %1")
                       .arg(step.task.targetEntityName.trimmed().isEmpty()
                                ? QStringLiteral("-")
                                : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::AttackSurface: {
      EntityTask initialTask;
      initialTask.taskType = QStringLiteral("AttackSurface");
      initialTask.enabled = true;
      initialTask.status = QStringLiteral("Queued");
      initialTask.targetLatitude = entity->latitude;
      initialTask.targetLongitude = entity->longitude;
      initialTask.targetAltitudeMeters = 0;
      if (!this->captureTaskConfiguration(
              entityName,
              initialTask,
              QStringLiteral("AttackSurface"),
              step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      const QString targetName = step.task.targetEntityName.trimmed();
      step.label = targetName.isEmpty()
          ? QStringLiteral("Attack Surface: %1")
                .arg(domain::attackPointLabel(step.task.targetLatitude, step.task.targetLongitude))
          : QStringLiteral("Attack Surface: %1").arg(targetName);
      return true;
    }
  }

  return false;
}

bool MainWindow::startEntityPlan(const QString& entityName) {
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

  const QVariantMap summary = this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    return false;
  }

  const bool hasRunningTaskTargets =
      summary.value(QStringLiteral("taskEnabled")).toBool() &&
      summary.value(QStringLiteral("taskStatus")).toString() == QStringLiteral("Running") &&
      !summary.value(QStringLiteral("taskType")).toString().trimmed().isEmpty();

  const int currentEntityAltitudeMeters = application::entityAltitudeMeters(
      this->_scenarioState,
      summary.value(QStringLiteral("name")).toString());

  headingDegrees = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble()
      : summary.value(QStringLiteral("headingDegrees")).toDouble();
  altitudeMeters = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt()
      : currentEntityAltitudeMeters;
  speedKnots = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetSpeedKnots")).toDouble()
      : summary.value(QStringLiteral("speedKnots")).toDouble();
  return true;
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

  if (!this->_scenarioState->assignTask(entityName, task)) {
    return;
  }

  if (domain::TaskStack* stack = this->_scenarioState->getTaskStack(entityName)) {
    while (!stack->isEmpty()) {
      stack->pop();
    }
    stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
        headingDegrees,
        static_cast<double>(altitudeMeters),
        speedKnots));
  }

  if (m_simulationEngine) {
    m_simulationEngine->enqueueCommand(
        std::make_unique<application::CmdAssignFlyHeadingTask>(
            entityName,
            headingDegrees,
            static_cast<double>(altitudeMeters),
            speedKnots));
  }

  this->appendLogMessage(
      QStringLiteral("Set FlyHeadingAltitudeSpeed applied to %1 (hdg %2 deg, alt %3 m, spd %4 kts)")
          .arg(entityName)
          .arg(headingDegrees, 0, 'f', 1)
          .arg(altitudeMeters)
          .arg(speedKnots, 0, 'f', 1));
  this->syncScenarioStateToUi();
}

void MainWindow::setSelectedEntityHeading() {
  double headingDegrees = 0.0;
  int altitudeMeters = 0;
  double speedKnots = 0.0;
  if (!this->resolveSelectedEntityFlyTargets(headingDegrees, altitudeMeters, speedKnots)) {
    return;
  }

  bool ok = false;
  const double newHeadingDegrees = QInputDialog::getDouble(
      this,
      QStringLiteral("Set Heading"),
      QStringLiteral("Heading (deg)"),
      headingDegrees,
      0.0,
      360.0,
      1,
      &ok);
  if (!ok) {
    return;
  }

  this->applyFlyHeadingAltitudeSpeedTask(newHeadingDegrees, altitudeMeters, speedKnots);
}

void MainWindow::setSelectedEntityAltitude() {
  double headingDegrees = 0.0;
  int altitudeMeters = 0;
  double speedKnots = 0.0;
  if (!this->resolveSelectedEntityFlyTargets(headingDegrees, altitudeMeters, speedKnots)) {
    return;
  }

  bool ok = false;
  const double newAltitudeMeters = QInputDialog::getDouble(
      this,
      QStringLiteral("Set Altitude"),
      QStringLiteral("Altitude (m)"),
      static_cast<double>(altitudeMeters),
      0.0,
      60000.0,
      0,
      &ok);
  if (!ok) {
    return;
  }

  this->applyFlyHeadingAltitudeSpeedTask(
      headingDegrees,
      static_cast<int>(std::lround(newAltitudeMeters)),
      speedKnots);
}

void MainWindow::setSelectedEntitySpeed() {
  double headingDegrees = 0.0;
  int altitudeMeters = 0;
  double speedKnots = 0.0;
  if (!this->resolveSelectedEntityFlyTargets(headingDegrees, altitudeMeters, speedKnots)) {
    return;
  }

  bool ok = false;
  const double newSpeedKnots = QInputDialog::getDouble(
      this,
      QStringLiteral("Set Speed"),
      QStringLiteral("Speed (kts)"),
      speedKnots,
      0.0,
      2000.0,
      1,
      &ok);
  if (!ok) {
    return;
  }

  this->applyFlyHeadingAltitudeSpeedTask(headingDegrees, altitudeMeters, newSpeedKnots);
}

void MainWindow::setSelectedEntityBehaviorMode(const QString& behaviorMode) {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty() || this->selectedEntityIsDestroyed()) {
    return;
  }

  if (!this->_scenarioState->setEntityBehaviorMode(entityName, behaviorMode)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo cambiar el behavior mode de %1.")
            .arg(entityName));
    return;
  }

  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("%1 behavior mode: %2.")
          .arg(entityName, behaviorMode));
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
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (!this->_scenarioState->setEntityDestroyed(entityName, destroyed)) {
    return;
  }

  if (destroyed && this->_taskDialog) {
    this->_taskDialog->close();
  }

  this->appendLogMessage(
      QStringLiteral("Entity %1 %2")
          .arg(entityName, destroyed ? QStringLiteral("marked as destroyed")
                                     : QStringLiteral("restored")));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      destroyed
          ? QStringLiteral("Entidad destruida: %1").arg(entityName)
          : QStringLiteral("Entidad restaurada: %1").arg(entityName));
}

void MainWindow::setSelectedEntityHidden(bool hidden) {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  presentation::EntityVisualState visualState = this->_entityVisualStateManager->stateFor(entityName);
  if (visualState.hidden == hidden) {
    return;
  }

  visualState.hidden = hidden;
  if (!visualState.hidden &&
      !visualState.radarCoverageVisible &&
      !visualState.trackHistoryVisible) {
    this->_entityVisualStateManager->remove(entityName);
  } else {
    this->_entityVisualStateManager->ensureState(entityName) = visualState;
  }
  this->_entityVisualStateManager->save();

  this->appendLogMessage(
      QStringLiteral("Entity %1 %2")
          .arg(entityName, hidden ? QStringLiteral("hidden")
                                  : QStringLiteral("shown")));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      hidden
          ? QStringLiteral("Entidad oculta: %1").arg(entityName)
          : QStringLiteral("Entidad visible: %1").arg(entityName));
}

void MainWindow::setSelectedEntityRadarCoverageVisible(bool visible) {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  presentation::EntityVisualState visualState = this->_entityVisualStateManager->stateFor(entityName);
  if (visualState.radarCoverageVisible == visible) {
    return;
  }

  visualState.radarCoverageVisible = visible;
  if (!visualState.hidden &&
      !visualState.radarCoverageVisible &&
      !visualState.trackHistoryVisible) {
    this->_entityVisualStateManager->remove(entityName);
  } else {
    this->_entityVisualStateManager->ensureState(entityName) = visualState;
  }
  this->_entityVisualStateManager->save();

  this->appendLogMessage(
      QStringLiteral("Radar coverage %1 for %2")
          .arg(visible ? QStringLiteral("enabled") : QStringLiteral("disabled"),
               entityName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      visible
          ? QStringLiteral("Cobertura radar visible para %1").arg(entityName)
          : QStringLiteral("Cobertura radar oculta para %1").arg(entityName));
}

void MainWindow::setSelectedEntityTrackHistoryVisible(bool visible) {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  presentation::EntityVisualState visualState = this->_entityVisualStateManager->stateFor(entityName);
  if (visualState.trackHistoryVisible == visible) {
    return;
  }

  visualState.trackHistoryVisible = visible;
  if (!visualState.hidden &&
      !visualState.radarCoverageVisible &&
      !visualState.trackHistoryVisible) {
    this->_entityVisualStateManager->remove(entityName);
  } else {
    this->_entityVisualStateManager->ensureState(entityName) = visualState;
  }
  this->_entityVisualStateManager->save();

  this->appendLogMessage(
      QStringLiteral("Track history %1 for %2")
          .arg(visible ? QStringLiteral("enabled") : QStringLiteral("disabled"),
               entityName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      visible
          ? QStringLiteral("Historial de trayectoria visible para %1").arg(entityName)
          : QStringLiteral("Historial de trayectoria oculto para %1").arg(entityName));
}

void MainWindow::destroySelectedEntity() {
  this->setSelectedEntityDestroyed(true);
}

void MainWindow::restoreSelectedEntity() {
  this->setSelectedEntityDestroyed(false);
}

void MainWindow::addMissileToSelectedEntity() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (!this->_scenarioState->addMissileToEntity(entityName, 1)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo anadir un misil a %1.").arg(entityName));
    return;
  }

  this->appendLogMessage(QStringLiteral("Missile added to %1").arg(entityName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("Misil anadido a %1.").arg(entityName));
}

void MainWindow::addBombToSelectedEntity() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (!this->_scenarioState->addBombToEntity(entityName, 1)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo anadir una bomba a %1.").arg(entityName));
    return;
  }

  this->appendLogMessage(QStringLiteral("Bomb added to %1").arg(entityName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("Bomba anadida a %1.").arg(entityName));
}

void MainWindow::launchMissileFromSelectedEntity() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  const Entity* entity = this->findEntityByName(entityName);
  const int missileCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Missile")) : 0;

  if (missileCount <= 0) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay misiles disponibles en %1.").arg(entityName));
    return;
  }

  if (!this->_simulationRunning) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Arranca la simulacion para lanzar el misil."));
    return;
  }

  if (!this->_scenarioState->launchMissile(entityName)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo lanzar un misil desde %1.").arg(entityName));
    return;
  }

  this->appendLogMessage(QStringLiteral("Missile launched from %1").arg(entityName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("Misil lanzado desde %1. Quedan %2.")
          .arg(entityName)
          .arg(qMax(0, missileCount - 1)));
}

void MainWindow::releaseBombFromSelectedEntity() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  const Entity* entity = this->findEntityByName(entityName);
  const int bombCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Bomb")) : 0;

  if (bombCount <= 0) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay bombas disponibles en %1.").arg(entityName));
    return;
  }

  if (!this->_simulationRunning) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Arranca la simulacion para soltar la bomba."));
    return;
  }

  if (!this->_scenarioState->releaseBomb(entityName)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No se pudo soltar una bomba desde %1.").arg(entityName));
    return;
  }

  this->appendLogMessage(QStringLiteral("Bomb released from %1").arg(entityName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("Bomba soltada desde %1. Quedan %2.")
          .arg(entityName)
          .arg(qMax(0, bombCount - 1)));
}

void MainWindow::launchMissileAtSelectedEntity() {
  const QString launcherName = this->selectedEntityName();
  if (launcherName.isEmpty()) {
    return;
  }

  const Entity* launcher = this->findEntityByName(launcherName);
  const int missileCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Missile")) : 0;
  if (!launcher || missileCount <= 0) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay misiles disponibles en %1.").arg(launcherName));
    return;
  }

  if (!this->_simulationRunning) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Arranca la simulacion para lanzar el misil."));
    return;
  }

  const QVector<application::MissileTargetCandidate> targets =
      application::detectedMissileTargetsInRange(this->_scenarioState, *launcher);
  if (targets.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No detected air targets in missile range for %1.")
            .arg(launcherName));
    return;
  }

  QStringList options;
  QHash<QString, QString> targetNameByOption;
  for (const application::MissileTargetCandidate& candidate : targets) {
    if (!candidate.entity) {
      continue;
    }
    const QString option =
        presentation::missileTargetDisplayLabel(*candidate.entity, candidate.rangeMeters);
    options.push_back(option);
    targetNameByOption.insert(option, candidate.entity->name);
  }

  bool ok = false;
  const QString selectedOption = QInputDialog::getItem(
      this,
      QStringLiteral("Launch Missile At"),
      QStringLiteral("Target"),
      options,
      0,
      false,
      &ok);
  if (!ok || selectedOption.trimmed().isEmpty()) {
    return;
  }

  const QString targetName = targetNameByOption.value(selectedOption).trimmed();
  if (targetName.isEmpty()) {
    return;
  }

  if (!this->_scenarioState->launchMissileAt(launcherName, targetName)) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Target out of missile range for %1.")
            .arg(launcherName));
    return;
  }

  this->appendLogMessage(
      QStringLiteral("Missile launched from %1 at %2")
          .arg(launcherName, targetName));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("Misil lanzado desde %1 hacia %2. Quedan %3.")
          .arg(launcherName, targetName)
          .arg(qMax(0, missileCount - 1)));
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
  this->openAssignTaskDialog(QStringLiteral("FlyHeadingAltitudeSpeed"));
}

void MainWindow::assignMoveToLocationTask() {
  this->openAssignTaskDialog(QStringLiteral("MoveToLocation"));
}

void MainWindow::assignMoveToWaypointTask() {
  this->openAssignTaskDialog(QStringLiteral("MoveToWaypoint"));
}

void MainWindow::assignMoveAlongRouteTask() {
  this->openAssignTaskDialog(QStringLiteral("MoveAlongRoute"));
}

void MainWindow::assignReturnToBaseTask() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty() || !this->currentSelectionIsOperableEntity()) {
    return;
  }

  const QVariantMap summary = this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  const presentation::EntityHomePosition homePosition = this->_entityHomePositionTracker->positionFor(entityName);

  double headingDegrees = 0.0;
  int currentAltitudeMeters = 0;
  double speedKnots = summary.value(QStringLiteral("speedKnots")).toDouble();
  this->resolveSelectedEntityFlyTargets(headingDegrees, currentAltitudeMeters, speedKnots);

  EntityTask task;
  task.taskType = QStringLiteral("MoveToLocation");
  task.enabled = true;
  task.status = QStringLiteral("Running");
  task.targetLatitude = homePosition.valid
      ? homePosition.latitude
      : summary.value(QStringLiteral("latitude")).toDouble();
  task.targetLongitude = homePosition.valid
      ? homePosition.longitude
      : summary.value(QStringLiteral("longitude")).toDouble();
  task.targetAltitudeMeters = homePosition.valid
      ? homePosition.altitudeMeters
      : currentAltitudeMeters;
  task.targetSpeedKnots = speedKnots;

  if (!this->applyEntityTask(entityName, task)) {
    return;
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("RTB asignado a %1.").arg(entityName));
}

void MainWindow::assignPatrolRouteTask() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty() || !this->currentSelectionIsOperableEntity()) {
    return;
  }

  const QStringList availableRoutes = this->availableRouteNames(true);
  if (availableRoutes.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay rutas disponibles para Patrol Route."));
    return;
  }

  const QVariantMap summary = this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  const QString currentRouteName = summary.value(QStringLiteral("taskTargetRouteName")).toString();
  int routeIndex = availableRoutes.indexOf(currentRouteName);
  if (routeIndex < 0) {
    routeIndex = 0;
  }

  bool ok = false;
  const QString routeName = QInputDialog::getItem(
      this,
      QStringLiteral("Patrol Route"),
      QStringLiteral("Route"),
      availableRoutes,
      routeIndex,
      false,
      &ok);
  if (!ok || routeName.trimmed().isEmpty()) {
    return;
  }

  double headingDegrees = 0.0;
  int altitudeMeters = 0;
  double speedKnots = summary.value(QStringLiteral("speedKnots")).toDouble();
  this->resolveSelectedEntityFlyTargets(headingDegrees, altitudeMeters, speedKnots);

  EntityTask task;
  task.taskType = QStringLiteral("MoveAlongRoute");
  task.enabled = true;
  task.status = QStringLiteral("Running");
  task.targetRouteName = routeName;
  task.targetAltitudeMeters = altitudeMeters;
  task.targetSpeedKnots = speedKnots;

  if (!this->applyEntityTask(entityName, task)) {
    return;
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("Patrol Route asignado a %1 sobre %2.")
          .arg(entityName, routeName));
}

void MainWindow::assignOrbitHoldLocationTask() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty() || !this->currentSelectionIsOperableEntity()) {
    return;
  }

  const QVariantMap summary = this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  double centerLatitude = summary.value(QStringLiteral("latitude")).toDouble();
  double centerLongitude = summary.value(QStringLiteral("longitude")).toDouble();

  bool ok = false;
  const QString centerMode = QInputDialog::getItem(
      this,
      QStringLiteral("Orbit / Hold (Location)"),
      QStringLiteral("Center"),
      QStringList{
          QStringLiteral("Current Position"),
          QStringLiteral("Custom Coordinates"),
      },
      0,
      false,
      &ok);
  if (!ok) {
    return;
  }

  if (centerMode == QStringLiteral("Custom Coordinates")) {
    centerLatitude = QInputDialog::getDouble(
        this,
        QStringLiteral("Orbit / Hold (Location)"),
        QStringLiteral("Latitude"),
        centerLatitude,
        -90.0,
        90.0,
        6,
        &ok);
    if (!ok) {
      return;
    }

    centerLongitude = QInputDialog::getDouble(
        this,
        QStringLiteral("Orbit / Hold (Location)"),
        QStringLiteral("Longitude"),
        centerLongitude,
        -180.0,
        180.0,
        6,
        &ok);
    if (!ok) {
      return;
    }
  }

  double headingDegrees = 0.0;
  int altitudeMeters = 0;
  double speedKnots = summary.value(QStringLiteral("speedKnots")).toDouble();
  this->resolveSelectedEntityFlyTargets(headingDegrees, altitudeMeters, speedKnots);

  EntityTask task;
  task.taskType = QStringLiteral("OrbitArea");
  task.enabled = true;
  task.status = QStringLiteral("Running");
  task.targetLatitude = centerLatitude;
  task.targetLongitude = centerLongitude;
  task.targetAltitudeMeters = altitudeMeters;
  task.targetSpeedKnots = speedKnots;
  task.targetAreaRadiusMeters = kOrbitHoldDefaultRadiusMeters;

  if (!this->applyEntityTask(entityName, task)) {
    return;
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("Orbit / Hold asignado a %1 alrededor de %2, %3.")
          .arg(entityName)
          .arg(centerLatitude, 0, 'f', 4)
          .arg(centerLongitude, 0, 'f', 4));
}

void MainWindow::assignPatrolAreaTask() {
  this->openAssignTaskDialog(QStringLiteral("PatrolArea"));
}

void MainWindow::assignOrbitAreaTask() {
  this->openAssignTaskDialog(QStringLiteral("OrbitArea"));
}

void MainWindow::assignFollowEntityTask() {
  this->openAssignTaskDialog(QStringLiteral("FollowEntity"));
}

void MainWindow::assignAttackAirTask() {
  this->openAssignTaskDialog(QStringLiteral("AttackAir"));
}

void MainWindow::assignAttackSurfaceTask() {
  this->openAssignTaskDialog(QStringLiteral("AttackSurface"));
}

void MainWindow::openEntityPlanDialog() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  this->ensureEntityPlan(entityName);

  QDialog dialog(this);
  dialog.setWindowTitle(QStringLiteral("Plan for %1").arg(entityName));
  dialog.resize(520, 420);
  dialog.setWindowModality(Qt::WindowModal);

  auto planForEntity = [this, entityName]() -> EntityPlan& {
    return this->ensureEntityPlan(entityName);
  };

  auto* layout = new QVBoxLayout(&dialog);
  auto* stepsList = new QListWidget(&dialog);
  layout->addWidget(stepsList);
  auto* planStatusLabel = new QLabel(&dialog);
  layout->addWidget(planStatusLabel);

  auto* editButtonsLayout = new QHBoxLayout();
  auto* addButton = new QPushButton(QStringLiteral("Add"), &dialog);
  auto* removeButton = new QPushButton(QStringLiteral("Remove"), &dialog);
  auto* upButton = new QPushButton(QStringLiteral("Up"), &dialog);
  auto* downButton = new QPushButton(QStringLiteral("Down"), &dialog);
  editButtonsLayout->addWidget(addButton);
  editButtonsLayout->addWidget(removeButton);
  editButtonsLayout->addWidget(upButton);
  editButtonsLayout->addWidget(downButton);
  editButtonsLayout->addStretch(1);
  layout->addLayout(editButtonsLayout);

  auto* actionButtonsLayout = new QHBoxLayout();
  auto* runButton = new QPushButton(QStringLiteral("Run Plan"), &dialog);
  auto* stopButton = new QPushButton(QStringLiteral("Stop Plan"), &dialog);
  auto* closeButton = new QPushButton(QStringLiteral("Close"), &dialog);
  actionButtonsLayout->addWidget(runButton);
  actionButtonsLayout->addWidget(stopButton);
  actionButtonsLayout->addStretch(1);
  actionButtonsLayout->addWidget(closeButton);
  layout->addLayout(actionButtonsLayout);

  auto refreshList = [this, planForEntity, stepsList, planStatusLabel]() {
    const EntityPlan& plan = planForEntity();
    const int previousRow = stepsList->currentRow();
    stepsList->clear();
    planStatusLabel->setText(
        QStringLiteral("Plan Status: %1").arg(plan.status.trimmed().isEmpty()
            ? QStringLiteral("NotStarted")
            : domain::planStatusDisplayLabel(plan.status.trimmed())));
    for (int index = 0; index < plan.steps.size(); ++index) {
      const QString prefix =
          (plan.running && index == plan.currentStepIndex) ? QStringLiteral(">> ") : QString();
      stepsList->addItem(
          QStringLiteral("%1%2. [%3] %4")
              .arg(prefix)
              .arg(index + 1)
              .arg(plan.steps.at(index).status.trimmed().isEmpty()
                       ? QStringLiteral("NotStarted")
                       : plan.steps.at(index).status.trimmed())
              .arg(presentation::EntityPlanExecutor::planStepDisplayLabel(plan.steps.at(index))));
    }

    if (!plan.steps.isEmpty()) {
      stepsList->setCurrentRow(qBound(0, previousRow, plan.steps.size() - 1));
    }
  };

  auto refreshButtons = [this,
                         planForEntity,
                         entityName,
                         stepsList,
                         addButton,
                         removeButton,
                         upButton,
                         downButton,
                         runButton,
                         stopButton]() {
    const EntityPlan& plan = planForEntity();
    const int currentRow = stepsList->currentRow();
    const bool hasSelection = currentRow >= 0 && currentRow < plan.steps.size();
    const bool editable = !plan.running;
    const Entity* entity = this->findEntityByName(entityName);

    addButton->setEnabled(editable);
    removeButton->setEnabled(editable && hasSelection);
    upButton->setEnabled(editable && hasSelection && currentRow > 0);
    downButton->setEnabled(
        editable &&
        hasSelection &&
        currentRow >= 0 &&
        currentRow < plan.steps.size() - 1);
    runButton->setEnabled(editable && entity && !entity->destroyed && !plan.steps.isEmpty());
    stopButton->setEnabled(plan.running);
  };

  QObject::connect(stepsList, &QListWidget::currentRowChanged, &dialog, refreshButtons);

  QObject::connect(addButton, &QPushButton::clicked, &dialog, [this, &dialog, planForEntity, entityName, stepsList, refreshList, refreshButtons, addButton]() {
    if (planForEntity().running) {
      this->_ui->statusLabel->setText(
          QStringLiteral("No puedes editar steps mientras el plan esta en ejecucion."));
      refreshButtons();
      return;
    }

    QMenu addMenu(&dialog);
    QAction* moveToLocationAction = addMenu.addAction(QStringLiteral("Move To Location"));
    moveToLocationAction->setData(static_cast<int>(PlanStepKind::MoveToLocation));
    QAction* moveToWaypointAction = addMenu.addAction(QStringLiteral("Move To Waypoint"));
    moveToWaypointAction->setData(static_cast<int>(PlanStepKind::MoveToWaypoint));
    QAction* moveAlongRouteAction = addMenu.addAction(QStringLiteral("Move Along Route"));
    moveAlongRouteAction->setData(static_cast<int>(PlanStepKind::MoveAlongRoute));
    QAction* patrolAreaAction = addMenu.addAction(QStringLiteral("Patrol Area"));
    patrolAreaAction->setData(static_cast<int>(PlanStepKind::PatrolArea));
    QAction* flyAction = addMenu.addAction(QStringLiteral("Fly Heading / Altitude / Speed"));
    flyAction->setData(static_cast<int>(PlanStepKind::FlyHeadingAltitudeSpeed));
    QAction* orbitAction = addMenu.addAction(QStringLiteral("Orbit / Hold (Location)"));
    orbitAction->setData(static_cast<int>(PlanStepKind::OrbitHoldLocation));
    QAction* rtbAction = addMenu.addAction(QStringLiteral("Return To Base"));
    rtbAction->setData(static_cast<int>(PlanStepKind::ReturnToBase));
    addMenu.addSeparator();
    QAction* attackAirAction = addMenu.addAction(QStringLiteral("Attack Air"));
    attackAirAction->setData(static_cast<int>(PlanStepKind::AttackAir));
    QAction* attackSurfaceAction = addMenu.addAction(QStringLiteral("Attack Surface"));
    attackSurfaceAction->setData(static_cast<int>(PlanStepKind::AttackSurface));

    QAction* selectedAction = addMenu.exec(addButton->mapToGlobal(QPoint(0, addButton->height())));
    if (!selectedAction) {
      return;
    }

    PlanStep step;
    dialog.hide();
    const bool configured = this->configurePlanStep(
        entityName,
        static_cast<PlanStepKind>(selectedAction->data().toInt()),
        step);
    dialog.show();
    dialog.raise();
    dialog.activateWindow();
    if (!configured) {
      return;
    }

    EntityPlan& plan = planForEntity();
    plan.steps.push_back(step);
    refreshList();
    stepsList->setCurrentRow(plan.steps.size() - 1);
    refreshButtons();
  });

  QObject::connect(removeButton, &QPushButton::clicked, &dialog, [this, planForEntity, stepsList, refreshList, refreshButtons]() {
    EntityPlan& plan = planForEntity();
    if (plan.running) {
      if (this->_ui && this->_ui->statusLabel) {
        this->_ui->statusLabel->setText(
            QStringLiteral("No puedes editar steps mientras el plan esta en ejecucion."));
      }
      refreshButtons();
      return;
    }

    const int row = stepsList->currentRow();
    if (row < 0 || row >= plan.steps.size()) {
      return;
    }
    plan.steps.removeAt(row);
    refreshList();
    refreshButtons();
  });

  QObject::connect(upButton, &QPushButton::clicked, &dialog, [this, planForEntity, stepsList, refreshList, refreshButtons]() {
    EntityPlan& plan = planForEntity();
    if (plan.running) {
      if (this->_ui && this->_ui->statusLabel) {
        this->_ui->statusLabel->setText(
            QStringLiteral("No puedes editar steps mientras el plan esta en ejecucion."));
      }
      refreshButtons();
      return;
    }

    const int row = stepsList->currentRow();
    if (row <= 0 || row >= plan.steps.size()) {
      return;
    }
    plan.steps.swapItemsAt(row, row - 1);
    refreshList();
    stepsList->setCurrentRow(row - 1);
    refreshButtons();
  });

  QObject::connect(downButton, &QPushButton::clicked, &dialog, [this, planForEntity, stepsList, refreshList, refreshButtons]() {
    EntityPlan& plan = planForEntity();
    if (plan.running) {
      if (this->_ui && this->_ui->statusLabel) {
        this->_ui->statusLabel->setText(
            QStringLiteral("No puedes editar steps mientras el plan esta en ejecucion."));
      }
      refreshButtons();
      return;
    }

    const int row = stepsList->currentRow();
    if (row < 0 || row >= plan.steps.size() - 1) {
      return;
    }
    plan.steps.swapItemsAt(row, row + 1);
    refreshList();
    stepsList->setCurrentRow(row + 1);
    refreshButtons();
  });

  QObject::connect(runButton, &QPushButton::clicked, &dialog, [this, entityName, refreshList, refreshButtons]() {
    if (!this->startEntityPlan(entityName)) {
      return;
    }
    this->syncScenarioStateToUi();
    refreshList();
    refreshButtons();
  });

  QObject::connect(stopButton, &QPushButton::clicked, &dialog, [this, entityName, refreshList, refreshButtons]() {
    this->stopEntityPlan(entityName, true);
    this->appendLogMessage(QStringLiteral("Plan stopped for %1").arg(entityName));
    this->_ui->statusLabel->setText(QStringLiteral("Plan detenido para %1.").arg(entityName));
    refreshList();
    refreshButtons();
  });

  QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

  QTimer refreshTimer(&dialog);
  refreshTimer.setInterval(250);
  QObject::connect(&refreshTimer, &QTimer::timeout, &dialog, [refreshList, refreshButtons]() {
    refreshList();
    refreshButtons();
  });
  refreshTimer.start();

  QEventLoop loop;
  QObject::connect(&dialog, &QDialog::accepted, &loop, [&loop]() {
    if (loop.isRunning()) {
      loop.quit();
    }
  });
  QObject::connect(&dialog, &QDialog::rejected, &loop, [&loop]() {
    if (loop.isRunning()) {
      loop.quit();
    }
  });
  QObject::connect(&dialog, &QObject::destroyed, &loop, [&loop]() {
    if (loop.isRunning()) {
      loop.quit();
    }
  });

  refreshList();
  refreshButtons();
  dialog.show();
  dialog.raise();
  dialog.activateWindow();
  loop.exec();
}

void MainWindow::clearSelectedTask() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty() || this->selectedEntityIsDestroyed()) {
    return;
  }

  if (this->_scenarioState->clearTask(entityName)) {
    this->appendLogMessage(QStringLiteral("Task cleared for %1").arg(entityName));
    this->syncScenarioStateToUi();
  }
}

void MainWindow::deleteSelectedEntity() {
  const QString objectName = this->selectedObjectName();
  if (objectName.isEmpty()) {
    return;
  }

  const QVariantMap summary = this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  const QString type = summary.value(QStringLiteral("type")).toString();

  bool removed = false;
  QString label = QStringLiteral("Object");
  if (this->currentSelectionIsEntity()) {
    removed = this->_scenarioState->removeEntity(objectName);
    label = QStringLiteral("Entity");
  } else if (type == QStringLiteral("Waypoint")) {
    removed = this->_scenarioState->removeWaypoint(objectName);
    label = QStringLiteral("Waypoint");
  } else if (type == QStringLiteral("Route")) {
    removed = this->_scenarioState->removeRoute(objectName);
    label = QStringLiteral("Route");
  } else if (type == QStringLiteral("Area")) {
    removed = this->_scenarioState->removeArea(objectName);
    label = QStringLiteral("Area");
  }

  QString cleanupStatusMessage;
  if (removed) {
    if (label == QStringLiteral("Entity")) {
      cleanupStatusMessage = this->cleanupRuntimeReferencesForRemovedEntity(objectName);
    }
    this->appendLogMessage(QStringLiteral("%1 deleted: %2").arg(label, objectName));
    this->removeTrackFromMap(objectName);
    this->syncScenarioStateToUi();
    this->_ui->statusLabel->setText(
        cleanupStatusMessage.isEmpty()
            ? QStringLiteral("%1 eliminado: %2").arg(label, objectName)
            : cleanupStatusMessage);
  }
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
    if (summary.value(QStringLiteral("name")).toString() == trackName) {
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
    if (entity.name == entityName) {
      return &entity;
    }
  }
  return nullptr;
}

QString MainWindow::selectedEntityName() const {
  if (!this->currentSelectionIsEntity()) {
    return QString();
  }
  return this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap().value(QStringLiteral("name")).toString();
}

QString MainWindow::selectedObjectName() const {
  const QModelIndex currentIndex = this->_ui->objectsTreeView->currentIndex();
  if (!currentIndex.isValid()) {
    return QString();
  }
  return currentIndex.data(kTrackSummaryRole).toMap().value(QStringLiteral("name")).toString();
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
  this->_bombReleaseController->clear();
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

void MainWindow::processPendingBombRelease() {
  this->_bombReleaseController->process();
}

void MainWindow::releaseBombAtSurfaceEntity() {
  this->clearPendingBombRelease();

  const QString launcherName = this->selectedEntityName();
  if (launcherName.isEmpty()) {
    return;
  }

  const Entity* launcher = this->findEntityByName(launcherName);
  const int bombCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) : 0;
  if (!launcher || bombCount <= 0) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay bombas disponibles en %1.").arg(launcherName));
    return;
  }

  if (!this->_simulationRunning) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Arranca la simulacion para programar el release de la bomba."));
    return;
  }

  const QVector<const Entity*> targets =
      application::validBombReleaseTargets(this->_scenarioState, *launcher);
  if (targets.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay surface targets validos para %1.")
            .arg(launcherName));
    return;
  }

  QStringList options;
  QHash<QString, QString> targetNameByOption;
  for (const Entity* target : targets) {
    if (!target) {
      continue;
    }
    const QString option = presentation::bombTargetDisplayLabel(*target);
    options.push_back(option);
    targetNameByOption.insert(option, target->name);
  }

  bool ok = false;
  const QString selectedOption = QInputDialog::getItem(
      this,
      QStringLiteral("Release Bomb At"),
      QStringLiteral("Surface Target"),
      options,
      0,
      false,
      &ok);
  if (!ok || selectedOption.trimmed().isEmpty()) {
    this->clearPendingBombRelease();
    return;
  }

  const QString targetName = targetNameByOption.value(selectedOption).trimmed();
  const Entity* target = this->findEntityByName(targetName);
  if (!target) {
    this->clearPendingBombRelease();
    return;
  }

  this->queuePendingBombRelease(
      launcherName,
      target->latitude,
      target->longitude,
      static_cast<double>(target->altitude),
      target->name,
      QStringLiteral("Surface Entity"),
      target->name);
}

void MainWindow::releaseBombAtCustomCoordinates() {
  this->clearPendingBombRelease();
  this->_bombReleaseController->cancelPickMode();

  const QString launcherName = this->selectedEntityName();
  if (launcherName.isEmpty()) {
    return;
  }

  const Entity* launcher = this->findEntityByName(launcherName);
  const int bombCount =
      launcher ? domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) : 0;
  if (!launcher || bombCount <= 0) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No hay bombas disponibles en %1.").arg(launcherName));
    return;
  }

  if (!this->_simulationRunning) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Arranca la simulacion para programar el release de la bomba."));
    return;
  }

  if (this->_graphicPickCoordinator->isPending()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Termina antes la captura de coordenadas que ya esta activa."));
    return;
  }

  this->_bombReleaseController->beginPickMode(launcherName);
  this->beginTaskCoordinatePick();
  this->_ui->statusLabel->setText(
      QStringLiteral("Haz clic en el mapa para fijar el punto de ataque de %1.")
          .arg(launcherName));
}

void MainWindow::cancelPendingBombRelease() {
  if (!this->_bombReleaseController->pendingRelease().pending) {
    this->_ui->statusLabel->setText(QStringLiteral("No hay release de bomba pendiente."));
    return;
  }

  const QString launcherName = this->_bombReleaseController->pendingRelease().launcherEntityName;
  const QString targetLabel = this->_bombReleaseController->pendingRelease().targetLabel;
  this->_bombReleaseController->cancelPickMode();
  this->clearPendingBombRelease();
  this->appendLogMessage(
      QStringLiteral("Bomb release canceled for %1 on %2")
          .arg(launcherName, targetLabel));
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(
      QStringLiteral("Release de bomba cancelado para %1.").arg(launcherName));
}

void MainWindow::openAssignTaskDialog(const QString& initialTaskType) {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (!this->currentSelectionIsOperableEntity()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("La entidad seleccionada no esta operable."));
    return;
  }

  const auto planIt = this->_planExecutor->plans().constFind(entityName);
  if (planIt != this->_planExecutor->plans().constEnd() && planIt->running) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No puedes editar la task mientras el plan esta en ejecucion."));
    return;
  }

  const QVariantMap currentSummary =
      this->_ui->objectsTreeView->currentIndex().data(kTrackSummaryRole).toMap();
  EntityTask currentTask;
  currentTask.taskType = currentSummary.value(QStringLiteral("taskType")).toString();
  currentTask.enabled = currentSummary.value(QStringLiteral("taskEnabled")).toBool();
  currentTask.status = currentSummary.value(QStringLiteral("taskStatus")).toString();
  currentTask.targetHeadingDegrees = currentSummary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble();
  currentTask.targetAltitudeMeters = currentSummary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt();
  currentTask.targetSpeedKnots = currentSummary.value(QStringLiteral("taskTargetSpeedKnots")).toDouble();
  currentTask.targetLatitude = currentSummary.value(QStringLiteral("taskTargetLatitude")).toDouble();
  currentTask.targetLongitude = currentSummary.value(QStringLiteral("taskTargetLongitude")).toDouble();
  currentTask.targetEntityName = currentSummary.value(QStringLiteral("taskTargetEntityName")).toString();
  currentTask.targetWaypointName = currentSummary.value(QStringLiteral("taskTargetWaypointName")).toString();
  currentTask.targetRouteName = currentSummary.value(QStringLiteral("taskTargetRouteName")).toString();
  currentTask.targetAreaName = currentSummary.value(QStringLiteral("taskTargetAreaName")).toString();
  currentTask.targetAreaRadiusMeters = currentSummary.value(QStringLiteral("taskTargetAreaRadiusMeters")).toDouble();

  EntityTask configuredTask;
  if (!this->captureTaskConfiguration(entityName, currentTask, initialTaskType, configuredTask)) {
    return;
  }

  this->applyEntityTask(entityName, configuredTask);
}

void MainWindow::populateTaskCommands() {
  this->_ui->tasksListWidget->clear();

  auto* flyItem = new QListWidgetItem(QStringLiteral("Movement: Fly Heading / Altitude / Speed..."));
  flyItem->setData(Qt::UserRole, QStringLiteral("FlyHeadingAltitudeSpeed"));
  this->_ui->tasksListWidget->addItem(flyItem);

  auto* moveItem = new QListWidgetItem(QStringLiteral("Movement: Move To Location..."));
  moveItem->setData(Qt::UserRole, QStringLiteral("MoveToLocation"));
  this->_ui->tasksListWidget->addItem(moveItem);

  auto* waypointItem = new QListWidgetItem(QStringLiteral("Movement: Move To Waypoint..."));
  waypointItem->setData(Qt::UserRole, QStringLiteral("MoveToWaypoint"));
  this->_ui->tasksListWidget->addItem(waypointItem);

  auto* routeItem = new QListWidgetItem(QStringLiteral("Movement: Move Along Route..."));
  routeItem->setData(Qt::UserRole, QStringLiteral("MoveAlongRoute"));
  this->_ui->tasksListWidget->addItem(routeItem);

  auto* patrolAreaItem = new QListWidgetItem(QStringLiteral("Movement: Patrol Area..."));
  patrolAreaItem->setData(Qt::UserRole, QStringLiteral("PatrolArea"));
  this->_ui->tasksListWidget->addItem(patrolAreaItem);

  auto* orbitAreaItem = new QListWidgetItem(QStringLiteral("Movement: Orbit Area..."));
  orbitAreaItem->setData(Qt::UserRole, QStringLiteral("OrbitArea"));
  this->_ui->tasksListWidget->addItem(orbitAreaItem);

  auto* followItem = new QListWidgetItem(QStringLiteral("Movement: Follow Entity..."));
  followItem->setData(Qt::UserRole, QStringLiteral("FollowEntity"));
  this->_ui->tasksListWidget->addItem(followItem);

  auto* attackAirItem = new QListWidgetItem(QStringLiteral("Attack: Attack Air..."));
  attackAirItem->setData(Qt::UserRole, QStringLiteral("AttackAir"));
  this->_ui->tasksListWidget->addItem(attackAirItem);

  auto* attackSurfaceItem = new QListWidgetItem(QStringLiteral("Attack: Attack Surface..."));
  attackSurfaceItem->setData(Qt::UserRole, QStringLiteral("AttackSurface"));
  this->_ui->tasksListWidget->addItem(attackSurfaceItem);

  auto* clearItem = new QListWidgetItem(QStringLiteral("Other: Clear Current Task"));
  clearItem->setData(Qt::UserRole, QStringLiteral("ClearTask"));
  this->_ui->tasksListWidget->addItem(clearItem);
}

void MainWindow::rebuildTacticalGraphicsTree() {
  if (!this->_tacticalGraphicsRootItem) {
    return;
  }

  this->_tacticalGraphicsRootItem->removeRows(0, this->_tacticalGraphicsRootItem->rowCount());

  for (const Waypoint& waypoint : this->_scenarioState->waypoints()) {
    QVariantMap waypointSummary = presentation::makeTrackSummary(
        waypoint.name,
        QStringLiteral("Waypoint"),
        QStringLiteral("Graphic"),
        QStringLiteral("%1 m").arg(waypoint.altitudeMeters, 0, 'f', 0),
        domain::formatPosition(waypoint.latitude, waypoint.longitude),
        QStringLiteral("Ready"),
        waypoint.latitude,
        waypoint.longitude);
    waypointSummary.insert(QStringLiteral("type"), QStringLiteral("Waypoint"));
    auto* waypointItem = new QStandardItem(waypoint.name);
    waypointItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Waypoint")));
    setTrackData(waypointItem, waypointSummary);
    this->_tacticalGraphicsRootItem->appendRow(waypointItem);
  }

  for (const RouteGraphic& route : this->_scenarioState->routes()) {
    QVariantList points;
    for (const RoutePoint& point : route.points) {
      points.push_back(QVariantMap{
          {QStringLiteral("latitude"), point.latitude},
          {QStringLiteral("longitude"), point.longitude},
          {QStringLiteral("altitudeMeters"), point.altitudeMeters},
      });
    }
    const RoutePoint firstPoint = route.points.isEmpty() ? RoutePoint{} : route.points.first();
    QVariantMap routeSummary = presentation::makeTrackSummary(
        route.name,
        QStringLiteral("Route"),
        QStringLiteral("Graphic"),
        QStringLiteral("-"),
        route.points.isEmpty() ? QStringLiteral("-") : domain::formatPosition(firstPoint.latitude, firstPoint.longitude),
        QStringLiteral("%1 points").arg(route.points.size()),
        firstPoint.latitude,
        firstPoint.longitude);
    routeSummary.insert(QStringLiteral("type"), QStringLiteral("Route"));
    routeSummary.insert(QStringLiteral("routePoints"), points);
    auto* routeItem = new QStandardItem(route.name);
    routeItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Route")));
    setTrackData(routeItem, routeSummary);
    this->_tacticalGraphicsRootItem->appendRow(routeItem);
  }

  for (const AreaDefinition& area : this->_scenarioState->areas()) {
    QVariantMap areaSummary = presentation::makeTrackSummary(
        area.name,
        QStringLiteral("Area"),
        QStringLiteral("Graphic"),
        QStringLiteral("%1 m").arg(area.centerAltitudeMeters, 0, 'f', 0),
        domain::formatPosition(area.centerLatitude, area.centerLongitude),
        QStringLiteral("%1").arg(area.areaType),
        area.centerLatitude,
        area.centerLongitude);
    areaSummary.insert(QStringLiteral("type"), QStringLiteral("Area"));
    areaSummary.insert(QStringLiteral("areaType"), area.areaType);
    areaSummary.insert(QStringLiteral("radiusMeters"), area.radiusMeters);
    areaSummary.insert(QStringLiteral("semiMajorAxisMeters"), area.semiMajorAxisMeters);
    areaSummary.insert(QStringLiteral("semiMinorAxisMeters"), area.semiMinorAxisMeters);
    areaSummary.insert(QStringLiteral("rotationDegrees"), area.rotationDegrees);
    QVariantList areaPoints;
    for (const RoutePoint& point : area.points) {
      areaPoints.push_back(QVariantMap{
          {QStringLiteral("latitude"), point.latitude},
          {QStringLiteral("longitude"), point.longitude},
          {QStringLiteral("altitudeMeters"), point.altitudeMeters},
      });
    }
    areaSummary.insert(QStringLiteral("areaPoints"), areaPoints);
    areaSummary.insert(QStringLiteral("minAltitudeMeters"), area.minAltitudeMeters);
    areaSummary.insert(QStringLiteral("maxAltitudeMeters"), area.maxAltitudeMeters);
    auto* areaItem = new QStandardItem(area.name);
    areaItem->setIcon(presentation::makeTacticalGraphicIcon(QStringLiteral("Engagement Area")));
    setTrackData(areaItem, areaSummary);
    this->_tacticalGraphicsRootItem->appendRow(areaItem);
  }

  if (this->_tacticalGraphicsRootItem->rowCount() > 0) {
    this->_ui->objectsTreeView->expand(this->_tacticalGraphicsRootItem->index());
  }
}

void MainWindow::beginTaskCoordinatePick() {
  this->beginEntityCoordinatePick();
}

void MainWindow::beginGraphicCoordinatePick() {
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
  bool ok = false;
  const QString name = QInputDialog::getText(
      this,
      QStringLiteral("Add Waypoint"),
      QStringLiteral("Waypoint name"),
      QLineEdit::Normal,
      QStringLiteral("Waypoint %1").arg(this->_scenarioState->waypoints().size() + 1),
      &ok).trimmed();
  if (!ok || name.isEmpty()) {
    return;
  }
  this->_graphicPickCoordinator->beginWaypointPick(name);
}

void MainWindow::openAddRouteDialog() {
  bool ok = false;
  const QString name = QInputDialog::getText(
      this,
      QStringLiteral("Add Route"),
      QStringLiteral("Route name"),
      QLineEdit::Normal,
      QStringLiteral("Route %1").arg(this->_scenarioState->routes().size() + 1),
      &ok).trimmed();
  if (!ok || name.isEmpty()) {
    return;
  }
  this->_graphicPickCoordinator->beginRoutePick(name);
}

void MainWindow::openAddAreaDialog() {
  bool ok = false;
  const QString name = QInputDialog::getText(
      this,
      QStringLiteral("Add Area"),
      QStringLiteral("Area name"),
      QLineEdit::Normal,
      QStringLiteral("Area %1").arg(this->_scenarioState->areas().size() + 1),
      &ok).trimmed();
  if (!ok || name.isEmpty()) {
    return;
  }

  const QStringList areaTypes = {
      QStringLiteral("Circle"),
      QStringLiteral("Ellipse"),
      QStringLiteral("Polygon"),
  };
  const QString areaType = QInputDialog::getItem(
      this,
      QStringLiteral("Area Type"),
      QStringLiteral("Type"),
      areaTypes,
      0,
      false,
      &ok);
  if (!ok) {
    return;
  }

  const double areaAltitudeMeters = QInputDialog::getDouble(
      this,
      QStringLiteral("Area Altitude"),
      QStringLiteral("Center altitude (m)"),
      0.0,
      -1000.0,
      80000.0,
      1,
      &ok);
  if (!ok) {
    return;
  }

  if (areaType == QStringLiteral("Circle")) {
    const double radiusMeters = QInputDialog::getDouble(
        this,
        QStringLiteral("Area Radius"),
        QStringLiteral("Radius (m)"),
        5000.0,
        50.0,
        500000.0,
        0,
        &ok);
    if (!ok) {
      return;
    }
    this->_graphicPickCoordinator->beginAreaCirclePick(
        name, {areaAltitudeMeters, radiusMeters});
    return;
  }

  if (areaType == QStringLiteral("Ellipse")) {
    const double semiMajorMeters = QInputDialog::getDouble(
        this,
        QStringLiteral("Ellipse Semi-major Axis"),
        QStringLiteral("Semi-major axis (m)"),
        6000.0,
        50.0,
        500000.0,
        0,
        &ok);
    if (!ok) {
      return;
    }
    const double semiMinorMeters = QInputDialog::getDouble(
        this,
        QStringLiteral("Ellipse Semi-minor Axis"),
        QStringLiteral("Semi-minor axis (m)"),
        3000.0,
        50.0,
        500000.0,
        0,
        &ok);
    if (!ok) {
      return;
    }
    const double rotationDegrees = QInputDialog::getDouble(
        this,
        QStringLiteral("Ellipse Rotation"),
        QStringLiteral("Rotation (deg)"),
        0.0,
        -360.0,
        360.0,
        1,
        &ok);
    if (!ok) {
      return;
    }
    this->_graphicPickCoordinator->beginAreaEllipsePick(
        name, {areaAltitudeMeters, semiMajorMeters, semiMinorMeters, rotationDegrees});
    return;
  }

  this->_graphicPickCoordinator->beginAreaPolygonPick(name, areaAltitudeMeters);
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
