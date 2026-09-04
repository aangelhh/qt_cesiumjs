#include "application/StartupConfiguration.h"

#include <QFileInfo>
#include <QDir>
#include <QSettings>

namespace application {
namespace {

QString modeKey(FederationMode mode) {
  switch (mode) {
  case FederationMode::Dis:
    return QStringLiteral("dis");
  case FederationMode::Hla:
    return QStringLiteral("hla");
  case FederationMode::Standalone:
    return QStringLiteral("standalone");
  }
  return QStringLiteral("standalone");
}

FederationMode modeFromKey(const QString& key) {
  if (key.compare(QStringLiteral("dis"), Qt::CaseInsensitive) == 0) {
    return FederationMode::Dis;
  }
  if (key.compare(QStringLiteral("hla"), Qt::CaseInsensitive) == 0) {
    return FederationMode::Hla;
  }
  return FederationMode::Standalone;
}

} // namespace

QString StartupConfiguration::validationError() const {
  if (federationMode == FederationMode::Dis) {
    if (dis.address.trimmed().isEmpty()) {
      return QStringLiteral("DIS address is required.");
    }
    if (dis.port < 1 || dis.port > 65535) {
      return QStringLiteral("DIS port must be between 1 and 65535.");
    }
  }

  if (federationMode == FederationMode::Hla) {
    if (hla.backendId.trimmed().isEmpty()) {
      return QStringLiteral("An HLA backend is required.");
    }
    if (hla.backendLibraryPath.trimmed().isEmpty() ||
        !QFileInfo::exists(hla.backendLibraryPath)) {
      return QStringLiteral("The selected HLA backend library is unavailable.");
    }
    if (hla.federationName.trimmed().isEmpty() ||
        hla.federateName.trimmed().isEmpty() ||
        hla.federateType.trimmed().isEmpty()) {
      return QStringLiteral(
          "HLA federation, federate name, and federate type are required.");
    }
  }

  if (ros2.enabled && ros2.topicPrefix.trimmed().isEmpty()) {
    return QStringLiteral("ROS 2 topic prefix is required when enabled.");
  }
  if (ros2.domainId < 0 || ros2.domainId > 232) {
    return QStringLiteral("ROS 2 domain ID must be between 0 and 232.");
  }

  if (cigi.enabled) {
    if (cigi.host.trimmed().isEmpty()) {
      return QStringLiteral("CIGI host is required when enabled.");
    }
    if (cigi.port < 1 || cigi.port > 65535) {
      return QStringLiteral("CIGI port must be between 1 and 65535.");
    }
  }
  return {};
}

QString StartupConfiguration::modeDisplayName() const {
  switch (federationMode) {
  case FederationMode::Dis:
    return QStringLiteral("DIS");
  case FederationMode::Hla:
    return QStringLiteral("HLA");
  case FederationMode::Standalone:
    return QStringLiteral("Standalone");
  }
  return QStringLiteral("Standalone");
}

void StartupConfiguration::addMissingHlaFomModules(
    const QString& fomDirectoryPath) {
  const QDir fomDirectory(fomDirectoryPath);
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

  for (const QString& moduleName : moduleNames) {
    const QString modulePath = fomDirectory.absoluteFilePath(moduleName);
    if (QFileInfo::exists(modulePath) && !hla.fomModules.contains(modulePath)) {
      hla.fomModules.push_back(modulePath);
    }
  }
}

StartupConfiguration StartupConfiguration::load(QSettings& settings) {
  StartupConfiguration configuration;
  settings.beginGroup(QStringLiteral("Startup"));
  configuration.federationMode = modeFromKey(
      settings.value(QStringLiteral("federationMode"), QStringLiteral("standalone"))
          .toString());

  configuration.dis.address = settings.value(
      QStringLiteral("dis/address"), configuration.dis.address).toString();
  configuration.dis.port = settings.value(
      QStringLiteral("dis/port"), configuration.dis.port).toInt();
  configuration.dis.siteId = settings.value(
      QStringLiteral("dis/siteId"), configuration.dis.siteId).toInt();
  configuration.dis.applicationId = settings.value(
      QStringLiteral("dis/applicationId"), configuration.dis.applicationId).toInt();

  configuration.hla.backendId = settings.value(
      QStringLiteral("hla/backendId")).toString();
  configuration.hla.backendLibraryPath = settings.value(
      QStringLiteral("hla/backendLibraryPath")).toString();
  configuration.hla.localSettingsDesignator = settings.value(
      QStringLiteral("hla/localSettingsDesignator")).toString();
  configuration.hla.federationName = settings.value(
      QStringLiteral("hla/federationName"), configuration.hla.federationName)
          .toString();
  configuration.hla.federateName = settings.value(
      QStringLiteral("hla/federateName"), configuration.hla.federateName)
          .toString();
  configuration.hla.federateType = settings.value(
      QStringLiteral("hla/federateType"), configuration.hla.federateType)
          .toString();
  configuration.hla.synchronizationPointLabel = settings.value(
      QStringLiteral("hla/synchronizationPointLabel")).toString();
  configuration.hla.fomModules = settings.value(
      QStringLiteral("hla/fomModules")).toStringList();
  configuration.hla.createFederationIfMissing = settings.value(
      QStringLiteral("hla/createFederationIfMissing"), true).toBool();

  configuration.ros2.enabled = settings.value(
      QStringLiteral("ros2/enabled"), false).toBool();
  configuration.ros2.domainId = settings.value(
      QStringLiteral("ros2/domainId"), 0).toInt();
  configuration.ros2.topicPrefix = settings.value(
      QStringLiteral("ros2/topicPrefix"), configuration.ros2.topicPrefix)
          .toString();

  configuration.cigi.enabled = settings.value(
      QStringLiteral("cigi/enabled"), false).toBool();
  configuration.cigi.host = settings.value(
      QStringLiteral("cigi/host"), configuration.cigi.host).toString();
  configuration.cigi.port = settings.value(
      QStringLiteral("cigi/port"), configuration.cigi.port).toInt();
  settings.endGroup();
  return configuration;
}

void StartupConfiguration::save(QSettings& settings) const {
  settings.beginGroup(QStringLiteral("Startup"));
  settings.setValue(QStringLiteral("federationMode"), modeKey(federationMode));
  settings.setValue(QStringLiteral("dis/address"), dis.address);
  settings.setValue(QStringLiteral("dis/port"), dis.port);
  settings.setValue(QStringLiteral("dis/siteId"), dis.siteId);
  settings.setValue(QStringLiteral("dis/applicationId"), dis.applicationId);
  settings.setValue(QStringLiteral("hla/backendId"), hla.backendId);
  settings.setValue(
      QStringLiteral("hla/backendLibraryPath"), hla.backendLibraryPath);
  settings.setValue(
      QStringLiteral("hla/localSettingsDesignator"),
      hla.localSettingsDesignator);
  settings.setValue(QStringLiteral("hla/federationName"), hla.federationName);
  settings.setValue(QStringLiteral("hla/federateName"), hla.federateName);
  settings.setValue(QStringLiteral("hla/federateType"), hla.federateType);
  settings.setValue(
      QStringLiteral("hla/synchronizationPointLabel"),
      hla.synchronizationPointLabel);
  settings.setValue(QStringLiteral("hla/fomModules"), hla.fomModules);
  settings.setValue(
      QStringLiteral("hla/createFederationIfMissing"),
      hla.createFederationIfMissing);
  settings.setValue(QStringLiteral("ros2/enabled"), ros2.enabled);
  settings.setValue(QStringLiteral("ros2/domainId"), ros2.domainId);
  settings.setValue(QStringLiteral("ros2/topicPrefix"), ros2.topicPrefix);
  settings.setValue(QStringLiteral("cigi/enabled"), cigi.enabled);
  settings.setValue(QStringLiteral("cigi/host"), cigi.host);
  settings.setValue(QStringLiteral("cigi/port"), cigi.port);
  settings.endGroup();

  // MainWindow already consumes this group for the native ROS 2 publisher.
  settings.beginGroup(QStringLiteral("Ros2Telemetry"));
  settings.setValue(QStringLiteral("ros2Enabled"), ros2.enabled);
  settings.setValue(QStringLiteral("topicPrefix"), ros2.topicPrefix);
  settings.endGroup();
  settings.sync();
}

} // namespace application
