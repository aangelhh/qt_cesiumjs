#include "presentation/StartupConfigurationDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QtAlgorithms>

namespace presentation {
namespace {

QWidget* makeStandalonePage(QWidget* parent) {
  auto* page = new QWidget(parent);
  auto* layout = new QVBoxLayout(page);
  auto* label = new QLabel(
      QStringLiteral(
          "Run locally without joining a DIS exercise or HLA federation."),
      page);
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addStretch();
  return page;
}

} // namespace

StartupConfigurationDialog::StartupConfigurationDialog(
    const application::StartupConfiguration& configuration,
    QVector<HlaBackendOption> hlaBackends,
    bool ros2Available,
    QWidget* parent)
    : QDialog(parent),
      _modeCombo(new QComboBox(this)),
      _modeStack(new QStackedWidget(this)),
      _disAddressEdit(new QLineEdit(this)),
      _disPortSpin(new QSpinBox(this)),
      _disExerciseSpin(new QSpinBox(this)),
      _disSiteSpin(new QSpinBox(this)),
      _disApplicationSpin(new QSpinBox(this)),
      _disTimeoutSpin(new QDoubleSpinBox(this)),
      _hlaBackendCombo(new QComboBox(this)),
      _hlaLocalSettingsEdit(new QLineEdit(this)),
      _hlaFederationEdit(new QLineEdit(this)),
      _hlaFederateEdit(new QLineEdit(this)),
      _hlaFederateTypeEdit(new QLineEdit(this)),
      _hlaSynchronizationPointEdit(new QLineEdit(this)),
      _hlaTimeManagementCheck(new QCheckBox(
          QStringLiteral("Enable HLA time management"), this)),
      _hlaLookaheadSpin(new QDoubleSpinBox(this)),
      _hlaCreateCheck(new QCheckBox(
          QStringLiteral("Create federation when missing"), this)),
      _hlaFomList(new QListWidget(this)),
      _removeFomButton(new QPushButton(QStringLiteral("Remove"), this)),
      _ros2Check(new QCheckBox(QStringLiteral("Enable ROS 2 / DDS"), this)),
      _ros2DomainSpin(new QSpinBox(this)),
      _ros2TopicEdit(new QLineEdit(this)),
      _cigiCheck(new QCheckBox(QStringLiteral("Enable CIGI output"), this)),
      _cigiHostEdit(new QLineEdit(this)),
      _cigiPortSpin(new QSpinBox(this)),
      _validationLabel(new QLabel(this)),
      _ros2Available(ros2Available) {
  this->setWindowTitle(QStringLiteral("qttest Startup Configuration"));
  this->setModal(true);
  this->resize(960, 820);
  this->setMinimumSize(820, 720);

  auto* title = new QLabel(QStringLiteral("Simulation Configuration"), this);
  QFont titleFont = title->font();
  titleFont.setPointSize(titleFont.pointSize() + 4);
  titleFont.setBold(true);
  title->setFont(titleFont);

  auto* subtitle = new QLabel(
      QStringLiteral(
          "Choose one federation mode. Telemetry and image generation are "
          "independent integrations."),
      this);
  subtitle->setWordWrap(true);

  _modeCombo->addItem(
      QStringLiteral("Standalone"),
      static_cast<int>(application::FederationMode::Standalone));
  _modeCombo->addItem(
      QStringLiteral("DIS"),
      static_cast<int>(application::FederationMode::Dis));
  _modeCombo->addItem(
      QStringLiteral("HLA"),
      static_cast<int>(application::FederationMode::Hla));
  const int modeIndex = _modeCombo->findData(
      static_cast<int>(configuration.federationMode));
  _modeCombo->setCurrentIndex(modeIndex >= 0 ? modeIndex : 0);

  _modeStack->addWidget(makeStandalonePage(_modeStack));

  auto* disPage = new QWidget(_modeStack);
  auto* disForm = new QFormLayout(disPage);
  _disAddressEdit->setText(configuration.dis.address);
  _disPortSpin->setRange(1, 65535);
  _disPortSpin->setValue(configuration.dis.port);
  _disExerciseSpin->setRange(0, 255);
  _disExerciseSpin->setValue(configuration.dis.exerciseId);
  _disSiteSpin->setRange(0, 65535);
  _disSiteSpin->setValue(configuration.dis.siteId);
  _disApplicationSpin->setRange(0, 65535);
  _disApplicationSpin->setValue(configuration.dis.applicationId);
  _disTimeoutSpin->setRange(1.0, 300.0);
  _disTimeoutSpin->setDecimals(1);
  _disTimeoutSpin->setSuffix(QStringLiteral(" s"));
  _disTimeoutSpin->setValue(configuration.dis.remoteEntityTimeoutSeconds);
  disForm->addRow(QStringLiteral("Address"), _disAddressEdit);
  disForm->addRow(QStringLiteral("Port"), _disPortSpin);
  disForm->addRow(QStringLiteral("Exercise ID"), _disExerciseSpin);
  disForm->addRow(QStringLiteral("Site ID"), _disSiteSpin);
  disForm->addRow(QStringLiteral("Application ID"), _disApplicationSpin);
  disForm->addRow(QStringLiteral("Remote timeout"), _disTimeoutSpin);
  auto* disStatus = new QLabel(
      QStringLiteral(
          "Entity State PDUs are published and received over UDP at 10 Hz."),
      disPage);
  disStatus->setWordWrap(true);
  disForm->addRow(QString(), disStatus);
  _modeStack->addWidget(disPage);

  auto* hlaPage = new QWidget(_modeStack);
  auto* hlaLayout = new QHBoxLayout(hlaPage);
  hlaLayout->setSpacing(18);
  auto* connectionGroup = new QGroupBox(
      QStringLiteral("Federation connection"), hlaPage);
  connectionGroup->setMinimumWidth(360);
  auto* hlaForm = new QFormLayout(connectionGroup);
  hlaForm->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  hlaForm->setHorizontalSpacing(18);
  hlaForm->setVerticalSpacing(10);
  for (const HlaBackendOption& backend : hlaBackends) {
    _hlaBackendCombo->addItem(backend.label, backend.id);
    _hlaBackendCombo->setItemData(
        _hlaBackendCombo->count() - 1,
        backend.libraryPath,
        Qt::UserRole + 1);
  }
  const int backendIndex = _hlaBackendCombo->findData(
      configuration.hla.backendId);
  _hlaBackendCombo->setCurrentIndex(backendIndex >= 0 ? backendIndex : 0);
  _hlaLocalSettingsEdit->setText(configuration.hla.localSettingsDesignator);
  if (_hlaLocalSettingsEdit->text().trimmed().isEmpty() &&
      _hlaBackendCombo->currentData().toString() ==
          QStringLiteral("openrti1516e")) {
    _hlaLocalSettingsEdit->setText(QStringLiteral("thread://"));
  }
  _hlaLocalSettingsEdit->setPlaceholderText(
      QStringLiteral(
          "thread://, rti://host:port or crcAddress=host:port"));
  _hlaFederationEdit->setText(configuration.hla.federationName);
  _hlaFederateEdit->setText(configuration.hla.federateName);
  _hlaFederateTypeEdit->setText(configuration.hla.federateType);
  _hlaSynchronizationPointEdit->setText(
      configuration.hla.synchronizationPointLabel);
  _hlaSynchronizationPointEdit->setPlaceholderText(
      QStringLiteral("Optional, for example ReadyToRun"));
  _hlaTimeManagementCheck->setChecked(
      configuration.hla.timeManagementEnabled);
  _hlaLookaheadSpin->setRange(0.001, 60.0);
  _hlaLookaheadSpin->setDecimals(3);
  _hlaLookaheadSpin->setSingleStep(0.01);
  _hlaLookaheadSpin->setSuffix(QStringLiteral(" s"));
  _hlaLookaheadSpin->setValue(configuration.hla.timeLookaheadSeconds);
  _hlaLookaheadSpin->setEnabled(_hlaTimeManagementCheck->isChecked());
  _hlaCreateCheck->setChecked(configuration.hla.createFederationIfMissing);
  hlaForm->addRow(QStringLiteral("Backend"), _hlaBackendCombo);
  hlaForm->addRow(QStringLiteral("Local settings"), _hlaLocalSettingsEdit);
  hlaForm->addRow(QStringLiteral("Federation"), _hlaFederationEdit);
  hlaForm->addRow(QStringLiteral("Federate name"), _hlaFederateEdit);
  hlaForm->addRow(QStringLiteral("Federate type"), _hlaFederateTypeEdit);
  hlaForm->addRow(
      QStringLiteral("Synchronization point"),
      _hlaSynchronizationPointEdit);
  hlaForm->addRow(QString(), _hlaTimeManagementCheck);
  hlaForm->addRow(QStringLiteral("Time lookahead"), _hlaLookaheadSpin);
  hlaForm->addRow(QString(), _hlaCreateCheck);
  auto* hlaStatus = new QLabel(
      QStringLiteral(
          "qttest will connect, create or join the federation, and load the "
          "selected FOM modules before opening the main window."),
      connectionGroup);
  hlaStatus->setWordWrap(true);
  hlaForm->addRow(QString(), hlaStatus);

  auto* fomGroup = new QGroupBox(QStringLiteral("FOM modules"), hlaPage);
  auto* fomLayout = new QVBoxLayout(fomGroup);
  _hlaFomList->addItems(configuration.hla.fomModules);
  _hlaFomList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _hlaFomList->setAlternatingRowColors(true);
  _hlaFomList->setMinimumHeight(240);
  auto* fomButtons = new QHBoxLayout;
  auto* addFomButton = new QPushButton(QStringLiteral("Add FOM..."), this);
  fomButtons->addWidget(addFomButton);
  fomButtons->addWidget(_removeFomButton);
  fomButtons->addStretch();
  fomLayout->addWidget(_hlaFomList, 1);
  fomLayout->addLayout(fomButtons);

  hlaLayout->addWidget(connectionGroup, 2);
  hlaLayout->addWidget(fomGroup, 3);
  _modeStack->addWidget(hlaPage);

  auto* federationGroup = new QGroupBox(QStringLiteral("Federation mode"), this);
  auto* federationLayout = new QVBoxLayout(federationGroup);
  auto* modeForm = new QFormLayout;
  modeForm->addRow(QStringLiteral("Mode"), _modeCombo);
  federationLayout->addLayout(modeForm);
  federationLayout->addWidget(_modeStack);

  _ros2Check->setChecked(configuration.ros2.enabled && ros2Available);
  _ros2Check->setEnabled(ros2Available);
  _ros2DomainSpin->setRange(0, 232);
  _ros2DomainSpin->setValue(configuration.ros2.domainId);
  _ros2TopicEdit->setText(configuration.ros2.topicPrefix);
  auto* rosStatus = new QLabel(
      ros2Available
          ? QStringLiteral("Native ROS 2/DDS backend available")
          : QStringLiteral("Native ROS 2/DDS backend is not available in this build"),
      this);
  rosStatus->setWordWrap(true);

  auto* rosGroup = new QGroupBox(QStringLiteral("ROS 2 / DDS"), this);
  auto* rosForm = new QFormLayout(rosGroup);
  rosForm->addRow(QString(), _ros2Check);
  rosForm->addRow(QStringLiteral("Domain ID"), _ros2DomainSpin);
  rosForm->addRow(QStringLiteral("Topic prefix"), _ros2TopicEdit);
  rosForm->addRow(QString(), rosStatus);

  _cigiCheck->setChecked(configuration.cigi.enabled);
  _cigiHostEdit->setText(configuration.cigi.host);
  _cigiPortSpin->setRange(1, 65535);
  _cigiPortSpin->setValue(configuration.cigi.port);
  auto* cigiStatus = new QLabel(
      QStringLiteral("Configuration is stored; the CIGI runtime adapter is pending."),
      this);
  cigiStatus->setWordWrap(true);

  auto* cigiGroup = new QGroupBox(QStringLiteral("CIGI image generator"), this);
  auto* cigiForm = new QFormLayout(cigiGroup);
  cigiForm->addRow(QString(), _cigiCheck);
  cigiForm->addRow(QStringLiteral("Host"), _cigiHostEdit);
  cigiForm->addRow(QStringLiteral("Port"), _cigiPortSpin);
  cigiForm->addRow(QString(), cigiStatus);

  _validationLabel->setWordWrap(true);
  _validationLabel->setStyleSheet(QStringLiteral("color: #d9534f;"));

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Cancel,
      this);
  auto* startButton = buttons->addButton(
      QStringLiteral("Start qttest"),
      QDialogButtonBox::AcceptRole);
  startButton->setDefault(true);

