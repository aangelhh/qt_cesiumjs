#include "presentation/DisConnectionPanel.h"
#include <QComboBox>
#include <QCheckBox>
#include <QScrollArea>
#include <array>

#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace presentation {
namespace {

QString stateText(DisConnectionState state) {
  switch (state) {
    case DisConnectionState::Unavailable: return QStringLiteral("Unavailable");
    case DisConnectionState::Disconnected: return QStringLiteral("Disconnected");
    case DisConnectionState::Connecting: return QStringLiteral("Connecting");
    case DisConnectionState::Active: return QStringLiteral("Active");
    case DisConnectionState::Error: return QStringLiteral("Connection error");
  }
  return QStringLiteral("Disconnected");
}

QString stateColor(DisConnectionState state) {
  switch (state) {
    case DisConnectionState::Unavailable: return QStringLiteral("#59636b");
    case DisConnectionState::Disconnected: return QStringLiteral("#7a8791");
    case DisConnectionState::Connecting: return QStringLiteral("#e5a93d");
    case DisConnectionState::Active: return QStringLiteral("#32b76c");
    case DisConnectionState::Error: return QStringLiteral("#d9534f");
  }
  return QStringLiteral("#7a8791");
}

QDoubleSpinBox* makeDoubleSpin(
    QWidget* parent, double minimum, double maximum, int decimals) {
  auto* spin = new QDoubleSpinBox(parent);
  spin->setRange(minimum, maximum);
  spin->setDecimals(decimals);
  spin->setKeyboardTracking(false);
  return spin;
}

} // namespace

