#include "../include/DataTypes/BaseEntityDt.h"
#include "../include/utils/hla_utils.hpp"

DevStudio::BaseEntityDt::BaseEntityDt()
{
	// EntityType
	m_EntityKind = 0;
	m_Domain = 0;
	m_CountryCode = 0;
	m_Category = 0;
	m_Subcategory = 0;
	m_Specific = 0;
	m_Extra = 0;

	// EntityIdentifier
	m_EntityNumber = 0;

	// FederateIdentifier
	m_SiteID = 0;
	m_ApplicationID = 0;

	// IsPartOf
	m_SiteIdIspartOf = 0;
	m_ApplicationIdIspartOf = 0;
	m_EntityNumIsPartOf = 0;
	m_HostRTIObjectIdentifier = ""; // Cadena vac�a
	m_ConstituentPartNature = 0;
	m_ConstituentPartPosition = 0;
	m_StationNumber = 0;
	m_StationName = 0;
	m_BodyXDistance = 0.0f;
	m_BodyYDistance = 0.0f;
	m_BodyZDistance = 0.0f;
	m_Range = 0.0f;
	m_Bearing = 0.0f;

	// Spatial
	m_Velocity = 0.0;
	m_Lat = 0.0;
	m_Lon = 0.0;
	m_Alt = 0.0;
	m_Pitch = 0.0;
	m_Roll = 0.0;
	m_Yaw = 0.0;
	m_Psi = 0.0f;
	m_Theta = 0.0f;
	m_Phi = 0.0f;
	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;
	m_IsFrozen = false; // Inicializado a "no congelado"
	m_SpatialVariantEnum = 0;
	m_VelocityX = 0.0f;
	m_VelocityY = 0.0f;
	m_VelocityZ = 0.0f;
	m_AngularvelocityX = 0.0f;
	m_AngularvelocityY = 0.0f;
	m_AngularvelocityZ = 0.0f;
	m_AccelerationX = 0.0f;
	m_AccelerationY = 0.0f;
	m_AccelerationZ = 0.0f;
	m_heading = 0.0;

	// RelativeSpatial
	m_RelativeVelocity = 0.0;
	m_RelativeLat = 0.0;
	m_RelativeLon = 0.0;
	m_RelativeAlt = 0.0;
	m_RelativePitch = 0.0;
	m_RelativeRoll = 0.0;
	m_RelativeYaw = 0.0;
	m_RelativePsi = 0.0f;
	m_RelativeTheta = 0.0f;
	m_RelativePhi = 0.0f;
	m_RelativeX = 0.0f;
	m_RelativeY = 0.0f;
	m_RelativeZ = 0.0f;
	m_RelativeIsFrozen = false; // Inicializado a "no congelado"
	m_RelativeSpatialVariantEnum = 0;
	m_RelativeVelocityX = 0.0f;
	m_RelativeVelocityY = 0.0f;
	m_RelativeVelocityZ = 0.0f;
	m_RelativeAngularvelocityX = 0.0f;
	m_RelativeAngularvelocityY = 0.0f;
	m_RelativeAngularvelocityZ = 0.0f;
	m_RelativeAccelerationX = 0.0f;
	m_RelativeAccelerationY = 0.0f;
	m_RelativeAccelerationZ = 0.0f;
	m_Relativeheading = 0.0;

}

DevStudio::BaseEntityDt::~BaseEntityDt()
{
}

