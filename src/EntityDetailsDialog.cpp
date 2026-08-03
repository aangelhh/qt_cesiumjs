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
#include <QVariantList>
#include <QVBoxLayout>

namespace {

const DisEntityCatalog& disEntityCatalog() {
  static const DisEntityCatalog catalog = DisEntityCatalog::loadDefault();
  return catalog;
}

QString formatFeetFromMetersString(const QString& altitudeText) {
  bool ok = false;
  const double meters = altitudeText.split(' ').value(0).toDouble(&ok);
  if (!ok) {
    return altitudeText;
  }
  const int feet = static_cast<int>(meters * 3.28084);
  return QStringLiteral("%1 ft").arg(feet);
}

QString synthesizeSpeedFromAltitude(const QString& altitudeText, int baseValue) {
  bool ok = false;
  const double meters = altitudeText.split(' ').value(0).toDouble(&ok);
  const int altitudeFactor = ok ? static_cast<int>(meters / 75.0) : 0;
  return QString::number(baseValue + altitudeFactor);
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

EntityDetailsDialog::EntityDetailsDialog(const QVariantMap& summary, QWidget* parent)
    : QDialog(parent),
      _summary(summary),
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
      QStringLiteral("Tasks"),
      QStringLiteral("State Data"),
      QStringLiteral("Appearance"),
      QStringLiteral("Resources"),
      QStringLiteral("Sensor Information"),
      QStringLiteral("Emitters"),
      QStringLiteral("Embarkation"),
      QStringLiteral("Subsystems")});
  _sectionsList->setCurrentRow(1);
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

  const QString altitude = this->value("altitude");
  _groundSpeedLabel->setText(
      QStringLiteral("Ground Speed (kts): %1")
          .arg(synthesizeSpeedFromAltitude(altitude, 420)));
  _altitudeLabel->setText(
      QStringLiteral("Altitude MSL (ft): %1")
          .arg(formatFeetFromMetersString(altitude)));
  _tasLabel->setText(
      QStringLiteral("TAS (kts): %1")
          .arg(synthesizeSpeedFromAltitude(altitude, 460)));
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

  QList<QPair<QString, QString>> rows;
  rows.append({QStringLiteral("Location (Lat, Lon, Alt)"), this->locationText()});
  rows.append({QStringLiteral("Ground Speed (kts)"), synthesizeSpeedFromAltitude(this->value("altitude"), 420)});
  rows.append({QStringLiteral("True Airspeed (kts)"), synthesizeSpeedFromAltitude(this->value("altitude"), 460)});
  rows.append({QStringLiteral("Mach"), QStringLiteral("0.82")});
  rows.append({QStringLiteral("Altitude MSL (ft)"), formatFeetFromMetersString(this->value("altitude"))});
  rows.append({QStringLiteral("Velocity (kts)"), QStringLiteral("-10 -158 561")});
  rows.append({QStringLiteral("Heading, Pitch, Roll (deg)"), QStringLiteral("16, 27, 68")});
  rows.append({QStringLiteral("Bounding Volume L/W/H (m)"), QStringLiteral("17, 12, 5")});
  rows.append({QStringLiteral("Entity Id"), this->value("name")});
  rows.append({QStringLiteral("Force"), this->value("team")});
  rows.append({QStringLiteral("Activity"), this->value("status")});
  rows.append({QStringLiteral("Callsign"), this->value("name")});
  rows.append({QStringLiteral("Domain"), this->value("domain")});
  rows.append({QStringLiteral("Category"), this->value("category")});
  rows.append({QStringLiteral("Model"), this->value("modelName")});
  rows.append({QStringLiteral("Heading"), QStringLiteral("%1 deg").arg(this->value("headingDegrees"))});
  rows.append({QStringLiteral("Flight Dynamics"), this->value("flightDynamicsEnabled", QStringLiteral("false")) == QStringLiteral("true") ? QStringLiteral("Enabled") : QStringLiteral("Disabled")});
  rows.append({QStringLiteral("Dynamics Mode"), this->value("flightDynamicsMode", QStringLiteral("-"))});
  rows.append({QStringLiteral("JSBSim Aircraft"), this->value("jsbsimAircraftModel", QStringLiteral("-"))});
  rows.append({QStringLiteral("Control Profile"), this->value("controlProfileId", QStringLiteral("-"))});
  rows.append({QStringLiteral("Fuel Remaining"), QStringLiteral("%1 / %2 kg")
                                                   .arg(this->value("fuelRemainingKilograms", QStringLiteral("0")))
                                                   .arg(this->value("fuelCapacityKilograms", QStringLiteral("0")))});
  rows.append({QStringLiteral("Speed"), QStringLiteral("%1 kts").arg(this->value("speedKnots", QStringLiteral("0")))});
  rows.append({QStringLiteral("Vertical Speed"), QStringLiteral("%1 m/s").arg(this->value("verticalSpeedMetersPerSecond", QStringLiteral("0")))});
  rows.append({QStringLiteral("Current Task"), this->value("taskType", QStringLiteral("-"))});
  rows.append({QStringLiteral("Task Status"), this->value("taskStatus", QStringLiteral("-"))});
  rows.append({QStringLiteral("Task Target Heading"), QStringLiteral("%1 deg").arg(this->value("taskTargetHeadingDegrees", QStringLiteral("0")))});
  rows.append({QStringLiteral("Task Target Altitude"), QStringLiteral("%1 m").arg(this->value("taskTargetAltitudeMeters", QStringLiteral("0")))});
  rows.append({QStringLiteral("Task Target Speed"), QStringLiteral("%1 kts").arg(this->value("taskTargetSpeedKnots", QStringLiteral("0")))});
  rows.append({QStringLiteral("Task Target Location"), QStringLiteral("%1, %2")
                                                    .arg(this->value("taskTargetLatitude", QStringLiteral("0")))
                                                    .arg(this->value("taskTargetLongitude", QStringLiteral("0")))});
  rows.append({QStringLiteral("Task Target Entity"), this->value("taskTargetEntityName", QStringLiteral("-"))});
  rows.append({QStringLiteral("Task Target Waypoint"), this->value("taskTargetWaypointName", QStringLiteral("-"))});
  rows.append({QStringLiteral("Task Target Route"), this->value("taskTargetRouteName", QStringLiteral("-"))});
  rows.append({QStringLiteral("Task Target Area"), this->value("taskTargetAreaName", QStringLiteral("-"))});
  rows.append({QStringLiteral("Sensors"), this->value("sensorCount", QStringLiteral("0"))});
  rows.append({QStringLiteral("Sensor Contacts"), this->value("contactCount", QStringLiteral("0"))});
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
  rows.append({QStringLiteral("Invisible"), QStringLiteral("No")});
  rows.append({QStringLiteral("Invulnerable"), QStringLiteral("No")});
  this->setTableRows(rows);
}

