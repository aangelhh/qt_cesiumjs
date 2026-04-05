#include "ObjectManager.h"
//pragma region Entities Implementation Includes
#include <DevStudio/HlaAircraftImpl.h>
#include <DevStudio/HlaGroundVehicleImpl.h>
#include <DevStudio/HlaSurfaceVesselImpl.h>
#include <DevStudio/HlaAmphibiousVehicleImpl.h>
#include <DevStudio/HlaMultiDomainPlatformImpl.h>
#include <DevStudio/HlaSpacecraftImpl.h>
#include <DevStudio/HlaSubmersibleVesselImpl.h>
#include <DevStudio/HlaCulturalFeatureImpl.h>
#include <DevStudio/HlaCOLPROImpl.h>
#include <DevStudio/HlaDecontaminationStationImpl.h>
#include <DevStudio/HlaEmitterSystemImpl.h>
#include <DevStudio/HlaRadarBeamImpl.h>
#include <DevStudio/HlaJammerBeamImpl.h>
#include <DevStudio/HlaMunitionImpl.h>
#include <DevStudio/HlaSuppliesImpl.h>
#include <DevStudio/HlaRadioImpl.h>
#include <DevStudio/HlaExpendablesImpl.h>
#include <DevStudio/HlaAggregateEntityImpl.h>
#include <DevStudio/HlaSensorImpl.h>
#include <DevStudio/HlaDesignatorImpl.h>
//pragma endregion
//pragma region Entities Uptader Includes
#include <DevStudio/HlaBaseEntityUpdater.h>
#include <DevStudio/HlaORGRootUpdater.h>
#include <DevStudio/HlaLifeformUpdater.h>
#include <DevStudio/HlaHumanUpdater.h>
#include <DevStudio/HlaEnvironmentalEntityUpdater.h>
#include <DevStudio/HlaWeatherUpdater.h>
#include <DevStudio/HlaSubsurfaceLayerUpdater.h>
#include <DevStudio/HlaMinefieldUpdater.h>
#include <DevStudio/HlaEmbeddedSystemUpdater.h>
#include <DevStudio/HlaMinefieldDataUpdater.h>
#include <DevStudio/HlaRadioTransmitterUpdater.h>
#include <DevStudio/HlaRadioReceiverUpdater.h>
#include <DevStudio/HlaDesignatorUpdater.h>
#include <DevStudio/HlaEmitterSystemUpdater.h>
#include <DevStudio/HlaIFFUpdater.h>
#include <DevStudio/HlaNatoIFFUpdater.h>
#include <DevStudio/HlaNatoIFFInterrogatorUpdater.h>
#include <DevStudio/HlaNatoIFFTransponderUpdater.h>
#include <DevStudio/HlaSovietIFFUpdater.h>
#include <DevStudio/HlaSovietIFFInterrogatorUpdater.h>
#include <DevStudio/HlaSovietIFFTransponderUpdater.h>
#include <DevStudio/HlaUnderwaterAcousticsEmissionUpdater.h>
#include <DevStudio/HlaActiveSonarUpdater.h>
#include <DevStudio/HlaPropulsionNoiseUpdater.h>
#include <DevStudio/HlaAdditionalPassiveActivitiesUpdater.h>
#include <DevStudio/HlaActiveSonarBeamUpdater.h>
#include <DevStudio/HlaRadarBeamUpdater.h>
#include <DevStudio/HlaJammerBeamUpdater.h>
#include <DevStudio/HlaCommunicationNetworkUpdater.h>
#include <DevStudio/HlaDisruptionEffectUpdater.h>
#include <DevStudio/HlaEnvironmentObjectUpdater.h>
#include <DevStudio/HlaLinkStatesUpdater.h>
#include <DevStudio/HlaLinearObjectUpdater.h>
#include <DevStudio/HlaBreachableLinearObjectUpdater.h>
#include <DevStudio/HlaConnectionUpdater.h>
#include <DevStudio/HlaBreachObjectUpdater.h>
#include <DevStudio/HlaExhaustSmokeObjectUpdater.h>
#include <DevStudio/HlaMinefieldLaneMarkerObjectUpdater.h>
#include <DevStudio/HlaPointObjectUpdater.h>
#include <DevStudio/HlaBreachablePointObjectUpdater.h>
#include <DevStudio/HlaBurstPointObjectUpdater.h>
#include <DevStudio/HlaCraterObjectUpdater.h>
#include <DevStudio/HlaRibbonBridgeObjectUpdater.h>
#include <DevStudio/HlaArealObjectUpdater.h>
#include <DevStudio/HlaMinefieldObjectUpdater.h>
#include <DevStudio/HlaEnvironmentProcessUpdater.h>
#include <DevStudio/HlaGriddedDataUpdater.h>
#include <DevStudio/HlaLandSurfaceUpdater.h>
#include <DevStudio/HlaWaterSurfaceUpdater.h>
#include <DevStudio/HlaCBRNSensorUpdater.h>
#include <DevStudio/HlaTroposphereLayerUpdater.h>
#include <DevStudio/HlaCBRNDetectorUpdater.h>
//pragma endregion
#include <DataTypes/Converters.h>
#include <utils/MathHelpers.h>
#include <optional>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <DevStudio/HlaWorld.h>
#include <DevStudio/datatypes/Relation.h>

using namespace std;

namespace SDL_RTI
{
	ObjectManager::ObjectManager(DevStudio::HlaWorldPtr world) :
		m_AircraftManager(std::dynamic_pointer_cast<DevStudio::HlaAircraftManagerImpl>(world->getHlaAircraftManager())),
		//m_TankerAircraftManager(std::dynamic_pointer_cast<DevStudio::HlaTankerAircraftManagerImpl>(world->getHlaTankerAircraftManager())),
		m_GroundVehicleManager(std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleManagerImpl>(world->getHlaGroundVehicleManager())),
		m_SurfaceVesselManager(std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselManagerImpl>(world->getHlaSurfaceVesselManager())),
		m_AmphibiousVehicleManager(std::dynamic_pointer_cast<DevStudio::HlaAmphibiousVehicleManagerImpl>(world->getHlaAmphibiousVehicleManager())),
		m_MultiDomainPlatformManager(std::dynamic_pointer_cast<DevStudio::HlaMultiDomainPlatformManagerImpl>(world->getHlaMultiDomainPlatformManager())),
		m_SpacecraftManager(std::dynamic_pointer_cast<DevStudio::HlaSpacecraftManagerImpl>(world->getHlaSpacecraftManager())),
		m_SubmersibleVesselManager(std::dynamic_pointer_cast<DevStudio::HlaSubmersibleVesselManagerImpl>(world->getHlaSubmersibleVesselManager())),
		m_MunitionManager(std::dynamic_pointer_cast<DevStudio::HlaMunitionManagerImpl>(world->getHlaMunitionManager())),
		m_EmitterBeamManager(std::dynamic_pointer_cast<DevStudio::HlaEmitterBeamManagerImpl>(world->getHlaEmitterBeamManager())),
		m_EmitterSystemManager(std::dynamic_pointer_cast<DevStudio::HlaEmitterSystemManagerImpl>(world->getHlaEmitterSystemManager())),
		m_RadarBeamManager(std::dynamic_pointer_cast<DevStudio::HlaRadarBeamManagerImpl>(world->getHlaRadarBeamManager())),
		m_JammerBeamManager(std::dynamic_pointer_cast<DevStudio::HlaJammerBeamManagerImpl>(world->getHlaJammerBeamManager())),
		m_ExpendablesManager(std::dynamic_pointer_cast<DevStudio::HlaExpendablesManagerImpl>(world->getHlaExpendablesManager())),
		m_AggregateEntityManager(std::dynamic_pointer_cast<DevStudio::HlaAggregateEntityManagerImpl>(world->getHlaAggregateEntityManager())),
		m_SensorManager(std::dynamic_pointer_cast<DevStudio::HlaSensorManagerImpl>(world->getHlaSensorManager()))
	{
		m_configuration = SDL_RTI::Configuration::getInstance();
		m_World = world;
	}

