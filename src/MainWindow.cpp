#include "MainWindow.h"

#include "AddEntityDialog.h"
#include "AssignTaskDialog.h"
#include "EntityDetailsDialog.h"
#include "application/ScenarioState.h"
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVariantList>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QTimer>
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

namespace {
constexpr int kTrackSummaryRole = Qt::UserRole + 1;

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
      _simulationTimer(new QTimer(this)),
      _applyingMapSelection(false),
      _simulationRunning(false)
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
  _simulationTimer->setInterval(1000);
  QObject::connect(_simulationTimer, &QTimer::timeout, this, [this]() {
    this->_scenarioState->advanceSimulation(1.0);
    this->syncScenarioStateToUi();
  });

  QObject::connect(this->_ui->actionQuit, &QAction::triggered, this, &QWidget::close);
  QObject::connect(
      this->_ui->actionAddEntity,
      &QAction::triggered,
      this,
      &MainWindow::openAddEntityDialog);
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
            this->sendTrackToMap(
                this->_friendlyRootItem->child(0)->data(kTrackSummaryRole).toMap(),
                true);
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
          QStringLiteral("2 tracks"),
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
          QStringLiteral("1 track"),
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

  const QVariantMap fighterSummary = makeTrackSummary(
      QStringLiteral("F-18 Alpha"),
      QStringLiteral("Fighter"),
      QStringLiteral("Friendly"),
      QStringLiteral("18000 m"),
      QStringLiteral("55.1032, -3.2201"),
      QStringLiteral("On station"),
      55.1032,
      -3.2201);
  QStandardItem* friendlyFighterGroup = this->ensureGroupItem(
      this->_friendlyRootItem,
      QStringLiteral("Fighter"),
      makeTrackSummary(
          QStringLiteral("Fighter"),
          QStringLiteral("Category"),
          QStringLiteral("Friendly"),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("Category"),
          0.0,
          0.0));
  auto* trackAlpha = new QStandardItem(QStringLiteral("F-18 Alpha"));
  trackAlpha->setIcon(makeTrackIcon(QStringLiteral("Friendly"), QStringLiteral("Fighter"), false));
  setTrackData(trackAlpha, fighterSummary);
  friendlyFighterGroup->appendRow(trackAlpha);

  const QVariantMap aewSummary = makeTrackSummary(
      QStringLiteral("AEW North"),
      QStringLiteral("AEW"),
      QStringLiteral("Friendly"),
      QStringLiteral("9500 m"),
      QStringLiteral("55.6200, -4.0810"),
      QStringLiteral("Orbiting"),
      55.6200,
      -4.0810);
  QStandardItem* friendlyOtherGroup = this->ensureGroupItem(
      this->_friendlyRootItem,
      QStringLiteral("Other"),
      makeTrackSummary(
          QStringLiteral("Other"),
          QStringLiteral("Category"),
          QStringLiteral("Friendly"),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("Category"),
          0.0,
          0.0));
  auto* aewNorth = new QStandardItem(QStringLiteral("AEW North"));
  aewNorth->setIcon(makeTrackIcon(QStringLiteral("Friendly"), QStringLiteral("Other"), false));
  setTrackData(aewNorth, aewSummary);
  friendlyOtherGroup->appendRow(aewNorth);

  const QVariantMap submarineSummary = makeTrackSummary(
      QStringLiteral("SSN 1"),
      QStringLiteral("Submarine"),
      QStringLiteral("Opposing"),
      QStringLiteral("-20 m"),
      QStringLiteral("54.5000, -3.2000"),
      QStringLiteral("Idle"),
      54.5000,
      -3.2000);
  QStandardItem* opposingOtherGroup = this->ensureGroupItem(
      this->_opposingRootItem,
      QStringLiteral("Other"),
      makeTrackSummary(
          QStringLiteral("Other"),
          QStringLiteral("Category"),
          QStringLiteral("Opposing"),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("Category"),
          0.0,
          0.0));
  auto* ssnOne = new QStandardItem(QStringLiteral("SSN 1"));
  ssnOne->setIcon(makeTrackIcon(QStringLiteral("Opposing"), QStringLiteral("Other"), false));
  setTrackData(ssnOne, submarineSummary);
  opposingOtherGroup->appendRow(ssnOne);

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

  const QModelIndex initialIndex = trackAlpha->index();
  this->_ui->objectsTreeView->setCurrentIndex(initialIndex);
  this->setSelectedTrackDetails(initialIndex.data(kTrackSummaryRole).toMap());
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

