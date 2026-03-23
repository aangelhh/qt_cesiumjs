#include "infrastructure/CesiumScenePage.h"

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
  const QString milsymbolUrl = vendorBaseUrl.resolved(
      QUrl(QStringLiteral("../milsymbol/milsymbol.js"))).toString();

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
    <script src="qrc:///qtwebchannel/qwebchannel.js"></script>
    <script src="%5"></script>
    <script src="%6"></script>
  </head>
  <body>
    <div id="loading">Cargando globo 3D de CesiumJS...</div>
    <div id="cesiumContainer"></div>
    <script>
      let qtBridge = null;
      let viewer = null;
      let coordinatePickEnabled = false;
      let highlightedEntity = null;
      let overlaysVisible = true;
      const qtEntitiesByName = new Map();
      const qtOverlayEntitiesByName = new Map();

      function qtAffiliationCode(forceIdentifier) {
        const code = Number(forceIdentifier || 0);
        if (code === 1) {
          return '03';
        }
        if (code === 2) {
          return '06';
        }
        if (code === 3) {
          return '04';
        }
        return '00';
      }

      function qtAirFunctionCode(track) {
        const category = String(track.category || track.type || '').toLowerCase();
        const type = String(track.type || '').toLowerCase();

        if (type.includes('aew')) {
          return '110116';
        }
        if (category.includes('fighter')) {
          return '110104';
        }
        if (category.includes('bomber')) {
          return '110103';
        }
        if (category.includes('transport') || category.includes('cargo')) {
          return '110107';
        }
        if (category.includes('helicopter') || category.includes('rotary')) {
          return '110200';
        }
        return '110100';
      }

      function buildQtSidc(track) {
        const domain = String(track.domain || '').toLowerCase();
        const category = String(track.category || track.type || '').toLowerCase();
        const type = String(track.type || '').toLowerCase();
        const isAirTrack = domain.includes('air') ||
          category.includes('fighter') ||
          category.includes('bomber') ||
          category.includes('transport') ||
          category.includes('cargo') ||
          category.includes('helicopter') ||
          type.includes('aew');

        if (!isAirTrack) {
          return null;
        }

        return '10' +
          qtAffiliationCode(track.forceIdentifier) +
          '01' +
          '0' +
          '0' +
          '00' +
          qtAirFunctionCode(track) +
          '00' +
          '00';
      }

      function buildQtSymbolDataUrl(track) {
        const sidc = buildQtSidc(track);
        if (!sidc || typeof ms === 'undefined') {
          return null;
        }

        try {
          const symbol = new ms.Symbol(sidc, {
            size: 34,
            frame: true,
            fill: true,
            colorMode: 'Light'
          });
          const svg = symbol.asSVG ? symbol.asSVG() :
            (symbol.getMarker ? symbol.getMarker().XML : null);
          return svg
            ? 'data:image/svg+xml;charset=utf-8,' + encodeURIComponent(svg)
            : null;
        } catch (error) {
          console.error('No se pudo generar la simbologia APP-6:', error);
          return null;
        }
      }

      function updateLoading(message) {
        const loading = document.getElementById('loading');
        if (loading) {
          loading.textContent = message;
        }
      }

      function reportStatus(message) {
        if (qtBridge && qtBridge.reportMapStatus) {
          qtBridge.reportMapStatus(message);
        }
      }

      function trackColor(team) {
        const normalized = String(team || '').toLowerCase();
        if (normalized.includes('red') || normalized.includes('opposing')) {
          return Cesium.Color.ORANGE;
        }
        return Cesium.Color.CYAN;
      }

      function buildRoutePositions(track, altitude) {
        const lon = Number(track.longitude || 0.0);
        const lat = Number(track.latitude || 0.0);
        const kind = String(track.type || '').toLowerCase();

        if (kind.includes('aew')) {
          return Cesium.Cartesian3.fromDegreesArrayHeights([
            lon - 0.70, lat, altitude,
            lon - 0.25, lat + 0.25, altitude,
            lon + 0.25, lat + 0.25, altitude,
            lon + 0.70, lat, altitude
          ]);
        }

        if (kind.includes('sub')) {
          return Cesium.Cartesian3.fromDegreesArrayHeights([
            lon - 0.45, lat - 0.10, 0.0,
            lon - 0.10, lat - 0.02, 0.0,
            lon + 0.20, lat + 0.06, 0.0,
            lon + 0.55, lat + 0.02, 0.0
          ]);
        }

        return Cesium.Cartesian3.fromDegreesArrayHeights([
          lon - 1.00, lat - 0.40, altitude,
          lon - 0.55, lat - 0.18, altitude,
          lon - 0.18, lat - 0.05, altitude,
          lon, lat, altitude
        ]);
      }

      function buildAreaOptions(track, color, altitude) {
        const kind = String(track.type || '').toLowerCase();
        if (kind.includes('aew')) {
          return {
            semiMajorAxis: 65000.0,
            semiMinorAxis: 42000.0,
            material: color.withAlpha(0.10),
            outline: true,
            outlineColor: color.withAlpha(0.75),
            height: altitude
          };
        }

        if (kind.includes('fighter')) {
          return {
            semiMajorAxis: 28000.0,
            semiMinorAxis: 18000.0,
            material: color.withAlpha(0.08),
            outline: true,
            outlineColor: color.withAlpha(0.65),
            height: altitude
          };
        }

        return null;
      }

      function setOverlayVisibility(bundle, visible) {
        if (!bundle) {
          return;
        }
        if (bundle.route) {
          bundle.route.show = visible;
        }
        if (bundle.area) {
          bundle.area.show = visible;
        }
      }

      window.setQtOverlayVisibility = function(enabled) {
        overlaysVisible = !!enabled;
        for (const bundle of qtOverlayEntitiesByName.values()) {
          setOverlayVisibility(bundle, overlaysVisible);
        }
        reportStatus(overlaysVisible
          ? 'Overlays tacticos visibles.'
          : 'Overlays tacticos ocultos.');
        return overlaysVisible;
      };

      function applyHighlight(entity) {
        if (highlightedEntity && highlightedEntity.point) {
          highlightedEntity.point.outlineWidth = 1;
          highlightedEntity.point.pixelSize = 11;
        }
        if (highlightedEntity && highlightedEntity.billboard) {
          highlightedEntity.billboard.scale = 0.85;
        }

        highlightedEntity = entity || null;
        if (highlightedEntity && highlightedEntity.point) {
          highlightedEntity.point.outlineWidth = 3;
          highlightedEntity.point.pixelSize = 15;
        }
        if (highlightedEntity && highlightedEntity.billboard) {
          highlightedEntity.billboard.scale = 1.05;
        }
      }

      function entityNameFromPick(picked) {
        if (!Cesium.defined(picked) || !picked.id) {
          return null;
        }

        const entity = picked.id;
        if (entity.properties && entity.properties.qtTrackName) {
          return entity.properties.qtTrackName.getValue();
        }
        return entity.name || null;
      }

      window.addOrUpdateQtTrack = function(track, focus) {
        if (!viewer || !track || !track.name) {
          return false;
        }

        const longitude = Number(track.longitude || 0.0);
        const latitude = Number(track.latitude || 0.0);
        const altitudeMatch = String(track.altitude || '').match(/-?\d+(?:\.\d+)?/);
        const altitude = altitudeMatch ? Number(altitudeMatch[0]) : 0.0;
        const color = trackColor(track.team);
        const modelUri = String(track.modelUri || '');
        const hasModel = modelUri.length > 0;
        const symbolDataUrl = buildQtSymbolDataUrl(track);
        const hasSymbol = !!symbolDataUrl;
        const symbolSidc = buildQtSidc(track);
        const entityId = 'qt-track:' + track.name;
        let entity = qtEntitiesByName.get(track.name);
        let overlayBundle = qtOverlayEntitiesByName.get(track.name) || { route: null, area: null };

        const position = Cesium.Cartesian3.fromDegrees(longitude, latitude, altitude);
        if (!entity) {
          const entityOptions = {
            id: entityId,
            name: track.name,
            position,
            point: {
              show: !hasModel && !hasSymbol,
              pixelSize: 11,
              color,
              outlineColor: Cesium.Color.BLACK,
              outlineWidth: 1,
              disableDepthTestDistance: Number.POSITIVE_INFINITY,
            },
            billboard: hasSymbol ? {
              image: symbolDataUrl,
              scale: 0.85,
              verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
              pixelOffset: new Cesium.Cartesian2(0, hasModel ? -54 : -8),
              disableDepthTestDistance: Number.POSITIVE_INFINITY,
            } : undefined,
            label: {
              text: track.name,
              font: '12px sans-serif',
              style: Cesium.LabelStyle.FILL_AND_OUTLINE,
              fillColor: Cesium.Color.WHITE,
              outlineColor: Cesium.Color.BLACK,
              outlineWidth: 2,
              verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
              pixelOffset: new Cesium.Cartesian2(0, -14),
              disableDepthTestDistance: Number.POSITIVE_INFINITY,
            },
            properties: {
              qtTrackName: track.name,
              qtTrackType: track.type || '',
              qtTrackTeam: track.team || '',
              qtTrackModelName: track.modelName || '',
              qtTrackSidc: symbolSidc || '',
            },
          };
          if (hasModel) {
            entityOptions.model = {
              uri: modelUri,
              minimumPixelSize: 48,
              maximumScale: 20000,
              scale: 1.0,
            };
          }
          entity = viewer.entities.add(entityOptions);
          qtEntitiesByName.set(track.name, entity);
        } else {
          entity.position = position;
          entity.name = track.name;
          entity.point.show = !hasModel && !hasSymbol;
          entity.point.color = color;
          entity.label.text = track.name;
          entity.properties = new Cesium.PropertyBag({
            qtTrackName: track.name,
            qtTrackType: track.type || '',
            qtTrackTeam: track.team || '',
            qtTrackModelName: track.modelName || '',
            qtTrackSidc: symbolSidc || '',
          });
          if (hasModel) {
            entity.model = new Cesium.ModelGraphics({
              uri: modelUri,
              minimumPixelSize: 48,
              maximumScale: 20000,
              scale: 1.0,
            });
          } else {
            entity.model = undefined;
          }
          if (hasSymbol) {
            entity.billboard = new Cesium.BillboardGraphics({
              image: symbolDataUrl,
              scale: 0.85,
              verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
              pixelOffset: new Cesium.Cartesian2(0, hasModel ? -54 : -8),
              disableDepthTestDistance: Number.POSITIVE_INFINITY,
            });
          } else {
            entity.billboard = undefined;
          }
        }

        const routePositions = buildRoutePositions(track, altitude);
        if (!overlayBundle.route) {
          overlayBundle.route = viewer.entities.add({
            id: entityId + ':route',
            polyline: {
              positions: routePositions,
              width: 2.5,
              material: color.withAlpha(0.75),
              arcType: Cesium.ArcType.GEODESIC,
            },
            show: overlaysVisible,
          });
        } else {
          overlayBundle.route.polyline.positions = routePositions;
        }

        const areaOptions = buildAreaOptions(track, color, altitude);
        if (areaOptions) {
          if (!overlayBundle.area) {
            overlayBundle.area = viewer.entities.add({
              id: entityId + ':area',
              position,
              ellipse: areaOptions,
              show: overlaysVisible,
            });
          } else {
            overlayBundle.area.position = position;
            overlayBundle.area.ellipse.semiMajorAxis = areaOptions.semiMajorAxis;
            overlayBundle.area.ellipse.semiMinorAxis = areaOptions.semiMinorAxis;
            overlayBundle.area.ellipse.material = areaOptions.material;
            overlayBundle.area.ellipse.outline = areaOptions.outline;
            overlayBundle.area.ellipse.outlineColor = areaOptions.outlineColor;
            overlayBundle.area.ellipse.height = areaOptions.height;
          }
        } else if (overlayBundle.area) {
          viewer.entities.remove(overlayBundle.area);
          overlayBundle.area = null;
        }

        setOverlayVisibility(overlayBundle, overlaysVisible);
        qtOverlayEntitiesByName.set(track.name, overlayBundle);

        if (focus) {
          viewer.selectedEntity = entity;
          applyHighlight(entity);
          viewer.flyTo(entity, {
            duration: 1.1,
            offset: new Cesium.HeadingPitchRange(0.0, -0.7, 250000.0),
          });
        }

        return true;
      };

      window.beginQtCoordinatePick = function() {
        coordinatePickEnabled = true;
        updateLoading('Haz clic en el mapa para elegir coordenadas...');
        reportStatus('Haz clic en el mapa para elegir coordenadas.');
      };

      window.addEventListener('load', async function () {
        try {
          new QWebChannel(qt.webChannelTransport, function(channel) {
            qtBridge = channel.objects.qtBridge;
          });

          Cesium.Ion.defaultAccessToken = '%1';

          viewer = new Cesium.Viewer('cesiumContainer', {
            terrainProvider: await Cesium.createWorldTerrainAsync(),
            animation: false,
            baseLayerPicker: true,
            fullscreenButton: false,
            geocoder: true,
            homeButton: true,
            infoBox: false,
            sceneModePicker: false,
            selectionIndicator: true,
            timeline: false,
            navigationHelpButton: false
          });

          viewer.scene.globe.depthTestAgainstTerrain = true;
          viewer.scene.skyAtmosphere.show = true;
          viewer.scene.globe.enableLighting = true;
          viewer.camera.flyHome(0);

          const handler = new Cesium.ScreenSpaceEventHandler(viewer.scene.canvas);
          handler.setInputAction(function(click) {
            if (coordinatePickEnabled) {
              let cartesian;
              if (viewer.scene.pickPositionSupported) {
                cartesian = viewer.scene.pickPosition(click.position);
              }
              if (!Cesium.defined(cartesian)) {
                cartesian = viewer.camera.pickEllipsoid(
                  click.position,
                  viewer.scene.globe.ellipsoid
                );
              }
              if (!Cesium.defined(cartesian)) {
                reportStatus('No se pudo leer la coordenada de ese clic.');
                updateLoading('No se pudo leer la coordenada. Vuelve a intentarlo.');
                return;
              }

              const cartographic = Cesium.Cartographic.fromCartesian(cartesian);
              const longitude = Cesium.Math.toDegrees(cartographic.longitude);
              const latitude = Cesium.Math.toDegrees(cartographic.latitude);
              const height = cartographic.height || 0.0;

              coordinatePickEnabled = false;
              updateLoading('Coordenadas capturadas.');
              if (qtBridge && qtBridge.reportPickedCoordinate) {
                qtBridge.reportPickedCoordinate(longitude, latitude, height);
              }
              return;
            }

            const picked = viewer.scene.pick(click.position);
            const trackName = entityNameFromPick(picked);
            if (!trackName) {
              return;
            }

            const entity = qtEntitiesByName.get(trackName);
            if (entity) {
              viewer.selectedEntity = entity;
              applyHighlight(entity);
            }
            reportStatus('Track seleccionado en mapa: ' + trackName);
            if (qtBridge && qtBridge.reportSelectedTrack) {
              qtBridge.reportSelectedTrack(trackName);
            }
          }, Cesium.ScreenSpaceEventType.LEFT_CLICK);

          const loading = document.getElementById('loading');
          if (loading) {
            loading.remove();
          }
          reportStatus('CesiumJS listo. Puedes seleccionar tracks o usar Pick on map.');
        } catch (error) {
          updateLoading('Error al cargar CesiumJS: ' + error);
          reportStatus('Error al cargar CesiumJS: ' + error);
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
           scriptUrl,
           milsymbolUrl);
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