	void ObjectManager::sendObject(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		switch (objectIdentifier.GetType())
		{
		case DevStudio::HlaObjectInstanceBase::SENSOR:
		{
			sendSensor(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::AIRCRAFT:
		{
			sendAircraft(objectIdentifier, attributeVector);
			break;
		}
		/*case DevStudio::HlaObjectInstanceBase::TANKER_AIRCRAFT:
		{
			sendTankerAircraft(objectIdentifier, attributeVector);
			break;
		}*/
		case DevStudio::HlaObjectInstanceBase::GROUND_VEHICLE:
		{
			sendGroundVehicle(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SURFACE_VESSEL:
		{
			sendSurfaceVessel(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::AMPHIBIOUS_VEHICLE:
		{
			sendAmphibiousVehicle(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::MULTI_DOMAIN_PLATFORM:
		{
			sendMultiDomainPlatform(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SPACECRAFT:
		{
			sendSpacecraft(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SUBMERSIBLE_VESSEL:
		{
			sendSubmersibleVessel(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::MUNITION:
		{
			sendMunition(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::EMITTER_SYSTEM:
		{
			sendEmitterSystem(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::JAMMER_BEAM:
		{
			sendEmitterSystem(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::RADAR_BEAM:
		{
			sendEmitterSystem(objectIdentifier, attributeVector);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::DESIGNATOR:
		{
			sendDesignator(objectIdentifier, attributeVector);
			break;
		}
		default:
		{
			break;
		}
		}
	}

	void ObjectManager::sendObject(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector, const double& lat, const double& lon, const double& alt, const double& phi, const double& theta, const double& psi, const double& speedX, const double& speedY, const double& speedZ, const double& accelX, const double& accelY, const double& accelZ, const double& angularVelocityX, const double& angularVelocityY, const double& angularVelocityZ)
	{
		std::cout << "sendObject.Start: " << std::endl;

		// We cant pass const variables to the function "calculate_spatial_discriminant", so we need to copy them to local variables
		double localSpeedX = speedX;
		double localSpeedY = speedY;
		double localSpeedZ = speedZ;
		double localAccelX = accelX;
		double localAccelY = accelY;
		double localAccelZ = accelZ;
		double localAngularVelocityX = angularVelocityX;
		double localAngularVelocityY = angularVelocityY;
		double localAngularVelocityZ = angularVelocityZ;

		auto spatialDiscriminant = SDL_RTI::Converters::calculate_spatial_discriminant(localSpeedX, localSpeedY, localSpeedZ, localAccelX, localAccelY, localAccelZ, localAngularVelocityX, localAngularVelocityY, localAngularVelocityZ);
		DevStudio::SpatialVariantStruct spatialVariant;
		spatialVariant = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);

		// Copia el vector original
		std::vector<DataTypes::AttributeValuePair> attributesWithSpatial = attributeVector;

		// Crea el nuevo AttributeValuePair para SPATIAL
		DataTypes::AttributeValuePair spatialPair(objectIdentifier.GetType(), "SPATIAL", &spatialVariant);

		// A�ade el nuevo par al vector
		attributesWithSpatial.push_back(spatialPair);

		sendObject(objectIdentifier, attributesWithSpatial);
	}

	void ObjectManager::sendAircraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_AircraftManager)
			return;

		auto aircraft = std::dynamic_pointer_cast<DevStudio::HlaAircraftImpl>(m_AircraftManager->createLocalHlaAircraft());
		auto updater = aircraft->getHlaAircraftUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		std::cout << "CREATE AIRCRAFT" << std::endl;
		auto identifier = aircraft->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::AIRCRAFT;
		entityData.entity = aircraft;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendGroundVehicle(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_GroundVehicleManager)
			return;

		auto groundVehicle = std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleImpl>(m_GroundVehicleManager->createLocalHlaGroundVehicle());
		auto updater = groundVehicle->getHlaGroundVehicleUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = groundVehicle->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::GROUND_VEHICLE;
		entityData.entity = groundVehicle;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendSurfaceVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_SurfaceVesselManager)
			return;

		auto surfaceVessel = std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselImpl>(m_SurfaceVesselManager->createLocalHlaSurfaceVessel());
		auto updater = surfaceVessel->getHlaSurfaceVesselUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = surfaceVessel->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::SURFACE_VESSEL;
		entityData.entity = surfaceVessel;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendAmphibiousVehicle(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_AmphibiousVehicleManager)
			return;

		auto amphibiousVehicle = std::dynamic_pointer_cast<DevStudio::HlaAmphibiousVehicleImpl>(m_AmphibiousVehicleManager->createLocalHlaAmphibiousVehicle());
		auto updater = amphibiousVehicle->getHlaAmphibiousVehicleUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = amphibiousVehicle->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::AMPHIBIOUS_VEHICLE;
		entityData.entity = amphibiousVehicle;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendMultiDomainPlatform(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_MultiDomainPlatformManager)
			return;

		auto multiDomainPlatform = std::dynamic_pointer_cast<DevStudio::HlaMultiDomainPlatformImpl>(m_MultiDomainPlatformManager->createLocalHlaMultiDomainPlatform());
		auto updater = multiDomainPlatform->getHlaMultiDomainPlatformUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = multiDomainPlatform->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::MULTI_DOMAIN_PLATFORM;
		entityData.entity = multiDomainPlatform;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendSpacecraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_SpacecraftManager)
			return;

		auto spacecraft = std::dynamic_pointer_cast<DevStudio::HlaSpacecraftImpl>(m_SpacecraftManager->createLocalHlaSpacecraft());
		auto updater = spacecraft->getHlaSpacecraftUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = spacecraft->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::SPACECRAFT;
		entityData.entity = spacecraft;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendSubmersibleVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_SubmersibleVesselManager)
			return;

		auto submersibleVessel = std::dynamic_pointer_cast<DevStudio::HlaSubmersibleVesselImpl>(m_SubmersibleVesselManager->createLocalHlaSubmersibleVessel());
		auto updater = submersibleVessel->getHlaSubmersibleVesselUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = submersibleVessel->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::SUBMERSIBLE_VESSEL;
		entityData.entity = submersibleVessel;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendCulturalFeature(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_CulturalFeatureManager)
			return;

		auto culturalFeature = std::dynamic_pointer_cast<DevStudio::HlaCulturalFeatureImpl>(m_CulturalFeatureManager->createLocalHlaCulturalFeature());
		auto updater = culturalFeature->getHlaCulturalFeatureUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = culturalFeature->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::CULTURAL_FEATURE;
		entityData.entity = culturalFeature;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendColPro(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_COLPROManager)
			return;

		auto colPro = std::dynamic_pointer_cast<DevStudio::HlaCOLPROImpl>(m_COLPROManager->createLocalHlaCOLPRO());
		auto updater = colPro->getHlaCOLPROUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = colPro->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::COLPRO;
		entityData.entity = colPro;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendDecontaminationStation(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_DecontaminationStationManager)
			return;

		auto decontaminationStation = std::dynamic_pointer_cast<DevStudio::HlaDecontaminationStationImpl>(m_DecontaminationStationManager->createLocalHlaDecontaminationStation());
		auto updater = decontaminationStation->getHlaDecontaminationStationUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = decontaminationStation->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::DECONTAMINATION_STATION;
		entityData.entity = decontaminationStation;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendMunition(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_MunitionManager)
			return;

		auto munition = std::dynamic_pointer_cast<DevStudio::HlaMunitionImpl>(m_MunitionManager->createLocalHlaMunition());
		auto updater = munition->getHlaMunitionUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = munition->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::MUNITION;
		entityData.entity = munition;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendExpendables(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_ExpendablesManager)
			return;

		auto expendables = std::dynamic_pointer_cast<DevStudio::HlaExpendablesImpl>(m_ExpendablesManager->createLocalHlaExpendables());
		auto updater = expendables->getHlaExpendablesUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = expendables->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::EXPENDABLES;
		entityData.entity = expendables;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendRadio(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_RadioManager)
			return;

		auto radio = std::dynamic_pointer_cast<DevStudio::HlaRadioImpl>(m_RadioManager->createLocalHlaRadio());
		auto updater = radio->getHlaRadioUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = radio->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::RADIO;
		entityData.entity = radio;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendSupplies(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_SuppliesManager)
			return;

		auto supplies = std::dynamic_pointer_cast<DevStudio::HlaSuppliesImpl>(m_SuppliesManager->createLocalHlaSupplies());
		auto updater = supplies->getHlaSuppliesUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = supplies->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::SUPPLIES;
		entityData.entity = supplies;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendSensor(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_SensorManager)
			return;

		auto sensor = std::dynamic_pointer_cast<DevStudio::HlaSensorImpl>(m_SensorManager->createLocalHlaSensor());
		auto updater = sensor->getHlaSensorUpdater();

		if (updater)
		{
			updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			updater->sendUpdate();
		}
		auto identifier = sensor->getEntityIdentifier();
		CreatedEntityDataStruct entityData;
		entityData.type = DevStudio::HlaObjectInstanceBase::ObjectClassType::SENSOR;
		entityData.entity = sensor;
		m_CreatedIdsEntitiesMap.insert({ identifier.entityNumber, entityData });
	}

	void ObjectManager::sendEmitterSystem(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_EmitterSystemManager)
			return;

		std::shared_ptr<DevStudio::HlaEmitterSystem> aircraft = m_EmitterSystemManager->createLocalHlaEmitterSystem();
		auto updater = std::dynamic_pointer_cast<DevStudio::HlaEmitterSystemImpl>(aircraft)->getHlaEmitterSystemUpdater();

		if (updater)
		{
			for (auto attribute : attributeVector)
			{
				auto attributeHandle = static_cast<DevStudio::HlaEmitterSystemAttributes::Attribute>(attribute.getAttribute());
				updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			}
			updater->sendUpdate();
		}
	}

	void ObjectManager::sendRadarBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_RadarBeamManager)
			return;

		std::shared_ptr<DevStudio::HlaRadarBeam> aircraft = m_RadarBeamManager->createLocalHlaRadarBeam();
		auto updater = std::dynamic_pointer_cast<DevStudio::HlaRadarBeamImpl>(aircraft)->getHlaRadarBeamUpdater();

		if (updater)
		{
			for (auto attribute : attributeVector)
			{
				auto attributeHandle = static_cast<DevStudio::HlaRadarBeamAttributes::Attribute>(attribute.getAttribute());
				updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			}
			updater->sendUpdate();
		}
	}

	void ObjectManager::sendJammerBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_JammerBeamManager)
			return;

		std::shared_ptr<DevStudio::HlaJammerBeam> aircraft = m_JammerBeamManager->createLocalHlaJammerBeam();
		auto updater = std::dynamic_pointer_cast<DevStudio::HlaJammerBeamImpl>(aircraft)->getHlaJammerBeamUpdater();

		if (updater)
		{
			for (auto attribute : attributeVector)
			{
				auto attributeHandle = static_cast<DevStudio::HlaJammerBeamAttributes::Attribute>(attribute.getAttribute());
				updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			}
			updater->sendUpdate();
		}
	}

	void ObjectManager::sendDesignator(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		if (!m_DesignatorManager)
			return;
		std::shared_ptr<DevStudio::HlaDesignator> aircraft = m_DesignatorManager->createLocalHlaDesignator();
		auto updater = std::dynamic_pointer_cast<DevStudio::HlaDesignatorImpl>(aircraft)->getHlaDesignatorUpdater();
		if (updater)
		{
			for (auto attribute : attributeVector)
			{
				auto attributeHandle = static_cast<DevStudio::HlaDesignatorAttributes::Attribute>(attribute.getAttribute());
				updateAttribute(static_cast<void*>(updater.get()), attributeVector);
			}
			updater->sendUpdate();
		}
	}

	void ObjectManager::removeObject(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		switch (objectIdentifier.GetType())
		{
		case DevStudio::HlaObjectInstanceBase::HUMAN:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::NON_HUMAN:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SENSOR:
		{
			removeSensor(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::CBRNDETECTOR:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::CBRNSENSOR:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::PLATFORM:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::AIRCRAFT:
		{
			removeAircraft(objectIdentifier);
			break;
		}
		/*case DevStudio::HlaObjectInstanceBase::TANKER_AIRCRAFT:
		{
			removeTankerAircraft(objectIdentifier);
			break;
		}*/
		case DevStudio::HlaObjectInstanceBase::GROUND_VEHICLE:
		{
			removeGroundVehicle(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SURFACE_VESSEL:
		{
			removeSurfaceVessel(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::AMPHIBIOUS_VEHICLE:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::MULTI_DOMAIN_PLATFORM:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SPACECRAFT:
		{
			removeSpacecraft(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SUBMERSIBLE_VESSEL:
		{
			removeSubmersibleVessel(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::CULTURAL_FEATURE:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::COLPRO:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::DECONTAMINATION_STATION:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::MUNITION:
		{
			removeMunition(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::EXPENDABLES:
		{
			removeExpendables(objectIdentifier);
			break;
		}
		case DevStudio::HlaObjectInstanceBase::RADIO:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::SUPPLIES:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::AGGREGATE_ENTITY:
		{
			break;
		}
		case DevStudio::HlaObjectInstanceBase::ENVIRONMENTAL_ENTITY:
		{
			break;
		}
		
		}
	}

	void ObjectManager::removeAircraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_AircraftManager)
			return;

		//Delete the aircraft from the HLA world
		auto aircraft = static_pointer_cast<DevStudio::HlaAircraft>(objectIdentifier.GetHlaObject(m_World));
		if (!aircraft)
		{
			std::cerr << "ERROR: ObjectManager::removeAircraft could not convert to Aircraft" << std::endl;
			return;
		}
		auto entityId = static_cast<DevStudio::EntityIdentifierStruct*>(objectIdentifier.GetObjectIdentifier());
		m_CreatedIdsEntitiesMap.erase(entityId->entityNumber);
		m_AircraftManager->deleteLocalHlaAircraft(aircraft);
	}

	void ObjectManager::removeGroundVehicle(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_GroundVehicleManager)
			return;

		//Delete the aircraft from the HLA world
		auto vehicle = static_pointer_cast<DevStudio::HlaGroundVehicle>(objectIdentifier.GetHlaObject(m_World));
		if (!vehicle)
		{
			std::cerr << "ERROR: ObjectManager::removeAircraft could not convert to Aircraft" << std::endl;
			return;
		}
		auto entityId = static_cast<DevStudio::EntityIdentifierStruct*>(objectIdentifier.GetObjectIdentifier());
		m_CreatedIdsEntitiesMap.erase(entityId->entityNumber);
		m_GroundVehicleManager->deleteLocalHlaGroundVehicle(vehicle);
	}

	void ObjectManager::removeSurfaceVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_SurfaceVesselManager)
			return;

		//Delete the aircraft from the HLA world
		auto vessel = static_pointer_cast<DevStudio::HlaSurfaceVessel>(objectIdentifier.GetHlaObject(m_World));
		if (!vessel)
		{
			std::cerr << "ERROR: ObjectManager::removeAircraft could not convert to Aircraft" << std::endl;
			return;
		}
		auto entityId = static_cast<DevStudio::EntityIdentifierStruct*>(objectIdentifier.GetObjectIdentifier());
		m_CreatedIdsEntitiesMap.erase(entityId->entityNumber);
		m_SurfaceVesselManager->deleteLocalHlaSurfaceVessel(vessel);
	}

	void ObjectManager::removeSubmersibleVessel(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_SubmersibleVesselManager)
			return;

		//Delete the aircraft from the HLA world
		auto vessel = static_pointer_cast<DevStudio::HlaSubmersibleVessel>(objectIdentifier.GetHlaObject(m_World));
		if (!vessel)
		{
			std::cerr << "ERROR: ObjectManager::removeAircraft could not convert to Aircraft" << std::endl;
			return;
		}
		auto entityId = static_cast<DevStudio::EntityIdentifierStruct*>(objectIdentifier.GetObjectIdentifier());
		m_CreatedIdsEntitiesMap.erase(entityId->entityNumber);
		m_SubmersibleVesselManager->deleteLocalHlaSubmersibleVessel(vessel);
	}

	void ObjectManager::removeSpacecraft(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_SpacecraftManager)
			return;

		//Delete the aircraft from the HLA world
		auto spacecraft = static_pointer_cast<DevStudio::HlaSpacecraft>(objectIdentifier.GetHlaObject(m_World));
		if (!spacecraft)
		{
			std::cerr << "ERROR: ObjectManager::removeAircraft could not convert to Aircraft" << std::endl;
			return;
		}
		auto entityId = static_cast<DevStudio::EntityIdentifierStruct*>(objectIdentifier.GetObjectIdentifier());
		m_CreatedIdsEntitiesMap.erase(entityId->entityNumber);
		m_SpacecraftManager->deleteLocalHlaSpacecraft(spacecraft);
	}

	void ObjectManager::removeEmitterBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_EmitterBeamManager)
			return;
		//Delete the emitter beam from the HLA world
		auto emitterBeam = static_pointer_cast<DevStudio::HlaEmitterBeam>(objectIdentifier.GetHlaObject(m_World));
		if (!emitterBeam)
		{
			std::cerr << "ERROR: ObjectManager::removeEmitterBeam could not convert to Emitter Beam" << std::endl;
			return;
		}
		m_EmitterBeamManager->deleteLocalHlaEmitterBeam(emitterBeam);
	}

	void ObjectManager::removeEmitterSystem(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_EmitterSystemManager)
			return;

		//Delete the aircraft from the HLA world
		auto emitter = static_pointer_cast<DevStudio::HlaEmitterSystem>(objectIdentifier.GetHlaObject(m_World));
		if (!emitter)
		{
			std::cerr << "ERROR: ObjectManager::removeEmitterSystem could not convert to Emitter System" << std::endl;
			return;
		}
		m_EmitterSystemManager->deleteLocalHlaEmitterSystem(emitter);
	}

	void ObjectManager::removeRadarBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_RadarBeamManager)
			return;

		//Delete the radar beam from the HLA world
		auto radarBeam = static_pointer_cast<DevStudio::HlaRadarBeam>(objectIdentifier.GetHlaObject(m_World));
		if (!radarBeam)
		{
			std::cerr << "ERROR: ObjectManager::removeRadarBeam could not convert to Radar Beam" << std::endl;
			return;
		}
		m_RadarBeamManager->deleteLocalHlaRadarBeam(radarBeam);
	}

	void ObjectManager::removeJammerBeam(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_JammerBeamManager)
			return;

		//Delete the jammer beam from the HLA world
		auto jammerBeam = static_pointer_cast<DevStudio::HlaJammerBeam>(objectIdentifier.GetHlaObject(m_World));
		if (!jammerBeam)
		{
			std::cerr << "ERROR: ObjectManager::removeJammerBeam could not convert to Jammer Beam" << std::endl;
			return;
		}
		m_JammerBeamManager->deleteLocalHlaJammerBeam(jammerBeam);
	}

	void ObjectManager::removeMunition(const unsigned short id)
	{
		if (!m_MunitionManager)
			return;

		auto physical = m_CreatedIdsEntitiesMap[id].entity;
		if (physical)
		{
			//Delete the munition from the HLA world
			auto munition = std::dynamic_pointer_cast<DevStudio::HlaMunitionImpl>(physical);
			if (!munition)
			{
				std::cerr << "ERROR: ObjectManager::removeMunition could not convert to Munition" << std::endl;
				return;
			}
			m_MunitionManager->deleteLocalHlaMunition(munition);
			//Remove the munition from the map
			m_CreatedIdsEntitiesMap.erase(id);
		}
	}

	void ObjectManager::removeMunition(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_MunitionManager)
			return;
		//Delete the munition from the HLA world
		auto munition = static_pointer_cast<DevStudio::HlaMunition>(objectIdentifier.GetHlaObject(m_World));
		if (!munition)
		{
			std::cerr << "ERROR: ObjectManager::removeMunition could not convert to Munition" << std::endl;
			return;
		}
		m_MunitionManager->deleteLocalHlaMunition(munition);
	}

	void ObjectManager::removeExpendables(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_ExpendablesManager)
			return;

		auto expendable = static_pointer_cast<DevStudio::HlaExpendables>(objectIdentifier.GetHlaObject(m_World));
		if (!expendable)
		{
			std::cerr << "ERROR: ObjectManager::removeExpendable could not convert to Expendable" << std::endl;
			return;
		}
		m_ExpendablesManager->deleteLocalHlaExpendables(expendable);
	}

	void ObjectManager::removeAggregateEntity(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_AggregateEntityManager)
			return;

		auto aggregateEntity = static_pointer_cast<DevStudio::HlaAggregateEntity>(objectIdentifier.GetHlaObject(m_World));
		if (!aggregateEntity)
		{
			std::cerr << "ERROR: ObjectManager::removeAggregateEntity could not convert to Aggregate Entity" << std::endl;
			return;
		}
		m_AggregateEntityManager->deleteLocalHlaAggregateEntity(aggregateEntity);
	}

	void ObjectManager::removeSensor(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_SensorManager)
			return;

		auto sensor = static_pointer_cast<DevStudio::HlaSensor>(objectIdentifier.GetHlaObject(m_World));
		if (!sensor)
		{
			std::cerr << "ERROR: ObjectManager::removeSensor could not convert to Sensor" << std::endl;
			return;
		}
		m_SensorManager->deleteLocalHlaSensor(sensor);
	}

	void ObjectManager::removeDesignator(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier)
	{
		if (!m_DesignatorManager)
			return;
		auto designator = static_pointer_cast<DevStudio::HlaDesignator>(objectIdentifier.GetHlaObject(m_World));
		if (!designator)
		{
			std::cerr << "ERROR: ObjectManager::removeDesignator could not convert to Designator" << std::endl;
			return;
		}
		m_DesignatorManager->deleteLocalHlaDesignator(designator);
	}

	void ObjectManager::sendUpdatePosition(const SDL_RTI::DataTypes::ObjectIdentifier& objectIdentifier, const DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum spatialDiscriminant, const double lat, const double lon, const double alt, const double phi, const double theta, const double psi, const float speedX, const float speedY, const float speedZ, const float accelX, const float accelY, const float accelZ, const float angularVelocityX, const float angularVelocityY, const float angularVelocityZ)
	{

		// We determine which type the entity is and call the helper of this function with the appropriate updater
		switch (objectIdentifier.GetType())
		{
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::AIRCRAFT:
		{
			auto aircraftPtr = std::static_pointer_cast<DevStudio::HlaAircraft>(objectIdentifier.GetHlaObject(m_World));
			auto aircraft = std::dynamic_pointer_cast<DevStudio::HlaAircraftImpl>(aircraftPtr);
			if (!aircraft)
			{
				std::cerr << "ERROR: ObjectManager::SendUpdatePosition could not convert to Aircraft" << std::endl;
				return;
			}
			auto updater = aircraft->getHlaAircraftUpdater();
			if (updater)
			{
				DevStudio::SpatialVariantStruct spatial = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);
				updater->setSpatial(spatial);
				updater->sendUpdate();
			}
			break;
		}
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::PHYSICAL_ENTITY:
		{
			auto physicalPtr = std::static_pointer_cast<DevStudio::HlaPhysicalEntity>(objectIdentifier.GetHlaObject(m_World));
			auto physical = std::dynamic_pointer_cast<DevStudio::HlaPhysicalEntityImpl>(physicalPtr);
			if (!physical)
			{
				std::cerr << "ERROR: ObjectManager::SendUpdatePosition could not convert to Physical" << std::endl;
				return;
			}
			auto updater = physical->getHlaPhysicalEntityUpdater();
			if (updater)
			{
				DevStudio::SpatialVariantStruct spatial = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);
				updater->setSpatial(spatial);
				updater->sendUpdate();
			}
			break;
		}
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::MUNITION:
		{
			auto munitionPtr = std::static_pointer_cast<DevStudio::HlaMunition>(objectIdentifier.GetHlaObject(m_World));
			auto munition = std::dynamic_pointer_cast<DevStudio::HlaMunitionImpl>(munitionPtr);
			if (!munition)
			{
				std::cerr << "ERROR: ObjectManager::SendUpdatePosition could not convert to Munition" << std::endl;
				return;
			}			
			auto updater = munition->getHlaMunitionUpdater();
			if (updater)
			{
				DevStudio::SpatialVariantStruct spatial = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);
				updater->setSpatial(spatial);
				updater->sendUpdate();
			}
			break;
		}
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::GROUND_VEHICLE:
		{
			auto groundPtr = std::static_pointer_cast<DevStudio::HlaGroundVehicle>(objectIdentifier.GetHlaObject(m_World));
			auto ground = std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleImpl>(groundPtr);
			if (!ground)
			{
				std::cerr << "ERROR: ObjectManager::SendUpdatePosition could not convert to GroundVehicle" << std::endl;
				return;
			}
			auto updater = ground->getHlaGroundVehicleUpdater();
			if (updater)
			{
				DevStudio::SpatialVariantStruct spatial = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);
				updater->setSpatial(spatial);
				updater->sendUpdate();
			}
			break;
		}
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::SURFACE_VESSEL:
		{
			auto surfaceVesselPtr = std::static_pointer_cast<DevStudio::HlaSurfaceVessel>(objectIdentifier.GetHlaObject(m_World));
			auto surfaceVessel = std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselImpl>(surfaceVesselPtr);
			if (!surfaceVessel)
			{
				std::cerr << "ERROR: ObjectManager::SendUpdatePosition could not convert to SurfaceVessel" << std::endl;
				return;
			}
			auto updater = surfaceVessel->getHlaSurfaceVesselUpdater();
			if (updater)
			{
				DevStudio::SpatialVariantStruct spatial = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);
				updater->setSpatial(spatial);
				updater->sendUpdate();
			}
			break;
		}
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::AMPHIBIOUS_VEHICLE:
		{
			auto amphibiousVehiclePtr = std::static_pointer_cast<DevStudio::HlaAmphibiousVehicle>(objectIdentifier.GetHlaObject(m_World));
			auto amphibiousVehicle = std::dynamic_pointer_cast<DevStudio::HlaAmphibiousVehicleImpl>(amphibiousVehiclePtr);
			if (!amphibiousVehicle)
			{
				std::cerr << "ERROR: ObjectManager::SendUpdatePosition could not convert to AmphibiousVehicle" << std::endl;
				return;
			}
			auto updater = amphibiousVehicle->getHlaAmphibiousVehicleUpdater();
			if (updater)
			{
				DevStudio::SpatialVariantStruct spatial = getSpatialVariant(spatialDiscriminant, lat, lon, alt, phi, theta, psi, speedX, speedY, speedZ, accelX, accelY, accelZ, angularVelocityX, angularVelocityY, angularVelocityZ);
				updater->setSpatial(spatial);
				updater->sendUpdate();
			}
			break;
		}
		default:
		{
			std::cerr << "Error: ObjectManager::sendUpdatePosition invalid type" << std::endl;
			return;
		}
		}
	}

