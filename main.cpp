#include "src/MainWindow.h"
#include "application/HlaStartupSession.h"
#include "application/StartupConfiguration.h"
#include "presentation/StartupConfigurationDialog.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QSet>
#include <QTimer>

#include <algorithm>

namespace {

QVector<presentation::HlaBackendOption> availableHlaBackends() {
  QVector<presentation::HlaBackendOption> backends;
#ifdef QTTEST_HLA_OPENRTI_PLUGIN_PATH
  if (QFileInfo::exists(QStringLiteral(QTTEST_HLA_OPENRTI_PLUGIN_PATH))) {
    backends.push_back({
        QStringLiteral("openrti1516e"),
        QStringLiteral("OpenRTI (IEEE 1516e)"),
        QStringLiteral(QTTEST_HLA_OPENRTI_PLUGIN_PATH)});
  }
#endif
#ifdef QTTEST_HLA_PITCH_PLUGIN_PATH
  if (QFileInfo::exists(QStringLiteral(QTTEST_HLA_PITCH_PLUGIN_PATH))) {
    backends.push_back({
        QStringLiteral("pitch1516e"),
        QStringLiteral("Pitch pRTI (IEEE 1516e)"),
        QStringLiteral(QTTEST_HLA_PITCH_PLUGIN_PATH)});
  }
#endif
  return backends;
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("qttest"));
    QCoreApplication::setApplicationName(QStringLiteral("qttest"));

    QCommandLineParser commandLine;
    commandLine.setApplicationDescription(
        QStringLiteral("Tactical entity simulation and interoperability runtime."));
    commandLine.addHelpOption();
    commandLine.addOption({
        QStringLiteral("hla-combat-demo"),
        QStringLiteral("Start the graphical two-aircraft HLA combat demo.")});
    commandLine.addOption({
        QStringLiteral("hla-federation"),
        QStringLiteral("Federation name used by the HLA combat demo."),
        QStringLiteral("name"),
        QStringLiteral("qttest-federation")});
    commandLine.addOption({
        QStringLiteral("hla-local-settings"),
        QStringLiteral("Local settings designator used by the HLA combat demo."),
        QStringLiteral("value"),
        QStringLiteral("crcAddress=localhost:8989")});
    commandLine.process(app);
    const bool hlaCombatDemo =
        commandLine.isSet(QStringLiteral("hla-combat-demo"));

