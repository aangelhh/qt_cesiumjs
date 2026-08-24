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
  // Negative means "this backend does not track fuel" (e.g. kinematic).
  double fuelRemainingKilograms = -1.0;
  double fuelCapacityKilograms = -1.0;
};

struct DynamicsModelConfiguration {
  QString modelId;
  QString platformId;
  bool groundConstrained = false;
  // Requested platform fuel capacity in kilograms. Ignored by backends that
  // don't track fuel.
  double fuelCapacityKilograms = 0.0;
};

// Per-tick control target. Backends whose control law needs an explicit
// setpoint every step (e.g. JSBSim) read this; backends that dead-reckon off
// state already resolved upstream (kinematic) may ignore it entirely.
// controlProfileId is re-read every step rather than cached at configure()
// time, so a live profile change takes effect without rebuilding a session.
struct DynamicsControlSetpoint {
  bool valid = false;
  bool preferDirectControl = false;
  double targetHeadingDegrees = 0.0;
  double targetAltitudeMeters = 0.0;
  double targetSpeedKnots = 0.0;
  QString controlProfileId;
};

struct DynamicsStepContext {
  // The runtime owns both values. A model must not advance the global clock.
  double simulationTimeSeconds = 0.0;
  double deltaTimeSeconds = 0.0;
  DynamicsControlSetpoint controlSetpoint;
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

  // Out-of-band fuel push (e.g. a scripted refuel) outside the normal step
  // cycle. Default: unsupported. Backends that don't track fuel need not
  // override this.
  virtual bool applyExternalFuelOverride(double /*fuelRemainingKilograms*/) {
    return false;
  }
};

} // namespace application::dynamics
