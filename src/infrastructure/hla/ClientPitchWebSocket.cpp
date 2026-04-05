#include "ClientPitchWebSocket.h"

#include <DevStudio/HlaPointers.h>
#include <DevStudio/HlaWorld.h>
#include <DevStudio/HlaWorldImpl.h>
#include <DevStudio/ListenerSet.h>
#include <DevStudio/HlaAircraftManager.h>
#include <DevStudio/HlaInteractionManager.h>
#include <DevStudio/HlaAircraftImpl.h>
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/ImplPointers.h>
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>
#include <DevStudio/FederateManager.h>
#include <DevStudio/HlaPhysicalEntityAttributesImpl.h>
#include <DevStudio/HlaBaseEntityAttributesImpl.h>
#include <DevStudio/HlaBaseEntityAttributes.h>
#include <DevStudio/HlaHLAfederateAttributes.h>

#include <DevStudio/HlaPhysicalEntityImpl.h>
#include <DevStudio/HlaPlatformAttributesImpl.h>
#include <DevStudio/HlaAircraftAttributesImpl.h>
#include <DevStudio/HlaRadarBeamAttributesImpl.h>
#include <DevStudio/HlaJammerBeamAttributesImpl.h>
#include <DevStudio/HlaHLAfederateAttributesImpl.h>
#include <DevStudio/datatypes/EntityTypeStruct.h>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>

#include "include/ObjectManagerListeners.h"
#include "include/events/EventBus.h"
#include "include/events/EventSource.h"
#include "include/utils/LoggerManager.h"
#include "include/utils/Singleton.h"
#include "include/events/EventOnDataAvailableObject.h"

#include <memory>

using namespace SDL_RTI;

ClientPitchWebSocket::ClientPitchWebSocket()
{
    m_RequestID = 0;
    m_IdAll = -1;
    m_WebsocketServer = std::make_unique<Websocketserver>(this);
    m_WebsocketServer->initServer();
}

ClientPitchWebSocket::~ClientPitchWebSocket()
{

}

void ClientPitchWebSocket::initConnect(const RequestClientHLAProto& requestClientHla)
{
    initFederation(requestClientHla.federation(), requestClientHla.federate(), requestClientHla.federatetype(), 0, 0, requestClientHla.ip(), requestClientHla.port());
}

void ClientPitchWebSocket::finishConnect()
{
    finishedFederation();
}

void ClientPitchWebSocket::handleMagicAction(const CreateEntityMagicActionProto& action)
{

    unsigned int type = action.type();
    int id = action.id();
    std::string marking = action.marking();
    int team = action.team();
    const float latitude = action.latitude();
    const float longitude = action.longitude();
    const float altitude = action.altitude();
    const float heading = action.heading();
    float speed = action.speed();

    std::vector<DataTypes::AttributeValuePair> attributeVector;
    DevStudio::SpatialFPStruct spatialData;

    RprUtility::WorldLocation WorldLocation = RprUtility::createWorldLocationFromGeodeticLocation(RprUtility::GeodeticLocation{latitude * (3.14159265358979323846 / 180.0), longitude * (3.14159265358979323846 / 180.0), altitude * 0.3048});

    spatialData.worldLocation.x = WorldLocation.x;
    spatialData.worldLocation.y = WorldLocation.y;
    spatialData.worldLocation.z = WorldLocation.z;
    spatialData.isFrozen = false;
    spatialData.orientation = { heading, 0.0f, 0.0f };
    spatialData.velocityVector = { speed, 0.0f, 0.0f };
    DevStudio::SpatialVariantStruct variantSpatialData = DevStudio::SpatialVariantStruct::createSpatialFPB(spatialData);

    RprUtility::SpatialStruct pitchSpatialData = DevStudio::SpatialVariantStruct::convert(variantSpatialData);

    auto entityType = DevStudio::EntityTypeStruct(1, 2, 71, 1, 5, 10, 1);
    auto entityIdentifier = DevStudio::EntityIdentifierStruct{
        DevStudio::FederateIdentifierStruct(static_cast<unsigned short>(1), static_cast<unsigned short>(2)),
        static_cast<unsigned short>(id) };

    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::SPATIAL, &pitchSpatialData));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::ENTITY_TYPE, &entityType));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::ENTITY_IDENTIFIER, &entityIdentifier));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::FORCE_IDENTIFIER, &team));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::LIVE_ENTITY_MEASURED_SPEED, &speed));

    DevStudio::MarkingStruct markingStruct;
    markingStruct.markingEncodingType = DevStudio::MarkingEncodingEnum::ASCII;
    std::string newMarkingString = marking;
    markingStruct.markingData.assign(newMarkingString.begin(), newMarkingString.end());
    while (markingStruct.markingData.size() < 11) {
        markingStruct.markingData.push_back(' ');
    }

    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::MARKING, &markingStruct));
    auto damageState = DevStudio::DamageStatusEnum::NO_DAMAGE;
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::DAMAGE_STATE, &damageState));
    auto falseVar = false;
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::AFTERBURNER_ON, &falseVar));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::NAVIGATION_LIGHTS_ON, &falseVar));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::LANDING_LIGHTS_ON, &falseVar));
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::ENGINE_SMOKE_ON, &falseVar));

    ClientPitch::sendCreateObjectRequest(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), attributeVector);

    std::cout << "handleMagicAction Pre-create: " << std::endl;
}

