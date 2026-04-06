#include "MainWindow.h"

#include "AddEntityDialog.h"
#include "AssignTaskDialog.h"
#include "EntityDetailsDialog.h"
#include "application/ScenarioState.h"
#include "application/SimulationEngine.h"
#include "application/Command.h"
#include "domain/Entity.h"
#include "infrastructure/CesiumScenePage.h"
#include "infrastructure/MapBridge.h"
#include "infrastructure/ModelCatalog.h"
#include "presentation/EntityTextFormatter.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QFileInfo>
#include <QColor>
#include <QHeaderView>
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
#include <QStandardItemModel>
#include <QTimer>
#include <QInputDialog>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVariantList>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QTimer>

#include <functional>
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

namespace {
constexpr int kTrackSummaryRole = Qt::UserRole + 1;
constexpr double kGraphicAltitudeOffsetMeters = 15.0;
constexpr double kPolygonCloseDistanceMeters = 50.0;

QVariantMap makeTrackSummary(
    const QString& name,
    const QString& type,
    const QString& team,
    const QString& altitudeText,
    const QString& positionText,
    const QString& status,
    double latitude,
    double longitude) {
  return {
      {QStringLiteral("name"), name},
      {QStringLiteral("type"), type},
      {QStringLiteral("team"), team},
      {QStringLiteral("altitude"), altitudeText},
      {QStringLiteral("position"), positionText},
      {QStringLiteral("status"), status},
      {QStringLiteral("latitude"), latitude},
      {QStringLiteral("longitude"), longitude},
      {QStringLiteral("callsign"), QString()},
      {QStringLiteral("domain"), QString()},
      {QStringLiteral("category"), QString()},
      {QStringLiteral("forceIdentifier"), 0},
      {QStringLiteral("entityKind"), 0},
      {QStringLiteral("entityDomain"), 0},
      {QStringLiteral("entityCountry"), 0},
      {QStringLiteral("entityCategory"), 0},
      {QStringLiteral("entitySubcategory"), 0},
      {QStringLiteral("entitySpecific"), 0},
      {QStringLiteral("entityExtra"), 0},
      {QStringLiteral("entityTypeCode"), QString()},
      {QStringLiteral("modelName"), QString()},
      {QStringLiteral("modelUri"), QString()},
      {QStringLiteral("headingDegrees"), 0.0},
      {QStringLiteral("flightDynamicsEnabled"), false},
      {QStringLiteral("flightDynamicsMode"), QStringLiteral("kinematic")},
      {QStringLiteral("jsbsimAircraftModel"), QString()},
      {QStringLiteral("speedKnots"), 0.0},
      {QStringLiteral("verticalSpeedMetersPerSecond"), 0.0},
      {QStringLiteral("taskType"), QString()},
      {QStringLiteral("taskEnabled"), false},
      {QStringLiteral("taskStatus"), QStringLiteral("Idle")},
      {QStringLiteral("taskTargetHeadingDegrees"), 0.0},
      {QStringLiteral("taskTargetAltitudeMeters"), 0},
      {QStringLiteral("taskTargetSpeedKnots"), 0.0},
      {QStringLiteral("taskTargetLatitude"), 0.0},
      {QStringLiteral("taskTargetLongitude"), 0.0},
      {QStringLiteral("taskTargetEntityName"), QString()},
      {QStringLiteral("taskTargetWaypointName"), QString()},
      {QStringLiteral("taskTargetRouteName"), QString()},
  };
}

QString forceIdentifierLabel(int forceIdentifier) {
  switch (forceIdentifier) {
    case 1:
      return QStringLiteral("Friendly");
    case 2:
      return QStringLiteral("Opposing");
    case 3:
      return QStringLiteral("Neutral");
    default:
      return QStringLiteral("Unknown");
  }
}

QString formatPosition(double latitude, double longitude) {
  return QStringLiteral("%1, %2")
      .arg(latitude, 0, 'f', 4)
      .arg(longitude, 0, 'f', 4);
}

double distanceMeters(
    double latitude1,
    double longitude1,
    double latitude2,
    double longitude2) {
  constexpr double earthRadiusMeters = 6371000.0;
  const double lat1 = qDegreesToRadians(latitude1);
  const double lon1 = qDegreesToRadians(longitude1);
  const double lat2 = qDegreesToRadians(latitude2);
  const double lon2 = qDegreesToRadians(longitude2);
  const double deltaLat = lat2 - lat1;
  const double deltaLon = lon2 - lon1;
  const double a = qPow(qSin(deltaLat / 2.0), 2.0) +
                   qCos(lat1) * qCos(lat2) * qPow(qSin(deltaLon / 2.0), 2.0);
  const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
  return earthRadiusMeters * c;
}

QVariantMap makeTrackSummary(const Entity& entity) {
  QVariantMap summary = makeTrackSummary(
      entity.name,
      entity.type,
      forceIdentifierLabel(entity.forceIdentifier),
      QStringLiteral("%1 m").arg(entity.altitude),
      formatPosition(entity.latitude, entity.longitude),
      QStringLiteral("Ready"),
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
  summary.insert(QStringLiteral("flightDynamicsEnabled"), entity.flightDynamicsEnabled);
  summary.insert(QStringLiteral("flightDynamicsMode"), entity.flightDynamicsMode);
  summary.insert(QStringLiteral("jsbsimAircraftModel"), entity.jsbsimAircraftModel);
  summary.insert(QStringLiteral("speedKnots"), entity.speedKnots);
  summary.insert(QStringLiteral("verticalSpeedMetersPerSecond"), entity.verticalSpeedMetersPerSecond);
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
        {QStringLiteral("azimuthWidthDegrees"), sensor.azimuthWidthDegrees},
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

void setTrackData(QStandardItem* item, const QVariantMap& summary) {
  item->setData(summary, kTrackSummaryRole);
}

QJsonObject mapToJsonObject(const QVariantMap& map) {
  return QJsonObject::fromVariantMap(map);
}
QColor forceColorFromLabel(const QString& team) {
  const QString normalized = team.trimmed().toLower();
  if (normalized.contains(QStringLiteral("opposing"))) {
    return QColor(QStringLiteral("#ff9b59"));
  }
  if (normalized.contains(QStringLiteral("neutral"))) {
    return QColor(QStringLiteral("#b7c0c9"));
  }
  return QColor(QStringLiteral("#55d3ff"));
}

QString categoryGlyph(const QString& category) {
  const QString normalized = category.trimmed().toLower();
  if (normalized == QStringLiteral("fighter")) {
    return QStringLiteral("F");
  }
  if (normalized == QStringLiteral("bomber")) {
    return QStringLiteral("B");
  }
  if (normalized == QStringLiteral("helicopter")) {
    return QStringLiteral("H");
  }
  if (normalized == QStringLiteral("transport")) {
    return QStringLiteral("T");
  }
  if (normalized == QStringLiteral("other")) {
    return QStringLiteral("O");
  }
  if (normalized == QStringLiteral("side")) {
    return QStringLiteral("S");
  }
  return QStringLiteral("E");
}

QIcon makeTacticalGraphicIcon(const QString& graphicType) {
  QPixmap pixmap(18, 18);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QString normalized = graphicType.trimmed().toLower();
  if (normalized == QStringLiteral("route")) {
    QPen pen(QColor(QStringLiteral("#ff6c52")));
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(3, 15, 15, 3);
  } else if (normalized == QStringLiteral("waypoint")) {
    QPen pen(QColor(QStringLiteral("#b6f4b2")));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(QColor(QStringLiteral("#dff9da")));
    painter.drawRect(5, 3, 8, 10);
  } else if (normalized == QStringLiteral("engagement area")) {
    QPen pen(QColor(QStringLiteral("#ff5656")));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(4, 4, 10, 10);
  } else {
    QPen pen(QColor(QStringLiteral("#ffd85e")));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawEllipse(4, 4, 10, 10);
  }

  return QIcon(pixmap);
}

QIcon makeTrackIcon(const QString& team, const QString& category, bool isGroup) {
  QPixmap pixmap(18, 18);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QColor accent = forceColorFromLabel(team);
  const QRect outerRect(1, 1, 16, 16);
  const QRect innerRect(3, 3, 12, 12);

  QPen pen(accent);
  pen.setWidth(isGroup ? 2 : 1);
  painter.setPen(pen);
  painter.setBrush(QColor(7, 17, 29, isGroup ? 220 : 245));
  painter.drawRoundedRect(outerRect, 4, 4);

  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(isGroup ? 10 : 9);
  painter.setFont(font);
  painter.setPen(accent);
  painter.drawText(innerRect, Qt::AlignCenter, categoryGlyph(category));

  return QIcon(pixmap);
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _contentWidget(nullptr),
      _mapBridge(new MapBridge(this)),
      _scenarioState(new ScenarioState()),
      _objectsModel(new QStandardItemModel(this)),
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
      _pendingGraphicMode(),
      _pendingGraphicName(),
      _pendingAreaType(QStringLiteral("Circle")),
      _pendingAreaRadiusMeters(1000.0),
      _pendingAreaSemiMajorMeters(1000.0),
      _pendingAreaSemiMinorMeters(600.0),
      _pendingAreaRotationDegrees(0.0),
      m_simulationEngine(new application::SimulationEngine(_scenarioState, this))
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
      , _webView(nullptr)
#endif
{
  this->_ui->setupUi(this);
  this->_ui->viewerHost->setMinimumSize(960, 640);
  this->_ui->viewerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->initializeModels();
  this->populateTaskCommands();
  for (const Entity& entity : this->_scenarioState->entities()) {
    this->appendEntityToUi(entity);
  }
  _simulationTimer->setInterval(100);
  QObject::connect(_simulationTimer, &QTimer::timeout, this, [this]() {
    this->_scenarioState->advanceSimulation(0.1);
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
  this->updateSimulationControls();
}

MainWindow::~MainWindow() {
  delete this->_scenarioState;
  delete this->_ui;
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
  this->_ui->selectionStateValueLabel->setText(
      taskType == QStringLiteral("-") || taskType == QStringLiteral("No current tasks")
          ? status
          : QStringLiteral("%1 (%2)").arg(taskType, taskStatus));
  this->_ui->selectionPositionValueLabel->setText(position);
}

void MainWindow::initializeModels() {
  this->_objectsModel->setHorizontalHeaderLabels({QStringLiteral("Name")});

  this->_friendlyRootItem = new QStandardItem(QStringLiteral("Friendly"));
  this->_friendlyRootItem->setIcon(makeTrackIcon(QStringLiteral("Friendly"), QStringLiteral("Side"), true));
  this->_opposingRootItem = new QStandardItem(QStringLiteral("Opposing"));
  this->_opposingRootItem->setIcon(makeTrackIcon(QStringLiteral("Opposing"), QStringLiteral("Side"), true));
  this->_neutralRootItem = new QStandardItem(QStringLiteral("Neutral"));
  this->_neutralRootItem->setIcon(makeTrackIcon(QStringLiteral("Neutral"), QStringLiteral("Side"), true));
  this->_tacticalGraphicsRootItem = new QStandardItem(QStringLiteral("Tactical Graphics"));
  this->_tacticalGraphicsRootItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Graphic")));
  setTrackData(
      this->_friendlyRootItem,
      makeTrackSummary(
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
      makeTrackSummary(
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
      makeTrackSummary(
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
      makeTrackSummary(
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

  this->_ui->eventLogPlainTextEdit->clear();
  this->appendLogMessage(QStringLiteral("Operational log ready."));
  this->appendLogMessage(QStringLiteral("Cesium map connected."));
  this->appendLogMessage(QStringLiteral("Awaiting commands..."));

  this->_ui->objectsTreeView->clearSelection();
  this->setSelectedTrackDetails(QVariantMap{});
  this->rebuildTacticalGraphicsTree();
}

void MainWindow::appendEntityToUi(const Entity& entity) {
  QStandardItem* rootItem = this->rootItemForForceIdentifier(entity.forceIdentifier);
  if (!rootItem) {
    return;
  }

  QString category = entity.category.trimmed();
  if (category != QStringLiteral("Fighter") &&
      category != QStringLiteral("Bomber") &&
      category != QStringLiteral("Helicopter") &&
      category != QStringLiteral("Transport")) {
    category = QStringLiteral("Other");
  }

  const QVariantMap summary = makeTrackSummary(entity);
  QStandardItem* categoryItem = this->ensureGroupItem(
      rootItem,
      category,
      makeTrackSummary(
          category,
          QStringLiteral("Category"),
          forceIdentifierLabel(entity.forceIdentifier),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("Category"),
          0.0,
          0.0));

  auto* item = new QStandardItem(entity.name);
  item->setIcon(makeTrackIcon(forceIdentifierLabel(entity.forceIdentifier), category, false));
  setTrackData(item, summary);
  categoryItem->appendRow(item);
  this->_ui->objectsTreeView->expand(rootItem->index());
  this->_ui->objectsTreeView->expand(categoryItem->index());
  this->_ui->objectsTreeView->setCurrentIndex(item->index());

  this->appendLogMessage(EntityTextFormatter::listLabel(entity));
  this->sendTrackToMap(summary, true);
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
  item->setIcon(makeTrackIcon(summary.value(QStringLiteral("team")).toString(), label, true));
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
  const double graphicAltitude = qMax(0.0, height + kGraphicAltitudeOffsetMeters);
  bool handledGraphic = false;

  if (_pendingGraphicMode == QStringLiteral("Waypoint")) {
    Waypoint waypoint;
    waypoint.name = _pendingGraphicName;
    waypoint.longitude = longitude;
    waypoint.latitude = latitude;
    waypoint.altitudeMeters = graphicAltitude;
    this->_scenarioState->addWaypoint(waypoint);
    this->_ui->statusLabel->setText(
        QStringLiteral("Waypoint %1 creado en lat %2, lon %3, alt %4 m.")
            .arg(_pendingGraphicName)
            .arg(latitude, 0, 'f', 5)
            .arg(longitude, 0, 'f', 5)
            .arg(graphicAltitude, 0, 'f', 0));
    _pendingGraphicMode.clear();
    _pendingGraphicName.clear();
    handledGraphic = true;
    this->syncScenarioStateToUi();
  } else if (_pendingGraphicMode == QStringLiteral("Route")) {
    _pendingRoutePoints.push_back(QVariantMap{
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("altitudeMeters"), graphicAltitude},
    });
    if (_pendingRoutePoints.size() >= 2) {
      RouteGraphic route;
      route.name = _pendingGraphicName;
      for (const QVariantMap& pointSummary : _pendingRoutePoints) {
        RoutePoint point;
        point.longitude = pointSummary.value(QStringLiteral("longitude")).toDouble();
        point.latitude = pointSummary.value(QStringLiteral("latitude")).toDouble();
        point.altitudeMeters = pointSummary.value(QStringLiteral("altitudeMeters")).toDouble();
        route.points.push_back(point);
      }
      this->_scenarioState->addRoute(route);
      this->_ui->statusLabel->setText(
          QStringLiteral("Route %1 creada con %2 puntos. Ajustada a +%3 m sobre el terreno.")
              .arg(_pendingGraphicName)
              .arg(route.points.size())
              .arg(kGraphicAltitudeOffsetMeters, 0, 'f', 0));
      this->clearDraftGraphicFromMap(_pendingGraphicName + QStringLiteral(" (draft)"));
      _pendingGraphicMode.clear();
      _pendingGraphicName.clear();
      _pendingRoutePoints.clear();
      handledGraphic = true;
      this->syncScenarioStateToUi();
    } else {
      QVariantMap draftSummary = makeTrackSummary(
          _pendingGraphicName + QStringLiteral(" (draft)"),
          QStringLiteral("Route"),
          QStringLiteral("Graphic"),
          QStringLiteral("%1 m").arg(graphicAltitude, 0, 'f', 0),
          formatPosition(latitude, longitude),
          QStringLiteral("Route draft"),
          latitude,
          longitude);
      draftSummary.insert(QStringLiteral("type"), QStringLiteral("Route"));
      QVariantList routePoints;
      for (const QVariantMap& pointSummary : _pendingRoutePoints) {
        routePoints.push_back(pointSummary);
      }
      draftSummary.insert(QStringLiteral("routePoints"), routePoints);
      this->sendDraftGraphicToMap(draftSummary);
      this->_ui->statusLabel->setText(
          QStringLiteral("Primer punto de la ruta capturado. Selecciona ahora el segundo punto en el mapa."));
      this->beginGraphicCoordinatePick();
      return;
    }
  } else if (_pendingGraphicMode == QStringLiteral("Area")) {
    AreaDefinition area;
    area.id = _pendingGraphicName;
    area.name = _pendingGraphicName;
    area.areaType = _pendingAreaType;
    area.centerLongitude = longitude;
    area.centerLatitude = latitude;
    area.centerAltitudeMeters = graphicAltitude;
    area.radiusMeters = _pendingAreaRadiusMeters;
    area.semiMajorAxisMeters = _pendingAreaSemiMajorMeters;
    area.semiMinorAxisMeters = _pendingAreaSemiMinorMeters;
    area.rotationDegrees = _pendingAreaRotationDegrees;
    this->_scenarioState->addArea(area);
    this->_ui->statusLabel->setText(
        QStringLiteral("Area %1 (%2) creada en lat %3, lon %4.")
            .arg(_pendingGraphicName)
            .arg(_pendingAreaType)
            .arg(latitude, 0, 'f', 5)
            .arg(longitude, 0, 'f', 5));
    _pendingGraphicMode.clear();
    _pendingGraphicName.clear();
    handledGraphic = true;
    this->syncScenarioStateToUi();
  } else if (_pendingGraphicMode == QStringLiteral("AreaPolygon")) {
    const QVariantMap capturedPoint = {
        {QStringLiteral("longitude"), longitude},
        {QStringLiteral("latitude"), latitude},
        {QStringLiteral("altitudeMeters"), graphicAltitude},
    };
    if (_pendingAreaPoints.size() >= 3) {
      const QVariantMap& firstPoint = _pendingAreaPoints.first();
      const double closeDistanceMeters = distanceMeters(
          latitude,
          longitude,
          firstPoint.value(QStringLiteral("latitude")).toDouble(),
          firstPoint.value(QStringLiteral("longitude")).toDouble());
      if (closeDistanceMeters <= kPolygonCloseDistanceMeters) {
        AreaDefinition area;
        area.id = _pendingGraphicName;
        area.name = _pendingGraphicName;
        area.areaType = QStringLiteral("Polygon");
        area.centerLongitude = firstPoint.value(QStringLiteral("longitude")).toDouble();
        area.centerLatitude = firstPoint.value(QStringLiteral("latitude")).toDouble();
        area.centerAltitudeMeters = firstPoint.value(QStringLiteral("altitudeMeters")).toDouble();
        for (const QVariantMap& pointSummary : _pendingAreaPoints) {
          RoutePoint point;
          point.longitude = pointSummary.value(QStringLiteral("longitude")).toDouble();
          point.latitude = pointSummary.value(QStringLiteral("latitude")).toDouble();
          point.altitudeMeters = pointSummary.value(QStringLiteral("altitudeMeters")).toDouble();
          area.points.push_back(point);
        }
        this->_scenarioState->addArea(area);
        this->clearDraftGraphicFromMap(_pendingGraphicName + QStringLiteral(" (draft)"));
        this->_ui->statusLabel->setText(
            QStringLiteral("Polygon area %1 creada con %2 puntos.")
                .arg(_pendingGraphicName)
                .arg(area.points.size()));
        _pendingGraphicMode.clear();
        _pendingGraphicName.clear();
        _pendingAreaPoints.clear();
        handledGraphic = true;
        this->syncScenarioStateToUi();
      } else {
        _pendingAreaPoints.push_back(capturedPoint);
        QVariantMap draftSummary = makeTrackSummary(
            _pendingGraphicName + QStringLiteral(" (draft)"),
            QStringLiteral("Area"),
            QStringLiteral("Graphic"),
            QStringLiteral("%1 m").arg(graphicAltitude, 0, 'f', 0),
            formatPosition(latitude, longitude),
            QStringLiteral("Polygon draft"),
            latitude,
            longitude);
        draftSummary.insert(QStringLiteral("type"), QStringLiteral("Area"));
        draftSummary.insert(QStringLiteral("areaType"), QStringLiteral("Polygon"));
        draftSummary.insert(QStringLiteral("radiusMeters"), 0.0);
        draftSummary.insert(QStringLiteral("semiMajorAxisMeters"), 0.0);
        draftSummary.insert(QStringLiteral("semiMinorAxisMeters"), 0.0);
        draftSummary.insert(QStringLiteral("rotationDegrees"), 0.0);
        QVariantList areaPoints;
        for (const QVariantMap& pointSummary : _pendingAreaPoints) {
          areaPoints.push_back(pointSummary);
        }
        draftSummary.insert(QStringLiteral("areaPoints"), areaPoints);
        this->sendDraftGraphicToMap(draftSummary);
        this->_ui->statusLabel->setText(
            QStringLiteral("Punto %1 del polygon capturado. Para cerrar, pincha cerca del primer punto.")
                .arg(_pendingAreaPoints.size()));
        this->beginGraphicCoordinatePick();
        return;
      }
    } else {
      _pendingAreaPoints.push_back(capturedPoint);
      QVariantMap draftSummary = makeTrackSummary(
          _pendingGraphicName + QStringLiteral(" (draft)"),
          QStringLiteral("Area"),
          QStringLiteral("Graphic"),
          QStringLiteral("%1 m").arg(graphicAltitude, 0, 'f', 0),
          formatPosition(latitude, longitude),
          QStringLiteral("Polygon draft"),
          latitude,
          longitude);
      draftSummary.insert(QStringLiteral("type"), QStringLiteral("Area"));
      draftSummary.insert(QStringLiteral("areaType"), QStringLiteral("Polygon"));
      draftSummary.insert(QStringLiteral("radiusMeters"), 0.0);
      draftSummary.insert(QStringLiteral("semiMajorAxisMeters"), 0.0);
      draftSummary.insert(QStringLiteral("semiMinorAxisMeters"), 0.0);
      draftSummary.insert(QStringLiteral("rotationDegrees"), 0.0);
      QVariantList areaPoints;
      for (const QVariantMap& pointSummary : _pendingAreaPoints) {
        areaPoints.push_back(pointSummary);
      }
      draftSummary.insert(QStringLiteral("areaPoints"), areaPoints);
      this->sendDraftGraphicToMap(draftSummary);
      this->_ui->statusLabel->setText(
          QStringLiteral("Punto %1 del polygon capturado. Sigue anadiendo puntos; para cerrar, pincha cerca del primero.")
              .arg(_pendingAreaPoints.size()));
      this->beginGraphicCoordinatePick();
      return;
    }
  }

  if (this->_entityDialog) {
    this->_entityDialog->setPickedCoordinate(longitude, latitude, height);
    this->_entityDialog->show();
    this->_entityDialog->raise();
    this->_entityDialog->activateWindow();
  }
  if (this->_taskDialog) {
    this->_taskDialog->setPickedCoordinate(longitude, latitude, height);
    this->_taskDialog->show();
    this->_taskDialog->raise();
    this->_taskDialog->activateWindow();
  }

  if (!handledGraphic) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Coordenadas capturadas: lat %1, lon %2")
            .arg(latitude, 0, 'f', 5)
            .arg(longitude, 0, 'f', 5));
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
  this->_ui->statusLabel->setText(QStringLiteral("Simulacion en pausa."));
  this->appendLogMessage(QStringLiteral("Simulation paused."));
  this->updateSimulationControls();
}

void MainWindow::stopSimulation() {
  this->_simulationRunning = false;
  this->_simulationTimer->stop();
  this->_scenarioState->stopMission();
  this->syncScenarioStateToUi();
  this->_ui->statusLabel->setText(QStringLiteral("Mision detenida. Todas las tasks han terminado."));
  this->appendLogMessage(QStringLiteral("Simulation stopped. Mission state cleared."));
  this->updateSimulationControls();
}

void MainWindow::updateSelectedTrackPanel(const QModelIndex& current, const QModelIndex&) {
  const QVariantMap summary = current.data(kTrackSummaryRole).toMap();
  if (summary.isEmpty()) {
    return;
  }

  this->setSelectedTrackDetails(summary);

  const QString selectedName = summary.value(QStringLiteral("name")).toString();
  if (!selectedName.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Track seleccionado: %1").arg(selectedName));
  }

  if (!this->_applyingMapSelection) {
    this->sendTrackToMap(summary, true);
  }
}

void MainWindow::handleMapTrackSelection(const QString& trackName) {
  if (trackName.trimmed().isEmpty()) {
    return;
  }

  this->_applyingMapSelection = true;
  this->selectObjectByName(trackName, false);
  this->_applyingMapSelection = false;
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

void MainWindow::syncScenarioStateToUi() {
  std::function<void(QStandardItem*)> removeMissingFromBranch = [this, &removeMissingFromBranch](QStandardItem* branch) {
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
        branch->removeRow(row);
      }
    }
  };

  removeMissingFromBranch(this->_friendlyRootItem);
  removeMissingFromBranch(this->_opposingRootItem);
  removeMissingFromBranch(this->_neutralRootItem);

  for (const Entity& entity : this->_scenarioState->entities()) {
    QStandardItem* item = this->findTrackItemByName(this->_friendlyRootItem, entity.name);
    if (!item) {
      item = this->findTrackItemByName(this->_opposingRootItem, entity.name);
    }
    if (!item) {
      item = this->findTrackItemByName(this->_neutralRootItem, entity.name);
    }

    const QVariantMap summary = makeTrackSummary(entity);
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

  this->rebuildTacticalGraphicsTree();
  this->syncTacticalGraphicsToMap();
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
    QMenu* taskMenu = menu.addMenu(QStringLiteral("Task"));
    QMenu* movementMenu = taskMenu->addMenu(QStringLiteral("Movement"));
    movementMenu->addAction(QStringLiteral("Fly Heading / Altitude / Speed..."), this, &MainWindow::assignFlyHeadingAltitudeSpeedTask);
    movementMenu->addAction(QStringLiteral("Move To Location..."), this, &MainWindow::assignMoveToLocationTask);
    movementMenu->addAction(QStringLiteral("Move To Waypoint..."), this, &MainWindow::assignMoveToWaypointTask);
    movementMenu->addAction(QStringLiteral("Move Along Route..."), this, &MainWindow::assignMoveAlongRouteTask);
    movementMenu->addAction(QStringLiteral("Patrol Area..."), this, &MainWindow::assignPatrolAreaTask);
    movementMenu->addAction(QStringLiteral("Orbit Area..."), this, &MainWindow::assignOrbitAreaTask);
    movementMenu->addAction(QStringLiteral("Follow Entity..."), this, &MainWindow::assignFollowEntityTask);
    taskMenu->addSeparator();
    taskMenu->addAction(QStringLiteral("Clear Current Task"), this, &MainWindow::clearSelectedTask);
    menu.addSeparator();
    menu.addAction(QStringLiteral("Entity Details..."), this, &MainWindow::openSelectedEntityDetails);
    menu.addAction(QStringLiteral("Delete Entity"), this, &MainWindow::deleteSelectedEntity);
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

void MainWindow::assignPatrolAreaTask() {
  this->openAssignTaskDialog(QStringLiteral("PatrolArea"));
}

void MainWindow::assignOrbitAreaTask() {
  this->openAssignTaskDialog(QStringLiteral("OrbitArea"));
}

void MainWindow::assignFollowEntityTask() {
  this->openAssignTaskDialog(QStringLiteral("FollowEntity"));
}

void MainWindow::clearSelectedTask() {
  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
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

  if (removed) {
    this->appendLogMessage(QStringLiteral("%1 deleted: %2").arg(label, objectName));
    this->removeTrackFromMap(objectName);
    this->syncScenarioStateToUi();
    this->_ui->statusLabel->setText(QStringLiteral("%1 eliminado: %2").arg(label, objectName));
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

void MainWindow::openAssignTaskDialog(const QString& initialTaskType) {
  if (this->_taskDialog) {
    this->_taskDialog->close();
    this->_taskDialog = nullptr;
  }

  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
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

  QStringList availableTargets;
  for (const Entity& entity : this->_scenarioState->entities()) {
    if (entity.name != entityName) {
      availableTargets.append(entity.name);
    }
  }

  QStringList availableWaypoints;
  for (const Waypoint& waypoint : this->_scenarioState->waypoints()) {
    availableWaypoints.append(waypoint.name);
  }

  QStringList availableRoutes;
  for (const RouteGraphic& route : this->_scenarioState->routes()) {
    availableRoutes.append(route.name);
  }

  QStringList availableAreas;
  for (const AreaDefinition& area : this->_scenarioState->areas()) {
    availableAreas.append(area.name);
  }

  auto* dialog = new AssignTaskDialog(
      entityName,
      availableTargets,
      availableWaypoints,
      availableRoutes,
      availableAreas,
      currentTask,
      initialTaskType,
      this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  this->_taskDialog = dialog;

  QObject::connect(
      dialog,
      &QObject::destroyed,
      this,
      [this]() { this->_taskDialog = nullptr; });
  QObject::connect(
      dialog,
      &AssignTaskDialog::pickOnMapRequested,
      this,
      &MainWindow::beginTaskCoordinatePick);
  QObject::connect(
      dialog,
      &QDialog::accepted,
      this,
      [this, dialog, entityName]() {
        const EntityTask task = dialog->task();
        if (this->_scenarioState->assignTask(entityName, task)) {
          this->appendLogMessage(
              QStringLiteral("Task %1 assigned to %2").arg(task.taskType, entityName));

          Entity resolvedEntity;
          for (const Entity& e : this->_scenarioState->entities()) {
            if (e.name == entityName) {
              resolvedEntity = e;
              break;
            }
          }

          // Push task to TaskStack immediately so movement works even if SimulationEngine isn't running.
          if (domain::TaskStack* stack = this->_scenarioState->getTaskStack(entityName)) {
            while (!stack->isEmpty()) {
              stack->pop();
            }
            if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint" || task.taskType == "MoveAlongRoute") {
              stack->push(std::make_unique<domain::MoveToLocationTask>(
                  resolvedEntity.currentTask.targetLatitude,
                  resolvedEntity.currentTask.targetLongitude,
                  resolvedEntity.currentTask.targetAltitudeMeters,
                  resolvedEntity.currentTask.targetSpeedKnots
              ));
            } else if (task.taskType == "FlyHeadingAltitudeSpeed") {
              stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
                  task.targetHeadingDegrees,
                  static_cast<double>(task.targetAltitudeMeters),
                  task.targetSpeedKnots
              ));
            } else if (task.taskType == "FollowEntity") {
              stack->push(std::make_unique<domain::FollowEntityTask>(
                  static_cast<double>(task.targetAltitudeMeters),
                  task.targetSpeedKnots
              ));
            } else if (task.taskType == "PatrolArea" || task.taskType == "OrbitArea") {
              stack->push(std::make_unique<domain::OrbitAreaTask>(
                  task.targetLatitude,
                  task.targetLongitude,
                  task.targetAreaRadiusMeters,
                  static_cast<double>(task.targetAltitudeMeters),
                  task.targetSpeedKnots,
                  (task.taskType == "PatrolArea")
              ));
            }
          }

          if (!m_simulationEngine) return;

          if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint" || task.taskType == "MoveAlongRoute") {
              // Map to existing CmdAssignMoveTask using resolved coordinates
              m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignMoveTask>(
                  0, entityName, resolvedEntity.currentTask.targetLatitude, resolvedEntity.currentTask.targetLongitude, resolvedEntity.currentTask.targetAltitudeMeters, resolvedEntity.currentTask.targetSpeedKnots
              ));
          } else if (task.taskType == "FlyHeadingAltitudeSpeed") {
              m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignFlyHeadingTask>(
                  entityName, task.targetHeadingDegrees, task.targetAltitudeMeters, task.targetSpeedKnots
              ));
          } else if (task.taskType == "FollowEntity") {
              m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignFollowTask>(
                  entityName, task.targetEntityName, task.targetAltitudeMeters, task.targetSpeedKnots
              ));
          } else if (task.taskType == "PatrolArea" || task.taskType == "OrbitArea") {
              m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignOrbitTask>(
                  entityName, task.targetLatitude, task.targetLongitude, task.targetAreaRadiusMeters,
                  task.targetAltitudeMeters, task.targetSpeedKnots, (task.taskType == "PatrolArea")
              ));
          }

          this->syncScenarioStateToUi();
        }
      });

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
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
    QVariantMap waypointSummary = makeTrackSummary(
        waypoint.name,
        QStringLiteral("Waypoint"),
        QStringLiteral("Graphic"),
        QStringLiteral("%1 m").arg(waypoint.altitudeMeters, 0, 'f', 0),
        formatPosition(waypoint.latitude, waypoint.longitude),
        QStringLiteral("Ready"),
        waypoint.latitude,
        waypoint.longitude);
    waypointSummary.insert(QStringLiteral("type"), QStringLiteral("Waypoint"));
    auto* waypointItem = new QStandardItem(waypoint.name);
    waypointItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Waypoint")));
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
    QVariantMap routeSummary = makeTrackSummary(
        route.name,
        QStringLiteral("Route"),
        QStringLiteral("Graphic"),
        QStringLiteral("-"),
        route.points.isEmpty() ? QStringLiteral("-") : formatPosition(firstPoint.latitude, firstPoint.longitude),
        QStringLiteral("%1 points").arg(route.points.size()),
        firstPoint.latitude,
        firstPoint.longitude);
    routeSummary.insert(QStringLiteral("type"), QStringLiteral("Route"));
    routeSummary.insert(QStringLiteral("routePoints"), points);
    auto* routeItem = new QStandardItem(route.name);
    routeItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Route")));
    setTrackData(routeItem, routeSummary);
    this->_tacticalGraphicsRootItem->appendRow(routeItem);
  }

  for (const AreaDefinition& area : this->_scenarioState->areas()) {
    QVariantMap areaSummary = makeTrackSummary(
        area.name,
        QStringLiteral("Area"),
        QStringLiteral("Graphic"),
        QStringLiteral("%1 m").arg(area.centerAltitudeMeters, 0, 'f', 0),
        formatPosition(area.centerLatitude, area.centerLongitude),
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
    areaItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Engagement Area")));
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
  _pendingGraphicMode = QStringLiteral("Waypoint");
  _pendingGraphicName = name;
  this->_ui->statusLabel->setText(
      QStringLiteral("Creando waypoint %1. Haz clic una vez en el mapa para fijar su posicion.")
          .arg(name));
  this->beginGraphicCoordinatePick();
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
  _pendingGraphicMode = QStringLiteral("Route");
  _pendingGraphicName = name;
  _pendingRoutePoints.clear();
  this->clearDraftGraphicFromMap(name + QStringLiteral(" (draft)"));
  this->_ui->statusLabel->setText(
      QStringLiteral("Creando route %1. Haz clic en el primer punto de la ruta en el mapa.")
          .arg(name));
  this->beginGraphicCoordinatePick();
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

  _pendingAreaType = areaType;
  _pendingGraphicName = name;
  _pendingAreaPoints.clear();
  this->clearDraftGraphicFromMap(name + QStringLiteral(" (draft)"));
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
    _pendingGraphicMode = QStringLiteral("Area");
    _pendingAreaRadiusMeters = radiusMeters;
    this->_ui->statusLabel->setText(
        QStringLiteral("Creando area circular %1. Haz clic en el mapa para fijar el centro.")
            .arg(name));
    this->beginGraphicCoordinatePick();
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
    _pendingGraphicMode = QStringLiteral("Area");
    _pendingAreaSemiMajorMeters = semiMajorMeters;
    _pendingAreaSemiMinorMeters = semiMinorMeters;
    _pendingAreaRotationDegrees = rotationDegrees;
    this->_ui->statusLabel->setText(
        QStringLiteral("Creando area eliptica %1. Haz clic en el mapa para fijar el centro.")
            .arg(name));
    this->beginGraphicCoordinatePick();
    return;
  }

  _pendingGraphicMode = QStringLiteral("AreaPolygon");
  this->_ui->statusLabel->setText(
      QStringLiteral("Creando polygon %1. Anade puntos en el mapa; para cerrar, pincha cerca del primer punto.")
          .arg(name));
  this->beginGraphicCoordinatePick();
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
