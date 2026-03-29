#pragma once

#include "domain/Entity.h"
#include "infrastructure/DisEntityCatalog.h"
#include "infrastructure/ModelCatalog.h"

#include <QDialog>
#include <QVector>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class AddEntityDialog : public QDialog {
  Q_OBJECT

public:
  explicit AddEntityDialog(const QVector<ModelCatalogEntry>& modelCatalog, QWidget* parent = nullptr);

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
  void syncModelFromDisSelection();
  void applyModelSelectionToDisFields();

  QVector<ModelCatalogEntry> _modelCatalog;
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
  QDoubleSpinBox* _latitudeSpin;
  QDoubleSpinBox* _longitudeSpin;
  QDoubleSpinBox* _groundHeightSpin;
  QSpinBox* _altitudeSpin;
  QPushButton* _pickOnMapButton;
};