DevStudio::BaseEntityDt::BaseEntityDt(const BaseEntityDt& obj):	m_EntityKind(obj.m_EntityKind),	m_Domain(obj.m_Domain),	m_CountryCode(obj.m_CountryCode),	m_Category(obj.m_Category),	m_Subcategory(obj.m_Subcategory),	m_Specific(obj.m_Specific),
	m_Extra(obj.m_Extra),m_EntityNumber(obj.m_EntityNumber),m_SiteID(obj.m_SiteID),	m_ApplicationID(obj.m_ApplicationID),m_SiteIdIspartOf(obj.m_SiteIdIspartOf),m_ApplicationIdIspartOf(obj.m_ApplicationIdIspartOf), m_EntityNumIsPartOf(obj.m_EntityNumIsPartOf),
	m_HostRTIObjectIdentifier(obj.m_HostRTIObjectIdentifier),m_ConstituentPartNature(obj.m_ConstituentPartNature),m_ConstituentPartPosition(obj.m_ConstituentPartPosition),	m_StationNumber(obj.m_StationNumber),	m_StationName(obj.m_StationName),
	m_BodyXDistance(obj.m_BodyXDistance),	m_BodyYDistance(obj.m_BodyYDistance),m_BodyZDistance(obj.m_BodyZDistance),m_Range(obj.m_Range),	m_Bearing(obj.m_Bearing),m_Velocity(obj.m_Velocity),m_Lat(obj.m_Lat),m_Lon(obj.m_Lon),m_Alt(obj.m_Alt),
	m_Pitch(obj.m_Pitch),	m_Roll(obj.m_Roll),	m_Yaw(obj.m_Yaw),	m_Psi(obj.m_Psi),	m_Theta(obj.m_Theta),	m_Phi(obj.m_Phi),	m_X(obj.m_X),	m_Y(obj.m_Y),	m_Z(obj.m_Z),	m_IsFrozen(obj.m_IsFrozen),	m_SpatialVariantEnum(obj.m_SpatialVariantEnum),	m_VelocityX(obj.m_VelocityX),	m_VelocityY(obj.m_VelocityY),
	m_VelocityZ(obj.m_VelocityZ),	m_AngularvelocityX(obj.m_AngularvelocityX),	m_AngularvelocityY(obj.m_AngularvelocityY),	m_AngularvelocityZ(obj.m_AngularvelocityZ),
	m_AccelerationX(obj.m_AccelerationX),	m_AccelerationY(obj.m_AccelerationY),	m_AccelerationZ(obj.m_AccelerationZ),	m_heading(obj.m_heading),		m_RelativeVelocity(obj.m_RelativeVelocity),	m_RelativeLat(obj.m_RelativeLat),	m_RelativeLon(obj.m_RelativeLon),	m_RelativeAlt(obj.m_RelativeAlt),
	m_RelativePitch(obj.m_RelativePitch),	m_RelativeRoll(obj.m_RelativeRoll),	m_RelativeYaw(obj.m_RelativeYaw),	m_RelativePsi(obj.m_RelativePsi),	m_RelativeTheta(obj.m_RelativeTheta),	m_RelativePhi(obj.m_RelativePhi),	m_RelativeX(obj.m_RelativeX),	m_RelativeY(obj.m_RelativeY),	m_RelativeZ(obj.m_RelativeZ),	m_RelativeIsFrozen(obj.m_RelativeIsFrozen),	m_RelativeSpatialVariantEnum(obj.m_RelativeSpatialVariantEnum),	m_RelativeVelocityX(obj.m_RelativeVelocityX),	m_RelativeVelocityY(obj.m_RelativeVelocityY),	m_RelativeVelocityZ(obj.m_RelativeVelocityZ),	m_RelativeAngularvelocityX(obj.m_RelativeAngularvelocityX),
	m_RelativeAngularvelocityY(obj.m_RelativeAngularvelocityY),	m_RelativeAngularvelocityZ(obj.m_RelativeAngularvelocityZ),	m_RelativeAccelerationX(obj.m_RelativeAccelerationX),	m_RelativeAccelerationY(obj.m_RelativeAccelerationY),	m_RelativeAccelerationZ(obj.m_RelativeAccelerationZ),
	m_Relativeheading(obj.m_Relativeheading)
{
}

