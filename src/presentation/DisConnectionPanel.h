#pragma once

#include "application/StartupConfiguration.h"
#include "infrastructure/interoperability/dis/DisTypes.h"

#include <QWidget>

#include <cstdint>
#include <vector>

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QWidget;

namespace presentation {

enum class DisConnectionState {
  Unavailable,
  Disconnected,
  Connecting,
  Active,
  Error
};

class DisConnectionPanel final : public QWidget {
  Q_OBJECT

public:
  explicit DisConnectionPanel(QWidget* parent = nullptr);

  void configure(
      const application::DisStartupConfiguration& configuration,
      bool backendAvailable,
      bool blockedByHla);
  void setConnectionState(
      DisConnectionState state,
      const QString& detail = QString());
  void setBlockedByHla(bool blocked);
  void updateStatistics(
      std::uint64_t transmittedPdus,
      std::uint64_t receivedPdus,
      qsizetype remoteEntities);
  void recordRemoteEntityChanges(
      const std::vector<tactical::dis::RemoteEntityChange>& changes);
  void recordRemoteWarfareEvents(
      const std::vector<tactical::dis::WarfareEvent>& events);
  DisConnectionState connectionState() const;
  application::DisStartupConfiguration configuration() const { return connectionConfiguration(); }
  void recordInteractions(const std::vector<tactical::dis::IffState>& iff,
      const std::vector<tactical::dis::CollisionEvent>& collisions);

signals:
  void connectRequested(const application::DisStartupConfiguration& configuration);
  void disconnectRequested();
  void testEntityStateRequested(const tactical::dis::EntityState& state);
  void entityManagementRequested(const tactical::dis::EntityManagementRequest& request);
  void iffRequested(const tactical::dis::IffState& state);
  void collisionRequested(const tactical::dis::CollisionEvent& event);

private:
  application::DisStartupConfiguration connectionConfiguration() const;
  tactical::dis::EntityState testEntityState() const;
  void requestConnection();
  void requestTestTransmission();
  void updateControls();

  QLabel* _stateIndicator;
  QLabel* _stateLabel;
  QLabel* _detailLabel;
  QLabel* _txValue;
  QLabel* _rxValue;
  QLabel* _remoteValue;
  QWidget* _connectionControls;
  QWidget* _testControls;
  QLineEdit* _addressEdit;
  QSpinBox* _portSpin;
  QSpinBox* _exerciseSpin;
  QSpinBox* _siteSpin;
  QSpinBox* _applicationSpin;
  QPushButton* _connectButton;
  QPushButton* _disconnectButton;
  QSpinBox* _entitySpin;
  QLineEdit* _markingEdit;
  QDoubleSpinBox* _latitudeSpin;
  QDoubleSpinBox* _longitudeSpin;
  QDoubleSpinBox* _altitudeSpin;
  QDoubleSpinBox* _headingSpin;
  QDoubleSpinBox* _pitchSpin;
  QDoubleSpinBox* _rollSpin;
  QDoubleSpinBox* _speedSpin;
  QPushButton* _sendButton;
  QTableWidget* _receiveTable;
  DisConnectionState _state = DisConnectionState::Disconnected;
  bool _backendAvailable = false;
  bool _blockedByHla = false;
};

} // namespace presentation
