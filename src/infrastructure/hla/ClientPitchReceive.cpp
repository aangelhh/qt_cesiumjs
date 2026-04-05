#include "ClientPitch.h"
#include "utils/LoggerManager.h"
#include "utils/Utils.h"
#include <DevStudio/HlaInteractionManager.h>
#include <DevStudio/HlaMunitionAttributesImpl.h>
#include <DevStudio/HlaSuppliesAttributesImpl.h>
#include <DevStudio/HlaExpendablesAttributesImpl.h>
#include <DevStudio/HlaEmbeddedSystemAttributesImpl.h>
#include <DevStudio/HlaEmitterSystemAttributesImpl.h>
#include <DevStudio/HlaAggregateEntityAttributesImpl.h>
#include <DevStudio/HlaSensorAttributesImpl.h>
#include <SDLDevStudio/Encoders.h>
#include <SDLDevStudio/Decoders.h>

using namespace enumsCommon_Client;

void SDL_RTI::ClientPitch::onEventReceived(const EventOnDataAvailableInteraction& event)
{
    if (event.getEventOnDataAvailableInteraction()) {
        switch (event.getInteractionEnum())
        {
            case enumsCommon_Client::InteractionEnum::NullInteractionEnum:
                //LogError
                break;

            case enumsCommon_Client::InteractionEnum::StartResumeInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaStartResumeParametersImpl> startResume = std::static_pointer_cast<DevStudio::HlaStartResumeParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (startResume)
                {
                    receivedStartResumeInteraction(&startResume);

                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::StartResumeAnswerInteractionEnum:
            {
                auto startResumeAnswer = std::static_pointer_cast<DevStudio::HlaStartResumeRParametersImpl>(event.getEventOnDataAvailableInteraction());
                if (startResumeAnswer)
                {
                    receivedStartResumeAnswerInteraction(&startResumeAnswer);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::StopFreezeAnswerInteractionEnum:
            {
                auto stopFreezeAnswer = std::static_pointer_cast<DevStudio::HlaStopFreezeRParametersImpl>(event.getEventOnDataAvailableInteraction());
                if (stopFreezeAnswer)
                {
                    receivedStopFreezeAnswerInteraction(&stopFreezeAnswer);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::StopFreezeInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaStopFreezeParametersImpl> stopFreeze = std::static_pointer_cast<DevStudio::HlaStopFreezeParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (stopFreeze)
                {
                    receivedStopFreezeInteraction(&stopFreeze);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::AcknowledgeInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaAcknowledgeParametersImpl> acknowledge = std::static_pointer_cast<DevStudio::HlaAcknowledgeParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (acknowledge)
                {
                    receivedAcknowledgeInteraction(&acknowledge);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::CreateEntityInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaCreateEntityParametersImpl> create = std::static_pointer_cast<DevStudio::HlaCreateEntityParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (create)
                {
                    receivedCreateInteraction(&create);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::RemoveEntityInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaRemoveEntityParametersImpl> remove = std::static_pointer_cast<DevStudio::HlaRemoveEntityParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (remove)
                {
                    receivedRemoveInteraction(&remove);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::CollisionInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaCollisionParametersImpl> collision = std::static_pointer_cast<DevStudio::HlaCollisionParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (collision)
                {
                    receivedCollisionInteraction(&collision);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::MunitionDetonationInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaMunitionDetonationParametersImpl> munition = std::static_pointer_cast<DevStudio::HlaMunitionDetonationParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (munition)
                {
                    receivedMunitionDetonationInteraction(&munition);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::ResupplyCancelInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaResupplyCancelParametersImpl> cancel = std::static_pointer_cast<DevStudio::HlaResupplyCancelParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (cancel)
                {
                    receivedResupplyCancelInteraction(&cancel);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::ResupplyOfferInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaResupplyOfferParametersImpl> offer = std::static_pointer_cast<DevStudio::HlaResupplyOfferParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (offer)
                {
                    receivedResupplyOfferInteraction(&offer);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::ResupplyReceivedInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaResupplyReceivedParametersImpl> received = std::static_pointer_cast<DevStudio::HlaResupplyReceivedParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (received)
                {
                    receivedResupplyReceivedInteraction(&received);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::WeaponFireInteractionEnum:
            {
                std::shared_ptr<DevStudio::HlaWeaponFireParametersImpl> weapon = std::static_pointer_cast<DevStudio::HlaWeaponFireParametersImpl>(event.getEventOnDataAvailableInteraction());
                if (weapon)
                {
                    receivedWeaponFireInteraction(&weapon);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::CreateObjectRequestEnum:
            {
                auto request = std::static_pointer_cast<DevStudio::HlaCreateObjectRequestParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (request)
                {
                    receivedCreateObjectRequestInteraction(&request);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::RemoveObjectRequestEnum:
            {
                auto request = std::static_pointer_cast<DevStudio::HlaRemoveObjectRequestParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (request)
                {
                    receivedRemoveObjectRequestInteraction(&request);
                }
                break;
            }
            case enumsCommon_Client::InteractionEnum::AttributeChangeRequestEnum:
            {
                auto request = std::static_pointer_cast<DevStudio::HlaAttributeChangeRequestParametersImpl>(event.getEventOnDataAvailableInteraction());

                if (request)
                {
                    receivedAttributeChangeRequestInteraction(&request);
                }
                break;
            }
        }
    }
}

void SDL_RTI::ClientPitch::onEventReceived(const EventOnDataAvailableObject& event)
{
    if (event.getEventOnDataAvailableObject()) {

        switch (event.getObjectObject()) {

            case enumsCommon_Client::ObjectEnum::NullObjectEnum: {
                //LogError
                break;
            }

            case enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum: {

                std::shared_ptr<DevStudio::HlaBaseEntityAttributesImpl> baseEntity =  
                std::static_pointer_cast<DevStudio::HlaBaseEntityAttributesImpl>(event.getEventOnDataAvailableObject());

                if (baseEntity)
                {
                    //Example

                    // if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::AddObject) {
                        
                    //     double latitude = converter.convert_xyz_to_lat(baseEntity.get()->getSpatial()); 
                    //     double longitude = converter.convert_xyz_to_lon(baseEntity.get()->getSpatial());
                    //     double altitude = converter.convert_xyz_to_alt(baseEntity.get()->getSpatial());
                    //     int idEntity =  baseEntity.get()->getEntityIdentifier().getEntityNumber();

                    //     //Using Proto
                    //     ProtoGeneration protoGen;
                    //     Envelope envelope = protoGen.createProtoPhysicalEntity(
                    //          enumsCommon_Client::ActionObjectEnum::AddObject, 
                    //          idEntity,
                    //          latitude,
                    //          longitude,
                    //          altitude);

                    //     mapProtoEntity.insert(std::make_pair(baseEntity.get()->getEntityIdentifier().getEntityNumber(), envelope));

                    //     websocketServer->enqueueSend(envelope);
                }
                else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::UpdateObject) 
                {
                    //available data
                }
                else if (event.getActionObject() == enumsCommon_Client::ActionObjectEnum::RemoveObject) 
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum:
            {
                std::shared_ptr<DevStudio::HlaPhysicalEntityImpl> physicalEntity = 
                std::static_pointer_cast<DevStudio::HlaPhysicalEntityImpl>(
                event.getEventOnDataAvailableObject());

                if (physicalEntity)
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::PlatformObjectEnum:
            {
                std::shared_ptr<DevStudio::HlaPlatformAttributesImpl> platform = 
                std::static_pointer_cast<DevStudio::HlaPlatformAttributesImpl>(event.getEventOnDataAvailableObject());

                if (platform)
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::AircraftObjectEnum:
            {
                std::shared_ptr<DevStudio::HlaAircraftAttributesImpl> aircraftPtr = 
                std::static_pointer_cast<DevStudio::HlaAircraftAttributesImpl>(event.getEventOnDataAvailableObject());

                if (aircraftPtr)
                {
                    //available data   
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum:
            {
                auto embeddedSystem = std::static_pointer_cast<DevStudio::HlaEmbeddedSystemAttributesImpl>(event.getEventOnDataAvailableObject());
                
                if (embeddedSystem)
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum:
            {
                auto emitterSystem = std::static_pointer_cast<DevStudio::HlaEmitterSystemAttributesImpl>(event.getEventOnDataAvailableObject());
                
                if (emitterSystem)
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum:
            {
                auto emitterBeam = std::static_pointer_cast<DevStudio::HlaEmitterBeamAttributesImpl>(event.getEventOnDataAvailableObject());
                
                if (emitterBeam)
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum:
            {
                auto radarBeam = std::static_pointer_cast<DevStudio::HlaRadarBeamAttributesImpl>(event.getEventOnDataAvailableObject());

                if (radarBeam)
                {
                    //available data
                }
                break;
            }
            case enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum:
            {
                auto jammerBeam = std::static_pointer_cast<DevStudio::HlaJammerBeamAttributesImpl>(event.getEventOnDataAvailableObject());
                
                if (jammerBeam)
                {
                    //available data
                }
                break;
            }
        }
    }   
}

void SDL_RTI::ClientPitch::receivedStartResumeInteraction(DevStudio::HlaStartResumeParametersImplPtr* startResume)
{
    if (m_PitchListener)
    {
        DevStudio::HlaStartResumeParametersPtr* params = reinterpret_cast<DevStudio::HlaStartResumeParametersPtr*>(startResume);
        (*m_PitchListener)->receivedInteractionStartResume(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedStartResumeAnswerInteraction(DevStudio::HlaStartResumeRParametersImplPtr* startResume)
{
	if (m_PitchListener)
	{
		DevStudio::HlaStartResumeRParametersPtr* params = reinterpret_cast<DevStudio::HlaStartResumeRParametersPtr*>(startResume);
		(*m_PitchListener)->receivedInteractionStartResumeAnswer(params);
		//manageFederate(params->get()->getProducingFederate()->getFederateName(), params->get()->getOriginatingEntity().getFederateIdentifier());
	}
	else
	{
		std::cout << "The wrapper object is invalid" << std::endl;
	}
}

void SDL_RTI::ClientPitch::receivedStopFreezeInteraction(DevStudio::HlaStopFreezeParametersImplPtr* stopFreeze)
{
    if (m_PitchListener)
    {
        DevStudio::HlaStopFreezeParametersPtr* params = reinterpret_cast<DevStudio::HlaStopFreezeParametersPtr*>(stopFreeze);
        (*m_PitchListener)->receivedInteractionStopFreeze(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedStopFreezeAnswerInteraction(DevStudio::HlaStopFreezeRParametersImplPtr* stopFreeze)
{
	if (m_PitchListener)
	{
		DevStudio::HlaStopFreezeRParametersPtr* params = reinterpret_cast<DevStudio::HlaStopFreezeRParametersPtr*>(stopFreeze);
		(*m_PitchListener)->receivedInteractionStopFreezeAnswer(params);
		// manageFederate(params->get()->getProducingFederate()->getFederateName(), params->get()->getOriginatingEntity().getFederateIdentifier());
	}
	else
	{
		std::cout << "The wrapper object is invalid" << std::endl;
	}
}

void SDL_RTI::ClientPitch::receivedAcknowledgeInteraction(DevStudio::HlaAcknowledgeParametersImplPtr* acknowledge)
{
    if (m_PitchListener)
    {
        DevStudio::HlaAcknowledgeParametersPtr* params = reinterpret_cast<DevStudio::HlaAcknowledgeParametersPtr*>(acknowledge);
        (*m_PitchListener)->receivedInteractionAcknowledge(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}


void SDL_RTI::ClientPitch::receivedCollisionInteraction(DevStudio::HlaCollisionParametersImplPtr* collision)
{
    if (m_PitchListener)
    {
        DevStudio::HlaCollisionParametersPtr* params = reinterpret_cast<DevStudio::HlaCollisionParametersPtr*>(collision);
        (*m_PitchListener)->receivedInteractionCollision(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedMunitionDetonationInteraction(DevStudio::HlaMunitionDetonationParametersImplPtr* munition)
{
    if (m_PitchListener)
    {
        DevStudio::HlaMunitionDetonationParametersPtr* params = reinterpret_cast<DevStudio::HlaMunitionDetonationParametersPtr*>(munition);
        (*m_PitchListener)->receivedInteractionMunitionDetonation(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedResupplyCancelInteraction(DevStudio::HlaResupplyCancelParametersImplPtr* resupplyCancel)
{
    if (m_PitchListener)
    {
        DevStudio::HlaResupplyCancelParametersPtr* params = reinterpret_cast<DevStudio::HlaResupplyCancelParametersPtr*>(resupplyCancel);
        (*m_PitchListener)->receivedInteractionResupplyCancel(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedResupplyOfferInteraction(DevStudio::HlaResupplyOfferParametersImplPtr* resupplyOffer)
{
    if (m_PitchListener)
    {
        DevStudio::HlaResupplyOfferParametersPtr* params = reinterpret_cast<DevStudio::HlaResupplyOfferParametersPtr*>(resupplyOffer);
        (*m_PitchListener)->receivedInteractionResupplyOffer(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedResupplyReceivedInteraction(DevStudio::HlaResupplyReceivedParametersImplPtr* resupplyReceived)
{
    if (m_PitchListener)
    {
        DevStudio::HlaResupplyReceivedParametersPtr* params = reinterpret_cast<DevStudio::HlaResupplyReceivedParametersPtr*>(resupplyReceived);
        (*m_PitchListener)->receivedInteractionResupplyReceived(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedWeaponFireInteraction(DevStudio::HlaWeaponFireParametersImplPtr* weaponFire)
{
    if (m_PitchListener)
    {
        DevStudio::HlaWeaponFireParametersPtr* params = reinterpret_cast<DevStudio::HlaWeaponFireParametersPtr*>(weaponFire);
        (*m_PitchListener)->receivedInteractionWeaponFire(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedCreateInteraction(DevStudio::HlaCreateEntityParametersImplPtr* create)
{
    if (m_PitchListener)
    {
        DevStudio::HlaCreateEntityParametersPtr* params = reinterpret_cast<DevStudio::HlaCreateEntityParametersPtr*>(create);

        (*m_PitchListener)->receivedInteractionCreateEntity(params); 
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedRemoveInteraction(DevStudio::HlaRemoveEntityParametersImplPtr* remove)
{
    if (m_PitchListener)
    {
        DevStudio::HlaRemoveEntityParametersPtr* params = reinterpret_cast<DevStudio::HlaRemoveEntityParametersPtr*>(remove);
        (*m_PitchListener)->receivedInteractionRemoveEntity(params);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedCreateObjectRequestInteraction(DevStudio::HlaCreateObjectRequestParametersImplPtr* val)
{
    if (m_PitchListener != nullptr)
    {
        DevStudio::HlaCreateObjectRequestParametersPtr* params = reinterpret_cast<DevStudio::HlaCreateObjectRequestParametersPtr*>(val);
        auto classType = (HlaObjectInstanceBase::ObjectClassType)params->get()->getObjectClass();
        (*m_PitchListener)->receivedInteractionCreateObjectRequest(classType, DataTypes::AttributeValuePair::getVector(classType, params->get()->getAttributeValueSet()));
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedRemoveObjectRequestInteraction(DevStudio::HlaRemoveObjectRequestParametersImplPtr* val)
{
    if (m_PitchListener != nullptr)
    {
        DevStudio::HlaRemoveObjectRequestParametersPtr* params = reinterpret_cast<DevStudio::HlaRemoveObjectRequestParametersPtr*>(val);
        std::vector<DataTypes::ObjectIdentifier> objectIdentifiers;
        objectIdentifiers.clear();
        for (auto& identifier : params->get()->getObjectIdentifiers())
        {
            std::cout << "Identifier: " << identifier << std::endl;
            objectIdentifiers.push_back(DataTypes::ObjectIdentifier(m_World, identifier));
        }
        (*m_PitchListener)->receivedInteractionRemoveObjectRequest(objectIdentifiers);
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}

void SDL_RTI::ClientPitch::receivedAttributeChangeRequestInteraction(DevStudio::HlaAttributeChangeRequestParametersImplPtr* val)
{
    if (m_PitchListener != nullptr)
    {
        DevStudio::HlaAttributeChangeRequestParametersPtr* params = reinterpret_cast<DevStudio::HlaAttributeChangeRequestParametersPtr*>(val);
        std::vector<DataTypes::ObjectIdentifier> objectIdentifiers;
        objectIdentifiers.clear();
        for (auto& identifier : params->get()->getObjectIdentifiers())
        {
            objectIdentifiers.push_back(DataTypes::ObjectIdentifier(m_World, identifier));
        }
        (*m_PitchListener)->receivedInteractionAttributeChangeRequest(objectIdentifiers, DataTypes::AttributeValuePair::getVector(objectIdentifiers[0].GetType(), params->get()->getAttributeValueSet()));
    }
    else
    {
        std::cout << "The wrapper object is invalid" << std::endl;
    }
}