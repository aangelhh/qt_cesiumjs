#include <gtest/gtest.h>

#include "application/HlaStartupSession.h"

#include <QDir>
#include <QFileInfo>

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
#endif
