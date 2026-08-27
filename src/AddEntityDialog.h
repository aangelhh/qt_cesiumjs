#pragma once

#include "domain/Entity.h"
#include "infrastructure/DisEntityCatalog.h"
#include "infrastructure/ModelCatalog.h"
#include "infrastructure/SensorModelProviderCatalog.h"

#include <QDialog>
#include <QVector>

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class AddEntityDialog : public QDialog {
  Q_OBJECT

public:
  explicit AddEntityDialog(
      const QVector<ModelCatalogEntry>& modelCatalog,
      const QVector<infrastructure::SensorModelProviderEntry>& sensorModelProviders,
      QWidget* parent = nullptr);

  Entity entity() const;
  void setPickedCoordinate(double longitude, double latitude, double height);

signals:
  void pickOnMapRequested();

private:
  void populateDomainCombo();
  void populateCategoryCombo();
  void populateModelCombo();
  void populateDisKindCombo();
  void populateDisDomainCombo();
  void populateDisCountryCombo();
  void populateDisCategoryCombo();
  void populateDisSubcategoryCombo();
  void populateDisSpecificCombo();
  void populateDisExtraCombo();
  void populateJsbsimModelCombo();
  void syncModelFromDisSelection();
  void applyModelSelectionToDisFields();
  void syncDynamicsControls();
  void syncFuelControls();
  void syncWeaponControls();

  QVector<ModelCatalogEntry> _modelCatalog;
  QVector<infrastructure::SensorModelProviderEntry> _sensorModelProviders;
  DisEntityCatalog _disCatalog;
  QLineEdit* _nameEdit;
  QLineEdit* _callsignEdit;
  QComboBox* _forceIdentifierCombo;
  QComboBox* _domainCombo;
  QComboBox* _categoryCombo;
  QComboBox* _modelCombo;
  QComboBox* _disKindCombo;
  QComboBox* _disDomainCombo;
  QComboBox* _disCountryCombo;
  QComboBox* _disCategoryCombo;
  QComboBox* _disSubcategoryCombo;
  QComboBox* _disSpecificCombo;
  QComboBox* _disExtraCombo;
  QCheckBox* _addRadarCheck;
  QLineEdit* _radarNameEdit;
  QComboBox* _radarModelProviderCombo;
  QDoubleSpinBox* _headingSpin;
  QCheckBox* _enableDynamicsCheck;
  QComboBox* _dynamicsModeCombo;
  QComboBox* _jsbsimModelCombo;
  QDoubleSpinBox* _fuelCapacitySpin;
  QDoubleSpinBox* _initialFuelSpin;
  QDoubleSpinBox* _speedSpin;
  QDoubleSpinBox* _verticalSpeedSpin;
  QCheckBox* _enableFlightTaskCheck;
  QCheckBox* _attachMissilesCheck;
  QDoubleSpinBox* _taskHeadingSpin;
  QSpinBox* _taskAltitudeSpin;
  QDoubleSpinBox* _taskSpeedSpin;
  QSpinBox* _missileCountSpin;
  QDoubleSpinBox* _radarRangeSpin;
  QDoubleSpinBox* _radarAzimuthSpin;
  QDoubleSpinBox* _radarElevationCenterSpin;
  QDoubleSpinBox* _radarElevationWidthSpin;
  QDoubleSpinBox* _radarDetectionProbabilitySpin;
  QSpinBox* _radarMaxTracksSpin;
  QDoubleSpinBox* _radarSignatureSpin;
  QDoubleSpinBox* _thermalSignatureSpin;
  QDoubleSpinBox* _visualSignatureSpin;
  QDoubleSpinBox* _latitudeSpin;
  QDoubleSpinBox* _longitudeSpin;
  QDoubleSpinBox* _groundHeightSpin;
  QSpinBox* _altitudeSpin;
  QPushButton* _pickOnMapButton;
};
