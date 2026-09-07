#include <gtest/gtest.h>

#include "application/HlaStartupSession.h"
#include "application/StartupConfiguration.h"
#include "infrastructure/interoperability/hla/HlaBackendFactory.h"
#include "infrastructure/interoperability/hla/HlaRuntime.h"
#include "infrastructure/interoperability/hla/HlaEntityPublisher.h"
#include "infrastructure/interoperability/hla/HlaInboundAdapter.h"
#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"
#include "infrastructure/interoperability/hla/HlaSensorPublisher.h"
#include "infrastructure/interoperability/hla/HlaSimulationControlPublisher.h"
#include "infrastructure/interoperability/hla/SharedLibraryHlaBackend.h"

#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QThread>

#include <algorithm>
#include <memory>
#include <cmath>
#include <vector>

namespace {

void verifyOwnershipTransfer(
    const std::string& pluginPath,
    const std::string& localSettings,
    bool allowUnsupported);

std::string mockPluginPath() {
  return QTTEST_HLA_MOCK_PLUGIN_PATH;
}

std::vector<std::string> netnFomModules() {
  const QDir fomDirectory(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));
  const QStringList moduleNames = {
      QStringLiteral("RPR_FOM_v2.0_1516-2010.xml"),
      QStringLiteral("NETN-BASE.xml"),
      QStringLiteral("NETN-ORG.xml"),
      QStringLiteral("NETN-AIS.xml"),
      QStringLiteral("NETN-CBRN.xml"),
      QStringLiteral("NETN-TMR.xml"),
      QStringLiteral("NETN-ETR.xml"),
      QStringLiteral("NETN-Physical.xml"),
      QStringLiteral("NETN-METOC.xml"),
      QStringLiteral("NETN-SE.xml"),
      QStringLiteral("NETN-COM.xml"),
      QStringLiteral("NETN-MRM.xml"),
      QStringLiteral("NETN-LOG.xml")};

  std::vector<std::string> modules;
  modules.reserve(static_cast<std::size_t>(moduleNames.size()));
  for (const QString& moduleName : moduleNames) {
    modules.push_back(fomDirectory.filePath(moduleName).toStdString());
  }
  return modules;
}

} // namespace

TEST(HlaBackendPlugin, ReportsMissingLibraryWithoutCrashing) {
  tactical::hla::SharedLibraryHlaBackend backend(
      "/path/that/does/not/exist/hla_backend");

  EXPECT_FALSE(backend.isAvailable());
  EXPECT_EQ(backend.state(), tactical::hla::BackendState::Unavailable);
  EXPECT_FALSE(backend.loadError().empty());
}

TEST(HlaBackendPlugin, LoadsMockPluginAndExecutesFederationLifecycle) {
  const std::string pluginPath = mockPluginPath();
  ASSERT_TRUE(QFileInfo::exists(QString::fromStdString(pluginPath)))
      << pluginPath;

  auto backend =
      std::make_unique<tactical::hla::SharedLibraryHlaBackend>(pluginPath);
  ASSERT_TRUE(backend->isAvailable()) << backend->loadError();
  EXPECT_EQ(backend->id(), "mock-plugin");
  EXPECT_EQ(backend->version(), "1.0");
  EXPECT_EQ(backend->state(), tactical::hla::BackendState::Disconnected);

  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "PluginTestFederation";
  configuration.federateName = "plugin-test-01";
  configuration.federateType = "test";
  tactical::hla::HlaRuntime runtime(std::move(backend));

  EXPECT_TRUE(runtime.start(configuration).success);
  EXPECT_TRUE(runtime.poll(0.0).success);
  EXPECT_TRUE(runtime.stop().success);
  EXPECT_EQ(runtime.state(), tactical::hla::BackendState::Disconnected);
}