void ClientPitchWebSocket::handleRemoveEntityMagicAction(const RemoveEntityMagicActionProto& action)
{
    std::vector<DataTypes::ObjectIdentifier> objectIdentifiers;
    DevStudio::FederateIdentifierStruct federateId(static_cast<unsigned short>(std::stoi(action.entityidentifier().federateidentifier().siteid())), static_cast<unsigned short>(std::stoi(action.entityidentifier().federateidentifier().applicationid())));
    DevStudio::EntityIdentifierStruct entityId(federateId, static_cast<unsigned short>(std::stoi(action.entityidentifier().entitynumber()))); 
    unsigned int type = action.type();

    DataTypes::ObjectIdentifier entityToRemove((static_cast<HlaObjectInstanceBase::ObjectClassType>(type)), entityId);

    objectIdentifiers.push_back(entityToRemove);

    ClientPitch::sendRemoveObjectsRequest(objectIdentifiers);
}

void ClientPitchWebSocket::handleDestroyEntityMagicAction(const DestroyEntityProto& action)
{
    DevStudio::FederateIdentifierStruct federateId(static_cast<unsigned short>(std::stoi(action.entityidentifier().federateidentifier().siteid())), static_cast<unsigned short>(std::stoi(action.entityidentifier().federateidentifier().applicationid())));
    DevStudio::EntityIdentifierStruct entityId(federateId, static_cast<unsigned short>(std::stoi(action.entityidentifier().entitynumber()))); 
    unsigned int type = action.type();
    std::vector<DataTypes::ObjectIdentifier> entitiesList;
    std::vector<DataTypes::AttributeValuePair> attributeVector;

    DataTypes::ObjectIdentifier entityToDestroy((static_cast<HlaObjectInstanceBase::ObjectClassType>(type)), entityId);
    entitiesList.push_back(entityToDestroy);

    DevStudio::DamageStatusEnum::DamageStatusEnum damage = DevStudio::DamageStatusEnum::DamageStatusEnum::DESTROYED;
    attributeVector.push_back(DataTypes::AttributeValuePair(static_cast<HlaObjectInstanceBase::ObjectClassType>(type), DevStudio::HlaAircraftAttributes::Attribute::DAMAGE_STATE, &damage));

    ClientPitch::sendAttributeChangeRequest(entitiesList, attributeVector);
}

void SDL_RTI::ClientPitchWebSocket::handleMagicMoveAction(const MagicMoveProto &action)
{
    double x, y, z;
    DevStudio::FederateIdentifierStruct federateId(static_cast<unsigned short>(std::stoi(action.entityidentifier().federateidentifier().siteid())), static_cast<unsigned short>(std::stoi(action.entityidentifier().federateidentifier().applicationid())));
    DevStudio::EntityIdentifierStruct entityId(federateId, static_cast<unsigned short>(std::stoi(action.entityidentifier().entitynumber()))); 
    TaskParams task(entityId);
    SDL_RTI::Converters::convert_latlonalt_to_xyz(action.latitude(), action.longitude(), action.altitude(), x, y, z);
    DevStudio::LocationStruct location(x, y, z);
    DevStudio::MagicMoveTaskStruct magicMoveParams(location, action.heading());

    ClientPitch::sendMagicMove(task, magicMoveParams);
}

void ClientPitchWebSocket::handlePlayResume()
{
    DevStudio::EntityIdentifierStruct originatingEntity;
    originatingEntity.entityNumber = 0;
    originatingEntity.federateIdentifier.applicationID = 0;
    originatingEntity.federateIdentifier.siteID = 0;

    DevStudio::EntityIdentifierStruct receivingEntity;
    receivingEntity.entityNumber = 0;
    receivingEntity.federateIdentifier.applicationID = 0;
    receivingEntity.federateIdentifier.siteID = 0;

    DevStudio::ClockTimeStruct realWorldTimeStruct = {
    static_cast<int32_t>(std::floor(1)),
    static_cast<uint32_t>((1 - std::floor(1)) * 1e9)
    };

    DevStudio::ClockTimeStruct simulationTimeStruct = {
        static_cast<int32_t>(std::floor(1)),
        static_cast<uint32_t>((1 - std::floor(1)) * 1e9)
    };

    ClientPitch::sendStartResumeInteraction(originatingEntity, receivingEntity, realWorldTimeStruct, simulationTimeStruct);

    std::cout << "handlePlayResume preSend" << std::endl;
}