  auto* integrations = new QHBoxLayout;
  integrations->addWidget(rosGroup, 1);
  integrations->addWidget(cigiGroup, 1);

  auto* root = new QVBoxLayout(this);
  root->addWidget(title);
  root->addWidget(subtitle);
  root->addWidget(federationGroup, 1);
  root->addLayout(integrations);
  root->addWidget(_validationLabel);
  root->addWidget(buttons);

  connect(
      _modeCombo,
      qOverload<int>(&QComboBox::currentIndexChanged),
      this,
      &StartupConfigurationDialog::updateModePage);
  connect(
      _ros2Check,
      &QCheckBox::toggled,
      this,
      &StartupConfigurationDialog::updateIntegrationControls);
  connect(
      _cigiCheck,
      &QCheckBox::toggled,
      this,
      &StartupConfigurationDialog::updateIntegrationControls);
  connect(
      _hlaTimeManagementCheck,
      &QCheckBox::toggled,
      _hlaLookaheadSpin,
      &QWidget::setEnabled);
  connect(addFomButton, &QPushButton::clicked, this, &StartupConfigurationDialog::addFomModules);
  connect(_removeFomButton, &QPushButton::clicked, this, &StartupConfigurationDialog::removeSelectedFomModules);
  connect(startButton, &QPushButton::clicked, this, &StartupConfigurationDialog::acceptConfiguration);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  this->updateModePage();
  this->updateIntegrationControls();
}