	void ObjectManager::sendUpdateAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		switch (objectId.GetType())
		{
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::AIRCRAFT:
			sendUpdateAircraftAttribute(objectId, attributeVector);
			break;
		/*case DevStudio::HlaObjectInstanceBase::ObjectClassType::TANKER_AIRCRAFT:
			sendUpdateTankerAircraftAttribute(objectId, attributeVector);
			break;*/
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::GROUND_VEHICLE:
			sendUpdateGroundVehicleAttribute(objectId, attributeVector);
			break;
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::SURFACE_VESSEL:
			sendUpdateSurfaceVesselAttribute(objectId, attributeVector);
			break;
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::SUBMERSIBLE_VESSEL:
			sendUpdateSubmersibleVesselAttribute(objectId, attributeVector);
			break;
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::SPACECRAFT:
			sendUpdateSpacecraftAttribute(objectId, attributeVector);
			break;
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::RADAR_BEAM:
			sendUpdateRadarBeamAttribute(objectId, attributeVector);
			break;
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::JAMMER_BEAM:
			sendUpdateJammerBeamAttribute(objectId, attributeVector);
			break;
		case DevStudio::HlaObjectInstanceBase::ObjectClassType::SENSOR:
			sendUpdateSensorAttribute(objectId, attributeVector);
			break;
		default:
			break;
		}
	}

	void ObjectManager::sendUpdateAircraftAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto aircraftPtr = std::static_pointer_cast<DevStudio::HlaAircraft>(objectId.GetHlaObject(m_World));
		auto aircraft = std::dynamic_pointer_cast<DevStudio::HlaAircraftImpl>(aircraftPtr);
		if (!aircraft)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to Aircraft" << std::endl;
			return;
		}
		auto updater = aircraft->getHlaAircraftUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}

	void ObjectManager::sendUpdateGroundVehicleAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto vehiclePtr = std::static_pointer_cast<DevStudio::HlaGroundVehicle>(objectId.GetHlaObject(m_World));
		auto vehicle = std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleImpl>(vehiclePtr);
		if (!vehicle)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to GroundVehicle" << std::endl;
			return;
		}
		auto updater = vehicle->getHlaGroundVehicleUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}

	void ObjectManager::sendUpdateSurfaceVesselAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto vesselPtr = std::static_pointer_cast<DevStudio::HlaSurfaceVessel>(objectId.GetHlaObject(m_World));
		auto vessel = std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselImpl>(vesselPtr);
		if (!vessel)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to SurfaceVessel" << std::endl;
			return;
		}
		auto updater = vessel->getHlaSurfaceVesselUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}

	void ObjectManager::sendUpdateSubmersibleVesselAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto vesselPtr = std::static_pointer_cast<DevStudio::HlaSubmersibleVessel>(objectId.GetHlaObject(m_World));
		auto vessel = std::dynamic_pointer_cast<DevStudio::HlaSubmersibleVesselImpl>(vesselPtr);
		if (!vessel)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to SubmersibleVessel" << std::endl;
			return;
		}
		auto updater = vessel->getHlaSubmersibleVesselUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}

	void ObjectManager::sendUpdateSpacecraftAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto spacecraftPtr = std::static_pointer_cast<DevStudio::HlaSpacecraft>(objectId.GetHlaObject(m_World));
		auto spacecraft = std::dynamic_pointer_cast<DevStudio::HlaSpacecraftImpl>(spacecraftPtr);
		if (!spacecraft)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to Spacecraft" << std::endl;
			return;
		}
		auto updater = spacecraft->getHlaSpacecraftUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}

	void ObjectManager::sendUpdateRadarBeamAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto objectPtr = std::static_pointer_cast<DevStudio::HlaRadarBeam>(objectId.GetHlaObject(m_World));
		auto objectImpl = std::dynamic_pointer_cast<DevStudio::HlaRadarBeamImpl>(objectPtr);
		if (!objectImpl)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to RadarBeam" << std::endl;
			return;
		}
		auto updater = objectImpl->getHlaRadarBeamUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}
	void ObjectManager::sendUpdateJammerBeamAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto objectPtr = std::static_pointer_cast<DevStudio::HlaJammerBeam>(objectId.GetHlaObject(m_World));
		auto objectImpl = std::dynamic_pointer_cast<DevStudio::HlaJammerBeamImpl>(objectPtr);
		if (!objectImpl)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to JammerBeam" << std::endl;
			return;
		}
		auto updater = objectImpl->getHlaJammerBeamUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}
	void ObjectManager::sendUpdateExpendablesAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto objectPtr = std::static_pointer_cast<DevStudio::HlaExpendables>(objectId.GetHlaObject(m_World));
		auto objectImpl = std::dynamic_pointer_cast<DevStudio::HlaExpendablesImpl>(objectPtr);
		if (!objectImpl)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to Expendables" << std::endl;
			return;
		}
		auto updater = objectImpl->getHlaExpendablesUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}
	void ObjectManager::sendUpdateSensorAttribute(const SDL_RTI::DataTypes::ObjectIdentifier& objectId, const std::vector<DataTypes::AttributeValuePair>& attributeVector)
	{
		auto objectPtr = std::static_pointer_cast<DevStudio::HlaSensor>(objectId.GetHlaObject(m_World));
		auto objectImpl = std::dynamic_pointer_cast<DevStudio::HlaSensorImpl>(objectPtr);
		if (!objectImpl)
		{
			std::cerr << "ERROR: ObjectManager::SendUpdateAttribute could not convert to Sensor" << std::endl;
			return;
		}
		auto updater = objectImpl->getHlaSensorUpdater();
		updateAttribute(static_cast<void*>(updater.get()), attributeVector);
		updater->sendUpdate();
	}

	DevStudio::SpatialVariantStruct ObjectManager::getSpatialVariant(const DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum spatialDiscriminant, const double lat, const double lon, const double alt, const double phi, const double theta, const double psi, const float speedX, const float speedY, const float speedZ, const float accelX, const float accelY, const float accelZ, const float angularVelocityX, const float angularVelocityY, const float angularVelocityZ)
	{
		double x = 0, y = 0, z = 0;
		SDL_RTI::geoCoordsToXYZ(lat, lon, alt, x, y, z);
		DevStudio::SpatialVariantStruct spVariant;

		switch (spatialDiscriminant)
		{
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
		{
			DevStudio::SpatialStaticStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spVariant.setSpatialStatic(spatialData);
			return DevStudio::SpatialVariantStruct::createSpatialStatic(spatialData);
			break;
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
		{
			DevStudio::SpatialFPStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			return DevStudio::SpatialVariantStruct::createSpatialFPW(spatialData);
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
		{
			DevStudio::SpatialRPStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spatialData.angularVelocity.xAngularVelocity = angularVelocityX;
			spatialData.angularVelocity.yAngularVelocity = angularVelocityY;
			spatialData.angularVelocity.zAngularVelocity = angularVelocityZ;

			return DevStudio::SpatialVariantStruct::createSpatialRPW(spatialData);
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
		{
			DevStudio::SpatialRVStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spatialData.accelerationVector.xAcceleration = accelX;
			spatialData.accelerationVector.yAcceleration = accelY;
			spatialData.accelerationVector.zAcceleration = accelZ;

			spatialData.angularVelocity.xAngularVelocity = angularVelocityX;
			spatialData.angularVelocity.yAngularVelocity = angularVelocityY;
			spatialData.angularVelocity.zAngularVelocity = angularVelocityZ;

			return DevStudio::SpatialVariantStruct::createSpatialRVW(spatialData);
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
		{
			DevStudio::SpatialFVStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spatialData.accelerationVector.xAcceleration = accelX;
			spatialData.accelerationVector.yAcceleration = accelY;
			spatialData.accelerationVector.zAcceleration = accelZ;

			return DevStudio::SpatialVariantStruct::createSpatialFVW(spatialData);
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
		{
			DevStudio::SpatialFPStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			return DevStudio::SpatialVariantStruct::createSpatialFPB(spatialData);
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
		{
			DevStudio::SpatialRPStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spatialData.angularVelocity.xAngularVelocity = angularVelocityX;
			spatialData.angularVelocity.yAngularVelocity = angularVelocityY;
			spatialData.angularVelocity.zAngularVelocity = angularVelocityZ;
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
		{
			DevStudio::SpatialRVStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spatialData.accelerationVector.xAcceleration = accelX;
			spatialData.accelerationVector.yAcceleration = accelY;
			spatialData.accelerationVector.zAcceleration = accelZ;

			spatialData.angularVelocity.xAngularVelocity = angularVelocityX;
			spatialData.angularVelocity.yAngularVelocity = angularVelocityY;
			spatialData.angularVelocity.zAngularVelocity = angularVelocityZ;

			return DevStudio::SpatialVariantStruct::createSpatialRVB(spatialData);
		}

		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
		{
			DevStudio::SpatialFVStruct spatialData;

			spatialData.worldLocation = DevStudio::WorldLocationStruct{ x, y, z };

			spatialData.velocityVector.xVelocity = speedX;
			spatialData.velocityVector.yVelocity = speedY;
			spatialData.velocityVector.zVelocity = speedZ;

			spatialData.orientation.phi = phi;
			spatialData.orientation.theta = theta;
			spatialData.orientation.psi = psi;

			spatialData.accelerationVector.xAcceleration = accelX;
			spatialData.accelerationVector.yAcceleration = accelY;
			spatialData.accelerationVector.zAcceleration = accelZ;

			return DevStudio::SpatialVariantStruct::createSpatialFVB(spatialData);
		}

		default:
		{
			std::cerr << "Error: SpatialType no soportado para cambio de velocidad." << std::endl;
			return spVariant;
		}
		}
	}
	
	void ObjectManager::updateAttribute(void* updater, const std::vector<DataTypes::AttributeValuePair>& attributeVectorConst)
	{
		if (updater == nullptr)
		{
			std::cerr << "ERROR: ObjectManager::updateAttribute - Updater is null." << std::endl;
			return;
		}

		static const std::unordered_map<std::string, std::function<void(void*, DataTypes::AttributeValuePair)>> acciones = 
		{
//pragma region BASE ENTITY ATTRIBUTES
			{ "ENTITY_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityIdentifierStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBaseEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEntityIdentifier(value);
				else
					std::cerr << "Error updating ENTITY_IDENTIFIER: updater is not HlaBaseEntityUpdater";
			} },
			{ "IS_PART_OF", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IsPartOfStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBaseEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setIsPartOf(value);
				else
					std::cerr << "Error updating IS_PART_OF: updater is not HlaBaseEntityUpdater";
			} },
			{ "SPATIAL", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SpatialVariantStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBaseEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSpatial(value);
				else
					std::cerr << "Error updating SPATIAL: updater is not HlaBaseEntityUpdater";
			} },
			{ "RELATIVE_SPATIAL", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SpatialVariantStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBaseEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRelativeSpatial(value);
				else
					std::cerr << "Error updating RELATIVE_SPATIAL: updater is not HlaBaseEntityUpdater";
			} },
			{"ENTITY_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBaseEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEntityType(value);
				else
					std::cerr << "Error updating ENTITY_TYPE: updater is not HlaBaseEntityUpdater";
			}},
//pragma endregion
//pragma region PHYSICAL ENTITY ATTRIBUTES
			{ "ACOUSTIC_SIGNATURE_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAcousticSignatureIndex(value);
				else
					std::cerr << "Error updating ACOUSTIC_SIGNATURE_INDEX: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "ALTERNATE_ENTITY_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAlternateEntityType(value);
				else
					std::cerr << "Error updating ALTERNATE_ENTITY_TYPE: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "ARTICULATED_PARAMETERS_ARRAY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::ArticulatedParameterStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setArticulatedParametersArray(value);
				else
					std::cerr << "Error updating ARTICULATED_PARAMETERS_ARRAY: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "CAMOUFLAGE_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CamouflageEnum::CamouflageEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCamouflageType(value);
				else
					std::cerr << "Error updating CAMOUFLAGE_TYPE: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "DAMAGE_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater)) {
					auto value = *static_cast<DevStudio::DamageStatusEnum::DamageStatusEnum*>(attribute.getData());
					castUpdater->setDamageState(value);
				}
				else
					std::cerr << "Error updating DAMAGE_STATE: updater is not HlaPhysicalEntityUpdater or HlaEnvironmentObjectUpdater";
			} },
			{ "ENGINE_SMOKE_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEngineSmokeOn(value);
				else
					std::cerr << "Error updating ENGINE_SMOKE_ON: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "FIRE_POWER_DISABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFirePowerDisabled(value);
				else
					std::cerr << "Error updating FIRE_POWER_DISABLED: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "FLAMES_PRESENT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFlamesPresent(value);
				else
					std::cerr << "Error updating FLAMES_PRESENT: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "HAS_AMMUNITION_SUPPLY_CAP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHasAmmunitionSupplyCap(value);
				else
					std::cerr << "Error updating HAS_AMMUNITION_SUPPLY_CAP: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "HAS_FUEL_SUPPLY_CAP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHasFuelSupplyCap(value);
				else
					std::cerr << "Error updating HAS_FUEL_SUPPLY_CAP: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "HAS_RECOVERY_CAP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHasRecoveryCap(value);
				else
					std::cerr << "Error updating HAS_RECOVERY_CAP: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "HAS_REPAIR_CAP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHasRepairCap(value);
				else
					std::cerr << "Error updating HAS_REPAIR_CAP: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "IMMOBILIZED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setImmobilized(value);
				else
					std::cerr << "Error updating IMMOBILIZED: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "INFRARED_SIGNATURE_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setInfraredSignatureIndex(value);
				else
					std::cerr << "Error updating INFRARED_SIGNATURE_INDEX: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "IS_CONCEALED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setIsConcealed(value);
				else
					std::cerr << "Error updating IS_CONCEALED: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "LIVE_ENTITY_MEASURED_SPEED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLiveEntityMeasuredSpeed(value);
				else
					std::cerr << "Error updating LIVE_ENTITY_MEASURED_SPEED: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "MARKING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::MarkingStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMarking(value);
				else
					std::cerr << "Error updating MARKING: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "POWER_PLANT_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPowerPlantOn(value);
				else
					std::cerr << "Error updating POWER_PLANT_ON: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "PROPULSION_SYSTEMS_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::PropulsionSystemDataStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPropulsionSystemsData(value);
				else
					std::cerr << "Error updating PROPULSION_SYSTEMS_DATA: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "RADAR_CROSS_SECTION_SIGNATURE_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRadarCrossSectionSignatureIndex(value);
				else
					std::cerr << "Error updating RADAR_CROSS_SECTION_SIGNATURE_INDEX: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "SMOKE_PLUME_PRESENT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSmokePlumePresent(value);
				else
					std::cerr << "Error updating SMOKE_PLUME_PRESENT: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "TENT_DEPLOYED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTentDeployed(value);
				else
					std::cerr << "Error updating TENT_DEPLOYED: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "TRAILING_EFFECTS_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::TrailingEffectsCodeEnum::TrailingEffectsCodeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTrailingEffectsCode(value);
				else
					std::cerr << "Error updating TRAILING_EFFECTS_CODE: updater is not HlaPhysicalEntityUpdater";
			} },
			{ "VECTORING_NOZZLE_SYSTEM_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::VectoringNozzleSystemDataStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setVectoringNozzleSystemData(value);
				else
					std::cerr << "Error updating VECTORING_NOZZLE_SYSTEM_DATA: updater is not HlaPhysicalEntityUpdater";
			} },
			{"FORCE_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::ForceIdentifierEnum::ForceIdentifierEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPhysicalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setForceIdentifier(value);
				else
					std::cerr << "Error updating FORCE_IDENTIFIER: updater is not HlaPhysicalEntityUpdater";
			}},
