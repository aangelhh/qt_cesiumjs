#pragma once

#include "domain/Entity.h"
#include "infrastructure/Ros2TelemetryPublisher.h"

#include <QDialog>
#include <QVector>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;

namespace presentation {

class Ros2TelemetryDialog final : public QDialog {
  Q_OBJECT

public:
  Ros2TelemetryDialog(
      const infrastructure::Ros2TelemetryConfiguration& configuration,
      const QVector<Entity>& entities,
      bool ros2Available,
      const QString& availabilityMessage,
      QWidget* parent = nullptr);

  infrastructure::Ros2TelemetryConfiguration configuration() const;

private:
  void chooseCsvPath();
  void updateCsvControls();

  QCheckBox* _enabledCheckBox;
  QLineEdit* _topicPrefixEdit;
  QComboBox* _entityComboBox;
  QDoubleSpinBox* _frequencySpinBox;
  QCheckBox* _csvCheckBox;
  QLineEdit* _csvPathEdit;
  class QPushButton* _csvBrowseButton;
};

} // namespace presentation
