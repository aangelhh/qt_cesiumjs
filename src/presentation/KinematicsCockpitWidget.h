#pragma once

#include "application/KinematicsTelemetry.h"

#include <QWidget>

#include <memory>

namespace presentation {

class KinematicsCockpitWidget : public QWidget {
  Q_OBJECT

public:
  enum class PanelMode {
    All,
    ModernPfd,
    QFlightEadi,
    EcamEngine,
  };

  explicit KinematicsCockpitWidget(
      QWidget* parent = nullptr,
      PanelMode panelMode = PanelMode::All);
  ~KinematicsCockpitWidget() override;

  void applySnapshot(const application::KinematicsTelemetrySnapshot& snapshot);
  void setControlActive(bool active);
  void clear();

signals:
  void takeControlRequested(
      const QString& entityName,
      double headingDegrees,
      int altitudeMeters,
      double speedKnots);
  void setpointsRequested(
      const QString& entityName,
      double headingDegrees,
      int altitudeMeters,
      double speedKnots);
  void releaseControlRequested(const QString& entityName);

private slots:
  void handleTakeControlRequested(
      double headingDegrees,
      double altitudeFeet,
      double speedKnots);
  void handleSetpointsRequested(
      double headingDegrees,
      double altitudeFeet,
      double speedKnots);
  void handleReleaseControlRequested();

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

} // namespace presentation