TEST(HlaBackendPlugin, MockPluginExchangesSynchronizationPointCallbacks) {
  auto backend = std::make_unique<tactical::hla::SharedLibraryHlaBackend>(
      mockPluginPath());
  ASSERT_TRUE(backend->isAvailable()) << backend->loadError();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  tactical::hla::HlaInboundAdapter inbound;
  runtime.setEventSink(&inbound);

  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "SynchronizationTestFederation";
  configuration.federateName = "synchronization-test-01";
  configuration.federateType = "test";
  ASSERT_TRUE(runtime.start(configuration).success);

  ASSERT_TRUE(runtime.registerSynchronizationPoint(
      "ReadyToRun", {1, 2, 3}).success);
  auto changes = inbound.takeSynchronizationChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_EQ(changes.front().label, "ReadyToRun");
  EXPECT_EQ(changes.front().tag, (tactical::hla::ByteBuffer{1, 2, 3}));
  EXPECT_FALSE(changes.front().federationSynchronized);

  ASSERT_TRUE(runtime.achieveSynchronizationPoint("ReadyToRun").success);
  changes = inbound.takeSynchronizationChanges();
  ASSERT_EQ(changes.size(), 1U);
  EXPECT_EQ(changes.front().label, "ReadyToRun");
  EXPECT_TRUE(changes.front().federationSynchronized);
  EXPECT_TRUE(runtime.stop().success);
}

TEST(HlaBackendPlugin, MockPluginExchangesTimeManagementCallbacks) {
  auto backend = std::make_unique<tactical::hla::SharedLibraryHlaBackend>(
      mockPluginPath());
  ASSERT_TRUE(backend->isAvailable()) << backend->loadError();
  tactical::hla::HlaRuntime runtime(std::move(backend));
  tactical::hla::HlaInboundAdapter inbound;
  runtime.setEventSink(&inbound);

  tactical::hla::SessionConfiguration configuration;
  configuration.federationName = "TimeManagementTestFederation";
  configuration.federateName = "time-management-test-01";
  configuration.federateType = "test";
  ASSERT_TRUE(runtime.start(configuration).success);
  ASSERT_TRUE(runtime.enableTimeRegulation(0.01).success);
  ASSERT_TRUE(runtime.enableTimeConstrained().success);
  ASSERT_TRUE(runtime.requestTimeAdvance(0.033).success);

  const auto events = inbound.takeTimeManagementEvents();
  ASSERT_EQ(events.size(), 3U);
  EXPECT_EQ(
      events[0].kind,
      tactical::hla::RemoteTimeManagementEventKind::RegulationEnabled);
  EXPECT_EQ(
      events[1].kind,
      tactical::hla::RemoteTimeManagementEventKind::ConstrainedEnabled);
  EXPECT_EQ(
      events[2].kind,
      tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted);
  EXPECT_DOUBLE_EQ(events[2].logicalTimeSeconds, 0.033);
  EXPECT_TRUE(runtime.stop().success);
}

#ifdef QTTEST_HLA_PITCH_PLUGIN_PATH
TEST(HlaBackendPlugin, LoadsInstalledPitchBackendWhenBuilt) {
  const std::string pluginPath = QTTEST_HLA_PITCH_PLUGIN_PATH;
  ASSERT_TRUE(QFileInfo::exists(QString::fromStdString(pluginPath)))
      << pluginPath;

  tactical::hla::SharedLibraryHlaBackend backend(pluginPath);
  ASSERT_TRUE(backend.isAvailable()) << backend.loadError();
  EXPECT_EQ(backend.id(), "pitch1516e");
  EXPECT_EQ(backend.state(), tactical::hla::BackendState::Disconnected);
  EXPECT_FALSE(backend.version().empty());
}

