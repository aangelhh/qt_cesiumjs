#include "src/MainWindow.h"
#include "application/HlaStartupSession.h"
#ifdef QTTEST_HAS_DIS
#include "application/DisStartupSession.h"
#endif
#include "application/StartupConfiguration.h"
#include "presentation/StartupConfigurationDialog.h"

#include <QApplication>
#include <QColor>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QPalette>
#include <QSettings>
#include <QSet>
#include <QStringList>
#include <QStyleFactory>
#include <QTimer>

#include <algorithm>

namespace {

QString firstExistingFile(const QStringList& candidates) {
  for (const QString& candidate : candidates) {
    if (candidate.trimmed().isEmpty()) continue;
    const QFileInfo info(candidate);
    if (info.exists() && info.isFile()) return info.absoluteFilePath();
  }
  return {};
}

QString firstExistingDirectory(const QStringList& candidates) {
  for (const QString& candidate : candidates) {
    if (candidate.trimmed().isEmpty()) continue;
    const QFileInfo info(candidate);
    if (info.exists() && info.isDir()) return info.absoluteFilePath();
  }
  return {};
}

QString runtimeFile(const QString& relativePath) {
  QStringList candidates;
  QDir cursor(QCoreApplication::applicationDirPath());
  for (int depth = 0; depth < 6; ++depth) {
    candidates.push_back(cursor.absoluteFilePath(relativePath));
    if (!cursor.cdUp()) break;
  }
#ifdef QTTEST_SOURCE_DIR
  candidates.push_back(
      QDir(QStringLiteral(QTTEST_SOURCE_DIR)).absoluteFilePath(relativePath));
#endif
  candidates.push_back(QDir::current().absoluteFilePath(relativePath));
  return firstExistingFile(candidates);
}

QString runtimeDirectory(const QString& relativePath) {
  QStringList candidates;
  QDir cursor(QCoreApplication::applicationDirPath());
  for (int depth = 0; depth < 6; ++depth) {
    candidates.push_back(cursor.absoluteFilePath(relativePath));
    if (!cursor.cdUp()) break;
  }
#ifdef QTTEST_SOURCE_DIR
  candidates.push_back(
      QDir(QStringLiteral(QTTEST_SOURCE_DIR)).absoluteFilePath(relativePath));
#endif
  candidates.push_back(QDir::current().absoluteFilePath(relativePath));
  return firstExistingDirectory(candidates);
}

QString hlaPluginFileName(const QString& targetName) {
#ifdef Q_OS_WIN
  return targetName + QStringLiteral(".dll");
#elif defined(Q_OS_MACOS)
  return QStringLiteral("lib") + targetName + QStringLiteral(".dylib");
#else
  return QStringLiteral("lib") + targetName + QStringLiteral(".so");
#endif
}

QString locateHlaPlugin(
    const char* environmentVariable,
    const QString& targetName,
    const QString& developmentFallback = {}) {
  QStringList candidates;
  const QByteArray environmentValue = qgetenv(environmentVariable);
  if (!environmentValue.isEmpty()) {
    candidates.push_back(QString::fromLocal8Bit(environmentValue));
  }

  const QString pluginFileName = hlaPluginFileName(targetName);
  QDir cursor(QCoreApplication::applicationDirPath());
  for (int depth = 0; depth < 6; ++depth) {
    candidates.push_back(
        cursor.absoluteFilePath(QStringLiteral("hla-plugins/%1").arg(pluginFileName)));
    if (!cursor.cdUp()) break;
  }
  candidates.push_back(
      QDir::current().absoluteFilePath(
          QStringLiteral("hla-plugins/%1").arg(pluginFileName)));
  if (!developmentFallback.trimmed().isEmpty()) {
    candidates.push_back(developmentFallback);
  }
  return firstExistingFile(candidates);
}

QVector<presentation::HlaBackendOption> availableHlaBackends() {
  QVector<presentation::HlaBackendOption> backends;

  QString openRtiDevelopmentFallback;
#ifdef QTTEST_HLA_OPENRTI_PLUGIN_PATH
  openRtiDevelopmentFallback = QStringLiteral(QTTEST_HLA_OPENRTI_PLUGIN_PATH);
#endif
  const QString openRtiPlugin = locateHlaPlugin(
      "QTTEST_HLA_OPENRTI_PLUGIN",
      QStringLiteral("qttest_hla_openrti1516e_backend"),
      openRtiDevelopmentFallback);
  if (!openRtiPlugin.isEmpty()) {
    backends.push_back({
        QStringLiteral("openrti1516e"),
        QStringLiteral("OpenRTI (IEEE 1516e)"),
        openRtiPlugin});
  }

  QString pitchDevelopmentFallback;
#ifdef QTTEST_HLA_PITCH_PLUGIN_PATH
  pitchDevelopmentFallback = QStringLiteral(QTTEST_HLA_PITCH_PLUGIN_PATH);
#endif
  const QString pitchPlugin = locateHlaPlugin(
      "QTTEST_HLA_PITCH_PLUGIN",
      QStringLiteral("qttest_hla_pitch1516e_backend"),
      pitchDevelopmentFallback);
  if (!pitchPlugin.isEmpty()) {
    backends.push_back({
        QStringLiteral("pitch1516e"),
        QStringLiteral("Pitch pRTI (IEEE 1516e)"),
        pitchPlugin});
  }

  return backends;
}

void applyWindowsTheme(QApplication& app) {
#ifdef Q_OS_WIN
  const QStringList availableStyles = QStyleFactory::keys();
  QString selectedStyle = QStringLiteral("Fusion");
  for (const QString& style : availableStyles) {
    if (style.compare(QStringLiteral("windows11"), Qt::CaseInsensitive) == 0) {
      selectedStyle = style;
      break;
    }
  }
  app.setStyle(QStyleFactory::create(selectedStyle));
  QPalette palette = app.palette();
  palette.setColor(QPalette::Window, QColor(QStringLiteral("#f7f9fc")));
  palette.setColor(QPalette::WindowText, QColor(QStringLiteral("#172033")));
  palette.setColor(QPalette::Base, QColor(QStringLiteral("#ffffff")));
  palette.setColor(QPalette::AlternateBase, QColor(QStringLiteral("#f4f7fb")));
  palette.setColor(QPalette::Text, QColor(QStringLiteral("#172033")));
  palette.setColor(QPalette::Button, QColor(QStringLiteral("#ffffff")));
  palette.setColor(QPalette::ButtonText, QColor(QStringLiteral("#172033")));
  palette.setColor(QPalette::Highlight, QColor(QStringLiteral("#0f6cbd")));
  palette.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#ffffff")));
  app.setPalette(palette);
  qInfo() << "Windows UI style:" << selectedStyle
          << "available styles:" << availableStyles;

  QString styleSheetPath = QStringLiteral(":/Data/windows-modern.qss");
  if (!QFileInfo::exists(styleSheetPath)) {
    styleSheetPath = runtimeFile(QStringLiteral("Data/windows-modern.qss"));
  }
  if (styleSheetPath.isEmpty() || !QFileInfo::exists(styleSheetPath)) {
    qWarning() << "Windows modern stylesheet not found; using Qt style only";
    return;
  }

  QFile styleSheetFile(styleSheetPath);
  if (!styleSheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Unable to open Windows modern stylesheet:" << styleSheetPath;
    return;
  }
  app.setStyleSheet(QString::fromUtf8(styleSheetFile.readAll()));
#endif
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    applyWindowsTheme(app);
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
    QString hlaFomPath = runtimeDirectory(QStringLiteral("src/infrastructure/hla/FOM"));
    if (hlaFomPath.isEmpty()) {
      hlaFomPath = QDir(QCoreApplication::applicationDirPath())
                       .absoluteFilePath(QStringLiteral("src/infrastructure/hla/FOM"));
    }
    startupConfiguration.addMissingHlaFomModules(hlaFomPath);
    application::HlaStartupSession hlaSession;
#ifdef QTTEST_HAS_DIS
    application::DisStartupSession disSession;
#endif
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

        if (startupConfiguration.federationMode ==
            application::FederationMode::Standalone) {
          break;
        }

#ifdef QTTEST_HAS_DIS
        if (startupConfiguration.federationMode ==
            application::FederationMode::Dis) {
          const tactical::dis::Result disResult =
              disSession.start(startupConfiguration.dis);
          if (disResult.success) {
            qInfo().noquote()
                << "DIS gateway active:"
                << startupConfiguration.dis.address
                << "port=" << startupConfiguration.dis.port
                << "exercise=" << startupConfiguration.dis.exerciseId
                << "site/application="
                << QStringLiteral("%1/%2")
                       .arg(startupConfiguration.dis.siteId)
                       .arg(startupConfiguration.dis.applicationId);
            break;
          }
          QMessageBox::critical(
              &startupDialog,
              QStringLiteral("DIS startup failed"),
              disResult.message);
          continue;
        }
#endif

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
    if (hlaSession.isTimeManagementActive()) {
      window.setHlaTimeManagementActive(true);
    }
    if (hlaCombatDemo) {
      QTimer::singleShot(0, &window, &MainWindow::startHlaCombatDemo);
    }

    QTimer hlaPollTimer;
    QTimer hlaPublishTimer;
    qsizetype lastPublishedEntityCount = -1;
    {
      QString synchronizationPointLabel =
          startupConfiguration.hla.synchronizationPointLabel.trimmed();
      QSet<QString> achievedSynchronizationPoints;
      if (hlaSession.isActive() && !synchronizationPointLabel.isEmpty()) {
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
            if (!hlaSession.isActive()) return;
            const tactical::hla::Result result =
                hlaSession.publishSimulationControl(
                    control, simulationTimeSeconds);
            if (!result.success) {
              qCritical().noquote()
                  << "HLA simulation control publication failed:"
                  << QString::fromStdString(result.message);
            }
          });
      QObject::connect(
          &window,
          &MainWindow::hlaTimeAdvanceRequested,
          &window,
          [&](double logicalTimeSeconds) {
            if (!hlaSession.isActive()) return;
            const tactical::hla::Result result =
                hlaSession.requestTimeAdvance(logicalTimeSeconds);
            if (!result.success) {
              window.reportHlaSynchronizationStatus(
                  QStringLiteral("time advance request failed: %1")
                      .arg(QString::fromStdString(result.message)));
              window.applyHlaTimeAdvanceGrant(
                  hlaSession.grantedLogicalTimeSeconds());
            }
          });
      hlaPollTimer.setInterval(20);
      QObject::connect(&hlaPollTimer, &QTimer::timeout, &window, [&]() {
        if (!hlaSession.isActive()) return;
        const tactical::hla::Result result = hlaSession.poll(0.0);
        if (!result.success) {
          const QString detail = QString::fromStdString(result.message);
          qCritical().noquote()
              << "HLA callback polling failed:"
              << detail;
          hlaPollTimer.stop();
          hlaPublishTimer.stop();
          hlaSession.stop();
          window.setHlaTimeManagementActive(false);
          window.setHlaConnectionState(false, false, detail);
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA callback error]"));
          return;
        }
        const std::vector<tactical::hla::ConnectionLostEvent>
            connectionLosses = hlaSession.takeConnectionLostEvents();
        if (!connectionLosses.empty()) {
          const QString reason = QString::fromStdString(
              connectionLosses.back().reason).trimmed();
          const QString detail = QStringLiteral("RTI connection lost: %1")
              .arg(reason.isEmpty()
                       ? QStringLiteral("no fault description provided")
                       : reason);
          qCritical().noquote() << detail;
          hlaPollTimer.stop();
          hlaPublishTimer.stop();
          hlaSession.stop();
          window.setHlaTimeManagementActive(false);
          window.setHlaConnectionState(false, false, detail);
          window.reportHlaSynchronizationStatus(detail);
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
          if (change.registrationCompleted) {
            if (change.registrationSucceeded) {
              window.reportHlaSynchronizationStatus(
                  QStringLiteral("registration accepted for %1").arg(label));
            } else {
              window.reportHlaSynchronizationStatus(
                  QStringLiteral("registration rejected for %1: %2")
                      .arg(label, QString::fromStdString(change.reason)));
            }
            continue;
          }
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
        for (const tactical::hla::RemoteTimeManagementEvent& event :
             hlaSession.takeRemoteTimeManagementEvents()) {
          switch (event.kind) {
            case tactical::hla::RemoteTimeManagementEventKind::RegulationEnabled:
              window.reportHlaSynchronizationStatus(
                  QStringLiteral("time regulation enabled at %1 s")
                      .arg(event.logicalTimeSeconds, 0, 'f', 3));
              break;
            case tactical::hla::RemoteTimeManagementEventKind::ConstrainedEnabled:
              window.reportHlaSynchronizationStatus(
                  QStringLiteral("time constrained enabled at %1 s")
                      .arg(event.logicalTimeSeconds, 0, 'f', 3));
              break;
            case tactical::hla::RemoteTimeManagementEventKind::AdvanceGranted:
              window.applyHlaTimeAdvanceGrant(event.logicalTimeSeconds);
              break;
          }
        }
        for (const tactical::hla::RemoteSimulationControl control :
             hlaSession.takeRemoteSimulationControls()) {
          window.applyHlaRemoteSimulationControl(control);
        }
      });
      if (hlaSession.isActive()) hlaPollTimer.start();

      hlaPublishTimer.setInterval(100);
      QObject::connect(&hlaPublishTimer, &QTimer::timeout, &window, [&]() {
        if (!hlaSession.isActive()) return;
        const QVector<Entity> entities = window.entitySnapshot();
        const qsizetype localEntityCount = std::count_if(
            entities.cbegin(), entities.cend(), [](const Entity& entity) {
              return !entity.externallyControlled;
            });
        const tactical::hla::Result result =
            hlaSession.publishEntities(entities);
        if (!result.success) {
          const QString detail = QString::fromStdString(result.message);
          qCritical().noquote()
              << "HLA entity publication failed:"
              << detail;
          hlaPublishTimer.stop();
          hlaPollTimer.stop();
          hlaSession.stop();
          window.setHlaTimeManagementActive(false);
          window.setHlaConnectionState(false, false, detail);
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
          const QString detail =
              QString::fromStdString(detonationResult.message);
          qCritical().noquote()
              << "HLA detonation publication failed:"
              << detail;
          hlaPublishTimer.stop();
          hlaPollTimer.stop();
          hlaSession.stop();
          window.setHlaTimeManagementActive(false);
          window.setHlaConnectionState(false, false, detail);
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA detonation error]"));
          return;
        }
        const tactical::hla::Result warfareResult =
            hlaSession.publishMunitions(window.activeMunitionSnapshot());
        if (!warfareResult.success) {
          const QString detail =
              QString::fromStdString(warfareResult.message);
          qCritical().noquote()
              << "HLA interaction publication failed:"
              << detail;
          hlaPublishTimer.stop();
          hlaPollTimer.stop();
          hlaSession.stop();
          window.setHlaTimeManagementActive(false);
          window.setHlaConnectionState(false, false, detail);
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA interaction error]"));
          return;
        }
        const tactical::hla::Result sensorResult =
            hlaSession.publishSensors(entities);
        if (!sensorResult.success) {
          const QString detail = QString::fromStdString(sensorResult.message);
          qCritical().noquote()
              << "HLA sensor publication failed:"
              << detail;
          hlaPublishTimer.stop();
          hlaPollTimer.stop();
          hlaSession.stop();
          window.setHlaTimeManagementActive(false);
          window.setHlaConnectionState(false, false, detail);
          window.setWindowTitle(
              window.windowTitle() + QStringLiteral(" [HLA sensor error]"));
        }
      });
      if (hlaSession.isActive()) hlaPublishTimer.start();

      const bool hlaBackendAvailable =
          !startupConfiguration.hla.backendId.trimmed().isEmpty() &&
          !startupConfiguration.hla.backendLibraryPath.trimmed().isEmpty() &&
          QFileInfo::exists(startupConfiguration.hla.backendLibraryPath);
      window.configureHlaConnection(
          startupConfiguration.hla,
          hlaBackendAvailable,
          hlaSession.isActive());
