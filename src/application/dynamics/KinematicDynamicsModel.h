#pragma once

#include "application/dynamics/IDynamicsModel.h"

namespace application::dynamics {

class KinematicDynamicsModel final : public IDynamicsModel {
public:
  QString backendId() const override;
  bool configure(const DynamicsModelConfiguration& configuration) override;
  bool initialize(const DynamicsState& initialState) override;
  DynamicsStepResult step(const DynamicsStepContext& context) override;
  DynamicsState state() const override;
  bool reset(const DynamicsState& initialState) override;
  void shutdown() override;
  bool isInitialized() const override;

private:
  static bool stateIsValid(const DynamicsState& state);

  DynamicsModelConfiguration _configuration;
  DynamicsState _state;
  bool _configured = false;
  bool _initialized = false;
};

} // namespace application::dynamics