TEST(HlaBackendPlugin, PitchPublishesAndUpdatesAircraftWhenIntegrationEnabled) {
  if (!qEnvironmentVariableIsSet("QTTEST_RUN_PITCH_INTEGRATION")) {
    GTEST_SKIP() << "Set QTTEST_RUN_PITCH_INTEGRATION=1 to use a running Pitch CRC";
  }

  application::StartupConfiguration startupConfiguration;
  startupConfiguration.addMissingHlaFomModules(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));

  application::HlaStartupConfiguration& configuration =
      startupConfiguration.hla;
  configuration.backendId = QStringLiteral("pitch1516e");
  configuration.backendLibraryPath =
      QStringLiteral(QTTEST_HLA_PITCH_PLUGIN_PATH);
  configuration.localSettingsDesignator = qEnvironmentVariable(
      "QTTEST_PITCH_LOCAL_SETTINGS", "crcAddress=localhost:8989");
  configuration.federationName = qEnvironmentVariable(
      "QTTEST_PITCH_FEDERATION", "qttest-federation");
  configuration.federateName = QStringLiteral("qttest-pitch-test-%1")
      .arg(QCoreApplication::applicationPid());
  configuration.federateType = QStringLiteral("qttest-integration-test");
  configuration.createFederationIfMissing = true;
  configuration.timeManagementEnabled = true;
  configuration.timeLookaheadSeconds = 0.01;

  application::HlaStartupSession session;
  const tactical::hla::Result startResult = session.start(configuration);
  ASSERT_TRUE(startResult.success) << startResult.message;

  const std::string synchronizationLabel =
      QStringLiteral("qttest-pitch-ready-%1")
          .arg(QCoreApplication::applicationPid())
          .toStdString();
  ASSERT_TRUE(session.registerSynchronizationPoint(
      synchronizationLabel, {0x71, 0x74, 0x74, 0x65, 0x73, 0x74}).success);
  bool synchronizationAnnounced = false;
  for (int attempt = 0; attempt < 200 && !synchronizationAnnounced; ++attempt) {
    ASSERT_TRUE(session.poll(0.05).success);
    for (const auto& change : session.takeRemoteSynchronizationChanges()) {
      if (change.label == synchronizationLabel &&
          !change.federationSynchronized) {
        synchronizationAnnounced = true;
      }
    }
    if (!synchronizationAnnounced) QThread::msleep(5);
  }
  ASSERT_TRUE(synchronizationAnnounced);
  ASSERT_TRUE(session.achieveSynchronizationPoint(
      synchronizationLabel).success);
  ASSERT_TRUE(session.isTimeManagementActive());
  ASSERT_TRUE(session.requestTimeAdvance(0.033).success);
  bool timeGranted = false;
  for (int attempt = 0; attempt < 200 && !timeGranted; ++attempt) {
    ASSERT_TRUE(session.poll(0.05).success);
    for (const auto& event : session.takeRemoteTimeManagementEvents()) {
      if (event.kind ==
              tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted &&
          event.logicalTimeSeconds >= 0.033) {
        timeGranted = true;
      }
    }
    if (!timeGranted) QThread::msleep(5);
  }
  ASSERT_TRUE(timeGranted);

  Entity aircraft;
  aircraft.entityId = QStringLiteral("pitch-aircraft-%1")
      .arg(QCoreApplication::applicationPid());
  aircraft.name = QStringLiteral("PitchFighter");
  aircraft.domain = QStringLiteral("Air");
  aircraft.entityKind = 1;
  aircraft.entityDomain = 2;
  aircraft.entityCountry = 225;
  aircraft.entityCategory = 1;
  aircraft.entitySubcategory = 1;
  aircraft.forceIdentifier = 1;
  aircraft.latitude = 40.0;
  aircraft.longitude = -4.0;
  aircraft.altitude = 3000.0;
  aircraft.headingDegrees = 90.0;
  aircraft.speedKnots = 320.0;
  SensorDefinition radar;
  radar.id = QStringLiteral("pitch-radar-01");
  radar.name = QStringLiteral("Pitch Test Radar");
  radar.sensorType = QStringLiteral("radar");
  radar.sensorSubType = QStringLiteral("airborne-radar");
  radar.enabled = true;
  radar.emitting = true;
  radar.azimuthWidthDegrees = 120.0;
  radar.elevationWidthDegrees = 60.0;
  radar.radarProfile.frequencyHertz = 10.0e9;
  radar.radarProfile.bandwidthHertz = 2.0e6;
  radar.radarProfile.peakPowerWatts = 5000.0;
  aircraft.sensors.push_back(radar);

  Entity target;
  target.entityId = QStringLiteral("pitch-track-target-%1")
      .arg(QCoreApplication::applicationPid());
  target.name = QStringLiteral("PitchTrackTarget");
  target.domain = QStringLiteral("Air");
  target.latitude = 40.01;
  target.longitude = -3.99;
  target.altitude = 3200.0;
  target.headingDegrees = 270.0;
  target.speedKnots = 280.0;
  SensorContact track;
  track.sensorId = radar.id;
  track.targetEntityId = target.entityId;
  track.targetEntityName = target.name;
  track.detected = true;
  track.confidence = 0.9;
  aircraft.sensorContacts.push_back(track);

  tactical::hla::Result result = session.publishEntities({aircraft, target});
  ASSERT_TRUE(result.success) << result.message;
  result = session.publishSensors({aircraft, target});
  ASSERT_TRUE(result.success) << result.message;

  ActiveMunition munition;
  munition.id = QStringLiteral("pitch-missile-%1")
      .arg(QCoreApplication::applicationPid());
  munition.launcherEntityId = aircraft.entityId;
  munition.launcherEntityName = aircraft.name;
  munition.targetEntityId = target.entityId;
  munition.targetEntityName = target.name;
  munition.munitionType = QStringLiteral("Missile");
  munition.latitude = aircraft.latitude;
  munition.longitude = aircraft.longitude;
  munition.altitudeMeters = aircraft.altitude;
  munition.headingDegrees = aircraft.headingDegrees;
  munition.speedMetersPerSecond = 600.0;
  result = session.publishMunitions({munition});
  ASSERT_TRUE(result.success) << result.message;

  TransientEffect detonation;
  detonation.id = munition.id + QStringLiteral("-impact");
  detonation.effectType = QStringLiteral("ImpactFlash");
  detonation.latitude = aircraft.latitude;
  detonation.longitude = aircraft.longitude + 0.001;
  detonation.altitudeMeters = aircraft.altitude;
  result = session.publishDetonations({detonation});
  ASSERT_TRUE(result.success) << result.message;
  ASSERT_TRUE(session.poll(0.05).success);

  aircraft.longitude = -3.999;
  aircraft.headingDegrees = 100.0;
  aircraft.altitude = 3200.0;
  aircraft.speedKnots = 340.0;
  result = session.publishEntities({aircraft, target});
  ASSERT_TRUE(result.success) << result.message;
  ASSERT_TRUE(session.poll(0.05).success);

  bool validObservationDuration = false;
  const int observationSeconds = qEnvironmentVariableIntValue(
      "QTTEST_PITCH_OBSERVE_SECONDS", &validObservationDuration);
  if (validObservationDuration && observationSeconds > 0) {
    for (int tick = 0; tick < observationSeconds * 10; ++tick) {
      aircraft.longitude += 0.00001;
      aircraft.headingDegrees = std::fmod(
          aircraft.headingDegrees + 0.5, 360.0);
      result = session.publishEntities({aircraft, target});
      ASSERT_TRUE(result.success) << result.message;
      result = session.publishSensors({aircraft, target});
      ASSERT_TRUE(result.success) << result.message;
      ASSERT_TRUE(session.poll(0.01).success);
      QThread::msleep(100);
    }
  }

  result = session.publishEntities({});
  ASSERT_TRUE(result.success) << result.message;
  result = session.publishSensors({});
  ASSERT_TRUE(result.success) << result.message;
  EXPECT_TRUE(session.stop().success);
}