#ifdef QTTEST_HAS_DIS
      if (disSession.isActive()) window.setHlaBlockedByDis(true);
#endif

      QObject::connect(
          &window,
          &MainWindow::hlaConnectRequested,
          &window,
          [&](const QString& federationName, const QString& federateName) {
#ifdef QTTEST_HAS_DIS
            if (disSession.isActive()) {
              window.setHlaConnectionState(
                  false, false,
                  QStringLiteral("Disconnect DIS before connecting HLA."));
              return;
            }
#endif
            hlaPollTimer.stop();
            hlaPublishTimer.stop();
            lastPublishedEntityCount = -1;
            achievedSynchronizationPoints.clear();
            window.setHlaConnectionState(
                false,
                true,
                QStringLiteral("Joining %1 as %2...")
                    .arg(federationName, federateName));

            startupConfiguration.federationMode =
                application::FederationMode::Hla;
            startupConfiguration.hla.federationName = federationName;
            startupConfiguration.hla.federateName = federateName;
            startupConfiguration.save(settings);
            synchronizationPointLabel =
                startupConfiguration.hla.synchronizationPointLabel.trimmed();

            const tactical::hla::Result result =
                hlaSession.start(startupConfiguration.hla);
            if (!result.success) {
              const QString detail = QString::fromStdString(result.message);
              window.setHlaConnectionState(false, false, detail);
              window.reportHlaSynchronizationStatus(
                  QStringLiteral("connection failed: %1").arg(detail));
              return;
            }

            if (!synchronizationPointLabel.isEmpty()) {
              const tactical::hla::Result synchronizationResult =
                  hlaSession.registerSynchronizationPoint(
                      synchronizationPointLabel.toStdString());
              window.reportHlaSynchronizationStatus(
                  synchronizationResult.success
                      ? QStringLiteral("registration requested for %1")
                            .arg(synchronizationPointLabel)
                      : QStringLiteral("registration failed for %1: %2")
                            .arg(
                                synchronizationPointLabel,
                                QString::fromStdString(
                                    synchronizationResult.message)));
            }

            window.setHlaTimeManagementActive(
                hlaSession.isTimeManagementActive());
            window.configureHlaConnection(
                startupConfiguration.hla, true, true);
#ifdef QTTEST_HAS_DIS
            window.setDisBlockedByHla(true);
#endif
            hlaPollTimer.start();
            hlaPublishTimer.start();
            qInfo().noquote()
                << "HLA joined from connection panel:"
                << startupConfiguration.hla.federateName
                << "backend=" << startupConfiguration.hla.backendId
                << "federation=" << startupConfiguration.hla.federationName;
          });

      QObject::connect(
          &window,
          &MainWindow::hlaDisconnectRequested,
          &window,
          [&]() {
            hlaPollTimer.stop();
            hlaPublishTimer.stop();
            lastPublishedEntityCount = -1;
            achievedSynchronizationPoints.clear();
            const tactical::hla::Result result = hlaSession.stop();
            window.setHlaTimeManagementActive(false);
            if (!result.success) {
              window.setHlaConnectionState(
                  false,
                  false,
                  QString::fromStdString(result.message));
              return;
            }
            window.setHlaConnectionState(
                false,
                false,
                QString());
#ifdef QTTEST_HAS_DIS
            window.setDisBlockedByHla(false);
#endif
            window.reportHlaSynchronizationStatus(
                QStringLiteral("disconnected from federation"));
          });
    }

