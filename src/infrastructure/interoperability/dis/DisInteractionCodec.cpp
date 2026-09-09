#include "infrastructure/interoperability/dis/DisInteractionCodec.h"
#include <KDIS/PDU/Simulation_Management/Create_Entity_PDU.hpp>
#include <KDIS/PDU/Simulation_Management/Remove_Entity_PDU.hpp>
#include <KDIS/PDU/Entity_Info_Interaction/Collision_PDU.hpp>
#include <KDIS/PDU/Distributed_Emission_Regeneration/IFF_PDU.hpp>
#include <cmath>
#include <stdexcept>

namespace tactical::dis {
namespace {
using namespace KDIS::DATA_TYPE::ENUMS;
KDIS::DATA_TYPE::EntityIdentifier id(const EntityIdentifier& v) { return {v.site, v.application, v.entity}; }
EntityIdentifier id(const KDIS::DATA_TYPE::EntityIdentifier& v) { return {v.GetSiteID(), v.GetApplicationID(), v.GetEntityID()}; }
QByteArray bytes(KDIS::PDU::Header& pdu, std::uint8_t exercise) {
  pdu.SetProtocolVersion(IEEE_1278_1_2012);
  pdu.SetExerciseID(exercise);
  const auto stream = pdu.Encode();
  return {reinterpret_cast<const char*>(stream.GetBufferPtr()), stream.GetBufferSize()};
}
void validate(const QByteArray& data, int type, int family, int size) {
  if (data.size() != size || static_cast<unsigned char>(data[0]) != 7 ||
      static_cast<unsigned char>(data[2]) != type || static_cast<unsigned char>(data[3]) != family ||
      static_cast<unsigned char>(data[8]) * 256 + static_cast<unsigned char>(data[9]) != size)
    throw std::runtime_error("Invalid DIS interaction header or length.");
}
void validateCollision(const CollisionEvent& v) {
  for (float f : {v.massKilograms, v.velocityX, v.velocityY, v.velocityZ, v.locationX, v.locationY, v.locationZ})
    if (!std::isfinite(f)) throw std::runtime_error("Non-finite DIS collision value.");
  if (v.massKilograms < 0 || v.collisionType > 1) throw std::runtime_error("Invalid DIS collision parameters.");
}
template<class Pdu>
QByteArray management(const EntityManagementRequest& v) {
  Pdu pdu;
  pdu.SetOriginatingEntityID(id(v.source));
  pdu.SetReceivingEntityID(id(v.destination));
  pdu.SetRequestID(v.requestId);
  return bytes(pdu, v.exerciseId);
}
template<class Pdu>
EntityManagementRequest management(KDIS::KDataStream& stream, bool remove) {
  Pdu pdu(stream);
  return {remove, id(pdu.GetOriginatingEntityID()), id(pdu.GetReceivingEntityID()), pdu.GetExerciseID(), pdu.GetRequestID()};
}
}
QByteArray DisInteractionCodec::encode(const EntityManagementRequest& v, QString* error) {
  if (error) error->clear();
  try {
    return v.remove ? management<KDIS::PDU::Remove_Entity_PDU>(v) : management<KDIS::PDU::Create_Entity_PDU>(v);
  } catch (const std::exception& e) { if (error) *error = QString::fromUtf8(e.what()); return {}; }
}
bool DisInteractionCodec::decode(const QByteArray& data, EntityManagementRequest& v, QString* error) {
  if (error) error->clear();
  try {
    const bool remove = data.size() >= 3 && data[2] == 12;
    validate(data, remove ? 12 : 11, 5, 28);
    KDIS::KDataStream stream(reinterpret_cast<const KDIS::KOCTET*>(data.constData()), data.size());
    v = remove ? management<KDIS::PDU::Remove_Entity_PDU>(stream, true) : management<KDIS::PDU::Create_Entity_PDU>(stream, false);
    return true;
  } catch (const std::exception& e) { if (error) *error = QString::fromUtf8(e.what()); return false; }
}
QByteArray DisInteractionCodec::encode(const IffState& v, QString* error) {
  if (error) error->clear();
  try {
    // Layer 1 only: do not claim Mode 4/5 authentication or synthesize identity codes.
    if (v.systemType != 1) throw std::runtime_error("Only Mark X/XII transponder layer 1 is supported.");
    KDIS::PDU::IFF_PDU pdu;
    pdu.SetEmittingEntityID(id(v.host));
    pdu.SetEventID(id(v.event));
    pdu.SetSystemDesignator(v.systemId);
    KDIS::DATA_TYPE::SystemIdentifier system;
    system.SetSystemType(Mark_X_XII_ATCRBS_ModeS_Transponder);
    system.SetSystemMode(v.on ? Normal : OffSystemMode);
    pdu.SetSystemIdentifier(system);
    KDIS::DATA_TYPE::FundamentalOperationalData data;
    auto& transponder = data.GetSetFundamentalOperationalData_MarkXTransponder();
    transponder.SetSystemStatusSystemOn(v.on);
    transponder.SetSystemStatusIsOperational(v.operational);
    transponder.SetInfomationLayer1(true);
    pdu.SetFundamentalOperationalData(data);
    return bytes(pdu, v.exerciseId);
  } catch (const std::exception& e) { if (error) *error = QString::fromUtf8(e.what()); return {}; }
}
bool DisInteractionCodec::decode(const QByteArray& data, IffState& v, QString* error) {
  if (error) error->clear();
  try {
    validate(data, 28, 6, 60);
    KDIS::KDataStream stream(reinterpret_cast<const KDIS::KOCTET*>(data.constData()), data.size());
    KDIS::PDU::IFF_PDU pdu(stream);
    if (pdu.GetSystemIdentifier().GetSystemType() != Mark_X_XII_ATCRBS_ModeS_Transponder)
      throw std::runtime_error("Unsupported DIS IFF system type.");
    const auto& t = pdu.GetFundamentalOperationalData().GetSetFundamentalOperationalData_MarkXTransponder();
    v = {id(pdu.GetEmittingEntityID()), id(pdu.GetEventID()), pdu.GetExerciseID(), pdu.GetSystemDesignator(), 1,
        t.GetSystemStatusSystemOn(), t.GetSystemStatusIsOperational()};
    return true;
  } catch (const std::exception& e) { if (error) *error = QString::fromUtf8(e.what()); return false; }
}
QByteArray DisInteractionCodec::encode(const CollisionEvent& v, QString* error) {
  if (error) error->clear();
  try {
    validateCollision(v);
    KDIS::PDU::Collision_PDU pdu;
    pdu.SetIssuingEntityID(id(v.source)); pdu.SetCollidingEntityID(id(v.target)); pdu.SetEventID(id(v.event));
    pdu.SetCollisionType(static_cast<CollisionType>(v.collisionType));
    pdu.SetMass(v.massKilograms);
    pdu.SetVelocity({v.velocityX, v.velocityY, v.velocityZ});
    pdu.SetLocation({v.locationX, v.locationY, v.locationZ});
    return bytes(pdu, v.exerciseId);
  } catch (const std::exception& e) { if (error) *error = QString::fromUtf8(e.what()); return {}; }
}
bool DisInteractionCodec::decode(const QByteArray& data, CollisionEvent& v, QString* error) {
  if (error) error->clear();
  try {
    validate(data, 4, 1, 60);
    KDIS::KDataStream stream(reinterpret_cast<const KDIS::KOCTET*>(data.constData()), data.size());
    KDIS::PDU::Collision_PDU pdu(stream);
    CollisionEvent result;
    result.source = id(pdu.GetIssuingEntityID()); result.target = id(pdu.GetCollidingEntityID()); result.event = id(pdu.GetEventID());
    result.exerciseId = pdu.GetExerciseID(); result.collisionType = pdu.GetCollisionType(); result.massKilograms = pdu.GetMass();
    result.velocityX = pdu.GetVelocity().GetX(); result.velocityY = pdu.GetVelocity().GetY(); result.velocityZ = pdu.GetVelocity().GetZ();
    result.locationX = pdu.GetLocation().GetX(); result.locationY = pdu.GetLocation().GetY(); result.locationZ = pdu.GetLocation().GetZ();
    validateCollision(result);
    v = result;
    return true;
  } catch (const std::exception& e) { if (error) *error = QString::fromUtf8(e.what()); return false; }
}
}
