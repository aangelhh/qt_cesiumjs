#include "MainWindow.h"

#include "AddEntityDialog.h"
#include "AssignTaskDialog.h"
#include "EntityDetailsDialog.h"
#include "application/ScenarioState.h"
#include "application/SimulationEngine.h"
#include "application/Command.h"
#include "domain/BombReleaseGate.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"
#include "domain/GeoMath.h"
#include "infrastructure/CesiumScenePage.h"
#include "infrastructure/MapBridge.h"
#include "infrastructure/ModelCatalog.h"
#include "presentation/EntityTextFormatter.h"
#include "presentation/EntityHomePositionTracker.h"
#include "presentation/EntityVisualStateManager.h"
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
constexpr double kGraphicAltitudeOffsetMeters = 15.0;
constexpr double kPolygonCloseDistanceMeters = 50.0;
constexpr int kTaskQuickBarMarginPixels = 14;
constexpr int kTaskQuickBarButtonPixels = 30;
constexpr int kTaskQuickBarIconPixels = 18;
constexpr double kOrbitHoldDefaultRadiusMeters = 1500.0;
// Bomb release constants now in domain/BombReleaseGate.h
constexpr double kAutoBombReleaseCooldownSeconds = 20.0;
constexpr double kAttackAirTimeoutSeconds = 120.0;
constexpr double kAttackAirMissileCooldownSeconds = 8.0;
constexpr double kAttackAirMinimumPursuitSpeedKnots = 320.0;
constexpr double kAttackAirTargetSpeedMarginKnots = 60.0;
constexpr QLatin1StringView kTaskStatusNotStarted("NotStarted");
constexpr QLatin1StringView kTaskStatusRunning("Running");
constexpr QLatin1StringView kTaskStatusCompleted("Completed");
constexpr QLatin1StringView kTaskStatusFailed("Failed");
constexpr QLatin1StringView kTaskStatusCompletedWithFailures("CompletedWithFailures");

QString planStatusDisplayLabel(const QString& status) {
  if (status == kTaskStatusCompletedWithFailures) {
    return QStringLiteral("Completed (with failures)");
  }
  return status;
}

// attackTaskStatusIsTerminal, attackSurfaceCoordinatesAreUsable,
// autoBehaviorDamageReactionLevel, autoBehaviorCanEngageByDamage
// moved to domain/CombatRules.h

bool activeMissileInFlightForTarget(
    const ScenarioState* scenarioState,
    const QString& launcherName,
    const QString& targetName) {
  if (!scenarioState) {
    return false;
  }

  for (const ActiveMunition& munition : scenarioState->activeMunitions()) {
    if (!munition.active ||
        munition.munitionType.compare(QStringLiteral("Missile"), Qt::CaseInsensitive) != 0) {
      continue;
    }
    if (munition.launcherEntityName.compare(launcherName, Qt::CaseInsensitive) == 0 &&
        munition.targetEntityName.compare(targetName, Qt::CaseInsensitive) == 0) {
      return true;
    }
  }
  return false;
}


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
      {QStringLiteral("munitionType"), QString()},
      {QStringLiteral("effectType"), QString()},
      {QStringLiteral("headingDegrees"), 0.0},
      {QStringLiteral("pitchDegrees"), 0.0},
      {QStringLiteral("rollDegrees"), 0.0},
      {QStringLiteral("modelScale"), 1.0},
      {QStringLiteral("pointSize"), 11},
      {QStringLiteral("labelVisible"), true},
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
      {QStringLiteral("damagePercent"), 0.0},
      {QStringLiteral("damageState"), QStringLiteral("Intact")},
      {QStringLiteral("hidden"), false},
      {QStringLiteral("radarCoverageVisible"), false},
      {QStringLiteral("trackHistoryVisible"), false},
  };
}

// forceIdentifierLabel, entityCanUseMissileActions moved to domain/CombatRules.h

// weaponQuantity moved to domain/BombReleaseGate.h

struct MissileTargetCandidate {
  const Entity* entity = nullptr;
  double rangeMeters = -1.0;
};

QVector<MissileTargetCandidate> detectedMissileTargetsInRange(
    const ScenarioState* scenarioState,
    const Entity& launcher) {
  QVector<MissileTargetCandidate> targets;
  if (!scenarioState) {
    return targets;
  }

  const double maxRangeMeters = ScenarioState::missileMaxRangeMeters();
  QSet<QString> addedTargetNames;
  for (const SensorContact& contact : launcher.sensorContacts) {
    if (!contact.detected || contact.rangeMeters <= 0.0 ||
        contact.rangeMeters > maxRangeMeters) {
      continue;
    }

    const QString targetName = contact.targetEntityName.trimmed();
    const QString targetKey = targetName.toCaseFolded();
    if (targetName.isEmpty() || addedTargetNames.contains(targetKey)) {
      continue;
    }

    for (const Entity& candidate : scenarioState->entities()) {
      if (candidate.name.compare(targetName, Qt::CaseInsensitive) != 0 ||
          candidate.name == launcher.name ||
          candidate.destroyed ||
          candidate.forceIdentifier == launcher.forceIdentifier ||
          candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
        continue;
      }
      targets.push_back({&candidate, contact.rangeMeters});
      addedTargetNames.insert(targetKey);
      break;
    }
  }

  return targets;
}

QString missileTargetDisplayLabel(const Entity& entity, double rangeMeters) {
  return QStringLiteral("%1 (%2 / %3, %4 km)")
      .arg(
          entity.name,
          domain::forceIdentifierLabel(entity.forceIdentifier),
          entity.category,
          QString::number(rangeMeters / 1000.0, 'f', 1));
}

QString formatPosition(double latitude, double longitude) {
  return QStringLiteral("%1, %2")
      .arg(latitude, 0, 'f', 4)
      .arg(longitude, 0, 'f', 4);
}

QVariantMap makeMunitionTrackSummary(const ActiveMunition& munition) {
  const double altitudeMeters = qMax(0.0, munition.altitudeMeters);
  const double speedKnots = munition.speedMetersPerSecond / 0.514444;
  const QString munitionType = munition.munitionType.trimmed();
  const bool isBomb = munitionType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0;
  QVariantMap summary = makeTrackSummary(
      munition.id,
      QStringLiteral("Munition"),
      domain::forceIdentifierLabel(munition.forceIdentifier),
      QStringLiteral("%1 m").arg(altitudeMeters, 0, 'f', 0),
      formatPosition(munition.latitude, munition.longitude),
      munition.status.trimmed().isEmpty() ? QStringLiteral("Flying") : munition.status,
      munition.latitude,
      munition.longitude);
  summary.insert(QStringLiteral("category"), isBomb ? QStringLiteral("Bomb") : QStringLiteral("Missile"));
  summary.insert(QStringLiteral("munitionType"), munition.munitionType);
  summary.insert(QStringLiteral("forceIdentifier"), munition.forceIdentifier);
  summary.insert(QStringLiteral("modelName"), isBomb ? QStringLiteral("Bomb") : QStringLiteral("Missile"));
  summary.insert(QStringLiteral("modelUri"), munition.modelUri);
  summary.insert(QStringLiteral("headingDegrees"), munition.headingDegrees);
  summary.insert(QStringLiteral("pitchDegrees"), munition.pitchDegrees);
  summary.insert(QStringLiteral("rollDegrees"), munition.rollDegrees);
  summary.insert(QStringLiteral("speedKnots"), speedKnots);
  summary.insert(QStringLiteral("modelScale"), isBomb ? 0.5 : 0.35);
  summary.insert(QStringLiteral("labelVisible"), false);
  return summary;
}

QVariantMap makeTransientEffectTrackSummary(const TransientEffect& effect) {
  const double altitudeMeters = qMax(0.0, effect.altitudeMeters);
  const QString effectType = effect.effectType.trimmed().toCaseFolded();
  int pointSize = 10;
  if (effectType == QStringLiteral("impactflash")) {
    pointSize = 18;
  } else if (effectType == QStringLiteral("bombsmoketrail")) {
    pointSize = 8;
  } else if (effectType == QStringLiteral("bombimpactflash")) {
    pointSize = 16;
  } else if (effectType == QStringLiteral("bombsmoke")) {
    pointSize = 20;
  }
  QVariantMap summary = makeTrackSummary(
      effect.id,
      QStringLiteral("Effect"),
      domain::forceIdentifierLabel(effect.forceIdentifier),
      QStringLiteral("%1 m").arg(altitudeMeters, 0, 'f', 0),
      formatPosition(effect.latitude, effect.longitude),
      effect.effectType,
      effect.latitude,
      effect.longitude);
  summary.insert(QStringLiteral("category"), QStringLiteral("Effect"));
  summary.insert(QStringLiteral("effectType"), effect.effectType);
  summary.insert(QStringLiteral("forceIdentifier"), effect.forceIdentifier);
  summary.insert(QStringLiteral("labelVisible"), false);
  summary.insert(QStringLiteral("pointSize"), pointSize);
  return summary;
}