    QSettings settings;
    application::StartupConfiguration startupConfiguration =
        application::StartupConfiguration::load(settings);
    startupConfiguration.addMissingHlaFomModules(
        QDir(QStringLiteral(QTTEST_SOURCE_DIR))
            .filePath(QStringLiteral("src/infrastructure/hla/FOM")));
    application::HlaStartupSession hlaSession;
    const QVector<presentation::HlaBackendOption> hlaBackends =
        availableHlaBackends();
    if (hlaCombatDemo) {
      const auto pitchBackend = std::find_if(
          hlaBackends.cbegin(), hlaBackends.cend(),
          [](const presentation::HlaBackendOption& backend) {
            return backend.id == QStringLiteral("pitch1516e");
          });
      if (pitchBackend == hlaBackends.cend()) {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("HLA combat demo"),
            QStringLiteral("The Pitch pRTI backend is not available in this build."));
        return 1;
      }
      startupConfiguration.federationMode = application::FederationMode::Hla;
      startupConfiguration.hla.backendId = pitchBackend->id;
      startupConfiguration.hla.backendLibraryPath = pitchBackend->libraryPath;
      startupConfiguration.hla.localSettingsDesignator =
          commandLine.value(QStringLiteral("hla-local-settings"));
      startupConfiguration.hla.federationName =
          commandLine.value(QStringLiteral("hla-federation"));
      startupConfiguration.hla.federateName = QStringLiteral("qttest-combat-demo-%1")
                                                  .arg(QCoreApplication::applicationPid());
      startupConfiguration.hla.federateType = QStringLiteral("qttest-combat-demo");
      startupConfiguration.hla.createFederationIfMissing = true;
      const tactical::hla::Result hlaResult =
          hlaSession.start(startupConfiguration.hla);
      if (!hlaResult.success) {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("HLA combat demo startup failed"),
            QString::fromStdString(hlaResult.message));
        return 1;
      }
    } else {
      presentation::StartupConfigurationDialog startupDialog(
          startupConfiguration,
          hlaBackends,
#ifdef QTTEST_HAS_ROS2_TELEMETRY
          true
#else
          false
#endif
      );
      while (true) {
        if (startupDialog.exec() != QDialog::Accepted) {
          return 0;
        }
        startupConfiguration = startupDialog.configuration();
        startupConfiguration.save(settings);

        if (startupConfiguration.federationMode !=
            application::FederationMode::Hla) {
          break;
        }

        const tactical::hla::Result hlaResult =
            hlaSession.start(startupConfiguration.hla);
        if (hlaResult.success) {
          qInfo().noquote()
              << "HLA joined:"
              << startupConfiguration.hla.federateName
              << "backend=" << startupConfiguration.hla.backendId
              << "federation=" << startupConfiguration.hla.federationName;
          break;
        }
        QMessageBox::critical(
            &startupDialog,
            QStringLiteral("HLA startup failed"),
            QString::fromStdString(hlaResult.message));
      }
    }

    MainWindow window;
    window.setWindowTitle(
        QStringLiteral("%1 [%2]")
            .arg(window.windowTitle(), startupConfiguration.modeDisplayName()));
    window.show();
    if (hlaCombatDemo) {
      QTimer::singleShot(0, &window, &MainWindow::startHlaCombatDemo);
    }

    QTimer hlaPollTimer;
    QTimer hlaPublishTimer;
    qsizetype lastPublishedEntityCount = -1;
    if (hlaSession.isActive()) {
      const QString synchronizationPointLabel =
          startupConfiguration.hla.synchronizationPointLabel.trimmed();
      QSet<QString> achievedSynchronizationPoints;
      if (!synchronizationPointLabel.isEmpty()) {
        const tactical::hla::Result synchronizationResult =
            hlaSession.registerSynchronizationPoint(
                synchronizationPointLabel.toStdString());
        if (synchronizationResult.success) {
          window.reportHlaSynchronizationStatus(
              QStringLiteral("registration requested for %1")
                  .arg(synchronizationPointLabel));
        } else {
          window.reportHlaSynchronizationStatus(
              QStringLiteral("registration failed for %1: %2")
                  .arg(
                      synchronizationPointLabel,
                      QString::fromStdString(synchronizationResult.message)));
        }
      }
      QObject::connect(
          &window,
          &MainWindow::hlaSimulationControlRequested,
          &window,
          [&](tactical::hla::RemoteSimulationControl control,
              double simulationTimeSeconds) {
            const tactical::hla::Result result =
                hlaSession.publishSimulationControl(
                    control, simulationTimeSeconds);
            if (!result.success) {
              qCritical().noquote()
                  << "HLA simulation control publication failed:"
                  << QString::fromStdString(result.message);
            }
          });
      hlaPollTimer.setInterval(20);
      QObject::connect(&hlaPollTimer, &QTimer::timeout, &window, [&]() {
        const tactical::hla::Result result = hlaSession.poll(0.0);
        if (!result.success) {
          qCritical().noquote()
              << "HLA callback polling failed:"
              << QString::fromStdString(result.message);
          hlaPollTimer.stop();
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA callback error]"));
          return;
        }
        window.applyHlaRemoteEntityChanges(
            hlaSession.takeRemoteEntityChanges());
        window.applyHlaRemoteMunitionChanges(
            hlaSession.takeRemoteMunitionChanges());
        window.applyHlaRemoteSensorChanges(
            hlaSession.takeRemoteSensorChanges());
        window.applyHlaRemoteWarfareEvents(
            hlaSession.takeRemoteWarfareEvents());
        for (const tactical::hla::RemoteSynchronizationChange& change :
             hlaSession.takeRemoteSynchronizationChanges()) {
          const QString label = QString::fromStdString(change.label);
          if (change.federationSynchronized) {
            window.reportHlaSynchronizationStatus(
                QStringLiteral("federation synchronized at %1").arg(label));
            continue;
          }
          window.reportHlaSynchronizationStatus(
              QStringLiteral("point announced: %1").arg(label));
          if (label != synchronizationPointLabel ||
              achievedSynchronizationPoints.contains(label)) {
            continue;
          }
          const tactical::hla::Result achieveResult =
              hlaSession.achieveSynchronizationPoint(change.label);
          if (achieveResult.success) {
            achievedSynchronizationPoints.insert(label);
            window.reportHlaSynchronizationStatus(
                QStringLiteral("achieved %1").arg(label));
          } else {
            window.reportHlaSynchronizationStatus(
                QStringLiteral("failed to achieve %1: %2")
                    .arg(label, QString::fromStdString(achieveResult.message)));
          }
        }
        for (const tactical::hla::RemoteSimulationControl control :
             hlaSession.takeRemoteSimulationControls()) {
          window.applyHlaRemoteSimulationControl(control);
        }
      });
      hlaPollTimer.start();

      hlaPublishTimer.setInterval(100);
      QObject::connect(&hlaPublishTimer, &QTimer::timeout, &window, [&]() {
        const QVector<Entity> entities = window.entitySnapshot();
        const qsizetype localEntityCount = std::count_if(
            entities.cbegin(), entities.cend(), [](const Entity& entity) {
              return !entity.externallyControlled;
            });
        const tactical::hla::Result result =
            hlaSession.publishEntities(entities);
        if (!result.success) {
          qCritical().noquote()
              << "HLA entity publication failed:"
              << QString::fromStdString(result.message);
          hlaPublishTimer.stop();
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA publish error]"));
          return;
        }
        if (lastPublishedEntityCount != localEntityCount) {
          lastPublishedEntityCount = localEntityCount;
          qInfo().noquote()
              << "HLA entity synchronization active:"
              << lastPublishedEntityCount
              << "local object(s) registered and updating at 10 Hz via"
              << hlaSession.backendId();
        }
        const tactical::hla::Result detonationResult =
            hlaSession.publishDetonations(window.transientEffectSnapshot());
        if (!detonationResult.success) {
          qCritical().noquote()
              << "HLA detonation publication failed:"
              << QString::fromStdString(detonationResult.message);
          hlaPublishTimer.stop();
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA detonation error]"));
          return;
        }
        const tactical::hla::Result warfareResult =
            hlaSession.publishMunitions(window.activeMunitionSnapshot());
        if (!warfareResult.success) {
          qCritical().noquote()
              << "HLA interaction publication failed:"
              << QString::fromStdString(warfareResult.message);
          hlaPublishTimer.stop();
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA interaction error]"));
          return;
        }
        const tactical::hla::Result sensorResult =
            hlaSession.publishSensors(entities);
        if (!sensorResult.success) {
          qCritical().noquote()
              << "HLA sensor publication failed:"
              << QString::fromStdString(sensorResult.message);
          hlaPublishTimer.stop();
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA sensor error]"));
        }
      });
      hlaPublishTimer.start();
    }

    const int result = app.exec();
    hlaSession.stop();
    return result;
}
