#include "EntityDetailsDialog.h"
#include "infrastructure/DisEntityCatalog.h"

#include <QAbstractItemView>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVariantList>
#include <QVBoxLayout>

#include <utility>

namespace {

const DisEntityCatalog& disEntityCatalog() {
  static const DisEntityCatalog catalog = DisEntityCatalog::loadDefault();
  return catalog;
}

double summaryDoubleValue(
    const QVariantMap& summary,
    const char* key,
    double fallback = 0.0) {
  bool ok = false;
  const double result =
      summary.value(QString::fromLatin1(key)).toDouble(&ok);
  return ok ? result : fallback;
}

QString formatNumber(double value, int decimals = 1) {
  return QString::number(value, 'f', decimals);
}

QString formatFeet(double altitudeMeters) {
  return QStringLiteral("%1 ft")
      .arg(qRound(altitudeMeters * 3.28084));
}

QString yesNo(bool value) {
  return value ? QStringLiteral("Yes") : QStringLiteral("No");
}

QString textOrDash(const QVariant& value) {
  const QString text = value.toString().trimmed();
  return text.isEmpty() ? QStringLiteral("-") : text;
}

int summaryIntValue(const QVariantMap& summary, const char* key) {
  bool ok = false;
  const int value = summary.value(QString::fromLatin1(key)).toString().toInt(&ok);
  if (ok) {
    return value;
  }
  return summary.value(QString::fromLatin1(key)).toInt();
}

} // namespace

EntityDetailsDialog::EntityDetailsDialog(
    const QVariantMap& summary,
    SummaryProvider summaryProvider,
    QWidget* parent)
    : QDialog(parent),
      _summary(summary),
      _summaryProvider(std::move(summaryProvider)),
      _iconLabel(nullptr),
      _nameLabel(nullptr),
      _typeLabel(nullptr),
      _groundSpeedLabel(nullptr),
      _altitudeLabel(nullptr),
      _tasLabel(nullptr),
      _statusLabel(nullptr),
      _sectionsList(nullptr),
      _detailsTable(nullptr),
      _console(nullptr) {
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(this->value("name", QStringLiteral("Entity Details")));
  this->resize(1180, 760);
  this->buildUi();
  this->populateHeader();
  this->populateStateData();

  if (_summaryProvider) {
    auto* refreshTimer = new QTimer(this);
    refreshTimer->setInterval(500);
    QObject::connect(
        refreshTimer,
        &QTimer::timeout,
        this,
        &EntityDetailsDialog::refreshSummary);
    refreshTimer->start();
  }
}