QVariantMap makePendingBombTargetTrackSummary(
    const QString& targetLabel,
    double latitude,
    double longitude,
    double targetAltitudeMeters,
    const QString& teamLabel,
    const QString& releaseStateLabel,
    double distanceMetersToTarget) {
  const QString distanceText = distanceMetersToTarget >= 0.0
      ? QStringLiteral("%1 km").arg(distanceMetersToTarget / 1000.0, 0, 'f', 1)
      : QString();
  QVariantMap summary = makeTrackSummary(
      QStringLiteral("Bomb Target"),
      QStringLiteral("PendingBombTarget"),
      teamLabel.trimmed().isEmpty() ? QStringLiteral("Friendly") : teamLabel,
      QStringLiteral("%1 m").arg(qMax(0.0, targetAltitudeMeters), 0, 'f', 0),
      formatPosition(latitude, longitude),
      distanceText.trimmed().isEmpty()
          ? (releaseStateLabel.trimmed().isEmpty() ? QStringLiteral("Armed") : releaseStateLabel)
          : QStringLiteral("%1 | %2")
                .arg(
                    releaseStateLabel.trimmed().isEmpty() ? QStringLiteral("Armed") : releaseStateLabel,
                    distanceText),
      latitude,
      longitude);
  summary.insert(QStringLiteral("category"), QStringLiteral("PendingBombTarget"));
  summary.insert(QStringLiteral("pointSize"), 16);
  summary.insert(QStringLiteral("labelVisible"), true);
  summary.insert(QStringLiteral("pendingBombReleaseState"), releaseStateLabel);
  summary.insert(QStringLiteral("pendingBombTargetLabel"), targetLabel);
  summary.insert(QStringLiteral("pendingBombTargetDistanceMeters"), distanceMetersToTarget);
  return summary;
}

QVariantMap makePendingBombTargetLineTrackSummary(
    const Entity& launcher,
    double targetLatitude,
    double targetLongitude,
    double targetAltitudeMeters,
    const QString& teamLabel,
    const QString& releaseStateLabel) {
  QVariantMap summary = makeTrackSummary(
      QStringLiteral("Bomb Target Line"),
      QStringLiteral("PendingBombTargetLine"),
      teamLabel.trimmed().isEmpty() ? QStringLiteral("Friendly") : teamLabel,
      QStringLiteral("%1 m").arg(qMax(0, launcher.altitude)),
      formatPosition(launcher.latitude, launcher.longitude),
      releaseStateLabel.trimmed().isEmpty() ? QStringLiteral("Armed") : releaseStateLabel,
      launcher.latitude,
      launcher.longitude);
  summary.insert(QStringLiteral("category"), QStringLiteral("PendingBombTargetLine"));
  summary.insert(QStringLiteral("labelVisible"), false);
  summary.insert(QStringLiteral("pendingBombReleaseState"), releaseStateLabel);
  QVariantList routePoints;
  routePoints.push_back(QVariantMap{
      {QStringLiteral("longitude"), launcher.longitude},
      {QStringLiteral("latitude"), launcher.latitude},
      {QStringLiteral("altitudeMeters"), static_cast<double>(launcher.altitude)},
  });
  routePoints.push_back(QVariantMap{
      {QStringLiteral("longitude"), targetLongitude},
      {QStringLiteral("latitude"), targetLatitude},
      {QStringLiteral("altitudeMeters"), targetAltitudeMeters},
  });
  summary.insert(QStringLiteral("routePoints"), routePoints);
  return summary;
}

// domain::BombReleaseGateEvaluation, distanceMeters, normalizeDegrees360,
// shortestSignedAngle, bearingDegrees, evaluateBombReleaseGate
// moved to domain/GeoMath.h and domain/BombReleaseGate.h

QString attackPointLabel(double latitude, double longitude) {
  return QStringLiteral("%1, %2")
      .arg(latitude, 0, 'f', 4)
      .arg(longitude, 0, 'f', 4);
}

int entityAltitudeMeters(const ScenarioState* scenarioState, const QString& entityName) {
  if (!scenarioState || entityName.trimmed().isEmpty()) {
    return 0;
  }

  for (const Entity& entity : scenarioState->entities()) {
    if (entity.name == entityName) {
      return entity.altitude;
    }
  }
  return 0;
}

QVector<const Entity*> validBombReleaseTargets(
    const ScenarioState* scenarioState,
    const Entity& launcher) {
  QVector<const Entity*> targets;
  if (!scenarioState) {
    return targets;
  }

  for (const Entity& candidate : scenarioState->entities()) {
    if (candidate.name == launcher.name ||
        candidate.destroyed ||
        candidate.forceIdentifier == launcher.forceIdentifier ||
        candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
      continue;
    }
    targets.push_back(&candidate);
  }
  return targets;
}

const Entity* bestDetectedSurfaceBombTarget(
    const ScenarioState* scenarioState,
    const Entity& launcher) {
  if (!scenarioState) {
    return nullptr;
  }

  const Entity* selectedTarget = nullptr;
  double selectedRangeMeters = -1.0;
  for (const SensorContact& contact : launcher.sensorContacts) {
    if (!contact.detected || contact.targetEntityName.trimmed().isEmpty()) {
      continue;
    }

    const QString targetName = contact.targetEntityName.trimmed();
    for (const Entity& candidate : scenarioState->entities()) {
      if (candidate.name.compare(targetName, Qt::CaseInsensitive) != 0 ||
          candidate.name == launcher.name ||
          candidate.destroyed ||
          candidate.forceIdentifier == launcher.forceIdentifier ||
          candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
        continue;
      }

      if (selectedRangeMeters < 0.0 ||
          contact.rangeMeters < selectedRangeMeters) {
        selectedTarget = &candidate;
        selectedRangeMeters = contact.rangeMeters;
      }
      break;
    }
  }

  return selectedTarget;
}

QString bombTargetDisplayLabel(const Entity& entity) {
  return QStringLiteral("%1 (%2 / %3)")
      .arg(entity.name, domain::forceIdentifierLabel(entity.forceIdentifier), entity.domain);
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
    return QColor(QStringLiteral("#ff3b30"));
  }
  if (normalized.contains(QStringLiteral("neutral"))) {
    return QColor(QStringLiteral("#35c759"));
  }
  if (normalized.contains(QStringLiteral("unknown"))) {
    return QColor(QStringLiteral("#ffd60a"));
  }
  return QColor(QStringLiteral("#55d3ff"));
}