  this->_ui->statusLabel->setText(
      QStringLiteral("Coordenadas capturadas: lat %1, lon %2")
          .arg(latitude, 0, 'f', 5)
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
  const QString script = QStringLiteral(
      "window.addOrUpdateQtTrack && window.addOrUpdateQtTrack(%1, %2);")
                             .arg(json, focus ? QStringLiteral("true") : QStringLiteral("false"));
  this->_webView->page()->runJavaScript(script);
#else
  Q_UNUSED(summary)
  Q_UNUSED(focus)
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

void MainWindow::syncScenarioStateToUi() {
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
}

void MainWindow::openObjectsContextMenu(const QPoint& position) {
  const QModelIndex index = this->_ui->objectsTreeView->indexAt(position);
  if (!index.isValid()) {
    return;
  }

  this->_ui->objectsTreeView->setCurrentIndex(index);
  if (!this->currentSelectionIsEntity()) {
    return;
  }

  QMenu menu(this);
  QMenu* taskMenu = menu.addMenu(QStringLiteral("Task"));
  QMenu* movementMenu = taskMenu->addMenu(QStringLiteral("Movement"));
  movementMenu->addAction(QStringLiteral("Fly Heading / Altitude / Speed..."), this, &MainWindow::assignFlyHeadingAltitudeSpeedTask);
  movementMenu->addAction(QStringLiteral("Move To Location..."), this, &MainWindow::assignMoveToLocationTask);
  movementMenu->addAction(QStringLiteral("Follow Entity..."), this, &MainWindow::assignFollowEntityTask);
  taskMenu->addSeparator();
  taskMenu->addAction(QStringLiteral("Clear Current Task"), this, &MainWindow::clearSelectedTask);
  menu.addSeparator();
  menu.addAction(QStringLiteral("Entity Details..."), this, &MainWindow::openSelectedEntityDetails);
  menu.exec(this->_ui->objectsTreeView->viewport()->mapToGlobal(position));
}

void MainWindow::assignFlyHeadingAltitudeSpeedTask() {
  this->openAssignTaskDialog(QStringLiteral("FlyHeadingAltitudeSpeed"));
}

void MainWindow::assignMoveToLocationTask() {
  this->openAssignTaskDialog(QStringLiteral("MoveToLocation"));
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

void MainWindow::openAssignTaskDialog(const QString& initialTaskType) {
  if (this->_taskDialog) {
    this->_taskDialog->show();
    this->_taskDialog->raise();
    this->_taskDialog->activateWindow();
    return;
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

  QStringList availableTargets;
  for (const Entity& entity : this->_scenarioState->entities()) {
    if (entity.name != entityName) {
      availableTargets.append(entity.name);
    }
  }

  auto* dialog = new AssignTaskDialog(entityName, availableTargets, currentTask, initialTaskType, this);
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

  int routeCount = 0;
  int waypointCount = 0;
  bool engagementAreaAdded = false;

  for (const Entity& entity : this->_scenarioState->entities()) {
    const QVariantMap entitySummary = makeTrackSummary(entity);

    const bool hasRouteGraphic =
        entity.category.compare(QStringLiteral("Fighter"), Qt::CaseInsensitive) == 0 ||
        entity.type.contains(QStringLiteral("AEW"), Qt::CaseInsensitive);
    if (hasRouteGraphic) {
      ++routeCount;
      QVariantMap routeSummary = entitySummary;
      routeSummary.insert(QStringLiteral("name"), QStringLiteral("Route %1").arg(routeCount));
      routeSummary.insert(QStringLiteral("type"), QStringLiteral("Route"));
      routeSummary.insert(QStringLiteral("status"), QStringLiteral("Assigned to %1").arg(entity.name));
      auto* routeItem = new QStandardItem(routeSummary.value(QStringLiteral("name")).toString());
      routeItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Route")));
      setTrackData(routeItem, routeSummary);
      this->_tacticalGraphicsRootItem->appendRow(routeItem);
    }

    if (!engagementAreaAdded &&
        entity.category.compare(QStringLiteral("Fighter"), Qt::CaseInsensitive) == 0) {
      engagementAreaAdded = true;
      QVariantMap areaSummary = entitySummary;
      areaSummary.insert(QStringLiteral("name"), QStringLiteral("Engagement Area"));
      areaSummary.insert(QStringLiteral("type"), QStringLiteral("Engagement Area"));
      areaSummary.insert(QStringLiteral("status"), QStringLiteral("Overlay"));
      auto* areaItem = new QStandardItem(areaSummary.value(QStringLiteral("name")).toString());
      areaItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Engagement Area")));
      setTrackData(areaItem, areaSummary);
      this->_tacticalGraphicsRootItem->appendRow(areaItem);
    }

    if (entity.currentTask.enabled &&
        (entity.currentTask.taskType == QStringLiteral("MoveToLocation") ||
         entity.currentTask.taskType == QStringLiteral("FollowEntity"))) {
      ++waypointCount;
      QVariantMap waypointSummary = entitySummary;
      waypointSummary.insert(QStringLiteral("name"), QStringLiteral("Waypoint %1").arg(waypointCount));
      waypointSummary.insert(QStringLiteral("type"), QStringLiteral("Waypoint"));
      waypointSummary.insert(
          QStringLiteral("position"),
          QStringLiteral("%1, %2")
              .arg(entity.currentTask.targetLatitude, 0, 'f', 4)
              .arg(entity.currentTask.targetLongitude, 0, 'f', 4));
      waypointSummary.insert(
          QStringLiteral("status"),
          entity.currentTask.taskType == QStringLiteral("FollowEntity")
              ? QStringLiteral("Follow target: %1").arg(entity.currentTask.targetEntityName)
              : QStringLiteral("Move target for %1").arg(entity.name));
      waypointSummary.insert(QStringLiteral("latitude"), entity.currentTask.targetLatitude);
      waypointSummary.insert(QStringLiteral("longitude"), entity.currentTask.targetLongitude);
      auto* waypointItem = new QStandardItem(waypointSummary.value(QStringLiteral("name")).toString());
      waypointItem->setIcon(makeTacticalGraphicIcon(QStringLiteral("Waypoint")));
      setTrackData(waypointItem, waypointSummary);
      this->_tacticalGraphicsRootItem->appendRow(waypointItem);
    }
  }

  if (this->_tacticalGraphicsRootItem->rowCount() > 0) {
    this->_ui->objectsTreeView->expand(this->_tacticalGraphicsRootItem->index());
  }
}

void MainWindow::beginTaskCoordinatePick() {
  this->beginEntityCoordinatePick();
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
