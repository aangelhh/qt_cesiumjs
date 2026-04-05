#include "../include/InteractionManager.h"
#include <iostream>

void InteractionManager::sendCollision(
    const std::string& collidingObjectIdentifier,
    const float& issuingObjectMass,
    const VelocityVectorStruct& issuingObjectVelocityVector,
    const CollisionTypeEnum::CollisionTypeEnum& collisionType,
    const RelativePositionStruct& collisionLocation,
    const EventIdentifierStruct& eventIdentifier,
    const std::string& issuingObjectIdentifier
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction Collision " << std::endl;
}

void InteractionManager::sendStartResume(
    const EntityIdentifierStruct& originatingEntity,
    const EntityIdentifierStruct& receivingEntity,
    const ClockTimeStruct& realWorldTime,
    const unsigned int& requestIdentifier,
    const ClockTimeStruct& simulationTime
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction StartResume " << std::endl;
}

void InteractionManager::sendStopFreeze(
    const EntityIdentifierStruct& originatingEntity,
    const EntityIdentifierStruct& receivingEntity,
    const unsigned int& requestIdentifier,
    const ClockTimeStruct& realWorldTime,
    const StopFreezeReasonEnum::StopFreezeReasonEnum& reason,
    const bool& reflectValues,
    const bool& runInternalSimulationClock,
    const bool& updateAttributes
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction StopFreeze " << std::endl;
}

void InteractionManager::sendAcknowledge(
    const EntityIdentifierStruct& originatingEntity,
    const EntityIdentifierStruct& receivingEntity,
    const unsigned int& requestIdentifier,
    const DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum& acknowledgeFlag,
    const DevStudio::ResponseFlagEnum::ResponseFlagEnum& responseFlag
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction Acknowledge " << std::endl;
}

void InteractionManager::sendCreateEntity(
    const EntityIdentifierStruct& originatingEntity,
    const EntityIdentifierStruct& receivingEntity,
    const unsigned int& requestIdentifier
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction CreateEntity " << std::endl;
}

void InteractionManager::sendRemoveEntity(
    const EntityIdentifierStruct& originatingEntity,
    const EntityIdentifierStruct& receivingEntity,
    const unsigned int& requestIdentifier
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction RemoveEntity " << std::endl;
}

void InteractionManager::sendMunitionDetonation(
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
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction MunitionDetonation " << std::endl;
}

void InteractionManager::sendResupplyCancel(
    const std::string& receivingObject,
    const std::string& supplyingObject
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction ResupplyCancel " << std::endl;
}

void InteractionManager::sendResupplyOffer(
    const std::string& receivingObject,
    const std::string& supplyingObject,
    const std::vector<DevStudio::SupplyStruct >& suppliesData
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction ResupplyOffer " << std::endl;
}

void InteractionManager::sendResupplyReceived(
    const std::string& receivingObject,
    const std::string& supplyingObject,
    const std::vector<DevStudio::SupplyStruct >& suppliesData
) THROW_SPEC(HlaNotConnectedException, HlaFomException, HlaInternalException, HlaRtiException,
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction ResupplyReceived " << std::endl;
}

void InteractionManager::sendWeaponFire(
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
    HlaSaveInProgressException, HlaRestoreInProgressException) {
    std::cout << "Send interaction WeaponFire " << std::endl;
}