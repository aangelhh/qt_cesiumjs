#include "MainWindow.h"
#include "CesiumScenePage.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QVBoxLayout>
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _contentWidget(nullptr),
      _mapBridge(new MapBridge(this)),
      _fighterDialog(nullptr),
      _fighterLatitudeSpin(nullptr),
      _fighterLongitudeSpin(nullptr),
      _fighterHeightSpin(nullptr)
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
      , _webView(nullptr)
#endif
{
  this->_ui->setupUi(this);
  this->_ui->viewerHost->setMinimumSize(960, 640);
  this->_ui->viewerHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QObject::connect(this->_ui->actionQuit, &QAction::triggered, this, &QWidget::close);
  QObject::connect(
      this->_ui->actionAddFighter,
      &QAction::triggered,
      this,
      &MainWindow::openAddFighterDialog);
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
                        "Pagina local de CesiumJS cargada. Ya puedes seleccionar coordenadas en el mapa.")
                  : QStringLiteral(
                        "Qt WebEngine no pudo terminar de cargar la pagina local de CesiumJS."));
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
  delete this->_ui;
}

void MainWindow::openAddFighterDialog() {
  auto* dialog = new QDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->setWindowTitle(QStringLiteral("Add Fighter"));
  dialog->setModal(false);
  dialog->setWindowModality(Qt::NonModal);

  this->_fighterDialog = dialog;

  auto* layout = new QVBoxLayout(dialog);
  auto* formLayout = new QFormLayout();

  auto* nameEdit = new QLineEdit(dialog);
  auto* typeEdit = new QLineEdit(dialog);
  auto* callsignEdit = new QLineEdit(dialog);
  auto* teamEdit = new QLineEdit(dialog);
  auto* latitudeSpin = new QDoubleSpinBox(dialog);
  auto* longitudeSpin = new QDoubleSpinBox(dialog);
  auto* heightSpin = new QDoubleSpinBox(dialog);
  auto* altitudeSpin = new QSpinBox(dialog);

  latitudeSpin->setRange(-90.0, 90.0);
  latitudeSpin->setDecimals(6);
  latitudeSpin->setSingleStep(0.001);
  latitudeSpin->setSuffix(QStringLiteral(" deg"));

  longitudeSpin->setRange(-180.0, 180.0);
  longitudeSpin->setDecimals(6);
  longitudeSpin->setSingleStep(0.001);
  longitudeSpin->setSuffix(QStringLiteral(" deg"));

  heightSpin->setRange(-1000.0, 100000.0);
  heightSpin->setDecimals(2);
  heightSpin->setSingleStep(10.0);
  heightSpin->setSuffix(QStringLiteral(" m"));

  altitudeSpin->setRange(0, 80000);
  altitudeSpin->setSuffix(QStringLiteral(" m"));
  altitudeSpin->setValue(1200);

  nameEdit->setPlaceholderText(QStringLiteral("F-18 Alpha"));
  typeEdit->setPlaceholderText(QStringLiteral("Fighter"));
  callsignEdit->setPlaceholderText(QStringLiteral("Eagle 1"));
  teamEdit->setPlaceholderText(QStringLiteral("Blue"));

  formLayout->addRow(QStringLiteral("Name"), nameEdit);
  formLayout->addRow(QStringLiteral("Type"), typeEdit);
  formLayout->addRow(QStringLiteral("Callsign"), callsignEdit);
  formLayout->addRow(QStringLiteral("Team"), teamEdit);
  formLayout->addRow(QStringLiteral("Latitude"), latitudeSpin);
  formLayout->addRow(QStringLiteral("Longitude"), longitudeSpin);
  formLayout->addRow(QStringLiteral("Ground Height"), heightSpin);
  formLayout->addRow(QStringLiteral("Altitude"), altitudeSpin);
  layout->addLayout(formLayout);

  this->_fighterLatitudeSpin = latitudeSpin;
  this->_fighterLongitudeSpin = longitudeSpin;
  this->_fighterHeightSpin = heightSpin;

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal,
      dialog);
  auto* pickOnMapButton = buttons->addButton(
      QStringLiteral("Pick on map"),
      QDialogButtonBox::ActionRole);
  layout->addWidget(buttons);

  QObject::connect(
      dialog,
      &QObject::destroyed,
      this,
      [this]() {
        this->_fighterDialog = nullptr;
        this->_fighterLatitudeSpin = nullptr;
        this->_fighterLongitudeSpin = nullptr;
        this->_fighterHeightSpin = nullptr;
      });

  QObject::connect(
      buttons,
      &QDialogButtonBox::accepted,
      this,
      [this, dialog, nameEdit, typeEdit, callsignEdit, teamEdit, latitudeSpin, longitudeSpin, altitudeSpin]() {
        const QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
          QMessageBox::warning(
              this,
              QStringLiteral("Fighter"),
              QStringLiteral("El fighter necesita al menos un nombre."));
          return;
        }

        const QString trimmedType = typeEdit->text().trimmed();
        const QString type = trimmedType.isEmpty() ? QStringLiteral("Fighter") : trimmedType;
        const QString callsign = callsignEdit->text().trimmed();
        const QString team = teamEdit->text().trimmed();

        QString label = QStringLiteral("%1 [%2]").arg(name, type);
        if (!callsign.isEmpty()) {
          label += QStringLiteral(" - %1").arg(callsign);
        }
        if (!team.isEmpty()) {
          label += QStringLiteral(" (%1)").arg(team);
        }
        label += QStringLiteral(" @ lat %1 lon %2 alt %3 m")
                     .arg(latitudeSpin->value(), 0, 'f', 4)
                     .arg(longitudeSpin->value(), 0, 'f', 4)
                     .arg(altitudeSpin->value());

        this->_ui->entitiesListWidget->addItem(label);
        this->_ui->entitiesListWidget->setCurrentRow(
            this->_ui->entitiesListWidget->count() - 1);
        this->_ui->statusLabel->setText(
            QStringLiteral("Fighter anadido: %1").arg(name));
        dialog->close();
      });

  QObject::connect(
      buttons,
      &QDialogButtonBox::rejected,
      dialog,
      &QDialog::close);
  QObject::connect(
      pickOnMapButton,
      &QPushButton::clicked,
      this,
      &MainWindow::beginFighterCoordinatePick);

  dialog->show();
  dialog->raise();
  dialog->activateWindow();
}