//pragma endregion
//pragma region LIFEFORM ATTRIBUTES
			{ "FLASH_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLifeformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFlashLightsOn(value);
				else
					std::cerr << "Error updating FLASH_LIGHTS_ON: updater is not HlaLifeformUpdater";
			} },
			{ "STANCE_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::StanceCodeEnum::StanceCodeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLifeformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setStanceCode(value);
				else
					std::cerr << "Error updating STANCE_CODE: updater is not HlaLifeformUpdater";
			} },
			{ "PRIMARY_WEAPON_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WeaponStateEnum::WeaponStateEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLifeformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPrimaryWeaponState(value);
				else
					std::cerr << "Error updating PRIMARY_WEAPON_STATE: updater is not HlaLifeformUpdater";
			} },
			{ "SECONDARY_WEAPON_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WeaponStateEnum::WeaponStateEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLifeformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSecondaryWeaponState(value);
				else
					std::cerr << "Error updating SECONDARY_WEAPON_STATE: updater is not HlaLifeformUpdater";
			} },
			{ "COMPLIANCE_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::ComplianceStateEnum::ComplianceStateEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLifeformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setComplianceState(value);
				else
					std::cerr << "Error updating COMPLIANCE_STATE: updater is not HlaLifeformUpdater";
			} },
//pragma endregion
//pragma region SENSOR ATTRIBUTES
			{ "ANTENNA_RAISED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAntennaRaised(value);
				else
					std::cerr << "Error updating ANTENNA_RAISED: updater is not HlaSensorUpdater";
			} },
			{ "BLACKOUT_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBlackoutLightsOn(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBlackOutLightsOn(value);
				else
					std::cerr << "Error updating BLACKOUT_LIGHTS_ON: updater is not HlaSensorUpdater or HlaPlatformUpdater";
			} },
			{ "LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLightsOn(value);
				else
					std::cerr << "Error updating LIGHTS_ON: updater is not HlaSensorUpdater";
			} },
			{ "INTERIOR_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setInteriorLightsOn(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setInteriorLightsOn(value);
				else
					std::cerr << "Error updating INTERIOR_LIGHTS_ON: updater is not HlaSensorUpdater";
			} },
			{ "MISSION_KILL", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMissionKill(value);
				else
					std::cerr << "Error updating MISSION_KILL: updater is not HlaSensorUpdater";
			} },
//pragma endregion
//pragma region CBRN_DETECTOR ATTRIBUTES
			{ "PROCESSING_TIME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNDetectorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setProcessingTime(value);
				else
					std::cerr << "Error updating PROCESSING_TIME, updater is not HlaCBRNDetectorUpdater";
			} },
			{ "AVERAGING_TIME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNDetectorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAveragingTime(value);
				else
					std::cerr << "Error updating AVERAGING_TIME, updater is not HlaCBRNDetectorUpdater";
			} },
			{ "DETECTABLE_AGENTS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::AgentConcentrationStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNDetectorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDetectableAgents(value);
				else
					std::cerr << "Error updating DETECTABLE_AGENTS, updater is not HlaCBRNDetectorUpdater";
			} },
			{ "ALARM", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CBRNAlarmStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNDetectorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAlarm(value);
				else
					std::cerr << "Error updating ALARM, updater is not HlaCBRNDetectorUpdater";
			} },
//pragma endregion
//pragma region CBRN_SENSOR ATTRIBUTES
			{ "UPDATE_FREQUENCY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setUpdateFrequency(value);
				else
					std::cerr << "Error updating UPDATE_FREQUENCY, updater is not HlaCBRNSensorUpdater";
			} },
			{ "AVERAGING_TIME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAveragingTime(value);
				else
					std::cerr << "Error updating AVERAGING_TIME, updater is not HlaCBRNSensorUpdater";
			} },
			{ "DETECTABLE_AGENTS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::AgentTypeEnum::AgentTypeEnum>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDetectableAgents(value);
				else
					std::cerr << "Error updating DETECTABLE_AGENTS, updater is not HlaCBRNSensorUpdater";
			} },
			{ "SENSOR_READINGS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::CBRNSensorReadingStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCBRNSensorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSensorReadings(value);
				else
					std::cerr << "Error updating SENSOR_READINGS, updater is not HlaCBRNSensorUpdater";
			} },
//pragma endregion
//pragma region PLATFORM ATTRIBUTES
			{ "AFTERBURNER_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAfterburnerOn(value);
				else
					std::cerr << "Error updating AFTERBURNER_ON: updater is not HlaPlatformUpdater";
			} },
			{ "ANTI_COLLISION_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAntiCollisionLightsOn(value);
				else
					std::cerr << "Error updating ANTI_COLLISION_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "BLACK_OUT_BRAKE_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBlackOutBrakeLightsOn(value);
				else
					std::cerr << "Error updating BLACK_OUT_BRAKE_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "BRAKE_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBrakeLightsOn(value);
				else
					std::cerr << "Error updating BRAKE_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "FORMATION_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFormationLightsOn(value);
				else
					std::cerr << "Error updating FORMATION_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "HATCH_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::HatchStateEnum::HatchStateEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHatchState(value);
				else
					std::cerr << "Error updating HATCH_STATE: updater is not HlaPlatformUpdater";
			} },
			{ "HEAD_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHeadLightsOn(value);
				else
					std::cerr << "Error updating HEAD_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "LANDING_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLandingLightsOn(value);
				else
					std::cerr << "Error updating LANDING_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "LAUNCHER_RAISED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLauncherRaised(value);
				else
					std::cerr << "Error updating LAUNCHER_RAISED: updater is not HlaPlatformUpdater";
			} },
			{ "NAVIGATION_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNavigationLightsOn(value);
				else
					std::cerr << "Error updating NAVIGATION_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "RAMP_DEPLOYED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRampDeployed(value);
				else
					std::cerr << "Error updating RAMP_DEPLOYED: updater is not HlaPlatformUpdater";
			} },
			{ "RUNNING_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRunningLightsOn(value);
				else
					std::cerr << "Error updating RUNNING_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "SPOT_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSpotLightsOn(value);
				else
					std::cerr << "Error updating SPOT_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
			{ "TAIL_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPlatformUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTailLightsOn(value);
				else
					std::cerr << "Error updating TAIL_LIGHTS_ON: updater is not HlaPlatformUpdater";
			} },
