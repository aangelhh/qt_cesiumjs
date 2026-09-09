#pragma once

#include "application/StartupConfiguration.h"

#include <QDialog>
#include <QVector>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QStackedWidget;

namespace presentation {

struct HlaBackendOption {
  QString id;
  QString label;
  QString libraryPath;
};

class StartupConfigurationDialog final : public QDialog {
  Q_OBJECT

public:
  StartupConfigurationDialog(
      const application::StartupConfiguration& configuration,
      QVector<HlaBackendOption> hlaBackends,
      bool ros2Available,
      QWidget* parent = nullptr);

  application::StartupConfiguration configuration() const;

private slots:
  void updateModePage();
  void updateIntegrationControls();
  void addFomModules();
  void removeSelectedFomModules();
  void acceptConfiguration();

private:
  QComboBox* _modeCombo;
  QStackedWidget* _modeStack;
  QLineEdit* _disAddressEdit;
  QSpinBox* _disPortSpin;
  QSpinBox* _disExerciseSpin;
  QSpinBox* _disSiteSpin;
  QSpinBox* _disApplicationSpin;
  QDoubleSpinBox* _disTimeoutSpin;
  QComboBox* _hlaBackendCombo;
  QLineEdit* _hlaLocalSettingsEdit;
  QLineEdit* _hlaFederationEdit;
  QLineEdit* _hlaFederateEdit;
  QLineEdit* _hlaFederateTypeEdit;
  QLineEdit* _hlaSynchronizationPointEdit;
  QCheckBox* _hlaTimeManagementCheck;
  QDoubleSpinBox* _hlaLookaheadSpin;
  QCheckBox* _hlaCreateCheck;
  QListWidget* _hlaFomList;
  QPushButton* _removeFomButton;
  QCheckBox* _ros2Check;
  QSpinBox* _ros2DomainSpin;
  QLineEdit* _ros2TopicEdit;
  QCheckBox* _cigiCheck;
  QLineEdit* _cigiHostEdit;
  QSpinBox* _cigiPortSpin;
  QLabel* _validationLabel;
  bool _ros2Available;
};

} // namespace presentation
