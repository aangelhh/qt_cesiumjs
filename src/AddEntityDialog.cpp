#include "AddEntityDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QDir>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QSet>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QUrl>
#include <QVBoxLayout>

#include <algorithm>

namespace {

const ModelCatalogEntry* findSelectedModelEntry(
    const QVector<ModelCatalogEntry>& catalog,
    const QString& domain,
    const QString& category,
    const QString& name,
    const QString& absolutePath) {
  for (const ModelCatalogEntry& entry : catalog) {
    if (entry.domain == domain && entry.category == category &&
        entry.name == name && entry.absolutePath == absolutePath) {
      return &entry;
    }
  }
  return nullptr;
}

} // namespace

namespace {

QStringList availableJsbsimAircraftModels() {
  QStringList models;
#ifdef QTTEST_SOURCE_DIR
  const QDir aircraftRoot(
      QDir(QString::fromUtf8(QTTEST_SOURCE_DIR))
          .absoluteFilePath(QStringLiteral("Dependencies/jsbsim/aircraft")));
  const QFileInfoList entries = aircraftRoot.entryInfoList(
      QDir::Dirs | QDir::NoDotAndDotDot,
      QDir::Name);
  for (const QFileInfo& entry : entries) {
    if (entry.isDir()) {
      models.append(entry.fileName());
    }
  }
#endif
  return models;
}

QString suggestedJsbsimModel(const QString& domain, const QString& category, const QString& modelName) {
  const QString normalizedDomain = domain.trimmed().toLower();
  const QString normalizedCategory = category.trimmed().toLower();
  const QString normalizedModelName = modelName.trimmed().toLower();

  if (normalizedModelName.contains(QStringLiteral("f-16"))) {
    return QStringLiteral("f16");
  }
  if (normalizedModelName.contains(QStringLiteral("f-22"))) {
    return QStringLiteral("f22");
  }
  if (normalizedModelName.contains(QStringLiteral("a-4"))) {
    return QStringLiteral("A4");
  }
  if (normalizedModelName.contains(QStringLiteral("t-38"))) {
    return QStringLiteral("T38");
  }
  if (normalizedModelName.contains(QStringLiteral("dhc"))) {
    return QStringLiteral("DHC6");
  }
  if (normalizedModelName.contains(QStringLiteral("c172"))) {
    return QStringLiteral("c172r");
  }

  if (normalizedDomain == QStringLiteral("air")) {
    if (normalizedCategory == QStringLiteral("fighter")) {
      return QStringLiteral("f16");
    }
    if (normalizedCategory == QStringLiteral("bomber")) {
      return QStringLiteral("B17");
    }
    if (normalizedCategory == QStringLiteral("transport")) {
      return QStringLiteral("DHC6");
    }
    if (normalizedCategory == QStringLiteral("helicopter")) {
      return QStringLiteral("F450");
    }
  }
  return QStringLiteral("c172r");
}

} // namespace

