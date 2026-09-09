#include "infrastructure/interoperability/dis/DisEntityStateCodec.h"

#include "geospatial/GeographicLibGeospatialService.h"

#include <KDIS/DataTypes/DeadReckoningParameter.hpp>
#include <KDIS/DataTypes/EntityAppearance.hpp>
#include <KDIS/DataTypes/EntityCapabilities.hpp>
#include <KDIS/DataTypes/EntityIdentifier.hpp>
#include <KDIS/DataTypes/EntityMarking.hpp>
#include <KDIS/DataTypes/EntityType.hpp>
#include <KDIS/DataTypes/EulerAngles.hpp>
#include <KDIS/DataTypes/TimeStamp.hpp>
#include <KDIS/DataTypes/Vector.hpp>
#include <KDIS/DataTypes/WorldCoordinates.hpp>
#include <KDIS/Extras/KConversions.hpp>
#include <KDIS/Extras/PDU_Factory.hpp>
#include <KDIS/PDU/Entity_Info_Interaction/Entity_State_PDU.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>

namespace tactical::dis {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr double kKnotsToMetersPerSecond = 0.514444;

double radians(double degrees) { return degrees * kPi / 180.0; }
double degrees(double radiansValue) { return radiansValue * 180.0 / kPi; }

double normalizedHeading(double heading) {
  heading = std::fmod(heading, 360.0);
  return heading < 0.0 ? heading + 360.0 : heading;
}

KDIS::DATA_TYPE::Vector ecefVelocity(const EntityState& state) {
  const double latitude = radians(state.latitudeDegrees);
  const double longitude = radians(state.longitudeDegrees);
  const double heading = radians(state.headingDegrees);
  const double horizontal =
      std::max(0.0, state.speedKnots) * kKnotsToMetersPerSecond;
  const double east = horizontal * std::sin(heading);
  const double north = horizontal * std::cos(heading);
  const double up = state.verticalSpeedMetersPerSecond;
  const double sinLatitude = std::sin(latitude);
  const double cosLatitude = std::cos(latitude);
  const double sinLongitude = std::sin(longitude);
  const double cosLongitude = std::cos(longitude);
  return {
      static_cast<float>(-sinLongitude * east -
                         sinLatitude * cosLongitude * north +
                         cosLatitude * cosLongitude * up),
      static_cast<float>(cosLongitude * east -
                         sinLatitude * sinLongitude * north +
                         cosLatitude * sinLongitude * up),
      static_cast<float>(cosLatitude * north + sinLatitude * up)};
}

void decodeVelocity(
    const KDIS::DATA_TYPE::Vector& velocity,
    EntityState& state) {
  const double latitude = radians(state.latitudeDegrees);
  const double longitude = radians(state.longitudeDegrees);
  const double sinLatitude = std::sin(latitude);
  const double cosLatitude = std::cos(latitude);
  const double sinLongitude = std::sin(longitude);
  const double cosLongitude = std::cos(longitude);
  const double x = velocity.GetX();
  const double y = velocity.GetY();
  const double z = velocity.GetZ();
  const double east = -sinLongitude * x + cosLongitude * y;
  const double north = -sinLatitude * cosLongitude * x -
      sinLatitude * sinLongitude * y + cosLatitude * z;
  const double up = cosLatitude * cosLongitude * x +
      cosLatitude * sinLongitude * y + sinLatitude * z;
  state.speedKnots = std::hypot(east, north) / kKnotsToMetersPerSecond;
  state.verticalSpeedMetersPerSecond = up;
}

std::uint32_t appearanceData(const EntityState& state) {
  const std::uint32_t damage = state.destroyed || state.damagePercent >= 100.0
      ? 3U
      : state.damagePercent >= 30.0 ? 2U
      : state.damagePercent > 0.0 ? 1U : 0U;
  return damage << 3U;
}

} // namespace