TEST(HlaBackendPlugin, PitchTransfersSpatialOwnershipWhenIntegrationEnabled) {
  if (!qEnvironmentVariableIsSet("QTTEST_RUN_PITCH_INTEGRATION")) {
    GTEST_SKIP() << "Set QTTEST_RUN_PITCH_INTEGRATION=1 to use a running Pitch CRC";
  }

  verifyOwnershipTransfer(
      QTTEST_HLA_PITCH_PLUGIN_PATH,
      qEnvironmentVariable("QTTEST_PITCH_LOCAL_SETTINGS",
                           "crcAddress=localhost:8989").toStdString(),
      false);
}
#endif

#ifdef QTTEST_HLA_OPENRTI_PLUGIN_PATH
TEST(HlaBackendPlugin, LoadsOpenRtiBackendWhenBuilt) {
  const std::string pluginPath = QTTEST_HLA_OPENRTI_PLUGIN_PATH;
  ASSERT_FALSE(pluginPath.empty());

  tactical::hla::SharedLibraryHlaBackend backend(pluginPath);

  ASSERT_TRUE(backend.isAvailable()) << backend.loadError();
  EXPECT_EQ(backend.id(), "openrti1516e");
  EXPECT_NE(backend.version().find("OpenRTI"), std::string::npos);
  EXPECT_EQ(backend.state(), tactical::hla::BackendState::Disconnected);
}

