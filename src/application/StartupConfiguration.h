#pragma once

#include <QString>
#include <QStringList>

class QSettings;

namespace application {

enum class FederationMode {
  Standalone,
  Dis,
  Hla
};

struct DisStartupConfiguration {
  QString address = QStringLiteral("239.1.2.3");
  int port = 3000;
  int siteId = 1;
  int applicationId = 1;
};

struct HlaStartupConfiguration {
  QString backendId;
  QString backendLibraryPath;
  QString localSettingsDesignator;
  QString federationName = QStringLiteral("qttest-federation");
  QString federateName = QStringLiteral("qttest-01");
  QString federateType = QStringLiteral("qttest");
  QString synchronizationPointLabel;
  bool timeManagementEnabled = false;
  double timeLookaheadSeconds = 0.01;
  QStringList fomModules;
  bool createFederationIfMissing = true;
};

struct Ros2StartupConfiguration {
  bool enabled = false;
  int domainId = 0;
  QString topicPrefix = QStringLiteral("/qttest/entities");
};

struct CigiStartupConfiguration {
  bool enabled = false;
  QString host = QStringLiteral("127.0.0.1");
  int port = 8004;
};

struct StartupConfiguration {
  FederationMode federationMode = FederationMode::Standalone;
  DisStartupConfiguration dis;
  HlaStartupConfiguration hla;
  Ros2StartupConfiguration ros2;
  CigiStartupConfiguration cigi;

  QString validationError() const;
  QString modeDisplayName() const;
  void addMissingHlaFomModules(const QString& fomDirectoryPath);

  static StartupConfiguration load(QSettings& settings);
  void save(QSettings& settings) const;
};

} // namespace application