void EntityDetailsDialog::populateSensorInformation() {
  QList<QPair<QString, QString>> rows;
  const QVariantList sensors = _summary.value(QStringLiteral("sensors")).toList();
  const QVariantList contacts = _summary.value(QStringLiteral("sensorContacts")).toList();

  rows.append({QStringLiteral("Configured Sensors"), QString::number(sensors.size())});
  if (sensors.isEmpty()) {
    rows.append({QStringLiteral("Sensor"), QStringLiteral("No sensors configured")});
  } else {
    for (int index = 0; index < sensors.size(); ++index) {
      const QVariantMap sensor = sensors.at(index).toMap();
      const QString prefix = QStringLiteral("Sensor %1").arg(index + 1);
      rows.append({prefix + QStringLiteral(" Name"), sensor.value(QStringLiteral("name")).toString()});
      rows.append({prefix + QStringLiteral(" Type"), sensor.value(QStringLiteral("sensorType")).toString()});
      rows.append({prefix + QStringLiteral(" Subtype"), sensor.value(QStringLiteral("sensorSubType")).toString()});
      rows.append({prefix + QStringLiteral(" Enabled"), sensor.value(QStringLiteral("enabled")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" Emitting"), sensor.value(QStringLiteral("emitting")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" Range"), QStringLiteral("%1 km").arg(sensor.value(QStringLiteral("maxRangeMeters")).toDouble() / 1000.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Azimuth"), QStringLiteral("%1 deg").arg(sensor.value(QStringLiteral("azimuthWidthDegrees")).toDouble(), 0, 'f', 1)});
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
      rows.append({prefix + QStringLiteral(" Sensor"), contact.value(QStringLiteral("sensorId")).toString()});
      rows.append({prefix + QStringLiteral(" Range"), QStringLiteral("%1 km").arg(contact.value(QStringLiteral("rangeMeters")).toDouble() / 1000.0, 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" Bearing"), QStringLiteral("%1 deg").arg(contact.value(QStringLiteral("bearingDegrees")).toDouble(), 0, 'f', 1)});
      rows.append({prefix + QStringLiteral(" LOS"), contact.value(QStringLiteral("lineOfSight")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
      rows.append({prefix + QStringLiteral(" Detected"), contact.value(QStringLiteral("detected")).toBool() ? QStringLiteral("Yes") : QStringLiteral("No")});
    }
  }

  this->setTableRows(rows);
}

void EntityDetailsDialog::populatePlaceholderSection(const QString& sectionName) {
  QList<QPair<QString, QString>> rows;
  rows.append({QStringLiteral("Section"), sectionName});
  rows.append({QStringLiteral("Entity"), this->value("name")});
  rows.append({QStringLiteral("Status"), QStringLiteral("Not implemented yet")});
  rows.append({QStringLiteral("Notes"), QStringLiteral("This panel is reserved for the next iteration.")});
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

  const QString sectionName = current->text();
  if (sectionName == QStringLiteral("State Data")) {
    this->populateStateData();
  } else if (sectionName == QStringLiteral("Sensor Information")) {
    this->populateSensorInformation();
  } else {
    this->populatePlaceholderSection(sectionName);
  }
  _console->appendPlainText(QStringLiteral("Switched to section: %1").arg(sectionName));
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
  return QStringLiteral("%1, %2, %3")
      .arg(this->value("latitude"))
      .arg(this->value("longitude"))
      .arg(formatFeetFromMetersString(this->value("altitude")));
}