void EntityDetailsDialog::buildUi() {
  auto* rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(12, 12, 12, 12);
  rootLayout->setSpacing(10);

  auto* headerFrame = new QFrame(this);
  headerFrame->setObjectName(QStringLiteral("entityHeader"));
  headerFrame->setStyleSheet(QStringLiteral(
      "QFrame#entityHeader { background: #2f2c2b; border: 1px solid #4c4745; }"
      "QLabel { color: #eef3f8; }"));
  auto* headerLayout = new QHBoxLayout(headerFrame);
  headerLayout->setContentsMargins(16, 14, 16, 14);
  headerLayout->setSpacing(18);

  _iconLabel = new QLabel(headerFrame);
  _iconLabel->setFixedSize(64, 64);
  _iconLabel->setAlignment(Qt::AlignCenter);
  _iconLabel->setStyleSheet(QStringLiteral(
      "background: #7fddff; color: #0d1822; border-radius: 8px; font: bold 30px;"));
  headerLayout->addWidget(_iconLabel, 0, Qt::AlignTop);

  auto* infoLayout = new QGridLayout();
  infoLayout->setHorizontalSpacing(24);
  infoLayout->setVerticalSpacing(8);

  _nameLabel = new QLabel(headerFrame);
  _typeLabel = new QLabel(headerFrame);
  _groundSpeedLabel = new QLabel(headerFrame);
  _altitudeLabel = new QLabel(headerFrame);
  _tasLabel = new QLabel(headerFrame);
  _statusLabel = new QLabel(headerFrame);

  QFont titleFont;
  titleFont.setBold(true);
  titleFont.setPointSize(15);
  _nameLabel->setFont(titleFont);

  infoLayout->addWidget(_nameLabel, 0, 0);
  infoLayout->addWidget(_typeLabel, 0, 1);
  infoLayout->addWidget(_groundSpeedLabel, 0, 2);
  infoLayout->addWidget(_statusLabel, 0, 3);
  infoLayout->addWidget(_altitudeLabel, 1, 2);
  infoLayout->addWidget(_tasLabel, 2, 2);

  headerLayout->addLayout(infoLayout, 1);
  rootLayout->addWidget(headerFrame);

  auto* bodyLayout = new QHBoxLayout();
  bodyLayout->setSpacing(8);

  _sectionsList = new QListWidget(this);
  _sectionsList->setFixedWidth(250);
  _sectionsList->addItems({
      QStringLiteral("State Data"),
      QStringLiteral("Tasks"),
      QStringLiteral("Resources"),
      QStringLiteral("Sensor Information"),
      QStringLiteral("Emitters"),
      QStringLiteral("Appearance"),
      QStringLiteral("Subsystems")});
  _sectionsList->setCurrentRow(0);
  _sectionsList->setStyleSheet(QStringLiteral(
      "QListWidget { background: #3a3635; color: #e7edf4; border: 1px solid #4c4745; }"
      "QListWidget::item { padding: 10px 12px; }"
      "QListWidget::item:selected { background: #4a6f89; }"));
  bodyLayout->addWidget(_sectionsList);

  _detailsTable = new QTableWidget(this);
  _detailsTable->setColumnCount(2);
  _detailsTable->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Value")});
  _detailsTable->horizontalHeader()->setStretchLastSection(true);
  _detailsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  _detailsTable->verticalHeader()->setVisible(false);
  _detailsTable->setSelectionMode(QAbstractItemView::NoSelection);
  _detailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  _detailsTable->setAlternatingRowColors(false);
  _detailsTable->setStyleSheet(QStringLiteral(
      "QTableWidget { background: #3a3635; color: #eef3f8; gridline-color: #4c4745; border: 1px solid #4c4745; }"
      "QHeaderView::section { background: #2f2c2b; color: #eef3f8; padding: 6px; border: 0; }"));
  bodyLayout->addWidget(_detailsTable, 1);

  rootLayout->addLayout(bodyLayout, 1);

  auto* consoleLabel = new QLabel(QStringLiteral("Object Console"), this);
  consoleLabel->setStyleSheet(QStringLiteral("color: #eef3f8; font-weight: 700;"));
  rootLayout->addWidget(consoleLabel);

  _console = new QPlainTextEdit(this);
  _console->setReadOnly(true);
  _console->setMaximumBlockCount(200);
  _console->setStyleSheet(QStringLiteral(
      "QPlainTextEdit { background: #2d2928; color: #e7edf4; border: 1px solid #4c4745; }"));
  rootLayout->addWidget(_console);

  auto* footerLayout = new QHBoxLayout();
  footerLayout->addStretch(1);
  auto* closeButton = new QPushButton(QStringLiteral("Close"), this);
  QObject::connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
  footerLayout->addWidget(closeButton);
  rootLayout->addLayout(footerLayout);

  QObject::connect(
      _sectionsList,
      &QListWidget::currentItemChanged,
      this,
      &EntityDetailsDialog::updateSection);
}