void ClientPitchWebSocket::handlePauseFreeze()
{
    
    DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum devStudioReason =
        static_cast<DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum>(2);


    // Crear las estructuras necesarias
    DevStudio::EntityIdentifierStruct originatingEntity;
    originatingEntity.entityNumber = m_IdAll;
    originatingEntity.federateIdentifier.applicationID = m_IdAll;
    originatingEntity.federateIdentifier.siteID = m_IdAll;

    DevStudio::EntityIdentifierStruct receivingEntity;
    receivingEntity.entityNumber = m_IdAll;
    receivingEntity.federateIdentifier.applicationID = m_IdAll;
    receivingEntity.federateIdentifier.siteID = m_IdAll;

    DevStudio::ClockTimeStruct realWorldTimeStruct = {
        static_cast<int32_t>(std::floor(1)),
        static_cast<uint32_t>((1 - std::floor(1)) * 1e9)
    };

    DevStudio::ClockTimeStruct simulationTimeStruct = {
        static_cast<int32_t>(std::floor(1)),
        static_cast<uint32_t>((1 - std::floor(1)) * 1e9)
    };

    bool reflectValues = true;
    bool runInternalSimulationClock = true;
    bool updateAttributes = true;
    ++m_RequestID;

    ClientPitch::sendStopFreezeInteraction(originatingEntity, receivingEntity, realWorldTimeStruct, devStudioReason, reflectValues, runInternalSimulationClock, updateAttributes);

    std::cout << "handlePlayPause preSend" << std::endl;
}

void ClientPitchWebSocket::handleStop()
{
    
    DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum devStudioReason =
        static_cast<DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum>(7);

    // Crear las estructuras necesarias
    DevStudio::EntityIdentifierStruct originatingEntity;
    originatingEntity.entityNumber = m_IdAll;
    originatingEntity.federateIdentifier.applicationID = m_IdAll;
    originatingEntity.federateIdentifier.siteID = m_IdAll;

    DevStudio::EntityIdentifierStruct receivingEntity;
    receivingEntity.entityNumber = m_IdAll;
    receivingEntity.federateIdentifier.applicationID = m_IdAll;
    receivingEntity.federateIdentifier.siteID = m_IdAll;

    DevStudio::ClockTimeStruct realWorldTimeStruct = {
        static_cast<int32_t>(std::floor(1)),
        static_cast<uint32_t>((1 - std::floor(1)) * 1e9)
    };

    DevStudio::ClockTimeStruct simulationTimeStruct = {
        static_cast<int32_t>(std::floor(1)),
        static_cast<uint32_t>((1 - std::floor(1)) * 1e9)
    };

    bool reflectValues = true;
    bool runInternalSimulationClock = true;
    bool updateAttributes = true;
    ++m_RequestID;

    ClientPitch::sendStopFreezeInteraction(originatingEntity, receivingEntity, realWorldTimeStruct, devStudioReason, reflectValues, runInternalSimulationClock, updateAttributes);

    std::cout << "handlePlayStio preSend" << std::endl;
}