QString categoryGlyph(const QString& category) {
  const QString normalized = category.trimmed().toLower();
  const QString compact = QString(normalized)
      .remove(QChar(' '))
      .remove(QChar('_'))
      .remove(QChar('-'));
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
  if (normalized == QStringLiteral("tank")) {
    return QStringLiteral("K");
  }
  if (normalized == QStringLiteral("truck")) {
    return QStringLiteral("R");
  }
  if (compact == QStringLiteral("armoredvehicle") ||
      compact == QStringLiteral("armouredvehicle")) {
    return QStringLiteral("V");
  }
  if (normalized == QStringLiteral("radar")) {
    return QStringLiteral("D");
  }
  if (compact == QStringLiteral("samlauncher")) {
    return QStringLiteral("A");
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
      _isPickingBombTarget(false),
      _pendingGraphicMode(),
      _pendingGraphicName(),
      _bombTargetPickLauncherName(),
      _pendingAreaType(QStringLiteral("Circle")),
      _pendingAreaRadiusMeters(1000.0),
      _pendingAreaAltitudeMeters(0.0),
      _pendingAreaSemiMajorMeters(1000.0),
      _pendingAreaSemiMinorMeters(600.0),
      _pendingAreaRotationDegrees(0.0),
      m_simulationEngine(new application::SimulationEngine(_scenarioState, this)),
      _entityVisualStateManager(std::make_unique<presentation::EntityVisualStateManager>(
          QDir(projectRootPath()).absoluteFilePath(QStringLiteral("Data/entity_visual_state.json")))),
      _entityHomePositionTracker(std::make_unique<presentation::EntityHomePositionTracker>())
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
  const auto value = [&summary](const char* key, const QString& fallback = QStringLiteral("-")) {
    const QString text = summary.value(QString::fromLatin1(key)).toString().trimmed();
    return text.isEmpty() ? fallback : text;
  };

  const QString taskType = value("taskType", QStringLiteral("No current tasks"));
  const QString taskStatus = value("taskStatus", QStringLiteral("-"));
  const QString status = value("status");
  QString operationalState =
      taskType == QStringLiteral("-") || taskType == QStringLiteral("No current tasks")
          ? status
          : QStringLiteral("%1 (%2)").arg(taskType, taskStatus);

  if (!entity) {
    return operationalState;
  }

  const QString liveTaskType = entity->currentTask.taskType.trimmed();
  const QString liveTaskStatus = entity->currentTask.status.trimmed();
  if (entity->currentTask.enabled && !liveTaskType.isEmpty()) {
    operationalState = QStringLiteral("%1 (%2)")
        .arg(liveTaskType, liveTaskStatus.isEmpty() ? QStringLiteral("-") : liveTaskStatus);
  }

  const QString entityName = entity->name;
  const int missileCount = domain::weaponQuantity(*entity, QStringLiteral("Missile"));
  const int bombCount = domain::weaponQuantity(*entity, QStringLiteral("Bomb"));
  const QString behaviorMode = entity->behaviorMode.trimmed().isEmpty()
      ? QStringLiteral("Manual")
      : entity->behaviorMode.trimmed();
  const QString behaviorTargetName = entity->behaviorTargetEntityName.trimmed();
  const Entity* behaviorTarget = this->findEntityByName(behaviorTargetName);

  bool behaviorTargetDetected = false;
  double behaviorTargetRangeMeters = -1.0;
  for (const SensorContact& contact : entity->sensorContacts) {
    if (contact.targetEntityName.compare(behaviorTargetName, Qt::CaseInsensitive) != 0) {
      continue;
    }
    if (contact.detected) {
      behaviorTargetDetected = true;
      if (behaviorTargetRangeMeters < 0.0 ||
          contact.rangeMeters < behaviorTargetRangeMeters) {
        behaviorTargetRangeMeters = contact.rangeMeters;
      }
    }
  }

  const QString behaviorTargetStatus = behaviorTargetName.isEmpty()
      ? QStringLiteral("-")
      : QStringLiteral("%1 | %2 | %3%4")
            .arg(
                behaviorTargetDetected
                    ? QStringLiteral("detected")
                    : QStringLiteral("not detected"),
                behaviorTarget
                    ? (behaviorTarget->forceIdentifier == entity->forceIdentifier
                           ? QStringLiteral("friendly")
                           : QStringLiteral("enemy"))
                    : QStringLiteral("unknown side"),
                behaviorTarget
                    ? (behaviorTarget->destroyed
                           ? QStringLiteral("destroyed")
                           : QStringLiteral("alive"))
                    : QStringLiteral("missing"),
                behaviorTargetRangeMeters >= 0.0
                    ? QStringLiteral(" | %1 km")
                          .arg(behaviorTargetRangeMeters / 1000.0, 0, 'f', 1)
                    : QString());

  struct ContactDebugLine {
    double rangeMeters = 0.0;
    QString text;
  };
  QVector<ContactDebugLine> contactLines;
  for (const SensorContact& contact : entity->sensorContacts) {
    const QString targetName = contact.targetEntityName.trimmed();
    const Entity* target = this->findEntityByName(targetName);
    const bool friendly = target && target->forceIdentifier == entity->forceIdentifier;
    const bool destroyed = target && target->destroyed;
    const bool validBehaviorTarget =
        contact.detected &&
        target &&
        !destroyed &&
        target->name != entityName &&
        !friendly;

    contactLines.push_back(ContactDebugLine{
        contact.rangeMeters,
        QStringLiteral("%1 | %2 | %3 km | brg %4 deg | %5 | %6 | %7")
            .arg(targetName.isEmpty() ? QStringLiteral("<unknown>") : targetName)
            .arg(contact.detected ? QStringLiteral("detected") : QStringLiteral("not detected"))
            .arg(contact.rangeMeters / 1000.0, 0, 'f', 1)
            .arg(contact.bearingDegrees, 0, 'f', 1)
            .arg(target
                     ? (friendly ? QStringLiteral("friendly") : QStringLiteral("enemy"))
                     : QStringLiteral("unknown side"))
            .arg(target
                     ? (destroyed ? QStringLiteral("destroyed") : QStringLiteral("alive"))
                     : QStringLiteral("unknown state"))
            .arg(validBehaviorTarget
                     ? QStringLiteral("behavior valid")
                     : QStringLiteral("behavior blocked")),
    });
  }
  std::sort(
      contactLines.begin(),
      contactLines.end(),
      [](const ContactDebugLine& left, const ContactDebugLine& right) {
        return left.rangeMeters < right.rangeMeters;
      });

  QStringList contactTextLines;
  for (int index = 0; index < contactLines.size() && index < 10; ++index) {
    contactTextLines.push_back(QStringLiteral("  %1").arg(contactLines.at(index).text));
  }
  const QString contactsText = contactTextLines.isEmpty()
      ? QStringLiteral("  No contacts detected")
      : contactTextLines.join(QStringLiteral("\n"));

  QString bombReleaseState = QStringLiteral("None");
  QString bombTargetText = QStringLiteral("-");
  QString bombDistanceText = QStringLiteral("-");
  if (this->_pendingBombRelease.pending &&
      this->_pendingBombRelease.launcherEntityName.compare(entityName, Qt::CaseInsensitive) == 0) {
    const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
        *entity,
        this->_pendingBombRelease.targetLatitude,
        this->_pendingBombRelease.targetLongitude,
        this->_pendingBombRelease.targetAltitudeMeters);
    bombReleaseState = evaluation.stateLabel();
    bombTargetText = this->_pendingBombRelease.targetLabel.trimmed().isEmpty()
        ? attackPointLabel(
              this->_pendingBombRelease.targetLatitude,
              this->_pendingBombRelease.targetLongitude)
        : this->_pendingBombRelease.targetLabel.trimmed();
    bombDistanceText = QStringLiteral("%1 km")
        .arg(
            domain::distanceMeters(
                entity->latitude,
                entity->longitude,
                this->_pendingBombRelease.targetLatitude,
                this->_pendingBombRelease.targetLongitude) / 1000.0,
            0,
            'f',
            1);
  } else {
    for (const ActiveMunition& munition : this->_scenarioState->activeMunitions()) {
      if (munition.launcherEntityName.compare(entityName, Qt::CaseInsensitive) == 0 &&
          munition.munitionType.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0) {
        bombReleaseState = QStringLiteral("Released");
        bombTargetText = munition.id;
        break;
      }
    }
  }

  const bool canUseWeapons = domain::entityCanUseMissileActions(*entity);

  bool hasAirTarget = false;
  bool hasDetectedAirTarget = false;
  bool hasMissileTargetInRange = false;
  const double missileMaxRangeMeters = ScenarioState::missileMaxRangeMeters();
  for (const Entity& candidate : this->_scenarioState->entities()) {
    if (candidate.name == entityName ||
        candidate.destroyed ||
        candidate.forceIdentifier == entity->forceIdentifier ||
        candidate.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
      continue;
    }
    hasAirTarget = true;
    for (const SensorContact& contact : entity->sensorContacts) {
      if (!contact.detected ||
          contact.targetEntityName.compare(candidate.name, Qt::CaseInsensitive) != 0) {
        continue;
      }
      hasDetectedAirTarget = true;
      if (contact.rangeMeters > 0.0 && contact.rangeMeters <= missileMaxRangeMeters) {
        hasMissileTargetInRange = true;
      }
    }
  }

  const auto availabilityText = [](bool available, const QString& reason) {
    return available
        ? QStringLiteral("✔ Available")
        : QStringLiteral("✖ Blocked (%1)").arg(reason);
  };
  const auto fieldLine = [](const QString& label, const QString& text) {
    return QStringLiteral("  %1 %2")
        .arg(label + QStringLiteral(":"), -24, QLatin1Char(' '))
        .arg(text);
  };

  QString launchMissileReason;
  if (!canUseWeapons || entity->destroyed) {
    launchMissileReason = QStringLiteral("platform not eligible");
  } else if (missileCount <= 0) {
    launchMissileReason = QStringLiteral("no missiles");
  } else if (!this->_simulationRunning) {
    launchMissileReason = QStringLiteral("simulation stopped");
  } else if (!hasAirTarget) {
    launchMissileReason = QStringLiteral("no target");
  } else if (!hasDetectedAirTarget) {
    launchMissileReason = QStringLiteral("target not detected");
  } else if (!hasMissileTargetInRange) {
    launchMissileReason = QStringLiteral("out of range");
  }
  const bool launchMissileAvailable = launchMissileReason.isEmpty();

  QString releaseBombReason;
  if (!canUseWeapons || entity->destroyed) {
    releaseBombReason = QStringLiteral("platform not eligible");
  } else if (bombCount <= 0) {
    releaseBombReason = QStringLiteral("no bombs");
  } else if (!this->_simulationRunning) {
    releaseBombReason = QStringLiteral("simulation stopped");
  }
  const bool releaseBombAvailable = releaseBombReason.isEmpty();

  const bool cancelBombAvailable =
      this->_pendingBombRelease.pending &&
      this->_pendingBombRelease.launcherEntityName.compare(entityName, Qt::CaseInsensitive) == 0;
  const auto activePlanIt = this->_entityPlans.constFind(entityName);
  const EntityPlan* activePlan = (activePlanIt != this->_entityPlans.constEnd())
      ? &activePlanIt.value()
      : nullptr;
  const QString planStatus = (!activePlan || activePlan->status.trimmed().isEmpty())
      ? QString(kTaskStatusNotStarted)
      : planStatusDisplayLabel(activePlan->status.trimmed());
  QString currentPlanStep = QStringLiteral("-");
  if (activePlan &&
      activePlan->running &&
      activePlan->currentStepIndex >= 0 &&
      activePlan->currentStepIndex < activePlan->steps.size()) {
    const PlanStep& step = activePlan->steps.at(activePlan->currentStepIndex);
    const QString stepStatus = step.status.trimmed().isEmpty()
        ? QString(kTaskStatusNotStarted)
        : step.status.trimmed();
    currentPlanStep = QStringLiteral("%1 [%2]")
        .arg(this->planStepDisplayLabel(step), stepStatus);
  }

  QStringList lines;
  lines << QStringLiteral("[ENTITY]")
        << fieldLine(QStringLiteral("State"), operationalState)
        << fieldLine(
               QStringLiteral("Damage"),
               QStringLiteral("%1 (%2%)")
                   .arg(
                       value("damageState", QStringLiteral("Intact")),
                       QString::number(
                           summary.value(QStringLiteral("damagePercent"), 0.0).toDouble(),
                           'f',
                           0)))
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[WEAPONS]")
        << fieldLine(QStringLiteral("Missiles"), QString::number(missileCount))
        << fieldLine(QStringLiteral("Bombs"), QString::number(bombCount))
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[BEHAVIOR]")
        << fieldLine(QStringLiteral("Mode"), behaviorMode)
        << fieldLine(
               QStringLiteral("Target"),
               behaviorTargetName.isEmpty() ? QStringLiteral("-") : behaviorTargetName)
        << fieldLine(QStringLiteral("Target status"), behaviorTargetStatus)
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[SENSORS]")
        << contactsText
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[BOMBING]")
        << fieldLine(QStringLiteral("Pending Release"), bombReleaseState)
        << fieldLine(QStringLiteral("Bomb Target"), bombTargetText)
        << fieldLine(QStringLiteral("Distance"), bombDistanceText)
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[PLAN]")
        << fieldLine(QStringLiteral("Status"), planStatus)
        << fieldLine(QStringLiteral("Current Step"), currentPlanStep)
        << QStringLiteral("")
        << QStringLiteral("")
        << QStringLiteral("[ACTIONS]")
        << fieldLine(
               QStringLiteral("Launch Missile At"),
               availabilityText(launchMissileAvailable, launchMissileReason))
        << fieldLine(
               QStringLiteral("Release Bomb At"),
               availabilityText(releaseBombAvailable, releaseBombReason))
        << fieldLine(
               QStringLiteral("Cancel Bomb Release"),
               availabilityText(
                   cancelBombAvailable,
                   QStringLiteral("no pending release")));

  return lines.join(QStringLiteral("\n"));
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
      makeTrackSummary(
          category,
          QStringLiteral("Category"),
          domain::forceIdentifierLabel(entity.forceIdentifier),
          QStringLiteral("-"),
          QStringLiteral("Multiple tracks"),
          QStringLiteral("Category"),
          0.0,
          0.0));

  auto* item = new QStandardItem(entity.name);
  item->setIcon(makeTrackIcon(domain::forceIdentifierLabel(entity.forceIdentifier), category, false));
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

  QVariantMap summary = makeTrackSummary(
      entity.name,
      entity.type,
      domain::forceIdentifierLabel(entity.forceIdentifier),
      QStringLiteral("%1 m").arg(entity.altitude),
      formatPosition(entity.latitude, entity.longitude),
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
  return this->_entityPlans[entityName];
}

