#include <gtest/gtest.h>

#include "application/StartupConfiguration.h"

#include <QSettings>
#include <QTemporaryDir>
#include <QTemporaryFile>

TEST(StartupConfiguration, DefaultsToValidStandaloneMode) {
  const application::StartupConfiguration configuration;

  EXPECT_EQ(
      configuration.federationMode,
      application::FederationMode::Standalone);
  EXPECT_TRUE(configuration.validationError().isEmpty());
}

TEST(StartupConfiguration, ValidatesOnlySelectedFederationMode) {
  application::StartupConfiguration configuration;
  configuration.dis.address.clear();
  EXPECT_TRUE(configuration.validationError().isEmpty());

  configuration.federationMode = application::FederationMode::Dis;
  EXPECT_EQ(configuration.validationError(), "DIS address is required.");
}

TEST(StartupConfiguration, RequiresAvailableHlaBackendLibrary) {
  application::StartupConfiguration configuration;
  configuration.federationMode = application::FederationMode::Hla;
  configuration.hla.backendId = QStringLiteral("openrti1516e");
  configuration.hla.backendLibraryPath = QStringLiteral("/missing/backend");

  EXPECT_EQ(
      configuration.validationError(),
      "The selected HLA backend library is unavailable.");

  QTemporaryFile backendLibrary;
  ASSERT_TRUE(backendLibrary.open());
  configuration.hla.backendLibraryPath = backendLibrary.fileName();
  EXPECT_TRUE(configuration.validationError().isEmpty());
}

TEST(StartupConfiguration, PersistsFederationAndOptionalIntegrations) {
  QTemporaryDir directory;
  ASSERT_TRUE(directory.isValid());
  QSettings settings(
      directory.filePath(QStringLiteral("startup.ini")),
      QSettings::IniFormat);

  application::StartupConfiguration expected;
  expected.federationMode = application::FederationMode::Dis;
  expected.dis.address = QStringLiteral("239.10.20.30");
  expected.dis.port = 3010;
  expected.dis.exerciseId = 17;
  expected.dis.siteId = 12;
  expected.dis.applicationId = 34;
  expected.dis.remoteEntityTimeoutSeconds = 9.5;
  expected.hla.synchronizationPointLabel = QStringLiteral("ReadyToRun");
  expected.hla.timeManagementEnabled = true;
  expected.hla.timeLookaheadSeconds = 0.025;
  expected.ros2.enabled = true;
  expected.ros2.domainId = 42;
  expected.ros2.topicPrefix = QStringLiteral("/exercise/entities");
  expected.cigi.enabled = true;
  expected.cigi.host = QStringLiteral("192.0.2.20");
  expected.cigi.port = 8005;
  expected.save(settings);

  const auto actual = application::StartupConfiguration::load(settings);

  EXPECT_EQ(actual.federationMode, application::FederationMode::Dis);
  EXPECT_EQ(actual.dis.address, expected.dis.address);
  EXPECT_EQ(actual.dis.port, expected.dis.port);
  EXPECT_EQ(actual.dis.exerciseId, expected.dis.exerciseId);
  EXPECT_EQ(actual.dis.siteId, expected.dis.siteId);
  EXPECT_EQ(actual.dis.applicationId, expected.dis.applicationId);
  EXPECT_DOUBLE_EQ(
      actual.dis.remoteEntityTimeoutSeconds,
      expected.dis.remoteEntityTimeoutSeconds);
  EXPECT_EQ(
      actual.hla.synchronizationPointLabel,
      expected.hla.synchronizationPointLabel);
  EXPECT_TRUE(actual.hla.timeManagementEnabled);
  EXPECT_DOUBLE_EQ(
      actual.hla.timeLookaheadSeconds,
      expected.hla.timeLookaheadSeconds);
  EXPECT_TRUE(actual.ros2.enabled);
  EXPECT_EQ(actual.ros2.domainId, 42);
  EXPECT_EQ(actual.ros2.topicPrefix, expected.ros2.topicPrefix);
  EXPECT_TRUE(actual.cigi.enabled);
  EXPECT_EQ(actual.cigi.host, expected.cigi.host);
  EXPECT_EQ(actual.cigi.port, expected.cigi.port);
}

TEST(StartupConfiguration, AddsAllRepositoryFomModulesInDependencyOrder) {
  application::StartupConfiguration configuration;
  configuration.hla.fomModules = {QStringLiteral("/custom/exercise-fom.xml")};

  configuration.addMissingHlaFomModules(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));

  ASSERT_EQ(configuration.hla.fomModules.size(), 14);
  EXPECT_EQ(
      QFileInfo(configuration.hla.fomModules.at(1)).fileName(),
      QStringLiteral("RPR_FOM_v2.0_1516-2010.xml"));
  EXPECT_EQ(
      QFileInfo(configuration.hla.fomModules.at(2)).fileName(),
      QStringLiteral("NETN-BASE.xml"));
  EXPECT_EQ(
      QFileInfo(configuration.hla.fomModules.constLast()).fileName(),
      QStringLiteral("NETN-LOG.xml"));
  for (int index = 1; index < configuration.hla.fomModules.size(); ++index) {
    EXPECT_TRUE(QFileInfo::exists(configuration.hla.fomModules.at(index)));
  }

  configuration.addMissingHlaFomModules(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));
  EXPECT_EQ(configuration.hla.fomModules.size(), 14);
}
