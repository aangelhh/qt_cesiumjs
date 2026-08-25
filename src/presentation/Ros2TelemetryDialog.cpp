#include "presentation/Ros2TelemetryDialog.h"

#include "domain/EntityIdentity.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

namespace presentation {

Ros2TelemetryDialog::Ros2TelemetryDialog(
    const infrastructure::Ros2TelemetryConfiguration& configuration,
    const QVector<Entity>& entities,
    bool ros2Available,
    const QString& availabilityMessage,
    QWidget* parent)
    : QDialog(parent),
      _enabledCheckBox(new QCheckBox(QStringLiteral("Enabled"), this)),
      _topicPrefixEdit(new QLineEdit(this)),
      _entityComboBox(new QComboBox(this)),
      _frequencySpinBox(new QDoubleSpinBox(this)),
      _csvCheckBox(new QCheckBox(QStringLiteral("Record CSV"), this)),
      _csvPathEdit(new QLineEdit(this)),
      _csvBrowseButton(new QPushButton(QStringLiteral("Browse..."), this)) {
  this->setWindowTitle(QStringLiteral("ROS 2 Telemetry"));
  this->setModal(true);
  this->setMinimumWidth(480);

  _enabledCheckBox->setChecked(configuration.ros2Enabled);
  _enabledCheckBox->setEnabled(ros2Available);
  _topicPrefixEdit->setText(configuration.topicPrefix);
  _frequencySpinBox->setRange(0.1, 60.0);
  _frequencySpinBox->setDecimals(1);
  _frequencySpinBox->setSuffix(QStringLiteral(" Hz"));
  _frequencySpinBox->setValue(configuration.frequencyHertz);

  _entityComboBox->addItem(QStringLiteral("All entities"), QString{});
  for (const Entity& entity : entities) {
    const QString entityId = domain::entityKey(entity);
    _entityComboBox->addItem(
        QStringLiteral("%1 [%2]").arg(entity.name, entityId.left(8)),
        entityId);
  }
  const int entityIndex = _entityComboBox->findData(
      configuration.entityReference);
  _entityComboBox->setCurrentIndex(entityIndex >= 0 ? entityIndex : 0);

  _csvCheckBox->setChecked(configuration.csvEnabled);
  QString csvPath = configuration.csvPath;
  if (csvPath.isEmpty()) {
    csvPath = QStandardPaths::writableLocation(
        QStandardPaths::DocumentsLocation) +
        QStringLiteral("/qttest_kinematics.csv");
  }
  _csvPathEdit->setText(csvPath);

  auto* csvRow = new QWidget(this);
  auto* csvLayout = new QHBoxLayout(csvRow);
  csvLayout->setContentsMargins(0, 0, 0, 0);
  csvLayout->addWidget(_csvPathEdit, 1);
  csvLayout->addWidget(_csvBrowseButton);

  auto* form = new QFormLayout;
  form->addRow(QString(), _enabledCheckBox);
  form->addRow(QStringLiteral("Topic prefix"), _topicPrefixEdit);
  form->addRow(QStringLiteral("Entity"), _entityComboBox);
  form->addRow(QStringLiteral("Frequency"), _frequencySpinBox);
  form->addRow(QString(), _csvCheckBox);
  form->addRow(QStringLiteral("CSV file"), csvRow);

  auto* hint = new QLabel(
      ros2Available
      ? QStringLiteral("ROS 2/DDS backend available")
      : availabilityMessage,
      this);
  hint->setWordWrap(true);

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      this);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(
      _csvBrowseButton,
      &QPushButton::clicked,
      this,
      &Ros2TelemetryDialog::chooseCsvPath);
  connect(
      _csvCheckBox,
      &QCheckBox::toggled,
      this,
      &Ros2TelemetryDialog::updateCsvControls);

  auto* layout = new QVBoxLayout(this);
  layout->addLayout(form);
  layout->addWidget(hint);
  layout->addWidget(buttons);
  this->updateCsvControls();
}

infrastructure::Ros2TelemetryConfiguration
Ros2TelemetryDialog::configuration() const {
  infrastructure::Ros2TelemetryConfiguration configuration;
  configuration.ros2Enabled = _enabledCheckBox->isChecked();
  configuration.topicPrefix = _topicPrefixEdit->text().trimmed();
  configuration.entityReference = _entityComboBox->currentData().toString();
  configuration.frequencyHertz = _frequencySpinBox->value();
  configuration.csvEnabled = _csvCheckBox->isChecked();
  configuration.csvPath = _csvPathEdit->text().trimmed();
  return configuration;
}

void Ros2TelemetryDialog::chooseCsvPath() {
  const QString path = QFileDialog::getSaveFileName(
      this,
      QStringLiteral("Record Kinematics Telemetry"),
      _csvPathEdit->text(),
      QStringLiteral("CSV files (*.csv)"));
  if (!path.isEmpty()) {
    _csvPathEdit->setText(path);
  }
}

void Ros2TelemetryDialog::updateCsvControls() {
  const bool enabled = _csvCheckBox->isChecked();
  _csvPathEdit->setEnabled(enabled);
  _csvBrowseButton->setEnabled(enabled);
}

} // namespace presentation
