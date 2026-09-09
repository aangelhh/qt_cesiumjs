#include "infrastructure/interoperability/dis/DisEmissionCodec.h"
#include <KDIS/PDU/Distributed_Emission_Regeneration/Electromagnetic_Emission_PDU.hpp>
#include <cmath>
#include <stdexcept>

namespace tactical::dis {
namespace {
constexpr double radians = 3.14159265358979323846 / 180.0;
bool finite(const RadarBeam& b) {
  return std::isfinite(b.azimuthCenterDegrees) && std::isfinite(b.azimuthWidthDegrees) &&
      std::isfinite(b.elevationCenterDegrees) && std::isfinite(b.elevationWidthDegrees) &&
      std::isfinite(b.frequencyHertz) && std::isfinite(b.bandwidthHertz) &&
      std::isfinite(b.effectiveRadiatedPowerDbm);
}
}
QByteArray DisEmissionCodec::encode(const RadarEmission& value, QString* error) {
  if (error) error->clear();
  try {
    using namespace KDIS::DATA_TYPE;
    using namespace KDIS::DATA_TYPE::ENUMS;
    if (value.beams.size() > 255) throw std::runtime_error("Too many DIS radar systems.");
    KDIS::PDU::Electromagnetic_Emission_PDU pdu;
    pdu.SetProtocolVersion(IEEE_1278_1_2012);
    pdu.SetExerciseID(value.exerciseId);
    pdu.SetEmittingEntityID({value.host.site, value.host.application, value.host.entity});
    pdu.SetStateUpdateIndicator(value.changedData ? ChangedDataUpdate : StateUpdateHeartbeat);
    for (const auto& b : value.beams) {
      if (!finite(b)) throw std::runtime_error("Invalid DIS radar parameters.");
      EmissionSystem system;
      system.SetEmitterSystemRecord(EmitterSystem(static_cast<EmitterName>(0), EarlyWarning_SurveillanceFunction, b.systemId));
      EmitterBeam beam;
      beam.SetEmitterBeamIDNumber(b.beamId);
      beam.SetEmitterBeamFunction(Search);
      beam.SetBeamStatus(BeamStatus(b.emitting ? BS_Active : BS_Deactivated));
      beam.SetFundamentalParameterData(FundamentalParameterData(
          b.frequencyHertz, b.bandwidthHertz, b.effectiveRadiatedPowerDbm, 0, 0,
          b.azimuthCenterDegrees * radians, b.azimuthWidthDegrees * radians / 2,
          b.elevationCenterDegrees * radians, b.elevationWidthDegrees * radians / 2, 0));
      system.AddEmitterBeam(beam);
      pdu.AddEmissionSystem(system);
    }
    const auto stream = pdu.Encode();
    return {reinterpret_cast<const char*>(stream.GetBufferPtr()), stream.GetBufferSize()};
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return {};
  }
}
bool DisEmissionCodec::decode(const QByteArray& data, RadarEmission& value, QString* error) {
  if (error) error->clear();
  try {
    if (data.size() < 28 || data.size() > 65535 || static_cast<unsigned char>(data[0]) != 7 ||
        static_cast<unsigned char>(data[2]) != 23 || static_cast<unsigned char>(data[3]) != 6 ||
        static_cast<unsigned char>(data[8]) * 256 + static_cast<unsigned char>(data[9]) != data.size())
      throw std::runtime_error("Invalid DIS emission header.");
    KDIS::KDataStream stream(reinterpret_cast<const KDIS::KOCTET*>(data.constData()), data.size());
    KDIS::PDU::Electromagnetic_Emission_PDU pdu(stream);
    RadarEmission result;
    const auto& host = pdu.GetEmittingEntityID();
    result.host = {host.GetSiteID(), host.GetApplicationID(), host.GetEntityID()};
    result.exerciseId = pdu.GetExerciseID();
    result.changedData = pdu.GetStateUpdateIndicator() == KDIS::DATA_TYPE::ENUMS::ChangedDataUpdate;
    for (const auto& system : pdu.GetEmissionSystems()) {
      const auto id = system.GetEmitterSystemRecord().GetEmitterID();
      if (system.GetEmitterBeams().empty()) {
        RadarBeam beam;
        beam.systemId = id;
        beam.beamId = 0;
        result.beams.push_back(beam);
      }
      for (const auto& beam : system.GetEmitterBeams()) {
        const auto& f = beam.GetFundamentalParameterData();
        RadarBeam b;
        b.systemId = id;
        b.beamId = beam.GetEmitterBeamIDNumber();
        b.emitting = beam.GetBeamStatus().GetBeamState() == KDIS::DATA_TYPE::ENUMS::BS_Active;
        b.frequencyHertz = f.GetFrequency();
        b.bandwidthHertz = f.GetFrequencyRange();
        b.effectiveRadiatedPowerDbm = f.GetEffectiveRadiatedPower();
        b.azimuthCenterDegrees = f.GetBeamAzimuthCenter() / radians;
        b.azimuthWidthDegrees = f.GetBeamAzimuthSweep() * 2 / radians;
        b.elevationCenterDegrees = f.GetBeamElevationCenter() / radians;
        b.elevationWidthDegrees = f.GetBeamElevationSweep() * 2 / radians;
        if (!finite(b)) throw std::runtime_error("Invalid DIS radar parameters.");
        result.beams.push_back(b);
      }
    }
    value = std::move(result);
    return true;
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return false;
  }
}
}