void EntityDetailsDialog::populateHeader() {
  _iconLabel->setText(this->forceGlyph());
  _nameLabel->setText(QStringLiteral("Name: %1").arg(this->value("name")));
  _typeLabel->setText(QStringLiteral("Type: %1").arg(this->value("type")));

  const double altitudeMeters = summaryDoubleValue(_summary, "altitudeMeters");
  const double speedKnots = summaryDoubleValue(_summary, "speedKnots");
  _groundSpeedLabel->setText(
      QStringLiteral("Speed: %1 kts").arg(formatNumber(speedKnots)));
  _altitudeLabel->setText(
      QStringLiteral("Altitude: %1 m / %2")
          .arg(formatNumber(altitudeMeters, 0), formatFeet(altitudeMeters)));
  _tasLabel->setText(
      QStringLiteral("Heading / Pitch / Roll: %1 / %2 / %3 deg")
          .arg(formatNumber(summaryDoubleValue(_summary, "headingDegrees")))
          .arg(formatNumber(summaryDoubleValue(_summary, "pitchDegrees")))
          .arg(formatNumber(summaryDoubleValue(_summary, "rollDegrees"))));
  _statusLabel->setText(QStringLiteral("Status: %1").arg(this->value("status")));

  _console->setPlainText(
      QStringLiteral("[%1] Entity selected\n[%2] %3\n[%4] Details panel ready")
          .arg(this->value("name"), this->value("type"), this->value("team"), this->locationText()));
}

void EntityDetailsDialog::populateStateData() {
  const QString entityTypeCode = this->value(
      "entityTypeCode",
      Entity::buildEntityTypeCode(
          summaryIntValue(_summary, "entityKind"),
          summaryIntValue(_summary, "entityDomain"),
          summaryIntValue(_summary, "entityCountry"),
          summaryIntValue(_summary, "entityCategory"),
          summaryIntValue(_summary, "entitySubcategory"),
          summaryIntValue(_summary, "entitySpecific"),
          summaryIntValue(_summary, "entityExtra")));
  const DisEntityInfo disInfo = disEntityCatalog().lookup(entityTypeCode);
  const double altitudeMeters = summaryDoubleValue(_summary, "altitudeMeters");

  QList<QPair<QString, QString>> rows;
  rows.append({QStringLiteral("Entity ID"), this->value("entityId")});
  rows.append({QStringLiteral("Name"), this->value("name")});
  rows.append({QStringLiteral("Callsign"), this->value("callsign")});
  rows.append({QStringLiteral("Force"), this->value("team")});
  rows.append({QStringLiteral("Domain"), this->value("domain")});
  rows.append({QStringLiteral("Category"), this->value("category")});
  rows.append({QStringLiteral("Damage State"), this->value("damageState")});
  rows.append({QStringLiteral("Damage"), QStringLiteral("%1 %").arg(formatNumber(summaryDoubleValue(_summary, "damagePercent")))});
  rows.append({QStringLiteral("Destroyed"), yesNo(_summary.value(QStringLiteral("destroyed")).toBool())});
  rows.append({QStringLiteral("Latitude"), formatNumber(summaryDoubleValue(_summary, "latitude"), 6)});
  rows.append({QStringLiteral("Longitude"), formatNumber(summaryDoubleValue(_summary, "longitude"), 6)});
  rows.append({QStringLiteral("Altitude MSL"), QStringLiteral("%1 m / %2").arg(formatNumber(altitudeMeters, 0), formatFeet(altitudeMeters))});
  rows.append({QStringLiteral("Ground Height"), QStringLiteral("%1 m").arg(formatNumber(summaryDoubleValue(_summary, "groundHeightMeters"), 1))});
  rows.append({QStringLiteral("Speed"), QStringLiteral("%1 kts").arg(formatNumber(summaryDoubleValue(_summary, "speedKnots")))});
  rows.append({QStringLiteral("Vertical Speed"), QStringLiteral("%1 m/s").arg(formatNumber(summaryDoubleValue(_summary, "verticalSpeedMetersPerSecond")))});
  rows.append({QStringLiteral("Heading"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "headingDegrees")))});
  rows.append({QStringLiteral("Pitch"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "pitchDegrees")))});
  rows.append({QStringLiteral("Roll"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "rollDegrees")))});
  rows.append({QStringLiteral("Entity Type Code"), entityTypeCode});
  rows.append({QStringLiteral("DIS Entity"), disInfo.displayName().isEmpty() ? QStringLiteral("-") : disInfo.displayName()});
  rows.append({QStringLiteral("DIS Kind Description"), disInfo.kindDescription.isEmpty() ? QStringLiteral("-") : disInfo.kindDescription});
  rows.append({QStringLiteral("DIS Domain Description"), disInfo.domainDescription.isEmpty() ? QStringLiteral("-") : disInfo.domainDescription});
  rows.append({QStringLiteral("DIS Country Description"), disInfo.countryDescription.isEmpty() ? QStringLiteral("-") : disInfo.countryDescription});
  rows.append({QStringLiteral("DIS Category Description"), disInfo.categoryDescription.isEmpty() ? QStringLiteral("-") : disInfo.categoryDescription});
  rows.append({QStringLiteral("DIS Subcategory Description"), disInfo.subcategoryDescription.isEmpty() ? QStringLiteral("-") : disInfo.subcategoryDescription});
  rows.append({QStringLiteral("DIS Specific Description"), disInfo.specificDescription.isEmpty() ? QStringLiteral("-") : disInfo.specificDescription});
  rows.append({QStringLiteral("DIS Extra Description"), disInfo.extraDescription.isEmpty() ? QStringLiteral("-") : disInfo.extraDescription});
  rows.append({QStringLiteral("APP-6 Kind"), this->value("entityKind")});
  rows.append({QStringLiteral("APP-6 Domain"), this->value("entityDomain")});
  rows.append({QStringLiteral("APP-6 Country"), this->value("entityCountry")});
  rows.append({QStringLiteral("APP-6 Category"), this->value("entityCategory")});
  rows.append({QStringLiteral("APP-6 Subcategory"), this->value("entitySubcategory")});
  rows.append({QStringLiteral("APP-6 Specific"), this->value("entitySpecific")});
  rows.append({QStringLiteral("APP-6 Extra"), this->value("entityExtra")});
  if (!disEntityCatalog().isLoaded()) {
    rows.append({QStringLiteral("DIS Catalog Status"), disEntityCatalog().errorString().isEmpty()
                                                    ? QStringLiteral("Unavailable")
                                                    : disEntityCatalog().errorString()});
  }
  this->setTableRows(rows);
}

