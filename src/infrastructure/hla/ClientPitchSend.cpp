#include "ClientPitch.h"
#include "utils/LoggerManager.h"
#include "utils/Utils.h"
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/HlaWorld.h>
#include <DevStudio/HlaWorldImpl.h>
#include <DataTypes/Converters.h>
#include <SDLDevStudio/Encoders.h>
#include <SDLDevStudio/Decoders.h>

using namespace enumsCommon_Client;

void SDL_RTI::ClientPitch::sendStartResumeInteraction(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::ClockTimeStruct& simulationTime) {
    if (!m_World) {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager) {
        std::cerr << "Interaction Manager is not initialized" << std::endl;
        return;
    }

	interactionManager->sendStartResume(originatingEntity, receivingEntity, realWorldTime, m_IdMagicAction, simulationTime);
	m_IdMagicAction += 1;
}

void SDL_RTI::ClientPitch::sendStartResumeInteraction(const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::ClockTimeStruct& simulationTime) {
	DevStudio::EntityIdentifierStruct originatingEntity(DevStudio::FederateIdentifierStruct(Configuration::getInstance().getSiteId(), Configuration::getInstance().getApplicationId()), 65535);
	sendStartResumeInteraction(originatingEntity, receivingEntity, realWorldTime, simulationTime);
}

void SDL_RTI::ClientPitch::sendStartResumeAnswerInteraction(const AcknowledgementProtocolEnum::AcknowledgementProtocolEnum& acknowledgementProtocol, const ClockTimeStruct& realWorldTime, const unsigned int& requestIdentifier, const ClockTimeStruct& simulationTime)
{
	if (!m_World)
	{
		std::cerr << "World is not initialized" << std::endl;
		return;
	}
	auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
	if (!interactionManager)
	{
		std::cerr << "Interaction Manager is not initialized" << std::endl;
		return;
	}
	DevStudio::EntityIdentifierStruct originatingEntity(DevStudio::FederateIdentifierStruct(Configuration::getInstance().getSiteId(), Configuration::getInstance().getApplicationId()), 65535);
	DevStudio::EntityIdentifierStruct receivingEntity(DevStudio::FederateIdentifierStruct(65535, 65535), 65535);

	interactionManager->sendStartResumeR(acknowledgementProtocol, originatingEntity, receivingEntity, realWorldTime, requestIdentifier, simulationTime);
}

void SDL_RTI::ClientPitch::sendStopFreezeInteraction(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& reason, const bool& reflectValues, const bool& runInternalSimulationClock, const bool& updateAttributes)
{
    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager)
    {
        std::cerr << "Interaction Manager is not initialized" << std::endl;
        return;
    }

	interactionManager->sendStopFreeze(originatingEntity, receivingEntity, m_IdMagicAction, realWorldTime, reason, reflectValues, runInternalSimulationClock, updateAttributes);
	m_IdMagicAction += 1;
}

void SDL_RTI::ClientPitch::sendStopFreezeInteraction(const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& reason, const bool& reflectValues, const bool& runInternalSimulationClock, const bool& updateAttributes)
{
	DevStudio::EntityIdentifierStruct originatingEntity(DevStudio::FederateIdentifierStruct(Configuration::getInstance().getSiteId(), Configuration::getInstance().getApplicationId()), 65535);
	sendStopFreezeInteraction(originatingEntity, receivingEntity, realWorldTime, reason, reflectValues, runInternalSimulationClock, updateAttributes);
}

void SDL_RTI::ClientPitch::sendStopFreezeAnswerInteraction(const AcknowledgementProtocolEnum::AcknowledgementProtocolEnum& acknowledgementProtocol, const unsigned int& requestIdentifier, const ClockTimeStruct& realWorldTime, const StopFreezeReasonEnum::StopFreezeReasonEnum& reason, const bool& reflectValues, const bool& runInternalSimulationClock, const bool& updateAttributes)
{
	if (!m_World)
	{
		std::cerr << "World is not initialized" << std::endl;
		return;
	}
	auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
	if (!interactionManager)
	{
		std::cerr << "Interaction Manager is not initialized" << std::endl;
		return;
	}
	DevStudio::EntityIdentifierStruct federateEntity(DevStudio::FederateIdentifierStruct(Configuration::getInstance().getSiteId(), Configuration::getInstance().getApplicationId()), 65535);

	interactionManager->sendStopFreezeR(acknowledgementProtocol, federateEntity, federateEntity, requestIdentifier, realWorldTime, reason, reflectValues, runInternalSimulationClock, updateAttributes);
}

void SDL_RTI::ClientPitch::sendCreateObjectRequest(unsigned int classObject, std::vector<DataTypes::AttributeValuePair>& attributes)
{
    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

	std::vector<DevStudio::AttributeValuePairStruct> attributeValuePairStructs;
	for (DataTypes::AttributeValuePair& attribute : attributes)
		attributeValuePairStructs.push_back(attribute.getAttributeValuePairStruct());

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    interactionManager->sendCreateObjectRequest(classObject, attributeValuePairStructs, m_IdMagicAction++);
}

