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

#include <QAbstractItemView>
#include <QAction>
#include <QDateTime>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QColor>
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
constexpr double kGraphicAltitudeOffsetMeters = 15.0;
constexpr double kPolygonCloseDistanceMeters = 50.0;
constexpr int kTaskQuickBarMarginPixels = 14;
constexpr int kTaskQuickBarButtonPixels = 30;
constexpr int kTaskQuickBarIconPixels = 18;

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

QIcon makeTaskQuickFallbackIcon(const QString& glyph, const QColor& accent) {
  QPixmap pixmap(kTaskQuickBarIconPixels, kTaskQuickBarIconPixels);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(22, 30, 40, 220));
  painter.drawRoundedRect(pixmap.rect().adjusted(1, 1, -1, -1), 4, 4);

  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(10);
  painter.setFont(font);
  painter.setPen(accent);
  painter.drawText(pixmap.rect(), Qt::AlignCenter, glyph.left(2).toUpper());
  return QIcon(pixmap);
}

QIcon loadTaskQuickBarIcon(
    const QString& fileName,
    const QString& fallbackGlyph,
    const QColor& accent) {
  const QString path = taskQuickBarIconPath(fileName);
  if (QFileInfo::exists(path)) {
    const QIcon icon(path);
    if (!icon.isNull()) {
      return icon;
    }
  }
  return makeTaskQuickFallbackIcon(fallbackGlyph, accent);
}

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
      {QStringLiteral("destroyed"), false},
      {QStringLiteral("hidden"), false},
      {QStringLiteral("radarCoverageVisible"), false},
      {QStringLiteral("trackHistoryVisible"), false},
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
      entity.destroyed ? QStringLiteral("Destroyed") : QStringLiteral("Ready"),
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
  summary.insert(QStringLiteral("destroyed"), entity.destroyed);
  summary.insert(QStringLiteral("hidden"), entity.hidden);
  summary.insert(QStringLiteral("radarCoverageVisible"), entity.radarCoverageVisible);
  summary.insert(QStringLiteral("trackHistoryVisible"), entity.trackHistoryVisible);
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
      _pendingGraphicMode(),
      _pendingGraphicName(),
      _pendingAreaType(QStringLiteral("Circle")),
      _pendingAreaRadiusMeters(1000.0),
      _pendingAreaAltitudeMeters(0.0),
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
  this->syncDetectedContactsToUi();
  _simulationTimer->setInterval(33);
  QObject::connect(_simulationTimer, &QTimer::timeout, this, [this]() {
    this->_scenarioState->advanceSimulation(0.033);
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
  this->syncDetectedContactsToUi();
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
    area.centerAltitudeMeters = _pendingAreaAltitudeMeters;
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
          new QStandardItem(forceIdentifierLabel(target->forceIdentifier)),
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

  this->syncDetectedContactsToUi();

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
    button->setIcon(loadTaskQuickBarIcon(iconFileName, fallbackGlyph, accent));
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
  taskMenu->addSeparator();
  taskMenu->addAction(QStringLiteral("Clear Current Task"), this, &MainWindow::clearSelectedTask);
  taskMenu->setEnabled(!entityDestroyed);

  QMenu* setMenu = menu.addMenu(QStringLiteral("Set"));
  setMenu->addAction(QStringLiteral("Heading..."), this, &MainWindow::setSelectedEntityHeading);
  setMenu->addAction(QStringLiteral("Altitude..."), this, &MainWindow::setSelectedEntityAltitude);
  setMenu->addAction(QStringLiteral("Speed..."), this, &MainWindow::setSelectedEntitySpeed);
  setMenu->setEnabled(!entityDestroyed);

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

  headingDegrees = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetHeadingDegrees")).toDouble()
      : summary.value(QStringLiteral("headingDegrees")).toDouble();
  altitudeMeters = hasRunningTaskTargets
      ? summary.value(QStringLiteral("taskTargetAltitudeMeters")).toInt()
      : summary.value(QStringLiteral("altitude")).toInt();
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

  if (!this->_scenarioState->setEntityHidden(entityName, hidden)) {
    return;
  }

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

  if (!this->_scenarioState->setEntityRadarCoverageVisible(entityName, visible)) {
    return;
  }

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

  if (!this->_scenarioState->setEntityTrackHistoryVisible(entityName, visible)) {
    return;
  }

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

void MainWindow::openAssignTaskDialog(const QString& initialTaskType) {
  if (this->_taskDialog) {
    this->_taskDialog->close();
    this->_taskDialog = nullptr;
  }

  const QString entityName = this->selectedEntityName();
  if (entityName.isEmpty()) {
    return;
  }

  if (!this->currentSelectionIsOperableEntity()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("La entidad seleccionada no esta operable."));
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
            if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint") {
              double targetLat = resolvedEntity.currentTask.targetLatitude;
              double targetLon = resolvedEntity.currentTask.targetLongitude;
              double targetAlt = resolvedEntity.currentTask.targetAltitudeMeters;
              double targetSpeed = resolvedEntity.currentTask.targetSpeedKnots;
              if (task.taskType == "MoveToWaypoint" && !task.targetWaypointName.trimmed().isEmpty()) {
                for (const Waypoint& waypoint : this->_scenarioState->waypoints()) {
                  if (waypoint.name == task.targetWaypointName) {
                    targetLat = waypoint.latitude;
                    targetLon = waypoint.longitude;
                    targetAlt = waypoint.altitudeMeters;
                    break;
                  }
                }
              }
              stack->push(std::make_unique<domain::MoveToLocationTask>(
                  targetLat,
                  targetLon,
                  targetAlt,
                  targetSpeed
              ));
            } else if (task.taskType == "MoveAlongRoute") {
              bool createdRouteTask = false;
              for (const RouteGraphic& route : this->_scenarioState->routes()) {
                if (route.name != resolvedEntity.currentTask.targetRouteName ||
                    route.points.isEmpty()) {
                  continue;
                }
                stack->push(std::make_unique<domain::RouteTask>(
                    route.points,
                    resolvedEntity.currentTask.targetSpeedKnots));
                createdRouteTask = true;
                break;
              }
              if (!createdRouteTask) {
                stack->push(std::make_unique<domain::MoveToLocationTask>(
                    resolvedEntity.currentTask.targetLatitude,
                    resolvedEntity.currentTask.targetLongitude,
                    resolvedEntity.currentTask.targetAltitudeMeters,
                    resolvedEntity.currentTask.targetSpeedKnots));
              }
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
            } else if (task.taskType == "PatrolArea") {
              for (const AreaDefinition& area : this->_scenarioState->areas()) {
                if (area.name != resolvedEntity.currentTask.targetAreaName &&
                    area.id != resolvedEntity.currentTask.targetAreaName) {
                  continue;
                }
                stack->push(std::make_unique<domain::PatrolAreaTask>(
                    domain::buildPatrolRouteFromArea(area),
                    static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
                    resolvedEntity.currentTask.targetSpeedKnots
                ));
                break;
              }
              if (stack->isEmpty()) {
                stack->push(std::make_unique<domain::OrbitAreaTask>(
                    resolvedEntity.currentTask.targetLatitude,
                    resolvedEntity.currentTask.targetLongitude,
                    resolvedEntity.currentTask.targetAreaRadiusMeters,
                    static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
                    resolvedEntity.currentTask.targetSpeedKnots,
                    true
                ));
              }
            } else if (task.taskType == "OrbitArea") {
              stack->push(std::make_unique<domain::OrbitAreaTask>(
                  resolvedEntity.currentTask.targetLatitude,
                  resolvedEntity.currentTask.targetLongitude,
                  resolvedEntity.currentTask.targetAreaRadiusMeters,
                  static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
                  resolvedEntity.currentTask.targetSpeedKnots,
                  false
              ));
            }
          }

          if (!m_simulationEngine) return;

          if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint") {
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
                  entityName,
                  resolvedEntity.currentTask.targetAreaName,
                  resolvedEntity.currentTask.targetLatitude,
                  resolvedEntity.currentTask.targetLongitude,
                  resolvedEntity.currentTask.targetAreaRadiusMeters,
                  resolvedEntity.currentTask.targetAltitudeMeters,
                  resolvedEntity.currentTask.targetSpeedKnots,
                  (task.taskType == "PatrolArea")
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
  _pendingAreaAltitudeMeters = areaAltitudeMeters;
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
