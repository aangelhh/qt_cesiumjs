#pragma once

#include <QString>

namespace application::dynamics {

// Canonical SI/WGS84 state exchanged between the runtime and dynamics models.
struct DynamicsState {
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;
  double speedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
};

struct DynamicsModelConfiguration {
  QString modelId;
  QString platformId;
  bool groundConstrained = false;
};

struct DynamicsStepContext {
  // The runtime owns both values. A model must not advance the global clock.
  double simulationTimeSeconds = 0.0;
  double deltaTimeSeconds = 0.0;
};

struct DynamicsStepResult {
  bool advanced = false;
  QString errorMessage;

  explicit operator bool() const {
    return advanced;
  }
};

class IDynamicsModel {
public:
  virtual ~IDynamicsModel() = default;

  virtual QString backendId() const = 0;
  virtual bool configure(const DynamicsModelConfiguration& configuration) = 0;
  virtual bool initialize(const DynamicsState& initialState) = 0;
  virtual DynamicsStepResult step(const DynamicsStepContext& context) = 0;
  virtual DynamicsState state() const = 0;
  virtual bool reset(const DynamicsState& initialState) = 0;
  virtual void shutdown() = 0;
  virtual bool isInitialized() const = 0;
};

} // namespace application::dynamics