#ifdef QTTEST_HAS_DIS
    QTimer disPollTimer;
    QTimer disPublishTimer;
    disSession.setEntityManagementHandler([&](const auto& request) {
      return window.applyDisEntityManagement(request);
    });
    QObject::connect(&window, &MainWindow::disEntityManagementRequested, &window, [&](const auto& request) {
      const auto result = disSession.publishEntityManagement(request);
      if (!result.success) window.setDisConnectionState(disSession.isActive(), false, result.message);
    });
    QObject::connect(&window, &MainWindow::disIffRequested, &window, [&](const auto& state) {
      const auto result = disSession.publishIff(state);
      if (!result.success) window.setDisConnectionState(disSession.isActive(), false, result.message);
    });
    QObject::connect(&window, &MainWindow::disCollisionRequested, &window, [&](const auto& event) {
      const auto result = disSession.publishCollision(event);
      if (!result.success) window.setDisConnectionState(disSession.isActive(), false, result.message);
    });
    QObject::connect(&window, &MainWindow::hlaSimulationControlRequested,
        &window, [&](tactical::hla::RemoteSimulationControl control, double time) {
          if (!disSession.isActive()) return;
          tactical::dis::SimulationControl mapped;
          switch (control) {
            case tactical::hla::RemoteSimulationControl::StartResume: mapped = tactical::dis::SimulationControl::StartResume; break;
            case tactical::hla::RemoteSimulationControl::Pause: mapped = tactical::dis::SimulationControl::Pause; break;
            case tactical::hla::RemoteSimulationControl::Stop: mapped = tactical::dis::SimulationControl::Stop; break;
          }
          const auto result = disSession.publishSimulationControl(mapped, time);
          if (!result.success) qWarning().noquote() << "DIS control publication failed:" << result.message;
        });
    window.configureDisConnection(
        startupConfiguration.dis,
        true,
        disSession.isActive(),
        hlaSession.isActive());
    if (hlaSession.isActive()) window.setDisBlockedByHla(true);
    if (disSession.isActive()) window.setHlaBlockedByDis(true);

    disPollTimer.setInterval(20);
    QObject::connect(&disPollTimer, &QTimer::timeout, &window, [&]() {
      if (!disSession.isActive()) return;
      const tactical::dis::Result result = disSession.poll();
      if (!result.success) {
        qWarning().noquote() << "DIS receive failed:" << result.message;
        return;
      }
      const auto changes = disSession.takeRemoteEntityChanges();
      for (const auto& ack : disSession.takeRemoteAcknowledgements()) {
        qInfo().noquote() << "DIS control acknowledgement:" << ack.source.key()
                         << "request=" << ack.requestId << "response=" << ack.responseFlag;
      }
      for (const auto control : disSession.takeRemoteSimulationControls()) {
        switch (control) {
          case tactical::dis::SimulationControl::StartResume:
            window.applyHlaRemoteSimulationControl(tactical::hla::RemoteSimulationControl::StartResume); break;
          case tactical::dis::SimulationControl::Pause:
            window.applyHlaRemoteSimulationControl(tactical::hla::RemoteSimulationControl::Pause); break;
          case tactical::dis::SimulationControl::Stop:
            window.applyHlaRemoteSimulationControl(tactical::hla::RemoteSimulationControl::Stop); break;
        }
      }
      window.applyDisRemoteEntityChanges(changes);
      window.applyDisRemoteRadarEmissions(disSession.takeRemoteRadarEmissions());
      window.applyDisInteractions(disSession.takeRemoteIffStates(), disSession.takeRemoteCollisions());
      window.applyDisRemoteWarfareEvents(
          disSession.takeRemoteWarfareEvents());
      window.updateDisStatistics(
          disSession.publishedPduCount(),
          disSession.receivedPduCount(),
          disSession.remoteEntityCount());
    });
    if (disSession.isActive()) disPollTimer.start();

    disPublishTimer.setInterval(100);
    QObject::connect(&disPublishTimer, &QTimer::timeout, &window, [&]() {
      if (!disSession.isActive()) return;
      tactical::dis::Result result =
          disSession.publishEntities(window.entitySnapshot());
      if (result.success) result = disSession.publishSensors(window.entitySnapshot());
      if (result.success) {
        result = disSession.publishMunitions(window.activeMunitionSnapshot());
      }
      if (result.success) {
        result = disSession.publishDetonations(
            window.transientEffectSnapshot());
      }
      if (!result.success) {
        qWarning().noquote() << "DIS publication failed:" << result.message;
      }
      window.updateDisStatistics(
          disSession.publishedPduCount(),
          disSession.receivedPduCount(),
          disSession.remoteEntityCount());
    });
    if (disSession.isActive()) disPublishTimer.start();

    QObject::connect(
        &window,
        &MainWindow::disConnectRequested,
        &window,
        [&](const application::DisStartupConfiguration& configuration) {
          if (hlaSession.isActive()) {
            window.setDisConnectionState(
                false, false,
                QStringLiteral("Disconnect HLA before connecting DIS."));
            return;
          }
          disPollTimer.stop();
          disPublishTimer.stop();
          disSession.stop();
          window.setDisConnectionState(
              false, true,
              QStringLiteral("Opening DIS UDP session at %1:%2...")
                  .arg(configuration.address)
                  .arg(configuration.port));
          const tactical::dis::Result result = disSession.start(configuration);
          if (!result.success) {
            window.setDisConnectionState(false, false, result.message);
            return;
          }
          startupConfiguration.federationMode = application::FederationMode::Dis;
          startupConfiguration.dis = configuration;
          startupConfiguration.save(settings);
          window.configureDisConnection(configuration, true, true, false);
          window.setHlaBlockedByDis(true);
          disPollTimer.start();
          disPublishTimer.start();
        });

    QObject::connect(
        &window,
        &MainWindow::disDisconnectRequested,
        &window,
        [&]() {
          disPollTimer.stop();
          disPublishTimer.stop();
          const tactical::dis::Result result = disSession.stop();
          window.updateDisStatistics(0, 0, 0);
          window.setDisConnectionState(
              false, false, result.success ? QString() : result.message);
          window.setHlaBlockedByDis(false);
        });

    QObject::connect(
        &window,
        &MainWindow::disTestEntityStateRequested,
        &window,
        [&](const tactical::dis::EntityState& state) {
          const tactical::dis::Result result =
              disSession.publishTestEntity(state);
          if (!result.success) {
            window.setDisConnectionState(true, false, result.message);
            return;
          }
          window.updateDisStatistics(
              disSession.publishedPduCount(),
              disSession.receivedPduCount(),
              disSession.remoteEntityCount());
        });
#endif

    const int result = app.exec();
#ifdef QTTEST_HAS_DIS
    disSession.stop();
#endif
    hlaSession.stop();
    return result;
}