DisConnectionPanel::DisConnectionPanel(QWidget* parent)
    : QWidget(parent),
      _stateIndicator(new QLabel(this)),
      _stateLabel(new QLabel(this)),
      _detailLabel(new QLabel(this)),
      _txValue(new QLabel(QStringLiteral("0"), this)),
      _rxValue(new QLabel(QStringLiteral("0"), this)),
      _remoteValue(new QLabel(QStringLiteral("0"), this)),
      _connectionControls(new QWidget(this)),
      _testControls(new QWidget(this)),
      _addressEdit(new QLineEdit(_connectionControls)),
      _portSpin(new QSpinBox(_connectionControls)),
      _exerciseSpin(new QSpinBox(_connectionControls)),
      _siteSpin(new QSpinBox(_connectionControls)),
      _applicationSpin(new QSpinBox(_connectionControls)),
      _connectButton(new QPushButton(QStringLiteral("Connect"), _connectionControls)),
      _disconnectButton(new QPushButton(QStringLiteral("Disconnect"), _connectionControls)),
      _entitySpin(new QSpinBox(_testControls)),
      _markingEdit(new QLineEdit(_testControls)),
      _latitudeSpin(makeDoubleSpin(_testControls, -90.0, 90.0, 6)),
      _longitudeSpin(makeDoubleSpin(_testControls, -180.0, 180.0, 6)),
      _altitudeSpin(makeDoubleSpin(_testControls, -1000.0, 100000.0, 1)),
      _headingSpin(makeDoubleSpin(_testControls, 0.0, 359.99, 2)),
      _pitchSpin(makeDoubleSpin(_testControls, -90.0, 90.0, 2)),
      _rollSpin(makeDoubleSpin(_testControls, -180.0, 180.0, 2)),
      _speedSpin(makeDoubleSpin(_testControls, 0.0, 5000.0, 1)),
      _sendButton(new QPushButton(QStringLiteral("Send Entity State PDU"), _testControls)),
      _receiveTable(new QTableWidget(0, 7, this)) {
  setObjectName(QStringLiteral("disConnectionPanel"));
  setMinimumWidth(560);

  _stateIndicator->setFixedSize(12, 12);
  _stateIndicator->setAccessibleName(QStringLiteral("DIS connection state"));
  _stateLabel->setObjectName(QStringLiteral("disStateLabel"));
  _detailLabel->setObjectName(QStringLiteral("disConnectionDetail"));
  _detailLabel->setWordWrap(true);
  _detailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  auto* stateLayout = new QHBoxLayout;
  stateLayout->addWidget(_stateIndicator);
  stateLayout->addWidget(_stateLabel);
  stateLayout->addStretch(1);
  stateLayout->addWidget(new QLabel(QStringLiteral("TX"), this));
  stateLayout->addWidget(_txValue);
  stateLayout->addSpacing(10);
  stateLayout->addWidget(new QLabel(QStringLiteral("RX"), this));
  stateLayout->addWidget(_rxValue);
  stateLayout->addSpacing(10);
  stateLayout->addWidget(new QLabel(QStringLiteral("Remote"), this));
  stateLayout->addWidget(_remoteValue);

  _addressEdit->setObjectName(QStringLiteral("disAddressEdit"));
  _portSpin->setObjectName(QStringLiteral("disPortSpin"));
  _exerciseSpin->setObjectName(QStringLiteral("disExerciseSpin"));
  _siteSpin->setObjectName(QStringLiteral("disSiteSpin"));
  _applicationSpin->setObjectName(QStringLiteral("disApplicationSpin"));
  _connectButton->setObjectName(QStringLiteral("disConnectButton"));
  _disconnectButton->setObjectName(QStringLiteral("disDisconnectButton"));
  _portSpin->setRange(1, 65535);
  _exerciseSpin->setRange(1, 255);
  _siteSpin->setRange(1, 65535);
  _applicationSpin->setRange(1, 65535);

  auto* connectionForm = new QFormLayout(_connectionControls);
  connectionForm->addRow(QStringLiteral("Address"), _addressEdit);
  connectionForm->addRow(QStringLiteral("UDP port"), _portSpin);
  connectionForm->addRow(QStringLiteral("Exercise"), _exerciseSpin);
  connectionForm->addRow(QStringLiteral("Site"), _siteSpin);
  connectionForm->addRow(QStringLiteral("Application"), _applicationSpin);
  auto* connectionButtons = new QHBoxLayout;
  connectionButtons->addStretch(1);
  connectionButtons->addWidget(_disconnectButton);
  connectionButtons->addWidget(_connectButton);
  connectionForm->addRow(connectionButtons);

  _entitySpin->setObjectName(QStringLiteral("disTestEntitySpin"));
  _markingEdit->setObjectName(QStringLiteral("disTestMarkingEdit"));
  _sendButton->setObjectName(QStringLiteral("disSendTestPduButton"));
  _entitySpin->setRange(1, 65535);
  _entitySpin->setValue(65000);
  _markingEdit->setText(QStringLiteral("DIS-TEST"));
  _latitudeSpin->setValue(40.4168);
  _longitudeSpin->setValue(-3.7038);
  _altitudeSpin->setValue(3000.0);
  _headingSpin->setValue(90.0);
  _speedSpin->setValue(250.0);

  auto* testForm = new QFormLayout(_testControls);
  testForm->addRow(QStringLiteral("Entity number"), _entitySpin);
  testForm->addRow(QStringLiteral("Marking"), _markingEdit);
  testForm->addRow(QStringLiteral("Latitude"), _latitudeSpin);
  testForm->addRow(QStringLiteral("Longitude"), _longitudeSpin);
  testForm->addRow(QStringLiteral("Altitude (m)"), _altitudeSpin);
  testForm->addRow(QStringLiteral("Heading (deg)"), _headingSpin);
  testForm->addRow(QStringLiteral("Pitch (deg)"), _pitchSpin);
  testForm->addRow(QStringLiteral("Roll (deg)"), _rollSpin);
  testForm->addRow(QStringLiteral("Speed (kt)"), _speedSpin);
  testForm->addRow(_sendButton);

  auto* pduType = new QComboBox(_testControls);
  pduType->setObjectName(QStringLiteral("disTestPduType"));
  pduType->addItems({QStringLiteral("Entity State"), QStringLiteral("Create Entity"),
      QStringLiteral("Remove Entity"), QStringLiteral("IFF"), QStringLiteral("Collision")});
  testForm->insertRow(0, QStringLiteral("PDU"), pduType);
  auto* target = new QWidget(_testControls);
  auto* targetForm = new QFormLayout(target);
  auto* targetSite = new QSpinBox(target);
  auto* targetApplication = new QSpinBox(target);
  auto* targetEntity = new QSpinBox(target);
  targetSite->setObjectName(QStringLiteral("disTargetSite"));
  targetApplication->setObjectName(QStringLiteral("disTargetApplication"));
  targetEntity->setObjectName(QStringLiteral("disTargetEntity"));
  for (auto* spin : {targetSite, targetApplication, targetEntity}) { spin->setRange(1, 65534); spin->setValue(1); }
  targetApplication->setValue(2);
  targetEntity->setValue(100);
  targetForm->addRow(QStringLiteral("Target site"), targetSite);
  targetForm->addRow(QStringLiteral("Target application"), targetApplication);
  targetForm->addRow(QStringLiteral("Target entity"), targetEntity);
  testForm->insertRow(testForm->rowCount() - 1, target);
  auto* iffOn = new QCheckBox(QStringLiteral("IFF system on"), _testControls);
  iffOn->setObjectName(QStringLiteral("disIffOn"));
  iffOn->setChecked(true);
  testForm->insertRow(testForm->rowCount() - 1, iffOn);
  auto* collisionFields = new QWidget(_testControls);
  auto* collisionForm = new QFormLayout(collisionFields);
  std::array<QDoubleSpinBox*, 7> collisionValues;
  const QStringList labels{QStringLiteral("Mass (kg)"), QStringLiteral("Velocity ECEF X (m/s)"),
      QStringLiteral("Velocity ECEF Y (m/s)"), QStringLiteral("Velocity ECEF Z (m/s)"),
      QStringLiteral("Contact local X (m)"), QStringLiteral("Contact local Y (m)"), QStringLiteral("Contact local Z (m)")};
  for (int i = 0; i < 7; ++i) {
    collisionValues[i] = makeDoubleSpin(collisionFields, i == 0 ? 0 : -1e6, 1e9, 2);
    collisionForm->addRow(labels[i], collisionValues[i]);
  }
  testForm->insertRow(testForm->rowCount() - 1, collisionFields);
  const auto updateType = [=](int type) {
    for (QWidget* field : std::array<QWidget*, 8>{_markingEdit, _latitudeSpin, _longitudeSpin,
             _altitudeSpin, _headingSpin, _pitchSpin, _rollSpin, _speedSpin}) testForm->setRowVisible(field, type == 0);
    testForm->setRowVisible(_entitySpin, type == 0 || type >= 3);
    testForm->setRowVisible(target, type == 1 || type == 2 || type == 4);
    testForm->setRowVisible(iffOn, type == 3);
    testForm->setRowVisible(collisionFields, type == 4);
    _sendButton->setText(QStringLiteral("Send %1 PDU").arg(pduType->currentText()));
  };
  connect(pduType, &QComboBox::currentIndexChanged, this, updateType);
  updateType(0);

  auto* connectionGroup = new QGroupBox(QStringLiteral("UDP session"), this);
  auto* connectionGroupLayout = new QVBoxLayout(connectionGroup);
  connectionGroupLayout->addWidget(_connectionControls);
  auto* testGroup = new QGroupBox(QStringLiteral("PDU tester"), this);
  auto* testGroupLayout = new QVBoxLayout(testGroup);
  testGroupLayout->addWidget(_testControls);

  _receiveTable->setObjectName(QStringLiteral("disReceiveTable"));
  _receiveTable->setHorizontalHeaderLabels({
      QStringLiteral("Time"), QStringLiteral("Event"), QStringLiteral("Entity ID"),
      QStringLiteral("Marking"), QStringLiteral("Latitude"),
      QStringLiteral("Longitude"), QStringLiteral("Altitude m")});
  _receiveTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _receiveTable->horizontalHeader()->setStretchLastSection(true);
  _receiveTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  _receiveTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  _receiveTable->setMinimumHeight(160);

  auto* layout = new QVBoxLayout(this);
  layout->addLayout(stateLayout);
  layout->addWidget(_detailLabel);
  auto* controls = new QWidget(this);
  auto* controlsLayout = new QVBoxLayout(controls);
  controlsLayout->addWidget(connectionGroup);
  controlsLayout->addWidget(testGroup);
  auto* scroll = new QScrollArea(this);
  scroll->setWidgetResizable(true);
  scroll->setWidget(controls);
  scroll->setMinimumHeight(220);
  layout->addWidget(scroll, 2);
  layout->addWidget(new QLabel(QStringLiteral("Received DIS PDUs"), this));
  layout->addWidget(_receiveTable, 1);

  connect(_connectButton, &QPushButton::clicked,
          this, &DisConnectionPanel::requestConnection);
  connect(_disconnectButton, &QPushButton::clicked,
          this, &DisConnectionPanel::disconnectRequested);
  connect(_sendButton, &QPushButton::clicked, this, [=]() {
    if (_state != DisConnectionState::Active || _blockedByHla) return;
    const int type = pduType->currentIndex();
    const tactical::dis::EntityIdentifier destination{static_cast<std::uint16_t>(targetSite->value()),
        static_cast<std::uint16_t>(targetApplication->value()), static_cast<std::uint16_t>(targetEntity->value())};
    if (type == 0) { requestTestTransmission(); return; }
    if (type == 1 || type == 2) {
      tactical::dis::EntityManagementRequest request;
      request.remove = type == 2; request.destination = destination;
      emit entityManagementRequested(request);
    } else if (type == 3) {
      tactical::dis::IffState state;
      state.host.entity = static_cast<std::uint16_t>(_entitySpin->value());
      state.on = iffOn->isChecked(); state.operational = true;
      emit iffRequested(state);
    } else {
      tactical::dis::CollisionEvent event;
      event.source.entity = static_cast<std::uint16_t>(_entitySpin->value()); event.target = destination;
      event.massKilograms = collisionValues[0]->value();
      event.velocityX = collisionValues[1]->value(); event.velocityY = collisionValues[2]->value(); event.velocityZ = collisionValues[3]->value();
      event.locationX = collisionValues[4]->value(); event.locationY = collisionValues[5]->value(); event.locationZ = collisionValues[6]->value();
      emit collisionRequested(event);
    }
  });

  setConnectionState(DisConnectionState::Disconnected);
}