//pragma endregion
//pragma region CULTURAL FEATURE ATTRIBUTES
			{ "EXTERNAL_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCulturalFeatureUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setExternalLightsOn(value);
				else
					std::cerr << "Error updating EXTERNAL_LIGHTS_ON, updater is not HlaCulturalFeatureUpdater";
			} },
			{ "INTERNAL_HEAT_SOURCE_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCulturalFeatureUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setInternalHeatSourceOn(value);
				else
					std::cerr << "Error updating INTERNAL_HEAT_SOURCE_ON, updater is not HlaCulturalFeatureUpdater";
			} },
			{ "INTERNAL_LIGHTS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCulturalFeatureUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setInternalLightsOn(value);
				else
					std::cerr << "Error updating INTERNAL_LIGHTS_ON, updater is not HlaCulturalFeatureUpdater";
			} },
//pragma endregion
//pragma region COLPRO ATTRIBUTES
			{ "PROTECTION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::ProtectionEffectivenessStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCOLPROUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setProtection(value);
				else
					std::cerr << "Error updating PROTECTION, updater is not HlaCOLPROUpdater";
			} },
//pragma endregion
//pragma region DECONTAMINATION STATION ATTRIBUTES
			{ "DECONTAMINATION_PERIOD", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDecontaminationStationUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDecontaminationPeriod(value);
				else
					std::cerr << "Error updating DECONTAMINATION_PERIOD, updater is not HlaDecontaminationStationUpdater";
			} },
			{ "TREATMENTS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::TreatmentStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDecontaminationStationUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTreatments(value);
				else
					std::cerr << "Error updating TREATMENTS, updater is not HlaDecontaminationStationUpdater";
			} },
//pragma endregion
//pragma region MUNITION ATTRIBUTES
			{ "LAUNCHER_FLASH_PRESENT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMunitionUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLauncherFlashPresent(value);
				else
					std::cerr << "Error updating LAUNCHER_FLASH_PRESENT, updater is not HlaMunitionUpdater";
			} },
//pragma endregion
//pragma region AGGREGATE ENTITY ATTRIBUTES
			{ "AGGREGATE_MARKING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::AggregateMarkingStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAggregateMarking(value);
				else
					std::cerr << "Error updating AGGREGATE_MARKING, updater is not HlaAggregateEntityUpdater";
			} },
			{ "AGGREGATE_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::AggregateStateEnum::AggregateStateEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAggregateState(value);
				else
					std::cerr << "Error updating AGGREGATE_STATE, updater is not HlaAggregateEntityUpdater";
			} },
			{ "DIMENSIONS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::DimensionStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDimensions(value);
				else
					std::cerr << "Error updating DIMENSIONS, updater is not HlaAggregateEntityUpdater";
			} },
			{ "ENTITY_IDENTIFIERS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<std::string>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEntityIdentifiers(value);
				else
					std::cerr << "Error updating ENTITY_IDENTIFIERS, updater is not HlaAggregateEntityUpdater";
			} },
			{ "FORMATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::FormationEnum::FormationEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFormation(value);
				else
					std::cerr << "Error updating FORMATION, updater is not HlaAggregateEntityUpdater";
			} },
			{ "NUMBER_OF_SILENT_ENTITIES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberOfSilentEntities(value);
				else
					std::cerr << "Error updating NUMBER_OF_SILENT_ENTITIES, updater is not HlaAggregateEntityUpdater";
			} },
			{ "NUMBER_OF_VARIABLE_DATUMS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberOfVariableDatums(value);
				else
					std::cerr << "Error updating NUMBER_OF_VARIABLE_DATUMS, updater is not HlaAggregateEntityUpdater";
			} },
			{ "SILENT_AGGREGATES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::SilentAggregateStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSilentAggregates(value);
				else
					std::cerr << "Error updating SILENT_AGGREGATES, updater is not HlaAggregateEntityUpdater";
			} },
			{ "SILENT_ENTITIES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::SilentEntityStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSilentEntities(value);
				else
					std::cerr << "Error updating SILENT_ENTITIES, updater is not HlaAggregateEntityUpdater";
			} },
			{ "SUB_AGGREGATE_IDENTIFIERS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<std::string>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSubAggregateIdentifiers(value);
				else
					std::cerr << "Error updating SUB_AGGREGATE_IDENTIFIERS, updater is not HlaAggregateEntityUpdater";
			} },
			{ "VARIABLE_DATUMS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::VariableDatumStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaAggregateEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setVariableDatums(value);
				else
					std::cerr << "Error updating VARIABLE_DATUMS, updater is not HlaAggregateEntityUpdater";
			} },
//pragma endregion
//pragma region ENVIRONMENTAL ENTITY ATTRIBUTES
			{ "OPACITY_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::OpacityCodeEnum::OpacityCodeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentalEntityUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setOpacityCode(value);
				else
					std::cerr << "Error updating OPACITY_CODE, updater is not HlaEnvironmentalEntityUpdater";
			} },
//pragma endregion
//pragma region COMMUNICATION NETWORK ATTRIBUTES
			{ "NETWORK_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CommunicationNetworkTypeEnum::CommunicationNetworkTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCommunicationNetworkUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNetworkType(value);
				else
					std::cerr << "Error updating NETWORK_TYPE, updater is not HlaCommunicationNetworkUpdater";
			} },
			{ "SERVICE_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CommunicationServiceTypeEnum::CommunicationServiceTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCommunicationNetworkUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setServiceType(value);
				else
					std::cerr << "Error updating SERVICE_TYPE, updater is not HlaCommunicationNetworkUpdater";
			} },
//pragma endregion
//pragma region CONNECTION ATTRIBUTES
			{ "RECEIVERS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::ConnectionReceiverStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaConnectionUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setReceivers(value);
				else
					std::cerr << "Error updating RECEIVERS, updater is not HlaConnectionUpdater";
			} },
//pragma endregion
//pragma region LINK STATES ATTRIBUTES
			{ "LINKS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::LinkStatusStruct >*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLinkStatesUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLinks(value);
				else
					std::cerr << "Error updating LINKS, updater is not HlaLinkStatesUpdater";
			} },
//pragma endregion
//pragma region DISRUPTION EFFECT ATTRIBUTES
			{ "EFFECT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDisruptionEffectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEffect(value);
				else
					std::cerr << "Error updating EFFECT, updater is not HlaDisruptionEffectUpdater";
			} },
//pragma endregion
//pragma region ENVIRONMENT OBJECT ATTRIBUTES
			{ "OBJECT_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityIdentifierStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setObjectIdentifier(value);
				else
					std::cerr << "Error updating OBJECT_IDENTIFIER, updater is not HlaEnvironmentObjectUpdater";
			} },
			{ "REFERENCED_OBJECT_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::string*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setReferencedObjectIdentifier(value);
				else
					std::cerr << "Error updating REFERENCED_OBJECT_IDENTIFIER, updater is not HlaEnvironmentObjectUpdater";
			} },
			{ "OBJECT_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EnvironmentObjectTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setObjectType(value);
				else
					std::cerr << "Error updating OBJECT_TYPE, updater is not HlaEnvironmentObjectUpdater";
			} },
//pragma endregion
//pragma region BREACHABLE LINEAR OBJECT ATTRIBUTES
			{ "SEGMENT_RECORDS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				if (attribute.getType() >= HlaObjectInstanceBase::ObjectClassType::EXHAUST_SMOKE_OBJECT && attribute.getType() < HlaObjectInstanceBase::ObjectClassType::OTHER_LINEAR_OBJECT) {
					auto value = *static_cast<std::vector<DevStudio::ExhaustSmokeStruct>*>(attribute.getData());
					if (auto castUpdater = dynamic_cast<DevStudio::HlaExhaustSmokeObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
						castUpdater->setSegmentRecords(value);
					else
						std::cerr << "Error updating SEGMENT_RECORDS, updater is not HlaExhaustSmokeObjectUpdater";
				}
				else if (attribute.getType() >= HlaObjectInstanceBase::ObjectClassType::MINEFIELD_LANE_MARKER_OBJECT && attribute.getType() < HlaObjectInstanceBase::ObjectClassType::POINT_OBJECT) {
					auto value = *static_cast<std::vector<DevStudio::MinefieldLaneMarkerStruct>*>(attribute.getData());
					if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldLaneMarkerObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
						castUpdater->setSegmentRecords(value);
					else
						std::cerr << "Error updating SEGMENT_RECORDS, updater is not HlaMinefieldLaneMarkerObjectUpdater";
				}
				else
					std::cerr << "Error updating SEGMENT_RECORDS, object type does not match expected ranges";
			} },
//pragma endregion
//pragma region POINT OBJECT ATTRIBUTES
			{ "ORIENTATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::OrientationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setOrientation(value);
				else
					std::cerr << "Error updating ORIENTATION, updater is not HlaPointObjectUpdater";
			} },
			{ "PERCENT_COMPLETE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPercentComplete(value);
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPercentComplete(value);
				else
					std::cerr << "Error updating PERCENT_COMPLETE, updater is not HlaPointObjectUpdater";
			} },
			{ "DAMAGED_APPEARANCE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::DamageStatusEnum::DamageStatusEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDamagedAppearance(value);
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDamagedAppearance(value);
				else
					std::cerr << "Error updating DAMAGED_APPEARANCE, updater is not HlaPointObjectUpdater";
			} },
			{ "OBJECT_PRE_DISTRIBUTED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setObjectPreDistributed(value);
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setObjectPreDistributed(value);
				else
					std::cerr << "Error updating OBJECT_PRE_DISTRIBUTED, updater is not HlaPointObjectUpdater";
			} },
			{ "DEACTIVATED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDeactivated(value);
				else
					std::cerr << "Error updating DEACTIVATED, updater is not HlaPointObjectUpdater";
			} },
			{ "SMOKING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSmoking(value);
				else
					std::cerr << "Error updating SMOKING, updater is not HlaPointObjectUpdater";
			} },
			{ "FLAMING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFlaming(value);
				else
					std::cerr << "Error updating FLAMING, updater is not HlaPointObjectUpdater";
			} },
//pragma endregion
//pragma region BREACHABLE POINT OBJECT ATTRIBUTES
			{ "BREACHED_STATUS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::BreachedStatusEnum::BreachedStatusEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBreachablePointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBreachedStatus(value);
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBreachedStatus(value);
				else
					std::cerr << "Error updating BREACHED_STATUS, updater is not HlaBreachablePointObjectUpdater";
			} },
//pragma endregion
//pragma region BURST POINT OBJECT ATTRIBUTES
			{ "PERCENT_OPACITY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBurstPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPercentOpacity(value);
				else
					std::cerr << "Error updating PERCENT_OPACITY, updater is not HlaBurstPointObjectUpdater";
			} },
			{ "CYLINDER_SIZE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBurstPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCylinderSize(value);
				else
					std::cerr << "Error updating CYLINDER_SIZE, updater is not HlaBurstPointObjectUpdater";
			} },
			{ "CYLINDER_HEIGHT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBurstPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCylinderHeight(value);
				else
					std::cerr << "Error updating CYLINDER_HEIGHT, updater is not HlaBurstPointObjectUpdater";
			} },
			{ "NUMBER_OF_BURSTS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBurstPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberOfBursts(value);
				else
					std::cerr << "Error updating NUMBER_OF_BURSTS, updater is not HlaBurstPointObjectUpdater";
			} },
			{ "CHEMICAL_CONTENT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::ChemicalContentEnum::ChemicalContentEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaBurstPointObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setChemicalContent(value);
				else
					std::cerr << "Error updating CHEMICAL_CONTENT, updater is not HlaBurstPointObjectUpdater";
			} },
//pragma endregion
//pragma region CRATER OBJECT ATTRIBUTES
			{ "CRATER_SIZE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaCraterObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCraterSize(value);
				else
					std::cerr << "Error updating CRATER_SIZE, updater is not HlaCraterObjectUpdater";
			} },
//pragma endregion
//pragma region RIBBON BRIDGE OBJECT ATTRIBUTES
			{ "NUMBER_OF_SEGMENTS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRibbonBridgeObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberOfSegments(value);
				else
					std::cerr << "Error updating NUMBER_OF_SEGMENTS, updater is not HlaRibbonBridgeObjectUpdater";
			} },
//pragma endregion
//pragma region AREAL OBJECT ATTRIBUTES
			{ "POINTS_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::WorldLocationStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPointsData(value);
				else
					std::cerr << "Error updating POINTS_DATA, updater is not HlaArealObjectUpdater";
			} },
			{ "DEACTIVATED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDeactivated(value);
				else
					std::cerr << "Error updating DEACTIVATED, updater is not HlaArealObjectUpdater";
			} },
			{ "SMOKING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSmoking(value);
				else
					std::cerr << "Error updating SMOKING, updater is not HlaArealObjectUpdater";
			} },
			{ "FLAMING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaArealObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFlaming(value);
				else
					std::cerr << "Error updating FLAMING, updater is not HlaArealObjectUpdater";
			} },
//pragma endregion
//pragma region MINEFIELD OBJECT ATTRIBUTES
			{ "MINE_COUNT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldObjectUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineCount(value);
				else
					std::cerr << "Error updating MINE_COUNT, updater is not HlaMinefieldObjectUpdater";
			} },
//pragma endregion
//pragma region Weather
			{ "VISIBILITY_RANGE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWeatherUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setVisibilityRange(value);
				else
					std::cerr << "Error updating VISIBILITY_RANGE, updater is not HlaWeatherUpdater or HlaSubsurfaceLayerUpdater";
			} },
			{ "WIND", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WindStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWeatherUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setWind(value);
				else
					std::cerr << "Error updating WIND, updater is not HlaWeatherUpdater";
			} },
			{ "PRECIPITATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::PrecipitationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWeatherUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPrecipitation(value);
				else
					std::cerr << "Error updating PRECIPITATION, updater is not HlaWeatherUpdater";
			} },
			{ "HAZE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::HazeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWeatherUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHaze(value);
				else
					std::cerr << "Error updating HAZE, updater is not HlaWeatherUpdater";
			} },
			{ "HUMIDITY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWeatherUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHumidity(value);
				else
					std::cerr << "Error updating HUMIDITY, updater is not HlaWeatherUpdater";
			} },
			{ "BAROMETRIC_PRESSURE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<double*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWeatherUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBarometricPressure(value);
				else
					std::cerr << "Error updating BAROMETRIC_PRESSURE, updater is not HlaWeatherUpdater";
			} },
