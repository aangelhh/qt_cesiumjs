#include <gtest/gtest.h>

#include "application/HlaStartupSession.h"

#include <QDir>
#include <QFileInfo>
#include <QThread>

#include <chrono>
#include <future>
#include <optional>

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

TEST(HlaStartupSession, OpenRtiDeliversTimestampedEntityBetweenFederates) {
  application::StartupConfiguration startupConfiguration;
  startupConfiguration.addMissingHlaFomModules(
      QStringLiteral(QTTEST_SOURCE_DIR "/src/infrastructure/hla/FOM"));
  auto configuration = startupConfiguration.hla;
  configuration.backendId = QStringLiteral("openrti1516e");
  configuration.backendLibraryPath =
      QStringLiteral(QTTEST_HLA_OPENRTI_PLUGIN_PATH);
  configuration.localSettingsDesignator = QStringLiteral("thread://");
  configuration.federationName =
      QStringLiteral("qttest-timestamp-roundtrip-test");
  configuration.federateType = QStringLiteral("qttest-test");
  configuration.timeManagementEnabled = true;
  configuration.timeLookaheadSeconds = 0.01;

  application::HlaStartupSession publisher;
  configuration.federateName = QStringLiteral("timestamp-publisher");
  configuration.timeManagementEnabled = true;
  const tactical::hla::Result publisherStart = publisher.start(configuration);
  ASSERT_TRUE(publisherStart.success) << publisherStart.message;
  application::HlaStartupSession subscriber;
  configuration.federateName = QStringLiteral("timestamp-subscriber");
  configuration.timeManagementEnabled = true;
  auto subscriberStartFuture = std::async(
      std::launch::async,
      [&subscriber, configuration]() {
        return subscriber.start(configuration);
      });
  while (subscriberStartFuture.wait_for(std::chrono::milliseconds(5)) !=
         std::future_status::ready) {
    ASSERT_TRUE(publisher.poll(0.01).success);
  }
  const tactical::hla::Result subscriberStart = subscriberStartFuture.get();
  ASSERT_TRUE(subscriberStart.success) << subscriberStart.message;
  for (int attempt = 0; attempt < 20; ++attempt) {
    ASSERT_TRUE(publisher.poll(0.01).success);
    ASSERT_TRUE(subscriber.poll(0.01).success);
  }

  Entity entity;
  entity.entityId = QStringLiteral("timestamp-aircraft");
  entity.name = QStringLiteral("Timestamp Aircraft");
  entity.domain = QStringLiteral("Air");
  entity.latitude = 40.0;
  entity.longitude = -4.0;
  entity.altitude = 2500;
  ASSERT_TRUE(publisher.publishEntities({entity}).success);
  ASSERT_TRUE(publisher.requestTimeAdvance(0.033).success);
  ASSERT_TRUE(subscriber.requestTimeAdvance(0.033).success);

  bool received = false;
  std::optional<double> receivedLogicalTime;
  bool receivedTimestamp = false;
  bool publisherGranted = false;
  bool subscriberGranted = false;
  for (int attempt = 0;
       attempt < 300 &&
       !(received && publisherGranted && subscriberGranted);
       ++attempt) {
    ASSERT_TRUE(publisher.poll(0.01).success);
    ASSERT_TRUE(subscriber.poll(0.01).success);
    for (const auto& event : publisher.takeRemoteTimeManagementEvents()) {
      publisherGranted = publisherGranted ||
          event.kind == tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted;
    }
    for (const auto& event : subscriber.takeRemoteTimeManagementEvents()) {
      subscriberGranted = subscriberGranted ||
          event.kind == tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted;
    }
    for (const auto& change : subscriber.takeRemoteEntityChanges()) {
      if (!change.removed && change.instanceId != 0) {
        received = true;
        receivedLogicalTime = change.receiveMetadata.logicalTimeSeconds;
        receivedTimestamp =
            change.receiveMetadata.order ==
                tactical::hla::DeliveryOrder::Timestamp &&
            change.receiveMetadata.logicalTimeSeconds.has_value() &&
            std::abs(*change.receiveMetadata.logicalTimeSeconds - 0.01) < 1.0e-9;
      }
    }
    if (!(received && publisherGranted && subscriberGranted)) {
      QThread::msleep(5);
    }
  }

  EXPECT_TRUE(publisherGranted);
  EXPECT_TRUE(subscriberGranted);
  EXPECT_TRUE(received);
  ASSERT_TRUE(receivedLogicalTime.has_value());
  EXPECT_DOUBLE_EQ(*receivedLogicalTime, 0.01);
  EXPECT_TRUE(receivedTimestamp);
  EXPECT_TRUE(subscriber.stop().success);
  EXPECT_TRUE(publisher.stop().success);
}
#endif
