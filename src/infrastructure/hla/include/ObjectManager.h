#pragma once
#include <string>
#include <vector>
#include <map>
#include <DevStudio/HlaPointers.h>
#include <DevStudio/HlaPhysicalEntityImpl.h>
#include <DevStudio/HlaAircraftManagerImpl.h>
//#include <DevStudio/HlaTankerAircraftManagerImpl.h>
#include <DevStudio/HlaGroundVehicleManagerImpl.h>
#include <DevStudio/HlaSurfaceVesselManagerImpl.h>
#include <DevStudio/HlaAmphibiousVehicleManagerImpl.h>
#include <DevStudio/HlaMultiDomainPlatformManagerImpl.h>
#include <DevStudio/HlaSpacecraftManagerImpl.h>
#include <DevStudio/HlaSubmersibleVesselManagerImpl.h>
#include <DevStudio/HlaCulturalFeatureManagerImpl.h>
#include <DevStudio/HlaDecontaminationStationManagerImpl.h>
#include <DevStudio/HlaCOLPROManagerImpl.h>
#include <DevStudio/HlaMunitionManagerImpl.h>
#include <DevStudio/HlaRadioManagerImpl.h>
#include <DevStudio/HlaSuppliesManagerImpl.h>
#include <DevStudio/HlaExpendablesManagerImpl.h>
#include <DevStudio/HlaEmitterBeamManagerImpl.h>
#include <DevStudio/HlaEmitterSystemManagerImpl.h>
#include <DevStudio/HlaRadarBeamManagerImpl.h>
#include <DevStudio/HlaSensorManagerImpl.h>
#include <DevStudio/HlaJammerBeamManagerImpl.h>
#include <DevStudio/HlaAggregateEntityManagerImpl.h>
#include <DevStudio/HlaDesignatorManagerImpl.h>
#include "../Configuration.h"
#include <DevStudio/datatypes/AttributeValuePairStruct.h>
#include "DataTypes/ObjectIdentifier.h"
#include "DataTypes/AttributeValuePair.h"

namespace std
{
	template <typename  T>
	class optional;
}

struct CreatedEntityDataStruct
{
	std::shared_ptr<DevStudio::HlaPhysicalEntityAttributes> entity;
	DevStudio::HlaObjectInstanceBase::ObjectClassType type;
};

struct CreatedEmbeddedSystemDataStruct
{
	std::shared_ptr<DevStudio::HlaEmbeddedSystemAttributes> emitter;
	DevStudio::HlaObjectInstanceBase::ObjectClassType type;
};

namespace SDL_RTI
{
	class ObjectManager
	{
	public:
		ObjectManager(DevStudio::HlaWorldPtr world);