void MainWindow::pruneEntityPlans() {
  QSet<QString> validEntityNames;
  for (const Entity& entity : this->_scenarioState->entities()) {
    validEntityNames.insert(entity.name);
  }

  for (auto it = this->_entityPlans.begin(); it != this->_entityPlans.end();) {
    if (!validEntityNames.contains(it.key())) {
      it = this->_entityPlans.erase(it);
      continue;
    }
    ++it;
  }
}

QString MainWindow::planStepDisplayLabel(const PlanStep& step) const {
  if (!step.label.trimmed().isEmpty()) {
    return step.label;
  }

  switch (step.kind) {
    case PlanStepKind::MoveToLocation:
      return QStringLiteral("Move To Location");
    case PlanStepKind::MoveToWaypoint:
      return QStringLiteral("Move To Waypoint");
    case PlanStepKind::MoveAlongRoute:
      return QStringLiteral("Move Along Route");
    case PlanStepKind::PatrolArea:
      return QStringLiteral("Patrol Area");
    case PlanStepKind::FlyHeadingAltitudeSpeed:
      return QStringLiteral("Fly Heading / Altitude / Speed");
    case PlanStepKind::OrbitHoldLocation:
      return QStringLiteral("Orbit / Hold (Location)");
    case PlanStepKind::ReturnToBase:
      return QStringLiteral("Return To Base");
    case PlanStepKind::AttackAir:
      return QStringLiteral("Attack Air");
    case PlanStepKind::AttackSurface:
      return QStringLiteral("Attack Surface");
  }

  return QStringLiteral("Plan Step");
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
      const double closeDistanceMeters = domain::distanceMeters(
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

  if (this->_isPickingBombTarget) {
    const QString launcherName = this->_bombTargetPickLauncherName.trimmed();
    this->_isPickingBombTarget = false;
    this->_bombTargetPickLauncherName.clear();

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
        attackPointLabel(latitude, longitude),
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
  this->_isPickingBombTarget = false;
  this->_bombTargetPickLauncherName.clear();
  this->_autoBombReleaseCooldownSeconds.clear();
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
    this->sendTrackToMap(makeMunitionTrackSummary(munition), false);
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
    this->sendTrackToMap(makeTransientEffectTrackSummary(effect), false);
  }

  for (const QString& previousName : this->_activeEffectTrackNames) {
    if (!currentEffectTrackNames.contains(previousName)) {
      this->removeTrackFromMap(previousName);
    }
  }
  this->_activeEffectTrackNames = currentEffectTrackNames;

  const QString pendingBombTargetTrackName = QStringLiteral("Bomb Target");
  const QString pendingBombTargetLineTrackName = QStringLiteral("Bomb Target Line");
  if (this->_pendingBombRelease.pending) {
    QString teamLabel = QStringLiteral("Friendly");
    QString releaseStateLabel = QStringLiteral("Armed");
    double distanceToBombTargetMeters = -1.0;
    if (const Entity* launcher =
            this->findEntityByName(this->_pendingBombRelease.launcherEntityName)) {
      teamLabel = domain::forceIdentifierLabel(launcher->forceIdentifier);
      distanceToBombTargetMeters = domain::distanceMeters(
          launcher->latitude,
          launcher->longitude,
          this->_pendingBombRelease.targetLatitude,
          this->_pendingBombRelease.targetLongitude);
      const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
          *launcher,
          this->_pendingBombRelease.targetLatitude,
          this->_pendingBombRelease.targetLongitude,
          this->_pendingBombRelease.targetAltitudeMeters);
      releaseStateLabel = evaluation.stateLabel();
      this->sendTrackToMap(
          makePendingBombTargetLineTrackSummary(
              *launcher,
              this->_pendingBombRelease.targetLatitude,
              this->_pendingBombRelease.targetLongitude,
              this->_pendingBombRelease.targetAltitudeMeters,
              teamLabel,
              releaseStateLabel),
          false);
    } else {
      this->removeTrackFromMap(pendingBombTargetLineTrackName);
    }
    this->sendTrackToMap(
        makePendingBombTargetTrackSummary(
            this->_pendingBombRelease.targetLabel,
            this->_pendingBombRelease.targetLatitude,
            this->_pendingBombRelease.targetLongitude,
            this->_pendingBombRelease.targetAltitudeMeters,
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
  const QString entityName = this->selectedEntityName();
  const Entity* entity = this->findEntityByName(entityName);
  const bool canUseWeapons = entity && domain::entityCanUseMissileActions(*entity);
  const int missileCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Missile")) : 0;
  const int bombCount =
      entity ? domain::weaponQuantity(*entity, QStringLiteral("Bomb")) : 0;
  const int detectedMissileTargetCount =
      entity ? detectedMissileTargetsInRange(this->_scenarioState, *entity).size() : 0;
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
      this->_pendingBombRelease.pending &&
      entity &&
      this->_pendingBombRelease.launcherEntityName.compare(entity->name, Qt::CaseInsensitive) == 0);
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
  if (entityName.isEmpty()) {
    return false;
  }

  if (!this->_scenarioState->assignTask(entityName, task)) {
    return false;
  }

  this->appendLogMessage(
      QStringLiteral("Task %1 assigned to %2").arg(task.taskType, entityName));

  Entity resolvedEntity;
  bool foundEntity = false;
  for (const Entity& entity : this->_scenarioState->entities()) {
    if (entity.name != entityName) {
      continue;
    }
    resolvedEntity = entity;
    foundEntity = true;
    break;
  }
  if (!foundEntity) {
    return false;
  }

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
          targetSpeed));
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
          task.targetSpeedKnots));
    } else if (task.taskType == "FollowEntity") {
      stack->push(std::make_unique<domain::FollowEntityTask>(
          static_cast<double>(task.targetAltitudeMeters),
          task.targetSpeedKnots));
    } else if (task.taskType == "PatrolArea") {
      for (const AreaDefinition& area : this->_scenarioState->areas()) {
        if (area.name != resolvedEntity.currentTask.targetAreaName &&
            area.id != resolvedEntity.currentTask.targetAreaName) {
          continue;
        }
        stack->push(std::make_unique<domain::PatrolAreaTask>(
            domain::buildPatrolRouteFromArea(area),
            static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
            resolvedEntity.currentTask.targetSpeedKnots));
        break;
      }
      if (stack->isEmpty()) {
        stack->push(std::make_unique<domain::OrbitAreaTask>(
            resolvedEntity.currentTask.targetLatitude,
            resolvedEntity.currentTask.targetLongitude,
            resolvedEntity.currentTask.targetAreaRadiusMeters,
            static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
            resolvedEntity.currentTask.targetSpeedKnots,
            true));
      }
    } else if (task.taskType == "OrbitArea") {
      stack->push(std::make_unique<domain::OrbitAreaTask>(
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAreaRadiusMeters,
          static_cast<double>(resolvedEntity.currentTask.targetAltitudeMeters),
          resolvedEntity.currentTask.targetSpeedKnots,
          false));
    }
  }

  if (m_simulationEngine) {
    if (task.taskType == "MoveToLocation" || task.taskType == "MoveToWaypoint") {
      m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignMoveTask>(
          0,
          entityName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots));
    } else if (task.taskType == "FlyHeadingAltitudeSpeed") {
      m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignFlyHeadingTask>(
          entityName,
          task.targetHeadingDegrees,
          task.targetAltitudeMeters,
          task.targetSpeedKnots));
    } else if (task.taskType == "FollowEntity") {
      m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignFollowTask>(
          entityName,
          task.targetEntityName,
          task.targetAltitudeMeters,
          task.targetSpeedKnots));
    } else if (task.taskType == "PatrolArea" || task.taskType == "OrbitArea") {
      m_simulationEngine->enqueueCommand(std::make_unique<application::CmdAssignOrbitTask>(
          entityName,
          resolvedEntity.currentTask.targetAreaName,
          resolvedEntity.currentTask.targetLatitude,
          resolvedEntity.currentTask.targetLongitude,
          resolvedEntity.currentTask.targetAreaRadiusMeters,
          resolvedEntity.currentTask.targetAltitudeMeters,
          resolvedEntity.currentTask.targetSpeedKnots,
          (task.taskType == "PatrolArea")));
    }
  }

  if (syncUi) {
    this->syncScenarioStateToUi();
  }
  return true;
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
                .arg(attackPointLabel(step.task.targetLatitude, step.task.targetLongitude))
          : QStringLiteral("Attack Surface: %1").arg(targetName);
      return true;
    }
  }

  return false;
}

