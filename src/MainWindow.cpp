#include "MainWindow.h"

#include <QFile>
#include <QLabel>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

namespace {

QString trimCopy(const QString& value) {
  return value.trimmed();
}

QString readConfigValue(const QString& key) {
  QFile file("cesium.conf");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return {};
  }

  QTextStream input(&file);
  while (!input.atEnd()) {
    const QString line = trimCopy(input.readLine());
    if (line.isEmpty() || line.startsWith('#')) {
      continue;
    }

    const int separatorIndex = line.indexOf('=');
    if (separatorIndex < 0) {
      continue;
    }

    const QString currentKey = trimCopy(line.left(separatorIndex));
    if (currentKey != key) {
      continue;
    }

    QString value = trimCopy(line.mid(separatorIndex + 1));
    if (value.startsWith('\'') && value.endsWith('\'') && value.size() >= 2) {
      value = value.mid(1, value.size() - 2);
    }
    if (value.startsWith('"') && value.endsWith('"') && value.size() >= 2) {
      value = value.mid(1, value.size() - 2);
    }
    return value;
  }

  return {};
}

QString defaultAccessToken() {
  return QStringLiteral(
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJjNWNiODVhZi1lMjRkLTQx"
      "MWEtOGUwOC00NzljMjJhMjkxZTYiLCJpZCI6MjQ2OTk4LCJpYXQiOjE3Mjg1MTI1MDd9."
      "R24O2_Qcl0RbryxLDr2WS5r7jkoxJIpdKJmg34N-Z28");
}

QString defaultAssetId() {
  return QStringLiteral("1");
}

QString buildCesiumHtml(const QString& accessToken) {
  QString escapedToken = accessToken;
  escapedToken.replace("\\", "\\\\");
  escapedToken.replace("'", "\\'");

  return QStringLiteral(R"HTML(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1.0, maximum-scale=1.0"
    />
    <title>Qt CesiumJS Preview</title>
    <link
      rel="stylesheet"
      href="https://cesium.com/downloads/cesiumjs/releases/1.118/Build/Cesium/Widgets/widgets.css"
    />
    <style>
      html, body, #cesiumContainer {
        width: 100%;
        height: 100%;
        margin: 0;
        padding: 0;
        overflow: hidden;
        background: #07111d;
      }

      .cesium-widget-credits {
        bottom: 6px !important;
      }

      #loading {
        position: absolute;
        top: 18px;
        left: 18px;
        z-index: 10;
        padding: 10px 14px;
        border-radius: 10px;
        background: rgba(10, 18, 27, 0.82);
        color: #e2ecf6;
        font: 13px/1.4 -apple-system, BlinkMacSystemFont, sans-serif;
      }
    </style>
    <script src="https://cesium.com/downloads/cesiumjs/releases/1.118/Build/Cesium/Cesium.js"></script>
  </head>
  <body>
    <div id="loading">Cargando globo 3D de CesiumJS...</div>
    <div id="cesiumContainer"></div>
    <script>
      window.addEventListener('load', async function () {
        const loading = document.getElementById('loading');
        try {
          Cesium.Ion.defaultAccessToken = '%1';

          const viewer = new Cesium.Viewer('cesiumContainer', {
            terrainProvider: await Cesium.createWorldTerrainAsync(),
            animation: false,
            baseLayerPicker: true,
            fullscreenButton: false,
            geocoder: true,
            homeButton: true,
            infoBox: false,
            sceneModePicker: false,
            selectionIndicator: false,
            timeline: false,
            navigationHelpButton: false
          });

          viewer.scene.globe.depthTestAgainstTerrain = true;
          viewer.scene.skyAtmosphere.show = true;
          viewer.scene.globe.enableLighting = true;
          viewer.camera.flyHome(0);

          loading.remove();
        } catch (error) {
          loading.textContent = 'Error al cargar CesiumJS: ' + error;
          console.error(error);
        }
      });
    </script>
  </body>
</html>
)HTML").arg(escapedToken);
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), _statusLabel(nullptr), _contentWidget(nullptr) {
  this->setWindowTitle("Qt + CesiumJS");
  this->resize(1440, 900);

  auto* central = new QWidget(this);
  auto* layout = new QVBoxLayout(central);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(10);

  const QString accessToken = []() {
    const QString configured = readConfigValue("ion_access_token");
    return configured.isEmpty() ? defaultAccessToken() : configured;
  }();
  const QString assetId = []() {
    const QString configured = readConfigValue("ion_asset_id");
    return configured.isEmpty() ? defaultAssetId() : configured;
  }();

  this->_statusLabel = new QLabel(
      QStringLiteral(
          "Vista rápida con CesiumJS dentro de Qt. Token de ion cargado y "
          "configuración encontrada para asset %1.")
          .arg(assetId),
      central);
  this->_statusLabel->setWordWrap(true);

#if defined(QT_CESIUMJS_WEBENGINE_AVAILABLE)
  auto* webView = new QWebEngineView(central);
  webView->settings()->setAttribute(
      QWebEngineSettings::LocalContentCanAccessRemoteUrls,
      true);
  webView->settings()->setAttribute(
      QWebEngineSettings::JavascriptEnabled,
      true);
  QObject::connect(webView, &QWebEngineView::loadStarted, this, [this]() {
    this->_statusLabel->setText(
        QStringLiteral("Cargando CesiumJS dentro de Qt WebEngine..."));
  });
  QObject::connect(
      webView,
      &QWebEngineView::loadFinished,
      this,
      [this](bool ok) {
        this->_statusLabel->setText(
            ok
                ? QStringLiteral(
                      "CesiumJS cargado en Qt WebEngine. Si la vista sigue "
                      "vacía, el problema ya está en la carga web del globo.")
                : QStringLiteral(
                      "Qt WebEngine no pudo terminar de cargar la página de "
                      "CesiumJS."));
      });
  webView->setHtml(
      buildCesiumHtml(accessToken),
      QUrl("https://cesium.com/"));
  this->_contentWidget = webView;
#else
  auto* fallback = new QLabel(
      QStringLiteral(
          "CesiumJS no puede activarse todavía porque tu instalación de Qt no "
          "tiene completa la cadena de WebEngine.\n\n"
          "Ahora mismo falta al menos Qt Positioning, que es una dependencia "
          "necesaria de Qt WebEngine en tu Qt 6.10.2."),
      central);
  fallback->setWordWrap(true);
  fallback->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  this->_contentWidget = fallback;
  this->_statusLabel->setText(
      QStringLiteral(
          "Qt WebEngine detectado a medias: falta Qt Positioning, por eso no "
          "se abre el globo 3D todavía."));
#endif

  layout->addWidget(this->_statusLabel);
  layout->addWidget(this->_contentWidget, 1);

  this->setCentralWidget(central);
}
