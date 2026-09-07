#include "presentation/HlaConnectionPanel.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace presentation {

namespace {

QString stateText(HlaConnectionState state) {
  switch (state) {
    case HlaConnectionState::Disconnected:
      return QStringLiteral("Disconnected");
    case HlaConnectionState::Connecting:
      return QStringLiteral("Connecting");
    case HlaConnectionState::Federated:
      return QStringLiteral("Federated");
    case HlaConnectionState::Error:
      return QStringLiteral("Connection error");
  }
  return QStringLiteral("Disconnected");
}

QString stateColor(HlaConnectionState state) {
  switch (state) {
    case HlaConnectionState::Disconnected:
      return QStringLiteral("#7a8791");
    case HlaConnectionState::Connecting:
      return QStringLiteral("#e5a93d");
    case HlaConnectionState::Federated:
      return QStringLiteral("#32b76c");
    case HlaConnectionState::Error:
      return QStringLiteral("#d9534f");
  }
  return QStringLiteral("#7a8791");
}

} // namespace

HlaConnectionPanel::HlaConnectionPanel(QWidget* parent)
    : QWidget(parent),
      _stateIndicator(new QLabel(this)),
      _stateLabel(new QLabel(this)),
      _detailLabel(new QLabel(this)),
      _backendValue(new QLabel(this)),
      _endpointValue(new QLabel(this)),
      _federateTypeValue(new QLabel(this)),
      _fomModulesValue(new QLabel(this)),
      _timeManagementValue(new QLabel(this)),
      _synchronizationPointValue(new QLabel(this)),
      _federationEdit(new QLineEdit(this)),
      _federateEdit(new QLineEdit(this)),
      _connectButton(new QPushButton(QStringLiteral("Connect"), this)),
      _disconnectButton(new QPushButton(QStringLiteral("Disconnect"), this)) {
  this->setObjectName(QStringLiteral("hlaConnectionPanel"));
  this->setMinimumWidth(420);

  _stateIndicator->setObjectName(QStringLiteral("hlaStateIndicator"));
  _stateIndicator->setFixedSize(12, 12);
  _stateIndicator->setAccessibleName(QStringLiteral("HLA connection state"));
  _stateLabel->setObjectName(QStringLiteral("hlaStateLabel"));
  _detailLabel->setObjectName(QStringLiteral("hlaConnectionDetail"));
  _detailLabel->setWordWrap(true);
  _detailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  _backendValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
  _endpointValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
  _federateTypeValue->setObjectName(QStringLiteral("hlaFederateTypeValue"));
  _fomModulesValue->setObjectName(QStringLiteral("hlaFomModulesValue"));
  _timeManagementValue->setObjectName(QStringLiteral("hlaTimeManagementValue"));
  _synchronizationPointValue->setObjectName(
      QStringLiteral("hlaSynchronizationPointValue"));
  _federateTypeValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
  _fomModulesValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
  _timeManagementValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
  _synchronizationPointValue->setTextInteractionFlags(
      Qt::TextSelectableByMouse);
  _federationEdit->setObjectName(QStringLiteral("hlaFederationNameEdit"));
  _federateEdit->setObjectName(QStringLiteral("hlaFederateNameEdit"));
  _connectButton->setObjectName(QStringLiteral("hlaConnectButton"));
  _disconnectButton->setObjectName(QStringLiteral("hlaDisconnectButton"));

  auto* stateLayout = new QHBoxLayout;
  stateLayout->addWidget(_stateIndicator);
  stateLayout->addWidget(_stateLabel);
  stateLayout->addStretch(1);

  auto* form = new QFormLayout;
  form->addRow(QStringLiteral("Backend"), _backendValue);
  form->addRow(QStringLiteral("RTI endpoint"), _endpointValue);
  form->addRow(QStringLiteral("Federate type"), _federateTypeValue);
  form->addRow(QStringLiteral("FOM modules"), _fomModulesValue);
  form->addRow(QStringLiteral("Time management"), _timeManagementValue);
  form->addRow(QStringLiteral("Synchronization point"),
               _synchronizationPointValue);
  form->addRow(QStringLiteral("Federation"), _federationEdit);
  form->addRow(QStringLiteral("Federate name"), _federateEdit);

  auto* buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(_disconnectButton);
  buttonLayout->addWidget(_connectButton);

  auto* layout = new QVBoxLayout(this);
  layout->addLayout(stateLayout);
  layout->addWidget(_detailLabel);
  layout->addLayout(form);
  layout->addStretch(1);
  layout->addLayout(buttonLayout);

  connect(
      _connectButton,
      &QPushButton::clicked,
      this,
      &HlaConnectionPanel::requestConnection);
  connect(
      _disconnectButton,
      &QPushButton::clicked,
      this,
      &HlaConnectionPanel::disconnectRequested);
  connect(
      _federationEdit,
      &QLineEdit::textChanged,
      this,
      &HlaConnectionPanel::updateControls);
  connect(
      _federateEdit,
      &QLineEdit::textChanged,
      this,
      &HlaConnectionPanel::updateControls);

  this->setConnectionState(HlaConnectionState::Disconnected);
}

