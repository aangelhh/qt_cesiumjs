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
      let draftCursorCartographic = null;
      let highlightedEntity = null;
      let overlaysVisible = true;
      let selectedQtTrackName = null;
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

      function shouldShowTrackLine(track) {
        const speedKnots = Number(track.speedKnots || 0.0);
        const taskType = String(track.taskType || '').trim();
        return speedKnots > 1.0 || taskType.length > 0;
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
        if (bundle.label) {
          bundle.label.show = visible && bundle.trackName === selectedQtTrackName;
        }
        if (bundle.radarFans) {
          for (const fan of bundle.radarFans) {
            fan.show = visible;
          }
        }
      }

      function refreshSimulationLabelVisibility() {
        for (const bundle of qtOverlayEntitiesByName.values()) {
          setOverlayVisibility(bundle, overlaysVisible);
        }
      }

      function qtLabelMarking(track) {
        const callsign = String(track.callsign || '').trim();
        if (callsign.length > 0) {
          return callsign;
        }
        return String(track.name || 'Unknown');
      }

      function qtLabelType(track) {
        const entityTypeCode = String(track.entityTypeCode || '').trim();
        if (entityTypeCode.length > 0) {
          return entityTypeCode;
        }
        return String(track.type || 'Unknown');
      }

      function qtLabelStableNumber(seedText, minimum, span) {
        const text = String(seedText || '');
        let hash = 0;
        for (let i = 0; i < text.length; ++i) {
          hash = ((hash << 5) - hash + text.charCodeAt(i)) | 0;
        }
        return minimum + Math.abs(hash % span);
      }

      function qtLabelSpeed(track) {
        return Number(track.speedKnots || 0.0).toFixed(0) + ' kts';
      }

      function qtLabelHeading(track) {
        const heading = Number(track.headingDegrees || 0.0);
        const normalized = ((heading % 360.0) + 360.0) % 360.0;
        return normalized.toFixed(0) + ' Deg';
      }

      function qtDecimalDegreesToDdm(value, positiveSuffix, negativeSuffix) {
        const numeric = Number(value || 0);
        const absolute = Math.abs(numeric);
        const degrees = Math.floor(absolute);
        const minutes = (absolute - degrees) * 60.0;
        const suffix = numeric >= 0 ? positiveSuffix : negativeSuffix;
        return degrees + ':' + minutes.toFixed(2).padStart(5, '0') + ' ' + suffix;
      }

      function qtLabelLocation(track) {
        const latitude = qtDecimalDegreesToDdm(track.latitude, 'N', 'S');
        const longitude = qtDecimalDegreesToDdm(track.longitude, 'E', 'W');
        return latitude + ' ' + longitude + ' (DDM)';
      }

      function qtLabelAltitude(track) {
        const altitudeMatch = String(track.altitude || '').match(/-?\d+(?:\.\d+)?/);
        const meters = altitudeMatch ? Number(altitudeMatch[0]) : 0.0;
        const feet = Math.round(meters * 3.28084);
        return feet + ' ft MSL';
      }

      function destinationPoint(latitude, longitude, bearingDegrees, distanceMeters) {
        const angularDistance = distanceMeters / 6371000.0;
        const bearing = Cesium.Math.toRadians(bearingDegrees);
        const lat1 = Cesium.Math.toRadians(Number(latitude || 0.0));
        const lon1 = Cesium.Math.toRadians(Number(longitude || 0.0));

        const sinLat1 = Math.sin(lat1);
        const cosLat1 = Math.cos(lat1);
        const sinAngular = Math.sin(angularDistance);
        const cosAngular = Math.cos(angularDistance);

        const lat2 = Math.asin(
          sinLat1 * cosAngular +
          cosLat1 * sinAngular * Math.cos(bearing)
        );
        const lon2 = lon1 + Math.atan2(
          Math.sin(bearing) * sinAngular * cosLat1,
          cosAngular - sinLat1 * Math.sin(lat2)
        );

        return {
          latitude: Cesium.Math.toDegrees(lat2),
          longitude: Cesium.Math.toDegrees(lon2),
        };
      }

      function buildRadarFanPositions(track, sensor, altitude) {
        const centerBearing = Number(track.headingDegrees || 0.0) +
          Number(sensor.azimuthCenterDegrees || 0.0);
        const azimuthWidth = Math.max(1.0, Number(sensor.azimuthWidthDegrees || 360.0));
        const rangeMeters = Math.max(1.0, Number(sensor.maxRangeMeters || 0.0));
        const steps = Math.max(12, Math.ceil(azimuthWidth / 8.0));
        const startBearing = centerBearing - azimuthWidth / 2.0;
        const positions = [
          Cesium.Cartesian3.fromDegrees(
            Number(track.longitude || 0.0),
            Number(track.latitude || 0.0),
            altitude
          )
        ];

        for (let step = 0; step <= steps; ++step) {
          const bearing = startBearing + (azimuthWidth * step / steps);
          const destination = destinationPoint(
            track.latitude,
            track.longitude,
            bearing,
            rangeMeters
          );
          positions.push(
            Cesium.Cartesian3.fromDegrees(
              destination.longitude,
              destination.latitude,
              altitude
            )
          );
        }

        return positions;
      }

      function buildAreaCircleHierarchy(graphic) {
        const longitude = Number(graphic.longitude || 0.0);
        const latitude = Number(graphic.latitude || 0.0);
        const altitudeMatch = String(graphic.altitude || '').match(/-?\d+(?:\.\d+)?/);
        const altitude = altitudeMatch ? Number(altitudeMatch[0]) : 15.0;
        const radiusMeters = Math.max(1.0, Number(graphic.radiusMeters || 1000.0));
        const positions = [];
        const steps = 48;

        for (let index = 0; index <= steps; ++index) {
          const bearing = (index / steps) * 360.0;
          const point = destinationPoint(latitude, longitude, bearing, radiusMeters);
          positions.push(
            Cesium.Cartesian3.fromDegrees(
              point.longitude,
              point.latitude,
              altitude
            )
          );
        }

        return new Cesium.PolygonHierarchy(positions);
      }

      function buildAreaEllipseHierarchy(graphic) {
        const longitude = Number(graphic.longitude || 0.0);
        const latitude = Number(graphic.latitude || 0.0);
        const altitudeMatch = String(graphic.altitude || '').match(/-?\d+(?:\.\d+)?/);
        const altitude = altitudeMatch ? Number(altitudeMatch[0]) : 15.0;
        const semiMajorAxisMeters = Math.max(1.0, Number(graphic.semiMajorAxisMeters || 1000.0));
        const semiMinorAxisMeters = Math.max(1.0, Number(graphic.semiMinorAxisMeters || 600.0));
        const rotationRadians = Cesium.Math.toRadians(Number(graphic.rotationDegrees || 0.0));
        const positions = [];
        const steps = 64;

        for (let index = 0; index <= steps; ++index) {
          const theta = (index / steps) * Cesium.Math.TWO_PI;
          const x = semiMajorAxisMeters * Math.cos(theta);
          const y = semiMinorAxisMeters * Math.sin(theta);
          const rotatedX = (x * Math.cos(rotationRadians)) - (y * Math.sin(rotationRadians));
          const rotatedY = (x * Math.sin(rotationRadians)) + (y * Math.cos(rotationRadians));
          const distanceMeters = Math.sqrt((rotatedX * rotatedX) + (rotatedY * rotatedY));
          const bearing = Cesium.Math.toDegrees(Math.atan2(rotatedX, rotatedY));
          const point = destinationPoint(latitude, longitude, bearing, distanceMeters);
          positions.push(
            Cesium.Cartesian3.fromDegrees(
              point.longitude,
              point.latitude,
              altitude
            )
          );
        }

        return new Cesium.PolygonHierarchy(positions);
      }

      function buildAreaPolygonHierarchy(graphic) {
        const points = Array.isArray(graphic.areaPoints) ? graphic.areaPoints : [];
        const positions = points.map(function(point) {
          return Cesium.Cartesian3.fromDegrees(
            Number(point.longitude || 0.0),
            Number(point.latitude || 0.0),
            Math.max(15.0, Number(point.altitudeMeters || 0.0))
          );
        });
        return new Cesium.PolygonHierarchy(positions);
      }

      function radarSensorsForTrack(track) {
        if (!Array.isArray(track.sensors)) {
          return [];
        }
        return track.sensors.filter(function(sensor) {
          return sensor &&
            String(sensor.sensorType || '').toLowerCase() === 'radar' &&
            sensor.enabled !== false &&
            sensor.emitting !== false &&
            Number(sensor.maxRangeMeters || 0.0) > 0.0;
        });
      }

      function buildSimulationObjectLabel(track) {
        return [
          'Marking : ' + qtLabelMarking(track),
          'Type    : ' + qtLabelType(track),
          'Force   : ' + String(track.team || 'Unknown'),
          'Name    : ' + String(track.name || 'Unknown'),
          'Speed   : ' + qtLabelSpeed(track),
          'Heading : ' + qtLabelHeading(track),
          'Location: ' + qtLabelLocation(track),
          'Altitude: ' + qtLabelAltitude(track),
        ].join('\n');
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
        selectedQtTrackName = highlightedEntity && highlightedEntity.name
          ? highlightedEntity.name
          : null;
        refreshSimulationLabelVisibility();
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

      function createInterpolatedMotionState(initialPosition) {
        return {
          previousPosition: Cesium.Cartesian3.clone(initialPosition),
          targetPosition: Cesium.Cartesian3.clone(initialPosition),
          startTimeMs: performance.now(),
          durationMs: 150.0,
        };
      }

      function motionStatePosition(motionState) {
        const elapsed = performance.now() - motionState.startTimeMs;
        const t = Cesium.Math.clamp(elapsed / motionState.durationMs, 0.0, 1.0);
        return Cesium.Cartesian3.lerp(
          motionState.previousPosition,
          motionState.targetPosition,
          t,
          new Cesium.Cartesian3()
        );
      }

      function installInterpolatedPosition(entity, initialPosition) {
        const motionState = createInterpolatedMotionState(initialPosition);
        entity._qtMotionState = motionState;
        entity.position = new Cesium.CallbackProperty(function() {
          return motionStatePosition(motionState);
        }, false);
      }

      function predictTrackPosition(track, altitude, secondsAhead) {
        if (!track || !secondsAhead || secondsAhead <= 0.0) {
          return Cesium.Cartesian3.fromDegrees(
            Number(track && track.longitude || 0.0),
            Number(track && track.latitude || 0.0),
            altitude
          );
        }

        const longitude = Number(track.longitude || 0.0);
        const latitude = Number(track.latitude || 0.0);
        const headingRadians = Cesium.Math.toRadians(Number(track.headingDegrees || 0.0));
        const speedMetersPerSecond = Number(track.speedKnots || 0.0) * 0.514444;
        const surfaceDistance = speedMetersPerSecond * secondsAhead;
        const earthRadiusMeters = 6371000.0;
        const latitudeRadians = Cesium.Math.toRadians(latitude);
        const longitudeRadians = Cesium.Math.toRadians(longitude);
        const angularDistance = surfaceDistance / earthRadiusMeters;
        const predictedLatitudeRadians = Math.asin(
          Math.sin(latitudeRadians) * Math.cos(angularDistance) +
          Math.cos(latitudeRadians) * Math.sin(angularDistance) * Math.cos(headingRadians)
        );
        const predictedLongitudeRadians = longitudeRadians + Math.atan2(
          Math.sin(headingRadians) * Math.sin(angularDistance) * Math.cos(latitudeRadians),
          Math.cos(angularDistance) - Math.sin(latitudeRadians) * Math.sin(predictedLatitudeRadians)
        );

        return Cesium.Cartesian3.fromRadians(
          predictedLongitudeRadians,
          predictedLatitudeRadians,
          altitude
        );
      }

      function predictedEntityPosition(track, nextPosition) {
        if (!track) {
          return nextPosition;
        }

        const altitudeMatch = String(track.altitude || '').match(/-?\d+(?:\.\d+)?/);
        const altitude = altitudeMatch ? Number(altitudeMatch[0]) : 0.0;
        const speedKnots = Number(track.speedKnots || 0.0);
        if (speedKnots <= 1.0) {
          return nextPosition;
        }

        return predictTrackPosition(track, altitude, 0.18);
      }

      function updateInterpolatedPosition(entity, nextPosition, track) {
        if (!entity._qtMotionState) {
          installInterpolatedPosition(entity, nextPosition);
          return;
        }

        const motionState = entity._qtMotionState;
        motionState.previousPosition = motionStatePosition(motionState);
        motionState.targetPosition = Cesium.Cartesian3.clone(
          predictedEntityPosition(track, nextPosition)
        );
        motionState.startTimeMs = performance.now();
        motionState.durationMs = 150.0;
      }

      function currentEntityCartesian(entity) {
        if (!entity || !entity.position) {
          return null;
        }
        if (typeof entity.position.getValue === 'function') {
          return entity.position.getValue(Cesium.JulianDate.now());
        }
        return entity.position;
      }

      function buildRoutePositionsForEntity(entity) {
        const cartesian = currentEntityCartesian(entity);
        const track = entity && entity._qtTrackData ? entity._qtTrackData : null;
        if (!cartesian || !track) {
          return null;
        }

        const cartographic = Cesium.Cartographic.fromCartesian(cartesian);
        if (!cartographic) {
          return null;
        }

        return buildRoutePositions(
          {
            longitude: Cesium.Math.toDegrees(cartographic.longitude),
            latitude: Cesium.Math.toDegrees(cartographic.latitude),
            type: track.type || '',
          },
          cartographic.height || 0.0
        );
      }

      function buildRadarFanHierarchyForEntity(entity, sensor) {
        const cartesian = currentEntityCartesian(entity);
        const track = entity && entity._qtTrackData ? entity._qtTrackData : null;
        if (!cartesian || !track || !sensor) {
          return null;
        }

        const cartographic = Cesium.Cartographic.fromCartesian(cartesian);
        if (!cartographic) {
          return null;
        }

        const longitude = Cesium.Math.toDegrees(cartographic.longitude);
        const latitude = Cesium.Math.toDegrees(cartographic.latitude);
        const altitude = cartographic.height || 0.0;
        const syntheticTrack = Object.assign({}, track, {
          longitude: longitude,
          latitude: latitude,
          headingDegrees: Number(track.headingDegrees || 0.0),
        });
        return new Cesium.PolygonHierarchy(
          buildRadarFanPositions(syntheticTrack, sensor, altitude)
        );
      }

      function buildEntityOrientationProperty(entity) {
        return new Cesium.CallbackProperty(function(time) {
          const position = currentEntityCartesian(entity);
          const track = entity && entity._qtTrackData ? entity._qtTrackData : null;
          if (!position || !track) {
            return Cesium.Quaternion.IDENTITY;
          }

          const headingRadians = Cesium.Math.toRadians(Number(track.headingDegrees || 0.0));
          const headingPitchRoll = new Cesium.HeadingPitchRoll(headingRadians, 0.0, 0.0);
          return Cesium.Transforms.headingPitchRollQuaternion(
            position,
            headingPitchRoll
          );
        }, false);
      }

      const qtGraphicsByName = new Map();
      const qtDraftGraphicsByName = new Map();

      function parseGraphicAltitude(graphic, fallbackAltitude) {
        const altitudeMatch = String(graphic.altitude || '').match(/-?\d+(?:\.\d+)?/);
        if (altitudeMatch) {
          return Number(altitudeMatch[0]);
        }
        return Number(fallbackAltitude || 15.0);
      }

      function cartesianFromPointSummary(point, fallbackAltitude) {
        return Cesium.Cartesian3.fromDegrees(
          Number(point.longitude || 0.0),
          Number(point.latitude || 0.0),
          Math.max(15.0, Number(point.altitudeMeters || fallbackAltitude || 15.0))
        );
      }

      function cartographicFromWindowPosition(windowPosition) {
        if (!viewer || !windowPosition) {
          return null;
        }

        let cartesian;
        if (viewer.scene.pickPositionSupported) {
          cartesian = viewer.scene.pickPosition(windowPosition);
        }
        if (!Cesium.defined(cartesian)) {
          cartesian = viewer.camera.pickEllipsoid(
            windowPosition,
            viewer.scene.globe.ellipsoid
          );
        }
        if (!Cesium.defined(cartesian)) {
          return null;
        }
        return Cesium.Cartographic.fromCartesian(cartesian);
      }

      function draftCursorPointSummary(fallbackAltitude) {
        if (!draftCursorCartographic) {
          return null;
        }
        return {
          longitude: Cesium.Math.toDegrees(draftCursorCartographic.longitude),
          latitude: Cesium.Math.toDegrees(draftCursorCartographic.latitude),
          altitudeMeters: Math.max(15.0, Number(draftCursorCartographic.height || fallbackAltitude || 15.0)),
        };
      }

      function removeDraftGraphicBundle(bundle) {
        if (!bundle || !viewer) {
          return;
        }
        if (bundle.main) {
          viewer.entities.remove(bundle.main);
        }
        if (bundle.points) {
          bundle.points.forEach(function(pointEntity) {
            viewer.entities.remove(pointEntity);
          });
        }
      }

      function syncDraftPointEntities(bundle, graphicName, pointSummaries, color) {
        if (!bundle.points) {
          bundle.points = [];
        }

        while (bundle.points.length > pointSummaries.length) {
          const pointEntity = bundle.points.pop();
          viewer.entities.remove(pointEntity);
        }

        pointSummaries.forEach(function(pointSummary, index) {
          const position = cartesianFromPointSummary(pointSummary, 15.0);
          const isFirstPoint = index === 0;
          const pointColor = isFirstPoint
            ? Cesium.Color.WHITE.withAlpha(0.95)
            : color.withAlpha(0.92);
          const outlineColor = isFirstPoint
            ? color.withAlpha(0.98)
            : Cesium.Color.BLACK.withAlpha(0.85);

          let pointEntity = bundle.points[index];
          if (!pointEntity) {
            pointEntity = viewer.entities.add({
              id: 'qt-draft-graphic:' + graphicName + ':point:' + index,
              position: position,
              point: {
                pixelSize: isFirstPoint ? 16 : 11,
                color: pointColor,
                outlineColor: outlineColor,
                outlineWidth: isFirstPoint ? 3 : 2,
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              },
              label: isFirstPoint ? {
                text: 'Start / Close',
                font: '12px sans-serif',
                style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                fillColor: Cesium.Color.WHITE,
                outlineColor: Cesium.Color.BLACK,
                outlineWidth: 2,
                verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                pixelOffset: new Cesium.Cartesian2(0, -16),
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              } : undefined,
            });
            bundle.points.push(pointEntity);
          } else {
            pointEntity.position = position;
            pointEntity.point.pixelSize = isFirstPoint ? 16 : 11;
            pointEntity.point.color = pointColor;
            pointEntity.point.outlineColor = outlineColor;
            pointEntity.point.outlineWidth = isFirstPoint ? 3 : 2;
            pointEntity.label = isFirstPoint ? new Cesium.LabelGraphics({
              text: 'Start / Close',
              font: '12px sans-serif',
              style: Cesium.LabelStyle.FILL_AND_OUTLINE,
              fillColor: Cesium.Color.WHITE,
              outlineColor: Cesium.Color.BLACK,
              outlineWidth: 2,
              verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
              pixelOffset: new Cesium.Cartesian2(0, -16),
              disableDepthTestDistance: Number.POSITIVE_INFINITY,
            }) : undefined;
          }
        });
      }

      function refreshAllDraftGraphics() {
        qtDraftGraphicsByName.forEach(function(bundle) {
          if (bundle && bundle.graphic) {
            window.addOrUpdateQtDraftGraphic(bundle.graphic);
          }
        });
      }

      function graphicColor(type) {
        const normalized = String(type || '').toLowerCase();
        if (normalized === 'area') {
          return Cesium.Color.fromCssColorString('#ffd166');
        }
        if (normalized === 'route') {
          return Cesium.Color.fromCssColorString('#ff6c52');
        }
        return Cesium.Color.fromCssColorString('#9ef06a');
      }

      window.addOrUpdateQtGraphic = function(graphic, focus) {
        if (!viewer || !graphic || !graphic.name) {
          return false;
        }

        const type = String(graphic.type || '');
        const color = graphicColor(type);
        let entity = qtGraphicsByName.get(graphic.name);

        if (type === 'Waypoint') {
          const longitude = Number(graphic.longitude || 0.0);
          const latitude = Number(graphic.latitude || 0.0);
          const altitudeMatch = String(graphic.altitude || '').match(/-?\d+(?:\.\d+)?/);
          const altitude = altitudeMatch ? Number(altitudeMatch[0]) : 15.0;
          const position = Cesium.Cartesian3.fromDegrees(longitude, latitude, altitude);
          if (!entity) {
            entity = viewer.entities.add({
              id: 'qt-graphic:' + graphic.name,
              name: graphic.name,
              position: position,
              point: {
                pixelSize: 10,
                color: color,
                outlineColor: Cesium.Color.BLACK,
                outlineWidth: 1,
                heightReference: Cesium.HeightReference.NONE,
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              },
              label: {
                text: graphic.name,
                font: '12px sans-serif',
                style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                fillColor: Cesium.Color.WHITE,
                outlineColor: Cesium.Color.BLACK,
                outlineWidth: 2,
                verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                pixelOffset: new Cesium.Cartesian2(0, -12),
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              },
            });
          } else {
            entity.position = position;
            entity.label.text = graphic.name;
            entity.point.color = color;
          }
        } else if (type === 'Route') {
          const points = Array.isArray(graphic.routePoints) ? graphic.routePoints : [];
          const positions = points.map(function(point) {
            return Cesium.Cartesian3.fromDegrees(
              Number(point.longitude || 0.0),
              Number(point.latitude || 0.0),
              Math.max(15.0, Number(point.altitudeMeters || 0.0))
            );
          });
          if (!entity) {
            entity = viewer.entities.add({
              id: 'qt-graphic:' + graphic.name,
              name: graphic.name,
              polyline: {
                positions: positions,
                width: 3.0,
                material: color,
                arcType: Cesium.ArcType.GEODESIC,
                clampToGround: false,
              },
              label: positions.length > 0 ? {
                text: graphic.name,
                font: '12px sans-serif',
                style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                fillColor: Cesium.Color.WHITE,
                outlineColor: Cesium.Color.BLACK,
                outlineWidth: 2,
                verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                pixelOffset: new Cesium.Cartesian2(0, -12),
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              } : undefined,
              position: positions.length > 0 ? positions[0] : undefined,
            });
          } else {
            entity.polyline.positions = positions;
            entity.position = positions.length > 0 ? positions[0] : undefined;
            if (entity.label) {
              entity.label.text = graphic.name;
            }
          }
        } else if (type === 'Area') {
          const longitude = Number(graphic.longitude || 0.0);
          const latitude = Number(graphic.latitude || 0.0);
          const altitudeMatch = String(graphic.altitude || '').match(/-?\d+(?:\.\d+)?/);
          const altitude = altitudeMatch ? Number(altitudeMatch[0]) : 15.0;
          const position = Cesium.Cartesian3.fromDegrees(longitude, latitude, altitude);
          const areaType = String(graphic.areaType || 'Circle');
          let hierarchy = buildAreaCircleHierarchy(graphic);
          if (areaType === 'Ellipse') {
            hierarchy = buildAreaEllipseHierarchy(graphic);
          } else if (areaType === 'Polygon') {
            hierarchy = buildAreaPolygonHierarchy(graphic);
          }
          if (!entity) {
            entity = viewer.entities.add({
              id: 'qt-graphic:' + graphic.name,
              name: graphic.name,
              position: position,
              polygon: {
                hierarchy: hierarchy,
                material: color.withAlpha(0.18),
                outline: true,
                outlineColor: color.withAlpha(0.75),
                perPositionHeight: true,
              },
              label: {
                text: graphic.name,
                font: '12px sans-serif',
                style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                fillColor: Cesium.Color.WHITE,
                outlineColor: Cesium.Color.BLACK,
                outlineWidth: 2,
                verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                pixelOffset: new Cesium.Cartesian2(0, -12),
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              },
            });
          } else {
            entity.position = position;
            entity.polygon.hierarchy = hierarchy;
            entity.polygon.material = color.withAlpha(0.18);
            entity.polygon.outlineColor = color.withAlpha(0.75);
            if (entity.label) {
              entity.label.text = graphic.name;
            }
          }
        } else {
          return false;
        }

        qtGraphicsByName.set(graphic.name, entity);

        if (focus && entity) {
          viewer.flyTo(entity, {
            duration: 1.0,
            offset: new Cesium.HeadingPitchRange(0.0, -0.7, 150000.0),
          });
        }
        return true;
      };

      window.addOrUpdateQtDraftGraphic = function(graphic) {
        if (!viewer || !graphic || !graphic.name) {
          return false;
        }

        let bundle = qtDraftGraphicsByName.get(graphic.name);
        if (!bundle) {
          bundle = {
            main: null,
            points: [],
            graphic: null,
          };
        }
        bundle.graphic = Object.assign({}, graphic);

        const color = Cesium.Color.fromCssColorString('#ffe082');
        const longitude = Number(graphic.longitude || 0.0);
        const latitude = Number(graphic.latitude || 0.0);
        const altitude = parseGraphicAltitude(graphic, 15.0);
        const position = Cesium.Cartesian3.fromDegrees(longitude, latitude, altitude);

        const type = String(graphic.type || 'Area');
        if (type === 'Route') {
          const routePoints = Array.isArray(graphic.routePoints) ? graphic.routePoints.slice() : [];
          const cursorPoint = routePoints.length === 1 ? draftCursorPointSummary(altitude) : null;
          if (cursorPoint) {
            routePoints.push(cursorPoint);
          }
          const positions = routePoints.map(function(point) {
            return cartesianFromPointSummary(point, altitude);
          });

          if (!bundle.main) {
            bundle.main = viewer.entities.add({
              id: 'qt-draft-graphic:' + graphic.name,
              name: graphic.name,
              position: position,
              polyline: {
                positions: positions,
                width: 3.0,
                material: color,
                arcType: Cesium.ArcType.GEODESIC,
                clampToGround: false,
              },
              label: {
                text: graphic.name,
                font: '12px sans-serif',
                style: Cesium.LabelStyle.FILL_AND_OUTLINE,
                fillColor: Cesium.Color.WHITE,
                outlineColor: Cesium.Color.BLACK,
                outlineWidth: 2,
                verticalOrigin: Cesium.VerticalOrigin.BOTTOM,
                pixelOffset: new Cesium.Cartesian2(0, -12),
                disableDepthTestDistance: Number.POSITIVE_INFINITY,
              },
            });
          } else {
            bundle.main.position = position;
            bundle.main.polyline.positions = positions;
            if (bundle.main.label) {
              bundle.main.label.text = graphic.name;
            }
          }

          syncDraftPointEntities(
            bundle,
            graphic.name,
            Array.isArray(graphic.routePoints) ? graphic.routePoints : [],
            color);
          qtDraftGraphicsByName.set(graphic.name, bundle);
          return true;
        }

        const areaType = String(graphic.areaType || 'Polygon');
        const areaPoints = Array.isArray(graphic.areaPoints) ? graphic.areaPoints.slice() : [];
        const cursorPoint =
          areaType === 'Polygon' && areaPoints.length >= 1 ? draftCursorPointSummary(altitude) : null;
        const previewPoints = areaPoints.slice();
        if (cursorPoint) {
          previewPoints.push(cursorPoint);
        }

        let hierarchy = buildAreaCircleHierarchy(graphic);
        if (areaType === 'Ellipse') {
          hierarchy = buildAreaEllipseHierarchy(graphic);
        } else if (areaType === 'Polygon') {
          hierarchy = new Cesium.PolygonHierarchy(previewPoints.map(function(point) {
            return cartesianFromPointSummary(point, altitude);
          }));
        }

        const polygonEnabled = areaType !== 'Polygon' || previewPoints.length >= 3;
        const polylinePositions =
          areaType === 'Polygon'
            ? previewPoints.map(function(point) {
                return cartesianFromPointSummary(point, altitude);
              })
            : [];

        if (!bundle.main) {
          bundle.main = viewer.entities.add({
            id: 'qt-draft-graphic:' + graphic.name,
            name: graphic.name,
            position: position,
            polygon: polygonEnabled ? {
              hierarchy: hierarchy,
              material: color.withAlpha(0.12),
              outline: true,
              outlineColor: color.withAlpha(0.95),
              perPositionHeight: true,
            } : undefined,
            polyline: areaType === 'Polygon' ? {
              positions: polylinePositions,
              width: 2.0,
              material: color,
              clampToGround: false,
            } : undefined,
          });
        } else {
          bundle.main.position = position;
          bundle.main.polygon = polygonEnabled ? new Cesium.PolygonGraphics({
            hierarchy: hierarchy,
            material: color.withAlpha(0.12),
            outline: true,
            outlineColor: color.withAlpha(0.95),
            perPositionHeight: true,
          }) : undefined;
          bundle.main.polyline = areaType === 'Polygon' ? new Cesium.PolylineGraphics({
            positions: polylinePositions,
            width: 2.0,
            material: color,
            clampToGround: false,
          }) : undefined;
        }

        syncDraftPointEntities(bundle, graphic.name, areaPoints, color);
        qtDraftGraphicsByName.set(graphic.name, bundle);
        return true;
      };

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
        let overlayBundle = qtOverlayEntitiesByName.get(track.name) || {
          route: null,
          area: null,
          label: null,
          radarFans: [],
          trackName: track.name,
        };
        overlayBundle.trackName = track.name;

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
            orientation: undefined,
          };
          if (hasModel) {
            entityOptions.model = {
              uri: modelUri,
              minimumPixelSize: 48,
              maximumScale: 20000,
              scale: 1.0,
            };
            entityOptions.orientation = buildEntityOrientationProperty({
              _qtTrackData: track,
              position: {
                getValue: function() { return position; }
              }
            });
          }
          entity = viewer.entities.add(entityOptions);
          installInterpolatedPosition(entity, position);
          entity._qtTrackData = Object.assign({}, track);
          if (hasModel) {
            entity.orientation = buildEntityOrientationProperty(entity);
          }
          qtEntitiesByName.set(track.name, entity);
        } else {
          updateInterpolatedPosition(entity, position, track);
          entity.name = track.name;
          entity._qtTrackData = Object.assign({}, track);
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
            entity.orientation = buildEntityOrientationProperty(entity);
          } else {
            entity.model = undefined;
            entity.orientation = undefined;
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

        if (!entity._qtMotionState) {
          updateInterpolatedPosition(entity, position, track);
        }

        if (shouldShowTrackLine(track)) {
          if (!overlayBundle.route) {
            overlayBundle.route = viewer.entities.add({
              id: entityId + ':route',
              polyline: {
                positions: new Cesium.CallbackProperty(function() {
                  return buildRoutePositionsForEntity(entity);
                }, false),
                width: 2.5,
                material: color.withAlpha(0.75),
                arcType: Cesium.ArcType.GEODESIC,
              },
              show: overlaysVisible,
            });
          }
        } else if (overlayBundle.route) {
          viewer.entities.remove(overlayBundle.route);
          overlayBundle.route = null;
        }

        const areaOptions = buildAreaOptions(track, color, altitude);
        if (areaOptions) {
          if (!overlayBundle.area) {
            overlayBundle.area = viewer.entities.add({
              id: entityId + ':area',
              position: entity.position,
              ellipse: areaOptions,
              show: overlaysVisible,
            });
          } else {
            overlayBundle.area.position = entity.position;
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

        const simulationLabelText = buildSimulationObjectLabel(track);
        if (!overlayBundle.label) {
          overlayBundle.label = viewer.entities.add({
            id: entityId + ':label',
            position: entity.position,
            label: {
              text: simulationLabelText,
              font: '15px monospace',
              showBackground: true,
              backgroundColor: new Cesium.Color(0.02, 0.08, 0.92, 0.72),
              fillColor: Cesium.Color.WHITE,
              outlineColor: Cesium.Color.BLACK,
              outlineWidth: 1,
              style: Cesium.LabelStyle.FILL_AND_OUTLINE,
              horizontalOrigin: Cesium.HorizontalOrigin.LEFT,
              verticalOrigin: Cesium.VerticalOrigin.TOP,
              pixelOffset: new Cesium.Cartesian2(72, 56),
              eyeOffset: new Cesium.Cartesian3(0.0, 0.0, -20.0),
              disableDepthTestDistance: Number.POSITIVE_INFINITY,
            },
            properties: {
              qtTrackName: track.name,
            },
            show: overlaysVisible && track.name === selectedQtTrackName,
          });
        } else {
          overlayBundle.label.position = entity.position;
          overlayBundle.label.label.text = simulationLabelText;
        }

        const radarSensors = radarSensorsForTrack(track);
        if (overlayBundle.radarFans && overlayBundle.radarFans.length > radarSensors.length) {
          for (let index = radarSensors.length; index < overlayBundle.radarFans.length; ++index) {
            viewer.entities.remove(overlayBundle.radarFans[index]);
          }
          overlayBundle.radarFans.length = radarSensors.length;
        }

        radarSensors.forEach(function(sensor, index) {
          const radarColor = color.withAlpha(0.12 + Math.min(index, 3) * 0.04);
          const outlineColor = color.withAlpha(0.65);
          if (!overlayBundle.radarFans[index]) {
            overlayBundle.radarFans[index] = viewer.entities.add({
              id: entityId + ':radar:' + index,
              polygon: {
                hierarchy: new Cesium.CallbackProperty(function() {
                  return buildRadarFanHierarchyForEntity(entity, sensor);
                }, false),
                material: radarColor,
                outline: true,
                outlineColor,
                perPositionHeight: true,
              },
              show: overlaysVisible,
            });
          } else {
            overlayBundle.radarFans[index].polygon.material = radarColor;
            overlayBundle.radarFans[index].polygon.outlineColor = outlineColor;
          }
        });

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

      window.removeQtTrack = function(trackName) {
        if (!viewer || !trackName) {
          return false;
        }

        const entity = qtEntitiesByName.get(trackName);
        if (entity) {
          viewer.entities.remove(entity);
          qtEntitiesByName.delete(trackName);
        }

        const overlayBundle = qtOverlayEntitiesByName.get(trackName);
        if (overlayBundle) {
          if (overlayBundle.route) {
            viewer.entities.remove(overlayBundle.route);
          }
          if (overlayBundle.area) {
            viewer.entities.remove(overlayBundle.area);
          }
          if (overlayBundle.label) {
            viewer.entities.remove(overlayBundle.label);
          }
          if (overlayBundle.radarFans) {
            for (const fan of overlayBundle.radarFans) {
              viewer.entities.remove(fan);
            }
          }
          qtOverlayEntitiesByName.delete(trackName);
        }

        if (selectedQtTrackName === trackName) {
          selectedQtTrackName = null;
          applyHighlight(null);
        }
        return true;
      };

      window.removeQtGraphic = function(graphicName) {
        if (!viewer || !graphicName) {
          return false;
        }
        const entity = qtGraphicsByName.get(graphicName);
        if (entity) {
          viewer.entities.remove(entity);
          qtGraphicsByName.delete(graphicName);
        }
        return true;
      };

      window.removeQtDraftGraphic = function(graphicName) {
        if (!viewer || !graphicName) {
          return false;
        }
        const bundle = qtDraftGraphicsByName.get(graphicName);
        if (bundle) {
          removeDraftGraphicBundle(bundle);
          qtDraftGraphicsByName.delete(graphicName);
        }
        return true;
      };

      window.beginQtCoordinatePick = function() {
        coordinatePickEnabled = true;
        draftCursorCartographic = null;
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
          viewer.clock.shouldAnimate = true;
          viewer.camera.flyHome(0);

          const handler = new Cesium.ScreenSpaceEventHandler(viewer.scene.canvas);
          handler.setInputAction(function(movement) {
            if (!coordinatePickEnabled || qtDraftGraphicsByName.size === 0) {
              draftCursorCartographic = null;
              return;
            }
            draftCursorCartographic = cartographicFromWindowPosition(movement.endPosition);
            refreshAllDraftGraphics();
          }, Cesium.ScreenSpaceEventType.MOUSE_MOVE);

          handler.setInputAction(function(click) {
            if (coordinatePickEnabled) {
              let cartographic = null;
              const pickedDraft = viewer.scene.pick(click.position);
              if (Cesium.defined(pickedDraft) &&
                  Cesium.defined(pickedDraft.id) &&
                  typeof pickedDraft.id.id === 'string' &&
                  pickedDraft.id.id.indexOf('qt-draft-graphic:') === 0 &&
                  pickedDraft.id.id.indexOf(':point:0') !== -1) {
                const pickedPosition = pickedDraft.id.position
                  ? pickedDraft.id.position.getValue(Cesium.JulianDate.now())
                  : null;
                if (Cesium.defined(pickedPosition)) {
                  cartographic = Cesium.Cartographic.fromCartesian(pickedPosition);
                }
              }

              if (!cartographic) {
                cartographic = cartographicFromWindowPosition(click.position);
              }
              if (!cartographic) {
                reportStatus('No se pudo leer la coordenada de ese clic.');
                updateLoading('No se pudo leer la coordenada. Vuelve a intentarlo.');
                return;
              }
              const longitude = Cesium.Math.toDegrees(cartographic.longitude);
              const latitude = Cesium.Math.toDegrees(cartographic.latitude);
              const height = cartographic.height || 0.0;

              coordinatePickEnabled = false;
              draftCursorCartographic = null;
              refreshAllDraftGraphics();
              updateLoading('Coordenadas capturadas.');
              if (qtBridge && qtBridge.reportPickedCoordinate) {
                qtBridge.reportPickedCoordinate(longitude, latitude, height);
              }
              return;
            }

            const picked = viewer.scene.pick(click.position);
            const trackName = entityNameFromPick(picked);
            if (!trackName) {
              viewer.selectedEntity = undefined;
              applyHighlight(null);
              reportStatus('Seleccion borrada en mapa.');
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