void SDL_RTI::ClientPitch::sendRemoveObjectsRequest(const std::vector<DataTypes::ObjectIdentifier>& objectIdentifiers)
{
    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }
	std::vector<std::string> identifiers;
	identifiers.clear();
	for (auto identifier : objectIdentifiers)
	{
		std::cout << "Type: " << identifier.GetType() << std::endl;
		identifiers.push_back(identifier.GetObjectIdentifierString(m_World));
	}
    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    interactionManager->sendRemoveObjectRequest(identifiers, m_IdMagicAction++);
}

void SDL_RTI::ClientPitch::sendAttributeChangeRequest(
	const std::vector<DataTypes::ObjectIdentifier>& objectIdentifiers,
    const std::vector<DataTypes::AttributeValuePair >& attributeValueSet
)
{
    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

	std::vector<std::string> identifiers;
	identifiers.clear();
	for (auto identifier : objectIdentifiers)
		identifiers.push_back(identifier.GetObjectIdentifierString(m_World));

	std::vector<DevStudio::AttributeValuePairStruct> attributeValuePairStructs;
	for (DataTypes::AttributeValuePair attribute : attributeValueSet)
		attributeValuePairStructs.push_back(attribute.getAttributeValuePairStruct());

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager)
    {
        std::cerr << "Failed to retrieve HlaInteractionManager." << std::endl;
        return;
    }

    try
    {
        interactionManager->sendAttributeChangeRequest(identifiers, attributeValuePairStructs);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while sending reposition request: " << e.what() << std::endl;
    }
}

void SDL_RTI::ClientPitch::sendChangeSpeedMagicActionChangeSpeedObjectRequest(
    const std::vector<std::string >& objectIdentifiers,
    const std::vector</* not empty */ DevStudio::AttributeValuePairStruct >& attributeValueSet
)
{
    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager)
    {
        std::cerr << "Failed to retrieve HlaInteractionManager." << std::endl;
        return;
    }

    try
    {
        interactionManager->sendAttributeChangeRequest(objectIdentifiers, attributeValueSet);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while sending change veloctity request: " << e.what() << std::endl;
    }
}

void SDL_RTI::ClientPitch::sendWeaponFire(const DevStudio::EventIdentifierStruct& eventIdentifier, const float& fireControlSolutionRange, const unsigned int& fireMissionIndex, const DevStudio::WorldLocationStruct& firingLocation, DataTypes::ObjectIdentifier& firingObjectIdentifier, const DevStudio::FuseTypeEnum::FuseTypeEnum& fuseType, const DevStudio::VelocityVectorStruct& initialVelocityVector, DataTypes::ObjectIdentifier& munitionObjectIdentifier, const DevStudio::EntityTypeStruct& munitionType, const unsigned short& quantityFired, const unsigned short& rateOfFire, DataTypes::ObjectIdentifier& targetObjectIdentifier, const DevStudio::WarheadTypeEnum::WarheadTypeEnum& warheadType)
{
	if (!m_World)
	{
		std::cerr << "World is not initialized" << std::endl;
		return;
	}
	auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
	if (!interactionManager)
	{
		std::cerr << "Failed to retrieve HlaInteractionManager." << std::endl;
		return;
	}
	try
	{
		std::string firingObjectIdentifierString = firingObjectIdentifier.GetObjectIdentifierString(m_World);
		std::string munitionObjectIdentifierString = munitionObjectIdentifier.GetObjectIdentifierString(m_World);
		std::string targetObjectIdentifierString = targetObjectIdentifier.GetObjectIdentifierString(m_World);

		auto munition = std::static_pointer_cast<DevStudio::HlaMunition>(munitionObjectIdentifier.GetHlaObject(m_World));
		auto munitionUpdater = munition->getHlaMunitionUpdater();
		munitionUpdater->setIsPartOf(DevStudio::IsPartOfStruct());
		munitionUpdater->sendUpdate();

		interactionManager.get()->sendWeaponFire(eventIdentifier, fireControlSolutionRange, fireMissionIndex, firingLocation, firingObjectIdentifierString, fuseType, initialVelocityVector, munitionObjectIdentifierString, munitionType, quantityFired, rateOfFire, targetObjectIdentifierString, warheadType);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error while sending weapon fire request: " << e.what() << std::endl;
	}
}