DevStudio::BaseEntityDt& DevStudio::BaseEntityDt::operator=(const BaseEntityDt& obj)
{
	if (this != &obj) {
		// EntityType
		m_EntityKind = obj.m_EntityKind;
		m_Domain = obj.m_Domain;
		m_CountryCode = obj.m_CountryCode;
		m_Category = obj.m_Category;
		m_Subcategory = obj.m_Subcategory;
		m_Specific = obj.m_Specific;
		m_Extra = obj.m_Extra;

		// EntityIdentifier
		m_EntityNumber = obj.m_EntityNumber;

		// FederateIdentifier
		m_SiteID = obj.m_SiteID;
		m_ApplicationID = obj.m_ApplicationID;

		// IsPartOf
		m_SiteIdIspartOf = obj.m_SiteIdIspartOf;
		m_ApplicationIdIspartOf = obj.m_ApplicationIdIspartOf;
		m_EntityNumIsPartOf = obj.m_EntityNumIsPartOf;
		m_HostRTIObjectIdentifier = obj.m_HostRTIObjectIdentifier;
		m_ConstituentPartNature = obj.m_ConstituentPartNature;
		m_ConstituentPartPosition = obj.m_ConstituentPartPosition;
		m_StationNumber = obj.m_StationNumber;
		m_StationName = obj.m_StationName;
		m_BodyXDistance = obj.m_BodyXDistance;
		m_BodyYDistance = obj.m_BodyYDistance;
		m_BodyZDistance = obj.m_BodyZDistance;
		m_Range = obj.m_Range;
		m_Bearing = obj.m_Bearing;

		// Spatial
		m_Velocity = obj.m_Velocity;
		m_Lat = obj.m_Lat;
		m_Lon = obj.m_Lon;
		m_Alt = obj.m_Alt;
		m_Pitch = obj.m_Pitch;
		m_Roll = obj.m_Roll;
		m_Yaw = obj.m_Yaw;
		m_Psi = obj.m_Psi;
		m_Theta = obj.m_Theta;
		m_Phi = obj.m_Phi;
		m_X = obj.m_X;
		m_Y = obj.m_Y;
		m_Z = obj.m_Z;
		m_IsFrozen = obj.m_IsFrozen;
		m_SpatialVariantEnum = obj.m_SpatialVariantEnum;
		m_VelocityX = obj.m_VelocityX;
		m_VelocityY = obj.m_VelocityY;
		m_VelocityZ = obj.m_VelocityZ;
		m_AngularvelocityX = obj.m_AngularvelocityX;
		m_AngularvelocityY = obj.m_AngularvelocityY;
		m_AngularvelocityZ = obj.m_AngularvelocityZ;
		m_AccelerationX = obj.m_AccelerationX;
		m_AccelerationY = obj.m_AccelerationY;
		m_AccelerationZ = obj.m_AccelerationZ;
		m_heading = obj.m_heading;

		// RelativeSpatial
		m_RelativeVelocity = obj.m_RelativeVelocity;
		m_RelativeLat = obj.m_RelativeLat;
		m_RelativeLon = obj.m_RelativeLon;
		m_RelativeAlt = obj.m_RelativeAlt;
		m_RelativePitch = obj.m_RelativePitch;
		m_RelativeRoll = obj.m_RelativeRoll;
		m_RelativeYaw = obj.m_RelativeYaw;
		m_RelativePsi = obj.m_RelativePsi;
		m_RelativeTheta = obj.m_RelativeTheta;
		m_RelativePhi = obj.m_RelativePhi;
		m_RelativeX = obj.m_RelativeX;
		m_RelativeY = obj.m_RelativeY;
		m_RelativeZ = obj.m_RelativeZ;
		m_RelativeIsFrozen = obj.m_RelativeIsFrozen;
		m_RelativeSpatialVariantEnum = obj.m_RelativeSpatialVariantEnum;
		m_RelativeVelocityX = obj.m_RelativeVelocityX;
		m_RelativeVelocityY = obj.m_RelativeVelocityY;
		m_RelativeVelocityZ = obj.m_RelativeVelocityZ;
		m_RelativeAngularvelocityX = obj.m_RelativeAngularvelocityX;
		m_RelativeAngularvelocityY = obj.m_RelativeAngularvelocityY;
		m_RelativeAngularvelocityZ = obj.m_RelativeAngularvelocityZ;
		m_RelativeAccelerationX = obj.m_RelativeAccelerationX;
		m_RelativeAccelerationY = obj.m_RelativeAccelerationY;
		m_RelativeAccelerationZ = obj.m_RelativeAccelerationZ;
		m_Relativeheading = obj.m_Relativeheading;

	}
	return *this;
}