AddEntityDialog::AddEntityDialog(const QVector<ModelCatalogEntry>& modelCatalog, QWidget* parent)
    : QDialog(parent),
      _modelCatalog(modelCatalog),
      _disCatalog(DisEntityCatalog::loadDefault()),
      _nameEdit(new QLineEdit(this)),
      _callsignEdit(new QLineEdit(this)),
      _forceIdentifierCombo(new QComboBox(this)),
      _domainCombo(new QComboBox(this)),
      _categoryCombo(new QComboBox(this)),
      _modelCombo(new QComboBox(this)),
      _disKindCombo(new QComboBox(this)),
      _disDomainCombo(new QComboBox(this)),
      _disCountryCombo(new QComboBox(this)),
      _disCategoryCombo(new QComboBox(this)),
      _disSubcategoryCombo(new QComboBox(this)),
      _disSpecificCombo(new QComboBox(this)),
      _disExtraCombo(new QComboBox(this)),
      _addRadarCheck(new QCheckBox(QStringLiteral("Attach primary radar"), this)),
      _radarNameEdit(new QLineEdit(this)),
      _headingSpin(new QDoubleSpinBox(this)),
      _enableDynamicsCheck(new QCheckBox(QStringLiteral("Enable flight dynamics"), this)),
      _dynamicsModeCombo(new QComboBox(this)),
      _jsbsimModelCombo(new QComboBox(this)),
      _speedSpin(new QDoubleSpinBox(this)),
      _verticalSpeedSpin(new QDoubleSpinBox(this)),
      _enableFlightTaskCheck(new QCheckBox(QStringLiteral("Assign initial flight task"), this)),
      _taskHeadingSpin(new QDoubleSpinBox(this)),
      _taskAltitudeSpin(new QSpinBox(this)),
      _taskSpeedSpin(new QDoubleSpinBox(this)),
      _radarRangeSpin(new QDoubleSpinBox(this)),
      _radarAzimuthSpin(new QDoubleSpinBox(this)),
      _radarMaxTracksSpin(new QSpinBox(this)),
      _latitudeSpin(new QDoubleSpinBox(this)),
      _longitudeSpin(new QDoubleSpinBox(this)),
      _groundHeightSpin(new QDoubleSpinBox(this)),
      _altitudeSpin(new QSpinBox(this)),
      _pickOnMapButton(nullptr) {
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(QStringLiteral("Add Entity"));
  this->setModal(false);
  this->setWindowModality(Qt::NonModal);

  auto* layout = new QVBoxLayout(this);
  auto* formLayout = new QFormLayout();

  _latitudeSpin->setRange(-90.0, 90.0);
  _latitudeSpin->setDecimals(6);
  _latitudeSpin->setSingleStep(0.001);
  _latitudeSpin->setSuffix(QStringLiteral(" deg"));

  _longitudeSpin->setRange(-180.0, 180.0);
  _longitudeSpin->setDecimals(6);
  _longitudeSpin->setSingleStep(0.001);
  _longitudeSpin->setSuffix(QStringLiteral(" deg"));

  _groundHeightSpin->setRange(-1000.0, 100000.0);
  _groundHeightSpin->setDecimals(2);
  _groundHeightSpin->setSingleStep(10.0);
  _groundHeightSpin->setSuffix(QStringLiteral(" m"));

  _altitudeSpin->setRange(0, 80000);
  _altitudeSpin->setSuffix(QStringLiteral(" m"));
  _altitudeSpin->setValue(1200);

  _headingSpin->setRange(0.0, 359.0);
  _headingSpin->setDecimals(1);
  _headingSpin->setSingleStep(5.0);
  _headingSpin->setSuffix(QStringLiteral(" deg"));

  _radarRangeSpin->setRange(1.0, 1000.0);
  _radarRangeSpin->setDecimals(1);
  _radarRangeSpin->setSingleStep(10.0);
  _radarRangeSpin->setSuffix(QStringLiteral(" km"));
  _radarRangeSpin->setValue(280.0);

  _radarAzimuthSpin->setRange(1.0, 360.0);
  _radarAzimuthSpin->setDecimals(1);
  _radarAzimuthSpin->setSingleStep(5.0);
  _radarAzimuthSpin->setSuffix(QStringLiteral(" deg"));
  _radarAzimuthSpin->setValue(120.0);

  _radarMaxTracksSpin->setRange(1, 256);
  _radarMaxTracksSpin->setValue(16);

  _speedSpin->setRange(0.0, 1200.0);
  _speedSpin->setDecimals(1);
  _speedSpin->setSingleStep(10.0);
  _speedSpin->setSuffix(QStringLiteral(" kts"));
  _speedSpin->setValue(320.0);

  _verticalSpeedSpin->setRange(-200.0, 200.0);
  _verticalSpeedSpin->setDecimals(1);
  _verticalSpeedSpin->setSingleStep(1.0);
  _verticalSpeedSpin->setSuffix(QStringLiteral(" m/s"));

  _taskHeadingSpin->setRange(0.0, 359.0);
  _taskHeadingSpin->setDecimals(1);
  _taskHeadingSpin->setSingleStep(5.0);
  _taskHeadingSpin->setSuffix(QStringLiteral(" deg"));

  _taskAltitudeSpin->setRange(0, 80000);
  _taskAltitudeSpin->setSuffix(QStringLiteral(" m"));
  _taskAltitudeSpin->setValue(1200);

  _taskSpeedSpin->setRange(0.0, 1200.0);
  _taskSpeedSpin->setDecimals(1);
  _taskSpeedSpin->setSingleStep(10.0);
  _taskSpeedSpin->setSuffix(QStringLiteral(" kts"));
  _taskSpeedSpin->setValue(320.0);

  _nameEdit->setPlaceholderText(QStringLiteral("Entity Alpha"));
  _callsignEdit->setPlaceholderText(QStringLiteral("Eagle 1"));
  _radarNameEdit->setPlaceholderText(QStringLiteral("Primary Radar"));
  _addRadarCheck->setChecked(true);

  _forceIdentifierCombo->addItem(QStringLiteral("Friendly"), 1);
  _forceIdentifierCombo->addItem(QStringLiteral("Opposing"), 2);
  _forceIdentifierCombo->addItem(QStringLiteral("Neutral"), 3);

  _dynamicsModeCombo->addItem(QStringLiteral("Kinematic"), QStringLiteral("kinematic"));
#if defined(QTTEST_HAS_JSBSIM)
  _dynamicsModeCombo->addItem(QStringLiteral("JSBSim"), QStringLiteral("jsbsim"));
#endif

  this->populateDomainCombo();
  this->populateCategoryCombo();
  this->populateModelCombo();
  this->populateDisKindCombo();
  this->populateDisCountryCombo();
  this->populateDisDomainCombo();
  this->populateDisCategoryCombo();
  this->populateDisSubcategoryCombo();
  this->populateDisSpecificCombo();
  this->populateDisExtraCombo();
  this->populateJsbsimModelCombo();

  QObject::connect(_domainCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->populateCategoryCombo();
    this->populateModelCombo();
    this->populateJsbsimModelCombo();
  });
  QObject::connect(_categoryCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->populateModelCombo();
    this->populateJsbsimModelCombo();
  });
  QObject::connect(_modelCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->applyModelSelectionToDisFields();
  });
  QObject::connect(_disKindCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->populateDisDomainCombo();
    this->populateDisCategoryCombo();
    this->populateDisSubcategoryCombo();
    this->populateDisSpecificCombo();
    this->populateDisExtraCombo();
    this->syncModelFromDisSelection();
  });
  QObject::connect(_disCountryCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->populateDisSubcategoryCombo();
    this->populateDisSpecificCombo();
    this->populateDisExtraCombo();
    this->syncModelFromDisSelection();
  });
  QObject::connect(_disDomainCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->populateDisCategoryCombo();
    this->populateDisSubcategoryCombo();
    this->populateDisSpecificCombo();
    this->populateDisExtraCombo();
    this->syncModelFromDisSelection();
  });
  QObject::connect(_disCategoryCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->populateDisSubcategoryCombo();
    this->populateDisSpecificCombo();
    this->populateDisExtraCombo();
    this->syncModelFromDisSelection();
  });
  QObject::connect(_disSubcategoryCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->populateDisSpecificCombo();
    this->populateDisExtraCombo();
    this->syncModelFromDisSelection();
  });
  QObject::connect(_disSpecificCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->populateDisExtraCombo();
    this->syncModelFromDisSelection();
  });
  QObject::connect(_disExtraCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->syncModelFromDisSelection();
  });
  QObject::connect(_addRadarCheck, &QCheckBox::toggled, this, [this](bool enabled) {
    _radarNameEdit->setEnabled(enabled);
    _radarRangeSpin->setEnabled(enabled);
    _radarAzimuthSpin->setEnabled(enabled);
    _radarMaxTracksSpin->setEnabled(enabled);
  });
  QObject::connect(_enableDynamicsCheck, &QCheckBox::toggled, this, [this](bool enabled) {
    Q_UNUSED(enabled)
    this->syncDynamicsControls();
  });
  QObject::connect(_dynamicsModeCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->syncDynamicsControls();
  });
  QObject::connect(_enableFlightTaskCheck, &QCheckBox::toggled, this, [this](bool enabled) {
    _taskHeadingSpin->setEnabled(enabled);
    _taskAltitudeSpin->setEnabled(enabled);
    _taskSpeedSpin->setEnabled(enabled);
  });

  formLayout->addRow(QStringLiteral("Name"), _nameEdit);
  formLayout->addRow(QStringLiteral("forceIdentifier"), _forceIdentifierCombo);
  formLayout->addRow(QStringLiteral("Domain"), _domainCombo);
  formLayout->addRow(QStringLiteral("Category"), _categoryCombo);
  formLayout->addRow(QStringLiteral("3D Model"), _modelCombo);
  formLayout->addRow(QStringLiteral("DIS Kind"), _disKindCombo);
  formLayout->addRow(QStringLiteral("DIS Domain"), _disDomainCombo);
  formLayout->addRow(QStringLiteral("DIS Country"), _disCountryCombo);
  formLayout->addRow(QStringLiteral("DIS Category"), _disCategoryCombo);
  formLayout->addRow(QStringLiteral("DIS Subcategory"), _disSubcategoryCombo);
  formLayout->addRow(QStringLiteral("DIS Specific"), _disSpecificCombo);
  formLayout->addRow(QStringLiteral("DIS Extra"), _disExtraCombo);
  formLayout->addRow(QStringLiteral("Callsign"), _callsignEdit);
  formLayout->addRow(QStringLiteral("Heading"), _headingSpin);
  formLayout->addRow(QStringLiteral("Dynamics"), _enableDynamicsCheck);
  formLayout->addRow(QStringLiteral("Dynamics Mode"), _dynamicsModeCombo);
  formLayout->addRow(QStringLiteral("JSBSim Aircraft"), _jsbsimModelCombo);
  formLayout->addRow(QStringLiteral("Speed"), _speedSpin);
  formLayout->addRow(QStringLiteral("Vertical Speed"), _verticalSpeedSpin);
  formLayout->addRow(QStringLiteral("Initial Task"), _enableFlightTaskCheck);
  formLayout->addRow(QStringLiteral("Initial Task Heading"), _taskHeadingSpin);
  formLayout->addRow(QStringLiteral("Initial Task Altitude"), _taskAltitudeSpin);
  formLayout->addRow(QStringLiteral("Initial Task Speed"), _taskSpeedSpin);
  formLayout->addRow(QStringLiteral("Sensor"), _addRadarCheck);
  formLayout->addRow(QStringLiteral("Radar Name"), _radarNameEdit);
  formLayout->addRow(QStringLiteral("Radar Range"), _radarRangeSpin);
  formLayout->addRow(QStringLiteral("Radar Azimuth"), _radarAzimuthSpin);
  formLayout->addRow(QStringLiteral("Radar Max Tracks"), _radarMaxTracksSpin);
  formLayout->addRow(QStringLiteral("Latitude"), _latitudeSpin);
  formLayout->addRow(QStringLiteral("Longitude"), _longitudeSpin);
  formLayout->addRow(QStringLiteral("Ground Height"), _groundHeightSpin);
  formLayout->addRow(QStringLiteral("Altitude"), _altitudeSpin);
  if (!_disCatalog.isLoaded()) {
    auto* warningLabel = new QLabel(
        QStringLiteral("DIS catalog unavailable: %1").arg(_disCatalog.errorString()),
        this);
    warningLabel->setWordWrap(true);
    layout->addWidget(warningLabel);
  }
  _radarNameEdit->setEnabled(_addRadarCheck->isChecked());
  _radarRangeSpin->setEnabled(_addRadarCheck->isChecked());
  _radarAzimuthSpin->setEnabled(_addRadarCheck->isChecked());
  _radarMaxTracksSpin->setEnabled(_addRadarCheck->isChecked());
  this->syncDynamicsControls();
  _taskHeadingSpin->setEnabled(_enableFlightTaskCheck->isChecked());
  _taskAltitudeSpin->setEnabled(_enableFlightTaskCheck->isChecked());
  _taskSpeedSpin->setEnabled(_enableFlightTaskCheck->isChecked());
  layout->addLayout(formLayout);

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal,
      this);
  _pickOnMapButton = buttons->addButton(
      QStringLiteral("Pick on map"),
      QDialogButtonBox::ActionRole);
  layout->addWidget(buttons);

  QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::close);
  QObject::connect(
      _pickOnMapButton,
      &QPushButton::clicked,
      this,
      &AddEntityDialog::pickOnMapRequested);
  QObject::connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
    if (_nameEdit->text().trimmed().isEmpty()) {
      QMessageBox::warning(
          this,
          QStringLiteral("Entity"),
          QStringLiteral("La entidad necesita al menos un nombre."));
      return;
    }
    this->accept();
  });
}

