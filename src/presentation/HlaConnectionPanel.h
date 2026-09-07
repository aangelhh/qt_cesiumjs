#pragma once

#include "application/StartupConfiguration.h"

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

namespace presentation {

enum class HlaConnectionState {
  Disconnected,
  Connecting,
  Federated,
  Error
};

class HlaConnectionPanel final : public QWidget {
  Q_OBJECT

public:
  explicit HlaConnectionPanel(QWidget* parent = nullptr);

  void configure(
      const application::HlaStartupConfiguration& configuration,
      bool backendAvailable);
  void setConnectionState(
      HlaConnectionState state,
      const QString& detail = QString());

  HlaConnectionState connectionState() const;
  QString federationName() const;
  QString federateName() const;

signals:
  void connectRequested(
      const QString& federationName,
      const QString& federateName);
  void disconnectRequested();

private:
  void requestConnection();
  void updateControls();

  QLabel* _stateIndicator;
  QLabel* _stateLabel;
  QLabel* _detailLabel;
  QLabel* _backendValue;
  QLabel* _endpointValue;
  QLabel* _federateTypeValue;
  QLabel* _fomModulesValue;
  QLabel* _timeManagementValue;
  QLabel* _synchronizationPointValue;
  QLineEdit* _federationEdit;
  QLineEdit* _federateEdit;
  QPushButton* _connectButton;
  QPushButton* _disconnectButton;
  HlaConnectionState _state = HlaConnectionState::Disconnected;
  bool _backendAvailable = false;
};

} // namespace presentation