void DevStudio::BaseEntityDt::convert_xyz_to_latlonalt(void)
{
	
		const float a = 6378137.0;  // Semi-major axis, meters
		const float e = static_cast<float>(8.1819190842622e-2);  // First eccentricity
		//std::cout << "X: " << m_X << "Y: " << m_Y << "Z: " << m_Z << std::endl;


		float b = std::sqrt(a * a * (1 - e * e));
		float ep = std::sqrt((a * a - b * b) / (b * b));
		float p = std::sqrt(m_X * m_X + m_Y * m_Y);
		float th = std::atan2(a * m_Z, b * p);

		float lon = std::atan2(m_Y, m_X);
		float lat = static_cast<float>(std::atan2((m_Z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		float N = static_cast<float>(a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		float alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LAT: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		m_Lon=(lon);
		m_Lat=(lat);
		m_Alt=(alt);
	
}


void DevStudio::BaseEntityDt::Relativeconvert_xyz_to_latlonalt(void)
{

	const float a = 6378137.0;  // Semi-major axis, meters
	const float e = static_cast<float>(8.1819190842622e-2);  // First eccentricity
	//std::cout << "X: " << m_X << "Y: " << m_Y << "Z: " << m_Z << std::endl;


	float b = std::sqrt(a * a * (1 - e * e));
	float ep = std::sqrt((a * a - b * b) / (b * b));
	float p = std::sqrt(m_RelativeX * m_RelativeX + m_RelativeY * m_RelativeY);
	float th = std::atan2(a * m_Z, b * p);

	float lon = std::atan2(m_RelativeY, m_RelativeX);
	float lat = static_cast<float>(std::atan2((m_Z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
	float N = static_cast<float>(a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
	float alt = p / std::cos(lat) - N;

	// Convert radians to degrees
	lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
	lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
	alt = static_cast<float>(meterToFeet(alt));
	//std::cout << "LAT: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
	m_RelativeLon = (lon);
	m_RelativeLat = (lat);
	m_RelativeAlt = (alt);

}

double DevStudio::BaseEntityDt::meterToFeet(float altitudeMeter)
{
	const float feetToMeter = static_cast<float>(3.28084);
	float altitudeFeet = altitudeMeter * feetToMeter;
	return altitudeFeet;
}

void DevStudio::BaseEntityDt::calculate(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll)
{
	lat = lat * 3.14159265358979323846 / 180;
	lon = lon * 3.14159265358979323846 / 180;
	EulerToHeadingPitchRoll(lat, lon, psi, theta, phi, heading, pitch, roll);
	float grados = static_cast<float>((heading * 180 / 3.14159265358979323846));
	if (grados < 0) {
		grados = grados + 360;
	}
	if (grados == 360) {
		grados = 0;
	}

	m_heading=(grados);
	m_Pitch=(pitch * 180 / 3.14159265358979323846);
	m_Roll=(roll * 180 / 3.14159265358979323846);

}

void DevStudio::BaseEntityDt::Relativecalculate(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll)
{
	lat = lat * 3.14159265358979323846 / 180;
	lon = lon * 3.14159265358979323846 / 180;
	EulerToHeadingPitchRoll(lat, lon, psi, theta, phi, heading, pitch, roll);
	float grados = static_cast<float>((heading * 180 / 3.14159265358979323846));
	if (grados < 0) {
		grados = grados + 360;
	}
	if (grados == 360) {
		grados = 0;
	}

	m_Relativeheading = (grados);
	m_RelativePitch = (pitch * 180 / 3.14159265358979323846);
	m_RelativeRoll = (roll * 180 / 3.14159265358979323846);
}

void DevStudio::BaseEntityDt::setEntityType(DevStudio::EntityTypeStruct val)
{
	m_EntityKind = val.getEntityKind();
	m_Domain = val.getDomain();
	m_CountryCode = val.getCountryCode();
	m_Category = val.getCategory();
	m_Subcategory = val.getSubcategory();
	m_Specific = val.getSpecific();
	m_Extra = val.getExtra();
}

void DevStudio::BaseEntityDt::setEntityIdentifier(DevStudio::EntityIdentifierStruct val)
{
	m_EntityNumber = val.getEntityNumber();
	m_SiteID = val.getFederateIdentifier().getSiteID();
	m_ApplicationID = val.getFederateIdentifier().getApplicationID();

}

void DevStudio::BaseEntityDt::setIsPartOf(DevStudio::IsPartOfStruct val)
{
	m_ConstituentPartNature = val.getRelationship().getNature();
	m_ConstituentPartPosition = val.getRelationship().getPosition();
	m_HostRTIObjectIdentifier=val.getHostRTIObjectIdentifier();
	m_EntityNumIsPartOf =val.getHostEntityIdentifier().getEntityNumber();
	m_SiteIdIspartOf =val.getHostEntityIdentifier().getFederateIdentifier().getSiteID();
	m_ApplicationIdIspartOf =val.getHostEntityIdentifier().getFederateIdentifier().getApplicationID();
	m_StationName=val.getNamedLocation().getStationName().getDiscriminant();
	m_BodyXDistance =val.getNamedLocation().getStationName().getRelativeLocation()->getBodyXDistance();
	m_BodyYDistance =val.getNamedLocation().getStationName().getRelativeLocation()->getBodyYDistance();
	m_BodyZDistance =val.getNamedLocation().getStationName().getRelativeLocation()->getBodyZDistance();
	m_Range = val.getNamedLocation().getStationName().getRelativeRangeAndBearing()->getRange();
	m_Bearing = val.getNamedLocation().getStationName().getRelativeRangeAndBearing()->getBearing();
	m_StationNumber = val.getNamedLocation().getStationNumber();
}

void DevStudio::BaseEntityDt::setSpatial(DevStudio::SpatialVariantStruct val)
{
	switch (val.getDiscriminant()) {
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
		m_X = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getZ());
		m_Psi = val.getSpatialFPB()->getOrientation().getPsi();
		m_Theta = val.getSpatialFPB()->getOrientation().getTheta();
		m_Phi = val.getSpatialFPB()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialFPB()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialFPB()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialFPB()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialFPB()->getIsFrozen();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
		m_X = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getZ());
		m_Psi = val.getSpatialFPW()->getOrientation().getPsi();
		m_Theta = val.getSpatialFPW()->getOrientation().getTheta();
		m_Phi = val.getSpatialFPW()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialFPW()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialFPW()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialFPW()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialFPW()->getIsFrozen();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
		m_X = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getZ());
		m_Psi = val.getSpatialFVB()->getOrientation().getPsi();
		m_Theta = val.getSpatialFVB()->getOrientation().getTheta();
		m_Phi = val.getSpatialFVB()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialFVB()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialFVB()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialFVB()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialFVB()->getIsFrozen();
		m_AccelerationX = val.getSpatialFVB()->getAccelerationVector().getXAcceleration();
		m_AccelerationY = val.getSpatialFVB()->getAccelerationVector().getYAcceleration();
		m_AccelerationZ = val.getSpatialFVB()->getAccelerationVector().getZAcceleration();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
		m_X = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getZ());
		m_Psi = val.getSpatialFVW()->getOrientation().getPsi();
		m_Theta = val.getSpatialFVW()->getOrientation().getTheta();
		m_Phi = val.getSpatialFVW()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialFVW()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialFVW()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialFVW()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialFVW()->getIsFrozen();
		m_AccelerationX = val.getSpatialFVW()->getAccelerationVector().getXAcceleration();
		m_AccelerationY = val.getSpatialFVW()->getAccelerationVector().getYAcceleration();
		m_AccelerationZ = val.getSpatialFVW()->getAccelerationVector().getZAcceleration();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
		m_X = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getZ());
		m_Psi = val.getSpatialRPB()->getOrientation().getPsi();
		m_Theta = val.getSpatialRPB()->getOrientation().getTheta();
		m_Phi = val.getSpatialRPB()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialRPB()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialRPB()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialRPB()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialRPB()->getIsFrozen();
		m_AngularvelocityX = val.getSpatialRPB()->getAngularVelocity().getXAngularVelocity();
		m_AngularvelocityY = val.getSpatialRPB()->getAngularVelocity().getYAngularVelocity();
		m_AngularvelocityZ = val.getSpatialRPB()->getAngularVelocity().getZAngularVelocity();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
		m_X = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getZ());
		m_Psi = val.getSpatialRPW()->getOrientation().getPsi();
		m_Theta = val.getSpatialRPW()->getOrientation().getTheta();
		m_Phi = val.getSpatialRPW()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialRPW()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialRPW()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialRPW()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialRPW()->getIsFrozen();
		m_AngularvelocityX = val.getSpatialRPW()->getAngularVelocity().getXAngularVelocity();
		m_AngularvelocityY = val.getSpatialRPW()->getAngularVelocity().getYAngularVelocity();
		m_AngularvelocityZ = val.getSpatialRPW()->getAngularVelocity().getZAngularVelocity();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
		m_X = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getZ());
		m_Psi = val.getSpatialRVB()->getOrientation().getPsi();
		m_Theta = val.getSpatialRVB()->getOrientation().getTheta();
		m_Phi = val.getSpatialRVB()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialRVB()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialRVB()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialRVB()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialRVB()->getIsFrozen();
		m_AngularvelocityX = val.getSpatialRVB()->getAngularVelocity().getXAngularVelocity();
		m_AngularvelocityY = val.getSpatialRVB()->getAngularVelocity().getYAngularVelocity();
		m_AngularvelocityZ = val.getSpatialRVB()->getAngularVelocity().getZAngularVelocity();
		m_AccelerationX = val.getSpatialRVB()->getAccelerationVector().getXAcceleration();
		m_AccelerationY = val.getSpatialRVB()->getAccelerationVector().getYAcceleration();
		m_AccelerationZ = val.getSpatialRVB()->getAccelerationVector().getZAcceleration();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
		m_X = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getX());
		m_Y = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getY());
		m_Z = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getZ());
		m_Psi = val.getSpatialRVW()->getOrientation().getPsi();
		m_Theta = val.getSpatialRVW()->getOrientation().getTheta();
		m_Phi = val.getSpatialRVW()->getOrientation().getPhi();
		m_VelocityX = val.getSpatialRVW()->getVelocityVector().getXVelocity();
		m_VelocityY = val.getSpatialRVW()->getVelocityVector().getYVelocity();
		m_VelocityZ = val.getSpatialRVW()->getVelocityVector().getZVelocity();
		m_IsFrozen = val.getSpatialRVW()->getIsFrozen();
		m_AngularvelocityX = val.getSpatialRVW()->getAngularVelocity().getXAngularVelocity();
		m_AngularvelocityY = val.getSpatialRVW()->getAngularVelocity().getYAngularVelocity();
		m_AngularvelocityZ = val.getSpatialRVW()->getAngularVelocity().getZAngularVelocity();
		m_AccelerationX = val.getSpatialRVW()->getAccelerationVector().getXAcceleration();
		m_AccelerationY = val.getSpatialRVW()->getAccelerationVector().getYAcceleration();
		m_AccelerationZ = val.getSpatialRVW()->getAccelerationVector().getZAcceleration();
		convert_xyz_to_latlonalt();
		calculate(m_Lat, m_Lon, m_Psi, m_Theta, m_Phi, m_heading, m_Pitch, m_Roll);
		break;
	}
}