application::StartupConfiguration
StartupConfigurationDialog::configuration() const {
  application::StartupConfiguration result;
  result.federationMode = static_cast<application::FederationMode>(
      _modeCombo->currentData().toInt());
  result.dis.address = _disAddressEdit->text().trimmed();
  result.dis.port = _disPortSpin->value();
  result.dis.exerciseId = _disExerciseSpin->value();
  result.dis.siteId = _disSiteSpin->value();
  result.dis.applicationId = _disApplicationSpin->value();
  result.dis.remoteEntityTimeoutSeconds = _disTimeoutSpin->value();

  result.hla.backendId = _hlaBackendCombo->currentData().toString();
  result.hla.backendLibraryPath = _hlaBackendCombo->currentData(
      Qt::UserRole + 1).toString();
  result.hla.localSettingsDesignator = _hlaLocalSettingsEdit->text().trimmed();
  result.hla.federationName = _hlaFederationEdit->text().trimmed();
  result.hla.federateName = _hlaFederateEdit->text().trimmed();
  result.hla.federateType = _hlaFederateTypeEdit->text().trimmed();
  result.hla.synchronizationPointLabel =
      _hlaSynchronizationPointEdit->text().trimmed();
  result.hla.timeManagementEnabled = _hlaTimeManagementCheck->isChecked();
  result.hla.timeLookaheadSeconds = _hlaLookaheadSpin->value();
  result.hla.createFederationIfMissing = _hlaCreateCheck->isChecked();
  for (int index = 0; index < _hlaFomList->count(); ++index) {
    result.hla.fomModules.push_back(_hlaFomList->item(index)->text());
  }

  result.ros2.enabled = _ros2Available && _ros2Check->isChecked();
  result.ros2.domainId = _ros2DomainSpin->value();
  result.ros2.topicPrefix = _ros2TopicEdit->text().trimmed();
  result.cigi.enabled = _cigiCheck->isChecked();
  result.cigi.host = _cigiHostEdit->text().trimmed();
  result.cigi.port = _cigiPortSpin->value();
  return result;
}

