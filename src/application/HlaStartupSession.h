#pragma once

#include "application/StartupConfiguration.h"
#include "infrastructure/interoperability/hla/HlaRuntime.h"
#include "infrastructure/interoperability/hla/HlaEntityPublisher.h"
#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"
#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"
#include "infrastructure/interoperability/hla/HlaSimulationControlPublisher.h"
#include "infrastructure/interoperability/hla/HlaSensorPublisher.h"
#include "domain/Entity.h"
#include "domain/Munition.h"

#include <memory>
#include <optional>
#include <QVector>

namespace application {

class HlaStartupSession {
public:
  tactical::hla::Result start(const HlaStartupConfiguration& configuration);
  tactical::hla::Result poll(double maximumSeconds);
  tactical::hla::Result publishEntities(const QVector<Entity>& entities);
  tactical::hla::Result publishMunitions(
      const QVector<ActiveMunition>& activeMunitions);
  tactical::hla::Result publishDetonations(
      const QVector<TransientEffect>& transientEffects);
  tactical::hla::Result publishSensors(const QVector<Entity>& entities);
  std::vector<tactical::hla::RemoteEntityChange> takeRemoteEntityChanges();
  std::vector<tactical::hla::RemoteMunitionChange>
      takeRemoteMunitionChanges();
  std::vector<tactical::hla::RemoteSensorChange> takeRemoteSensorChanges();
  std::vector<tactical::hla::RemoteWarfareEvent> takeRemoteWarfareEvents();
  std::vector<tactical::hla::RemoteSynchronizationChange>
      takeRemoteSynchronizationChanges();
  std::vector<tactical::hla::RemoteTimeManagementEvent>
      takeRemoteTimeManagementEvents();
  std::vector<tactical::hla::AttributeOwnershipEvent>
      takeOwnershipEvents();
  std::vector<tactical::hla::ConnectionLostEvent>
      takeConnectionLostEvents();
  std::vector<tactical::hla::RemoteSimulationControl>
      takeRemoteSimulationControls();
  tactical::hla::Result publishSimulationControl(
      tactical::hla::RemoteSimulationControl control,
      double simulationTimeSeconds);
  tactical::hla::Result registerSynchronizationPoint(
      const std::string& label,
      const tactical::hla::ByteBuffer& tag = {});
  tactical::hla::Result achieveSynchronizationPoint(
      const std::string& label);
  tactical::hla::Result requestTimeAdvance(double logicalTimeSeconds);
  tactical::hla::Result requestAttributeOwnershipAcquisition(
      tactical::hla::ObjectInstanceId instanceId,
      const std::vector<std::string>& attributeNames,
      const tactical::hla::ByteBuffer& tag = {});
  tactical::hla::Result unconditionalAttributeOwnershipDivestiture(
      tactical::hla::ObjectInstanceId instanceId,
      const std::vector<std::string>& attributeNames);
  tactical::hla::Result stop();

  bool isActive() const;
  bool isTimeManagementActive() const;
  double grantedLogicalTimeSeconds() const;
  QString backendId() const;

private:
  std::optional<double> publicationLogicalTimeSeconds() const;

  std::unique_ptr<tactical::hla::HlaRuntime> _runtime;
  std::unique_ptr<tactical::hla::HlaEntityPublisher> _entityPublisher;
  std::unique_ptr<tactical::hla::HlaWarfarePublisher> _warfarePublisher;
  std::unique_ptr<tactical::hla::HlaInboundAdapter> _inboundAdapter;
  std::unique_ptr<tactical::hla::HlaSimulationControlPublisher>
      _simulationControlPublisher;
  std::unique_ptr<tactical::hla::HlaSensorPublisher> _sensorPublisher;
  QString _backendId;
  bool _timeManagementActive = false;
  bool _timeAdvancePending = false;
  double _grantedLogicalTimeSeconds = 0.0;
  double _timeLookaheadSeconds = 0.0;
};

} // namespace application