void DevStudio::BaseEntityDt::setRelativeSpatial(DevStudio::SpatialVariantStruct val)
{
	switch (val.getDiscriminant()) {
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
		m_RelativeX = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialFPB()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialFPB()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialFPB()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialFPB()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialFPB()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialFPB()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialFPB()->getIsFrozen();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
		m_RelativeX = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialFPW()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialFPW()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialFPW()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialFPW()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialFPW()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialFPW()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialFPW()->getIsFrozen();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
		m_RelativeX = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialFVB()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialFVB()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialFVB()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialFVB()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialFVB()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialFVB()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialFVB()->getIsFrozen();
		m_RelativeAccelerationX = val.getSpatialFVB()->getAccelerationVector().getXAcceleration();
		m_RelativeAccelerationY = val.getSpatialFVB()->getAccelerationVector().getYAcceleration();
		m_RelativeAccelerationZ = val.getSpatialFVB()->getAccelerationVector().getZAcceleration();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
		m_RelativeX = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialFVW()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialFVW()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialFVW()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialFVW()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialFVW()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialFVW()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialFVW()->getIsFrozen();
		m_RelativeAccelerationX = val.getSpatialFVW()->getAccelerationVector().getXAcceleration();
		m_RelativeAccelerationY = val.getSpatialFVW()->getAccelerationVector().getYAcceleration();
		m_RelativeAccelerationZ = val.getSpatialFVW()->getAccelerationVector().getZAcceleration();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
		m_RelativeX = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialRPB()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialRPB()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialRPB()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialRPB()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialRPB()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialRPB()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialRPB()->getIsFrozen();
		m_RelativeAngularvelocityX = val.getSpatialRPB()->getAngularVelocity().getXAngularVelocity();
		m_RelativeAngularvelocityY = val.getSpatialRPB()->getAngularVelocity().getYAngularVelocity();
		m_RelativeAngularvelocityZ = val.getSpatialRPB()->getAngularVelocity().getZAngularVelocity();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
		m_RelativeX = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialRPW()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialRPW()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialRPW()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialRPW()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialRPW()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialRPW()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialRPW()->getIsFrozen();
		m_RelativeAngularvelocityX = val.getSpatialRPW()->getAngularVelocity().getXAngularVelocity();
		m_RelativeAngularvelocityY = val.getSpatialRPW()->getAngularVelocity().getYAngularVelocity();
		m_RelativeAngularvelocityZ = val.getSpatialRPW()->getAngularVelocity().getZAngularVelocity();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
		m_RelativeX = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialRVB()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialRVB()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialRVB()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialRVB()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialRVB()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialRVB()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialRVB()->getIsFrozen();
		m_RelativeAngularvelocityX = val.getSpatialRVB()->getAngularVelocity().getXAngularVelocity();
		m_RelativeAngularvelocityY = val.getSpatialRVB()->getAngularVelocity().getYAngularVelocity();
		m_RelativeAngularvelocityZ = val.getSpatialRVB()->getAngularVelocity().getZAngularVelocity();
		m_RelativeAccelerationX = val.getSpatialRVB()->getAccelerationVector().getXAcceleration();
		m_RelativeAccelerationY = val.getSpatialRVB()->getAccelerationVector().getYAcceleration();
		m_RelativeAccelerationZ = val.getSpatialRVB()->getAccelerationVector().getZAcceleration();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
		m_RelativeX = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getX());
		m_RelativeY = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getY());
		m_RelativeZ = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getZ());
		m_RelativePsi = val.getSpatialRVW()->getOrientation().getPsi();
		m_RelativeTheta = val.getSpatialRVW()->getOrientation().getTheta();
		m_RelativePhi = val.getSpatialRVW()->getOrientation().getPhi();
		m_RelativeVelocityX = val.getSpatialRVW()->getVelocityVector().getXVelocity();
		m_RelativeVelocityY = val.getSpatialRVW()->getVelocityVector().getYVelocity();
		m_RelativeVelocityZ = val.getSpatialRVW()->getVelocityVector().getZVelocity();
		m_RelativeIsFrozen = val.getSpatialRVW()->getIsFrozen();
		m_RelativeAngularvelocityX = val.getSpatialRVW()->getAngularVelocity().getXAngularVelocity();
		m_RelativeAngularvelocityY = val.getSpatialRVW()->getAngularVelocity().getYAngularVelocity();
		m_RelativeAngularvelocityZ = val.getSpatialRVW()->getAngularVelocity().getZAngularVelocity();
		m_RelativeAccelerationX = val.getSpatialRVW()->getAccelerationVector().getXAcceleration();
		m_RelativeAccelerationY = val.getSpatialRVW()->getAccelerationVector().getYAcceleration();
		m_RelativeAccelerationZ = val.getSpatialRVW()->getAccelerationVector().getZAcceleration();
		Relativeconvert_xyz_to_latlonalt();
		Relativecalculate(m_RelativeLat, m_RelativeLon, m_RelativePsi, m_RelativeTheta, m_RelativePhi, m_Relativeheading, m_RelativePitch, m_RelativeRoll);
		break;
	}
}

