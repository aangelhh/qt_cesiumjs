#pragma once
#include <DevStudio/HlaInteractionManager.h>
#include "../ClientPitch.h"
#include <set>

#include <iostream>

#include <DevStudio/HlaLibSettings.h>
#include <DevStudio/HlaPointers.h>
#include <DevStudio/HlaException.h>
#include <DevStudio/HlaInteractionListener.h>

using namespace DevStudio;

class InteractionManager : public DevStudio::HlaInteractionManager
{
    void sendCollision(
        const std::string& collidingObjectIdentifier,
        const float& issuingObjectMass,
        const VelocityVectorStruct& issuingObjectVelocityVector,
        const CollisionTypeEnum::CollisionTypeEnum& collisionType,
        const RelativePositionStruct& collisionLocation,
        const EventIdentifierStruct& eventIdentifier,
        const std::string& issuingObjectIdentifier
    ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
        HlaSaveInProgressException, HlaRestoreInProgressException) override;

    void sendStartResume(
        const EntityIdentifierStruct& originatingEntity,
        const EntityIdentifierStruct& receivingEntity,
        const ClockTimeStruct& realWorldTime,
        const unsigned int& requestIdentifier,
        const ClockTimeStruct& simulationTime
    ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
        HlaSaveInProgressException, HlaRestoreInProgressException) override;

    void sendStopFreeze(
        const EntityIdentifierStruct& originatingEntity,
        const EntityIdentifierStruct& receivingEntity,
        const unsigned int& requestIdentifier,
        const ClockTimeStruct& realWorldTime,
        const StopFreezeReasonEnum::StopFreezeReasonEnum& reason,
        const bool& reflectValues,
        const bool& runInternalSimulationClock,
        const bool& updateAttributes
    ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
        HlaSaveInProgressException, HlaRestoreInProgressException) override;

    void sendAcknowledge(
        const EntityIdentifierStruct& originatingEntity,
        const EntityIdentifierStruct& receivingEntity,
        const unsigned int& requestIdentifier,
        const DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum& acknowledgeFlag,
        const DevStudio::ResponseFlagEnum::ResponseFlagEnum& responseFlag
    ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
        HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendCreateEntity(
        const EntityIdentifierStruct& originatingEntity,
        const EntityIdentifierStruct& receivingEntity,
        const unsigned int& requestIdentifier
    ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
        HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendRemoveEntity(
         const EntityIdentifierStruct& originatingEntity,
         const EntityIdentifierStruct& receivingEntity,
         const unsigned int& requestIdentifier
     ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
         HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendMunitionDetonation(
         const std::vector<DevStudio::ArticulatedParameterStruct >& articulatedPartData,
         const WorldLocationStruct& detonationLocation,
         const DetonationResultCodeEnum::DetonationResultCodeEnum& detonationResultCode,
         const EventIdentifierStruct& eventIdentifier,
         const std::string& firingObjectIdentifier,
         const VelocityVectorStruct& finalVelocityVector,
         const FuseTypeEnum::FuseTypeEnum& fuseType,
         const std::string& munitionObjectIdentifier,
         const EntityTypeStruct& munitionType,
         const unsigned short& quantityFired,
         const unsigned short& rateOfFire,
         const RelativePositionStruct& relativeDetonationLocation,
         const std::string& targetObjectIdentifier,
         const WarheadTypeEnum::WarheadTypeEnum& warheadType
     ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
         HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendResupplyCancel(
         const std::string& receivingObject,
         const std::string& supplyingObject
     ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
         HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendResupplyOffer(
         const std::string& receivingObject,
         const std::string& supplyingObject,
         const std::vector<DevStudio::SupplyStruct >& suppliesData
     ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
         HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendResupplyReceived(
         const std::string& receivingObject,
         const std::string& supplyingObject,
         const std::vector<DevStudio::SupplyStruct >& suppliesData
     ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
         HlaSaveInProgressException, HlaRestoreInProgressException) override;

     void sendWeaponFire(
         const EventIdentifierStruct& eventIdentifier,
         const float& fireControlSolutionRange,
         const unsigned int& fireMissionIndex,
         const WorldLocationStruct& firingLocation,
         const std::string& firingObjectIdentifier,
         const FuseTypeEnum::FuseTypeEnum& fuseType,
         const VelocityVectorStruct& initialVelocityVector,
         const std::string& munitionObjectIdentifier,
         const EntityTypeStruct& munitionType,
         const unsigned short& quantityFired,
         const unsigned short& rateOfFire,
         const std::string& targetObjectIdentifier,
         const WarheadTypeEnum::WarheadTypeEnum& warheadType
     ) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
         HlaSaveInProgressException, HlaRestoreInProgressException) override;
};

