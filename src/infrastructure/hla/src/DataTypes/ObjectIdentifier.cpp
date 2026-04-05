#include <tuple>
#include "DataTypes/ObjectIdentifier.h"
#include "DevStudio/datatypes/EntityIdentifierStruct.h"
#include "DevStudio/HlaWorld.h"

#include "DevStudio/HlaBaseEntityManager.h"
#include "DevStudio/HlaPhysicalEntityManager.h"
#include "DevStudio/HlaLifeformManager.h"
#include "DevStudio/HlaHumanManager.h"
#include "DevStudio/HlaNonHumanManager.h"
#include "DevStudio/HlaSensorManager.h"
#include "DevStudio/HlaCBRNDetectorManager.h"
#include "DevStudio/HlaCBRNSensorManager.h"
#include "DevStudio/HlaPlatformManager.h"
#include "DevStudio/HlaAircraftManager.h"
//#include "DevStudio/HlaTankerAircraftManager.h"
#include "DevStudio/HlaGroundVehicleManager.h"
#include "DevStudio/HlaSurfaceVesselManager.h"
#include "DevStudio/HlaAmphibiousVehicleManager.h"
#include "DevStudio/HlaMultiDomainPlatformManager.h"
#include "DevStudio/HlaSubmersibleVesselManager.h"
#include "DevStudio/HlaSpacecraftManager.h"
#include "DevStudio/HlaCulturalFeatureManager.h"
#include "DevStudio/HlaCOLPROManager.h"
#include "DevStudio/HlaDecontaminationStationManager.h"
#include "DevStudio/HlaMunitionManager.h"
#include "DevStudio/HlaExpendablesManager.h"
#include "DevStudio/HlaRadioManager.h"
#include "DevStudio/HlaSuppliesManager.h"
#include "DevStudio/HlaAggregateEntityManager.h"
#include "DevStudio/HlaEnvironmentalEntityManager.h"
#include "DevStudio/HlaEmbeddedSystemManager.h"
#include "DevStudio/HlaEmitterSystemManager.h"
#include "DevStudio/HlaRadarBeamManager.h"
#include "DevStudio/HlaJammerBeamManager.h"
#include "DevStudio/HlaDesignatorManager.h"

#include "DataTypes/Converters.h"