void DisConnectionPanel::recordInteractions(const std::vector<tactical::dis::IffState>& iff,
    const std::vector<tactical::dis::CollisionEvent>& collisions) {
  const auto append = [this](const QString& type, const QString& entity, const QString& detail) {
    _receiveTable->insertRow(0);
    const QStringList cells{QTime::currentTime().toString(), type, entity, detail, QString(), QString(), QString()};
    for (int i = 0; i < cells.size(); ++i) _receiveTable->setItem(0, i, new QTableWidgetItem(cells[i]));
    if (_receiveTable->rowCount() > 200) _receiveTable->removeRow(200);
  };
  for (const auto& state : iff) append(QStringLiteral("IFF"), state.host.key(),
      QStringLiteral("System %1: %2 / %3").arg(state.systemId).arg(state.on ? QStringLiteral("On") : QStringLiteral("Off"),
          state.operational ? QStringLiteral("Operational") : QStringLiteral("Unavailable")));
  for (const auto& event : collisions) append(QStringLiteral("Collision"), event.source.key(),
      QStringLiteral("Target %1; event %2; mass %3 kg").arg(event.target.key(), event.event.key()).arg(event.massKilograms));
}

void DisConnectionPanel::configure(
    const application::DisStartupConfiguration& configuration,
    bool backendAvailable,
    bool blockedByHla) {
  _backendAvailable = backendAvailable;
  _blockedByHla = blockedByHla;
  _addressEdit->setText(configuration.address);
  _portSpin->setValue(configuration.port);
  _exerciseSpin->setValue(configuration.exerciseId);
  _siteSpin->setValue(configuration.siteId);
  _applicationSpin->setValue(configuration.applicationId);
  updateControls();
}