bool MainWindow::validatePlanStepForExecution(const PlanStep& step, QString* reason) const {
  auto setReason = [reason](const QString& text) {
    if (reason) {
      *reason = text;
    }
    return false;
  };

  if (step.task.taskType.trimmed().isEmpty()) {
    return setReason(QStringLiteral("step task type is empty"));
  }

  switch (step.kind) {
    case PlanStepKind::MoveToWaypoint:
      if (step.task.targetWaypointName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("waypoint is not set"));
      }
      if (!this->findWaypointByName(step.task.targetWaypointName)) {
        return setReason(
            QStringLiteral("waypoint '%1' no longer exists").arg(step.task.targetWaypointName));
      }
      return true;

    case PlanStepKind::MoveAlongRoute: {
      if (step.task.targetRouteName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("route is not set"));
      }
      const RouteGraphic* route = this->findRouteByName(step.task.targetRouteName);
      if (!route) {
        return setReason(
            QStringLiteral("route '%1' no longer exists").arg(step.task.targetRouteName));
      }
      if (route->points.isEmpty()) {
        return setReason(
            QStringLiteral("route '%1' has no points").arg(step.task.targetRouteName));
      }
      return true;
    }

    case PlanStepKind::PatrolArea:
      if (step.task.targetAreaName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("area is not set"));
      }
      if (!this->findAreaByNameOrId(step.task.targetAreaName)) {
        return setReason(
            QStringLiteral("area '%1' no longer exists").arg(step.task.targetAreaName));
      }
      return true;

    case PlanStepKind::MoveToLocation:
    case PlanStepKind::FlyHeadingAltitudeSpeed:
    case PlanStepKind::OrbitHoldLocation:
    case PlanStepKind::ReturnToBase:
      return true;

    case PlanStepKind::AttackAir:
      if (step.task.targetEntityName.trimmed().isEmpty()) {
        return setReason(QStringLiteral("air target is not set"));
      }
      if (!this->findEntityByName(step.task.targetEntityName.trimmed())) {
        return setReason(
            QStringLiteral("air target '%1' no longer exists")
                .arg(step.task.targetEntityName.trimmed()));
      }
      return true;

    case PlanStepKind::AttackSurface:
      if (!step.task.targetEntityName.trimmed().isEmpty()) {
        if (!this->findEntityByName(step.task.targetEntityName.trimmed())) {
          return setReason(
              QStringLiteral("surface target '%1' no longer exists")
                  .arg(step.task.targetEntityName.trimmed()));
        }
        return true;
      }
      if (!domain::attackSurfaceCoordinatesAreUsable(
              step.task.targetLatitude,
              step.task.targetLongitude)) {
        return setReason(QStringLiteral("surface target coordinates are not set"));
      }
      return true;
  }

  return true;
}

bool MainWindow::activeTaskMatchesPlanStep(const Entity& entity, const PlanStep& step) const {
  const EntityTask& currentTask = entity.currentTask;
  if (currentTask.taskType != step.task.taskType) {
    return false;
  }

  auto nearlyEqual = [](double left, double right, double epsilon) {
    return qAbs(left - right) <= epsilon;
  };

  switch (step.kind) {
    case PlanStepKind::MoveToLocation:
    case PlanStepKind::ReturnToBase:
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::MoveToWaypoint:
      return currentTask.targetWaypointName == step.task.targetWaypointName &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::MoveAlongRoute:
      return currentTask.targetRouteName == step.task.targetRouteName &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::PatrolArea:
      return currentTask.targetAreaName == step.task.targetAreaName &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::FlyHeadingAltitudeSpeed:
      return nearlyEqual(currentTask.targetHeadingDegrees, step.task.targetHeadingDegrees, 0.1) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::OrbitHoldLocation:
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters &&
             nearlyEqual(currentTask.targetAreaRadiusMeters, step.task.targetAreaRadiusMeters, 1.0) &&
             nearlyEqual(currentTask.targetSpeedKnots, step.task.targetSpeedKnots, 0.1);

    case PlanStepKind::AttackAir:
      return currentTask.targetEntityName == step.task.targetEntityName;

    case PlanStepKind::AttackSurface:
      if (!step.task.targetEntityName.trimmed().isEmpty()) {
        return currentTask.targetEntityName == step.task.targetEntityName;
      }
      return nearlyEqual(currentTask.targetLatitude, step.task.targetLatitude, 1e-6) &&
             nearlyEqual(currentTask.targetLongitude, step.task.targetLongitude, 1e-6) &&
             currentTask.targetAltitudeMeters == step.task.targetAltitudeMeters;
  }

  return false;
}

bool MainWindow::startEntityPlan(const QString& entityName) {
  const Entity* entity = this->findEntityByName(entityName);
  if (!entity || entity->destroyed) {
    return false;
  }

  EntityPlan& plan = this->ensureEntityPlan(entityName);
  if (plan.steps.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("El plan esta vacio para %1.").arg(entityName));
    return false;
  }

  for (int index = 0; index < plan.steps.size(); ++index) {
    QString invalidReason;
    if (!this->validatePlanStepForExecution(plan.steps.at(index), &invalidReason)) {
      plan.running = false;
      plan.currentStepIndex = -1;
      plan.currentStableTicks = 0;
      const QString stepLabel = this->planStepDisplayLabel(plan.steps.at(index));
      this->appendLogMessage(
          QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
              .arg(entityName, stepLabel, invalidReason));
      this->_ui->statusLabel->setText(
          QStringLiteral("Plan detenido para %1: step invalido (%2).")
              .arg(entityName, stepLabel));
      return false;
    }
  }

  plan.running = true;
  plan.status = QString(kTaskStatusRunning);
  plan.currentStepIndex = 0;
  plan.currentStableTicks = 0;
  for (PlanStep& step : plan.steps) {
    step.status = QString(kTaskStatusNotStarted);
  }
  if (!this->startPlanStepTask(entityName, plan)) {
    return false;
  }

  this->appendLogMessage(
      QStringLiteral("Plan started for %1").arg(entityName));
  this->_ui->statusLabel->setText(
      QStringLiteral("Plan en ejecucion para %1.").arg(entityName));
  return true;
}

void MainWindow::stopEntityPlan(const QString& entityName, bool clearCurrentTask) {
  auto it = this->_entityPlans.find(entityName);
  if (it == this->_entityPlans.end()) {
    return;
  }

  it->running = false;
  it->currentStepIndex = -1;
  it->currentStableTicks = 0;
  if (it->status == QString(kTaskStatusRunning)) {
    it->status = QString(kTaskStatusNotStarted);
  }

  if (clearCurrentTask) {
    this->_scenarioState->clearTask(entityName);
    this->syncScenarioStateToUi();
  }
}

bool MainWindow::startPlanStepTask(const QString& entityName, EntityPlan& plan) {
  if (plan.currentStepIndex < 0 || plan.currentStepIndex >= plan.steps.size()) {
    return false;
  }
  PlanStep& step = plan.steps[plan.currentStepIndex];
  step.status = QString(kTaskStatusRunning);
  if (!this->applyEntityTask(entityName, step.task, false)) {
    step.status = QString(kTaskStatusFailed);
    plan.running = false;
    plan.status = QString(kTaskStatusFailed);
    plan.currentStepIndex = -1;
    plan.currentStableTicks = 0;
    return false;
  }
  return true;
}

void MainWindow::failRunningPlan(
    const QString& entityName,
    EntityPlan& plan,
    const QString& logMessage,
    const QString& statusMessage) {
  if (!logMessage.isEmpty()) {
    this->appendLogMessage(logMessage);
  }
  if (!statusMessage.isEmpty()) {
    this->_ui->statusLabel->setText(statusMessage);
  }
  if (plan.currentStepIndex >= 0 && plan.currentStepIndex < plan.steps.size()) {
    plan.steps[plan.currentStepIndex].status = QString(kTaskStatusFailed);
  }
  plan.running = false;
  plan.status = QString(kTaskStatusFailed);
  plan.currentStepIndex = -1;
  plan.currentStableTicks = 0;
}

void MainWindow::completeRunningPlan(
    const QString& entityName,
    EntityPlan& plan,
    const QString& completedLabel) {
  plan.running = false;
  bool hasFailedSteps = false;
  for (const PlanStep& step : plan.steps) {
    if (step.status == kTaskStatusFailed) {
      hasFailedSteps = true;
      break;
    }
  }
  plan.status = (hasFailedSteps ? QString(kTaskStatusCompletedWithFailures) : QString(kTaskStatusCompleted));
  plan.currentStepIndex = -1;
  if (hasFailedSteps) {
    this->appendLogMessage(
        QStringLiteral("Plan completed with failures for %1 after %2.")
            .arg(entityName, completedLabel));
    this->_ui->statusLabel->setText(
        QStringLiteral("Plan completado con fallas para %1.").arg(entityName));
  } else {
    this->appendLogMessage(
        QStringLiteral("Plan completed for %1 after %2.")
            .arg(entityName, completedLabel));
    this->_ui->statusLabel->setText(
        QStringLiteral("Plan completado para %1.").arg(entityName));
  }
}