int DevStudio::BaseEntityDt::getEntityKind() const
{
	return m_EntityKind;
}

int DevStudio::BaseEntityDt::getDomain() const
{
	return m_Domain;
}

int DevStudio::BaseEntityDt::getCountryCode() const
{
	return m_CountryCode;
}

int DevStudio::BaseEntityDt::getCategory() const
{
	return m_Category;
}

int DevStudio::BaseEntityDt::getSubcategory() const
{
	return m_Subcategory;
}

int DevStudio::BaseEntityDt::getSpecific() const
{
	return m_Specific;
}

int DevStudio::BaseEntityDt::getExtra() const
{
	return m_Extra;
}

int DevStudio::BaseEntityDt::getEntityNumber() const
{
	return m_EntityNumber;
}

int DevStudio::BaseEntityDt::getSiteID() const
{
	return m_SiteID;
}

int DevStudio::BaseEntityDt::getApplicationID() const
{
	return m_ApplicationID;
}

int DevStudio::BaseEntityDt::getSiteIdIsPartOf() const
{
	return m_SiteIdIspartOf;
}
int DevStudio::BaseEntityDt::getApplictionIdIsPartOf() const
{
	return m_ApplicationIdIspartOf;
}

int DevStudio::BaseEntityDt::getEntityNumIsPartOf() const
{
	return m_EntityNumIsPartOf;
}