void DisConnectionPanel::setConnectionState(
    DisConnectionState state, const QString& detail) {
  _state = state;
  _stateLabel->setText(stateText(state));
  _stateIndicator->setStyleSheet(
      QStringLiteral("background-color: %1; border-radius: 6px;")
          .arg(stateColor(state)));
  QString effectiveDetail = detail.trimmed();
  if (_blockedByHla) {
    effectiveDetail = QStringLiteral(
        "DIS is unavailable while the HLA federation is active.");
  } else if (effectiveDetail.isEmpty() && !_backendAvailable) {
    effectiveDetail = QStringLiteral("DIS support is not available in this build.");
  }
  _detailLabel->setText(effectiveDetail);
  _detailLabel->setVisible(!effectiveDetail.isEmpty());
  updateControls();
}

void DisConnectionPanel::setBlockedByHla(bool blocked) {
  _blockedByHla = blocked;
  setConnectionState(
      blocked ? DisConnectionState::Unavailable : _state,
      blocked ? QStringLiteral("DIS is unavailable while the HLA federation is active.")
              : QString());
  if (!blocked && _state == DisConnectionState::Unavailable) {
    setConnectionState(DisConnectionState::Disconnected);
  }
}

void DisConnectionPanel::updateStatistics(
    std::uint64_t transmittedPdus,
    std::uint64_t receivedPdus,
    qsizetype remoteEntities) {
  _txValue->setText(QString::number(transmittedPdus));
  _rxValue->setText(QString::number(receivedPdus));
  _remoteValue->setText(QString::number(remoteEntities));
}

