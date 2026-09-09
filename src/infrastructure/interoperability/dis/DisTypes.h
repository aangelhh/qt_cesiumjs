#pragma once

#include <QString>

#include <cstdint>
#include <vector>

namespace tactical::dis {

enum class SimulationControl { StartResume, Pause, Stop };

struct EntityIdentifier {
  std::uint16_t site = 0;
  std::uint16_t application = 0;
  std::uint16_t entity = 0;

  QString key() const {
    return QStringLiteral("dis:%1:%2:%3")
        .arg(site)
        .arg(application)
        .arg(entity);
  }

  bool operator==(const EntityIdentifier& other) const {
    return site == other.site && application == other.application &&
        entity == other.entity;
  }
};

struct SimulationControlMessage {
  SimulationControl control = SimulationControl::StartResume;
  EntityIdentifier source;
  EntityIdentifier destination{65535, 65535, 65535};
  std::uint8_t exerciseId = 1;
  std::uint32_t requestId = 0;
  double simulationTimeSeconds = 0.0;
};

struct Acknowledgement {
  EntityIdentifier source;
  EntityIdentifier destination;
  std::uint8_t exerciseId = 1;
  std::uint32_t requestId = 0;
  std::uint16_t acknowledgeFlag = 3;
  std::uint16_t responseFlag = 1;
};

struct EntityManagementRequest {
  bool remove = false;
  EntityIdentifier source;
  EntityIdentifier destination;
  std::uint8_t exerciseId = 1;
  std::uint32_t requestId = 0;
};

struct IffState {
  EntityIdentifier host;
  EntityIdentifier event;
  std::uint8_t exerciseId = 1;
  std::uint8_t systemId = 1;
  std::uint16_t systemType = 1;
  bool on = false;
  bool operational = false;
};

struct CollisionEvent {
  EntityIdentifier source;
  EntityIdentifier target;
  EntityIdentifier event;
  std::uint8_t exerciseId = 1;
  std::uint8_t collisionType = 0;
  float massKilograms = 0;
  // DIS uses ECEF velocity and a contact point relative to the issuing entity.
  float velocityX = 0, velocityY = 0, velocityZ = 0;
  float locationX = 0, locationY = 0, locationZ = 0;
};

struct EntityState {
  EntityIdentifier identifier;
  std::uint8_t exerciseId = 1;
  QString marking;
  std::uint8_t forceIdentifier = 0;
  std::uint8_t entityKind = 0;
  std::uint8_t entityDomain = 0;
  std::uint16_t country = 0;
  std::uint8_t category = 0;
  std::uint8_t subcategory = 0;
  std::uint8_t specific = 0;
  std::uint8_t extra = 0;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double rollDegrees = 0.0;
  double speedKnots = 0.0;
  double verticalSpeedMetersPerSecond = 0.0;
  double damagePercent = 0.0;
  bool destroyed = false;
};

struct RadarBeam {
  std::uint8_t systemId = 1;
  std::uint8_t beamId = 1;
  bool emitting = false;
  double azimuthCenterDegrees = 0;
  double azimuthWidthDegrees = 360;
  double elevationCenterDegrees = 0;
  double elevationWidthDegrees = 90;
  double frequencyHertz = 0;
  double bandwidthHertz = 0;
  double effectiveRadiatedPowerDbm = 0;
};

struct RadarEmission {
  EntityIdentifier host;
  std::uint8_t exerciseId = 1;
  bool changedData = false;
  std::vector<RadarBeam> beams;
};

struct RemoteEntityChange {
  EntityState state;
  bool removed = false;
};

enum class WarfareEventKind {
  Fire,
  Detonation
};

struct WarfareEvent {
  WarfareEventKind kind = WarfareEventKind::Fire;
  std::uint8_t exerciseId = 1;
  EntityIdentifier firingEntity;
  EntityIdentifier targetEntity;
  EntityIdentifier munitionEntity;
  EntityIdentifier eventIdentifier;
  QString munitionType;
  double latitudeDegrees = 0.0;
  double longitudeDegrees = 0.0;
  double altitudeMeters = 0.0;
  double headingDegrees = 0.0;
  double pitchDegrees = 0.0;
  double speedMetersPerSecond = 0.0;
  std::uint8_t detonationResult = 0;
};

struct Result {
  bool success = true;
  QString message;

  static Result fail(const QString& message) { return {false, message}; }
};

} // namespace tactical::dis