//pragma endregion
//pragma region LAND SURFACE ATTRIBUTES
			{ "SNOW", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SnowStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLandSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSnow(value);
				else
					std::cerr << "Error updating SNOW, updater is not HlaLandSurfaceUpdater";
			} },
			{ "MOISTURE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SurfaceMoistureEnum::SurfaceMoistureEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLandSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMoisture(value);
				else
					std::cerr << "Error updating MOISTURE, updater is not HlaLandSurfaceUpdater";
			} },
			{ "ICE_CONDITION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RoadIceConditionEnum::RoadIceConditionEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaLandSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setIceCondition(value);
				else
					std::cerr << "Error updating ICE_CONDITION, updater is not HlaLandSurfaceUpdater";
			} },
//pragma endregion
//pragma region WATER SURFACE ATTRIBUTES
			{ "SEA_STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SeaStateEnum::SeaStateEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWaterSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSeaState(value);
				else
					std::cerr << "Error updating SEA_STATE, updater is not HlaWaterSurfaceUpdater";
			} },
			{ "SALINITY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWaterSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSalinity(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaSubsurfaceLayerUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSalinity(value);
				else
					std::cerr << "Error updating SALINITY, updater is not HlaWaterSurfaceUpdater";
			} },
			{ "ICE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IceStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWaterSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setIce(value);
				else
					std::cerr << "Error updating ICE, updater is not HlaWaterSurfaceUpdater";
			} },
			{ "CURRENT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CurrentStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWaterSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCurrent(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaSubsurfaceLayerUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCurrent(value);
				else
					std::cerr << "Error updating CURRENT, updater is not HlaWaterSurfaceUpdater";
			} },
			{ "WAVE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WaveStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWaterSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setWave(value);
				else
					std::cerr << "Error updating WAVE, updater is not HlaWaterSurfaceUpdater";
			} },
			{ "SWELL", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WaveStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaWaterSurfaceUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSwell(value);
				else
					std::cerr << "Error updating SWELL, updater is not HlaWaterSurfaceUpdater";
			} },
//pragma endregion
//pragma region TROPOSHERE LAYER ATTRIBUTES
			{ "LAYER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::LayerStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaTroposphereLayerUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLayer(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaSubsurfaceLayerUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLayer(value);
				else
					std::cerr << "Error updating LAYER, updater is not HlaTroposphereLayerUpdater";
			} },
			{ "CLOUD", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CloudStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaTroposphereLayerUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCloud(value);
				else
					std::cerr << "Error updating CLOUD, updater is not HlaTroposphereLayerUpdater";
			} },
//pragma endregion
//pragma region SUBSURFACE LAYER ATTRIBUTES
			{ "BOTTOM_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SedimentTypeEnum::SedimentTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSubsurfaceLayerUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBottomType(value);
				else
					std::cerr << "Error updating BOTTOM_TYPE, updater is not HlaSubsurfaceLayerUpdater";
			} },
//pragma endregion
//pragma region Minefield 
			{ "ACTIVE_STATUS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::MinefieldStatusEnum::MinefieldStatusEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setActiveStatus(value);
				else
					std::cerr << "Error updating ACTIVE_STATUS, updater is not HlaMinefieldUpdater";
			} },
			{ "LANE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::MinefieldLaneEnum::MinefieldLaneEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLane(value);
				else
					std::cerr << "Error updating LANE, updater is not HlaMinefieldUpdater";
			} },
			{ "MINEFIELD_APPEARANCE_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::MinefieldTypeEnum::MinefieldTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMinefieldAppearanceType(value);
				else
					std::cerr << "Error updating MINEFIELD_APPEARANCE_TYPE, updater is not HlaMinefieldUpdater";
			} },
			{ "MINEFIELD_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityIdentifierStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMinefieldIdentifier(value);
				else
					std::cerr << "Error updating MINEFIELD_IDENTIFIER, updater is not HlaMinefieldUpdater";
			} },
			{ "MINEFIELD_LOCATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WorldLocationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMinefieldLocation(value);
				else
					std::cerr << "Error updating MINEFIELD_LOCATION, updater is not HlaMinefieldUpdater";
			} },
			{ "MINEFIELD_ORIENTATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::OrientationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMinefieldOrientation(value);
				else
					std::cerr << "Error updating MINEFIELD_ORIENTATION, updater is not HlaMinefieldUpdater";
			} },
			{ "MINEFIELD_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMinefieldType(value);
				else
					std::cerr << "Error updating MINEFIELD_TYPE, updater is not HlaMinefieldUpdater";
			} },
			{ "MINE_TYPES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::EntityTypeStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineTypes(value);
				else
					std::cerr << "Error updating MINE_TYPES, updater is not HlaMinefieldUpdater";
			} },
			{ "PERIMETER_POINT_COORDINATES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::PerimeterPointStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPerimeterPointCoordinates(value);
				else
					std::cerr << "Error updating PERIMETER_POINT_COORDINATES, updater is not HlaMinefieldUpdater";
			} },
			{ "PROTOCOL_MODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::MinefieldProtocolEnum::MinefieldProtocolEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setProtocolMode(value);
				else
					std::cerr << "Error updating PROTOCOL_MODE, updater is not HlaMinefieldUpdater";
			} },
			{ "STATE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setState(value);
				else
					std::cerr << "Error updating STATE, updater is not HlaMinefieldUpdater";
			} },
//pragma endregion
//pragma region EMBEDDED SYSTEM ATTRIBUTES
			{ "HOST_OBJECT_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::string*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmbeddedSystemUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHostObjectIdentifier(value);
				else
					std::cerr << "Error updating HOST_OBJECT_IDENTIFIER, updater is not HlaEmbeddedSystemUpdater";
			} },
			{ "RELATIVE_POSITION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RelativePositionStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmbeddedSystemUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRelativePosition(value);
				else
					std::cerr << "Error updating RELATIVE_POSITION, updater is not HlaEmbeddedSystemUpdater";
			} },
//pragma endregion
//pragma region MINEFIELD DATA ATTRIBUTES
			{ "GROUND_BURIAL_DEPTH_OFFSET", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<float>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setGroundBurialDepthOffset(value);
				else
					std::cerr << "Error updating GROUND_BURIAL_DEPTH_OFFSET, updater is not HlaMinefieldDataUpdater";
			} },
			{ "FUSING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::MineFusingStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFusing(value);
				else
					std::cerr << "Error updating FUSING, updater is not HlaMinefieldDataUpdater";
			} },
			{ "MINE_EMPLACEMENT_TIME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::ClockTimeStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineEmplacementTime(value);
				else
					std::cerr << "Error updating MINE_EMPLACEMENT_TIME, updater is not HlaMinefieldDataUpdater";
			} },
			{ "MINE_ENTITY_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<unsigned short>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineEntityIdentifier(value);
				else
					std::cerr << "Error updating MINE_ENTITY_IDENTIFIER, updater is not HlaMinefieldDataUpdater";
			} },
			{ "MINEFIELD_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::string*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMinefieldIdentifier(value);
				else
					std::cerr << "Error updating MINEFIELD_IDENTIFIER, updater is not HlaMinefieldDataUpdater";
			} },
			{ "MINE_LOCATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::WorldLocationStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineLocation(value);
				else
					std::cerr << "Error updating MINE_LOCATION, updater is not HlaMinefieldDataUpdater";
			} },
			{ "MINE_ORIENTATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::OrientationStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineOrientation(value);
				else
					std::cerr << "Error updating MINE_ORIENTATION, updater is not HlaMinefieldDataUpdater";
			} },
			{ "MINE_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMineType(value);
				else
					std::cerr << "Error updating MINE_TYPE, updater is not HlaMinefieldDataUpdater";
			} },
			{ "NUMBER_TRIP_DETONATION_WIRES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<char>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberTripDetonationWires(value);
				else
					std::cerr << "Error updating NUMBER_TRIP_DETONATION_WIRES, updater is not HlaMinefieldDataUpdater";
			} },
			{ "NUMBER_WIRE_VERTICES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<char>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberWireVertices(value);
				else
					std::cerr << "Error updating NUMBER_WIRE_VERTICES, updater is not HlaMinefieldDataUpdater";
			} },
			{ "PAINT_SCHEME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPaintScheme(value);
				else
					std::cerr << "Error updating PAINT_SCHEME, updater is not HlaMinefieldDataUpdater";
			} },
			{ "REFLECTANCE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<float>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setReflectance(value);
				else
					std::cerr << "Error updating REFLECTANCE, updater is not HlaMinefieldDataUpdater";
			} },
			{ "SCALAR_DETECTION_COEFFICIENT", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<char>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setScalarDetectionCoefficient(value);
				else
					std::cerr << "Error updating SCALAR_DETECTION_COEFFICIENT, updater is not HlaMinefieldDataUpdater";
			} },
			{ "SENSOR_TYPES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSensorTypes(value);
				else
					std::cerr << "Error updating SENSOR_TYPES, updater is not HlaMinefieldDataUpdater";
			} },
			{ "SNOW_BURIAL_DEPTH_OFFSET", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<float>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSnowBurialDepthOffset(value);
				else
					std::cerr << "Error updating SNOW_BURIAL_DEPTH_OFFSET, updater is not HlaMinefieldDataUpdater";
			} },
			{ "THERMAL_CONTRAST", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<float>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setThermalContrast(value);
				else
					std::cerr << "Error updating THERMAL_CONTRAST, updater is not HlaMinefieldDataUpdater";
			} },
			{ "WATER_BURIAL_DEPTH_OFFSET", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<float>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setWaterBurialDepthOffset(value);
				else
					std::cerr << "Error updating WATER_BURIAL_DEPTH_OFFSET, updater is not HlaMinefieldDataUpdater";
			} },
			{ "WIRE_VERTICES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::WorldLocationStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaMinefieldDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setWireVertices(value);
				else
					std::cerr << "Error updating WIRE_VERTICES, updater is not HlaMinefieldDataUpdater";
			} },
//pragma endregion
//pragma region RADIO TRANSMITTER ATTRIBUTES
			{ "ANTENNA_PATTERN_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::AntennaPatternVariantStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAntennaPatternData(value);
				else
					std::cerr << "Error updating ANTENNA_PATTERN_DATA, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "CRYPTOGRAPHIC_MODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CryptographicModeEnum::CryptographicModeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCryptographicMode(value);
				else
					std::cerr << "Error updating CRYPTOGRAPHIC_MODE, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "CRYPTO_SYSTEM", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::CryptographicSystemTypeEnum::CryptographicSystemTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCryptoSystem(value);
				else
					std::cerr << "Error updating CRYPTO_SYSTEM, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "ENCRYPTION_KEY_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEncryptionKeyIdentifier(value);
				else
					std::cerr << "Error updating ENCRYPTION_KEY_IDENTIFIER, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "FREQUENCY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned long long*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFrequency(value);
				else
					std::cerr << "Error updating FREQUENCY, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "FREQUENCY_BANDWIDTH", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFrequencyBandwidth(value);
				else
					std::cerr << "Error updating FREQUENCY_BANDWIDTH, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "RADIO_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRadioIndex(value);
				else
					std::cerr << "Error updating RADIO_INDEX, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "RADIO_INPUT_SOURCE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RadioInputSourceEnum::RadioInputSourceEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRadioInputSource(value);
				else
					std::cerr << "Error updating RADIO_INPUT_SOURCE, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "RADIO_SYSTEM_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RadioTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRadioSystemType(value);
				else
					std::cerr << "Error updating RADIO_SYSTEM_TYPE, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "R_FMODULATION_SYSTEM_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RFModulationSystemTypeEnum::RFModulationSystemTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRFModulationSystemType(value);
				else
					std::cerr << "Error updating R_FMODULATION_SYSTEM_TYPE, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "R_FMODULATION_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RFModulationTypeVariantStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRFModulationType(value);
				else
					std::cerr << "Error updating R_FMODULATION_TYPE, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "SPREAD_SPECTRUM", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::SpreadSpectrumVariantStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSpreadSpectrum(value);
				else
					std::cerr << "Error updating SPREAD_SPECTRUM, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "STREAM_TAG", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned long long*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setStreamTag(value);
				else
					std::cerr << "Error updating STREAM_TAG, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "TIME_HOP_IN_USE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTimeHopInUse(value);
				else
					std::cerr << "Error updating TIME_HOP_IN_USE, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "TRANSMITTED_POWER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTransmittedPower(value);
				else
					std::cerr << "Error updating TRANSMITTED_POWER, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "TRANSMITTER_OPERATIONAL_STATUS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::TransmitterOperationalStatusEnum::TransmitterOperationalStatusEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTransmitterOperationalStatus(value);
				else
					std::cerr << "Error updating TRANSMITTER_OPERATIONAL_STATUS, updater is not HlaRadioTransmitterUpdater";
			} },
			{ "WORLD_LOCATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WorldLocationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioTransmitterUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setWorldLocation(value);
				else
					std::cerr << "Error updating WORLD_LOCATION, updater is not HlaRadioTransmitterUpdater";
			} },
//pragma endregion
//pragma region RADIO RECEIVER ATTRIBUTES
			{"RADIO_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
					auto value = *static_cast<unsigned short*>(attribute.getData());
					if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioReceiverUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
						castUpdater->setRadioIndex(value);
					else
						std::cerr << "Error updating RADIO_INDEX, updater is not HlaRadioReceiverUpdater";
			} },
			{ "RECEIVED_POWER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				 auto value = *static_cast<float*>(attribute.getData());
				 if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioReceiverUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					 castUpdater->setReceivedPower(value);
				 else
					 std::cerr << "Error updating RECEIVED_POWER, updater is not HlaRadioReceiverUpdater";
			} },
			{ "RECEIVED_TRANSMITTER_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				 auto value = *static_cast<std::string*>(attribute.getData());
				 if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioReceiverUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					 castUpdater->setReceivedTransmitterIdentifier(value);
				 else
					 std::cerr << "Error updating RECEIVED_TRANSMITTER_IDENTIFIER, updater is not HlaRadioReceiverUpdater";
			} },
			{ "RECEIVER_OPERATIONAL_STATUS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				 auto value = *static_cast<DevStudio::ReceiverOperationalStatusEnum::ReceiverOperationalStatusEnum*>(attribute.getData());
				 if (auto castUpdater = dynamic_cast<DevStudio::HlaRadioReceiverUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					 castUpdater->setReceiverOperationalStatus(value);
				 else
					 std::cerr << "Error updating RECEIVER_OPERATIONAL_STATUS, updater is not HlaRadioReceiverUpdater";
			} },