std::string DevStudio::BaseEntityDt::getHostRTIObjectIdentifier() const
{
	return m_HostRTIObjectIdentifier;
}

int DevStudio::BaseEntityDt::getConstituentPartNature() const
{
	return m_ConstituentPartNature;
}

int DevStudio::BaseEntityDt::getConstituentPartPosition() const
{
	return m_ConstituentPartPosition;
}

int DevStudio::BaseEntityDt::getStationNumber() const
{
	return m_StationNumber;
}

int DevStudio::BaseEntityDt::getStationName() const
{
	return m_StationName;
}

float DevStudio::BaseEntityDt::getBodyXDistance() const
{
	return m_BodyXDistance;
}

float DevStudio::BaseEntityDt::getBodyYDistance() const
{
	return m_BodyYDistance;
}

float DevStudio::BaseEntityDt::getBodyZDistance() const
{
	return m_BodyZDistance;
}

float DevStudio::BaseEntityDt::getRange() const
{
	return m_Range;
}

float DevStudio::BaseEntityDt::getBearing() const
{
	return m_Bearing;
}

double DevStudio::BaseEntityDt::getVelocity() const
{
	return m_Velocity;
}

double DevStudio::BaseEntityDt::getLat() const
{
	return m_Lat;
}

double DevStudio::BaseEntityDt::getLon() const
{
	return m_Lon;
}

double DevStudio::BaseEntityDt::getAlt() const
{
	return m_Alt;
}

double DevStudio::BaseEntityDt::getPitch() const
{
	return m_Pitch;
}

double DevStudio::BaseEntityDt::getRoll() const
{
	return m_Roll;
}

double DevStudio::BaseEntityDt::getYaw() const
{
	return m_Yaw;
}

float DevStudio::BaseEntityDt::getPsi() const
{
	return m_Psi;
}