void DisConnectionPanel::recordRemoteEntityChanges(
    const std::vector<tactical::dis::RemoteEntityChange>& changes) {
  for (const auto& change : changes) {
    const int row = 0;
    _receiveTable->insertRow(row);
    const auto& state = change.state;
    const QStringList values = {
        QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz")),
        change.removed ? QStringLiteral("Expired") : QStringLiteral("Entity State"),
        QStringLiteral("%1:%2:%3")
            .arg(state.identifier.site)
            .arg(state.identifier.application)
            .arg(state.identifier.entity),
        state.marking,
        QString::number(state.latitudeDegrees, 'f', 6),
        QString::number(state.longitudeDegrees, 'f', 6),
        QString::number(state.altitudeMeters, 'f', 1)};
    for (int column = 0; column < values.size(); ++column) {
      _receiveTable->setItem(row, column, new QTableWidgetItem(values[column]));
    }
  }
  while (_receiveTable->rowCount() > 200) {
    _receiveTable->removeRow(_receiveTable->rowCount() - 1);
  }
}

void DisConnectionPanel::recordRemoteWarfareEvents(
    const std::vector<tactical::dis::WarfareEvent>& events) {
  for (const tactical::dis::WarfareEvent& event : events) {
    _receiveTable->insertRow(0);
    const QStringList values = {
        QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz")),
        event.kind == tactical::dis::WarfareEventKind::Fire
            ? QStringLiteral("Fire") : QStringLiteral("Detonation"),
        event.eventIdentifier.key(),
        event.munitionType,
        QString::number(event.latitudeDegrees, 'f', 6),
        QString::number(event.longitudeDegrees, 'f', 6),
        QString::number(event.altitudeMeters, 'f', 1)};
    for (int column = 0; column < values.size(); ++column) {
      _receiveTable->setItem(
          0, column, new QTableWidgetItem(values[column]));
    }
  }
  while (_receiveTable->rowCount() > 200) {
    _receiveTable->removeRow(_receiveTable->rowCount() - 1);
  }
}

DisConnectionState DisConnectionPanel::connectionState() const {
  return _state;
}

application::DisStartupConfiguration
DisConnectionPanel::connectionConfiguration() const {
  application::DisStartupConfiguration configuration;
  configuration.address = _addressEdit->text().trimmed();
  configuration.port = _portSpin->value();
  configuration.exerciseId = _exerciseSpin->value();
  configuration.siteId = _siteSpin->value();
  configuration.applicationId = _applicationSpin->value();
  return configuration;
}

tactical::dis::EntityState DisConnectionPanel::testEntityState() const {
  tactical::dis::EntityState state;
  state.identifier.entity = static_cast<std::uint16_t>(_entitySpin->value());
  state.marking = _markingEdit->text().trimmed();
  state.forceIdentifier = 1;
  state.entityKind = 1;
  state.entityDomain = 2;
  state.latitudeDegrees = _latitudeSpin->value();
  state.longitudeDegrees = _longitudeSpin->value();
  state.altitudeMeters = _altitudeSpin->value();
  state.headingDegrees = _headingSpin->value();
  state.pitchDegrees = _pitchSpin->value();
  state.rollDegrees = _rollSpin->value();
  state.speedKnots = _speedSpin->value();
  return state;
}

void DisConnectionPanel::requestConnection() {
  if (_addressEdit->text().trimmed().isEmpty()) {
    _detailLabel->setText(QStringLiteral("DIS destination address is required."));
    _detailLabel->setVisible(true);
    _addressEdit->setFocus();
    return;
  }
  emit connectRequested(connectionConfiguration());
}

void DisConnectionPanel::requestTestTransmission() {
  if (_markingEdit->text().trimmed().isEmpty()) {
    _detailLabel->setText(QStringLiteral("Test entity marking is required."));
    _detailLabel->setVisible(true);
    _markingEdit->setFocus();
    return;
  }
  emit testEntityStateRequested(testEntityState());
}

void DisConnectionPanel::updateControls() {
  const bool blocked = _blockedByHla || !_backendAvailable;
  const bool busy = _state == DisConnectionState::Connecting;
  const bool connected = _state == DisConnectionState::Active;
  _connectionControls->setEnabled(!blocked);
  _addressEdit->setEnabled(!busy && !connected);
  _portSpin->setEnabled(!busy && !connected);
  _exerciseSpin->setEnabled(!busy && !connected);
  _siteSpin->setEnabled(!busy && !connected);
  _applicationSpin->setEnabled(!busy && !connected);
  _connectButton->setEnabled(!blocked && !busy && !connected);
  _disconnectButton->setEnabled(!blocked && connected);
  _testControls->setEnabled(!blocked && connected);
  _receiveTable->setEnabled(!blocked);
}

} // namespace presentation
