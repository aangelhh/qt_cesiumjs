#include "presentation/DisConnectionPanel.h"

#include <QApplication>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>

int main(int argc, char** argv) {
  QApplication application(argc, argv);
  presentation::DisConnectionPanel panel;

  application::DisStartupConfiguration configuration;
  configuration.address = QStringLiteral("239.1.2.3");
  configuration.port = 3000;
  configuration.exerciseId = 7;
  configuration.siteId = 20;
  configuration.applicationId = 4;

  auto* address = panel.findChild<QLineEdit*>(QStringLiteral("disAddressEdit"));
  auto* connectButton =
      panel.findChild<QPushButton*>(QStringLiteral("disConnectButton"));
  auto* disconnectButton =
      panel.findChild<QPushButton*>(QStringLiteral("disDisconnectButton"));
  auto* sendButton =
      panel.findChild<QPushButton*>(QStringLiteral("disSendTestPduButton"));
  auto* receiveTable =
      panel.findChild<QTableWidget*>(QStringLiteral("disReceiveTable"));
  if (!address || !connectButton || !disconnectButton || !sendButton ||
      !receiveTable) {
    return 1;
  }

  panel.configure(configuration, true, false);
  if (address->text() != configuration.address || !connectButton->isEnabled() ||
      disconnectButton->isEnabled() || sendButton->isEnabled()) {
    return 2;
  }

  int connectRequests = 0;
  QObject::connect(
      &panel,
      &presentation::DisConnectionPanel::connectRequested,
      [&](const application::DisStartupConfiguration& requested) {
        ++connectRequests;
        if (requested.address != configuration.address ||
            requested.port != configuration.port ||
            requested.exerciseId != configuration.exerciseId) {
          connectRequests = -10;
        }
      });
  connectButton->click();
  if (connectRequests != 1) return 3;

  panel.setConnectionState(presentation::DisConnectionState::Active);
  if (connectButton->isEnabled() || !disconnectButton->isEnabled() ||
      !sendButton->isEnabled()) {
    return 4;
  }

  int testRequests = 0;
  QObject::connect(
      &panel,
      &presentation::DisConnectionPanel::testEntityStateRequested,
      [&](const tactical::dis::EntityState& state) {
        ++testRequests;
        if (state.identifier.entity != 65000 ||
            state.marking != QStringLiteral("DIS-TEST")) {
          testRequests = -10;
        }
      });
  sendButton->click();
  if (testRequests != 1) return 5;
  auto* type = panel.findChild<QComboBox*>(QStringLiteral("disTestPduType"));
  if (!type) return 11;
  int management = 0, iff = 0, collisions = 0;
  QObject::connect(&panel, &presentation::DisConnectionPanel::entityManagementRequested,
      [&](const auto& request) { management += request.remove ? 10 : 1; });
  QObject::connect(&panel, &presentation::DisConnectionPanel::iffRequested, [&](const auto&) { ++iff; });
  QObject::connect(&panel, &presentation::DisConnectionPanel::collisionRequested, [&](const auto&) { ++collisions; });
  for (int i = 1; i <= 4; ++i) { type->setCurrentIndex(i); sendButton->click(); }
  if (management != 11 || iff != 1 || collisions != 1) return 12;
  panel.resize(640, 820);
  panel.show();
  application.processEvents();
  if (qEnvironmentVariableIsSet("DIS_PANEL_SCREENSHOT")) panel.grab().save(qEnvironmentVariable("DIS_PANEL_SCREENSHOT"));

  tactical::dis::EntityState remote;
  remote.identifier = {3, 9, 12};
  remote.marking = QStringLiteral("REMOTE-12");
  remote.latitudeDegrees = 41.0;
  remote.longitudeDegrees = -4.0;
  remote.altitudeMeters = 1250.0;
  panel.recordRemoteEntityChanges({{remote, false}});
  if (receiveTable->rowCount() != 1 ||
      receiveTable->item(0, 3)->text() != remote.marking) {
    return 6;
  }

  tactical::dis::WarfareEvent fire;
  fire.kind = tactical::dis::WarfareEventKind::Fire;
  fire.eventIdentifier = {3, 9, 99};
  fire.munitionType = QStringLiteral("Missile");
  fire.latitudeDegrees = 41.1;
  fire.longitudeDegrees = -4.1;
  fire.altitudeMeters = 1300.0;
  panel.recordRemoteWarfareEvents({fire});
  if (receiveTable->rowCount() != 2 ||
      receiveTable->item(0, 1)->text() != QStringLiteral("Fire") ||
      receiveTable->item(0, 2)->text() != fire.eventIdentifier.key() ||
      receiveTable->item(0, 3)->text() != fire.munitionType) {
    return 10;
  }

  panel.setBlockedByHla(true);
  if (connectButton->isEnabled() || disconnectButton->isEnabled() ||
      sendButton->isEnabled() || receiveTable->isEnabled()) {
    return 7;
  }
  panel.setBlockedByHla(false);
  if (!connectButton->isEnabled() || disconnectButton->isEnabled()) return 8;

  panel.configure(configuration, false, false);
  if (connectButton->isEnabled()) return 9;
  return 0;
}
