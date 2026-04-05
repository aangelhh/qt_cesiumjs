#pragma once

#include "enumsCommon/EnumsCommons.h"
#include "DataTypes/EmitterBeamConverter.h"
#include "DataTypes/ObjectIdentifier.h"
#include "DataTypes/AttributeValuePair.h"
#include <DevStudio/datatypes/AttributeValuePairStruct.h>
#include <vector>

namespace SDL_RTI
{
	class PitchListener
	{
	public:
		// OBJECTS
		virtual void receivedObjectWrapperBaseEntity(DevStudio::HlaBaseEntityAttributesPtr* baseEntity, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperEmitterBeam(DevStudio::HlaEmitterBeamAttributesPtr* emitterBeam, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperRadarBeam(DevStudio::HlaRadarBeamAttributesPtr* radarBeam, enumsCommon_Client::ActionObjectEnum actionObjectEnum, SDL_RTI::EmitterBeamConverter* val, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperJammerBeam(DevStudio::HlaJammerBeamAttributesPtr* jammerBeam, enumsCommon_Client::ActionObjectEnum actionObjectEnum, SDL_RTI::EmitterBeamConverter* val, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperPhysicalEntity(DevStudio::HlaPhysicalEntityAttributesPtr* val, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperPlatform(DevStudio::HlaPlatformAttributesPtr* platform, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperAircraft(DevStudio::HlaAircraftAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperGroundVehicle(DevStudio::HlaGroundVehicleAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperSurfaceVessel(DevStudio::HlaSurfaceVesselAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperSubmersibleVessel(DevStudio::HlaSubmersibleVesselAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperSpacecraft(DevStudio::HlaSpacecraftAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperMunition(DevStudio::HlaMunitionAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperSupplies(DevStudio::HlaSuppliesAttributesPtr* aircraft, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperEmbeddedSystem(DevStudio::HlaEmbeddedSystemAttributesPtr* val, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperEmitterSystem(DevStudio::HlaEmitterSystemAttributesPtr* emitterSystem, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes){};
		virtual void receivedObjectWrapperExpendables(DevStudio::HlaExpendablesAttributesPtr* val, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes) {};
		virtual void receivedObjectWrapperAggregateEntity(DevStudio::HlaAggregateEntityAttributesPtr* val, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes) {};
		virtual void receivedObjectWrapperSensor(DevStudio::HlaSensorAttributesPtr* emitterSystem, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes) {};
		virtual void receivedObjectWrapperDesignator(DevStudio::HlaDesignatorAttributesPtr* emitterSystem, enumsCommon_Client::ActionObjectEnum actionObjectEnum, std::vector<int> modifiedAttributes) {};

		// INTERACTIONS
		virtual void receivedInteractionStartResume(DevStudio::HlaStartResumeParametersPtr* val){};
		virtual void receivedInteractionStartResumeAnswer(DevStudio::HlaStartResumeRParametersPtr* val){};
		virtual void receivedInteractionStopFreeze(DevStudio::HlaStopFreezeParametersPtr* val){};
		virtual void receivedInteractionStopFreezeAnswer(DevStudio::HlaStopFreezeRParametersPtr* val){};
		virtual void receivedInteractionAcknowledge(DevStudio::HlaAcknowledgeParametersPtr* val){};
		virtual void receivedInteractionCreateEntity(DevStudio::HlaCreateEntityParametersPtr* val){};
		virtual void receivedInteractionRemoveEntity(DevStudio::HlaRemoveEntityParametersPtr* val){};
		virtual void receivedInteractionCollision(DevStudio::HlaCollisionParametersPtr* val){};
		virtual void receivedInteractionWeaponFire(DevStudio::HlaWeaponFireParametersPtr* val){};
		virtual void receivedInteractionMunitionDetonation(DevStudio::HlaMunitionDetonationParametersPtr* val){};
		virtual void receivedInteractionResupplyCancel(DevStudio::HlaResupplyCancelParametersPtr* val){};
		virtual void receivedInteractionResupplyOffer(DevStudio::HlaResupplyOfferParametersPtr* val){};
		virtual void receivedInteractionResupplyReceived(DevStudio::HlaResupplyReceivedParametersPtr* val){};
		virtual void receivedInteractionRepairComplete(DevStudio::HlaRepairCompleteParametersPtr* val){};
		virtual void receivedInteractionRepairResponse(DevStudio::HlaRepairResponseParametersPtr* val){};
		virtual void receivedInteractionCreateObjectRequest(HlaObjectInstanceBase::ObjectClassType classType, std::vector<DataTypes::AttributeValuePair> attributeValuePairs){};
		virtual void receivedInteractionRemoveObjectRequest(std::vector<SDL_RTI::DataTypes::ObjectIdentifier>& val) {};
		virtual void receivedInteractionAttributeChangeRequest(std::vector<SDL_RTI::DataTypes::ObjectIdentifier> identifiers, std::vector<DataTypes::AttributeValuePair> attributeValuePairs) {};
		virtual void receivedInteractionAggregate(DevStudio::HlaAggregateParametersPtr* val) {};
		virtual void receivedInteractionDisaggregate(DevStudio::HlaDisaggregateParametersPtr* val) {};
		virtual void receivedInteractionDivide(DevStudio::HlaDivideParametersPtr* val) {};
		virtual void receivedInteractionMerge(DevStudio::HlaMergeParametersPtr* val) {};
		virtual void receivedInteractionRawBinaryRadioSignal(DevStudio::HlaRawBinaryRadioSignalParametersPtr* val) {};
		virtual void receivedInteractionDataQuery(DevStudio::HlaDataQueryParametersPtr* val) {};
		virtual void receivedInteractionData(DevStudio::HlaDataParametersPtr* val) {};

	};
}