float DevStudio::BaseEntityDt::getTheta() const
{
	return m_Theta;
}

float DevStudio::BaseEntityDt::getPhi() const
{
	return m_Phi;
}

float DevStudio::BaseEntityDt::getX() const
{
	return m_X;
}

float DevStudio::BaseEntityDt::getY() const
{
	return m_Y;
}

float DevStudio::BaseEntityDt::getZ() const
{
	return m_Z;
}

bool DevStudio::BaseEntityDt::getIsFrozen() const
{
	return m_IsFrozen;
}

int DevStudio::BaseEntityDt::getSpatialVariantEnum() const
{
	return m_SpatialVariantEnum;
}

float DevStudio::BaseEntityDt::getVelocityX() const
{
	return m_VelocityX;
}

float DevStudio::BaseEntityDt::getVelocityY() const
{
	return m_VelocityY;
}

float DevStudio::BaseEntityDt::getVelocityZ() const
{
	return m_VelocityZ;
}

float DevStudio::BaseEntityDt::getAngularvelocityX() const
{
	return m_AngularvelocityX;
}

float DevStudio::BaseEntityDt::getAngularvelocityY() const
{
	return m_AngularvelocityY;
}

float DevStudio::BaseEntityDt::getAngularvelocityZ() const
{
	return m_AngularvelocityZ;
}

float DevStudio::BaseEntityDt::getAccelerationX() const
{
	return m_AccelerationX;
}

float DevStudio::BaseEntityDt::getAccelerationY() const
{
	return m_AccelerationY;
}

float DevStudio::BaseEntityDt::getAccelerationZ() const
{
	return m_AccelerationZ;
}

double DevStudio::BaseEntityDt::getHeading() const
{
	return m_heading;
}

double DevStudio::BaseEntityDt::getRelativeVelocity() const
{
	return m_RelativeVelocity;
}

double DevStudio::BaseEntityDt::getRelativeLat() const
{
	return m_RelativeLat;
}

double DevStudio::BaseEntityDt::getRelativeLon() const
{
	return m_RelativeLon;
}

double DevStudio::BaseEntityDt::getRelativeAlt() const
{
	return m_RelativeAlt;
}

double DevStudio::BaseEntityDt::getRelativePitch() const
{
	return m_RelativePitch;
}

double DevStudio::BaseEntityDt::getRelativeRoll() const
{
	return m_RelativeRoll;
}

double DevStudio::BaseEntityDt::getRelativeYaw() const
{
	return m_RelativeYaw;
}

float DevStudio::BaseEntityDt::getRelativePsi() const
{
	return m_RelativePsi;
}

float DevStudio::BaseEntityDt::getRelativeTheta() const
{
	return m_RelativeTheta;
}

float DevStudio::BaseEntityDt::getRelativePhi() const
{
	return m_RelativePhi;
}

float DevStudio::BaseEntityDt::getRelativeX() const
{
	return m_RelativeX;
}

float DevStudio::BaseEntityDt::getRelativeY() const
{
	return m_RelativeY;
}

float DevStudio::BaseEntityDt::getRelativeZ() const
{
	return m_RelativeZ;
}

bool DevStudio::BaseEntityDt::getRelativeIsFrozen() const
{
	return m_RelativeIsFrozen;
}

int DevStudio::BaseEntityDt::getRelativeSpatialVariantEnum() const
{
	return m_RelativeSpatialVariantEnum;
}

float DevStudio::BaseEntityDt::getRelativeVelocityX() const
{
	return m_RelativeVelocityX;
}

float DevStudio::BaseEntityDt::getRelativeVelocityY() const
{
	return m_RelativeVelocityY;
}

float DevStudio::BaseEntityDt::getRelativeVelocityZ() const
{
	return m_RelativeVelocityZ;
}

float DevStudio::BaseEntityDt::getRelativeAngularvelocityX() const
{
	return m_RelativeAngularvelocityX;
}

float DevStudio::BaseEntityDt::getRelativeAngularvelocityY() const
{
	return m_RelativeAngularvelocityY;
}

float DevStudio::BaseEntityDt::getRelativeAngularvelocityZ() const
{
	return m_RelativeAngularvelocityZ;
}

float DevStudio::BaseEntityDt::getRelativeAccelerationX() const
{
	return m_RelativeAccelerationX;
}

float DevStudio::BaseEntityDt::getRelativeAccelerationY() const
{
	return m_RelativeAccelerationY;
}

float DevStudio::BaseEntityDt::getRelativeAccelerationZ() const
{
	return m_RelativeAccelerationZ;
}

double DevStudio::BaseEntityDt::getRelativeHeading() const
{
	return m_Relativeheading;
}
