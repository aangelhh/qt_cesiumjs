#include "../include/InteractionManagerListener.h"
#include "../include/enumsCommon/EnumsCommons.h"
#include "../include/events/EventBus.h"
#include <DevStudio/HlaPointers.h>
#include <DevStudio/HlaInteractionManager.h>
#include "../include/SimlabSettings.h"

using namespace enumsCommon_Client;

void InteractionManagerListener::startResume(bool local, DevStudio::HlaStartResumeParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Received Play/Pause Interaction" << std::endl;
    DevStudio::HlaStartResumeParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::StartResumeInteractionEnum));
    data.setActionEnum(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
    data.setEventOnDataAvailableInteraction(attributes);
    DevStudio::HlaFederateIdPtr federate = attributes->getProducingFederate();
    data.setFederateName(&federate);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::StartResumeInteractionEnum);
}

void InteractionManagerListener::stopFreeze(bool local, DevStudio::HlaStopFreezeParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Received Stop/Freeze Interaction" << std::endl;
    DevStudio::HlaStopFreezeParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::StopFreezeInteractionEnum));
    data.setActionEnum(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
    data.setEventOnDataAvailableInteraction(attributes);
    //std::cout << attributes->getProducingFederate() << std::endl;
    //DevStudio::HlaFederateIdPtr federate = attributes->getProducingFederate();
    //data.setFederateName(&federate);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::StopFreezeInteractionEnum);
}

void InteractionManagerListener::collision(bool local, DevStudio::HlaCollisionParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Collision Interaction received" << std::endl;
    DevStudio::HlaCollisionParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::CollisionInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::CollisionInteractionEnum);
}

void InteractionManagerListener::createEntity(bool local, DevStudio::HlaCreateEntityParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Create Entity" << std::endl;
    DevStudio::HlaCreateEntityParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::CreateEntityInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::CreateEntityInteractionEnum);
}

void InteractionManagerListener::removeEntity(bool local, DevStudio::HlaRemoveEntityParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Remove Entity" << std::endl;
    DevStudio::HlaRemoveEntityParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::RemoveEntityInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::RemoveEntityInteractionEnum);
}

void InteractionManagerListener::acknowledge(bool local, DevStudio::HlaAcknowledgeParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Acknowledge interaction received" << std::endl;
    DevStudio::HlaAcknowledgeParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::AcknowledgeInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::AcknowledgeInteractionEnum);
}

void InteractionManagerListener::munitionDetonation(bool local, DevStudio::HlaMunitionDetonationParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Munition detonation interaction received" << std::endl;
    DevStudio::HlaMunitionDetonationParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::MunitionDetonationInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::MunitionDetonationInteractionEnum);
}

void InteractionManagerListener::resupplyCancel(bool local, DevStudio::HlaResupplyCancelParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Resupply cancel interaction received" << std::endl;
    DevStudio::HlaResupplyCancelParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::ResupplyCancelInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::ResupplyCancelInteractionEnum);
}

void InteractionManagerListener::resupplyOffer(bool local, DevStudio::HlaResupplyOfferParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Resupply offer interaction received" << std::endl;
    DevStudio::HlaResupplyOfferParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::ResupplyOfferInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::ResupplyOfferInteractionEnum);
}

void InteractionManagerListener::resupplyReceived(bool local, DevStudio::HlaResupplyReceivedParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Resupply received interaction received" << std::endl;
    DevStudio::HlaResupplyReceivedParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::ResupplyReceivedInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::ResupplyReceivedInteractionEnum);
}

void InteractionManagerListener::weaponFire(bool local, DevStudio::HlaWeaponFireParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Weapon fire interaction received" << std::endl;
    DevStudio::HlaWeaponFireParametersPtr attributes = parameters;
    EventOnDataAvailableInteraction data;

    data.setInteractionEnum(static_cast<int>(enumsCommon_Client::InteractionEnum::WeaponFireInteractionEnum));
    data.setEventOnDataAvailableInteraction(attributes);
    events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::InteractionEnum::WeaponFireInteractionEnum);
}