void EntityDetailsDialog::populateTaskInformation() {
  QList<QPair<QString, QString>> rows;
  const bool enabled = _summary.value(QStringLiteral("taskEnabled")).toBool();
  rows.append({QStringLiteral("Task Type"), this->value("taskType", QStringLiteral("None"))});
  rows.append({QStringLiteral("Enabled"), yesNo(enabled)});
  rows.append({QStringLiteral("Status"), this->value("taskStatus", QStringLiteral("Idle"))});

  if (enabled) {
    rows.append({QStringLiteral("Elapsed"), QStringLiteral("%1 s").arg(formatNumber(summaryDoubleValue(_summary, "taskElapsedSeconds")))});
    const double durationSeconds = summaryDoubleValue(_summary, "taskDurationSeconds");
    rows.append({QStringLiteral("Duration"), durationSeconds > 0.0
                                              ? QStringLiteral("%1 s").arg(formatNumber(durationSeconds))
                                              : QStringLiteral("Until cancelled")});

    const QString targetEntity = this->value("taskTargetEntityName");
    const QString targetEntityId = this->value("taskTargetEntityId");
    if (targetEntity != QStringLiteral("-") || targetEntityId != QStringLiteral("-")) {
      rows.append({QStringLiteral("Target Entity"), targetEntity});
      rows.append({QStringLiteral("Target Entity ID"), targetEntityId});
    }
    if (this->value("taskTargetWaypointName") != QStringLiteral("-")) {
      rows.append({QStringLiteral("Target Waypoint"), this->value("taskTargetWaypointName")});
    }
    if (this->value("taskTargetRouteName") != QStringLiteral("-")) {
      rows.append({QStringLiteral("Target Route"), this->value("taskTargetRouteName")});
    }
    if (this->value("taskTargetAreaName") != QStringLiteral("-")) {
      rows.append({QStringLiteral("Target Area"), this->value("taskTargetAreaName")});
    }

    rows.append({QStringLiteral("Target Location"), QStringLiteral("%1, %2")
        .arg(formatNumber(summaryDoubleValue(_summary, "taskTargetLatitude"), 6))
        .arg(formatNumber(summaryDoubleValue(_summary, "taskTargetLongitude"), 6))});
    rows.append({QStringLiteral("Target Heading"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "taskTargetHeadingDegrees")))});
    rows.append({QStringLiteral("Target Altitude"), QStringLiteral("%1 m").arg(formatNumber(summaryDoubleValue(_summary, "taskTargetAltitudeMeters"), 0))});
    rows.append({QStringLiteral("Target Speed"), QStringLiteral("%1 kts").arg(formatNumber(summaryDoubleValue(_summary, "taskTargetSpeedKnots")))});

    const int routeTotal = _summary.value(QStringLiteral("taskRouteTotalWaypoints")).toInt();
    if (routeTotal > 0) {
      const int currentIndex = _summary.value(QStringLiteral("taskRouteCurrentWaypointIndex")).toInt();
      rows.append({QStringLiteral("Route Progress"), QStringLiteral("%1 / %2").arg(currentIndex + 1).arg(routeTotal)});
    }
    const double timeoutSeconds = summaryDoubleValue(_summary, "taskTimeoutSeconds");
    if (timeoutSeconds > 0.0) {
      rows.append({QStringLiteral("Timeout"), QStringLiteral("%1 s").arg(formatNumber(timeoutSeconds))});
    }
    if (this->value("taskWeaponType") != QStringLiteral("-")) {
      rows.append({QStringLiteral("Weapon Type"), this->value("taskWeaponType")});
    }
    rows.append({QStringLiteral("Arrival Tolerance"), QStringLiteral("%1 m").arg(formatNumber(summaryDoubleValue(_summary, "taskArrivalToleranceMeters"), 0))});
    rows.append({QStringLiteral("Follow Distance"), QStringLiteral("%1 m").arg(formatNumber(summaryDoubleValue(_summary, "taskFollowDistanceMeters"), 0))});
    rows.append({QStringLiteral("Intercept Distance"), QStringLiteral("%1 m").arg(formatNumber(summaryDoubleValue(_summary, "taskInterceptDistanceMeters"), 0))});
    rows.append({QStringLiteral("Altitude Tolerance"), QStringLiteral("%1 m").arg(formatNumber(summaryDoubleValue(_summary, "taskAltitudeToleranceMeters"), 0))});
  }

  this->setTableRows(rows);
}