TEST(HlaBackendPlugin, OpenRtiLoadsRepositoryNetnModules) {
  const std::string pluginPath = QTTEST_HLA_OPENRTI_PLUGIN_PATH;
  ASSERT_TRUE(QFileInfo::exists(QString::fromStdString(pluginPath)))
      << pluginPath;

  tactical::hla::SessionConfiguration configuration;
  configuration.localSettingsDesignator = "thread://";
  configuration.federationName = "qttest-netn-module-test";
  configuration.federateName = "qttest-netn-test-01";
  configuration.federateType = "qttest-test";
  configuration.fomModules = netnFomModules();
  for (const std::string& module : configuration.fomModules) {
    ASSERT_TRUE(QFileInfo::exists(QString::fromStdString(module))) << module;
  }

  tactical::hla::BackendConfiguration backendConfiguration;
  backendConfiguration.adapter = tactical::hla::BackendAdapter::SharedLibrary;
  backendConfiguration.expectedBackendId = "openrti1516e";
  backendConfiguration.libraryPath = pluginPath;
  tactical::hla::BackendCreationResult backendResult =
      tactical::hla::HlaBackendFactory::create(backendConfiguration);
  ASSERT_TRUE(backendResult) << backendResult.error;
  tactical::hla::HlaRuntime runtime(std::move(backendResult.backend));
  tactical::hla::HlaInboundAdapter inbound;
  runtime.setEventSink(&inbound);

  const tactical::hla::Result startResult = runtime.start(configuration);
  ASSERT_TRUE(startResult.success) << startResult.message;

  ASSERT_TRUE(runtime.registerSynchronizationPoint(
      "ReadyToRun", {0x71, 0x74, 0x74, 0x65, 0x73, 0x74}).success);
  bool announced = false;
  for (int attempt = 0; attempt < 200 && !announced; ++attempt) {
    ASSERT_TRUE(runtime.poll(0.05).success);
    for (const auto& change : inbound.takeSynchronizationChanges()) {
      if (change.label == "ReadyToRun" &&
          !change.federationSynchronized) {
        announced = true;
      }
    }
    if (!announced) QThread::msleep(5);
  }
  ASSERT_TRUE(announced);
  ASSERT_TRUE(runtime.achieveSynchronizationPoint("ReadyToRun").success);
  bool synchronized = false;
  for (int attempt = 0; attempt < 200 && !synchronized; ++attempt) {
    ASSERT_TRUE(runtime.poll(0.05).success);
    for (const auto& change : inbound.takeSynchronizationChanges()) {
      if (change.label == "ReadyToRun" &&
          change.federationSynchronized) {
        synchronized = true;
      }
    }
    if (!synchronized) QThread::msleep(5);
  }
  EXPECT_TRUE(synchronized);

  tactical::hla::HlaEntityPublisher publisher(runtime);
  tactical::hla::RprEntityState entity;
  entity.stableId = "openrti-aircraft-01";
  entity.name = "openrti-test";
  entity.domain = "Air";
  entity.entityKind = 1;
  entity.entityDomain = 2;
  entity.latitudeDegrees = 40.0;
  entity.longitudeDegrees = -4.0;
  entity.altitudeMeters = 3000.0;
  const tactical::hla::Result publishResult = publisher.synchronize({entity});
  ASSERT_TRUE(publishResult.success) << publishResult.message;
  EXPECT_EQ(publisher.registeredObjectCount(), 1U);

  tactical::hla::HlaWarfarePublisher warfarePublisher(runtime);
  tactical::hla::RprWeaponFireState munition;
  munition.stableId = "openrti-missile-01";
  munition.munitionType = "Missile";
  munition.latitudeDegrees = 40.0;
  munition.longitudeDegrees = -4.0;
  munition.altitudeMeters = 3000.0;
  munition.headingDegrees = 90.0;
  munition.speedMetersPerSecond = 500.0;
  const tactical::hla::Result sendInteractionResult =
      warfarePublisher.synchronize({munition});
  ASSERT_TRUE(sendInteractionResult.success) << sendInteractionResult.message;

  tactical::hla::RprMunitionDetonationState detonation;
  detonation.effectId = "openrti-missile-01-impact";
  detonation.munitionStableId = munition.stableId;
  detonation.munitionType = munition.munitionType;
  detonation.latitudeDegrees = munition.latitudeDegrees;
  detonation.longitudeDegrees = munition.longitudeDegrees;
  detonation.altitudeMeters = munition.altitudeMeters;
  const tactical::hla::Result detonationResult =
      warfarePublisher.synchronizeDetonations({detonation});
  ASSERT_TRUE(detonationResult.success) << detonationResult.message;

  tactical::hla::HlaSensorPublisher sensorPublisher(runtime);
  tactical::hla::RprSensorState radar;
  radar.entityStableId = entity.stableId;
  radar.sensorId = "radar-01";
  radar.frequencyHertz = 10.0e9;
  radar.bandwidthHertz = 2.0e6;
  radar.peakPowerWatts = 5000.0;
  const tactical::hla::Result sensorResult =
      sensorPublisher.synchronize({radar});
  ASSERT_TRUE(sensorResult.success) << sensorResult.message;

  tactical::hla::HlaSimulationControlPublisher controlPublisher(runtime);
  ASSERT_TRUE(controlPublisher.publish(
      tactical::hla::RemoteSimulationControl::StartResume, 1.0).success);
  ASSERT_TRUE(controlPublisher.publish(
      tactical::hla::RemoteSimulationControl::Pause, 2.0).success);
  ASSERT_TRUE(controlPublisher.publish(
      tactical::hla::RemoteSimulationControl::Stop, 3.0).success);

  EXPECT_TRUE(runtime.poll(0.0).success);
  EXPECT_TRUE(sensorPublisher.removeAll().success);
  EXPECT_TRUE(publisher.removeAll().success);
  EXPECT_TRUE(runtime.stop().success);
}