void AddEntityDialog::populateDomainCombo() {
  QSignalBlocker blocker(_domainCombo);
  const QString previous = _domainCombo->currentText();
  _domainCombo->clear();

  QSet<QString> domains;
  for (const ModelCatalogEntry& entry : _modelCatalog) {
    domains.insert(entry.domain);
  }
  if (domains.isEmpty()) {
    domains.insert(QStringLiteral("Air"));
  }

  QStringList ordered = domains.values();
  std::sort(ordered.begin(), ordered.end());
  _domainCombo->addItems(ordered);

  const int previousIndex = _domainCombo->findText(previous);
  if (previousIndex >= 0) {
    _domainCombo->setCurrentIndex(previousIndex);
  }
}

void AddEntityDialog::populateCategoryCombo() {
  QSignalBlocker blocker(_categoryCombo);
  const QString previous = _categoryCombo->currentText();
  const QString currentDomain = _domainCombo->currentText().trimmed();
  _categoryCombo->clear();

  QSet<QString> categories;
  for (const ModelCatalogEntry& entry : _modelCatalog) {
    if (entry.domain == currentDomain) {
      categories.insert(entry.category);
    }
  }
  if (categories.isEmpty()) {
    categories.insert(QStringLiteral("Fighter"));
  }

  QStringList ordered = categories.values();
  std::sort(ordered.begin(), ordered.end());
  _categoryCombo->addItems(ordered);

  const int previousIndex = _categoryCombo->findText(previous);
  if (previousIndex >= 0) {
    _categoryCombo->setCurrentIndex(previousIndex);
  }
}

