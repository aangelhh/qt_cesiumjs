#include "infrastructure/interoperability/dis/DisSimulationControlCodec.h"

#include <KDIS/PDU/Simulation_Management/Start_Resume_PDU.hpp>
#include <KDIS/PDU/Simulation_Management/Stop_Freeze_PDU.hpp>
#include <KDIS/PDU/Simulation_Management/Acknowledge_PDU.hpp>
#include <cmath>
#include <stdexcept>

namespace tactical::dis {
namespace {
template<class Pdu, class Message>
QByteArray encodePdu(Pdu& pdu, const Message& message) {
  pdu.SetProtocolVersion(KDIS::DATA_TYPE::ENUMS::IEEE_1278_1_2012);
  pdu.SetExerciseID(message.exerciseId);
  pdu.SetOriginatingEntityID({message.source.site, message.source.application, message.source.entity});
  pdu.SetReceivingEntityID({message.destination.site, message.destination.application, message.destination.entity});
  pdu.SetRequestID(message.requestId);
  const auto stream = pdu.Encode();
  return {reinterpret_cast<const char*>(stream.GetBufferPtr()), stream.GetBufferSize()};
}
template<class Pdu, class Message>
void decodePdu(const Pdu& pdu, Message& message) {
  const auto& source = pdu.GetOriginatingEntityID();
  const auto& destination = pdu.GetReceivingEntityID();
  message.source = {source.GetSiteID(), source.GetApplicationID(), source.GetEntityID()};
  message.destination = {destination.GetSiteID(), destination.GetApplicationID(), destination.GetEntityID()};
  message.exerciseId = pdu.GetExerciseID();
  message.requestId = pdu.GetRequestID();
}
}

QByteArray DisSimulationControlCodec::encodeAcknowledgement(const Acknowledgement& message, QString* error) {
  if (error) error->clear();
  try {
    KDIS::PDU::Acknowledge_PDU pdu;
    pdu.SetAcknowledgeFlag(static_cast<KDIS::DATA_TYPE::ENUMS::AcknowledgeFlag>(message.acknowledgeFlag));
    pdu.SetAcknowledgeResponseFlag(static_cast<KDIS::DATA_TYPE::ENUMS::AcknowledgeResponseFlag>(message.responseFlag));
    return encodePdu(pdu, message);
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return {};
  }
}

bool DisSimulationControlCodec::decodeAcknowledgement(const QByteArray& data, Acknowledgement& message, QString* error) {
  if (error) error->clear();
  try {
    if (data.size() != 32 || static_cast<unsigned char>(data[0]) != 7 ||
        static_cast<unsigned char>(data[2]) != 15 || static_cast<unsigned char>(data[3]) != 5 ||
        static_cast<unsigned char>(data[8]) != 0 || static_cast<unsigned char>(data[9]) != 32)
      throw std::runtime_error("Invalid DIS acknowledgement header or length.");
    KDIS::KDataStream stream(reinterpret_cast<const KDIS::KOCTET*>(data.constData()), 32);
    KDIS::PDU::Acknowledge_PDU pdu(stream);
    Acknowledgement result;
    decodePdu(pdu, result);
    result.acknowledgeFlag = pdu.GetAcknowledgeFlag();
    result.responseFlag = pdu.GetAcknowledgeResponseFlag();
    message = result;
    return true;
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return false;
  }
}

QByteArray DisSimulationControlCodec::encode(const SimulationControlMessage& message, QString* error) {
  if (error) error->clear();
  try {
    if (!std::isfinite(message.simulationTimeSeconds) || message.simulationTimeSeconds < 0 ||
        message.simulationTimeSeconds / 3600.0 > 2147483647.0) {
      throw std::runtime_error("Invalid DIS simulation time.");
    }
    if (message.control == SimulationControl::StartResume) {
      KDIS::PDU::Start_Resume_PDU pdu;
      pdu.SetSimulationTime({static_cast<KDIS::KINT32>(message.simulationTimeSeconds / 3600.0),
          static_cast<KDIS::KUINT32>(std::fmod(message.simulationTimeSeconds, 3600.0) /
              KDIS::DATA_TYPE::ClockTime::SEC_PER_UNIT_TIME)});
      return encodePdu(pdu, message);
    }
    KDIS::PDU::Stop_Freeze_PDU pdu;
    pdu.SetReason(message.control == SimulationControl::Stop
        ? KDIS::DATA_TYPE::ENUMS::Termination : KDIS::DATA_TYPE::ENUMS::Recess);
    // Freeze the clock while continuing network transmission and reception.
    pdu.SetFrozenBehavior(static_cast<KDIS::DATA_TYPE::ENUMS::FrozenBehavior>(1));
    return encodePdu(pdu, message);
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return {};
  }
}

bool DisSimulationControlCodec::decode(const QByteArray& data, SimulationControlMessage& message, QString* error) {
  if (error) error->clear();
  try {
    if (data.size() < 12 || static_cast<unsigned char>(data[0]) != 7 ||
        static_cast<unsigned char>(data[3]) != 5) throw std::runtime_error("Invalid DIS control header.");
    const auto type = static_cast<unsigned char>(data[2]);
    const int size = type == 13 ? 44 : type == 14 ? 40 : 0;
    const int declared = static_cast<unsigned char>(data[8]) * 256 + static_cast<unsigned char>(data[9]);
    if (!size || data.size() != size || declared != size) throw std::runtime_error("Invalid DIS control length.");
    KDIS::KDataStream stream(reinterpret_cast<const KDIS::KOCTET*>(data.constData()), size);
    SimulationControlMessage result;
    if (type == 13) {
      KDIS::PDU::Start_Resume_PDU pdu(stream);
      decodePdu(pdu, result);
      result.simulationTimeSeconds = pdu.GetSimulationTime().GetHour() * 3600.0 +
          pdu.GetSimulationTime().GetTimePastHour() * KDIS::DATA_TYPE::ClockTime::SEC_PER_UNIT_TIME;
    } else {
      KDIS::PDU::Stop_Freeze_PDU pdu(stream);
      decodePdu(pdu, result);
      const auto reason = pdu.GetReason();
      result.control = reason == KDIS::DATA_TYPE::ENUMS::Termination ||
              reason == KDIS::DATA_TYPE::ENUMS::StopForReset ||
              reason == KDIS::DATA_TYPE::ENUMS::StopForRestart
          ? SimulationControl::Stop : SimulationControl::Pause;
    }
    message = result;
    return true;
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return false;
  }
}
}
