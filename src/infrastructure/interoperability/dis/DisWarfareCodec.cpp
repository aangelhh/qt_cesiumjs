#include "infrastructure/interoperability/dis/DisWarfareCodec.h"

#include "geospatial/GeographicLibGeospatialService.h"

#include <KDIS/DataTypes/EntityIdentifier.hpp>
#include <KDIS/DataTypes/EntityType.hpp>
#include <KDIS/DataTypes/MunitionDescriptor.hpp>
#include <KDIS/DataTypes/TimeStamp.hpp>
#include <KDIS/DataTypes/Vector.hpp>
#include <KDIS/DataTypes/WorldCoordinates.hpp>
#include <KDIS/Extras/PDU_Factory.hpp>
#include <KDIS/PDU/Warfare/Detonation_PDU.hpp>
#include <KDIS/PDU/Warfare/Fire_PDU.hpp>

#include <cmath>
#include <limits>
#include <memory>

namespace tactical::dis {
namespace {

constexpr double kPi = 3.14159265358979323846;

double radians(double degrees) { return degrees * kPi / 180.0; }
double degrees(double radiansValue) { return radiansValue * 180.0 / kPi; }

KDIS::DATA_TYPE::EntityIdentifier identifier(
    const EntityIdentifier& value) {
  return {value.site, value.application, value.entity};
}

EntityIdentifier identifier(
    const KDIS::DATA_TYPE::EntityIdentifier& value) {
  return {
      value.GetSiteID(), value.GetApplicationID(), value.GetEntityID()};
}

KDIS::DATA_TYPE::EntityType munitionEntityType(const QString& type) {
  using namespace KDIS::DATA_TYPE::ENUMS;
  return {
      Munition,
      Air,
      static_cast<Country>(0),
      static_cast<KDIS::KUINT8>(
          type.compare(QStringLiteral("Bomb"), Qt::CaseInsensitive) == 0
              ? 2 : 1),
      0,
      0,
      0};
}

KDIS::DATA_TYPE::DescPtr munitionDescriptor(const QString& type) {
  using namespace KDIS::DATA_TYPE;
  using namespace KDIS::DATA_TYPE::ENUMS;
  return DescPtr(new MunitionDescriptor(
      munitionEntityType(type),
      HighExplosive_HE_Warhead,
      Contact_Instant_Impact,
      1,
      0));
}

KDIS::DATA_TYPE::WorldCoordinates worldLocation(const WarfareEvent& event) {
  const auto ecef = geospatial::wgs84GeospatialService().geodeticToEcef({
      event.latitudeDegrees,
      event.longitudeDegrees,
      event.altitudeMeters});
  return {ecef.xMeters, ecef.yMeters, ecef.zMeters};
}

void decodeWorldLocation(
    const KDIS::DATA_TYPE::WorldCoordinates& location,
    WarfareEvent& event) {
  const auto geodetic = geospatial::wgs84GeospatialService().ecefToGeodetic({
      location.GetX(), location.GetY(), location.GetZ()});
  event.latitudeDegrees = geodetic.latitude;
  event.longitudeDegrees = geodetic.longitude;
  event.altitudeMeters = geodetic.altitudeMeters;
}

KDIS::DATA_TYPE::Vector ecefVelocity(const WarfareEvent& event) {
  const double latitude = radians(event.latitudeDegrees);
  const double longitude = radians(event.longitudeDegrees);
  const double heading = radians(event.headingDegrees);
  const double pitch = radians(event.pitchDegrees);
  const double horizontal =
      event.speedMetersPerSecond * std::cos(pitch);
  const double east = horizontal * std::sin(heading);
  const double north = horizontal * std::cos(heading);
  const double up = event.speedMetersPerSecond * std::sin(pitch);
  return {
      static_cast<float>(
          -std::sin(longitude) * east -
          std::sin(latitude) * std::cos(longitude) * north +
          std::cos(latitude) * std::cos(longitude) * up),
      static_cast<float>(
          std::cos(longitude) * east -
          std::sin(latitude) * std::sin(longitude) * north +
          std::cos(latitude) * std::sin(longitude) * up),
      static_cast<float>(
          std::cos(latitude) * north + std::sin(latitude) * up)};
}

void decodeVelocity(
    const KDIS::DATA_TYPE::Vector& velocity,
    WarfareEvent& event) {
  const double latitude = radians(event.latitudeDegrees);
  const double longitude = radians(event.longitudeDegrees);
  const double x = velocity.GetX();
  const double y = velocity.GetY();
  const double z = velocity.GetZ();
  const double east = -std::sin(longitude) * x + std::cos(longitude) * y;
  const double north =
      -std::sin(latitude) * std::cos(longitude) * x -
      std::sin(latitude) * std::sin(longitude) * y +
      std::cos(latitude) * z;
  const double up =
      std::cos(latitude) * std::cos(longitude) * x +
      std::cos(latitude) * std::sin(longitude) * y +
      std::sin(latitude) * z;
  event.speedMetersPerSecond = std::sqrt(east * east + north * north + up * up);
  event.headingDegrees = std::fmod(degrees(std::atan2(east, north)) + 360.0, 360.0);
  event.pitchDegrees = degrees(std::atan2(up, std::hypot(east, north)));
}

void configureHeader(KDIS::PDU::Header& pdu, std::uint8_t exerciseId) {
  using namespace KDIS::DATA_TYPE::ENUMS;
  pdu.SetExerciseID(exerciseId);
  pdu.SetProtocolVersion(IEEE_1278_1_2012);
  pdu.SetTimeStamp(KDIS::DATA_TYPE::TimeStamp(RelativeTime, 0, true));
}

QByteArray encodedDatagram(const KDIS::KDataStream& stream) {
  return QByteArray(
      reinterpret_cast<const char*>(stream.GetBufferPtr()),
      stream.GetBufferSize());
}

QString descriptorType(
    const KDIS::DATA_TYPE::DescPtr& descriptor) {
  if (!descriptor.GetPtr()) return QStringLiteral("Munition");
  return descriptor->GetType().GetCategory() == 2
      ? QStringLiteral("Bomb") : QStringLiteral("Missile");
}

void decodeHeader(
    const KDIS::PDU::Warfare_Header& pdu,
    WarfareEvent& event) {
  event.exerciseId = pdu.GetExerciseID();
  event.firingEntity = identifier(pdu.GetFiringEntityID());
  event.targetEntity = identifier(pdu.GetTargetEntityID());
  event.munitionEntity = identifier(pdu.GetMunitionID());
  event.eventIdentifier = identifier(pdu.GetEventID());
}

} // namespace

QByteArray DisWarfareCodec::encodeFire(
    const WarfareEvent& event,
    QString* error) {
  try {
    KDIS::PDU::Fire_PDU pdu(
        identifier(event.firingEntity),
        identifier(event.targetEntity),
        identifier(event.munitionEntity),
        identifier(event.eventIdentifier),
        event.eventIdentifier.entity,
        worldLocation(event),
        munitionDescriptor(event.munitionType),
        ecefVelocity(event),
        0.0F);
    configureHeader(pdu, event.exerciseId);
    return encodedDatagram(pdu.Encode());
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return {};
  }
}

QByteArray DisWarfareCodec::encodeDetonation(
    const WarfareEvent& event,
    QString* error) {
  try {
    using namespace KDIS::DATA_TYPE;
    using namespace KDIS::DATA_TYPE::ENUMS;
    KDIS::PDU::Detonation_PDU pdu(
        identifier(event.firingEntity),
        identifier(event.targetEntity),
        identifier(event.munitionEntity),
        identifier(event.eventIdentifier),
        ecefVelocity(event),
        worldLocation(event),
        munitionDescriptor(event.munitionType),
        Vector(),
        static_cast<DetonationResult>(event.detonationResult));
    configureHeader(pdu, event.exerciseId);
    return encodedDatagram(pdu.Encode());
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return {};
  }
}

bool DisWarfareCodec::decode(
    const QByteArray& datagram,
    WarfareEvent& event,
    QString* error) {
  if (datagram.size() < KDIS::PDU::Header::HEADER6_PDU_SIZE ||
      datagram.size() > std::numeric_limits<KDIS::KUINT16>::max()) {
    if (error) *error = QStringLiteral("Invalid DIS warfare PDU size.");
    return false;
  }
  try {
    KDIS::KDataStream stream(
        reinterpret_cast<const KDIS::KOCTET*>(datagram.constData()),
        static_cast<KDIS::KUINT16>(datagram.size()));
    KDIS::UTILS::PDU_Factory factory;
    std::unique_ptr<KDIS::PDU::Header> header = factory.Decode(stream);
    if (auto* fire = dynamic_cast<KDIS::PDU::Fire_PDU*>(header.get())) {
      event = {};
      event.kind = WarfareEventKind::Fire;
      decodeHeader(*fire, event);
      decodeWorldLocation(fire->GetLocation(), event);
      decodeVelocity(fire->GetVelocity(), event);
      event.munitionType = descriptorType(fire->GetDescriptor());
      return true;
    }
    if (auto* detonation =
            dynamic_cast<KDIS::PDU::Detonation_PDU*>(header.get())) {
      event = {};
      event.kind = WarfareEventKind::Detonation;
      decodeHeader(*detonation, event);
      decodeWorldLocation(detonation->GetLocationInWorldCoords(), event);
      decodeVelocity(detonation->GetVelocity(), event);
      event.munitionType = descriptorType(detonation->GetDescriptor());
      event.detonationResult = static_cast<std::uint8_t>(
          detonation->GetDetonationResult());
      return true;
    }
    if (error) *error = QStringLiteral("Datagram is not a DIS warfare PDU.");
    return false;
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return false;
  }
}

} // namespace tactical::dis
