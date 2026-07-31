#include "application/KinematicsTelemetry.h"
#include "presentation/KinematicsCockpitWidget.h"

#include <QApplication>
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QSet>
#include <QTabBar>
#include <QTabWidget>

#if defined(QTTEST_HAS_QFLIGHTINSTRUMENTS)
#include <qfi/qfi_EADI.h>
#endif

#if defined(QTTEST_HAS_MODERN_PFD)
#include <QQuickItem>
#include <QQuickWidget>
#endif

namespace {

bool rendersContent(QWidget& widget) {
  QImage image(widget.size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::magenta);
  QPainter painter(&image);
  widget.render(&painter);
  painter.end();

  QSet<QRgb> colors;
  int unchangedPixels = 0;
  for (int y = 0; y < image.height(); y += 4) {
    for (int x = 0; x < image.width(); x += 4) {
      const QRgb pixel = image.pixel(x, y);
      colors.insert(pixel);
      if (pixel == QColor(Qt::magenta).rgba()) {
        ++unchangedPixels;
      }
    }
  }

  const int sampledPixels =
      ((image.width() + 3) / 4) * ((image.height() + 3) / 4);
  return colors.size() >= 20 && unchangedPixels <= sampledPixels / 20;
}

} // namespace

int main(int argc, char** argv) {
  QApplication application(argc, argv);

  application::KinematicsTelemetrySnapshot snapshot;
  snapshot.entityName = QStringLiteral("Cockpit Smoke Test");
  snapshot.domain = QStringLiteral("Air");
  snapshot.category = QStringLiteral("Fighter");
  snapshot.altitudeMeters = 3000.0;
  snapshot.headingDegrees = 275.0;
  snapshot.pitchDegrees = 8.0;
  snapshot.rollDegrees = -25.0;
  snapshot.speedKnots = 320.0;
  snapshot.verticalSpeedMetersPerSecond = 12.0;
  snapshot.targetHeadingDegrees = 290.0;
  snapshot.targetAltitudeMeters = 5000.0;
  snapshot.targetSpeedKnots = 400.0;
  snapshot.maxSpeedKnots = 900.0;
  snapshot.taskEnabled = true;
  snapshot.taskType = QStringLiteral("InterceptEntity");
  snapshot.taskStatus = QStringLiteral("Running");
  snapshot.dynamicsModel = QStringLiteral("kinematic");
  snapshot.systems.profileId = QStringLiteral("air-turbine-2-engine");
  snapshot.systems.dataSource = QStringLiteral("Kinematic estimate");
  for (int index = 0; index < 2; ++index) {
    application::EngineTelemetry engine;
    engine.engineId = QStringLiteral("ENG %1").arg(index + 1);
    engine.state = QStringLiteral("RUNNING");
    engine.n1Percent = 65.0 + index * 5.0;
    engine.n2Percent = 80.0 + index * 4.0;
    engine.exhaustTemperatureCelsius = 700.0 + index * 10.0;
    engine.fuelFlowKilogramsPerHour = 1200.0;
    engine.thrustKilonewtons = 45.0;
    engine.n1Available = true;
    engine.n2Available = true;
    engine.exhaustTemperatureAvailable = true;
    engine.fuelFlowAvailable = true;
    engine.thrustAvailable = true;
    engine.available = true;
    engine.estimated = true;
    snapshot.systems.engines.push_back(engine);
  }

  presentation::KinematicsCockpitWidget widget;
  widget.resize(720, 540);
  widget.applySnapshot(snapshot);
  widget.show();
  application.processEvents();

  auto* tabs = widget.findChild<QTabWidget*>(
      QStringLiteral("kinematicsCockpitTabs"));
  if (!tabs || tabs->count() < 3) {
    return 1;
  }

#if defined(QTTEST_HAS_QFLIGHTINSTRUMENTS)
  auto* eadi = widget.findChild<qfi_EADI*>(
      QStringLiteral("qflightKinematicsEadi"));
  if (!eadi || eadi->focusPolicy() != Qt::NoFocus || eadi->isInteractive() ||
      eadi->horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff ||
      eadi->verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff) {
    return 2;
  }
#endif

#if defined(QTTEST_HAS_MODERN_PFD)
  auto* modernPfd = widget.findChild<QQuickWidget*>(
      QStringLiteral("modernKinematicsPfd"));
  if (!modernPfd || !modernPfd->rootObject() ||
      modernPfd->rootObject()->property("rollDegrees").toDouble() != -25.0) {
    return 3;
  }

  auto* ecam = widget.findChild<QQuickWidget*>(
      QStringLiteral("dynamicEcamEngine"));
  if (!ecam || !ecam->rootObject() ||
      ecam->rootObject()->property("profileId").toString() !=
          QStringLiteral("air-turbine-2-engine") ||
      ecam->rootObject()->property("engineModel").toList().size() != 2) {
    return 4;
  }

  snapshot.rollDegrees = 18.0;
  snapshot.pitchDegrees = -6.0;
  widget.applySnapshot(snapshot);
  application.processEvents();
  if (modernPfd->rootObject()->property("rollDegrees").toDouble() != 18.0 ||
      modernPfd->rootObject()->property("pitchDegrees").toDouble() != -6.0) {
    return 5;
  }
#endif

  for (int index = 0; index < tabs->count(); ++index) {
    tabs->setCurrentIndex(index);
    application.processEvents();
    if (!rendersContent(widget)) {
      return index + 6;
    }
  }

  presentation::KinematicsCockpitWidget ecamPanel(
      nullptr,
      presentation::KinematicsCockpitWidget::PanelMode::EcamEngine);
  ecamPanel.resize(720, 540);
  ecamPanel.applySnapshot(snapshot);
  ecamPanel.show();
  application.processEvents();
  auto* ecamTabs = ecamPanel.findChild<QTabWidget*>(
      QStringLiteral("kinematicsCockpitTabs"));
  if (!ecamTabs || ecamTabs->count() != 1 || ecamTabs->tabBar()->isVisible() ||
      !rendersContent(ecamPanel)) {
    return 10;
  }

  return 0;
}