void SDL_RTI::ClientPitch::sendMunitionDetonation(const std::vector<DevStudio::ArticulatedParameterStruct>& articulatedPartData, const DevStudio::WorldLocationStruct& detonationLocation, const DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum& detonationResultCode, const DevStudio::EventIdentifierStruct& eventIdentifier, DataTypes::ObjectIdentifier& firingObjectIdentifier, const DevStudio::VelocityVectorStruct& finalVelocityVector, const DevStudio::FuseTypeEnum::FuseTypeEnum& fuseType, DataTypes::ObjectIdentifier& munitionObjectIdentifier, const DevStudio::EntityTypeStruct& munitionType, const unsigned short& quantityFired, const unsigned short& rateOfFire, const DevStudio::RelativePositionStruct& relativeDetonationLocation, DataTypes::ObjectIdentifier& targetObjectIdentifier, const DevStudio::WarheadTypeEnum::WarheadTypeEnum& warheadType)
{
	if (!m_World)
	{
		std::cerr << "World is not initialized" << std::endl;
		return;
	}
	auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
	if (!interactionManager)
	{
		std::cerr << "Failed to retrieve HlaInteractionManager." << std::endl;
		return;
	}
	try
	{
		std::string firingObjectIdentifierString = firingObjectIdentifier.GetObjectIdentifierString(m_World);
		std::string munitionObjectIdentifierString = munitionObjectIdentifier.GetObjectIdentifierString(m_World);
		std::string targetObjectIdentifierString = targetObjectIdentifier.GetObjectIdentifierString(m_World);

		interactionManager.get()->sendMunitionDetonation(articulatedPartData, detonationLocation, detonationResultCode, eventIdentifier, firingObjectIdentifierString, finalVelocityVector, fuseType, munitionObjectIdentifierString, munitionType, quantityFired, rateOfFire, relativeDetonationLocation, targetObjectIdentifierString, warheadType);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error while sending munition detonation request: " << e.what() << std::endl;
	}
}

void SDL_RTI::ClientPitch::sendMagicMove(TaskParams& task, const DevStudio::MagicMoveTaskStruct& magicMoveParams)
{
	if (!m_World)
	{
		std::cerr << "World is not initialized" << std::endl;
		return;
	}
	auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
	if (!interactionManager)
	{
		std::cerr << "Failed to retrieve HlaInteractionManager." << std::endl;
		return;
	}
	try
	{

			interactionManager.get()->sendMagicMove
			(
				magicMoveParams,
				task.getCommunicationNetworks(),
				task.getTaskId(),
#ifdef __APPLE__
				0,
#endif
				task.getActivity(),
				task.getAnnotation(),
				task.getMainTask(),
			task.getPreviousTask(),
			task.getNextTask(),
			task.getTasker(),
			task.getTaskMode(),
			task.getEntity()
		);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error while sending move by route request: " << e.what() << std::endl;
	}
}

#ifdef SEOS
void SDL_RTI::ClientPitch::sendStopFreezeInteraction(int originatingEntityId, int originatingApplicationId, int originatingSiteId, int receivingEntityId, int receivingApplicationId, int receivingSiteId, double realWorldTime, unsigned int requestIdentifier, enumsCommon::ReasonEnum reason, bool reflectValues, bool runInternalSimulationClock, bool updateAttributes)
{
    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager)
    {
        std::cerr << "Interaction Manager is not initialized" << std::endl;
        return;
    }

    auto stopFreezeInteraction = interactionManager->getHlaStopFreezeInteraction();
    if (!stopFreezeInteraction)
    {
        std::cerr << "Failed to get HlaStopFreezeInteractionImpl instance." << std::endl;
        return;
    }
    DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum devStudioReason =
        static_cast<DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum>(reason);

    // Crear las estructuras necesarias
    DevStudio::EntityIdentifierStruct originatingEntity;
    originatingEntity.entityNumber = originatingEntityId;
    originatingEntity.federateIdentifier.applicationID = originatingApplicationId;
    originatingEntity.federateIdentifier.siteID = originatingSiteId;

    DevStudio::EntityIdentifierStruct receivingEntity;
    receivingEntity.entityNumber = receivingEntityId;
    receivingEntity.federateIdentifier.applicationID = receivingApplicationId;
    receivingEntity.federateIdentifier.siteID = receivingSiteId;

    DevStudio::ClockTimeStruct realWorldTimeStruct = {
        static_cast<int32_t>(std::floor(realWorldTime)),
        static_cast<uint32_t>((realWorldTime - std::floor(realWorldTime)) * 1e9)
    };

    stopFreezeInteraction->setOriginatingEntity(originatingEntity);
    stopFreezeInteraction->setReceivingEntity(receivingEntity);
    stopFreezeInteraction->setRealWorldTime(realWorldTimeStruct);
    stopFreezeInteraction->setRequestIdentifier(requestIdentifier);
    stopFreezeInteraction->setReason(devStudioReason);
    stopFreezeInteraction->setReflectValues(reflectValues);
    stopFreezeInteraction->setRunInternalSimulationClock(runInternalSimulationClock);
    stopFreezeInteraction->setUpdateAttributes(updateAttributes);
    stopFreezeInteraction->sendInteraction();
}
#endif