TEST(HlaBackendPlugin, OpenRtiHandlesOwnershipAccordingToBackendSupport) {
  verifyOwnershipTransfer(QTTEST_HLA_OPENRTI_PLUGIN_PATH, "thread://", true);
}
#endif

namespace {
void verifyOwnershipTransfer(
    const std::string& pluginPath,
    const std::string& localSettings,
    bool allowUnsupported) {
  const std::string federationName =
      "qttest-ownership-" +
      std::to_string(QCoreApplication::applicationPid());
  const std::string aircraftClass =
      tactical::hla::RprFomEncoding::objectClassName("Air");
  const std::vector<std::string> attributes =
      tactical::hla::RprFomEncoding::publishedAttributeNames();

  auto ownerBackend =
      std::make_unique<tactical::hla::SharedLibraryHlaBackend>(pluginPath);
  auto acquirerBackend =
      std::make_unique<tactical::hla::SharedLibraryHlaBackend>(pluginPath);
  ASSERT_TRUE(ownerBackend->isAvailable()) << ownerBackend->loadError();
  ASSERT_TRUE(acquirerBackend->isAvailable()) << acquirerBackend->loadError();
  tactical::hla::HlaRuntime owner(std::move(ownerBackend));
  tactical::hla::HlaRuntime acquirer(std::move(acquirerBackend));
  tactical::hla::HlaInboundAdapter ownerInbound;
  tactical::hla::HlaInboundAdapter acquirerInbound;
  owner.setEventSink(&ownerInbound);
  acquirer.setEventSink(&acquirerInbound);

  tactical::hla::SessionConfiguration ownerConfiguration;
  ownerConfiguration.localSettingsDesignator = localSettings;
  ownerConfiguration.federationName = federationName;
  ownerConfiguration.federateName = "ownership-owner";
  ownerConfiguration.federateType = "qttest-test";
  ownerConfiguration.fomModules = {netnFomModules().front()};
  tactical::hla::SessionConfiguration acquirerConfiguration =
      ownerConfiguration;
  acquirerConfiguration.federateName = "ownership-acquirer";

  const tactical::hla::Result ownerStart = owner.start(ownerConfiguration);
  ASSERT_TRUE(ownerStart.success) << ownerStart.message;
  const tactical::hla::Result acquirerStart =
      acquirer.start(acquirerConfiguration);
  ASSERT_TRUE(acquirerStart.success) << acquirerStart.message;
  ASSERT_TRUE(owner.publishObjectClass(aircraftClass, attributes).success);
  ASSERT_TRUE(owner.subscribeObjectClass(aircraftClass, attributes).success);
  ASSERT_TRUE(acquirer.publishObjectClass(aircraftClass, attributes).success);
  ASSERT_TRUE(acquirer.subscribeObjectClass(aircraftClass, attributes).success);

  tactical::hla::ObjectInstanceId localInstanceId = 0;
  ASSERT_TRUE(owner.registerObjectInstance(
      aircraftClass, "qttest.ownership-aircraft", localInstanceId).success);
  tactical::hla::RprEntityState entity;
  entity.name = "ownership-aircraft";
  entity.domain = "Air";
  entity.latitudeDegrees = 40.0;
  entity.longitudeDegrees = -4.0;
  entity.altitudeMeters = 3000.0;
  ASSERT_TRUE(owner.updateObjectAttributes(
      localInstanceId,
      tactical::hla::RprFomEncoding::encodeAttributes(entity, 1, 1, 1)).success);

  tactical::hla::ObjectInstanceId remoteInstanceId = 0;
  for (int attempt = 0; attempt < 200 && remoteInstanceId == 0; ++attempt) {
    ASSERT_TRUE(owner.poll(0.01).success);
    ASSERT_TRUE(acquirer.poll(0.01).success);
    for (const auto& change : acquirerInbound.takeEntityChanges()) {
      if (!change.removed) remoteInstanceId = change.instanceId;
    }
    if (remoteInstanceId == 0) QThread::msleep(2);
  }
  ASSERT_NE(remoteInstanceId, 0U);
  EXPECT_NE(remoteInstanceId, localInstanceId);

  const tactical::hla::Result acquisitionRequest =
      acquirer.requestAttributeOwnershipAcquisition(
          remoteInstanceId, {"Spatial"}, {0x6f, 0x77, 0x6e});
  if (!acquisitionRequest.success) {
    ASSERT_TRUE(allowUnsupported) << acquisitionRequest.message;
    EXPECT_NE(
        acquisitionRequest.message.find("Not implemented"),
        std::string::npos) << acquisitionRequest.message;
    EXPECT_TRUE(acquirer.stop().success);
    EXPECT_TRUE(owner.stop().success);
    return;
  }
  bool releaseRequested = false;
  for (int attempt = 0; attempt < 200 && !releaseRequested; ++attempt) {
    ASSERT_TRUE(owner.poll(0.01).success);
    ASSERT_TRUE(acquirer.poll(0.01).success);
    for (const auto& event : ownerInbound.takeOwnershipEvents()) {
      if (event.kind == tactical::hla::OwnershipEventKind::ReleaseRequested &&
          event.instanceId == localInstanceId &&
          event.attributeNames == std::vector<std::string>{"Spatial"}) {
        releaseRequested = true;
      }
    }
    if (!releaseRequested) QThread::msleep(2);
  }
  ASSERT_TRUE(releaseRequested);
  ASSERT_TRUE(owner.unconditionalAttributeOwnershipDivestiture(
      localInstanceId, {"Spatial"}).success);

  bool acquired = false;
  for (int attempt = 0; attempt < 200 && !acquired; ++attempt) {
    ASSERT_TRUE(owner.poll(0.01).success);
    ASSERT_TRUE(acquirer.poll(0.01).success);
    for (const auto& event : acquirerInbound.takeOwnershipEvents()) {
      if (event.kind == tactical::hla::OwnershipEventKind::Acquired &&
          event.instanceId == remoteInstanceId &&
          event.attributeNames == std::vector<std::string>{"Spatial"}) {
        acquired = true;
      }
    }
    if (!acquired) QThread::msleep(2);
  }
  ASSERT_TRUE(acquired);

  entity.latitudeDegrees = 40.1;
  const auto updatedAttributes =
      tactical::hla::RprFomEncoding::encodeAttributes(entity, 1, 1, 1);
  const auto spatial = std::find_if(
      updatedAttributes.begin(), updatedAttributes.end(),
      [](const tactical::hla::NamedValue& value) {
        return value.name == "Spatial";
      });
  ASSERT_NE(spatial, updatedAttributes.end());
  EXPECT_TRUE(acquirer.updateObjectAttributes(
      remoteInstanceId, {*spatial}).success);
  EXPECT_FALSE(owner.updateObjectAttributes(
      localInstanceId, {*spatial}).success);
  EXPECT_TRUE(acquirer.unconditionalAttributeOwnershipDivestiture(
      remoteInstanceId, {"Spatial"}).success);

  EXPECT_TRUE(acquirer.stop().success);
  EXPECT_TRUE(owner.stop().success);
}
} // namespace
