#include <gtest/gtest.h>

#include "application/HlaStartupSession.h"
#include "application/StartupConfiguration.h"
#include "infrastructure/interoperability/hla/HlaBackendFactory.h"
#include "infrastructure/interoperability/hla/HlaRuntime.h"
#include "infrastructure/interoperability/hla/HlaEntityPublisher.h"
#include "infrastructure/interoperability/hla/HlaWarfarePublisher.h"
#include "infrastructure/interoperability/hla/SharedLibraryHlaBackend.h"

#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QThread>

#include <memory>
#include <cmath>
#include <vector>

namespace {

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
  configuration.createFederationIfMissing = false;

  application::HlaStartupSession session;
  const tactical::hla::Result startResult = session.start(configuration);
  ASSERT_TRUE(startResult.success) << startResult.message;

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

  tactical::hla::Result result = session.publishEntities({aircraft});
  ASSERT_TRUE(result.success) << result.message;
  ASSERT_TRUE(session.poll(0.05).success);

  aircraft.longitude = -3.999;
  aircraft.headingDegrees = 100.0;
  aircraft.altitude = 3200.0;
  aircraft.speedKnots = 340.0;
  result = session.publishEntities({aircraft});
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
      result = session.publishEntities({aircraft});
      ASSERT_TRUE(result.success) << result.message;
      ASSERT_TRUE(session.poll(0.01).success);
      QThread::msleep(100);
    }
  }

  result = session.publishEntities({});
  ASSERT_TRUE(result.success) << result.message;
  EXPECT_TRUE(session.stop().success);
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

  const tactical::hla::Result startResult = runtime.start(configuration);
  ASSERT_TRUE(startResult.success) << startResult.message;
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

  EXPECT_TRUE(runtime.poll(0.0).success);
  EXPECT_TRUE(publisher.removeAll().success);
  EXPECT_TRUE(runtime.stop().success);
}
#endif