void EntityDetailsDialog::populateAppearanceInformation() {
  QList<QPair<QString, QString>> rows;
  rows.append({QStringLiteral("Model"), this->value("modelName")});
  rows.append({QStringLiteral("Model URI"), this->value("modelUri")});
  rows.append({QStringLiteral("Cesium Model Axes"), this->value("cesiumModelAxes")});
  rows.append({QStringLiteral("Heading"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "headingDegrees")))});
  rows.append({QStringLiteral("Pitch"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "pitchDegrees")))});
  rows.append({QStringLiteral("Roll"), QStringLiteral("%1 deg").arg(formatNumber(summaryDoubleValue(_summary, "rollDegrees")))});
  rows.append({QStringLiteral("Hidden"), yesNo(_summary.value(QStringLiteral("hidden")).toBool())});
  rows.append({QStringLiteral("Radar Coverage Visible"), yesNo(_summary.value(QStringLiteral("radarCoverageVisible")).toBool())});
  rows.append({QStringLiteral("Track History Visible"), yesNo(_summary.value(QStringLiteral("trackHistoryVisible")).toBool())});
  this->setTableRows(rows);
}

void EntityDetailsDialog::populateResourcesInformation() {
  QList<QPair<QString, QString>> rows;
  const double fuelCapacity = summaryDoubleValue(_summary, "fuelCapacityKilograms");
  const double fuelRemaining = summaryDoubleValue(_summary, "fuelRemainingKilograms");
  rows.append({QStringLiteral("Fuel Capacity"), QStringLiteral("%1 kg").arg(formatNumber(fuelCapacity, 1))});
  rows.append({QStringLiteral("Fuel Remaining"), QStringLiteral("%1 kg").arg(formatNumber(fuelRemaining, 1))});
  rows.append({QStringLiteral("Fuel Remaining Percent"), fuelCapacity > 0.0
      ? QStringLiteral("%1 %").arg(formatNumber(100.0 * fuelRemaining / fuelCapacity, 1))
      : QStringLiteral("-")});

  const QVariantList weapons = _summary.value(QStringLiteral("weapons")).toList();
  int totalWeapons = 0;
  for (const QVariant& value : weapons) {
    totalWeapons += qMax(0, value.toMap().value(QStringLiteral("quantity")).toInt());
  }
  rows.append({QStringLiteral("Weapon Types"), QString::number(weapons.size())});
  rows.append({QStringLiteral("Total Weapons"), QString::number(totalWeapons)});
  for (int index = 0; index < weapons.size(); ++index) {
    const QVariantMap weapon = weapons.at(index).toMap();
    rows.append({
        QStringLiteral("Weapon %1").arg(index + 1),
        QStringLiteral("%1 x %2")
            .arg(weapon.value(QStringLiteral("quantity")).toInt())
            .arg(textOrDash(weapon.value(QStringLiteral("weaponType"))))});
  }
  this->setTableRows(rows);
}

