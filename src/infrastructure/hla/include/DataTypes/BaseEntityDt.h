#pragma once
#include <string>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>
#include <DevStudio/datatypes/FederateIdentifierStruct.h>
#include <DevStudio/datatypes/EntityTypeStruct.h>
#include <DevStudio/datatypes/IsPartOfStruct.h>
#include <DevStudio/datatypes/SpatialVariantStruct.h>
#include <DevStudio/datatypes/AccelerationVectorStruct.h>
#include <DevStudio/datatypes/VelocityVectorStruct.h>
#include <DevStudio/datatypes/AngularVelocityVectorStruct.h>
#include <DevStudio/datatypes/OrientationStruct.h>
#include <DevStudio/datatypes/NamedLocationStruct.h>
#include <DevStudio/datatypes/ConstituentPartRelationshipStruct.h>


namespace DevStudio
{
	class BaseEntityDt {
	public:
		BaseEntityDt();
		~BaseEntityDt();
		BaseEntityDt(const BaseEntityDt& obj);
		BaseEntityDt& operator=(const BaseEntityDt& obj);

		//Conversores
		void convert_xyz_to_latlonalt(void);
		void Relativeconvert_xyz_to_latlonalt(void);
		double meterToFeet(float altitudeMeter);
		void calculate(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll);
		void Relativecalculate(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll);

		void setEntityType(DevStudio::EntityTypeStruct val);
		void setEntityIdentifier(DevStudio::EntityIdentifierStruct val);
		void setIsPartOf(DevStudio::IsPartOfStruct val);
		void setSpatial(DevStudio::SpatialVariantStruct val);
		void setRelativeSpatial(DevStudio::SpatialVariantStruct val);


		int getEntityKind() const;
		int getDomain() const;
		int getCountryCode() const;
		int getCategory() const;
		int getSubcategory() const;
		int getSpecific() const;
		int getExtra() const;

		int getEntityNumber() const;
		int getSiteID() const;
		int getApplicationID() const;

		int getSiteIdIsPartOf() const;
		int getApplictionIdIsPartOf()const;

		int getEntityNumIsPartOf() const;
		
		
		std::string getHostRTIObjectIdentifier() const;
		int getConstituentPartNature() const;
		int getConstituentPartPosition() const;
		int getStationNumber() const;
		int getStationName() const;
		float getBodyXDistance() const;
		float getBodyYDistance() const;
		float getBodyZDistance() const;
		float getRange() const;
		float getBearing() const;

		double getVelocity() const;
		double getLat() const;
		double getLon() const;
		double getAlt() const;
		double getPitch() const;
		double getRoll() const;
		double getYaw() const;
		float getPsi() const;
		float getTheta() const;
		float getPhi() const;
		float getX() const;
		float getY() const;
		float getZ() const;
		bool getIsFrozen() const;
		int getSpatialVariantEnum() const;
		float getVelocityX() const;
		float getVelocityY() const;
		float getVelocityZ() const;
		float getAngularvelocityX() const;
		float getAngularvelocityY() const;
		float getAngularvelocityZ() const;
		float getAccelerationX() const;
		float getAccelerationY() const;
		float getAccelerationZ() const;
		double getHeading() const;

		double getRelativeVelocity() const;
		double getRelativeLat() const;
		double getRelativeLon() const;
		double getRelativeAlt() const;
		double getRelativePitch() const;
		double getRelativeRoll() const;
		double getRelativeYaw() const;
		float getRelativePsi() const;
		float getRelativeTheta() const;
		float getRelativePhi() const;
		float getRelativeX() const;
		float getRelativeY() const;
		float getRelativeZ() const;
		bool getRelativeIsFrozen() const;
		int getRelativeSpatialVariantEnum() const;
		float getRelativeVelocityX() const;
		float getRelativeVelocityY() const;
		float getRelativeVelocityZ() const;
		float getRelativeAngularvelocityX() const;
		float getRelativeAngularvelocityY() const;
		float getRelativeAngularvelocityZ() const;
		float getRelativeAccelerationX() const;
		float getRelativeAccelerationY() const;
		float getRelativeAccelerationZ() const;
		double getRelativeHeading() const;


	private:
		//EntityType
		int m_EntityKind;
		int m_Domain;
		int m_CountryCode;
		int m_Category;
		int m_Subcategory;
		int m_Specific;
		int m_Extra;
		//EntityIdentifier
		int m_EntityNumber;
		//FederateIdentifier
		int m_SiteID;
		int m_ApplicationID;
		//IsPartOf
		int m_SiteIdIspartOf;
		int m_ApplicationIdIspartOf;
		int m_EntityNumIsPartOf;
		std::string m_HostRTIObjectIdentifier;
		int m_ConstituentPartNature;
		int m_ConstituentPartPosition;
		int m_StationNumber;
		int m_StationName;
		float m_BodyXDistance;
		float m_BodyYDistance;
		float m_BodyZDistance;
		float m_Range;
		float m_Bearing;
		//Spatial
		double m_Velocity;
		double m_Lat;
		double m_Lon;
		double m_Alt;
		double m_Pitch;
		double m_Roll;
		double m_Yaw;
		float m_Psi;
		float m_Theta;
		float m_Phi;
		float m_X;
		float m_Y;
		float m_Z;
		bool  m_IsFrozen;
		int m_SpatialVariantEnum;
		float m_VelocityX;
		float m_VelocityY;
		float m_VelocityZ;
		float m_AngularvelocityX;
		float m_AngularvelocityY;
		float m_AngularvelocityZ;
		float m_AccelerationX;
		float m_AccelerationY;
		float m_AccelerationZ;
		double m_heading;
		//RelativeSpatial
		double m_RelativeVelocity;
		double m_RelativeLat;
		double m_RelativeLon;
		double m_RelativeAlt;
		double m_RelativePitch;
		double m_RelativeRoll;
		double m_RelativeYaw;
		float m_RelativePsi;
		float m_RelativeTheta;
		float m_RelativePhi;
		float m_RelativeX;
		float m_RelativeY;
		float m_RelativeZ;
		bool  m_RelativeIsFrozen;
		int m_RelativeSpatialVariantEnum;
		float m_RelativeVelocityX;
		float m_RelativeVelocityY;
		float m_RelativeVelocityZ;
		float m_RelativeAngularvelocityX;
		float m_RelativeAngularvelocityY;
		float m_RelativeAngularvelocityZ;
		float m_RelativeAccelerationX;
		float m_RelativeAccelerationY;
		float m_RelativeAccelerationZ;
		double m_Relativeheading;
		
	};
}