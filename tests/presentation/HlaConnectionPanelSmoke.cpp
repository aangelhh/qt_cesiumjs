#include "presentation/HlaConnectionPanel.h"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

int main(int argc, char** argv) {
  QApplication qtApplication(argc, argv);
  presentation::HlaConnectionPanel panel;

  application::HlaStartupConfiguration configuration;
  configuration.backendId = QStringLiteral("mock1516e");
  configuration.localSettingsDesignator = QStringLiteral("mock://local");
  configuration.federationName = QStringLiteral("test-federation");
  configuration.federateName = QStringLiteral("test-federate");
  configuration.federateType = QStringLiteral("integration-test");
  configuration.fomModules = {
      QStringLiteral("RPR-Base.xml"), QStringLiteral("NETN-Base.xml")};
  configuration.timeManagementEnabled = true;
  configuration.timeLookaheadSeconds = 0.05;
  configuration.synchronizationPointLabel = QStringLiteral("READY_TO_RUN");

  auto* federationEdit = panel.findChild<QLineEdit*>(
      QStringLiteral("hlaFederationNameEdit"));
  auto* federateEdit = panel.findChild<QLineEdit*>(
      QStringLiteral("hlaFederateNameEdit"));
  auto* connectButton = panel.findChild<QPushButton*>(
      QStringLiteral("hlaConnectButton"));
  auto* disconnectButton = panel.findChild<QPushButton*>(
      QStringLiteral("hlaDisconnectButton"));
  auto* federateTypeValue = panel.findChild<QLabel*>(
      QStringLiteral("hlaFederateTypeValue"));
  auto* fomModulesValue = panel.findChild<QLabel*>(
      QStringLiteral("hlaFomModulesValue"));
  auto* timeManagementValue = panel.findChild<QLabel*>(
      QStringLiteral("hlaTimeManagementValue"));
  auto* synchronizationPointValue = panel.findChild<QLabel*>(
      QStringLiteral("hlaSynchronizationPointValue"));
  if (!federationEdit || !federateEdit || !connectButton ||
      !disconnectButton || !federateTypeValue || !fomModulesValue ||
      !timeManagementValue || !synchronizationPointValue) {
    return 1;
  }

  panel.configure(configuration, false);
  if (connectButton->isEnabled()) return 2;

  panel.configure(configuration, true);
  if (!connectButton->isEnabled() || disconnectButton->isEnabled()) return 3;
  if (federateTypeValue->text() != configuration.federateType ||
      fomModulesValue->text() != QStringLiteral("2") ||
      !timeManagementValue->text().contains(QStringLiteral("0.05")) ||
      synchronizationPointValue->text() !=
          configuration.synchronizationPointLabel) {
    return 9;
  }

  int connectRequests = 0;
  QString requestedFederation;
  QString requestedFederate;
  QObject::connect(
      &panel,
      &presentation::HlaConnectionPanel::connectRequested,
      [&](const QString& federation, const QString& federate) {
        ++connectRequests;
        requestedFederation = federation;
        requestedFederate = federate;
      });
  connectButton->click();
  if (connectRequests != 1 ||
      requestedFederation != configuration.federationName ||
      requestedFederate != configuration.federateName) {
    return 4;
  }

  panel.setConnectionState(presentation::HlaConnectionState::Federated);
  if (federationEdit->isEnabled() || federateEdit->isEnabled() ||
      connectButton->isEnabled() || !disconnectButton->isEnabled()) {
    return 5;
  }

  int disconnectRequests = 0;
  QObject::connect(
      &panel,
      &presentation::HlaConnectionPanel::disconnectRequested,
      [&]() { ++disconnectRequests; });
  disconnectButton->click();
  if (disconnectRequests != 1) return 6;

  panel.setConnectionState(presentation::HlaConnectionState::Disconnected);
  if (!federationEdit->isEnabled() || !federateEdit->isEnabled() ||
      !connectButton->isEnabled() || disconnectButton->isEnabled()) {
    return 7;
  }

  federationEdit->clear();
  if (connectButton->isEnabled()) return 8;

  return 0;
}