QByteArray DisEntityStateCodec::encode(
    const EntityState& state,
    QString* error) {
  try {
    const auto& geospatial = geospatial::wgs84GeospatialService();
    const geospatial::EcefCoordinate location = geospatial.geodeticToEcef({
        state.latitudeDegrees,
        state.longitudeDegrees,
        state.altitudeMeters});

    double psi = 0.0;
    double theta = 0.0;
    double phi = 0.0;
    KDIS::UTILS::HeadingPitchRollToEuler(
        radians(state.headingDegrees),
        radians(state.pitchDegrees),
        radians(state.rollDegrees),
        radians(state.latitudeDegrees),
        radians(state.longitudeDegrees),
        psi,
        theta,
        phi);

    using namespace KDIS::DATA_TYPE;
    using namespace KDIS::DATA_TYPE::ENUMS;
    EntityAppearance appearance;
    appearance.SetData(appearanceData(state));
    const bool moving = state.speedKnots > 0.001 ||
        std::abs(state.verticalSpeedMetersPerSecond) > 0.001;
    const DeadReckoningParameter deadReckoning(
        moving ? DRM_F_P_W : Static, Vector(), Vector());
    KDIS::PDU::Entity_State_PDU pdu(
        KDIS::DATA_TYPE::EntityIdentifier(
            state.identifier.site,
            state.identifier.application,
            state.identifier.entity),
        static_cast<ForceID>(state.forceIdentifier),
        EntityType(
            static_cast<EntityKind>(state.entityKind),
            static_cast<EntityDomain>(state.entityDomain),
            static_cast<Country>(state.country),
            state.category,
            state.subcategory,
            state.specific,
            state.extra),
        EntityType(),
        ecefVelocity(state),
        WorldCoordinates(location.xMeters, location.yMeters, location.zMeters),
        EulerAngles(
            static_cast<float>(psi),
            static_cast<float>(theta),
            static_cast<float>(phi)),
        appearance,
        deadReckoning,
        EntityMarking(ASCII, state.marking.left(11).toStdString()),
        EntityCapabilities(false, false, false, false));
    pdu.SetExerciseID(state.exerciseId);
    pdu.SetProtocolVersion(IEEE_1278_1_2012);
    pdu.SetTimeStamp(TimeStamp(RelativeTime, 0, true));
    const KDIS::KDataStream stream = pdu.Encode();
    return QByteArray(
        reinterpret_cast<const char*>(stream.GetBufferPtr()),
        stream.GetBufferSize());
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return {};
  }
}

bool DisEntityStateCodec::decode(
    const QByteArray& datagram,
    EntityState& state,
    QString* error) {
  if (datagram.size() < KDIS::PDU::Entity_State_PDU::ENTITY_STATE_PDU_SIZE ||
      datagram.size() > std::numeric_limits<KDIS::KUINT16>::max()) {
    if (error) *error = QStringLiteral("Invalid DIS Entity State PDU size.");
    return false;
  }
  try {
    KDIS::KDataStream stream(
        reinterpret_cast<const KDIS::KOCTET*>(datagram.constData()),
        static_cast<KDIS::KUINT16>(datagram.size()));
    KDIS::UTILS::PDU_Factory factory;
    std::unique_ptr<KDIS::PDU::Header> header = factory.Decode(stream);
    auto* pdu = dynamic_cast<KDIS::PDU::Entity_State_PDU*>(header.get());
    if (!pdu) {
      if (error) *error = QStringLiteral("Datagram is not an Entity State PDU.");
      return false;
    }

    const auto& identifier = pdu->GetEntityIdentifier();
    state.identifier = {
        identifier.GetSiteID(),
        identifier.GetApplicationID(),
        identifier.GetEntityID()};
    state.exerciseId = pdu->GetExerciseID();
    state.marking = QString::fromStdString(
        pdu->GetEntityMarking().GetEntityMarkingString()).trimmed();
    state.forceIdentifier = static_cast<std::uint8_t>(pdu->GetForceID());
    const auto& type = pdu->GetEntityType();
    state.entityKind = static_cast<std::uint8_t>(type.GetEntityKind());
    state.entityDomain = static_cast<std::uint8_t>(type.GetDomain());
    state.country = static_cast<std::uint16_t>(type.GetCountry());
    state.category = type.GetCategory();
    state.subcategory = type.GetSubCategory();
    state.specific = type.GetSpecific();
    state.extra = type.GetExtra();

    const auto& location = pdu->GetEntityLocation();
    const auto geodetic = geospatial::wgs84GeospatialService().ecefToGeodetic({
        location.GetX(), location.GetY(), location.GetZ()});
    state.latitudeDegrees = geodetic.latitude;
    state.longitudeDegrees = geodetic.longitude;
    state.altitudeMeters = geodetic.altitudeMeters;

    const auto& orientation = pdu->GetEntityOrientation();
    double heading = 0.0;
    double pitch = 0.0;
    double roll = 0.0;
    KDIS::UTILS::EulerToHeadingPitchRoll(
        radians(state.latitudeDegrees),
        radians(state.longitudeDegrees),
        static_cast<double>(orientation.GetPsiInRadians()),
        static_cast<double>(orientation.GetThetaInRadians()),
        static_cast<double>(orientation.GetPhiInRadians()),
        heading,
        pitch,
        roll);
    state.headingDegrees = normalizedHeading(degrees(heading));
    state.pitchDegrees = degrees(pitch);
    state.rollDegrees = degrees(roll);
    decodeVelocity(pdu->GetEntityLinearVelocity(), state);

    const std::uint32_t damage =
        (pdu->GetEntityAppearance().GetData() >> 3U) & 0x3U;
    state.damagePercent = damage == 0U ? 0.0 : damage == 1U ? 10.0
        : damage == 2U ? 50.0 : 100.0;
    state.destroyed = damage == 3U;
    return true;
  } catch (const std::exception& exception) {
    if (error) *error = QString::fromUtf8(exception.what());
    return false;
  }
}

} // namespace tactical::dis