bool MainWindow::activePlanStepCompleted(const Entity& entity, EntityPlan& plan) const {
  if (plan.currentStepIndex < 0 || plan.currentStepIndex >= plan.steps.size()) {
    return false;
  }

  const PlanStep& step = plan.steps.at(plan.currentStepIndex);
  switch (step.kind) {
    case PlanStepKind::MoveToLocation:
    case PlanStepKind::MoveToWaypoint:
    case PlanStepKind::MoveAlongRoute:
    case PlanStepKind::ReturnToBase:
      plan.currentStableTicks = 0;
      return entity.currentTask.status == QStringLiteral("On target");

    case PlanStepKind::PatrolArea: {
      const double distanceToCenterMeters = domain::distanceMeters(
          entity.latitude,
          entity.longitude,
          step.task.targetLatitude,
          step.task.targetLongitude);
      const double holdDistanceMeters = qMax(
          100.0,
          step.task.targetAreaRadiusMeters * 1.15);
      if (distanceToCenterMeters <= holdDistanceMeters) {
        ++plan.currentStableTicks;
      } else {
        plan.currentStableTicks = 0;
      }
      return plan.currentStableTicks >= 3;
    }

    case PlanStepKind::FlyHeadingAltitudeSpeed: {
      const double headingErrorDegrees = qAbs(domain::shortestSignedAngle(
          entity.headingDegrees,
          step.task.targetHeadingDegrees));
      const int altitudeErrorMeters = qAbs(entity.altitude - step.task.targetAltitudeMeters);
      const double speedErrorKnots = qAbs(entity.speedKnots - step.task.targetSpeedKnots);
      if (headingErrorDegrees <= 5.0 &&
          altitudeErrorMeters <= 50 &&
          speedErrorKnots <= 10.0) {
        ++plan.currentStableTicks;
      } else {
        plan.currentStableTicks = 0;
      }
      return plan.currentStableTicks >= 3;
    }

    case PlanStepKind::OrbitHoldLocation: {
      const double distanceToCenterMeters = domain::distanceMeters(
          entity.latitude,
          entity.longitude,
          step.task.targetLatitude,
          step.task.targetLongitude);
      const double holdDistanceMeters = qMax(
          100.0,
          step.task.targetAreaRadiusMeters * 1.15);
      if (distanceToCenterMeters <= holdDistanceMeters) {
        ++plan.currentStableTicks;
      } else {
        plan.currentStableTicks = 0;
      }
      return plan.currentStableTicks >= 3;
    }

    case PlanStepKind::AttackAir:
    case PlanStepKind::AttackSurface:
      plan.currentStableTicks = 0;
      return entity.currentTask.status == QStringLiteral("Completed");
  }

  return false;
}

void MainWindow::advanceEntityPlans() {
  for (auto it = this->_entityPlans.begin(); it != this->_entityPlans.end(); ++it) {
    const QString entityName = it.key();
    EntityPlan& plan = it.value();
    if (!plan.running) {
      continue;
    }

    const Entity* entity = this->findEntityByName(entityName);
    if (!entity || entity->destroyed) {
      this->failRunningPlan(entityName, plan);
      continue;
    }

    if (plan.currentStepIndex < 0 || plan.currentStepIndex >= plan.steps.size()) {
      this->failRunningPlan(entityName, plan);
      continue;
    }

    PlanStep& activeStep = plan.steps[plan.currentStepIndex];
    activeStep.status = QString(kTaskStatusRunning);
    QString invalidReason;
    if (!this->validatePlanStepForExecution(activeStep, &invalidReason)) {
      this->failRunningPlan(
          entityName,
          plan,
          QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
              .arg(entityName, this->planStepDisplayLabel(activeStep), invalidReason),
          QStringLiteral("Plan detenido para %1: step invalido.").arg(entityName));
      continue;
    }

    const bool taskFailedForStep =
        entity->currentTask.status == QStringLiteral("Target unavailable") ||
        entity->currentTask.status == QStringLiteral("Failed");
    if (taskFailedForStep) {
      const QString failedLabel = this->planStepDisplayLabel(activeStep);
      activeStep.status = QString(kTaskStatusFailed);
      ++plan.currentStepIndex;
      plan.currentStableTicks = 0;

      if (plan.currentStepIndex >= plan.steps.size()) {
        this->completeRunningPlan(entityName, plan, failedLabel);
        continue;
      }

      PlanStep& nextStepAfterFailure = plan.steps[plan.currentStepIndex];
      QString nextInvalidReasonAfterFailure;
      if (!this->validatePlanStepForExecution(nextStepAfterFailure, &nextInvalidReasonAfterFailure)) {
        this->failRunningPlan(
            entityName,
            plan,
            QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
                .arg(entityName, this->planStepDisplayLabel(nextStepAfterFailure), nextInvalidReasonAfterFailure),
            QStringLiteral("Plan detenido para %1: step invalido.").arg(entityName));
        continue;
      }

      const QString nextLabelAfterFailure = this->planStepDisplayLabel(nextStepAfterFailure);
      if (!this->startPlanStepTask(entityName, plan)) {
        this->failRunningPlan(
            entityName,
            plan,
            QStringLiteral("Plan halted for %1 while starting step %2.")
                .arg(entityName, nextLabelAfterFailure),
            QStringLiteral("Plan fallido para %1: no se pudo arrancar step %2.")
                .arg(entityName, nextLabelAfterFailure));
        continue;
      }

      this->appendLogMessage(
          QStringLiteral("Plan continued for %1 after failed step %2; next step: %3")
              .arg(entityName, failedLabel, nextLabelAfterFailure));
      continue;
    }

    if (this->activePlanStepCompleted(*entity, plan)) {
      const QString completedLabel = this->planStepDisplayLabel(activeStep);
      activeStep.status = QString(kTaskStatusCompleted);
      ++plan.currentStepIndex;
      plan.currentStableTicks = 0;

      if (plan.currentStepIndex >= plan.steps.size()) {
        this->completeRunningPlan(entityName, plan, completedLabel);
        continue;
      }

      PlanStep& nextStep = plan.steps[plan.currentStepIndex];
      QString nextInvalidReason;
      if (!this->validatePlanStepForExecution(nextStep, &nextInvalidReason)) {
        this->failRunningPlan(
            entityName,
            plan,
            QStringLiteral("Plan halted for %1 because step %2 is no longer valid: %3.")
                .arg(entityName, this->planStepDisplayLabel(nextStep), nextInvalidReason),
            QStringLiteral("Plan detenido para %1: step invalido.").arg(entityName));
        continue;
      }

      const QString nextLabel = this->planStepDisplayLabel(nextStep);
      if (!this->startPlanStepTask(entityName, plan)) {
        this->failRunningPlan(
            entityName,
            plan,
            QStringLiteral("Plan halted for %1 while starting step %2.")
                .arg(entityName, nextLabel),
            QStringLiteral("Plan fallido para %1: no se pudo arrancar step %2.")
                .arg(entityName, nextLabel));
        continue;
      }

      this->appendLogMessage(
          QStringLiteral("Plan advanced for %1: %2").arg(entityName, nextLabel));
      continue;
    }

    if (!entity->currentTask.enabled ||
        !this->activeTaskMatchesPlanStep(*entity, activeStep)) {
      this->failRunningPlan(
          entityName,
          plan,
          QStringLiteral("Plan stopped for %1 after task override.").arg(entityName),
          QStringLiteral("Plan detenido para %1: task modificada manualmente.").arg(entityName));
      continue;
    }

    continue;
  }
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

  const int currentEntityAltitudeMeters = entityAltitudeMeters(
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

  const QVector<MissileTargetCandidate> targets =
      detectedMissileTargetsInRange(this->_scenarioState, *launcher);
  if (targets.isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("No detected air targets in missile range for %1.")
            .arg(launcherName));
    return;
  }

  QStringList options;
  QHash<QString, QString> targetNameByOption;
  for (const MissileTargetCandidate& candidate : targets) {
    if (!candidate.entity) {
      continue;
    }
    const QString option =
        missileTargetDisplayLabel(*candidate.entity, candidate.rangeMeters);
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
            : planStatusDisplayLabel(plan.status.trimmed())));
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
              .arg(this->planStepDisplayLabel(plan.steps.at(index))));
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
  this->_pendingBombRelease.launcherEntityName = launcherEntityName.trimmed();
  this->_pendingBombRelease.targetEntityName = targetEntityName.trimmed();
  this->_pendingBombRelease.targetLatitude = targetLatitude;
  this->_pendingBombRelease.targetLongitude = targetLongitude;
  this->_pendingBombRelease.targetAltitudeMeters = targetAltitudeMeters;
  this->_pendingBombRelease.targetLabel = targetLabel.trimmed();
  this->_pendingBombRelease.sourceDescription = sourceDescription.trimmed();
  this->_pendingBombRelease.pending = true;
  this->_pendingBombRelease.releaseCommandIssued = false;

  if (logQueued) {
    this->appendLogMessage(
        QStringLiteral("Bomb release queued for %1 on %2 (%3)")
            .arg(
                this->_pendingBombRelease.launcherEntityName,
                this->_pendingBombRelease.targetLabel,
                this->_pendingBombRelease.sourceDescription));
  }
  this->_ui->statusLabel->setText(
      QStringLiteral("Bomb release armed for %1 on %2.")
          .arg(
              this->_pendingBombRelease.launcherEntityName,
              this->_pendingBombRelease.targetLabel));
  if (focusLauncher) {
    this->selectObjectByName(this->_pendingBombRelease.launcherEntityName, true);
  }
}

void MainWindow::clearPendingBombRelease() {
  this->_pendingBombRelease = PendingBombRelease{};
}