void MainWindow::beginFighterCoordinatePick() {
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  if (!this->_webView) {
    QMessageBox::warning(
        this,
        QStringLiteral("Pick on map"),
        QStringLiteral("El visor de CesiumJS no esta disponible ahora mismo."));
    return;
  }

  if (this->_fighterDialog) {
    this->_fighterDialog->hide();
  }

  this->_ui->statusLabel->setText(
      QStringLiteral("Haz clic en el mapa para rellenar latitud y longitud."));
  this->_webView->page()->runJavaScript(QStringLiteral("window.beginQtCoordinatePick && window.beginQtCoordinatePick();"));
#else
  QMessageBox::information(
      this,
      QStringLiteral("Pick on map"),
      QStringLiteral("Qt WebEngine no esta disponible en este build."));
#endif
}

void MainWindow::reportPickedCoordinate(double longitude, double latitude, double height) {
  if (this->_fighterLongitudeSpin) {
    this->_fighterLongitudeSpin->setValue(longitude);
  }
  if (this->_fighterLatitudeSpin) {
    this->_fighterLatitudeSpin->setValue(latitude);
  }
  if (this->_fighterHeightSpin) {
    this->_fighterHeightSpin->setValue(height);
  }

  if (this->_fighterDialog) {
    this->_fighterDialog->show();
    this->_fighterDialog->raise();
    this->_fighterDialog->activateWindow();
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