//pragma endregion
//pragma region Designator
			{ "CODE_NAME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DesignatorCodeNameEnum::DesignatorCodeNameEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCodeName(value);
				else
					std::cerr << "Error updating CODE_NAME, updater is not HlaDesignatorUpdater";
			} },
			{ "DESIGNATED_OBJECT_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::string*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDesignatedObjectIdentifier(value);
				else
					std::cerr << "Error updating DESIGNATED_OBJECT_IDENTIFIER, updater is not HlaDesignatorUpdater";
			} },
			{ "DESIGNATOR_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DesignatorCodeEnum::DesignatorCodeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDesignatorCode(value);
				else
					std::cerr << "Error updating DESIGNATOR_CODE, updater is not HlaDesignatorUpdater";
			} },
			{ "DESIGNATOR_EMISSION_WAVELENGTH", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDesignatorEmissionWavelength(value);
				else
					std::cerr << "Error updating DESIGNATOR_EMISSION_WAVELENGTH, updater is not HlaDesignatorUpdater";
			} },
			{ "DESIGNATOR_OUTPUT_POWER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDesignatorOutputPower(value);
				else
					std::cerr << "Error updating DESIGNATOR_OUTPUT_POWER, updater is not HlaDesignatorUpdater";
			} },
			{ "DESIGNATOR_SPOT_LOCATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::WorldLocationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDesignatorSpotLocation(value);
				else
					std::cerr << "Error updating DESIGNATOR_SPOT_LOCATION, updater is not HlaDesignatorUpdater";
			} },
			{ "DEAD_RECKONING_ALGORITHM", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setDeadReckoningAlgorithm(value);
				else
					std::cerr << "Error updating DEAD_RECKONING_ALGORITHM, updater is not HlaDesignatorUpdater";
			} },
			{ "RELATIVE_SPOT_LOCATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::RelativePositionStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setRelativeSpotLocation(value);
				else
					std::cerr << "Error updating RELATIVE_SPOT_LOCATION, updater is not HlaDesignatorUpdater";
			} },
			{ "SPOT_LINEAR_ACCELERATION_VECTOR", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::AccelerationVectorStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaDesignatorUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSpotLinearAccelerationVector(value);
				else
					std::cerr << "Error updating SPOT_LINEAR_ACCELERATION_VECTOR, updater is not HlaDesignatorUpdater";
			} },
//pragma endregion
//pragma region EmitterSystem
			{ "EMITTER_FUNCTION_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<EmitterFunctionEnum::EmitterFunctionEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterSystemUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEmitterFunctionCode(value);
				else
					std::cerr << "Error updating EMITTER_FUNCTION_CODE, updater is not HlaEmitterSystemUpdater";
			} },
			{ "EMITTER_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<EmitterTypeEnum::EmitterTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterSystemUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEmitterType(value);
				else
					std::cerr << "Error updating EMITTER_TYPE, updater is not HlaEmitterSystemUpdater";
			} },
			{ "EMITTER_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<char*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterSystemUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEmitterIndex(value);
				else
					std::cerr << "Error updating EMITTER_INDEX, updater is not HlaEmitterSystemUpdater";
			} },
			{ "EVENT_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<EventIdentifierStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterSystemUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaUnderwaterAcousticsEmissionUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else if (auto castUpdater = dynamic_cast<DevStudio::HlaUnderwaterAcousticsEmissionUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEventIdentifier(value);
				else
					std::cerr << "Error updating EVENT_IDENTIFIER, updater is not a recognized updater";
			} },
//pragma endregion
//pragma region IFF ATTRIBUTES
			{ "BEAM_AZIMUTH_CENTER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamAzimuthCenter(value);
				else
					std::cerr << "Error updating BEAM_AZIMUTH_CENTER, updater is not HlaIFFUpdater";
			} },
			{ "BEAM_AZIMUTH_SWEEP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamAzimuthSweep(value);
				else
					std::cerr << "Error updating BEAM_AZIMUTH_SWEEP, updater is not HlaIFFUpdater";
			} },
			{ "BEAM_ELEVATION_CENTER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamElevationCenter(value);
				else
					std::cerr << "Error updating BEAM_ELEVATION_CENTER, updater is not HlaIFFUpdater";
			} },
			{ "BEAM_ELEVATION_SWEEP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamElevationSweep(value);
				else
					std::cerr << "Error updating BEAM_ELEVATION_SWEEP, updater is not HlaIFFUpdater";
			} },
			{ "BEAM_SWEEP_SYNC", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamSweepSync(value);
				else
					std::cerr << "Error updating BEAM_SWEEP_SYNC, updater is not HlaIFFUpdater";
			} },
			{ "FUNDAMENTAL_PARAMETER_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::FundamentalParameterDataStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFundamentalParameterData(value);
				else
					std::cerr << "Error updating FUNDAMENTAL_PARAMETER_DATA, updater is not HlaIFFUpdater";
			} },
			{ "LAYER2DATA_AVAILABLE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setLayer2DataAvailable(value);
				else
					std::cerr << "Error updating LAYER2DATA_AVAILABLE, updater is not HlaIFFUpdater";
			} },
			{ "SECONDARY_OPERATIONAL_DATA_PARAMETER1", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IffOperationalParameter1Enum::IffOperationalParameter1Enum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSecondaryOperationalDataParameter1(value);
				else
					std::cerr << "Error updating SECONDARY_OPERATIONAL_DATA_PARAMETER1, updater is not HlaIFFUpdater";
			} },
			{ "SECONDARY_OPERATIONAL_DATA_PARAMETER2", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IffOperationalParameter2Enum::IffOperationalParameter2Enum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSecondaryOperationalDataParameter2(value);
				else
					std::cerr << "Error updating SECONDARY_OPERATIONAL_DATA_PARAMETER2, updater is not HlaIFFUpdater";
			} },
			{ "SYSTEM_MODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IffSystemModeEnum::IffSystemModeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSystemMode(value);
				else
					std::cerr << "Error updating SYSTEM_MODE, updater is not HlaIFFUpdater";
			} },
			{ "SYSTEM_NAME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IffSystemNameEnum::IffSystemNameEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSystemName(value);
				else
					std::cerr << "Error updating SYSTEM_NAME, updater is not HlaIFFUpdater";
			} },
			{ "SYSTEM_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::IffSystemTypeEnum::IffSystemTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSystemType(value);
				else
					std::cerr << "Error updating SYSTEM_TYPE, updater is not HlaIFFUpdater";
			} },
			{ "SYSTEM_IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSystemIsOn(value);
				else
					std::cerr << "Error updating SYSTEM_IS_ON, updater is not HlaIFFUpdater";
			} },
			{ "SYSTEM_IS_OPERATIONAL", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSystemIsOperational(value);
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSystemIsOperational(value);
				else
					std::cerr << "Error updating SYSTEM_IS_OPERATIONAL, updater is not HlaIFFUpdater";
			} },
//pragma endregion
//pragma region NATO IFF ATTRIBUTES
			{ "ALTERNATE_MODE4", [](void* updater, DataTypes::AttributeValuePair attribute) {
					auto value = *static_cast<DevStudio::IffAlternateMode4Enum::IffAlternateMode4Enum*>(attribute.getData());
					if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
						castUpdater->setAlternateMode4(value);
					else
						std::cerr << "Error updating ALTERNATE_MODE4, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE1ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode1Enabled(value);
				else
					std::cerr << "Error updating MODE1ENABLED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE1IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode1IsDamaged(value);
				else
					std::cerr << "Error updating MODE1IS_DAMAGED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE1IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode1IsMalfunctioning(value);
				else
					std::cerr << "Error updating MODE1IS_MALFUNCTIONING, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE1IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode1IsOn(value);
				else
					std::cerr << "Error updating MODE1IS_ON, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE2ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode2Enabled(value);
				else
					std::cerr << "Error updating MODE2ENABLED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE2IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode2IsDamaged(value);
				else
					std::cerr << "Error updating MODE2IS_DAMAGED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE2IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode2IsMalfunctioning(value);
				else
					std::cerr << "Error updating MODE2IS_MALFUNCTIONING, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE2IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode2IsOn(value);
				else
					std::cerr << "Error updating MODE2IS_ON, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE3AENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode3AEnabled(value);
				else
					std::cerr << "Error updating MODE3AENABLED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE3AIS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode3AIsDamaged(value);
				else
					std::cerr << "Error updating MODE3AIS_DAMAGED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE3AIS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode3AIsMalfunctioning(value);
				else
					std::cerr << "Error updating MODE3AIS_MALFUNCTIONING, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE3AIS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode3AIsOn(value);
				else
					std::cerr << "Error updating MODE3AIS_ON, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE4ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode4Enabled(value);
				else
					std::cerr << "Error updating MODE4ENABLED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE4IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode4IsDamaged(value);
				else
					std::cerr << "Error updating MODE4IS_DAMAGED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE4IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode4IsMalfunctioning(value);
				else
					std::cerr << "Error updating MODE4IS_MALFUNCTIONING, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE4IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode4IsOn(value);
				else
					std::cerr << "Error updating MODE4IS_ON, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE4PSEUDO_CRYPTO", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode4PseudoCrypto(value);
				else
					std::cerr << "Error updating MODE4PSEUDO_CRYPTO, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE4PSEUDO_CRYPTO_AVAILABLE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode4PseudoCryptoAvailable(value);
				else
					std::cerr << "Error updating MODE4PSEUDO_CRYPTO_AVAILABLE, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE5CENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode5CEnabled(value);
				else
					std::cerr << "Error updating MODE5CENABLED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE5CIS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode5CIsDamaged(value);
				else
					std::cerr << "Error updating MODE5CIS_DAMAGED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE5CIS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode5CIsMalfunctioning(value);
				else
					std::cerr << "Error updating MODE5CIS_MALFUNCTIONING, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE5CIS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode5CIsOn(value);
				else
					std::cerr << "Error updating MODE5CIS_ON, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE_SENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setModeSEnabled(value);
				else
					std::cerr << "Error updating MODE_SENABLED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE_SIS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setModeSIsDamaged(value);
				else
					std::cerr << "Error updating MODE_SIS_DAMAGED, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE_SIS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setModeSIsMalfunctioning(value);
				else
					std::cerr << "Error updating MODE_SIS_MALFUNCTIONING, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE_SIS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setModeSIsOn(value);
				else
					std::cerr << "Error updating MODE_SIS_ON, updater is not HlaNatoIFFUpdater";
			} },
			{ "MODE_SIS_TCAS_I", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setModeSIsTcasI(value);
				else
					std::cerr << "Error updating MODE_SIS_TCAS_I, updater is not HlaNatoIFFUpdater";
			} },
//pragma endregion
//pragma region NATO TRANSPORT IFF ATTRIBUTES
			{ "EMERGENCY_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEmergencyOn(value);
				else
					std::cerr << "Error updating EMERGENCY_ON, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "IDENT_SQUAWK_FLASH_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setIdentSquawkFlashOn(value);
				else
					std::cerr << "Error updating IDENT_SQUAWK_FLASH_ON, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "MODE1CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode1Code(value);
				else
					std::cerr << "Error updating MODE1CODE, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "MODE2CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode2Code(value);
				else
					std::cerr << "Error updating MODE2CODE, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "MODE3ACODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode3ACode(value);
				else
					std::cerr << "Error updating MODE3ACODE, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "MODE5CALTITUDE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode5CAltitude(value);
				else
					std::cerr << "Error updating MODE5CALTITUDE, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "MODE5CALTITUDE_AVAILABLE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setMode5CAltitudeAvailable(value);
				else
					std::cerr << "Error updating MODE5CALTITUDE_AVAILABLE, updater is not HlaNatoIFFTransponderUpdater";
			} },
			{ "STI_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaNatoIFFTransponderUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setStiOn(value);
				else
					std::cerr << "Error updating STI_ON, updater is not HlaNatoIFFTransponderUpdater";
			} },
//pragma endregion
//pragma region SOVIET IFF ATTRIBUTES
			{ "PARAMETER1ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter1Enabled(value);
				else
					std::cerr << "Error updating PARAMETER1ENABLED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER1IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter1IsDamaged(value);
				else
					std::cerr << "Error updating PARAMETER1IS_DAMAGED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER1IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter1IsMalfunctioning(value);
				else
					std::cerr << "Error updating PARAMETER1IS_MALFUNCTIONING, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER1IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter1IsOn(value);
				else
					std::cerr << "Error updating PARAMETER1IS_ON, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER2ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter2Enabled(value);
				else
					std::cerr << "Error updating PARAMETER2ENABLED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER2IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter2IsDamaged(value);
				else
					std::cerr << "Error updating PARAMETER2IS_DAMAGED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER2IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter2IsMalfunctioning(value);
				else
					std::cerr << "Error updating PARAMETER2IS_MALFUNCTIONING, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER2IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter2IsOn(value);
				else
					std::cerr << "Error updating PARAMETER2IS_ON, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER3ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter3Enabled(value);
				else
					std::cerr << "Error updating PARAMETER3ENABLED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER3IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter3IsDamaged(value);
				else
					std::cerr << "Error updating PARAMETER3IS_DAMAGED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER3IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter3IsMalfunctioning(value);
				else
					std::cerr << "Error updating PARAMETER3IS_MALFUNCTIONING, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER3IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter3IsOn(value);
				else
					std::cerr << "Error updating PARAMETER3IS_ON, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER4ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter4Enabled(value);
				else
					std::cerr << "Error updating PARAMETER4ENABLED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER4IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter4IsDamaged(value);
				else
					std::cerr << "Error updating PARAMETER4IS_DAMAGED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER4IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter4IsMalfunctioning(value);
				else
					std::cerr << "Error updating PARAMETER4IS_MALFUNCTIONING, updater is not HlaSovietIFFUpdater";
			} },

			{ "PARAMETER4IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter4IsOn(value);
				else
					std::cerr << "Error updating PARAMETER4IS_ON, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER5ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter5Enabled(value);
				else
					std::cerr << "Error updating PARAMETER5ENABLED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER5IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter5IsDamaged(value);
				else
					std::cerr << "Error updating PARAMETER5IS_DAMAGED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER5IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter5IsMalfunctioning(value);
				else
					std::cerr << "Error updating PARAMETER5IS_MALFUNCTIONING, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER5IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter5IsOn(value);
				else
					std::cerr << "Error updating PARAMETER5IS_ON, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER6ENABLED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter6Enabled(value);
				else
					std::cerr << "Error updating PARAMETER6ENABLED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER6IS_DAMAGED", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter6IsDamaged(value);
				else
					std::cerr << "Error updating PARAMETER6IS_DAMAGED, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER6IS_MALFUNCTIONING", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter6IsMalfunctioning(value);
				else
					std::cerr << "Error updating PARAMETER6IS_MALFUNCTIONING, updater is not HlaSovietIFFUpdater";
			} },
			{ "PARAMETER6IS_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaSovietIFFUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setParameter6IsOn(value);
				else
					std::cerr << "Error updating PARAMETER6IS_ON, updater is not HlaSovietIFFUpdater";
			} },