namespace SDL_RTI
{
    namespace DataTypes
    {
        ObjectIdentifier::ObjectIdentifier(DevStudio::HlaWorldPtr world, std::string identifier)
        {
            if (identifier.find("Lifeform") != std::string::npos)
            {
                auto entity = world->getHlaLifeformManager()->getLifeformByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Human") != std::string::npos)
            {
                auto entity = world->getHlaHumanManager()->getHumanByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("NonHuman") != std::string::npos)
            {
                auto entity = world->getHlaNonHumanManager()->getNonHumanByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Sensor") != std::string::npos)
            {
                auto entity = world->getHlaSensorManager()->getSensorByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("CBRN_Detector") != std::string::npos)
            {
                auto entity = world->getHlaCBRNDetectorManager()->getCBRNDetectorByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("CBRN_Sensor") != std::string::npos)
            {
                auto entity = world->getHlaCBRNSensorManager()->getCBRNSensorByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Aircraft") != std::string::npos)
            {
                auto entity = world->getHlaAircraftManager()->getAircraftByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            /*else if (identifier.find("TankerAircraft") != std::string::npos)
            {
                auto entity = world->getHlaTankerAircraftManager()->getTankerAircraftByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }*/
            else if (identifier.find("GroundVehicle") != std::string::npos)
            {
                auto entity = world->getHlaGroundVehicleManager()->getGroundVehicleByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("SurfaceVessel") != std::string::npos)
            {
                auto entity = world->getHlaSurfaceVesselManager()->getSurfaceVesselByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("AmphibiousVehicle") != std::string::npos)
            {
                auto entity = world->getHlaAmphibiousVehicleManager()->getAmphibiousVehicleByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("MultiDomainPlatform") != std::string::npos)
            {
                auto entity = world->getHlaMultiDomainPlatformManager()->getMultiDomainPlatformByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Spacecraft") != std::string::npos)
            {
                auto entity = world->getHlaSpacecraftManager()->getSpacecraftByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("SubmersibleVessel") != std::string::npos)
            {
                auto entity = world->getHlaSubmersibleVesselManager()->getSubmersibleVesselByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("CulturalFeature") != std::string::npos)
            {
                auto entity = world->getHlaCulturalFeatureManager()->getCulturalFeatureByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("COLPRO") != std::string::npos)
            {
                auto entity = world->getHlaCOLPROManager()->getCOLPROByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("DecontaminationStation") != std::string::npos)
            {
                auto entity = world->getHlaDecontaminationStationManager()->getDecontaminationStationByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Munition") != std::string::npos)
            {
                auto entity = world->getHlaMunitionManager()->getMunitionByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Expendables") != std::string::npos)
            {
                auto entity = world->getHlaExpendablesManager()->getExpendablesByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Radio") != std::string::npos)
            {
                auto entity = world->getHlaRadioManager()->getRadioByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("Supplies") != std::string::npos)
            {
                auto entity = world->getHlaSuppliesManager()->getSuppliesByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("AggregateEntity") != std::string::npos)
            {
                auto entity = world->getHlaAggregateEntityManager()->getAggregateEntityByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("EnvironmentalEntity") != std::string::npos)
            {
                auto entity = world->getHlaEnvironmentalEntityManager()->getEnvironmentalEntityByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = entity->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(entity->getEntityIdentifier());
            }
            else if (identifier.find("EmitterSystem") != std::string::npos)
            {
                auto emitter = world->getHlaEmitterSystemManager()->getEmitterSystemByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = emitter->getClassType();
                entityIdentifier = new std::tuple<char, DevStudio::EntityIdentifierStruct>(emitter->getEmitterIndex(), emitter->getEntityIdentifier());
            }
            else if (identifier.find("RadarBeam") != std::string::npos)
            {
                auto radarBeam = world->getHlaRadarBeamManager()->getRadarBeamByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                auto allEmitterSystem = world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems();
                for (auto& emitterSystem : allEmitterSystem)
                {
                    if(SDL_RTI::Converters::convert_wstring_to_string(emitterSystem->getHlaInstanceName()) == radarBeam->getEmitterSystemIdentifier())
                    {
                        entityIdentifier = new std::tuple<char, char, DevStudio::EntityIdentifierStruct>(radarBeam->getBeamIdentifier(), emitterSystem->getEmitterIndex(), emitterSystem->getEntityIdentifier());
                    }
                    else {
                        entityIdentifier = nullptr;
                        std::cout << "Emitter system identifier from Radar Beam doesn`t exist in Emitters List" << std::endl;
                    }
                }
                objectType = radarBeam->getClassType();
                
            }
            else if (identifier.find("JammerBeam") != std::string::npos)
            {
                auto jammerBeam = world->getHlaJammerBeamManager()->getJammerBeamByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                auto allEmitterSystem = world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems();
                for (auto& emitterSystem : allEmitterSystem)
                {
                    if (SDL_RTI::Converters::convert_wstring_to_string(emitterSystem->getHlaInstanceName()) == jammerBeam->getEmitterSystemIdentifier())
                    {
                        entityIdentifier = new std::tuple<char, char, DevStudio::EntityIdentifierStruct>(jammerBeam->getBeamIdentifier(), emitterSystem->getEmitterIndex(), emitterSystem->getEntityIdentifier());
                    
                    }
                    else {
                        entityIdentifier = nullptr;
                        std::cout << "Emitter system identifier from Jammer Beam doesn`t exist in Emitters List" << std::endl;
                    }
                }
                objectType = jammerBeam->getClassType();

            }
            else if (identifier.find("Designator") != std::string::npos)
            {
                auto designator = world->getHlaDesignatorManager()->getDesignatorByHlaInstanceName(SDL_RTI::Converters::convert_string_to_wstring(identifier));
                objectType = designator->getClassType();
                entityIdentifier = new DevStudio::EntityIdentifierStruct(designator->getEntityIdentifier());
            }
            else
            {
                std::cout << "Unknown object type" << std::endl;
                objectType = HlaObjectInstanceBase::ObjectClassType::HLAOBJECT_ROOT;
                entityIdentifier = nullptr;

            }
        }
        ObjectIdentifier::ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, EntityIdentifierStruct entityIdentifier_) : objectType(objectType_)
        {
            entityIdentifier = new DevStudio::EntityIdentifierStruct(entityIdentifier_);
            objectType = objectType_;
        }
		ObjectIdentifier::ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, char index, EntityIdentifierStruct entityIdentifier_) : objectType(objectType_)
		{
			entityIdentifier = new std::tuple<char, DevStudio::EntityIdentifierStruct>(index, entityIdentifier_);
            objectType = objectType_;
		}

        ObjectIdentifier::ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, char beamIdentifier, char emitterIndex, EntityIdentifierStruct entityIdentifier_)
        {
            entityIdentifier = new std::tuple<char, char, DevStudio::EntityIdentifierStruct>(beamIdentifier, emitterIndex, entityIdentifier_);
            objectType = objectType_;
        }

        std::string ObjectIdentifier::GetObjectIdentifierString(DevStudio::HlaWorldPtr world)
        {
            if (objectType >= HlaObjectInstanceBase::ObjectClassType::BASE_ENTITY && objectType <= HlaObjectInstanceBase::ObjectClassType::ENVIRONMENTAL_ENTITY)
            {
                // get EntityIdStruct from m_objectIdentifier
                auto entityIdStruct = static_cast<DevStudio::EntityIdentifierStruct*>(entityIdentifier);
                switch (objectType)
                {
                case DevStudio::HlaObjectInstanceBase::BASE_ENTITY:
                    for (auto baseEntity : world->getHlaBaseEntityManager()->getAllHlaBaseEntitys())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::PHYSICAL_ENTITY:
                    for (auto baseEntity : world->getHlaPhysicalEntityManager()->getAllHlaPhysicalEntitys())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::LIFEFORM:
                    for (auto baseEntity : world->getHlaLifeformManager()->getAllHlaLifeforms())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::HUMAN:
                    for (auto baseEntity : world->getHlaHumanManager()->getAllHlaHumans())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::NON_HUMAN:
                    for (auto baseEntity : world->getHlaNonHumanManager()->getAllHlaNonHumans())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::SENSOR:
                    for (auto baseEntity : world->getHlaSensorManager()->getAllHlaSensors())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::CBRNDETECTOR:
                    for (auto baseEntity : world->getHlaCBRNDetectorManager()->getAllHlaCBRNDetectors())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::CBRNSENSOR:
                    for (auto baseEntity : world->getHlaCBRNSensorManager()->getAllHlaCBRNSensors())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::PLATFORM:
                    for (auto baseEntity : world->getHlaPlatformManager()->getAllHlaPlatforms())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::AIRCRAFT:
                    for (auto baseEntity : world->getHlaAircraftManager()->getAllHlaAircrafts())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                /*case DevStudio::HlaObjectInstanceBase::TANKER_AIRCRAFT:
                    for (auto baseEntity : world->getHlaTankerAircraftManager()->getAllHlaTankerAircrafts())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;*/
                case DevStudio::HlaObjectInstanceBase::GROUND_VEHICLE:
                    for (auto baseEntity : world->getHlaGroundVehicleManager()->getAllHlaGroundVehicles())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::SURFACE_VESSEL:
                    for (auto baseEntity : world->getHlaSurfaceVesselManager()->getAllHlaSurfaceVessels())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::AMPHIBIOUS_VEHICLE:
                    for (auto baseEntity : world->getHlaAmphibiousVehicleManager()->getAllHlaAmphibiousVehicles())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::MULTI_DOMAIN_PLATFORM:
                    for (auto baseEntity : world->getHlaMultiDomainPlatformManager()->getAllHlaMultiDomainPlatforms())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::SPACECRAFT:
                    for (auto baseEntity : world->getHlaSpacecraftManager()->getAllHlaSpacecrafts())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::SUBMERSIBLE_VESSEL:
                    for (auto baseEntity : world->getHlaSubmersibleVesselManager()->getAllHlaSubmersibleVessels())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::CULTURAL_FEATURE:
                    for (auto baseEntity : world->getHlaCulturalFeatureManager()->getAllHlaCulturalFeatures())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::COLPRO:
                    for (auto baseEntity : world->getHlaCOLPROManager()->getAllHlaCOLPROs())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::DECONTAMINATION_STATION:
                    for (auto baseEntity : world->getHlaDecontaminationStationManager()->getAllHlaDecontaminationStations())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::MUNITION:
                    for (auto baseEntity : world->getHlaMunitionManager()->getAllHlaMunitions())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::EXPENDABLES:
                    for (auto baseEntity : world->getHlaExpendablesManager()->getAllHlaExpendables())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::RADIO:
                    for (auto baseEntity : world->getHlaRadioManager()->getAllHlaRadios())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::SUPPLIES:
                    for (auto baseEntity : world->getHlaSuppliesManager()->getAllHlaSupplies())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::AGGREGATE_ENTITY:
                    for (auto baseEntity : world->getHlaAggregateEntityManager()->getAllHlaAggregateEntitys())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                case DevStudio::HlaObjectInstanceBase::ENVIRONMENTAL_ENTITY:
                    for (auto baseEntity : world->getHlaEnvironmentalEntityManager()->getAllHlaEnvironmentalEntitys())
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return SDL_RTI::Converters::convert_wstring_to_string(baseEntity->getHlaInstanceName());
                    break;
                default:
                {
                    std::cerr << "ObjectIdentifier::GetObjectIdentifierString: Unsupported object type: " << static_cast<int>(objectType) << std::endl;
                    return "";
                }
                }
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::EMITTER_SYSTEM)
            {
                auto emitterId = static_cast<std::tuple<char, DevStudio::EntityIdentifierStruct>*>(entityIdentifier);
                for (auto emitter : world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems())
					if (emitter->getEntityIdentifier().federateIdentifier.siteID == std::get<1>(*emitterId).federateIdentifier.siteID &&
						emitter->getEntityIdentifier().federateIdentifier.applicationID == std::get<1>(*emitterId).federateIdentifier.applicationID &&
						emitter->getEntityIdentifier().entityNumber == std::get<1>(*emitterId).entityNumber &&
                        emitter->getEmitterIndex() == std::get<0>(*emitterId))
                        return SDL_RTI::Converters::convert_wstring_to_string(emitter->getHlaInstanceName());
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::RADAR_BEAM)
            {
                auto radarBeamId = static_cast<std::tuple<char, char, DevStudio::EntityIdentifierStruct>*>(entityIdentifier);
                for (auto emitter : world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems())
                {
                    for(auto radarBeam : world->getHlaRadarBeamManager()->getAllHlaRadarBeams())
                    {
                        if (emitter->getEntityIdentifier().federateIdentifier.siteID == std::get<2>(*radarBeamId).federateIdentifier.siteID &&
                            emitter->getEntityIdentifier().federateIdentifier.applicationID == std::get<2>(*radarBeamId).federateIdentifier.applicationID &&
                            emitter->getEntityIdentifier().entityNumber == std::get<2>(*radarBeamId).entityNumber &&
                            emitter->getEmitterIndex() == std::get<1>(*radarBeamId) && radarBeam->getBeamIdentifier() == std::get<0>(*radarBeamId))
                            return SDL_RTI::Converters::convert_wstring_to_string(radarBeam->getHlaInstanceName());
                    }
                }
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::JAMMER_BEAM)
            {
                auto jammerBeamId = static_cast<std::tuple<char, char, DevStudio::EntityIdentifierStruct>*>(entityIdentifier);
                for (auto emitter : world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems())
                {
                    for (auto jammerBeam : world->getHlaJammerBeamManager()->getAllHlaJammerBeams())
                    {
                        if (emitter->getEntityIdentifier().federateIdentifier.siteID == std::get<2>(*jammerBeamId).federateIdentifier.siteID &&
                            emitter->getEntityIdentifier().federateIdentifier.applicationID == std::get<2>(*jammerBeamId).federateIdentifier.applicationID &&
                            emitter->getEntityIdentifier().entityNumber == std::get<2>(*jammerBeamId).entityNumber &&
                            emitter->getEmitterIndex() == std::get<1>(*jammerBeamId) && jammerBeam->getBeamIdentifier() == std::get<0>(*jammerBeamId))
                            return SDL_RTI::Converters::convert_wstring_to_string(jammerBeam->getHlaInstanceName());
                    }
                }
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::DESIGNATOR)
            {
                auto designatorId = static_cast<DevStudio::EntityIdentifierStruct*>(entityIdentifier);
                for (auto designator : world->getHlaDesignatorManager()->getAllHlaDesignators())
                    if (designator->getEntityIdentifier().federateIdentifier.siteID == designatorId->federateIdentifier.siteID &&
                        designator->getEntityIdentifier().federateIdentifier.applicationID == designatorId->federateIdentifier.applicationID &&
                        designator->getEntityIdentifier().entityNumber == designatorId->entityNumber)
                        return SDL_RTI::Converters::convert_wstring_to_string(designator->getHlaInstanceName());
            }
            std::cerr << "ObjectIdentifier::GetObjectIdentifierString: Unsupported object type: " << static_cast<int>(objectType) << std::endl;
            return "";
        }

        std::shared_ptr<void> ObjectIdentifier::GetHlaObject(DevStudio::HlaWorldPtr world) const
        {
            if (objectType >= HlaObjectInstanceBase::ObjectClassType::BASE_ENTITY && objectType <= HlaObjectInstanceBase::ObjectClassType::ENVIRONMENTAL_ENTITY)
            {
                auto entityIdStruct = static_cast<DevStudio::EntityIdentifierStruct*>(entityIdentifier);
                switch (objectType)
                {
                case DevStudio::HlaObjectInstanceBase::BASE_ENTITY:
                    for (auto& baseEntity : world->getHlaBaseEntityManager()->getAllHlaBaseEntitys())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == baseEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == baseEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == baseEntity->getEntityIdentifier().entityNumber)
                            return baseEntity;
					}
                    break;
                case DevStudio::HlaObjectInstanceBase::PHYSICAL_ENTITY:
                    for (auto& physicalEntity : world->getHlaPhysicalEntityManager()->getAllHlaPhysicalEntitys())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == physicalEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == physicalEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == physicalEntity->getEntityIdentifier().entityNumber)
                            return physicalEntity;
					}
                    break;
                case DevStudio::HlaObjectInstanceBase::LIFEFORM:
                {
                    for (auto& lifeform : world->getHlaLifeformManager()->getAllHlaLifeforms())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == lifeform->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == lifeform->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == lifeform->getEntityIdentifier().entityNumber)
                            return lifeform;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::HUMAN:
                {
                    for (auto& human : world->getHlaHumanManager()->getAllHlaHumans())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == human->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == human->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == human->getEntityIdentifier().entityNumber)
                            return human;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::NON_HUMAN:
                {
                    for (auto& nonHuman : world->getHlaNonHumanManager()->getAllHlaNonHumans())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == nonHuman->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == nonHuman->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == nonHuman->getEntityIdentifier().entityNumber)
                            return nonHuman;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::SENSOR:
                {
                    for (auto& sesor : world->getHlaSensorManager()->getAllHlaSensors())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == sesor->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == sesor->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == sesor->getEntityIdentifier().entityNumber)
                            return sesor;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::CBRNDETECTOR:
                {
                    for (auto& CBRNDetector : world->getHlaCBRNDetectorManager()->getAllHlaCBRNDetectors())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == CBRNDetector->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == CBRNDetector->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == CBRNDetector->getEntityIdentifier().entityNumber)
                            return CBRNDetector;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::CBRNSENSOR:
                {
                    for (auto& CBRNSensor : world->getHlaCBRNSensorManager()->getAllHlaCBRNSensors())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == CBRNSensor->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == CBRNSensor->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == CBRNSensor->getEntityIdentifier().entityNumber)
                            return CBRNSensor;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::PLATFORM:
                {
                    for (auto& platform : world->getHlaPlatformManager()->getAllHlaPlatforms())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == platform->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == platform->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == platform->getEntityIdentifier().entityNumber)
                            return platform;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::AIRCRAFT:
                {
                    for (auto& aircraft : world->getHlaAircraftManager()->getAllHlaAircrafts())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == aircraft->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == aircraft->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == aircraft->getEntityIdentifier().entityNumber)
                            return aircraft;
                    }
                    break;
                }
                /*case DevStudio::HlaObjectInstanceBase::TANKER_AIRCRAFT:
                {
                    for (auto& tankerAircraft : world->getHlaTankerAircraftManager()->getAllHlaTankerAircrafts())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == tankerAircraft->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == tankerAircraft->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == tankerAircraft->getEntityIdentifier().entityNumber)
                            return tankerAircraft;
                    }
                    break;
                }*/
                case DevStudio::HlaObjectInstanceBase::GROUND_VEHICLE:
                {
                    for (auto& groundVehicle : world->getHlaGroundVehicleManager()->getAllHlaGroundVehicles())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == groundVehicle->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == groundVehicle->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == groundVehicle->getEntityIdentifier().entityNumber)
                            return groundVehicle;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::SURFACE_VESSEL:
                {
                    for (auto& srfaceVessel : world->getHlaSurfaceVesselManager()->getAllHlaSurfaceVessels())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == srfaceVessel->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == srfaceVessel->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == srfaceVessel->getEntityIdentifier().entityNumber)
                            return srfaceVessel;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::AMPHIBIOUS_VEHICLE:
                {
                    for (auto& amphibiousVehicle : world->getHlaAmphibiousVehicleManager()->getAllHlaAmphibiousVehicles())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == amphibiousVehicle->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == amphibiousVehicle->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == amphibiousVehicle->getEntityIdentifier().entityNumber)
                            return amphibiousVehicle;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::MULTI_DOMAIN_PLATFORM:
                {
                    for (auto& multiDomainPlatform : world->getHlaMultiDomainPlatformManager()->getAllHlaMultiDomainPlatforms())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == multiDomainPlatform->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == multiDomainPlatform->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == multiDomainPlatform->getEntityIdentifier().entityNumber)
                            return multiDomainPlatform;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::SPACECRAFT:
                {
                    for (auto& spacecraft : world->getHlaSpacecraftManager()->getAllHlaSpacecrafts())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == spacecraft->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == spacecraft->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == spacecraft->getEntityIdentifier().entityNumber)
                            return spacecraft;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::SUBMERSIBLE_VESSEL:
                {
                    for (auto& submersibleVessel : world->getHlaSubmersibleVesselManager()->getAllHlaSubmersibleVessels())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == submersibleVessel->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == submersibleVessel->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == submersibleVessel->getEntityIdentifier().entityNumber)
                            return submersibleVessel;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::CULTURAL_FEATURE:
                {
                    for (auto& culturalFeature : world->getHlaCulturalFeatureManager()->getAllHlaCulturalFeatures())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == culturalFeature->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == culturalFeature->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == culturalFeature->getEntityIdentifier().entityNumber)
                            return culturalFeature;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::COLPRO:
                {
                    for (auto& colpro : world->getHlaCOLPROManager()->getAllHlaCOLPROs())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == colpro->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == colpro->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == colpro->getEntityIdentifier().entityNumber)
                            return colpro;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::DECONTAMINATION_STATION:
                {
                    for (auto& decontaminationStation : world->getHlaDecontaminationStationManager()->getAllHlaDecontaminationStations())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == decontaminationStation->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == decontaminationStation->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == decontaminationStation->getEntityIdentifier().entityNumber)
                            return decontaminationStation;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::MUNITION:
                {
                    for (auto& munition : world->getHlaMunitionManager()->getAllHlaMunitions())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == munition->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == munition->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == munition->getEntityIdentifier().entityNumber)
                            return munition;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::EXPENDABLES:
                {
                    for (auto& expendables : world->getHlaExpendablesManager()->getAllHlaExpendables())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == expendables->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == expendables->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == expendables->getEntityIdentifier().entityNumber)
                            return expendables;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::RADIO:
                {
                    for (auto& radio : world->getHlaRadioManager()->getAllHlaRadios())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == radio->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == radio->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == radio->getEntityIdentifier().entityNumber)
                            return radio;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::SUPPLIES:
                {
                    for (auto& supplies : world->getHlaSuppliesManager()->getAllHlaSupplies())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == supplies->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == supplies->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == supplies->getEntityIdentifier().entityNumber)
                            return supplies;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::AGGREGATE_ENTITY:
                {
                    for (auto& aggregateEntity : world->getHlaAggregateEntityManager()->getAllHlaAggregateEntitys())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == aggregateEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == aggregateEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == aggregateEntity->getEntityIdentifier().entityNumber)
                            return aggregateEntity;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::ENVIRONMENTAL_ENTITY:
                {
                    for (auto& environmentalEntity : world->getHlaEnvironmentalEntityManager()->getAllHlaEnvironmentalEntitys())
                    {
                        if (entityIdStruct->federateIdentifier.siteID == environmentalEntity->getEntityIdentifier().federateIdentifier.siteID &&
                            entityIdStruct->federateIdentifier.applicationID == environmentalEntity->getEntityIdentifier().federateIdentifier.applicationID &&
                            entityIdStruct->entityNumber == environmentalEntity->getEntityIdentifier().entityNumber)
                            return environmentalEntity;
                    }
                    break;
                }
                case DevStudio::HlaObjectInstanceBase::DESIGNATOR:
                {
                    auto designatorId = static_cast<DevStudio::EntityIdentifierStruct*>(entityIdentifier);
                    for (auto designator : world->getHlaDesignatorManager()->getAllHlaDesignators())
                        if (designator->getEntityIdentifier().federateIdentifier.siteID == designatorId->federateIdentifier.siteID &&
                            designator->getEntityIdentifier().federateIdentifier.applicationID == designatorId->federateIdentifier.applicationID &&
                            designator->getEntityIdentifier().entityNumber == designatorId->entityNumber)
                            return designator;
                    break;
				}
                default:
                {
                    std::cerr << "ObjectIdentifier::GetHlaObject: Unsupported object type: " << static_cast<int>(objectType) << std::endl;
					return nullptr;
                }
                }
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::EMITTER_SYSTEM)
            {
                auto emitterId = static_cast<std::tuple<char, DevStudio::EntityIdentifierStruct>*>(entityIdentifier);
                for (auto emitter : world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems())
                    if (emitter->getEntityIdentifier().federateIdentifier.siteID == std::get<1>(*emitterId).federateIdentifier.siteID &&
                        emitter->getEntityIdentifier().federateIdentifier.applicationID == std::get<1>(*emitterId).federateIdentifier.applicationID &&
                        emitter->getEntityIdentifier().entityNumber == std::get<1>(*emitterId).entityNumber &&
                        emitter->getEmitterIndex() == std::get<0>(*emitterId))
                        return emitter;
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::RADAR_BEAM)
            {
                auto radarBeamId = static_cast<std::tuple<char, char, DevStudio::EntityIdentifierStruct>*>(entityIdentifier);
                for (auto emitter : world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems())
                {
                    for (auto radarBeam : world->getHlaRadarBeamManager()->getAllHlaRadarBeams())
                    {
                        if (emitter->getEntityIdentifier().federateIdentifier.siteID == std::get<2>(*radarBeamId).federateIdentifier.siteID &&
                            emitter->getEntityIdentifier().federateIdentifier.applicationID == std::get<2>(*radarBeamId).federateIdentifier.applicationID &&
                            emitter->getEntityIdentifier().entityNumber == std::get<2>(*radarBeamId).entityNumber &&
                            emitter->getEmitterIndex() == std::get<1>(*radarBeamId) && radarBeam->getBeamIdentifier() == std::get<0>(*radarBeamId))
                            return radarBeam; 
                    }
                }
            }
            else if (objectType == HlaObjectInstanceBase::ObjectClassType::JAMMER_BEAM)
            {
                auto jammerBeamId = static_cast<std::tuple<char, char, DevStudio::EntityIdentifierStruct>*>(entityIdentifier);
                for (auto emitter : world->getHlaEmitterSystemManager()->getAllHlaEmitterSystems())
                {
                    for (auto jammerBeam : world->getHlaJammerBeamManager()->getAllHlaJammerBeams())
                    {
                        if (emitter->getEntityIdentifier().federateIdentifier.siteID == std::get<2>(*jammerBeamId).federateIdentifier.siteID &&
                            emitter->getEntityIdentifier().federateIdentifier.applicationID == std::get<2>(*jammerBeamId).federateIdentifier.applicationID &&
                            emitter->getEntityIdentifier().entityNumber == std::get<2>(*jammerBeamId).entityNumber &&
                            emitter->getEmitterIndex() == std::get<1>(*jammerBeamId) && jammerBeam->getBeamIdentifier() == std::get<0>(*jammerBeamId))
                            return jammerBeam;
                    }
                }
            }
			std::cerr << "ObjectIdentifier::GetHlaObject: Unsupported object type: " << static_cast<int>(objectType) << std::endl;
			return nullptr;
        }
    }
}