void StartupConfigurationDialog::updateModePage() {
  const auto mode = static_cast<application::FederationMode>(
      _modeCombo->currentData().toInt());
  _modeStack->setCurrentIndex(static_cast<int>(mode));
  _validationLabel->clear();
}

void StartupConfigurationDialog::updateIntegrationControls() {
  const bool rosEnabled = _ros2Available && _ros2Check->isChecked();
  _ros2DomainSpin->setEnabled(rosEnabled);
  _ros2TopicEdit->setEnabled(rosEnabled);
  const bool cigiEnabled = _cigiCheck->isChecked();
  _cigiHostEdit->setEnabled(cigiEnabled);
  _cigiPortSpin->setEnabled(cigiEnabled);
}

void StartupConfigurationDialog::addFomModules() {
  const QStringList paths = QFileDialog::getOpenFileNames(
      this,
      QStringLiteral("Add HLA FOM Modules"),
      QString(),
      QStringLiteral("HLA FOM modules (*.xml);;All files (*)"));
  for (const QString& path : paths) {
    if (_hlaFomList->findItems(path, Qt::MatchExactly).isEmpty()) {
      _hlaFomList->addItem(path);
    }
  }
}

void StartupConfigurationDialog::removeSelectedFomModules() {
  qDeleteAll(_hlaFomList->selectedItems());
}

void StartupConfigurationDialog::acceptConfiguration() {
  const QString error = this->configuration().validationError();
  if (!error.isEmpty()) {
    _validationLabel->setText(error);
    return;
  }
  this->accept();
}

} // namespace presentation
