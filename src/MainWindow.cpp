#include "MainWindow.h"

#include "AddEntityDialog.h"
#include "application/ScenarioState.h"
#include "domain/Entity.h"
#include "infrastructure/CesiumScenePage.h"
#include "infrastructure/MapBridge.h"
#include "infrastructure/ModelCatalog.h"
#include "presentation/EntityTextFormatter.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QFileInfo>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVariantMap>
#include <QVBoxLayout>
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
      {QStringLiteral("modelName"), QString()},
      {QStringLiteral("modelUri"), QString()},
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
  summary.insert(QStringLiteral("forceIdentifier"), entity.forceIdentifier);
  summary.insert(QStringLiteral("entityKind"), entity.entityKind);
  summary.insert(QStringLiteral("entityDomain"), entity.entityDomain);
  summary.insert(QStringLiteral("entityCountry"), entity.entityCountry);
  summary.insert(QStringLiteral("entityCategory"), entity.entityCategory);
  summary.insert(QStringLiteral("entitySubcategory"), entity.entitySubcategory);
  summary.insert(QStringLiteral("entitySpecific"), entity.entitySpecific);
  summary.insert(QStringLiteral("entityExtra"), entity.entityExtra);
  summary.insert(QStringLiteral("modelName"), entity.modelName);
  summary.insert(QStringLiteral("modelUri"), entity.modelUri);
  return summary;
}

void setTrackData(QStandardItem* item, const QVariantMap& summary) {
  item->setData(summary, kTrackSummaryRole);
}

QJsonObject mapToJsonObject(const QVariantMap& map) {
  return QJsonObject::fromVariantMap(map);
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
      _entityDialog(nullptr),
      _applyingMapSelection(false)
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
      , _webView(nullptr)
#endif
{
  this->_ui->setupUi(this);
  this->_ui->viewerHost->setMinimumSize(960, 640);
  this->_ui->viewerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  this->initializeModels();

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
  this->_ui->selectionStateValueLabel->setText(status);
  this->_ui->selectionPositionValueLabel->setText(position);
}

void MainWindow::initializeModels() {
  this->_objectsModel->setHorizontalHeaderLabels({QStringLiteral("Name")});

  this->_friendlyRootItem = new QStandardItem(QStringLiteral("Friendly"));
  this->_opposingRootItem = new QStandardItem(QStringLiteral("Opposing"));
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

  auto* trackAlpha = new QStandardItem(QStringLiteral("F-18 Alpha"));
  setTrackData(
      trackAlpha,
      makeTrackSummary(
          QStringLiteral("F-18 Alpha"),
          QStringLiteral("Fighter"),
          QStringLiteral("Friendly"),
          QStringLiteral("18000 m"),
          QStringLiteral("55.1032, -3.2201"),
          QStringLiteral("On station"),
          55.1032,
          -3.2201));
  auto* aewNorth = new QStandardItem(QStringLiteral("AEW North"));
  setTrackData(
      aewNorth,
      makeTrackSummary(
          QStringLiteral("AEW North"),
          QStringLiteral("AEW"),
          QStringLiteral("Friendly"),
          QStringLiteral("9500 m"),
          QStringLiteral("55.6200, -4.0810"),
          QStringLiteral("Orbiting"),
          55.6200,
          -4.0810));
  auto* ssnOne = new QStandardItem(QStringLiteral("SSN 1"));
  setTrackData(
      ssnOne,
      makeTrackSummary(
          QStringLiteral("SSN 1"),
          QStringLiteral("Submarine"),
          QStringLiteral("Opposing"),
          QStringLiteral("-20 m"),
          QStringLiteral("54.5000, -3.2000"),
          QStringLiteral("Idle"),
          54.5000,
          -3.2000));

  this->_friendlyRootItem->appendRow(trackAlpha);
  this->_friendlyRootItem->appendRow(aewNorth);
  this->_opposingRootItem->appendRow(ssnOne);

  this->_objectsModel->appendRow(this->_friendlyRootItem);
  this->_objectsModel->appendRow(this->_opposingRootItem);

  this->_ui->objectsTreeView->setModel(this->_objectsModel);
  this->_ui->objectsTreeView->expandAll();
  this->_ui->objectsTreeView->header()->setStretchLastSection(true);
  this->_ui->objectsTreeView->setHeaderHidden(false);

  QObject::connect(
      this->_ui->objectsTreeView->selectionModel(),
      &QItemSelectionModel::currentChanged,
      this,
      &MainWindow::updateSelectedTrackPanel);

  this->_ui->eventLogPlainTextEdit->clear();
  this->appendLogMessage(QStringLiteral("Operational log ready."));
  this->appendLogMessage(QStringLiteral("Cesium map connected."));
  this->appendLogMessage(QStringLiteral("Awaiting commands..."));

  const QModelIndex initialIndex = this->_friendlyRootItem->child(0)->index();
  this->_ui->objectsTreeView->setCurrentIndex(initialIndex);
  this->setSelectedTrackDetails(initialIndex.data(kTrackSummaryRole).toMap());
}

void MainWindow::appendEntityToUi(const Entity& entity) {
  if (!this->_friendlyRootItem) {
    return;
  }

  auto* item = new QStandardItem(entity.name);
  const QVariantMap summary = makeTrackSummary(entity);
  setTrackData(item, summary);
  this->_friendlyRootItem->appendRow(item);
  this->_ui->objectsTreeView->expand(this->_friendlyRootItem->index());
  this->_ui->objectsTreeView->setCurrentIndex(item->index());

  this->appendLogMessage(EntityTextFormatter::listLabel(entity));
  this->sendTrackToMap(summary, true);
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

  this->_ui->statusLabel->setText(
      QStringLiteral("Coordenadas capturadas: lat %1, lon %2")
          .arg(latitude, 0, 'f', 5)
          .arg(longitude, 0, 'f', 5));
}

void MainWindow::reportMapStatus(const QString& message) {
  if (!message.trimmed().isEmpty()) {
    this->_ui->statusLabel->setText(message);
  }
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
}

void MainWindow::selectObjectByName(const QString& trackName, bool notifyMap) {
  QStandardItem* item = this->findTrackItemByName(this->_friendlyRootItem, trackName);
  if (!item) {
    item = this->findTrackItemByName(this->_opposingRootItem, trackName);
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
