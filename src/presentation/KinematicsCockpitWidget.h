#pragma once

#include "application/KinematicsTelemetry.h"

#include <QWidget>

#include <memory>

namespace presentation {

class KinematicsCockpitWidget : public QWidget {
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
  void clear();

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};

} // namespace presentation
