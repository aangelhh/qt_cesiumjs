#include "src/MainWindow.h"
#include "application/HlaStartupSession.h"
#include "application/StartupConfiguration.h"
#include "presentation/StartupConfigurationDialog.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

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

    QSettings settings;
    application::StartupConfiguration startupConfiguration =
        application::StartupConfiguration::load(settings);
    startupConfiguration.addMissingHlaFomModules(
        QDir(QStringLiteral(QTTEST_SOURCE_DIR))
            .filePath(QStringLiteral("src/infrastructure/hla/FOM")));
    application::HlaStartupSession hlaSession;
    const QVector<presentation::HlaBackendOption> hlaBackends =
        availableHlaBackends();
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

    MainWindow window;
    window.setWindowTitle(
        QStringLiteral("%1 [%2]")
            .arg(window.windowTitle(), startupConfiguration.modeDisplayName()));
    window.show();

    QTimer hlaPollTimer;
    QTimer hlaPublishTimer;
    qsizetype lastPublishedEntityCount = -1;
    if (hlaSession.isActive()) {
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
        }
      });
      hlaPollTimer.start();

      hlaPublishTimer.setInterval(100);
      QObject::connect(&hlaPublishTimer, &QTimer::timeout, &window, [&]() {
        const QVector<Entity> entities = window.entitySnapshot();
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
        if (lastPublishedEntityCount != entities.size()) {
          lastPublishedEntityCount = entities.size();
          qInfo().noquote()
              << "HLA entity synchronization active:"
              << lastPublishedEntityCount
              << "local object(s) registered and updating at 10 Hz via"
              << hlaSession.backendId();
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
        }
      });
      hlaPublishTimer.start();
    }

    const int result = app.exec();
    hlaSession.stop();
    return result;
}