void AddEntityDialog::populateModelCombo() {
  QSignalBlocker blocker(_modelCombo);
  const QString previous = _modelCombo->currentText();
  const QString currentDomain = _domainCombo->currentText().trimmed();
  const QString currentCategory = _categoryCombo->currentText().trimmed();
  _modelCombo->clear();
  _modelCombo->addItem(QStringLiteral("No 3D model"), QString());

  for (const ModelCatalogEntry& entry : _modelCatalog) {
    if (entry.domain == currentDomain && entry.category == currentCategory) {
      _modelCombo->addItem(entry.name, entry.absolutePath);
    }
  }

  const int previousIndex = _modelCombo->findText(previous);
  if (previousIndex >= 0) {
    _modelCombo->setCurrentIndex(previousIndex);
  }
}

void AddEntityDialog::populateDisKindCombo() {
  QSignalBlocker blocker(_disKindCombo);
  const int previous = _disKindCombo->currentData().toInt();
  _disKindCombo->clear();
  for (const DisEnumOption& option : _disCatalog.kinds()) {
    _disKindCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
  }
  int index = _disKindCombo->findData(previous);
  if (index < 0) {
    index = _disKindCombo->findData(1);
  }
  if (index < 0 && _disKindCombo->count() > 0) {
    index = 0;
  }
  if (index >= 0) {
    _disKindCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateDisDomainCombo() {
  QSignalBlocker blocker(_disDomainCombo);
  const int previous = _disDomainCombo->currentData().toInt();
  _disDomainCombo->clear();
  for (const DisEnumOption& option : _disCatalog.domains(_disKindCombo->currentData().toInt())) {
    _disDomainCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
  }
  int index = _disDomainCombo->findData(previous);
  if (index < 0) {
    index = _disDomainCombo->findData(2);
  }
  if (index < 0 && _disDomainCombo->count() > 0) {
    index = 0;
  }
  if (index >= 0) {
    _disDomainCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateDisCountryCombo() {
  QSignalBlocker blocker(_disCountryCombo);
  const int previous = _disCountryCombo->currentData().toInt();
  _disCountryCombo->clear();
  for (const DisEnumOption& option : _disCatalog.countries()) {
    _disCountryCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
  }
  int index = _disCountryCombo->findData(previous);
  if (index < 0) {
    index = _disCountryCombo->findData(225);
  }
  if (index < 0 && _disCountryCombo->count() > 0) {
    index = 0;
  }
  if (index >= 0) {
    _disCountryCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateDisCategoryCombo() {
  QSignalBlocker blocker(_disCategoryCombo);
  const int previous = _disCategoryCombo->currentData().toInt();
  _disCategoryCombo->clear();
  for (const DisEnumOption& option : _disCatalog.categories(
           _disKindCombo->currentData().toInt(),
           _disDomainCombo->currentData().toInt())) {
    _disCategoryCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
  }
  int index = _disCategoryCombo->findData(previous);
  if (index < 0) {
    index = 0;
  }
  if (index < 0 && _disCategoryCombo->count() > 0) {
    index = 0;
  }
  if (index >= 0) {
    _disCategoryCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateDisSubcategoryCombo() {
  QSignalBlocker blocker(_disSubcategoryCombo);
  const int previous = _disSubcategoryCombo->currentData().toInt();
  _disSubcategoryCombo->clear();
  const QList<DisEnumOption> options = _disCatalog.subcategories(
      _disKindCombo->currentData().toInt(),
      _disDomainCombo->currentData().toInt(),
      _disCountryCombo->currentData().toInt(),
      _disCategoryCombo->currentData().toInt());
  if (options.isEmpty()) {
    _disSubcategoryCombo->addItem(QStringLiteral("Other (0)"), 0);
  } else {
    for (const DisEnumOption& option : options) {
      _disSubcategoryCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
    }
  }
  int index = _disSubcategoryCombo->findData(previous);
  if (index < 0) {
    index = 0;
  }
  if (index >= 0) {
    _disSubcategoryCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateDisSpecificCombo() {
  QSignalBlocker blocker(_disSpecificCombo);
  const int previous = _disSpecificCombo->currentData().toInt();
  _disSpecificCombo->clear();
  const QList<DisEnumOption> options = _disCatalog.specifics(
      _disKindCombo->currentData().toInt(),
      _disDomainCombo->currentData().toInt(),
      _disCountryCombo->currentData().toInt(),
      _disCategoryCombo->currentData().toInt(),
      _disSubcategoryCombo->currentData().toInt());
  if (options.isEmpty()) {
    _disSpecificCombo->addItem(QStringLiteral("Other (0)"), 0);
  } else {
    for (const DisEnumOption& option : options) {
      _disSpecificCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
    }
  }
  int index = _disSpecificCombo->findData(previous);
  if (index < 0) {
    index = 0;
  }
  if (index >= 0) {
    _disSpecificCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateDisExtraCombo() {
  QSignalBlocker blocker(_disExtraCombo);
  const int previous = _disExtraCombo->currentData().toInt();
  _disExtraCombo->clear();
  const QList<DisEnumOption> options = _disCatalog.extras(
      _disKindCombo->currentData().toInt(),
      _disDomainCombo->currentData().toInt(),
      _disCountryCombo->currentData().toInt(),
      _disCategoryCombo->currentData().toInt(),
      _disSubcategoryCombo->currentData().toInt(),
      _disSpecificCombo->currentData().toInt());
  _disExtraCombo->addItem(QStringLiteral("None (0)"), 0);
  for (const DisEnumOption& option : options) {
    _disExtraCombo->addItem(QStringLiteral("%1 (%2)").arg(option.description).arg(option.id), option.id);
  }
  int index = _disExtraCombo->findData(previous);
  if (index < 0) {
    index = 0;
  }
  if (index >= 0) {
    _disExtraCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::populateJsbsimModelCombo() {
  QSignalBlocker blocker(_jsbsimModelCombo);
  const QString previous = _jsbsimModelCombo->currentText();
  _jsbsimModelCombo->clear();

  const QStringList models = availableJsbsimAircraftModels();
  for (const QString& model : models) {
    _jsbsimModelCombo->addItem(model, model);
  }

  QString preferred = previous;
  if (preferred.isEmpty()) {
    preferred = suggestedJsbsimModel(
        _domainCombo->currentText(),
        _categoryCombo->currentText(),
        _modelCombo->currentText());
  }

  int index = _jsbsimModelCombo->findData(preferred);
  if (index < 0 && _jsbsimModelCombo->count() > 0) {
    index = 0;
  }
  if (index >= 0) {
    _jsbsimModelCombo->setCurrentIndex(index);
  }
}

void AddEntityDialog::syncModelFromDisSelection() {
  const int kind = _disKindCombo->currentData().toInt();
  const int domainCode = _disDomainCombo->currentData().toInt();
  const int country = _disCountryCombo->currentData().toInt();
  const int categoryCode = _disCategoryCombo->currentData().toInt();
  const int subcategory = _disSubcategoryCombo->currentData().toInt();
  const int specific = _disSpecificCombo->currentData().toInt();
  const int extra = _disExtraCombo->currentData().toInt();

  int targetIndex = 0;
  for (int index = 1; index < _modelCombo->count(); ++index) {
    const QString modelPath = _modelCombo->itemData(index).toString();
    const ModelCatalogEntry* entry = findSelectedModelEntry(
        _modelCatalog,
        _domainCombo->currentText().trimmed(),
        _categoryCombo->currentText().trimmed(),
        _modelCombo->itemText(index),
        modelPath);
    if (!entry) {
      continue;
    }
    if (entry->entityKind == kind &&
        entry->entityDomain == domainCode &&
        entry->entityCountry == country &&
        entry->entityCategory == categoryCode &&
        entry->entitySubcategory == subcategory &&
        entry->entitySpecific == specific &&
        entry->entityExtra == extra) {
      targetIndex = index;
      break;
    }
  }

  QSignalBlocker blocker(_modelCombo);
  _modelCombo->setCurrentIndex(targetIndex);
}

void AddEntityDialog::applyModelSelectionToDisFields() {
  const QString modelPath = _modelCombo->currentData().toString();
  const ModelCatalogEntry* selectedEntry = findSelectedModelEntry(
      _modelCatalog,
      _domainCombo->currentText().trimmed(),
      _categoryCombo->currentText().trimmed(),
      _modelCombo->currentText(),
      modelPath);
  if (!selectedEntry) {
    return;
  }

  {
    QSignalBlocker blocker(_disKindCombo);
    const int index = _disKindCombo->findData(selectedEntry->entityKind);
    if (index >= 0) {
      _disKindCombo->setCurrentIndex(index);
    }
  }
  this->populateDisDomainCombo();
  {
    QSignalBlocker blocker(_disDomainCombo);
    const int index = _disDomainCombo->findData(selectedEntry->entityDomain);
    if (index >= 0) {
      _disDomainCombo->setCurrentIndex(index);
    }
  }
  {
    QSignalBlocker blocker(_disCountryCombo);
    const int index = _disCountryCombo->findData(selectedEntry->entityCountry);
    if (index >= 0) {
      _disCountryCombo->setCurrentIndex(index);
    }
  }
  this->populateDisCategoryCombo();
  {
    QSignalBlocker blocker(_disCategoryCombo);
    const int index = _disCategoryCombo->findData(selectedEntry->entityCategory);
    if (index >= 0) {
      _disCategoryCombo->setCurrentIndex(index);
    }
  }
  this->populateDisSubcategoryCombo();
  {
    QSignalBlocker blocker(_disSubcategoryCombo);
    const int index = _disSubcategoryCombo->findData(selectedEntry->entitySubcategory);
    if (index >= 0) {
      _disSubcategoryCombo->setCurrentIndex(index);
    }
  }
  this->populateDisSpecificCombo();
  {
    QSignalBlocker blocker(_disSpecificCombo);
    const int index = _disSpecificCombo->findData(selectedEntry->entitySpecific);
    if (index >= 0) {
      _disSpecificCombo->setCurrentIndex(index);
    }
  }
  this->populateDisExtraCombo();
  {
    QSignalBlocker blocker(_disExtraCombo);
    const int index = _disExtraCombo->findData(selectedEntry->entityExtra);
    if (index >= 0) {
      _disExtraCombo->setCurrentIndex(index);
    }
  }
}

void AddEntityDialog::syncDynamicsControls() {
  const bool enabled = _enableDynamicsCheck->isChecked();
  const bool useJsbsim = enabled && _dynamicsModeCombo->currentData().toString() == QStringLiteral("jsbsim");
  _dynamicsModeCombo->setEnabled(enabled);
  _speedSpin->setEnabled(enabled);
  _verticalSpeedSpin->setEnabled(enabled);
  _jsbsimModelCombo->setEnabled(useJsbsim);
}

Entity AddEntityDialog::entity() const {
  Entity entity;
  entity.name = _nameEdit->text().trimmed();
  entity.forceIdentifier = _forceIdentifierCombo->currentData().toInt();
  entity.domain = _domainCombo->currentText().trimmed().isEmpty()
                      ? QStringLiteral("Air")
                      : _domainCombo->currentText().trimmed();
  entity.category = _categoryCombo->currentText().trimmed().isEmpty()
                        ? QStringLiteral("Fighter")
                        : _categoryCombo->currentText().trimmed();
  entity.type = entity.category;
  entity.callsign = _callsignEdit->text().trimmed();
  entity.headingDegrees = _headingSpin->value();
  entity.flightDynamicsEnabled = _enableDynamicsCheck->isChecked();
  entity.flightDynamicsMode = entity.flightDynamicsEnabled
      ? _dynamicsModeCombo->currentData().toString()
      : QStringLiteral("kinematic");
  entity.jsbsimAircraftModel = entity.flightDynamicsEnabled &&
          entity.flightDynamicsMode == QStringLiteral("jsbsim")
      ? _jsbsimModelCombo->currentData().toString()
      : QString();
  entity.speedKnots = _speedSpin->value();
  entity.verticalSpeedMetersPerSecond = _verticalSpeedSpin->value();
  entity.currentTask.enabled = _enableFlightTaskCheck->isChecked();
  entity.currentTask.taskType = entity.currentTask.enabled
      ? QStringLiteral("FlyHeadingAltitudeSpeed")
      : QString();
  entity.currentTask.status = entity.currentTask.enabled
      ? QStringLiteral("Queued")
      : QStringLiteral("Idle");
  entity.currentTask.targetHeadingDegrees = _taskHeadingSpin->value();
  entity.currentTask.targetAltitudeMeters = _taskAltitudeSpin->value();
  entity.currentTask.targetSpeedKnots = _taskSpeedSpin->value();
  entity.latitude = _latitudeSpin->value();
  entity.longitude = _longitudeSpin->value();
  entity.groundHeight = _groundHeightSpin->value();
  entity.altitude = _altitudeSpin->value();
  entity.modelName = _modelCombo->currentText();
  const QString modelPath = _modelCombo->currentData().toString();
  entity.modelUri = modelPath.isEmpty() ? QString() : QUrl::fromLocalFile(modelPath).toString();
  entity.entityKind = _disKindCombo->currentData().toInt();
  entity.entityDomain = _disDomainCombo->currentData().toInt();
  entity.entityCountry = _disCountryCombo->currentData().toInt();
  entity.entityCategory = _disCategoryCombo->currentData().toInt();
  entity.entitySubcategory = _disSubcategoryCombo->currentData().toInt();
  entity.entitySpecific = _disSpecificCombo->currentData().toInt();
  entity.entityExtra = _disExtraCombo->currentData().toInt();

  const ModelCatalogEntry* selectedEntry = findSelectedModelEntry(
      _modelCatalog,
      entity.domain,
      entity.category,
      entity.modelName,
      modelPath);
  if (selectedEntry) {
    if (entity.modelName == selectedEntry->name && entity.type == entity.category) {
      entity.type = selectedEntry->name;
    }
  }

  entity.refreshEntityTypeCode();

  if (_addRadarCheck->isChecked()) {
    SensorDefinition radar;
    radar.id = QStringLiteral("%1-radar-primary").arg(entity.name.trimmed().isEmpty()
                                                          ? QStringLiteral("entity")
                                                          : entity.name.trimmed());
    radar.name = _radarNameEdit->text().trimmed().isEmpty()
        ? QStringLiteral("Primary Radar")
        : _radarNameEdit->text().trimmed();
    radar.sensorType = QStringLiteral("radar");
    radar.maxRangeMeters = _radarRangeSpin->value() * 1000.0;
    radar.azimuthWidthDegrees = _radarAzimuthSpin->value();
    radar.maxTracks = _radarMaxTracksSpin->value();
    radar.canDetectAir = true;
    radar.canDetectGround = true;
    radar.canDetectSurface = true;
    entity.sensors.push_back(radar);
  }

  return entity;
}

void AddEntityDialog::setPickedCoordinate(double longitude, double latitude, double height) {
  _longitudeSpin->setValue(longitude);
  _latitudeSpin->setValue(latitude);
  _groundHeightSpin->setValue(height);
}