QString MainWindow::cleanupRuntimeReferencesForRemovedEntity(const QString& entityName) {
  const QString removedEntityName = entityName.trimmed();
  if (removedEntityName.isEmpty()) {
    return QString();
  }

  this->_autoBombReleaseCooldownSeconds.remove(removedEntityName);
  this->_attackAirElapsedSeconds.remove(removedEntityName);
  this->_attackAirMissileCooldownSeconds.remove(removedEntityName);

  if (this->_bombTargetPickLauncherName.compare(
          removedEntityName,
          Qt::CaseInsensitive) == 0) {
    this->_isPickingBombTarget = false;
    this->_bombTargetPickLauncherName.clear();
  }

  if (!this->_pendingBombRelease.pending) {
    return QString();
  }

  const bool launcherRemoved =
      this->_pendingBombRelease.launcherEntityName.compare(
          removedEntityName,
          Qt::CaseInsensitive) == 0;
  const bool targetRemoved =
      !this->_pendingBombRelease.targetEntityName.trimmed().isEmpty() &&
      this->_pendingBombRelease.targetEntityName.compare(
          removedEntityName,
          Qt::CaseInsensitive) == 0;
  if (!launcherRemoved && !targetRemoved) {
    return QString();
  }

  const QString reason = targetRemoved
      ? QStringLiteral("target removed")
      : QStringLiteral("launcher removed");
  this->clearPendingBombRelease();
  this->removeTrackFromMap(QStringLiteral("Bomb Target"));
  this->removeTrackFromMap(QStringLiteral("Bomb Target Line"));
  const QString message =
      QStringLiteral("Pending bomb release cancelled: %1").arg(reason);
  this->appendLogMessage(message);
  return message;
}

void MainWindow::validatePendingBombRelease() {
  if (!this->_pendingBombRelease.pending) {
    return;
  }

  const Entity* launcher =
      this->findEntityByName(this->_pendingBombRelease.launcherEntityName);
  if (!launcher || launcher->destroyed) {
    const QString launcherName = this->_pendingBombRelease.launcherEntityName;
    this->clearPendingBombRelease();
    if (!launcherName.trimmed().isEmpty()) {
      this->_ui->statusLabel->setText(
          QStringLiteral("Bomb release cleared for %1.").arg(launcherName));
    }
  }
}

void MainWindow::processAttackTasks(double deltaSeconds) {
  if (!this->_simulationRunning) {
    return;
  }

  if (deltaSeconds > 0.0) {
    for (auto it = this->_attackAirMissileCooldownSeconds.begin();
         it != this->_attackAirMissileCooldownSeconds.end();) {
      it.value() = qMax(0.0, it.value() - deltaSeconds);
      if (it.value() <= 0.0) {
        it = this->_attackAirMissileCooldownSeconds.erase(it);
        continue;
      }
      ++it;
    }
  }

  QStringList taskEntityNames;
  for (const Entity& entity : this->_scenarioState->entities()) {
    const QString taskType = entity.currentTask.taskType.trimmed();
    const QString taskStatus = entity.currentTask.status.trimmed();
    if (entity.destroyed) {
      this->_attackAirElapsedSeconds.remove(entity.name);
      this->_attackAirMissileCooldownSeconds.remove(entity.name);
    }
    if (taskType == QStringLiteral("AttackAir") && domain::attackTaskStatusIsTerminal(taskStatus)) {
      this->_attackAirElapsedSeconds.remove(entity.name);
      this->_attackAirMissileCooldownSeconds.remove(entity.name);
    }
    if (!entity.currentTask.enabled ||
        entity.destroyed ||
        (taskType != QStringLiteral("AttackAir") &&
         taskType != QStringLiteral("AttackSurface")) ||
        domain::attackTaskStatusIsTerminal(taskStatus)) {
      continue;
    }
    taskEntityNames.push_back(entity.name);
  }

  for (const QString& entityName : taskEntityNames) {
    const Entity* entity = this->findEntityByName(entityName);
    if (!entity || entity->destroyed) {
      this->setEntityTaskStatus(entityName, QStringLiteral("Target unavailable"));
      continue;
    }

    const QString taskType = entity->currentTask.taskType.trimmed();
    if (taskType == QStringLiteral("AttackAir")) {
      this->processAttackAirTask(entityName, deltaSeconds);
    } else if (taskType == QStringLiteral("AttackSurface")) {
      this->processAttackSurfaceTask(entityName);
    }
  }
}