void HlaConnectionPanel::configure(
    const application::HlaStartupConfiguration& configuration,
    bool backendAvailable) {
  _backendAvailable = backendAvailable;
  _backendValue->setText(
      configuration.backendId.trimmed().isEmpty()
          ? QStringLiteral("Not configured")
          : configuration.backendId);
  _endpointValue->setText(
      configuration.localSettingsDesignator.trimmed().isEmpty()
          ? QStringLiteral("Default RTI settings")
          : configuration.localSettingsDesignator);
  _federateTypeValue->setText(
      configuration.federateType.trimmed().isEmpty()
          ? QStringLiteral("Not configured")
          : configuration.federateType);
  _fomModulesValue->setText(QString::number(configuration.fomModules.size()));
  _timeManagementValue->setText(
      configuration.timeManagementEnabled
          ? QStringLiteral("Enabled, lookahead %1 s")
                .arg(configuration.timeLookaheadSeconds, 0, 'g', 6)
          : QStringLiteral("Disabled"));
  _synchronizationPointValue->setText(
      configuration.synchronizationPointLabel.trimmed().isEmpty()
          ? QStringLiteral("None")
          : configuration.synchronizationPointLabel);
  _federationEdit->setText(configuration.federationName);
  _federateEdit->setText(configuration.federateName);
  this->updateControls();
}

void HlaConnectionPanel::setConnectionState(
    HlaConnectionState state,
    const QString& detail) {
  _state = state;
  _stateLabel->setText(stateText(state));
  _stateIndicator->setStyleSheet(
      QStringLiteral("background-color: %1; border-radius: 6px;")
          .arg(stateColor(state)));
  QString effectiveDetail = detail.trimmed();
  if (effectiveDetail.isEmpty() && !_backendAvailable) {
    effectiveDetail = QStringLiteral(
        "No HLA backend is available in this build configuration.");
  }
  _detailLabel->setText(effectiveDetail);
  _detailLabel->setVisible(!effectiveDetail.isEmpty());
  this->updateControls();
}

HlaConnectionState HlaConnectionPanel::connectionState() const {
  return _state;
}

QString HlaConnectionPanel::federationName() const {
  return _federationEdit->text().trimmed();
}

QString HlaConnectionPanel::federateName() const {
  return _federateEdit->text().trimmed();
}

void HlaConnectionPanel::requestConnection() {
  if (this->federationName().isEmpty()) {
    _detailLabel->setText(QStringLiteral("Federation name is required."));
    _detailLabel->setVisible(true);
    _federationEdit->setFocus();
    return;
  }
  if (this->federateName().isEmpty()) {
    _detailLabel->setText(QStringLiteral("Federate name is required."));
    _detailLabel->setVisible(true);
    _federateEdit->setFocus();
    return;
  }
  emit connectRequested(this->federationName(), this->federateName());
}

void HlaConnectionPanel::updateControls() {
  const bool busy = _state == HlaConnectionState::Connecting;
  const bool connected = _state == HlaConnectionState::Federated;
  _federationEdit->setEnabled(!busy && !connected);
  _federateEdit->setEnabled(!busy && !connected);
  _connectButton->setEnabled(
      _backendAvailable && !busy && !connected &&
      !this->federationName().isEmpty() && !this->federateName().isEmpty());
  _disconnectButton->setEnabled(connected);
}

} // namespace presentation
