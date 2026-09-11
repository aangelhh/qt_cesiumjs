#include "AddEntityDialog.h"

#include "application/SystemsTelemetry.h"
#include "infrastructure/JsbsimAircraftCatalog.h"
#include "infrastructure/JsbsimModelRepository.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QDir>
#include <QFormLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QSet>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTabWidget>
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

bool isGroundDomain(const QString& domain) {
  return domain.trimmed().compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0;
}

} // namespace

namespace {

QVector<JsbsimAircraftCatalogEntry> availableJsbsimAircraftModels() {
  return JsbsimAircraftCatalog::discoverModelRoots(
             JsbsimModelRepository::defaultModelRoots())
      .entries();
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

AddEntityDialog::AddEntityDialog(
    const QVector<ModelCatalogEntry>& modelCatalog,
    const QVector<infrastructure::SensorModelProviderEntry>& sensorModelProviders,
    QWidget* parent)
    : QDialog(parent),
      _modelCatalog(modelCatalog),
      _sensorModelProviders(sensorModelProviders),
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
      _radarModelProviderCombo(new QComboBox(this)),
      _radarProfileCombo(new QComboBox(this)),
      _headingSpin(new QDoubleSpinBox(this)),
      _enableDynamicsCheck(new QCheckBox(QStringLiteral("Enable flight dynamics"), this)),
      _dynamicsModeCombo(new QComboBox(this)),
      _jsbsimModelCombo(new QComboBox(this)),
      _fuelCapacitySpin(new QDoubleSpinBox(this)),
      _initialFuelSpin(new QDoubleSpinBox(this)),
      _speedSpin(new QDoubleSpinBox(this)),
      _verticalSpeedSpin(new QDoubleSpinBox(this)),
      _enableFlightTaskCheck(new QCheckBox(QStringLiteral("Assign initial flight task"), this)),
      _attachMissilesCheck(new QCheckBox(QStringLiteral("Attach missiles"), this)),
      _taskHeadingSpin(new QDoubleSpinBox(this)),
      _taskAltitudeSpin(new QSpinBox(this)),
      _taskSpeedSpin(new QDoubleSpinBox(this)),
      _missileCountSpin(new QSpinBox(this)),
      _radarRangeSpin(new QDoubleSpinBox(this)),
      _radarAzimuthSpin(new QDoubleSpinBox(this)),
      _radarElevationCenterSpin(new QDoubleSpinBox(this)),
      _radarElevationWidthSpin(new QDoubleSpinBox(this)),
      _radarDetectionProbabilitySpin(new QDoubleSpinBox(this)),
      _radarMaxTracksSpin(new QSpinBox(this)),
      _radarPeakPowerSpin(new QDoubleSpinBox(this)),
      _radarDutyCycleSpin(new QDoubleSpinBox(this)),
      _radarBandwidthSpin(new QDoubleSpinBox(this)),
      _radarReceiverNoiseSpin(new QDoubleSpinBox(this)),
      _radarFrequencySpin(new QDoubleSpinBox(this)),
      _radarAntennaGainSpin(new QDoubleSpinBox(this)),
      _radarBeamWidthSpin(new QDoubleSpinBox(this)),
      _radarNumberPulsesSpin(new QSpinBox(this)),
      _radarSystemLossSpin(new QDoubleSpinBox(this)),
      _radarFalseAlarmProbabilitySpin(new QDoubleSpinBox(this)),
      _radarRcsScaleSpin(new QDoubleSpinBox(this)),
      _radarSignatureSpin(new QDoubleSpinBox(this)),
      _thermalSignatureSpin(new QDoubleSpinBox(this)),
      _visualSignatureSpin(new QDoubleSpinBox(this)),
      _latitudeSpin(new QDoubleSpinBox(this)),
      _longitudeSpin(new QDoubleSpinBox(this)),
      _groundHeightSpin(new QDoubleSpinBox(this)),
      _altitudeSpin(new QSpinBox(this)),
      _pickOnMapButton(nullptr) {
  this->setAttribute(Qt::WA_DeleteOnClose);
  this->setWindowTitle(QStringLiteral("Add Entity"));
  this->setModal(false);
  this->setWindowModality(Qt::NonModal);
  this->resize(560, 720);
  this->setMinimumSize(480, 560);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(6);

  auto* tabs = new QTabWidget(this);

  auto* generalScrollArea = new QScrollArea(tabs);
  generalScrollArea->setWidgetResizable(true);
  generalScrollArea->setFrameShape(QFrame::NoFrame);

  auto* formContainer = new QWidget(generalScrollArea);
  auto* formLayout = new QFormLayout(formContainer);
  formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  formLayout->setContentsMargins(4, 4, 4, 4);
  formLayout->setHorizontalSpacing(10);
  formLayout->setVerticalSpacing(6);
  generalScrollArea->setWidget(formContainer);
  tabs->addTab(generalScrollArea, QStringLiteral("General"));

  auto* radarScrollArea = new QScrollArea(tabs);
  radarScrollArea->setWidgetResizable(true);
  radarScrollArea->setFrameShape(QFrame::NoFrame);
  auto* radarContainer = new QWidget(radarScrollArea);
  auto* radarLayout = new QFormLayout(radarContainer);
  radarLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  radarLayout->setContentsMargins(8, 8, 8, 8);
  radarLayout->setHorizontalSpacing(10);
  radarLayout->setVerticalSpacing(6);
  radarScrollArea->setWidget(radarContainer);
  tabs->addTab(radarScrollArea, QStringLiteral("Radar / MIXR"));

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

  _radarElevationCenterSpin->setRange(-90.0, 90.0);
  _radarElevationCenterSpin->setDecimals(1);
  _radarElevationCenterSpin->setSingleStep(5.0);
  _radarElevationCenterSpin->setSuffix(QStringLiteral(" deg"));
  _radarElevationCenterSpin->setValue(0.0);

  _radarElevationWidthSpin->setRange(1.0, 180.0);
  _radarElevationWidthSpin->setDecimals(1);
  _radarElevationWidthSpin->setSingleStep(5.0);
  _radarElevationWidthSpin->setSuffix(QStringLiteral(" deg"));
  _radarElevationWidthSpin->setValue(60.0);

  _radarMaxTracksSpin->setRange(1, 256);
  _radarMaxTracksSpin->setValue(16);

  for (const infrastructure::SensorModelProviderEntry& provider :
       _sensorModelProviders) {
    if (!provider.enabled || provider.id.trimmed().isEmpty()) {
      continue;
    }
    QString label = provider.displayName.trimmed().isEmpty()
        ? provider.id
        : provider.displayName;
    if (!provider.available) {
      label += QStringLiteral(" (not loaded; native fallback)");
    }
    _radarModelProviderCombo->addItem(label, provider.id);
    const int index = _radarModelProviderCombo->count() - 1;
    _radarModelProviderCombo->setItemData(
        index,
        QStringLiteral("Adapter: %1").arg(provider.adapterType),
        Qt::ToolTipRole);
    if (provider.defaultForNewSensors) {
      _radarModelProviderCombo->setCurrentIndex(index);
    }
  }
  if (_radarModelProviderCombo->count() == 0) {
    _radarModelProviderCombo->addItem(
        QStringLiteral("Native deterministic"),
        QStringLiteral("native"));
  }
  _radarModelProviderCombo->setObjectName(
      QStringLiteral("radarModelProviderCombo"));

  QStringList providerStatuses;
  for (const infrastructure::SensorModelProviderEntry& provider :
       _sensorModelProviders) {
    if (!provider.enabled || provider.id == QStringLiteral("native")) {
      continue;
    }
    const QString name = provider.displayName.trimmed().isEmpty()
        ? provider.id
        : provider.displayName;
    providerStatuses.push_back(QStringLiteral("%1: %2")
        .arg(name, provider.available
            ? QStringLiteral("loaded")
            : QStringLiteral("unavailable; native fallback")));
  }
  if (!providerStatuses.isEmpty()) {
    auto* providerStatusLabel = new QLabel(providerStatuses.join(QLatin1Char('\n')), this);
    providerStatusLabel->setWordWrap(true);
    providerStatusLabel->setObjectName(QStringLiteral("radarProviderStatusLabel"));
    radarLayout->addRow(QStringLiteral("Provider Status"), providerStatusLabel);
  }

  _radarProfileCombo->addItem(
      QStringLiteral("Generic Radar"), QStringLiteral("generic"));
  _radarProfileCombo->addItem(
      QStringLiteral("Fighter AESA"), QStringLiteral("fighter-aesa"));
  _radarProfileCombo->addItem(
      QStringLiteral("Ground Surveillance"),
      QStringLiteral("ground-surveillance"));
  _radarProfileCombo->addItem(
      QStringLiteral("Custom"), QStringLiteral("custom"));
  _radarProfileCombo->setCurrentIndex(
      _radarProfileCombo->findData(QStringLiteral("fighter-aesa")));

  _radarPeakPowerSpin->setRange(0.1, 5000.0);
  _radarPeakPowerSpin->setDecimals(1);
  _radarPeakPowerSpin->setSuffix(QStringLiteral(" kW"));
  _radarDutyCycleSpin->setRange(0.1, 100.0);
  _radarDutyCycleSpin->setDecimals(1);
  _radarDutyCycleSpin->setSuffix(QStringLiteral(" %"));
  _radarBandwidthSpin->setRange(0.001, 1000.0);
  _radarBandwidthSpin->setDecimals(3);
  _radarBandwidthSpin->setSuffix(QStringLiteral(" MHz"));
  _radarReceiverNoiseSpin->setRange(0.0, 30.0);
  _radarReceiverNoiseSpin->setDecimals(2);
  _radarReceiverNoiseSpin->setSuffix(QStringLiteral(" dB"));
  _radarFrequencySpin->setRange(0.01, 100.0);
  _radarFrequencySpin->setDecimals(3);
  _radarFrequencySpin->setSuffix(QStringLiteral(" GHz"));
  _radarAntennaGainSpin->setRange(-20.0, 100.0);
  _radarAntennaGainSpin->setDecimals(2);
  _radarAntennaGainSpin->setSuffix(QStringLiteral(" dB"));
  _radarBeamWidthSpin->setRange(0.1, 360.0);
  _radarBeamWidthSpin->setDecimals(1);
  _radarBeamWidthSpin->setSuffix(QStringLiteral(" deg"));
  _radarNumberPulsesSpin->setRange(1, 100000);
  _radarSystemLossSpin->setRange(0.0, 100.0);
  _radarSystemLossSpin->setDecimals(2);
  _radarSystemLossSpin->setSuffix(QStringLiteral(" dB"));
  _radarFalseAlarmProbabilitySpin->setRange(1.0e-12, 0.1);
  _radarFalseAlarmProbabilitySpin->setDecimals(12);
  _radarFalseAlarmProbabilitySpin->setSingleStep(1.0e-6);
  _radarRcsScaleSpin->setRange(0.01, 1000.0);
  _radarRcsScaleSpin->setDecimals(3);
  _radarRcsScaleSpin->setSuffix(QStringLiteral(" m2"));
  this->applyRadarProfilePreset(QStringLiteral("fighter-aesa"));

  _radarDetectionProbabilitySpin->setRange(0.0, 100.0);
  _radarDetectionProbabilitySpin->setDecimals(1);
  _radarDetectionProbabilitySpin->setSingleStep(1.0);
  _radarDetectionProbabilitySpin->setSuffix(QStringLiteral(" %"));
  _radarDetectionProbabilitySpin->setValue(95.0);

  const auto configureSignatureSpin = [](QDoubleSpinBox* spin) {
    spin->setRange(0.0, 2.0);
    spin->setDecimals(2);
    spin->setSingleStep(0.1);
    spin->setValue(1.0);
  };
  configureSignatureSpin(_radarSignatureSpin);
  configureSignatureSpin(_thermalSignatureSpin);
  configureSignatureSpin(_visualSignatureSpin);

  _speedSpin->setRange(0.0, 1200.0);
  _speedSpin->setDecimals(1);
  _speedSpin->setSingleStep(10.0);
  _speedSpin->setSuffix(QStringLiteral(" kts"));
  _speedSpin->setValue(320.0);

  _verticalSpeedSpin->setRange(-200.0, 200.0);
  _verticalSpeedSpin->setDecimals(1);
  _verticalSpeedSpin->setSingleStep(1.0);
  _verticalSpeedSpin->setSuffix(QStringLiteral(" m/s"));

  _fuelCapacitySpin->setRange(0.0, 100000.0);
  _fuelCapacitySpin->setDecimals(0);
  _fuelCapacitySpin->setSingleStep(100.0);
  _fuelCapacitySpin->setSuffix(QStringLiteral(" kg"));

  _initialFuelSpin->setRange(0.0, 100000.0);
  _initialFuelSpin->setDecimals(0);
  _initialFuelSpin->setSingleStep(100.0);
  _initialFuelSpin->setSuffix(QStringLiteral(" kg"));

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

  _missileCountSpin->setRange(1, 12);
  _missileCountSpin->setValue(2);

  _nameEdit->setPlaceholderText(QStringLiteral("Entity Alpha"));
  _callsignEdit->setPlaceholderText(QStringLiteral("Eagle 1"));
  _radarNameEdit->setPlaceholderText(QStringLiteral("Primary Radar"));
  _addRadarCheck->setChecked(false);
  _enableDynamicsCheck->setChecked(true);
  _attachMissilesCheck->setChecked(false);

  _forceIdentifierCombo->addItem(QStringLiteral("Friendly"), 1);
  _forceIdentifierCombo->addItem(QStringLiteral("Opposing"), 2);
  _forceIdentifierCombo->addItem(QStringLiteral("Neutral"), 3);

  _dynamicsModeCombo->addItem(QStringLiteral("Kinematic"), QStringLiteral("kinematic"));
#if defined(QTTEST_HAS_JSBSIM)
  _dynamicsModeCombo->addItem(QStringLiteral("JSBSim"), QStringLiteral("jsbsim"));
  {
    const int jsbsimIndex = _dynamicsModeCombo->findData(QStringLiteral("jsbsim"));
    if (jsbsimIndex >= 0) {
      _dynamicsModeCombo->setCurrentIndex(jsbsimIndex);
    }
  }
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
    this->syncDynamicsControls();
    this->syncFuelControls();
    this->syncWeaponControls();
  });
  QObject::connect(_categoryCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->populateModelCombo();
    this->populateJsbsimModelCombo();
    this->syncDynamicsControls();
    this->syncFuelControls();
    this->syncWeaponControls();
  });
  QObject::connect(_modelCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->applyModelSelectionToDisFields();
    this->populateJsbsimModelCombo();
    this->syncDynamicsControls();
    this->syncFuelControls();
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
    Q_UNUSED(enabled)
    this->syncRadarControls();
  });
  QObject::connect(
      _radarProfileCombo,
      &QComboBox::currentIndexChanged,
      this,
      [this](int) {
        this->applyRadarProfilePreset(
            _radarProfileCombo->currentData().toString());
      });
  const auto markRadarProfileCustom = [this]() {
    if (_applyingRadarProfilePreset) {
      return;
    }
    const int customIndex =
        _radarProfileCombo->findData(QStringLiteral("custom"));
    if (customIndex >= 0 && _radarProfileCombo->currentIndex() != customIndex) {
      _radarProfileCombo->setCurrentIndex(customIndex);
    }
  };
  const QList<QDoubleSpinBox*> radarProfileDoubleSpins{
      _radarPeakPowerSpin,
      _radarDutyCycleSpin,
      _radarBandwidthSpin,
      _radarReceiverNoiseSpin,
      _radarFrequencySpin,
      _radarAntennaGainSpin,
      _radarBeamWidthSpin,
      _radarSystemLossSpin,
      _radarFalseAlarmProbabilitySpin,
      _radarRcsScaleSpin,
  };
  for (QDoubleSpinBox* spin : radarProfileDoubleSpins) {
    QObject::connect(
        spin,
        &QDoubleSpinBox::valueChanged,
        this,
        [markRadarProfileCustom](double) { markRadarProfileCustom(); });
  }
  QObject::connect(
      _radarNumberPulsesSpin,
      &QSpinBox::valueChanged,
      this,
      [markRadarProfileCustom](int) { markRadarProfileCustom(); });
  QObject::connect(_enableDynamicsCheck, &QCheckBox::toggled, this, [this](bool enabled) {
    Q_UNUSED(enabled)
    this->syncDynamicsControls();
  });
  QObject::connect(_dynamicsModeCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->syncDynamicsControls();
  });
  QObject::connect(_jsbsimModelCombo, &QComboBox::currentIndexChanged, this, [this](int) {
    this->syncFuelControls();
  });
  QObject::connect(
      _fuelCapacitySpin,
      &QDoubleSpinBox::valueChanged,
      this,
      [this](double capacityKilograms) {
        _initialFuelSpin->setMaximum(capacityKilograms);
      });
  QObject::connect(_enableFlightTaskCheck, &QCheckBox::toggled, this, [this](bool enabled) {
    _taskHeadingSpin->setEnabled(enabled);
    _taskAltitudeSpin->setEnabled(enabled);
    _taskSpeedSpin->setEnabled(enabled);
  });
  QObject::connect(_attachMissilesCheck, &QCheckBox::toggled, this, [this](bool enabled) {
    Q_UNUSED(enabled)
    this->syncWeaponControls();
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
  formLayout->addRow(QStringLiteral("Fuel Capacity"), _fuelCapacitySpin);
  formLayout->addRow(QStringLiteral("Initial Fuel"), _initialFuelSpin);
  formLayout->addRow(QStringLiteral("Speed"), _speedSpin);
  formLayout->addRow(QStringLiteral("Vertical Speed"), _verticalSpeedSpin);
  formLayout->addRow(QStringLiteral("Initial Task"), _enableFlightTaskCheck);
  formLayout->addRow(QStringLiteral("Initial Task Heading"), _taskHeadingSpin);
  formLayout->addRow(QStringLiteral("Initial Task Altitude"), _taskAltitudeSpin);
  formLayout->addRow(QStringLiteral("Initial Task Speed"), _taskSpeedSpin);
  formLayout->addRow(QStringLiteral("Weapons"), _attachMissilesCheck);
  formLayout->addRow(QStringLiteral("Missile Count"), _missileCountSpin);
  radarLayout->addRow(QStringLiteral("Enabled"), _addRadarCheck);
  radarLayout->addRow(QStringLiteral("Name"), _radarNameEdit);
  radarLayout->addRow(QStringLiteral("Model Provider"), _radarModelProviderCombo);
  radarLayout->addRow(QStringLiteral("Profile"), _radarProfileCombo);
  radarLayout->addRow(QStringLiteral("Range"), _radarRangeSpin);
  radarLayout->addRow(QStringLiteral("Azimuth"), _radarAzimuthSpin);
  radarLayout->addRow(QStringLiteral("Elevation Center"), _radarElevationCenterSpin);
  radarLayout->addRow(QStringLiteral("Elevation Width"), _radarElevationWidthSpin);
  radarLayout->addRow(QStringLiteral("Detection Probability"), _radarDetectionProbabilitySpin);
  radarLayout->addRow(QStringLiteral("Max Tracks"), _radarMaxTracksSpin);
  radarLayout->addRow(QStringLiteral("Peak Power"), _radarPeakPowerSpin);
  radarLayout->addRow(QStringLiteral("Duty Cycle"), _radarDutyCycleSpin);
  radarLayout->addRow(QStringLiteral("Bandwidth"), _radarBandwidthSpin);
  radarLayout->addRow(QStringLiteral("Receiver Noise"), _radarReceiverNoiseSpin);
  radarLayout->addRow(QStringLiteral("Frequency"), _radarFrequencySpin);
  radarLayout->addRow(QStringLiteral("Antenna Gain"), _radarAntennaGainSpin);
  radarLayout->addRow(QStringLiteral("Beam Width"), _radarBeamWidthSpin);
  radarLayout->addRow(QStringLiteral("Pulses per Evaluation"), _radarNumberPulsesSpin);
  radarLayout->addRow(QStringLiteral("System Loss"), _radarSystemLossSpin);
  radarLayout->addRow(QStringLiteral("Probability False Alarm"), _radarFalseAlarmProbabilitySpin);
  radarLayout->addRow(QStringLiteral("RCS Scale"), _radarRcsScaleSpin);
  radarLayout->addRow(QStringLiteral("Radar Signature"), _radarSignatureSpin);
  formLayout->addRow(QStringLiteral("Thermal Signature"), _thermalSignatureSpin);
  formLayout->addRow(QStringLiteral("Visual Signature"), _visualSignatureSpin);
  formLayout->addRow(QStringLiteral("Latitude"), _latitudeSpin);
  formLayout->addRow(QStringLiteral("Longitude"), _longitudeSpin);
  formLayout->addRow(QStringLiteral("Ground Height"), _groundHeightSpin);
  formLayout->addRow(QStringLiteral("Altitude"), _altitudeSpin);
  if (!_disCatalog.isLoaded()) {
    auto* warningLabel = new QLabel(
        QStringLiteral("DIS catalog unavailable: %1").arg(_disCatalog.errorString()),
        this);
    warningLabel->setWordWrap(true);
    formLayout->addRow(warningLabel);
  }
  this->syncRadarControls();
  this->syncDynamicsControls();
  this->syncFuelControls();
  this->syncWeaponControls();
  _taskHeadingSpin->setEnabled(_enableFlightTaskCheck->isChecked());
  _taskAltitudeSpin->setEnabled(_enableFlightTaskCheck->isChecked());
  _taskSpeedSpin->setEnabled(_enableFlightTaskCheck->isChecked());
  layout->addWidget(tabs);

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
  domains.insert(QStringLiteral("Air"));
  domains.insert(QStringLiteral("Ground"));

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
    if (isGroundDomain(currentDomain)) {
      categories.insert(QStringLiteral("Tank"));
      categories.insert(QStringLiteral("Truck"));
      categories.insert(QStringLiteral("ArmoredVehicle"));
      categories.insert(QStringLiteral("Radar"));
      categories.insert(QStringLiteral("SAMLauncher"));
    } else {
      categories.insert(QStringLiteral("Fighter"));
    }
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
  _jsbsimModelCombo->clear();

  const QVector<JsbsimAircraftCatalogEntry> models =
      availableJsbsimAircraftModels();
  for (const JsbsimAircraftCatalogEntry& model : models) {
    const QString label = model.displayName.compare(
        model.modelId, Qt::CaseInsensitive) == 0
        ? model.modelId
        : QStringLiteral("%1 (%2)").arg(model.displayName, model.modelId);
    _jsbsimModelCombo->addItem(label, model.modelId);
  }

  const ModelCatalogEntry* selectedEntry = findSelectedModelEntry(
      _modelCatalog,
      _domainCombo->currentText().trimmed(),
      _categoryCombo->currentText().trimmed(),
      _modelCombo->currentText(),
      _modelCombo->currentData().toString());

  QString preferred = selectedEntry
      ? selectedEntry->jsbsimAircraftModel.trimmed()
      : QString();
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

  const QString configuredBackend = selectedEntry->dynamicsBackend.trimmed().toLower();
  if (!configuredBackend.isEmpty() && !isGroundDomain(_domainCombo->currentText())) {
    const int backendIndex = _dynamicsModeCombo->findData(configuredBackend);
    if (backendIndex >= 0) {
      QSignalBlocker enabledBlocker(_enableDynamicsCheck);
      QSignalBlocker modeBlocker(_dynamicsModeCombo);
      _enableDynamicsCheck->setChecked(true);
      _dynamicsModeCombo->setCurrentIndex(backendIndex);
    }
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
  const bool isGround = isGroundDomain(_domainCombo->currentText());
  if (isGround) {
    QSignalBlocker dynamicsBlocker(_enableDynamicsCheck);
    QSignalBlocker taskBlocker(_enableFlightTaskCheck);
    _enableDynamicsCheck->setChecked(false);
    _enableFlightTaskCheck->setChecked(false);
    _altitudeSpin->setValue(0);
    _taskAltitudeSpin->setValue(0);
    _verticalSpeedSpin->setValue(0.0);
  }

  const bool enabled = !isGround && _enableDynamicsCheck->isChecked();
  const bool useJsbsim = enabled && _dynamicsModeCombo->currentData().toString() == QStringLiteral("jsbsim");
  if (isGround) {
    _speedSpin->setRange(0.0, 80.0);
    _speedSpin->setValue(0.0);
  } else {
    _speedSpin->setRange(0.0, 1200.0);
  }
  _enableDynamicsCheck->setEnabled(!isGround);
  _enableFlightTaskCheck->setEnabled(!isGround);
  _dynamicsModeCombo->setEnabled(enabled);
  _speedSpin->setEnabled(isGround || enabled);
  _verticalSpeedSpin->setEnabled(enabled);
  _jsbsimModelCombo->setEnabled(useJsbsim);
  _taskHeadingSpin->setEnabled(!isGround && _enableFlightTaskCheck->isChecked());
  _taskAltitudeSpin->setEnabled(!isGround && _enableFlightTaskCheck->isChecked());
  _taskSpeedSpin->setEnabled(!isGround && _enableFlightTaskCheck->isChecked());
  _altitudeSpin->setEnabled(!isGround);
}

void AddEntityDialog::syncWeaponControls() {
  const bool isGround = isGroundDomain(_domainCombo->currentText());
  if (isGround) {
    QSignalBlocker blocker(_attachMissilesCheck);
    _attachMissilesCheck->setChecked(false);
  }
  _attachMissilesCheck->setEnabled(!isGround);
  _missileCountSpin->setEnabled(!isGround && _attachMissilesCheck->isChecked());
}

void AddEntityDialog::syncFuelControls() {
  const QString domain = _domainCombo->currentText().trimmed();
  const bool isAir =
      domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0;
  if (!isAir) {
    QSignalBlocker capacityBlocker(_fuelCapacitySpin);
    QSignalBlocker initialBlocker(_initialFuelSpin);
    _fuelCapacitySpin->setValue(0.0);
    _initialFuelSpin->setMaximum(0.0);
    _initialFuelSpin->setValue(0.0);
    _fuelCapacitySpin->setEnabled(false);
    _initialFuelSpin->setEnabled(false);
    return;
  }

  Entity defaults;
  defaults.domain = domain;
  defaults.category = _categoryCombo->currentText().trimmed();
  defaults.modelName = _modelCombo->currentText();
  defaults.jsbsimAircraftModel = _jsbsimModelCombo->currentData().toString();

  const QString modelPath = _modelCombo->currentData().toString();
  const ModelCatalogEntry* selectedEntry = findSelectedModelEntry(
      _modelCatalog,
      defaults.domain,
      defaults.category,
      defaults.modelName,
      modelPath);
  const double capacityKilograms = selectedEntry &&
          selectedEntry->fuelCapacityKilograms > 0.0
      ? selectedEntry->fuelCapacityKilograms
      : application::defaultFuelCapacityKilograms(defaults);
  const double initialKilograms = selectedEntry &&
          selectedEntry->initialFuelKilograms > 0.0
      ? selectedEntry->initialFuelKilograms
      : application::defaultInitialFuelKilograms(defaults);

  QSignalBlocker capacityBlocker(_fuelCapacitySpin);
  QSignalBlocker initialBlocker(_initialFuelSpin);
  _fuelCapacitySpin->setEnabled(true);
  _initialFuelSpin->setEnabled(true);
  _fuelCapacitySpin->setValue(capacityKilograms);
  _initialFuelSpin->setMaximum(capacityKilograms);
  _initialFuelSpin->setValue(qMin(initialKilograms, capacityKilograms));
}

void AddEntityDialog::syncRadarControls() {
  const bool enabled = _addRadarCheck->isChecked();
  const QList<QWidget*> controls{
      _radarNameEdit,
      _radarRangeSpin,
      _radarAzimuthSpin,
      _radarElevationCenterSpin,
      _radarElevationWidthSpin,
      _radarDetectionProbabilitySpin,
      _radarMaxTracksSpin,
      _radarPeakPowerSpin,
      _radarDutyCycleSpin,
      _radarBandwidthSpin,
      _radarReceiverNoiseSpin,
      _radarFrequencySpin,
      _radarAntennaGainSpin,
      _radarBeamWidthSpin,
      _radarNumberPulsesSpin,
      _radarSystemLossSpin,
      _radarFalseAlarmProbabilitySpin,
      _radarRcsScaleSpin,
  };
  for (QWidget* control : controls) {
    control->setEnabled(enabled);
  }
  _radarModelProviderCombo->setEnabled(true);
  _radarProfileCombo->setEnabled(true);
}

void AddEntityDialog::applyRadarProfilePreset(const QString& profileId) {
  if (profileId == QStringLiteral("custom")) {
    return;
  }

  RadarProfile profile;
  if (profileId == QStringLiteral("fighter-aesa")) {
    profile.profileId = profileId;
  } else if (profileId == QStringLiteral("ground-surveillance")) {
    profile.profileId = profileId;
    profile.peakPowerWatts = 100000.0;
    profile.dutyCycle = 0.2;
    profile.bandwidthHertz = 2.0e6;
    profile.receiverNoiseDecibels = 4.0;
    profile.frequencyHertz = 3.0e9;
    profile.antennaGainDecibels = 38.0;
    profile.beamWidthDegrees = 360.0;
    profile.numberPulses = 32;
    profile.systemLossDecibels = 8.0;
  } else {
    profile.profileId = QStringLiteral("generic");
    profile.peakPowerWatts = 20000.0;
    profile.dutyCycle = 0.08;
    profile.receiverNoiseDecibels = 4.0;
    profile.antennaGainDecibels = 32.0;
    profile.numberPulses = 8;
    profile.systemLossDecibels = 8.0;
  }

  _applyingRadarProfilePreset = true;
  _radarPeakPowerSpin->setValue(profile.peakPowerWatts / 1000.0);
  _radarDutyCycleSpin->setValue(profile.dutyCycle * 100.0);
  _radarBandwidthSpin->setValue(profile.bandwidthHertz / 1.0e6);
  _radarReceiverNoiseSpin->setValue(profile.receiverNoiseDecibels);
  _radarFrequencySpin->setValue(profile.frequencyHertz / 1.0e9);
  _radarAntennaGainSpin->setValue(profile.antennaGainDecibels);
  _radarBeamWidthSpin->setValue(profile.beamWidthDegrees);
  _radarNumberPulsesSpin->setValue(profile.numberPulses);
  _radarSystemLossSpin->setValue(profile.systemLossDecibels);
  _radarFalseAlarmProbabilitySpin->setValue(
      profile.probabilityFalseAlarm);
  _radarRcsScaleSpin->setValue(profile.rcsScaleSquareMeters);
  _applyingRadarProfilePreset = false;
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
  const bool isGround = isGroundDomain(entity.domain);
  const bool isAir =
      entity.domain.compare(QStringLiteral("Air"), Qt::CaseInsensitive) == 0;
  entity.flightDynamicsEnabled = !isGround && _enableDynamicsCheck->isChecked();
  entity.flightDynamicsMode = entity.flightDynamicsEnabled
      ? _dynamicsModeCombo->currentData().toString()
      : QStringLiteral("kinematic");
  entity.jsbsimAircraftModel = entity.flightDynamicsEnabled &&
          entity.flightDynamicsMode == QStringLiteral("jsbsim")
      ? _jsbsimModelCombo->currentData().toString()
      : QString();
  entity.fuelCapacityKilograms = isAir ? _fuelCapacitySpin->value() : 0.0;
  entity.fuelRemainingKilograms = isAir ? _initialFuelSpin->value() : 0.0;
  entity.speedKnots = _speedSpin->value();
  entity.verticalSpeedMetersPerSecond = isGround ? 0.0 : _verticalSpeedSpin->value();
  entity.currentTask.enabled = !isGround && _enableFlightTaskCheck->isChecked();
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
  entity.altitude = isGround ? 0 : _altitudeSpin->value();
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
    if (!entity.jsbsimAircraftModel.isEmpty()) {
      entity.dynamicsModelCompatibility =
          entity.jsbsimAircraftModel == selectedEntry->jsbsimAircraftModel
          ? selectedEntry->dynamicsModelCompatibility
          : QStringLiteral("custom");
    }
    entity.controlProfileId = selectedEntry->controlProfileId;
    entity.systemsDisplayProfileId = selectedEntry->systemsDisplayProfileId;
    entity.cesiumModelAxes = selectedEntry->cesiumAxes;
    entity.engineCount = selectedEntry->engineCount;
    if (entity.modelName == selectedEntry->name && entity.type == entity.category) {
      entity.type = selectedEntry->name;
    }
  }
  if (!entity.jsbsimAircraftModel.isEmpty() &&
      entity.dynamicsModelCompatibility.isEmpty()) {
    entity.dynamicsModelCompatibility = QStringLiteral("custom");
  }
  if (entity.engineCount <= 0) {
    entity.engineCount = application::engineCountForEntity(entity);
  }
  if (entity.systemsDisplayProfileId.isEmpty()) {
    entity.systemsDisplayProfileId =
        application::systemsDisplayProfileForEntity(entity);
  }

  if (_attachMissilesCheck->isChecked()) {
    entity.weapons.push_back(
        WeaponInventoryItem{QStringLiteral("Missile"), _missileCountSpin->value()});
  }

  entity.refreshEntityTypeCode();

  entity.radarSignature = _radarSignatureSpin->value();
  entity.thermalSignature = _thermalSignatureSpin->value();
  entity.visualSignature = _visualSignatureSpin->value();

  if (_addRadarCheck->isChecked()) {
    SensorDefinition radar;
    radar.id = QStringLiteral("%1-radar-primary").arg(entity.name.trimmed().isEmpty()
                                                          ? QStringLiteral("entity")
                                                          : entity.name.trimmed());
    radar.name = _radarNameEdit->text().trimmed().isEmpty()
        ? QStringLiteral("Primary Radar")
        : _radarNameEdit->text().trimmed();
    radar.modelProviderId = _radarModelProviderCombo->currentData()
        .toString().trimmed().toLower();
    if (radar.modelProviderId.isEmpty()) {
      radar.modelProviderId = QStringLiteral("native");
    }
    radar.sensorType = QStringLiteral("radar");
    radar.sensorSubType = isGround
        ? QStringLiteral("groundRadar")
        : QStringLiteral("airborneRadar");
    radar.maxRangeMeters = _radarRangeSpin->value() * 1000.0;
    radar.azimuthWidthDegrees = _radarAzimuthSpin->value();
    radar.elevationCenterDegrees = _radarElevationCenterSpin->value();
    radar.elevationWidthDegrees = _radarElevationWidthSpin->value();
    radar.probabilityOfDetection =
        _radarDetectionProbabilitySpin->value() / 100.0;
    radar.maxTracks = _radarMaxTracksSpin->value();
    radar.radarProfile.profileId =
        _radarProfileCombo->currentData().toString();
    radar.radarProfile.peakPowerWatts =
        _radarPeakPowerSpin->value() * 1000.0;
    radar.radarProfile.dutyCycle =
        _radarDutyCycleSpin->value() / 100.0;
    radar.radarProfile.bandwidthHertz =
        _radarBandwidthSpin->value() * 1.0e6;
    radar.radarProfile.receiverNoiseDecibels =
        _radarReceiverNoiseSpin->value();
    radar.radarProfile.frequencyHertz =
        _radarFrequencySpin->value() * 1.0e9;
    radar.radarProfile.antennaGainDecibels =
        _radarAntennaGainSpin->value();
    radar.radarProfile.beamWidthDegrees = _radarBeamWidthSpin->value();
    radar.radarProfile.numberPulses = _radarNumberPulsesSpin->value();
    radar.radarProfile.systemLossDecibels =
        _radarSystemLossSpin->value();
    radar.radarProfile.probabilityFalseAlarm =
        _radarFalseAlarmProbabilitySpin->value();
    radar.radarProfile.rcsScaleSquareMeters = _radarRcsScaleSpin->value();
    radar.canDetectAir = true;
    radar.canDetectGround = false;
    radar.canDetectSurface = false;
    entity.sensors.push_back(radar);
  }

  return entity;
}

void AddEntityDialog::setPickedCoordinate(double longitude, double latitude, double height) {
  _longitudeSpin->setValue(longitude);
  _latitudeSpin->setValue(latitude);
  _groundHeightSpin->setValue(height);
}
