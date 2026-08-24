#include "application/dynamics/KinematicDynamicsModel.h"

#include <QtMath>

#include <cmath>

namespace application::dynamics {
namespace {

constexpr double kEarthRadiusMeters = 6371000.0;
constexpr double kKnotsToMetersPerSecond = 0.514444;

} // namespace

QString KinematicDynamicsModel::backendId() const {
  return QStringLiteral("kinematic");
}

bool KinematicDynamicsModel::configure(
    const DynamicsModelConfiguration& configuration) {
  _configuration = configuration;
  _configured = true;
  _initialized = false;
  return true;
}

bool KinematicDynamicsModel::initialize(const DynamicsState& initialState) {
  if (!_configured || !stateIsValid(initialState)) {
    return false;
  }
  _state = initialState;
  if (_configuration.groundConstrained) {
    _state.verticalSpeedMetersPerSecond = 0.0;
    _state.pitchDegrees = 0.0;
    _state.rollDegrees = 0.0;
  }
  _initialized = true;
  return true;
}

DynamicsStepResult KinematicDynamicsModel::step(
    const DynamicsStepContext& context) {
  if (!_initialized) {
    return {false, QStringLiteral("Dynamics model is not initialized")};
  }
  if (!std::isfinite(context.simulationTimeSeconds) ||
      !std::isfinite(context.deltaTimeSeconds) ||
      context.deltaTimeSeconds <= 0.0) {
    return {false, QStringLiteral("Invalid simulation time step")};
  }

  const double speedMetersPerSecond =
      qMax(0.0, _state.speedKnots) * kKnotsToMetersPerSecond;
  const double angularDistance =
      (speedMetersPerSecond * context.deltaTimeSeconds) /
      kEarthRadiusMeters;
  const double headingRadians = qDegreesToRadians(_state.headingDegrees);
  const double latitudeRadians = qDegreesToRadians(_state.latitudeDegrees);
  const double longitudeRadians = qDegreesToRadians(_state.longitudeDegrees);

  const double sinLatitude = qSin(latitudeRadians);
  const double cosLatitude = qCos(latitudeRadians);
  const double sinAngularDistance = qSin(angularDistance);
  const double cosAngularDistance = qCos(angularDistance);
  const double nextLatitudeRadians = qAsin(
      sinLatitude * cosAngularDistance +
      cosLatitude * sinAngularDistance * qCos(headingRadians));
  const double nextLongitudeRadians = longitudeRadians + qAtan2(
      qSin(headingRadians) * sinAngularDistance * cosLatitude,
      cosAngularDistance - sinLatitude * qSin(nextLatitudeRadians));

  _state.latitudeDegrees = qRadiansToDegrees(nextLatitudeRadians);
  _state.longitudeDegrees = qRadiansToDegrees(nextLongitudeRadians);
  if (_configuration.groundConstrained) {
    _state.verticalSpeedMetersPerSecond = 0.0;
    _state.pitchDegrees = 0.0;
    _state.rollDegrees = 0.0;
  } else {
    _state.altitudeMeters = qMax(
        0.0,
        _state.altitudeMeters +
            _state.verticalSpeedMetersPerSecond * context.deltaTimeSeconds);
  }
  return {true, {}};
}

DynamicsState KinematicDynamicsModel::state() const {
  return _state;
}

bool KinematicDynamicsModel::reset(const DynamicsState& initialState) {
  if (!_configured) {
    return false;
  }
  _initialized = false;
  return initialize(initialState);
}

void KinematicDynamicsModel::shutdown() {
  _state = DynamicsState{};
  _initialized = false;
}

bool KinematicDynamicsModel::isInitialized() const {
  return _initialized;
}

bool KinematicDynamicsModel::stateIsValid(const DynamicsState& state) {
  return std::isfinite(state.latitudeDegrees) &&
      std::isfinite(state.longitudeDegrees) &&
      std::isfinite(state.altitudeMeters) &&
      std::isfinite(state.headingDegrees) &&
      std::isfinite(state.pitchDegrees) &&
      std::isfinite(state.rollDegrees) &&
      std::isfinite(state.speedKnots) &&
      std::isfinite(state.verticalSpeedMetersPerSecond) &&
      state.latitudeDegrees >= -90.0 &&
      state.latitudeDegrees <= 90.0;
}

} // namespace application::dynamics