void ClientPitchWebSocket::onEventReceived(const EventOnDataAvailableInteraction& event)
{
    std::cout << "Aquuiii " << std::endl;
    if (event.getEventOnDataAvailableInteraction()) {

        ProtoGeneration protoGen;
        switch (event.getInteractionEnum())
        {
            case enumsCommon_Client::InteractionEnum::NullInteractionEnum:
                //LogError
                break;

            case enumsCommon_Client::InteractionEnum::StartResumeInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaStartResumeParametersImpl> startResume = std::static_pointer_cast<DevStudio::HlaStartResumeParametersImpl>(event.getEventOnDataAvailableInteraction());

                std::cout << "Aquuiii StartResumeInteractionEnum " << std::endl;
                if (startResume)
                {
                      std::cout << "Aquuiii StartResumeInteractionEnum 2 " << std::endl;
                    Envelope envelope = protoGen.createProtoPlayResume();
                    m_WebsocketServer->enqueueSend(envelope);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::StartResumeAnswerInteractionEnum:
            {
                auto startResumeAnswer = std::static_pointer_cast<DevStudio::HlaStartResumeRParametersImpl>(event.getEventOnDataAvailableInteraction());
                if (startResumeAnswer)
                {
                    //receivedStartResumeAnswerInteraction(&startResumeAnswer);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::StopFreezeAnswerInteractionEnum:
            {
                auto stopFreezeAnswer = std::static_pointer_cast<DevStudio::HlaStopFreezeRParametersImpl>(event.getEventOnDataAvailableInteraction());
                if (stopFreezeAnswer)
                {
                  //  receivedStopFreezeAnswerInteraction(&stopFreezeAnswer);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::StopFreezeInteractionEnum:
            {
               DevStudio::HlaStopFreezeParametersPtr stopFreeze = std::static_pointer_cast<DevStudio::HlaStopFreezeParameters>(event.getEventOnDataAvailableInteraction());

               if (stopFreeze && stopFreeze->hasReason()) 
               {
                    auto reason = stopFreeze->getReason();
                    Envelope envelope;

                    switch (reason)
                    {
                        case StopFreezeReasonEnum::RECESS:
                        case StopFreezeReasonEnum::STOP_FOR_RESTART:
                        case StopFreezeReasonEnum::STOP_FOR_RESET:
                        case StopFreezeReasonEnum::ABORT_TRAINING_RESUME_TAC_OPS:
                            envelope = protoGen.createProtoPause();
                            m_WebsocketServer->enqueueSend(envelope);
                            break;

                        default: // TERMINATION, SYSTEM_FAILURE, SECURITY_VIOLATION, ENTITY_RECONSTITUTION, OTHER
                            envelope = protoGen.createProtoStop();
                            m_WebsocketServer->enqueueSend(envelope);
                            break;
                    }

                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::AcknowledgeInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaAcknowledgeParametersImpl> acknowledge = std::static_pointer_cast<DevStudio::HlaAcknowledgeParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (acknowledge)
                {
                  //  receivedAcknowledgeInteraction(&acknowledge);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::CreateEntityInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaCreateEntityParametersImpl> create = std::static_pointer_cast<DevStudio::HlaCreateEntityParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (create)
                {
                  //  receivedCreateInteraction(&create);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::RemoveEntityInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaRemoveEntityParametersImpl> remove = std::static_pointer_cast<DevStudio::HlaRemoveEntityParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (remove)
                {
                  //  receivedRemoveInteraction(&remove);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::CollisionInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaCollisionParametersImpl> collision = std::static_pointer_cast<DevStudio::HlaCollisionParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (collision)
                {
                   // receivedCollisionInteraction(&collision);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::MunitionDetonationInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaMunitionDetonationParametersImpl> munition = std::static_pointer_cast<DevStudio::HlaMunitionDetonationParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (munition)
                {
                 //   receivedMunitionDetonationInteraction(&munition);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::ResupplyCancelInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaResupplyCancelParametersImpl> cancel = std::static_pointer_cast<DevStudio::HlaResupplyCancelParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (cancel)
                {
                  //  receivedResupplyCancelInteraction(&cancel);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::ResupplyOfferInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaResupplyOfferParametersImpl> offer = std::static_pointer_cast<DevStudio::HlaResupplyOfferParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (offer)
                {
                  //  receivedResupplyOfferInteraction(&offer);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::ResupplyReceivedInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaResupplyReceivedParametersImpl> received = std::static_pointer_cast<DevStudio::HlaResupplyReceivedParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (received)
                {
                  //  receivedResupplyReceivedInteraction(&received);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::WeaponFireInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaWeaponFireParametersImpl> weapon = std::static_pointer_cast<DevStudio::HlaWeaponFireParametersImpl>(event.getEventOnDataAvailableInteraction());
                if (weapon)
                {
                  //  receivedWeaponFireInteraction(&weapon);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::CreateObjectRequestEnum:
            {
                auto request = std::static_pointer_cast<DevStudio::HlaCreateObjectRequestParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (request)
                {
                 //   receivedCreateObjectRequestInteraction(&request);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::RemoveObjectRequestEnum:
            {
                auto request = std::static_pointer_cast<DevStudio::HlaRemoveObjectRequestParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (request)
                {
                  //  receivedRemoveObjectRequestInteraction(&request);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::AttributeChangeRequestEnum:
            {
                auto request = std::static_pointer_cast<DevStudio::HlaAttributeChangeRequestParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (request)
                {
                    //receivedAttributeChangeRequestInteraction(&request);
                }
                break;
            }
        }
    }
}

void ClientPitchWebSocket::onEventReceived(const EventOnDataAvailableObject& event)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    if (!event.getEventOnDataAvailableObject()) return;

    // Lambda to extract entity data and create envelope
    auto makeEnvelope = [&](auto& entity, enumsCommon_Client::ActionObjectEnum actionEnum, int forceIdentifier, const std::string& marking) {
        DevStudio::WorldLocationStruct worldLocationStruct = converter.calculate_WorldLocation(entity.get()->getSpatial());
        RprUtility::GeodeticLocation geodeticLocation = RprUtility::createGeodeticLocationFromXYZ(worldLocationStruct.x, worldLocationStruct.y, worldLocationStruct.z);
        
        const double latitude = geodeticLocation.latitude * (180.0 / 3.14159265358979323846);
        const double longitude = geodeticLocation.longitude * (180.0 / 3.14159265358979323846);
        const double altitude = geodeticLocation.altitude * 3.28084;
        const int idEntity = entity.get()->getEntityIdentifier().getEntityNumber();
        
        const double heading = converter.calculate_Heading(entity.get()->getSpatial());
        const double pitch = converter.calculate_Pitch(entity.get()->getSpatial());
        const double roll = converter.calculate_Roll(entity.get()->getSpatial());
        int kind = entity.get()->getEntityType().getEntityKind();
        int domain = entity.get()->getEntityType().getDomain();
        int countryCode = entity.get()->getEntityType().getCountryCode();
        int category = entity.get()->getEntityType().getCategory();
        int subcategory = entity.get()->getEntityType().getSubcategory();
        int specific = entity.get()->getEntityType().getSpecific();
        int extra = entity.get()->getEntityType().getExtra();

        ProtoGeneration protoGen;
        return protoGen.createProtoPhysicalEntity(actionEnum, idEntity, latitude, longitude, altitude, heading, pitch, roll, forceIdentifier, kind, domain,countryCode,category,subcategory,specific,extra, marking);
    };

    // Lambda to extract beam data and create envelope
    auto makeEmitterBeamEnvelope = [&](auto& emitterSystem, auto& beam, enumsCommon_Client::ActionObjectEnum actionEnum, std::vector<std::string> objectIdentifiers) {
        const int emitterSystemId = emitterSystem.get()->getEmitterIndex();
        const int entityId = emitterSystem.get()->getEntityIdentifier().getEntityNumber();
        const int emitterBeamId = beam.get()->getBeamIdentifier();
        std::vector<int> trackedObjectIdentifiers = {};
        for(auto& stringTrack : objectIdentifiers)
        {
            auto it = m_MapObjects.find(stringTrack);
            if(it != m_MapObjects.end())
            {
                trackedObjectIdentifiers.push_back(it->second);
            }
        }
        const float beamAzimuthCenter = beam.get()->getBeamAzimuthCenter();
        const float beamAzimuthSweep = beam.get()->getBeamAzimuthSweep();
        const float beamElevationCenter = beam.get()->getBeamElevationCenter();
        const float beamElevationSweep = beam.get()->getBeamElevationSweep();
        const float sweepSynch = beam.get()->getSweepSynch();
        const float pulseRepetitionFrequency = beam.get()->getPulseRepetitionFrequency();

        ProtoGeneration protoGen;
        return protoGen.createProtoEmitterBeam(actionEnum, emitterSystemId, emitterBeamId, entityId, trackedObjectIdentifiers,
            beamAzimuthCenter, beamAzimuthSweep, beamElevationCenter, beamElevationSweep, sweepSynch, pulseRepetitionFrequency);
    };

    // Lambda for handling Radar/Jammer Beam object add/remove
    auto handleObject = [&](int objectId, const std::string& objectName, int action) {
        constexpr int ACTION_ADD = enumsCommon_Client::ActionObjectEnum::AddObject;
        constexpr int ACTION_REMOVE = enumsCommon_Client::ActionObjectEnum::RemoveObject;
        if (action == ACTION_ADD) {
            m_MapObjects.insert(std::pair<std::string, int>(objectName, objectId)); 
        }
        else if (action == ACTION_REMOVE) {
            auto it = m_MapObjects.find(objectName);
            if (it != m_MapObjects.end()) {
                m_MapObjects.erase(it);
            }
        }
    };

    switch (event.getObjectObject()) 
    {
        case enumsCommon_Client::ObjectEnum::NullObjectEnum: {
            LoggerManager::getInstance().notifyNewLogger("Invalid Object data types, use a correct Object type", enumsCommon_Client::ErrorLevel);
            break;
        }

        case enumsCommon_Client::ObjectEnum::FederateObjectEnum:{
            
            std::shared_ptr<DevStudio::HlaHLAfederateAttributesImpl> federate =  
            std::static_pointer_cast<DevStudio::HlaHLAfederateAttributesImpl>(event.getEventOnDataAvailableObject());

            if (!federate) return;

            if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                createMessageFederate(federate, enumsCommon_Client::ActionObjectEnum::AddObject);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
            {
                createMessageFederate(federate, enumsCommon_Client::ActionObjectEnum::UpdateObject);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
            {
                createMessageFederate(federate, enumsCommon_Client::ActionObjectEnum::RemoveObject);
            }

            break;
        }
        case enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum: {
            std::shared_ptr<DevStudio::HlaBaseEntityAttributesImpl> baseEntity =  
            std::static_pointer_cast<DevStudio::HlaBaseEntityAttributesImpl>(event.getEventOnDataAvailableObject());

            if (!baseEntity) return;

            handleObject(baseEntity.get()->getEntityIdentifier().getEntityNumber(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());
        
            //Utilizar un enum para las acciones(1->añadir, 2->modificar, 3->borrar)
            if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                int forceIdentifier = 0;
                std::string marking = "";
                Envelope envelope = makeEnvelope(baseEntity, enumsCommon_Client::ActionObjectEnum::AddObject, forceIdentifier, marking);
                m_MapProtoEntity.insert(std::make_pair(baseEntity.get()->getEntityIdentifier().getEntityNumber(), envelope));
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
            {
                auto it = m_MapProtoEntity.find(baseEntity.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) {
                        std::cout << "Entity not found" << std::endl;
                        return;
                }
                int forceIdentifier = 0;
                std::string marking = "";
                Envelope envelope = makeEnvelope(baseEntity, enumsCommon_Client::ActionObjectEnum::UpdateObject, forceIdentifier, marking);
                it->second = envelope;
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
            {
                auto it = m_MapProtoEntity.find(baseEntity.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) {
                        std::cout << "Entity not found" << std::endl;
                        return;
                }
                int forceIdentifier = 0;
                std::string marking = "";
                Envelope envelope = makeEnvelope(baseEntity, enumsCommon_Client::ActionObjectEnum::RemoveObject, forceIdentifier, marking);
                m_WebsocketServer->enqueueSend(envelope);
                m_MapProtoEntity.erase(it);
            }
        
            break;
        }
        case enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum:
        {
            std::shared_ptr<DevStudio::HlaPhysicalEntityImpl> physicalEntity = 
            std::static_pointer_cast<DevStudio::HlaPhysicalEntityImpl>(event.getEventOnDataAvailableObject());

            if (!physicalEntity) return;
            
            handleObject(physicalEntity.get()->getEntityIdentifier().getEntityNumber(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());

            if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                int forceIdentifier = physicalEntity.get()->getForceIdentifier();
                
                auto data = physicalEntity.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());

                Envelope envelope = makeEnvelope(physicalEntity, enumsCommon_Client::ActionObjectEnum::AddObject, forceIdentifier, marking);
                m_MapProtoEntity.insert(std::make_pair(physicalEntity.get()->getEntityIdentifier().getEntityNumber(), envelope));
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
            {
                auto it = m_MapProtoEntity.find(physicalEntity.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) 
                {
                    std::cout << "Entity not found" << std::endl;
                    return;
                }
                int forceIdentifier = physicalEntity.get()->getForceIdentifier();
                auto data = physicalEntity.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(physicalEntity, enumsCommon_Client::ActionObjectEnum::UpdateObject, forceIdentifier, marking);
                it->second = envelope;
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
            {
                auto it = m_MapProtoEntity.find(physicalEntity.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) 
                {
                    std::cout << "Entity not found" << std::endl;
                    return;
                }
                int forceIdentifier = physicalEntity.get()->getForceIdentifier();
                auto data = physicalEntity.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(physicalEntity, enumsCommon_Client::ActionObjectEnum::RemoveObject, forceIdentifier, marking);
                m_WebsocketServer->enqueueSend(envelope);
                m_MapProtoEntity.erase(it);
            }
            
            break;
        }
        case enumsCommon_Client::ObjectEnum::PlatformObjectEnum:
        {
            std::shared_ptr<DevStudio::HlaPlatformAttributesImpl> platform = 
            std::static_pointer_cast<DevStudio::HlaPlatformAttributesImpl>(event.getEventOnDataAvailableObject());

            if (!platform) return;
        
            handleObject(platform.get()->getEntityIdentifier().getEntityNumber(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());

            if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                int forceIdentifier = platform.get()->getForceIdentifier();
                auto data = platform.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(platform, enumsCommon_Client::ActionObjectEnum::AddObject, forceIdentifier, marking);
                m_MapProtoEntity.insert(std::make_pair(platform.get()->getEntityIdentifier().getEntityNumber(), envelope));
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
            {
                auto it = m_MapProtoEntity.find(platform.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) {
                    std::cout << "Entity not found" << std::endl;
                    return;
                }
                int forceIdentifier = platform.get()->getForceIdentifier();
                auto data = platform.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(platform, enumsCommon_Client::ActionObjectEnum::UpdateObject, forceIdentifier, marking);
                it->second = envelope;
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
            {
                auto it = m_MapProtoEntity.find(platform.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) {
                    std::cout << "Entity not found" << std::endl;
                    return;
                }
                int forceIdentifier = platform.get()->getForceIdentifier();
                auto data = platform.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(platform, enumsCommon_Client::ActionObjectEnum::RemoveObject, forceIdentifier, marking);
                m_WebsocketServer->enqueueSend(envelope);
                m_MapProtoEntity.erase(it);
            }
            
            break;
        }
        case enumsCommon_Client::ObjectEnum::AircraftObjectEnum:
        {
            std::shared_ptr<DevStudio::HlaAircraftAttributesImpl> aircraftPtr = 
            std::static_pointer_cast<DevStudio::HlaAircraftAttributesImpl>(event.getEventOnDataAvailableObject());

            if (!aircraftPtr) return;

            handleObject(aircraftPtr.get()->getEntityIdentifier().getEntityNumber(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());
            
            if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                int forceIdentifier = aircraftPtr.get()->getForceIdentifier();
                auto data = aircraftPtr.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(aircraftPtr, enumsCommon_Client::ActionObjectEnum::AddObject, forceIdentifier, marking);
                m_MapProtoEntity.insert(std::make_pair(aircraftPtr.get()->getEntityIdentifier().getEntityNumber(), envelope));
                m_WebsocketServer->enqueueSend(envelope);
                std::cout << "enqueueSend newJson aircraft" << std::endl;
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
            {
                auto it = m_MapProtoEntity.find(aircraftPtr.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) 
                {   
                    std::cout << "Entity not found" << std::endl;
                    return;
                }
                int forceIdentifier = aircraftPtr.get()->getForceIdentifier();
                auto data = aircraftPtr.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(aircraftPtr, enumsCommon_Client::ActionObjectEnum::UpdateObject, forceIdentifier, marking);
                std::cout << "Heading: " << converter.calculate_Heading(aircraftPtr.get()->getSpatial()) << std::endl;
                it->second = envelope;
                m_WebsocketServer->enqueueSend(envelope);
            }
            else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
            {
                auto it = m_MapProtoEntity.find(aircraftPtr.get()->getEntityIdentifier().getEntityNumber());
                if (it == m_MapProtoEntity.end()) 
                {
                    std::cout << "Entity not found" << std::endl;
                    return;
                }
                int forceIdentifier = aircraftPtr.get()->getForceIdentifier();
                auto data = aircraftPtr.get()->getMarking().getMarkingData();
                std::string marking(data.begin(), data.end());
                Envelope envelope = makeEnvelope(aircraftPtr, enumsCommon_Client::ActionObjectEnum::RemoveObject, forceIdentifier, marking);
                m_WebsocketServer->enqueueSend(envelope);
                m_MapProtoEntity.erase(it);
            }
            
            break;
        }
            //Buscar a traves del HandleKind la entidad a la que se le asocia
        case enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum:
        {
            auto embeddedSystem = std::static_pointer_cast<DevStudio::HlaEmbeddedSystemAttributesImpl>(event.getEventOnDataAvailableObject());
            
            if (embeddedSystem)
            {
              //  receivedObjectWrapperEmbeddedSystem(&embeddedSystem, enumsCommon_Client::ActionObjectEnum(event.getActionObject()));
            }
            break;
        }
        case enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum:
        {
            auto emitterSystem = std::static_pointer_cast<DevStudio::HlaEmitterSystemAttributesImpl>(
                event.getEventOnDataAvailableObject()
            );

            // Si el puntero es nulo o ya existe en el mapa, salir
            if (!emitterSystem && event.getActionObject() != enumsCommon_Client::ActionObjectEnum::RemoveObject) return;

            std::string objectName = converter.convert_wstring_to_string(event.getObjectName());

            handleObject(emitterSystem ? emitterSystem.get()->getEmitterIndex() : -1, objectName, event.getActionObject());

            std::cout << "action emitter system: " << (int)event.getActionObject() << std::endl;
            switch (event.getActionObject())
            {
                case enumsCommon_Client::ActionObjectEnum::AddObject:
                    std::cout << "added Emitter System" << std::endl;
                    m_EmitterSystemMap[objectName] = emitterSystem;
                    break;

                case enumsCommon_Client::ActionObjectEnum::UpdateObject:
                    std::cout << "update Emitter System" << std::endl;
                    m_EmitterSystemMap[objectName] = emitterSystem;
                    break;

                case enumsCommon_Client::ActionObjectEnum::RemoveObject:
                    std::cout << "remove Emitter System" << std::endl;
                    // Borrar del mapa si existe
                    m_EmitterSystemMap.erase(objectName);
                    break;

                default:
                    break;
            }

            break;

            // auto emitterSystem = std::static_pointer_cast<DevStudio::HlaEmitterSystemAttributesImpl>(event.getEventOnDataAvailableObject());
            
            // if (!emitterSystem || m_EmitterSystemMap.find(converter.convert_wstring_to_string(event.getObjectName())) != m_EmitterSystemMap.end()) return;

            // handleObject(emitterSystem.get()->getEmitterIndex(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());

            // m_EmitterSystemMap[converter.convert_wstring_to_string(event.getObjectName())] = emitterSystem;

            // break;
        }
        case enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum:
        {
            auto emitterBeam = std::static_pointer_cast<DevStudio::HlaEmitterBeamAttributesImpl>(event.getEventOnDataAvailableObject());
            
            break;
        }
        case enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum:
        {
            auto radarBeam = std::static_pointer_cast<DevStudio::HlaRadarBeamAttributesImpl>(event.getEventOnDataAvailableObject());

            if (radarBeam && m_EmitterSystemMap.find(radarBeam.get()->getEmitterSystemIdentifier())!= m_EmitterSystemMap.end())
            {
                handleObject(radarBeam.get()->getBeamIdentifier(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());

                auto& emitterSystem = m_EmitterSystemMap.find(radarBeam.get()->getEmitterSystemIdentifier())->second;
                if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                    std::cout << "añadir radar beam " << std::endl;

                    std::vector<std::string> trackedIds;

                    try {
                        trackedIds = radarBeam.get()->getTrackObjectIdentifiers();
                    } catch (const DevStudio::HlaValueNotSetException& e) {
                        trackedIds = {};
                        std::cout << "trackObjectIdentifiers not set, using empty list" << std::endl;
                    }

                    Envelope envelope = makeEmitterBeamEnvelope(emitterSystem, radarBeam, enumsCommon_Client::ActionObjectEnum::AddObject, trackedIds);
                    m_MapProtoBeams.insert(std::make_pair(radarBeam.get()->getBeamIdentifier(), envelope));
                    m_WebsocketServer->enqueueSend(envelope);
                    std::cout << "enqueueSend newJson beam" << std::endl;
                }
                else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
                {
                    auto it = m_MapProtoBeams.find(radarBeam.get()->getBeamIdentifier());
                    if (it == m_MapProtoBeams.end()) 
                    {   
                        std::cout << "Beam not found" << std::endl;
                        return;
                    }

                    std::vector<std::string> trackedIds;

                    try {
                        trackedIds = radarBeam.get()->getTrackObjectIdentifiers();
                    } catch (const DevStudio::HlaValueNotSetException& e) {
                        trackedIds = {};
                        std::cout << "trackObjectIdentifiers not set, using empty list" << std::endl;
                    }

                    Envelope envelope = makeEmitterBeamEnvelope(emitterSystem, radarBeam, enumsCommon_Client::ActionObjectEnum::UpdateObject, trackedIds);
                    it->second = envelope;
                    m_WebsocketServer->enqueueSend(envelope);
                }
                else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
                {
                    auto it = m_MapProtoBeams.find(radarBeam.get()->getBeamIdentifier());
                    if (it == m_MapProtoBeams.end()) 
                    {
                        std::cout << "Beam not found" << std::endl;
                        return;
                    }

                    std::vector<std::string> trackedIds;

                    try {
                        trackedIds = radarBeam.get()->getTrackObjectIdentifiers();
                    } catch (const DevStudio::HlaValueNotSetException& e) {
                        trackedIds = {};
                        std::cout << "trackObjectIdentifiers not set, using empty list" << std::endl;
                    }
                    Envelope envelope = makeEmitterBeamEnvelope(emitterSystem, radarBeam, enumsCommon_Client::ActionObjectEnum::RemoveObject, trackedIds);
                    m_WebsocketServer->enqueueSend(envelope);
                    m_MapProtoBeams.erase(it);
                }
            }
            else {
                std::cout << "Emitter System associated not found" << std::endl;
                return;
            }

            break;
        }
        case enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum:
        {
            auto jammerBeam = std::static_pointer_cast<DevStudio::HlaJammerBeamAttributesImpl>(event.getEventOnDataAvailableObject());
            
            if (jammerBeam && m_EmitterSystemMap.find(jammerBeam.get()->getEmitterSystemIdentifier())!= m_EmitterSystemMap.end())
            {
                handleObject(jammerBeam.get()->getBeamIdentifier(), converter.convert_wstring_to_string(event.getObjectName()), event.getActionObject());

                auto& emitterSystem = m_EmitterSystemMap.find(jammerBeam.get()->getEmitterSystemIdentifier())->second;
                if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                    Envelope envelope = makeEmitterBeamEnvelope(emitterSystem, jammerBeam, enumsCommon_Client::ActionObjectEnum::AddObject, jammerBeam.get()->getJammedObjectIdentifiers());
                    m_MapProtoBeams.insert(std::make_pair(jammerBeam.get()->getBeamIdentifier(), envelope));
                    m_WebsocketServer->enqueueSend(envelope);
                    std::cout << "enqueueSend newJson beam" << std::endl;
                }
                else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
                {
                    auto it = m_MapProtoBeams.find(jammerBeam.get()->getBeamIdentifier());
                    if (it == m_MapProtoBeams.end()) 
                    {   
                        std::cout << "Beam not found" << std::endl;
                        return;
                    }
                    Envelope envelope = makeEmitterBeamEnvelope(emitterSystem, jammerBeam, enumsCommon_Client::ActionObjectEnum::AddObject, jammerBeam.get()->getJammedObjectIdentifiers());
                    it->second = envelope;
                    m_WebsocketServer->enqueueSend(envelope);
                }
                else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
                {
                    auto it = m_MapProtoBeams.find(jammerBeam.get()->getBeamIdentifier());
                    if (it == m_MapProtoBeams.end()) 
                    {
                        std::cout << "Beam not found" << std::endl;
                        return;
                    }
                    Envelope envelope = makeEmitterBeamEnvelope(emitterSystem, jammerBeam, enumsCommon_Client::ActionObjectEnum::AddObject, jammerBeam.get()->getJammedObjectIdentifiers());
                    m_WebsocketServer->enqueueSend(envelope);
                    m_MapProtoBeams.erase(it);
                }
            }
            else {
                std::cout << "Emitter System associated not found" << std::endl;
                return;
            }

            break;
        }

    }
}


void ClientPitchWebSocket::setRunningFlag(std::atomic<bool>* flag) 
{ 
    m_RunningFlag = flag; 
}

void ClientPitchWebSocket::onStopEventReceived() 
{
    if (m_RunningFlag)
    {
        *m_RunningFlag = false;
        std::cout << "[ClientPitchWebSocket] Stop main loop..." << std::endl;
    }
}

void ClientPitchWebSocket::createMessageFederate(const std::shared_ptr<DevStudio::HlaHLAfederateAttributesImpl>& federate, enumsCommon_Client::ActionObjectEnum action)
{

    if(federate)
    {
        //handle
        std::vector<char> federateHandleChar = federate.get()->getHLAfederateHandle();
        std::string federateHandle = std::string(federateHandleChar.begin(), federateHandleChar.end());

        //FederateName
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wstringFederateName = federate.get()->getHLAfederateName();
        std::string federateName = converter.to_bytes(wstringFederateName);

        //FederateType
        std::wstring wstringFederateType = federate.get()->getHLAfederateType();
        std::string federateType = converter.to_bytes(wstringFederateType);

        //FederateHost
        std::wstring wstringFederateHost = federate.get()->getHLAfederateHost();
        std::string federateHost = converter.to_bytes(wstringFederateHost);

        //FederateState
        DevStudio::HLAfederateState::HLAfederateState enumFederateState = federate.get()->getHLAfederateState();
        int federateState = (int)enumFederateState;

        //Using Proto                 
        ProtoGeneration protoGen;
                        
        Envelope envelope = protoGen.createProtoFederate(
            action
            , federateHandle
            , m_FederationName
            , federateName
            , federateType
            , federateHost
            , federateState
        );

        m_WebsocketServer->enqueueSend(envelope);
    }
}