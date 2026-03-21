#include "CesiumScenePage.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTextStream>

namespace {

QString trimCopy(const QString& value) {
  return value.trimmed();
}

QString readConfigValueFromPath(const QString& path, const QString& key) {
  QFile file(path);
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

} // namespace

QString CesiumScenePage::readConfigValue(const QString& key) {
#ifdef QTTEST_SOURCE_DIR
  const QString sourceConfigPath =
      QDir(QString::fromUtf8(QTTEST_SOURCE_DIR)).absoluteFilePath(
          QStringLiteral("cesium.conf"));
  const QString sourceValue = readConfigValueFromPath(sourceConfigPath, key);
  if (!sourceValue.isEmpty()) {
    return sourceValue;
  }
#endif

  return readConfigValueFromPath(QStringLiteral("cesium.conf"), key);
}

QString CesiumScenePage::defaultAccessToken() {
  return QStringLiteral(
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJjNWNiODVhZi1lMjRkLTQx"
      "MWEtOGUwOC00NzljMjJhMjkxZTYiLCJpZCI6MjQ2OTk4LCJpYXQiOjE3Mjg1MTI1MDd9."
      "R24O2_Qcl0RbryxLDr2WS5r7jkoxJIpdKJmg34N-Z28");
}

QString CesiumScenePage::defaultAssetId() {
  return QStringLiteral("1");
}

QUrl CesiumScenePage::cesiumSourceBaseUrl() {
#ifdef QTTEST_SOURCE_DIR
  const QString sourceVendorPath =
      QDir(QString::fromUtf8(QTTEST_SOURCE_DIR)).absoluteFilePath(
          QStringLiteral("vendor/cesiumjs/"));
  if (QFileInfo::exists(sourceVendorPath + QStringLiteral("/Cesium/Cesium.js"))) {
    return QUrl::fromLocalFile(QDir(sourceVendorPath).absolutePath() + QStringLiteral("/"));
  }
#endif

  const QString cwdVendorPath =
      QDir::current().absoluteFilePath(QStringLiteral("vendor/cesiumjs/"));
  return QUrl::fromLocalFile(QDir(cwdVendorPath).absolutePath() + QStringLiteral("/"));
}

QString CesiumScenePage::buildHtml(const QString& accessToken) {
  QString escapedToken = accessToken;
  escapedToken.replace("\\", "\\\\");
  escapedToken.replace("'", "\\'");

  const QUrl vendorBaseUrl = cesiumSourceBaseUrl();
  const QString vendorBaseHref = vendorBaseUrl.toString();
  const QString cesiumBaseUrl =
      vendorBaseUrl.resolved(QUrl(QStringLiteral("Cesium/"))).toString();
  const QString scriptUrl =
      vendorBaseUrl.resolved(QUrl(QStringLiteral("Cesium/Cesium.js"))).toString();
  const QString cssUrl = vendorBaseUrl.resolved(
      QUrl(QStringLiteral("Cesium/Widgets/widgets.css"))).toString();

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
    <base href="%2" />
    <script>
      window.CESIUM_BASE_URL = '%3';
    </script>
    <link rel="stylesheet" href="%4" />
    <style>
      html, body, #cesiumContainer {
        width: 100%%;
        height: 100%%;
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
    <script src="%5"></script>
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
)HTML")
      .arg(escapedToken,
           vendorBaseHref,
           cesiumBaseUrl,
           cssUrl,
           scriptUrl);
}

QUrl CesiumScenePage::writeLocalHtmlPage(const QString& accessToken) {
  const QString tempDir =
      QStandardPaths::writableLocation(QStandardPaths::TempLocation);
  if (tempDir.isEmpty()) {
    return {};
  }

  QDir dir(tempDir);
  if (!dir.exists() && !dir.mkpath(QStringLiteral("."))) {
    return {};
  }

  const QString htmlPath =
      dir.absoluteFilePath(QStringLiteral("qttest-cesium-view.html"));
  QSaveFile file(htmlPath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return {};
  }

  const QByteArray html = buildHtml(accessToken).toUtf8();
  if (file.write(html) != html.size()) {
    return {};
  }

  if (!file.commit()) {
    return {};
  }

  return QUrl::fromLocalFile(htmlPath);
}
