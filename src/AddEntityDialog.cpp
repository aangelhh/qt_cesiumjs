#include "AddEntityDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
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

AddEntityDialog::AddEntityDialog(const QVector<ModelCatalogEntry>& modelCatalog, QWidget* parent)
    : QDialog(parent),
      _modelCatalog(modelCatalog),
      _nameEdit(new QLineEdit(this)),
      _callsignEdit(new QLineEdit(this)),
      _forceIdentifierCombo(new QComboBox(this)),
      _domainCombo(new QComboBox(this)),
      _categoryCombo(new QComboBox(this)),
      _modelCombo(new QComboBox(this)),
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

  _nameEdit->setPlaceholderText(QStringLiteral("Entity Alpha"));
  _callsignEdit->setPlaceholderText(QStringLiteral("Eagle 1"));

  _forceIdentifierCombo->addItem(QStringLiteral("Friendly"), 1);
  _forceIdentifierCombo->addItem(QStringLiteral("Opposing"), 2);
  _forceIdentifierCombo->addItem(QStringLiteral("Neutral"), 3);

  this->populateDomainCombo();
  this->populateCategoryCombo();
  this->populateModelCombo();

  QObject::connect(_domainCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->populateCategoryCombo();
    this->populateModelCombo();
  });
  QObject::connect(_categoryCombo, &QComboBox::currentTextChanged, this, [this]() {
    this->populateModelCombo();
  });

  formLayout->addRow(QStringLiteral("Name"), _nameEdit);
  formLayout->addRow(QStringLiteral("forceIdentifier"), _forceIdentifierCombo);
  formLayout->addRow(QStringLiteral("Domain"), _domainCombo);
  formLayout->addRow(QStringLiteral("Category"), _categoryCombo);
  formLayout->addRow(QStringLiteral("3D Model"), _modelCombo);
  formLayout->addRow(QStringLiteral("Callsign"), _callsignEdit);
  formLayout->addRow(QStringLiteral("Latitude"), _latitudeSpin);
  formLayout->addRow(QStringLiteral("Longitude"), _longitudeSpin);
  formLayout->addRow(QStringLiteral("Ground Height"), _groundHeightSpin);
  formLayout->addRow(QStringLiteral("Altitude"), _altitudeSpin);
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
  entity.latitude = _latitudeSpin->value();
  entity.longitude = _longitudeSpin->value();
  entity.groundHeight = _groundHeightSpin->value();
  entity.altitude = _altitudeSpin->value();
  entity.modelName = _modelCombo->currentText();
  const QString modelPath = _modelCombo->currentData().toString();
  entity.modelUri = modelPath.isEmpty() ? QString() : QUrl::fromLocalFile(modelPath).toString();

  const ModelCatalogEntry* selectedEntry = findSelectedModelEntry(
      _modelCatalog,
      entity.domain,
      entity.category,
      entity.modelName,
      modelPath);
  if (selectedEntry) {
    entity.entityKind = selectedEntry->entityKind;
    entity.entityDomain = selectedEntry->entityDomain;
    entity.entityCountry = selectedEntry->entityCountry;
    entity.entityCategory = selectedEntry->entityCategory;
    entity.entitySubcategory = selectedEntry->entitySubcategory;
    entity.entitySpecific = selectedEntry->entitySpecific;
    entity.entityExtra = selectedEntry->entityExtra;
  }

  return entity;
}

void AddEntityDialog::setPickedCoordinate(double longitude, double latitude, double height) {
  _longitudeSpin->setValue(longitude);
  _latitudeSpin->setValue(latitude);
  _groundHeightSpin->setValue(height);
}
