#include <gtest/gtest.h>

#include "application/HlaStartupSession.h"

#include <QDir>
#include <QFileInfo>
#include <QThread>

TEST(HlaStartupSession, RejectsUnavailableBackend) {
  application::HlaStartupConfiguration configuration;
  configuration.backendId = QStringLiteral("missing");
  configuration.backendLibraryPath = QStringLiteral("/missing/hla/backend");

  application::HlaStartupSession session;
  const tactical::hla::Result result = session.start(configuration);

  EXPECT_FALSE(result.success);
  EXPECT_FALSE(session.isActive());
}

#ifdef QTTEST_HLA_OPENRTI_PLUGIN_PATH
TEST(HlaStartupSession, StartsAndStopsOpenRtiWithRepositoryFoms) {
  application::StartupConfiguration startupConfiguration;
  startupConfiguration.addMissingHlaFomModules(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));

  application::HlaStartupConfiguration& configuration =
      startupConfiguration.hla;
  configuration.backendId = QStringLiteral("openrti1516e");
  configuration.backendLibraryPath =
      QStringLiteral(QTTEST_HLA_OPENRTI_PLUGIN_PATH);
  configuration.localSettingsDesignator = QStringLiteral("thread://");
  configuration.federationName = QStringLiteral("qttest-startup-session-test");
  configuration.federateName = QStringLiteral("qttest-startup-test-01");
  configuration.federateType = QStringLiteral("qttest-test");

  ASSERT_TRUE(QFileInfo::exists(configuration.backendLibraryPath));
  ASSERT_EQ(configuration.fomModules.size(), 13);

  application::HlaStartupSession session;
  const tactical::hla::Result startResult = session.start(configuration);

  ASSERT_TRUE(startResult.success) << startResult.message;
  EXPECT_TRUE(session.isActive());
  EXPECT_EQ(session.backendId(), QStringLiteral("openrti1516e"));
  EXPECT_TRUE(session.poll(0.0).success);
  EXPECT_TRUE(session.stop().success);
  EXPECT_FALSE(session.isActive());
}

TEST(HlaStartupSession, AdvancesOnlyAfterOpenRtiLogicalTimeGrant) {
  application::StartupConfiguration startupConfiguration;
  startupConfiguration.addMissingHlaFomModules(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));
  auto& configuration = startupConfiguration.hla;
  configuration.backendId = QStringLiteral("openrti1516e");
  configuration.backendLibraryPath =
      QStringLiteral(QTTEST_HLA_OPENRTI_PLUGIN_PATH);
  configuration.localSettingsDesignator = QStringLiteral("thread://");
  configuration.federationName = QStringLiteral("qttest-time-session-test");
  configuration.federateName = QStringLiteral("qttest-time-test-01");
  configuration.federateType = QStringLiteral("qttest-test");
  configuration.timeManagementEnabled = true;
  configuration.timeLookaheadSeconds = 0.01;

  application::HlaStartupSession session;
  const tactical::hla::Result startResult = session.start(configuration);
  ASSERT_TRUE(startResult.success) << startResult.message;
  ASSERT_TRUE(session.isTimeManagementActive());
  EXPECT_DOUBLE_EQ(session.grantedLogicalTimeSeconds(), 0.0);
  Entity entity;
  entity.entityId = QStringLiteral("time-managed-aircraft");
  entity.name = QStringLiteral("Time Managed Aircraft");
  entity.domain = QStringLiteral("Air");
  ASSERT_TRUE(session.publishEntities({entity}).success);
  ASSERT_TRUE(session.requestTimeAdvance(0.033).success);
  EXPECT_FALSE(session.requestTimeAdvance(0.066).success);

  bool granted = false;
  for (int attempt = 0; attempt < 200 && !granted; ++attempt) {
    ASSERT_TRUE(session.poll(0.05).success);
    for (const auto& event : session.takeRemoteTimeManagementEvents()) {
      if (event.kind ==
              tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted &&
          event.logicalTimeSeconds >= 0.033) {
        granted = true;
      }
    }
    if (!granted) QThread::msleep(5);
  }
  EXPECT_TRUE(granted);
  EXPECT_DOUBLE_EQ(session.grantedLogicalTimeSeconds(), 0.033);
  entity.longitude = 0.001;
  EXPECT_TRUE(session.publishEntities({entity}).success);
  EXPECT_TRUE(session.stop().success);
}
#endif
