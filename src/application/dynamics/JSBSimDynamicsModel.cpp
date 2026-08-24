#include "application/dynamics/JSBSimDynamicsModel.h"

#include "domain/GeoMath.h"

#include <FGFDMExec.h>
#include <models/FGPropulsion.h>
#include <models/propulsion/FGTank.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QtMath>

#include <cmath>

namespace application::dynamics {
namespace {

constexpr double kMetersToFeet = 3.28084;
constexpr double kFeetToMeters = 1.0 / kMetersToFeet;
constexpr double kPoundsToKilograms = 0.45359237;
constexpr double kKilogramsToPounds = 1.0 / kPoundsToKilograms;
constexpr double kMaximumJsbsimStepSeconds = 1.0 / 120.0;
constexpr double kPoundsPerSecondToKilogramsPerHour = 1632.932532;
constexpr double kPoundsForceToKilonewtons = 0.0044482216153;

} // namespace

JSBSimDynamicsModel::JSBSimDynamicsModel() = default;
JSBSimDynamicsModel::~JSBSimDynamicsModel() = default;

QString JSBSimDynamicsModel::backendId() const {
  return QStringLiteral("jsbsim");
}

bool JSBSimDynamicsModel::configure(
    const DynamicsModelConfiguration& configuration) {
  _configuration = configuration;
  _configured = true;
  _initialized = false;
  return true;
}

bool JSBSimDynamicsModel::initialize(const DynamicsState& initialState) {
  if (!_configured) {
    return false;
  }
  if (!buildAndLoadModel()) {
    return false;
  }
  _state = initialState;
  if (!applyInitialConditions(initialState)) {
    _exec.reset();
    return false;
  }
  _initialized = true;
  return true;
}

DynamicsStepResult JSBSimDynamicsModel::step(const DynamicsStepContext& context) {
  if (!_initialized || !_exec) {
    return {false, QStringLiteral("JSBSim model is not initialized")};
  }
  if (!std::isfinite(context.deltaTimeSeconds) || context.deltaTimeSeconds <= 0.0) {
    return {false, QStringLiteral("Invalid simulation time step")};
  }

  const int substepCount = qBound(
      1,
      static_cast<int>(std::ceil(context.deltaTimeSeconds / kMaximumJsbsimStepSeconds)),
      64);
  const double substepSeconds = context.deltaTimeSeconds / substepCount;
  _exec->Setdt(substepSeconds);

  const ControlMode activeControlMode =
      context.controlSetpoint.preferDirectControl ? ControlMode::DirectFcs : _controlMode;

  if (activeControlMode == ControlMode::NativeAp) {
    if (context.controlSetpoint.valid) {
      _exec->SetPropertyValue("ap/heading_setpoint", context.controlSetpoint.targetHeadingDegrees);
      _exec->SetPropertyValue("ap/heading_hold", 1.0);
      _exec->SetPropertyValue(
          "ap/altitude_setpoint",
          context.controlSetpoint.targetAltitudeMeters * kMetersToFeet);
      _exec->SetPropertyValue("ap/altitude_hold", 1.0);
    } else {
      _exec->SetPropertyValue("ap/heading_hold", 0.0);
      _exec->SetPropertyValue("ap/altitude_hold", 0.0);
    }
  }

  for (int substep = 0; substep < substepCount; ++substep) {
    if (activeControlMode == ControlMode::DirectFcs) {
      AircraftState aircraftState;
      if (!readAircraftState(aircraftState)) {
        return {false, QStringLiteral("Failed to read JSBSim aircraft state")};
      }
      const application::JsbsimControlState controlState{
          aircraftState.headingDeg,
          qRadiansToDegrees(aircraftState.pitchRad),
          qRadiansToDegrees(aircraftState.bankRad),
          aircraftState.altitudeMeters,
          aircraftState.trueAirspeedKnots,
          aircraftState.verticalSpeedMetersPerSecond,
      };
      const application::JsbsimControlSetpoint setpoint{
          context.controlSetpoint.targetHeadingDegrees,
          context.controlSetpoint.targetAltitudeMeters,
          context.controlSetpoint.targetSpeedKnots,
      };
      const application::JsbsimControlOutput requested =
          application::computeJsbsimSetpointControl(
              controlState,
              setpoint,
              context.controlSetpoint.controlProfileId);
      _previousControl = application::rateLimitJsbsimControl(
          _previousControl,
          requested,
          substepSeconds,
          context.controlSetpoint.controlProfileId);
      applyControlCommands(_previousControl);
    }
    if (!_exec->Run()) {
      return {false, QStringLiteral("JSBSim FGFDMExec::Run failed")};
    }
  }

  AircraftState finalState;
  if (!readAircraftState(finalState)) {
    return {false, QStringLiteral("Failed to read JSBSim aircraft state")};
  }

  _state.latitudeDegrees = finalState.latitudeDeg;
  _state.longitudeDegrees = finalState.longitudeDeg;
  _state.altitudeMeters = finalState.altitudeMeters;
  _state.headingDegrees = finalState.headingDeg;
  _state.pitchDegrees = qRadiansToDegrees(finalState.pitchRad);
  _state.rollDegrees = qRadiansToDegrees(finalState.bankRad);
  _state.speedKnots = qMax(0.0, finalState.trueAirspeedKnots);
  _state.verticalSpeedMetersPerSecond = finalState.verticalSpeedMetersPerSecond;
  if (_state.fuelCapacityKilograms >= 0.0) {
    _state.fuelRemainingKilograms =
        qBound(0.0, sumTankFuelKilograms(), _state.fuelCapacityKilograms);
  }
  return {true, {}};
}

DynamicsState JSBSimDynamicsModel::state() const {
  return _state;
}

bool JSBSimDynamicsModel::reset(const DynamicsState& initialState) {
  if (!_configured) {
    return false;
  }
  if (!_exec) {
    return initialize(initialState);
  }
  _state = initialState;
  if (!applyInitialConditions(initialState)) {
    _exec.reset();
    _initialized = false;
    return false;
  }
  _initialized = true;
  return true;
}

void JSBSimDynamicsModel::shutdown() {
  _exec.reset();
  _state = DynamicsState{};
  _initialized = false;
}

bool JSBSimDynamicsModel::isInitialized() const {
  return _initialized;
}

bool JSBSimDynamicsModel::applyExternalFuelOverride(double fuelRemainingKilograms) {
  if (!_initialized || !_exec) {
    return false;
  }
  initializeFuelTanks(fuelRemainingKilograms);
  return true;
}

QVector<application::EngineTelemetry> JSBSimDynamicsModel::engineTelemetry(
    bool entityDestroyed) const {
  QVector<application::EngineTelemetry> engines;
  if (!_exec) {
    return engines;
  }
  const int engineCount = static_cast<int>(_exec->GetPropulsion()->GetNumEngines());
  if (engineCount <= 0) {
    return engines;
  }

  engines.reserve(engineCount);
  for (int index = 0; index < engineCount; ++index) {
    const std::string base = "propulsion/engine[" + std::to_string(index) + "]";
    auto readProperty = [this](const std::string& name) {
      const double value = _exec->GetPropertyValue(name);
      return std::isfinite(value) ? value : 0.0;
    };

    application::EngineTelemetry engine;
    engine.engineId = QStringLiteral("ENG %1").arg(index + 1);
    const bool running = readProperty(base + "/set-running") > 0.5;
    engine.state = entityDestroyed
        ? QStringLiteral("FAILED")
        : (running ? QStringLiteral("RUNNING") : QStringLiteral("OFF"));
    engine.n1Available = hasProperty(base + "/n1");
    engine.n2Available = hasProperty(base + "/n2");
    const bool rpmAvailable = hasProperty(base + "/engine-rpm");
    engine.n1Percent = engine.n1Available
        ? readProperty(base + "/n1")
        : (rpmAvailable ? readProperty(base + "/engine-rpm") : 0.0);
    engine.n1Available = engine.n1Available || rpmAvailable;
    engine.n2Percent = engine.n2Available
        ? readProperty(base + "/n2")
        : 0.0;

    const bool egtCelsiusAvailable = hasProperty(base + "/egt-degC");
    const bool egtFahrenheitAvailable = hasProperty(base + "/egt-degF");
    engine.exhaustTemperatureAvailable = egtCelsiusAvailable || egtFahrenheitAvailable;
    engine.exhaustTemperatureCelsius = egtCelsiusAvailable
        ? readProperty(base + "/egt-degC")
        : (egtFahrenheitAvailable
               ? (readProperty(base + "/egt-degF") - 32.0) * 5.0 / 9.0
               : 0.0);

    engine.fuelFlowAvailable = hasProperty(base + "/fuel-flow-rate-pps");
    engine.fuelFlowKilogramsPerHour = engine.fuelFlowAvailable
        ? readProperty(base + "/fuel-flow-rate-pps") * kPoundsPerSecondToKilogramsPerHour
        : 0.0;
    engine.thrustAvailable = hasProperty(base + "/thrust-lbs");
    engine.thrustKilonewtons = engine.thrustAvailable
        ? readProperty(base + "/thrust-lbs") * kPoundsForceToKilonewtons
        : 0.0;
    engine.available = engine.n1Available || engine.n2Available ||
        engine.exhaustTemperatureAvailable || engine.fuelFlowAvailable ||
        engine.thrustAvailable;
    engine.estimated = false;
    engines.push_back(engine);
  }
  return engines;
}

QString JSBSimDynamicsModel::loadedModelName() const {
  return _configuration.modelId;
}

bool JSBSimDynamicsModel::buildAndLoadModel() {
  const QString rootPath = findJsbsimRoot();
  const QString aircraftPath = QDir(rootPath).absoluteFilePath(QStringLiteral("aircraft"));
  const QString enginePath = QDir(rootPath).absoluteFilePath(QStringLiteral("engine"));
  const QString systemsPath = QDir(rootPath).absoluteFilePath(QStringLiteral("systems"));
  qDebug() << "JSBSim root:" << rootPath << "model:" << _configuration.modelId;

  auto exec = std::make_unique<JSBSim::FGFDMExec>();
  exec->SetRootDir(SGPath(rootPath.toStdString()));
  if (!exec->LoadModel(
          SGPath(aircraftPath.toStdString()),
          SGPath(enginePath.toStdString()),
          SGPath(systemsPath.toStdString()),
          _configuration.modelId.toStdString(),
          true)) {
    return false;
  }
  _exec = std::move(exec);
  return true;
}

bool JSBSimDynamicsModel::applyInitialConditions(const DynamicsState& initialState) {
  _exec->SetPropertyValue("ic/lat-geod-deg", initialState.latitudeDegrees);
  _exec->SetPropertyValue("ic/long-gc-deg", initialState.longitudeDegrees);
  _exec->SetPropertyValue("ic/h-sl-ft", initialState.altitudeMeters * kMetersToFeet);
  _exec->SetPropertyValue("ic/psi-true-deg", initialState.headingDegrees);
  _exec->SetPropertyValue("ic/vc-kts", qMax(0.0, initialState.speedKnots));
  _exec->SetPropertyValue("ic/gamma-deg", 0.0);
  if (!_exec->RunIC()) {
    return false;
  }
  initializeFuelTanks(initialState.fuelRemainingKilograms);
  const int engineCount = static_cast<int>(_exec->GetPropulsion()->GetNumEngines());
  for (int index = 0; index < engineCount; ++index) {
    _exec->SetPropertyValue(
        "propulsion/engine[" + std::to_string(index) + "]/set-running",
        1.0);
  }
  _controlMode = detectControlMode();
  _previousControl = application::JsbsimControlOutput{};
  qDebug() << "JSBSim control mode:"
           << (_controlMode == ControlMode::NativeAp ? "NativeAp" : "DirectFcs");
  return true;
}

bool JSBSimDynamicsModel::readAircraftState(AircraftState& out) const {
  const double latitudeDeg = _exec->GetPropertyValue("position/lat-geod-deg");
  const double longitudeDeg = _exec->GetPropertyValue("position/long-gc-deg");
  const double altitudeFeet = _exec->GetPropertyValue("position/h-sl-ft");
  const double headingDeg = _exec->GetPropertyValue("attitude/psi-deg");
  const double pitchRad = _exec->GetPropertyValue("attitude/theta-rad");
  const double bankRad = _exec->GetPropertyValue("attitude/phi-rad");
  const double trueAirspeedKnots = _exec->GetPropertyValue("velocities/vtrue-kts");
  const double verticalSpeedFeetPerSecond = _exec->GetPropertyValue("velocities/h-dot-fps");

  if (!std::isfinite(latitudeDeg) ||
      !std::isfinite(longitudeDeg) ||
      !std::isfinite(altitudeFeet) ||
      !std::isfinite(headingDeg) ||
      !std::isfinite(pitchRad) ||
      !std::isfinite(bankRad) ||
      !std::isfinite(trueAirspeedKnots) ||
      !std::isfinite(verticalSpeedFeetPerSecond)) {
    return false;
  }

  out.latitudeDeg = latitudeDeg;
  out.longitudeDeg = longitudeDeg;
  out.altitudeMeters = altitudeFeet * kFeetToMeters;
  out.headingDeg = domain::normalizeDegrees360(headingDeg);
  out.pitchRad = pitchRad;
  out.bankRad = bankRad;
  out.trueAirspeedKnots = trueAirspeedKnots;
  out.verticalSpeedMetersPerSecond = verticalSpeedFeetPerSecond * kFeetToMeters;
  return true;
}

void JSBSimDynamicsModel::applyControlCommands(
    const application::JsbsimControlOutput& commands) {
  _exec->SetPropertyValue("fcs/aileron-cmd-norm", commands.aileronNormalized);
  _exec->SetPropertyValue("fcs/rudder-cmd-norm", commands.rudderNormalized);
  _exec->SetPropertyValue("fcs/elevator-cmd-norm", commands.elevatorNormalized);
  const int engineCount = static_cast<int>(_exec->GetPropulsion()->GetNumEngines());
  for (int index = 0; index < engineCount; ++index) {
    _exec->SetPropertyValue(
        "fcs/throttle-cmd-norm[" + std::to_string(index) + "]",
        commands.throttleNormalized);
  }
}

void JSBSimDynamicsModel::initializeFuelTanks(double requestedFuelRemainingKilograms) {
  JSBSim::FGPropulsion& propulsion = *_exec->GetPropulsion();
  double physicalCapacityPounds = 0.0;
  for (unsigned int index = 0; index < propulsion.GetNumTanks(); ++index) {
    const auto tank = propulsion.GetTank(index);
    if (tank && tank->GetType() == JSBSim::FGTank::ttFUEL) {
      physicalCapacityPounds += qMax(0.0, tank->GetCapacity());
    }
  }
  if (physicalCapacityPounds <= 0.0) {
    return;
  }

  const double physicalCapacityKilograms = physicalCapacityPounds * kPoundsToKilograms;
  const double requestedCapacityKilograms =
      _configuration.fuelCapacityKilograms > 0.0
          ? _configuration.fuelCapacityKilograms
          : physicalCapacityKilograms;
  _state.fuelCapacityKilograms = qMin(requestedCapacityKilograms, physicalCapacityKilograms);
  _state.fuelRemainingKilograms = qBound(
      0.0,
      requestedFuelRemainingKilograms >= 0.0 ? requestedFuelRemainingKilograms : 0.0,
      _state.fuelCapacityKilograms);

  const double requestedFuelPounds = _state.fuelRemainingKilograms * kKilogramsToPounds;
  for (unsigned int index = 0; index < propulsion.GetNumTanks(); ++index) {
    const auto tank = propulsion.GetTank(index);
    if (!tank || tank->GetType() != JSBSim::FGTank::ttFUEL) {
      continue;
    }
    const double tankShare = tank->GetCapacity() / physicalCapacityPounds;
    tank->SetContents(requestedFuelPounds * tankShare);
  }
}

double JSBSimDynamicsModel::sumTankFuelKilograms() const {
  const JSBSim::FGPropulsion& propulsion = *_exec->GetPropulsion();
  double remainingPounds = 0.0;
  for (unsigned int index = 0; index < propulsion.GetNumTanks(); ++index) {
    const auto tank = propulsion.GetTank(index);
    if (tank && tank->GetType() == JSBSim::FGTank::ttFUEL) {
      remainingPounds += qMax(0.0, tank->GetContents());
    }
  }
  return remainingPounds * kPoundsToKilograms;
}

bool JSBSimDynamicsModel::hasProperty(const std::string& propertyName) const {
  std::string catalogName = propertyName;
  const std::string zeroIndex = "[0]";
  if (const std::size_t index = catalogName.find(zeroIndex); index != std::string::npos) {
    catalogName.erase(index, zeroIndex.size());
  }
  for (const std::string& catalogEntry : _exec->GetPropertyCatalog()) {
    if (catalogEntry.rfind(propertyName, 0) == 0 ||
        catalogEntry.rfind(catalogName, 0) == 0) {
      return true;
    }
  }
  return false;
}

JSBSimDynamicsModel::ControlMode JSBSimDynamicsModel::detectControlMode() const {
  const bool hasHeadingHold = hasProperty("ap/heading_hold");
  const bool hasHeadingSetpoint = hasProperty("ap/heading_setpoint");
  const bool hasAltitudeHold = hasProperty("ap/altitude_hold");
  const bool hasAltitudeSetpoint = hasProperty("ap/altitude_setpoint");
  if (hasHeadingHold && hasHeadingSetpoint && hasAltitudeHold && hasAltitudeSetpoint) {
    return ControlMode::NativeAp;
  }
  return ControlMode::DirectFcs;
}

QString JSBSimDynamicsModel::findJsbsimRoot() {
#ifdef QTTEST_SOURCE_DIR
  const QString sourceRoot =
      QDir(QString::fromUtf8(QTTEST_SOURCE_DIR)).absoluteFilePath(QStringLiteral("Dependencies/jsbsim"));
  QDir sourceDir(sourceRoot);
  if (sourceDir.exists(QStringLiteral("aircraft")) &&
      sourceDir.exists(QStringLiteral("engine")) &&
      sourceDir.exists(QStringLiteral("systems"))) {
    return sourceDir.absolutePath();
  }
#else
  Q_UNUSED(0);
#endif

  QDir appDir(QCoreApplication::applicationDirPath());
  QDir cursor = appDir;
  for (int depth = 0; depth < 8; ++depth) {
    const QString candidate = cursor.absoluteFilePath(QStringLiteral("Dependencies/jsbsim"));
    QDir candidateDir(candidate);
    if (candidateDir.exists(QStringLiteral("aircraft")) &&
        candidateDir.exists(QStringLiteral("engine")) &&
        candidateDir.exists(QStringLiteral("systems"))) {
      return candidateDir.absolutePath();
    }
    if (!cursor.cdUp()) {
      break;
    }
  }
  return QString();
}

} // namespace application::dynamics