		// SEND OBJECTS
		void sendObject(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="objectIdentifier"></param>
		/// <param name="attributeVector"></param>
		/// <param name="lat">Latitude in degrees</param>
		/// <param name="lon">Longitude in degrees</param>
		/// <param name="alt">Altitude in m</param>
		/// <param name="phi">rad</param>
		/// <param name="theta">rad</param>
		/// <param name="psi">rad</param>
		/// <param name="speedX">m/s</param>
		/// <param name="speedY">m/s</param>
		/// <param name="speedZ">m/s</param>
		/// <param name="accelX">m/s^2</param>
		/// <param name="accelY">m/s^2</param>
		/// <param name="accelZ">m/s^2</param>
		/// <param name="angularVelocityX">rad/s</param>
		/// <param name="angularVelocityY">rad/s</param>
		/// <param name="angularVelocityZ">rad/s</param>
		void sendObject(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector, const double& lat, const double& lon, const double& alt, const double& phi, const double& theta, const double& psi, const double& speedX, const double& speedY, const double& speedZ, const double& accelX, const double& accelY, const double& accelZ, const double& angularVelocityX, const double& angularVelocityY, const double& angularVelocityZ);

		// REMOVE OBJECTS
		void removeObject(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeAircraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeGroundVehicle(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeSurfaceVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeSubmersibleVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeSpacecraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeEmitterBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeEmitterSystem(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeRadarBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeJammerBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeMunition(const unsigned short id);
		void removeMunition(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeExpendables(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeAggregateEntity(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeSensor(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);
		void removeDesignator(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier);

		// UPDATE
		void sendUpdatePosition
		(
			/*Id:				*/const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier,
			/*EntityType:		*/const DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum spatialDiscriminant,
			/*Pos:				*/const double lat, const double lon, const double alt,
			/*Orientation:		*/const double phi, const double theta, const double psi,
			/*Speed:			*/const float speedX, const float speedY, const float speedZ,
			/*Acceleration:		*/const float accelX, const float accelY, const float accelZ,
			/*AngularVelocity:	*/const float angularVelocityX, const float angularVelocityY, const float angularVelocityZ
		);
		void sendUpdateAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateAircraftAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateGroundVehicleAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateSurfaceVesselAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateSubmersibleVesselAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateSpacecraftAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateRadarBeamAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateJammerBeamAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateExpendablesAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendUpdateSensorAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector);

	private:
		//Meant to only be used by the generic method sendUpdatePosition
		DevStudio::SpatialVariantStruct getSpatialVariant(const DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum spatialDiscriminant, const double lat, const double lon, const double alt, const double phi, const double theta, const double psi, const float speedX, const float speedY,
			const float speedZ, const float accelX, const float accelY, const float accelZ, const float angularVelocityX, const float angularVelocityY, const float angularVelocityZ
		);

		void updateAttribute(void* updater, const std::vector<DataTypes::AttributeValuePair>& attributeVector);

		// SEND OBJECTS
		void sendAircraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendGroundVehicle(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendSurfaceVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendAmphibiousVehicle(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendMultiDomainPlatform(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendSpacecraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendSubmersibleVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendCulturalFeature(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendColPro(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendDecontaminationStation(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendMunition(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendExpendables(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendRadio(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendSupplies(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendSensor(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendEmitterSystem(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendRadarBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendJammerBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);
		void sendDesignator(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector);

	private:
		SDL_RTI::Configuration m_configuration;
		std::string m_EntityToPlace;
		std::map<unsigned short/*id*/, CreatedEntityDataStruct/*aircraft*/> m_CreatedIdsEntitiesMap;
		std::map<unsigned short/*id*/, CreatedEmbeddedSystemDataStruct/*emitter*/> m_CreatedIdsEmbeddedSystemsMap;

		std::shared_ptr<DevStudio::HlaAircraftManagerImpl> m_AircraftManager;
		//std::shared_ptr<DevStudio::HlaTankerAircraftManagerImpl> m_TankerAircraftManager;
		std::shared_ptr<DevStudio::HlaGroundVehicleManagerImpl> m_GroundVehicleManager;
		std::shared_ptr<DevStudio::HlaSurfaceVesselManagerImpl> m_SurfaceVesselManager;
		std::shared_ptr<DevStudio::HlaAmphibiousVehicleManagerImpl> m_AmphibiousVehicleManager;
		std::shared_ptr<DevStudio::HlaMultiDomainPlatformManagerImpl> m_MultiDomainPlatformManager;
		std::shared_ptr<DevStudio::HlaSpacecraftManagerImpl> m_SpacecraftManager;
		std::shared_ptr<DevStudio::HlaSubmersibleVesselManagerImpl> m_SubmersibleVesselManager;
		std::shared_ptr<DevStudio::HlaCulturalFeatureManagerImpl> m_CulturalFeatureManager;
		std::shared_ptr<DevStudio::HlaCOLPROManagerImpl> m_COLPROManager;
		std::shared_ptr<DevStudio::HlaDecontaminationStationManagerImpl> m_DecontaminationStationManager;
		std::shared_ptr<DevStudio::HlaMunitionManagerImpl> m_MunitionManager;
		std::shared_ptr<DevStudio::HlaRadioManagerImpl> m_RadioManager;
		std::shared_ptr<DevStudio::HlaSuppliesManagerImpl> m_SuppliesManager;
		std::shared_ptr<DevStudio::HlaExpendablesManagerImpl> m_ExpendablesManager;
		std::shared_ptr<DevStudio::HlaEmitterBeamManagerImpl> m_EmitterBeamManager;
		std::shared_ptr<DevStudio::HlaEmitterSystemManagerImpl> m_EmitterSystemManager;
		std::shared_ptr<DevStudio::HlaRadarBeamManagerImpl> m_RadarBeamManager;
		std::shared_ptr<DevStudio::HlaJammerBeamManagerImpl> m_JammerBeamManager;
		std::shared_ptr<DevStudio::HlaAggregateEntityManagerImpl> m_AggregateEntityManager;
		std::shared_ptr<DevStudio::HlaSensorManagerImpl> m_SensorManager;
		std::shared_ptr<DevStudio::HlaDesignatorManagerImpl> m_DesignatorManager;
		DevStudio::HlaWorldPtr m_World;
	};
}

