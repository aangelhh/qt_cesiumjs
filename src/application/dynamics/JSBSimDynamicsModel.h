#pragma once

#include "application/JsbsimSetpointController.h"
#include "application/SystemsTelemetry.h"
#include "application/dynamics/IDynamicsModel.h"

#include <memory>
#include <string>

namespace JSBSim {
class FGFDMExec;
class FGPropulsion;
} // namespace JSBSim

namespace application::dynamics {

// JSBSim-backed IDynamicsModel implementation. Owns a single FGFDMExec
// instance for the lifetime of the model (expensive to load, must not be
// rebuilt every tick), unlike KinematicDynamicsModel which is cheap enough
// to reconstruct on every call.
class JSBSimDynamicsModel final : public IDynamicsModel {
public:
  JSBSimDynamicsModel();
  ~JSBSimDynamicsModel() override;

  QString backendId() const override;
  bool configure(const DynamicsModelConfiguration& configuration) override;
  bool initialize(const DynamicsState& initialState) override;
  DynamicsStepResult step(const DynamicsStepContext& context) override;
  DynamicsState state() const override;
  bool reset(const DynamicsState& initialState) override;
  void shutdown() override;
  bool isInitialized() const override;
  bool applyExternalFuelOverride(double fuelRemainingKilograms) override;

  // JSBSim-specific accessors. Deliberately not part of IDynamicsModel: this
  // telemetry (per-engine N1/N2/EGT/fuel-flow/thrust) is far richer than the
  // neutral DynamicsState, and only one backend supports it.
  QVector<application::EngineTelemetry> engineTelemetry(bool entityDestroyed) const;
  QString loadedModelName() const;

private:
  enum class ControlMode {
    NativeAp,
    DirectFcs,
  };

  struct AircraftState {
    double latitudeDeg = 0.0;
    double longitudeDeg = 0.0;
    double altitudeMeters = 0.0;
    double headingDeg = 0.0;
    double pitchRad = 0.0;
    double bankRad = 0.0;
    double trueAirspeedKnots = 0.0;
    double verticalSpeedMetersPerSecond = 0.0;
  };

  bool buildAndLoadModel();
  bool applyInitialConditions(const DynamicsState& initialState);
  bool readAircraftState(AircraftState& out) const;
  void applyControlCommands(const application::JsbsimControlOutput& commands);
  void initializeFuelTanks(double requestedFuelRemainingKilograms);
  double sumTankFuelKilograms() const;
  bool hasProperty(const std::string& propertyName) const;
  ControlMode detectControlMode() const;
  static QString findJsbsimRoot();

  DynamicsModelConfiguration _configuration;
  std::unique_ptr<JSBSim::FGFDMExec> _exec;
  DynamicsState _state;
  ControlMode _controlMode = ControlMode::DirectFcs;
  application::JsbsimControlOutput _previousControl;
  bool _configured = false;
  bool _initialized = false;
};

} // namespace application::dynamics