void EntityDetailsDialog::populateSensorInformation() {
  QList<QPair<QString, QString>> rows;
  const QVariantList sensors = _summary.value(QStringLiteral("sensors")).toList();
  const QVariantList contacts = _summary.value(QStringLiteral("sensorContacts")).toList();

  rows.append({QStringLiteral("Configured Sensors"), QString::number(sensors.size())});
  rows.append({QStringLiteral("Radar Signature"), this->value("radarSignature", QStringLiteral("1.00"))});
  rows.append({QStringLiteral("Thermal Signature"), this->value("thermalSignature", QStringLiteral("1.00"))});
  rows.append({QStringLiteral("Visual Signature"), this->value("visualSignature", QStringLiteral("1.00"))});
  if (sensors.isEmpty()) {
    rows.append({QStringLiteral("Sensor"), QStringLiteral("No sensors configured")});
  } else {
    for (int index = 0; index < sensors.size(); ++index) {
      const QVariantMap sensor = sensors.at(index).toMap();
      const QString prefix = QStringLiteral("Sensor %1").arg(index + 1);
      rows.append({prefix + QStringLiteral(" Name"), sensor.value(QStringLiteral("name")).toString()});
      rows.append({prefix + QStringLiteral(" Model Provider"), sensor.value(QStringLiteral("modelProviderId"), QStringLiteral("native")).toString()});
      rows.append({prefix + QStringLiteral(" Type"), sensor.value(QStringLiteral("sensorType")).toString()});
      rows.append({prefix + QStringLiteral(" Subtype"), sensor.value(QStringLiteral("sensorSubType")).toString()});
      rows.append({prefix + QStringLiteral(" Enabled"), sensor.value(QStringLiteral("enabled")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" Emitting"), sensor.value(QStringLiteral("emitting")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" Minimum Range"), QStringLiteral("%1 km").arg(sensor.value(QStringLiteral("minRangeMeters")).toDouble() / 1000.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Range"), QStringLiteral("%1 km").arg(sensor.value(QStringLiteral("maxRangeMeters")).toDouble() / 1000.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Azimuth"), QStringLiteral("%1 deg").arg(sensor.value(QStringLiteral("azimuthWidthDegrees")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Elevation"), QStringLiteral("%1 deg").arg(sensor.value(QStringLiteral("elevationWidthDegrees")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Update Period"), QStringLiteral("%1 s").arg(sensor.value(QStringLiteral("updatePeriodSeconds")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Detection Probability"), QStringLiteral("%1 %").arg(sensor.value(QStringLiteral("probabilityOfDetection")).toDouble() * 100.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Track Hold"), QStringLiteral("%1 s").arg(sensor.value(QStringLiteral("trackHoldSeconds")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Max Tracks"), QString::number(sensor.value(QStringLiteral("maxTracks")).toInt())});
    }
  }

  rows.append({QStringLiteral("Detected Contacts"), QString::number(contacts.size())});
  if (contacts.isEmpty()) {
    rows.append({QStringLiteral("Contact"), QStringLiteral("No current contacts")});
  } else {
    for (int index = 0; index < contacts.size(); ++index) {
      const QVariantMap contact = contacts.at(index).toMap();
      const QString prefix = QStringLiteral("Contact %1").arg(index + 1);
      rows.append({prefix + QStringLiteral(" Target"), contact.value(QStringLiteral("targetEntityName")).toString()});
      rows.append({prefix + QStringLiteral(" Target ID"), contact.value(QStringLiteral("targetEntityId")).toString()});
      rows.append({prefix + QStringLiteral(" Effective Model"), contact.value(QStringLiteral("sensorModelProviderId"), QStringLiteral("native")).toString()});
      rows.append({prefix + QStringLiteral(" Sensor"), contact.value(QStringLiteral("sensorId")).toString()});
      rows.append({prefix + QStringLiteral(" Range"), QStringLiteral("%1 km").arg(contact.value(QStringLiteral("rangeMeters")).toDouble() / 1000.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Bearing"), QStringLiteral("%1 deg").arg(contact.value(QStringLiteral("bearingDegrees")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" LOS"), contact.value(QStringLiteral("lineOfSight")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" Detected"), contact.value(QStringLiteral("detected")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" State"), contact.value(QStringLiteral("trackState")).toString()});
      rows.append({prefix + QStringLiteral(" Confidence"), QStringLiteral("%1 %").arg(contact.value(QStringLiteral("confidence")).toDouble() * 100.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Last Seen"), QStringLiteral("%1 s").arg(contact.value(QStringLiteral("lastSeenSimulationSeconds")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Missed Detections"), QString::number(contact.value(QStringLiteral("missedDetectionCount")).toInt())});
    }
  }

  this->setTableRows(rows);
}

void EntityDetailsDialog::populateEmitterInformation() {
  QList<QPair<QString, QString>> rows;
  const QVariantList sensors = _summary.value(QStringLiteral("sensors")).toList();
  int activeEmitters = 0;
  for (const QVariant& value : sensors) {
    const QVariantMap sensor = value.toMap();
    if (sensor.value(QStringLiteral("enabled")).toBool() &&
        sensor.value(QStringLiteral("emitting")).toBool()) {
      ++activeEmitters;
    }
  }
  rows.append({QStringLiteral("Active Emitters"), QString::number(activeEmitters)});
  for (int index = 0; index < sensors.size(); ++index) {
    const QVariantMap sensor = sensors.at(index).toMap();
    const QString prefix = QStringLiteral("Emitter %1").arg(index + 1);
    rows.append({prefix + QStringLiteral(" Name"), textOrDash(sensor.value(QStringLiteral("name")))});
    rows.append({prefix + QStringLiteral(" Type"), textOrDash(sensor.value(QStringLiteral("sensorType")))});
    rows.append({prefix + QStringLiteral(" Provider"), textOrDash(sensor.value(QStringLiteral("modelProviderId")))});
    rows.append({prefix + QStringLiteral(" Enabled"), yesNo(sensor.value(QStringLiteral("enabled")).toBool())});
    rows.append({prefix + QStringLiteral(" Emitting"), yesNo(sensor.value(QStringLiteral("emitting")).toBool())});
  }
  this->setTableRows(rows);
}

void EntityDetailsDialog::populateSubsystemInformation() {
  QList<QPair<QString, QString>> rows;
  rows.append({QStringLiteral("Flight Dynamics"), _summary.value(QStringLiteral("flightDynamicsEnabled")).toBool()
      ? QStringLiteral("Enabled") : QStringLiteral("Disabled")});
  rows.append({QStringLiteral("Configured Dynamics Mode"), this->value("flightDynamicsMode")});
  rows.append({QStringLiteral("Active Dynamics Backend"), this->value("activeDynamicsBackend")});
  rows.append({QStringLiteral("Dynamics Step Duration"), QStringLiteral("%1 ms").arg(formatNumber(summaryDoubleValue(_summary, "dynamicsStepDurationMilliseconds"), 3))});
  rows.append({QStringLiteral("Fallback Reason"), this->value("dynamicsFallbackReason")});
  rows.append({QStringLiteral("JSBSim Aircraft"), this->value("jsbsimAircraftModel")});
  rows.append({QStringLiteral("Control Profile"), this->value("controlProfileId")});
  rows.append({QStringLiteral("Systems Display Profile"), this->value("systemsDisplayProfileId")});
  rows.append({QStringLiteral("Engine Count"), QString::number(_summary.value(QStringLiteral("engineCount")).toInt())});
  rows.append({QStringLiteral("Behavior Mode"), this->value("behaviorMode", QStringLiteral("Manual"))});
  rows.append({QStringLiteral("Behavior Target"), this->value("behaviorTargetEntityName")});
  this->setTableRows(rows);
}

void EntityDetailsDialog::setTableRows(const QList<QPair<QString, QString>>& rows) {
  _detailsTable->setRowCount(rows.size());
  for (int row = 0; row < rows.size(); ++row) {
    auto* nameItem = new QTableWidgetItem(rows.at(row).first);
    auto* valueItem = new QTableWidgetItem(rows.at(row).second);
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
    _detailsTable->setItem(row, 0, nameItem);
    _detailsTable->setItem(row, 1, valueItem);
  }
  _detailsTable->resizeRowsToContents();
}

void EntityDetailsDialog::updateSection(QListWidgetItem* current, QListWidgetItem*) {
  if (!current) {
    return;
  }

  this->refreshCurrentSection(true);
}

void EntityDetailsDialog::refreshSummary() {
  if (!_summaryProvider) {
    return;
  }

  const QVariantMap updatedSummary = _summaryProvider();
  if (updatedSummary.isEmpty()) {
    _statusLabel->setText(QStringLiteral("Status: Unavailable"));
    _summaryProvider = {};
    _console->appendPlainText(QStringLiteral("Entity is no longer available"));
    return;
  }

  _summary = updatedSummary;
  this->setWindowTitle(this->value("name", QStringLiteral("Entity Details")));
  this->populateHeader();
  this->refreshCurrentSection(false);
}

void EntityDetailsDialog::refreshCurrentSection(bool logTransition) {
  const QListWidgetItem* current = _sectionsList->currentItem();
  if (!current) {
    return;
  }
  const QString sectionName = current->text();
  if (sectionName == QStringLiteral("State Data")) {
    this->populateStateData();
  } else if (sectionName == QStringLiteral("Tasks")) {
    this->populateTaskInformation();
  } else if (sectionName == QStringLiteral("Appearance")) {
    this->populateAppearanceInformation();
  } else if (sectionName == QStringLiteral("Resources")) {
    this->populateResourcesInformation();
  } else if (sectionName == QStringLiteral("Sensor Information")) {
    this->populateSensorInformation();
  } else if (sectionName == QStringLiteral("Emitters")) {
    this->populateEmitterInformation();
  } else if (sectionName == QStringLiteral("Subsystems")) {
    this->populateSubsystemInformation();
  }
  if (logTransition) {
    _console->appendPlainText(QStringLiteral("Switched to section: %1").arg(sectionName));
  }
}

QString EntityDetailsDialog::value(const char* key, const QString& fallback) const {
  const QString text = _summary.value(QString::fromLatin1(key)).toString().trimmed();
  return text.isEmpty() ? fallback : text;
}

QString EntityDetailsDialog::forceGlyph() const {
  const QString team = this->value("team").toLower();
  if (team.contains(QStringLiteral("opposing"))) {
    return QStringLiteral("O");
  }
  if (team.contains(QStringLiteral("neutral"))) {
    return QStringLiteral("N");
  }
  return QStringLiteral("F");
}

QString EntityDetailsDialog::locationText() const {
  const double altitudeMeters = summaryDoubleValue(_summary, "altitudeMeters");
  return QStringLiteral("%1, %2, %3 m / %4")
      .arg(formatNumber(summaryDoubleValue(_summary, "latitude"), 6))
      .arg(formatNumber(summaryDoubleValue(_summary, "longitude"), 6))
      .arg(formatNumber(altitudeMeters, 0))
      .arg(formatFeet(altitudeMeters));
}