//pragma endregion
//pragma region ACTIVE SONAR ATTRIBUTES
			{ "ACOUSTIC_NAME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::ActiveSonarEnum::ActiveSonarEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAcousticName(value);
				else
					std::cerr << "Error updating ACOUSTIC_NAME, updater is not HlaActiveSonarUpdater";
			} },
			{ "FUNCTION_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::ActiveSonarFunctionCodeEnum::ActiveSonarFunctionCodeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFunctionCode(value);
				else
					std::cerr << "Error updating FUNCTION_CODE, updater is not HlaActiveSonarUpdater";
			} },
			{ "ACOUSTICS_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<char*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAcousticsIdentifier(value);
				else
					std::cerr << "Error updating ACOUSTICS_IDENTIFIER, updater is not HlaActiveSonarUpdater";
			} },
//pragma endregion
//pragma region PROPULSION NOISE ATTRIBUTES
			{ "HULL_MASKER_ON", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPropulsionNoiseUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHullMaskerOn(value);
				else
					std::cerr << "Error updating HULL_MASKER_ON, updater is not HlaPropulsionNoiseUpdater";
			} },
			{ "PASSIVE_PARAMETER_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPropulsionNoiseUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPassiveParameterIndex(value);
				else
					std::cerr << "Error updating PASSIVE_PARAMETER_INDEX, updater is not HlaPropulsionNoiseUpdater";
			} },
			{ "PROPULSION_PLANT_CONFIGURATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::PropulsionPlantEnum::PropulsionPlantEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPropulsionNoiseUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPropulsionPlantConfiguration(value);
				else
					std::cerr << "Error updating PROPULSION_PLANT_CONFIGURATION, updater is not HlaPropulsionNoiseUpdater";
			} },
			{ "SHAFT_RATE_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::ShaftDataStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaPropulsionNoiseUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setShaftRateData(value);
				else
					std::cerr << "Error updating SHAFT_RATE_DATA, updater is not HlaPropulsionNoiseUpdater";
			} },
//pragma endregion
//pragma region ENVIRONMENT PROCESS ATTRIBUTES
			{ "PROCESS_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityIdentifierStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentProcessUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setProcessIdentifier(value);
				else
					std::cerr << "Error updating PROCESS_IDENTIFIER, updater is not HlaEnvironmentProcessUpdater";
			} },
			{ "TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EnvironmentTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentProcessUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setType(value);
				else
					std::cerr << "Error updating TYPE, updater is not HlaEnvironmentProcessUpdater";
			} },
			{ "MODEL_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentProcessUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
				{
					auto value = *static_cast<DevStudio::EnvironmentModelTypeEnum::EnvironmentModelTypeEnum*>(attribute.getData());
					castUpdater->setModelType(value);
				}
				else
					std::cerr << "Error updating MODEL_TYPE, updater is not HlaEnvironmentProcessUpdater";
			} },
			{ "ENVIRONMENT_PROCESS_ACTIVE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentProcessUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEnvironmentProcessActive(value);
				else
					std::cerr << "Error updating ENVIRONMENT_PROCESS_ACTIVE, updater is not HlaEnvironmentProcessUpdater";
			} },
			{ "SEQUENCE_NUMBER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentProcessUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSequenceNumber(value);
				else
					std::cerr << "Error updating SEQUENCE_NUMBER, updater is not HlaEnvironmentProcessUpdater";
			} },
			{ "ENVIRONMENT_REC_DATA", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::EnvironmentRecStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEnvironmentProcessUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEnvironmentRecData(value);
				else
					std::cerr << "Error updating ENVIRONMENT_REC_DATA, updater is not HlaEnvironmentProcessUpdater";
			} },
//pragma endregion
//pragma region GRIDDED DATA ATTRIBUTES
			{ "GRID_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EntityIdentifierStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setGridIdentifier(value);
				else
					std::cerr << "Error updating GRID_IDENTIFIER, updater is not HlaGriddedDataUpdater";
			} },
			{ "COORDINATE_SYSTEM", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EnvironmentDataCoordinateSystemEnum::EnvironmentDataCoordinateSystemEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setCoordinateSystem(value);
				else
					std::cerr << "Error updating COORDINATE_SYSTEM, updater is not HlaGriddedDataUpdater";
			} },
			{ "NUMBER_OF_GRID_AXES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<char*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setNumberOfGridAxes(value);
				else
					std::cerr << "Error updating NUMBER_OF_GRID_AXES, updater is not HlaGriddedDataUpdater";
			} },
			{ "CONSTANT_GRID", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EnvironmentGridTypeEnum::EnvironmentGridTypeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setConstantGrid(value);
				else
					std::cerr << "Error updating CONSTANT_GRID, updater is not HlaGriddedDataUpdater";
			} },
			{ "ENVIRONMENT_TYPE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::EnvironmentTypeStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEnvironmentType(value);
				else
					std::cerr << "Error updating ENVIRONMENT_TYPE, updater is not HlaGriddedDataUpdater";
			} },
			{ "ORIENTATION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::OrientationStruct*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setOrientation(value);
				else
					std::cerr << "Error updating ORIENTATION, updater is not HlaGriddedDataUpdater";
			} },
			{ "SAMPLE_TIME", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned long long*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSampleTime(value);
				else
					std::cerr << "Error updating SAMPLE_TIME, updater is not HlaGriddedDataUpdater";
			} },
			{ "TOTAL_VALUES", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTotalValues(value);
				else
					std::cerr << "Error updating TOTAL_VALUES, updater is not HlaGriddedDataUpdater";
			} },
			{ "VECTOR_DIMENSION", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<char*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setVectorDimension(value);
				else
					std::cerr << "Error updating VECTOR_DIMENSION, updater is not HlaGriddedDataUpdater";
			} },
			{ "GRID_AXIS_INFO", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::GridAxisStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setGridAxisInfo(value);
				else
					std::cerr << "Error updating GRID_AXIS_INFO, updater is not HlaGriddedDataUpdater";
			} },
			{ "GRID_DATA_INFO", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<DevStudio::GridDataStruct>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaGriddedDataUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setGridDataInfo(value);
				else
					std::cerr << "Error updating GRID_DATA_INFO, updater is not HlaGriddedDataUpdater";
			} },
//pragma endregion
//pragma region EmitterBeam
			{ "BEAM_AZIMUTH_CENTER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamAzimuthCenter(value);
				else
					std::cerr << "Error updating BEAM_AZIMUTH_CENTER, updater is not HlaEmitterBeamUpdater";
			} },
			{ "BEAM_AZIMUTH_SWEEP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamAzimuthSweep(value);
				else
					std::cerr << "Error updating BEAM_AZIMUTH_SWEEP, updater is not HlaEmitterBeamUpdater";
			} },
			{ "BEAM_ELEVATION_CENTER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<double*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamElevationCenter(value);
				else
					std::cerr << "Error updating BEAM_ELEVATION_CENTER, updater is not HlaEmitterBeamUpdater";
			} },
			{ "BEAM_ELEVATION_SWEEP", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamElevationSweep(value);
				else
					std::cerr << "Error updating BEAM_ELEVATION_SWEEP, updater is not HlaEmitterBeamUpdater";
			} },
			{ "BEAM_FUNCTION_CODE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<BeamFunctionCodeEnum::BeamFunctionCodeEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamFunctionCode(value);
				else
					std::cerr << "Error updating BEAM_FUNCTION_CODE, updater is not HlaEmitterBeamUpdater";
			} },
			{ "BEAM_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<char*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamIdentifier(value);
				else
					std::cerr << "Error updating BEAM_IDENTIFIER, updater is not HlaEmitterBeamUpdater";
			} },
			{ "BEAM_PARAMETER_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamParameterIndex(value);
				else
					std::cerr << "Error updating BEAM_PARAMETER_INDEX, updater is not HlaEmitterBeamUpdater";
			} },
			{ "EFFECTIVE_RADIATED_POWER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEffectiveRadiatedPower(value);
				else
					std::cerr << "Error updating EFFECTIVE_RADIATED_POWER, updater is not HlaEmitterBeamUpdater";
			} },
			{ "EMISSION_FREQUENCY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEmissionFrequency(value);
				else
					std::cerr << "Error updating EMISSION_FREQUENCY, updater is not HlaEmitterBeamUpdater";
			} },
			{ "EMITTER_SYSTEM_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::string*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setEmitterSystemIdentifier(value);
				else
					std::cerr << "Error updating EMITTER_SYSTEM_IDENTIFIER, updater is not HlaEmitterBeamUpdater";
			} },
			{ "FREQUENCY_RANGE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setFrequencyRange(value);
				else
					std::cerr << "Error updating FREQUENCY_RANGE, updater is not HlaEmitterBeamUpdater";
			} },
			{ "PULSE_REPETITION_FREQUENCY", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPulseRepetitionFrequency(value);
				else
					std::cerr << "Error updating PULSE_REPETITION_FREQUENCY, updater is not HlaEmitterBeamUpdater";
			} },
			{ "PULSE_WIDTH", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setPulseWidth(value);
				else
					std::cerr << "Error updating PULSE_WIDTH, updater is not HlaEmitterBeamUpdater";
			} },
			{ "SWEEP_SYNCH", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaEmitterBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setSweepSynch(value);
				else
					std::cerr << "Error updating SWEEP_SYNCH, updater is not HlaEmitterBeamUpdater";
			} },
//pragma endregion
//pragma region RadarBeam
			{ "HIGH_DENSITY_TRACK", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHighDensityTrack(value);
				else
					std::cerr << "Error updating HIGH_DENSITY_TRACK, updater is not HlaRadarBeamUpdater";
			} },
			{ "TRACK_OBJECT_IDENTIFIERS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<std::string>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaRadarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setTrackObjectIdentifiers(value);
				else
					std::cerr << "Error updating TRACK_OBJECT_IDENTIFIERS, updater is not HlaRadarBeamUpdater";
			} },
//pragma endregion
//pragma region JammerBeam
			{ "JAMMING_MODE_SEQUENCE", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<unsigned int*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaJammerBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setJammingModeSequence(value);
				else
					std::cerr << "Error updating JAMMING_MODE_SEQUENCE, updater is not HlaJammerBeamUpdater";
			} },
			{ "JAMMED_OBJECT_IDENTIFIERS", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::vector<std::string>*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaJammerBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setJammedObjectIdentifiers(value);
				else
					std::cerr << "Error updating JAMMED_OBJECT_IDENTIFIERS, updater is not HlaJammerBeamUpdater";
			} },
			{ "HIGH_DENSITY_JAM", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<bool*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaJammerBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setHighDensityJam(value);
				else
					std::cerr << "Error updating HIGH_DENSITY_JAM, updater is not HlaJammerBeamUpdater";
			} },
//pragma endregion
//pragma region ACTIVE SONAR BEAM ATTRIBUTES
			{ "ACTIVE_EMISSION_PARAMETER_INDEX", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<short*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setActiveEmissionParameterIndex(value);
				else
					std::cerr << "Error updating ACTIVE_EMISSION_PARAMETER_INDEX, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "ACTIVE_SONAR_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<std::string*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setActiveSonarIdentifier(value);
				else
					std::cerr << "Error updating ACTIVE_SONAR_IDENTIFIER, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "AZIMUTH_BEAMWIDTH", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAzimuthBeamwidth(value);
				else
					std::cerr << "Error updating AZIMUTH_BEAMWIDTH, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "AZIMUTH_CENTER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setAzimuthCenter(value);
				else
					std::cerr << "Error updating AZIMUTH_CENTER, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "BEAM_IDENTIFIER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<char*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setBeamIdentifier(value);
				else
					std::cerr << "Error updating BEAM_IDENTIFIER, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "ELEVATION_BEAMWIDTH", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setElevationBeamwidth(value);
				else
					std::cerr << "Error updating ELEVATION_BEAMWIDTH, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "ELEVATION_CENTER", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<float*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setElevationCenter(value);
				else
					std::cerr << "Error updating ELEVATION_CENTER, updater is not HlaActiveSonarBeamUpdater";
			} },
			{ "SCAN_PATTERN", [](void* updater, DataTypes::AttributeValuePair attribute) {
				auto value = *static_cast<DevStudio::ActiveSonarScanPatternEnum::ActiveSonarScanPatternEnum*>(attribute.getData());
				if (auto castUpdater = dynamic_cast<DevStudio::HlaActiveSonarBeamUpdater*>((DevStudio::HlaHLAobjectRootUpdater*)updater))
					castUpdater->setScanPattern(value);
				else
					std::cerr << "Error updating SCAN_PATTERN, updater is not HlaActiveSonarBeamUpdater";
			} },
//pragma endregion
		};

		for (const auto& attribute : attributeVectorConst)
		{
			std::cout << "Update attribute " << attribute.getAttributeName() << std::endl;
			auto it = acciones.find(attribute.getAttributeName());
			if (it != acciones.end()) {
				it->second(updater, attribute);
			}
			else {
				std::cout << "Opci�n no reconocida\n";
			}

			std::string attributeName = attribute.getAttributeName();
		}
		std::cout << "End update attributes" << std::endl;
	};
}