bool MainWindow::processAttackAirTask(const QString& entityName, double deltaSeconds) {
  const Entity* launcher = this->findEntityByName(entityName);
  if (!launcher || launcher->destroyed) {
    return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const QString targetName = launcher->currentTask.targetEntityName.trimmed();
  if (targetName.isEmpty()) {
    return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const Entity* target = this->findEntityByName(targetName);
  if (target && target->destroyed) {
    this->setEntityTaskStatus(entityName, QStringLiteral("Completed"));
    this->_attackAirElapsedSeconds.remove(entityName);
    this->_attackAirMissileCooldownSeconds.remove(entityName);
    this->appendLogMessage(
        QStringLiteral("Attack Air task completed for %1: target %2 destroyed.")
            .arg(entityName, targetName));
    this->_ui->statusLabel->setText(
        QStringLiteral("Attack Air completado para %1: target destruido.").arg(entityName));
    return true;
  }
  if (!target || target->name == launcher->name) {
    this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    this->_attackAirElapsedSeconds.remove(entityName);
    this->_attackAirMissileCooldownSeconds.remove(entityName);
    this->appendLogMessage(
        QStringLiteral("Attack Air task failed for %1: target no longer valid.")
            .arg(entityName));
    return true;
  }
  if (target->forceIdentifier == launcher->forceIdentifier ||
      target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0) {
    this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    this->_attackAirElapsedSeconds.remove(entityName);
    this->_attackAirMissileCooldownSeconds.remove(entityName);
    return true;
  }

  this->_attackAirElapsedSeconds[entityName] =
      this->_attackAirElapsedSeconds.value(entityName, 0.0) + qMax(0.0, deltaSeconds);
  if (this->_attackAirElapsedSeconds.value(entityName) >= kAttackAirTimeoutSeconds) {
    this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    this->_attackAirElapsedSeconds.remove(entityName);
    this->_attackAirMissileCooldownSeconds.remove(entityName);
    this->appendLogMessage(
        QStringLiteral("Attack Air task failed for %1: timeout against %2.")
            .arg(entityName, targetName));
    return true;
  }

  Entity* mutableLauncher = nullptr;
  for (Entity& entity : this->_scenarioState->entitiesMutable()) {
    if (entity.name == entityName) {
      mutableLauncher = &entity;
      break;
    }
  }
  if (!mutableLauncher) {
    return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  const double desiredHeadingDegrees = domain::bearingDegrees(
      launcher->latitude,
      launcher->longitude,
      target->latitude,
      target->longitude);
  const double desiredSpeedKnots = qMax(
      kAttackAirMinimumPursuitSpeedKnots,
      target->speedKnots + kAttackAirTargetSpeedMarginKnots);
  mutableLauncher->currentTask.targetLatitude = target->latitude;
  mutableLauncher->currentTask.targetLongitude = target->longitude;
  mutableLauncher->currentTask.targetAltitudeMeters = target->altitude;
  mutableLauncher->currentTask.targetHeadingDegrees = desiredHeadingDegrees;
  mutableLauncher->currentTask.targetSpeedKnots = desiredSpeedKnots;
  mutableLauncher->currentTask.status = QStringLiteral("Running");

  const bool activeMissile =
      activeMissileInFlightForTarget(this->_scenarioState, entityName, targetName);
  if (domain::weaponQuantity(*launcher, QStringLiteral("Missile")) <= 0) {
    if (!activeMissile) {
      this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
      this->_attackAirElapsedSeconds.remove(entityName);
      this->_attackAirMissileCooldownSeconds.remove(entityName);
      this->appendLogMessage(
          QStringLiteral("Attack Air task failed for %1: no missiles remaining.")
              .arg(entityName));
      return true;
    }
    return false;
  }

  if (activeMissile) {
    return false;
  }

  if (this->_attackAirMissileCooldownSeconds.value(entityName, 0.0) > 0.0) {
    return false;
  }

  if (this->_scenarioState->launchMissileAt(entityName, targetName)) {
    this->_attackAirMissileCooldownSeconds[entityName] =
        kAttackAirMissileCooldownSeconds;
    this->appendLogMessage(
        QStringLiteral("Attack Air task launched missile from %1 at %2")
            .arg(entityName, targetName));
    this->_ui->statusLabel->setText(
        QStringLiteral("Attack Air: %1 persiguiendo y atacando %2.")
            .arg(entityName, targetName));
    return true;
  }

  return false;
}

bool MainWindow::processAttackSurfaceTask(const QString& entityName) {
  const Entity* launcher = this->findEntityByName(entityName);
  if (!launcher || launcher->destroyed) {
    return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  if (domain::weaponQuantity(*launcher, QStringLiteral("Bomb")) <= 0) {
    return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
  }

  this->setEntityTaskStatus(entityName, QStringLiteral("Running"));

  if (this->_pendingBombRelease.pending) {
    if (this->_pendingBombRelease.launcherEntityName.compare(
            entityName,
            Qt::CaseInsensitive) == 0) {
      return this->setEntityTaskStatus(entityName, QStringLiteral("Completed"));
    }
    this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    return false;
  }

  const EntityTask task = launcher->currentTask;
  const QString targetName = task.targetEntityName.trimmed();
  QString targetLabel;
  QString targetEntityName;
  double targetLatitude = task.targetLatitude;
  double targetLongitude = task.targetLongitude;
  double targetAltitudeMeters = static_cast<double>(task.targetAltitudeMeters);

  if (!targetName.isEmpty()) {
    const Entity* target = this->findEntityByName(targetName);
    if (!target || target->destroyed) {
      this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
      this->appendLogMessage(
          QStringLiteral("Attack Surface task failed for %1: target no longer valid.")
              .arg(entityName));
      return true;
    }
    if (target->name == launcher->name ||
        target->forceIdentifier == launcher->forceIdentifier ||
        target->domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0) {
      return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }

    targetLatitude = target->latitude;
    targetLongitude = target->longitude;
    targetAltitudeMeters = static_cast<double>(target->altitude);
    targetLabel = target->name;
    targetEntityName = target->name;
  } else {
    if (!domain::attackSurfaceCoordinatesAreUsable(targetLatitude, targetLongitude)) {
      return this->setEntityTaskStatus(entityName, QStringLiteral("Failed"));
    }
    targetLabel = attackPointLabel(targetLatitude, targetLongitude);
  }

  this->queuePendingBombRelease(
      entityName,
      targetLatitude,
      targetLongitude,
      targetAltitudeMeters,
      targetLabel,
      QStringLiteral("Attack Surface Task"),
      targetEntityName,
      false,
      false);
  this->setEntityTaskStatus(entityName, QStringLiteral("Completed"));
  this->appendLogMessage(
      QStringLiteral("Attack Surface task armed bomb release for %1 at %2")
          .arg(entityName, targetLabel));
  this->_ui->statusLabel->setText(
      QStringLiteral("Attack Surface armado para %1.").arg(entityName));
  return true;
}

bool MainWindow::setEntityTaskStatus(const QString& entityName, const QString& status) {
  for (Entity& entity : this->_scenarioState->entitiesMutable()) {
    if (entity.name != entityName) {
      continue;
    }
    if (entity.currentTask.status == status) {
      return true;
    }
    entity.currentTask.status = status;
    this->_scenarioState->save();
    return true;
  }
  return false;
}

void MainWindow::processAutoBombingBehaviors(double deltaSeconds) {
  if (deltaSeconds > 0.0) {
    for (auto it = this->_autoBombReleaseCooldownSeconds.begin();
         it != this->_autoBombReleaseCooldownSeconds.end();) {
      it.value() = qMax(0.0, it.value() - deltaSeconds);
      if (it.value() <= 0.0) {
        it = this->_autoBombReleaseCooldownSeconds.erase(it);
        continue;
      }
      ++it;
    }
  }

  if (!this->_simulationRunning || this->_pendingBombRelease.pending) {
    return;
  }

  for (const Entity& launcher : this->_scenarioState->entities()) {
    const QString taskType = launcher.currentTask.taskType.trimmed();
    if (launcher.currentTask.enabled &&
        (taskType == QStringLiteral("AttackAir") ||
         taskType == QStringLiteral("AttackSurface"))) {
      continue;
    }

    const QString behaviorMode = launcher.behaviorMode.trimmed().isEmpty()
        ? QStringLiteral("Manual")
        : launcher.behaviorMode.trimmed();
    if (behaviorMode.compare(QStringLiteral("Aggressive"), Qt::CaseInsensitive) != 0 ||
        launcher.destroyed ||
        launcher.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) != 0 ||
        domain::weaponQuantity(launcher, QStringLiteral("Bomb")) <= 0) {
      this->_autoBehaviorDamageReactionLevel.remove(launcher.name);
      continue;
    }

    const int damageReactionLevel = domain::autoBehaviorDamageReactionLevel(launcher);
    const int previousReactionLevel =
        this->_autoBehaviorDamageReactionLevel.value(launcher.name, -1);
    if (damageReactionLevel != previousReactionLevel) {
      this->_autoBehaviorDamageReactionLevel.insert(launcher.name, damageReactionLevel);
      if (damageReactionLevel >= 2) {
        this->appendLogMessage(
            QStringLiteral("%1 auto bombing blocked: critical damage (%2%).")
                .arg(launcher.name)
                .arg(launcher.damagePercent, 0, 'f', 0));
      } else if (damageReactionLevel >= 1) {
        this->appendLogMessage(
            QStringLiteral("%1 auto bombing blocked: damage threshold reached (%2%).")
                .arg(launcher.name)
                .arg(launcher.damagePercent, 0, 'f', 0));
      } else {
        this->appendLogMessage(
            QStringLiteral("%1 auto bombing re-enabled by damage state (%2%).")
                .arg(launcher.name)
                .arg(launcher.damagePercent, 0, 'f', 0));
      }
    }
    if (!domain::autoBehaviorCanEngageByDamage(launcher)) {
      continue;
    }

    const auto cooldownIt =
        this->_autoBombReleaseCooldownSeconds.constFind(launcher.name);
    if (cooldownIt != this->_autoBombReleaseCooldownSeconds.constEnd() &&
        cooldownIt.value() > 0.0) {
      continue;
    }

    const Entity* target =
        bestDetectedSurfaceBombTarget(this->_scenarioState, launcher);
    if (!target) {
      continue;
    }

    this->queuePendingBombRelease(
        launcher.name,
        target->latitude,
        target->longitude,
        static_cast<double>(target->altitude),
        target->name,
        QStringLiteral("Auto Behavior"),
        target->name,
        false,
        false);
    this->_autoBombReleaseCooldownSeconds.insert(
        launcher.name,
        kAutoBombReleaseCooldownSeconds);
    this->appendLogMessage(
        QStringLiteral("%1 auto-armed bomb release at %2")
            .arg(launcher.name, target->name));
    this->_ui->statusLabel->setText(
        QStringLiteral("%1 auto-armed bomb release at %2.")
            .arg(launcher.name, target->name));
    return;
  }
}

void MainWindow::processPendingBombRelease() {
  if (!this->_pendingBombRelease.pending) {
    return;
  }

  const Entity* launcher =
      this->findEntityByName(this->_pendingBombRelease.launcherEntityName);
  if (!launcher || launcher->destroyed) {
    this->validatePendingBombRelease();
    return;
  }

  const int bombCount = domain::weaponQuantity(*launcher, QStringLiteral("Bomb"));
  if (bombCount <= 0) {
    const QString launcherName = this->_pendingBombRelease.launcherEntityName;
    this->clearPendingBombRelease();
    this->_ui->statusLabel->setText(
        QStringLiteral("Bomb release cleared for %1. No bombs available.")
            .arg(launcherName));
    return;
  }

  const domain::BombReleaseGateEvaluation evaluation = domain::evaluateBombReleaseGate(
      *launcher,
      this->_pendingBombRelease.targetLatitude,
      this->_pendingBombRelease.targetLongitude,
      this->_pendingBombRelease.targetAltitudeMeters);
  if (!evaluation.readyToRelease()) {
    return;
  }
  if (this->_pendingBombRelease.releaseCommandIssued) {
    return;
  }
  this->_pendingBombRelease.releaseCommandIssued = true;

  const QString launcherName = this->_pendingBombRelease.launcherEntityName;
  const QString targetLabel = this->_pendingBombRelease.targetLabel;
  const QString sourceDescription = this->_pendingBombRelease.sourceDescription;
  this->_ui->statusLabel->setText(
      QStringLiteral("Bomb release window reached for %1 on %2.")
          .arg(launcherName, targetLabel));
  QTimer::singleShot(0, this, [this, launcherName, targetLabel, sourceDescription]() {
    if (!this->_pendingBombRelease.pending ||
        this->_pendingBombRelease.launcherEntityName != launcherName) {
      return;
    }
    if (!this->_scenarioState->releaseBomb(launcherName)) {
      this->clearPendingBombRelease();
      this->_ui->statusLabel->setText(
          QStringLiteral("No se pudo soltar una bomba desde %1.")
              .arg(launcherName));
      this->syncScenarioStateToUi();
      return;
    }

    this->appendLogMessage(
        QStringLiteral("Bomb released from %1 at %2 (%3)")
            .arg(
                launcherName,
                targetLabel,
                sourceDescription));
    this->clearPendingBombRelease();
    this->_ui->statusLabel->setText(
        QStringLiteral("Bomba soltada desde %1 sobre %2.")
            .arg(launcherName, targetLabel));
    this->syncScenarioStateToUi();
  });
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
      validBombReleaseTargets(this->_scenarioState, *launcher);
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
    const QString option = bombTargetDisplayLabel(*target);
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
  this->_isPickingBombTarget = false;
  this->_bombTargetPickLauncherName.clear();

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

  if (!this->_pendingGraphicMode.trimmed().isEmpty()) {
    this->_ui->statusLabel->setText(
        QStringLiteral("Termina antes la captura de coordenadas que ya esta activa."));
    return;
  }

  this->_isPickingBombTarget = true;
  this->_bombTargetPickLauncherName = launcherName;
  this->beginTaskCoordinatePick();
  this->_ui->statusLabel->setText(
      QStringLiteral("Haz clic en el mapa para fijar el punto de ataque de %1.")
          .arg(launcherName));
}

void MainWindow::cancelPendingBombRelease() {
  if (!this->_pendingBombRelease.pending) {
    this->_ui->statusLabel->setText(QStringLiteral("No hay release de bomba pendiente."));
    return;
  }

  const QString launcherName = this->_pendingBombRelease.launcherEntityName;
  const QString targetLabel = this->_pendingBombRelease.targetLabel;
  this->_isPickingBombTarget = false;
  this->_bombTargetPickLauncherName.clear();
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

  const auto planIt = this->_entityPlans.constFind(entityName);
  if (planIt != this->_entityPlans.constEnd() && planIt->running) {
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
