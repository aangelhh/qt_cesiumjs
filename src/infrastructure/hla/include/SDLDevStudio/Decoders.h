#ifndef DEVELOPER_STUDIO_DECODERS_H
#define DEVELOPER_STUDIO_DECODERS_H

#include <mutex>

#include <RtiDriver/Encoding/BasicDataElements.h>
#include <DevStudio/HlaLibSettings.h>
#include <RtiDriver/Encoding/AbstractDataElement.h>
#include <RtiDriver/Encoding/HLAfixedRecord.h>


//pragma region Auto-generated Includes
// === AUTO-GENERATED INCLUDES BEGIN ===
#include <DevStudio/datatypes/ATP45HazardAreaTypeEnum.h>
#include <DevStudio/datatypes/ATP45HazardAreaTypeEnumEncoder.h>
#include <DevStudio/datatypes/AccelerationVectorStruct.h>
#include <DevStudio/datatypes/AccelerationVectorStructEncoder.h>
#include <DevStudio/datatypes/AcknowledgeFlagEnum.h>
#include <DevStudio/datatypes/AcknowledgeFlagEnumEncoder.h>
#include <DevStudio/datatypes/AcknowledgementProtocolEnum.h>
#include <DevStudio/datatypes/AcknowledgementProtocolEnumEncoder.h>
#include <DevStudio/datatypes/ActionEnum.h>
#include <DevStudio/datatypes/ActionEnumEncoder.h>
#include <DevStudio/datatypes/ActionResultEnum.h>
#include <DevStudio/datatypes/ActionResultEnumEncoder.h>
#include <DevStudio/datatypes/ActiveSonarEnum.h>
#include <DevStudio/datatypes/ActiveSonarEnumEncoder.h>
#include <DevStudio/datatypes/ActiveSonarFunctionCodeEnum.h>
#include <DevStudio/datatypes/ActiveSonarFunctionCodeEnumEncoder.h>
#include <DevStudio/datatypes/ActiveSonarScanPatternEnum.h>
#include <DevStudio/datatypes/ActiveSonarScanPatternEnumEncoder.h>
#include <DevStudio/datatypes/ActiveStatusEnum.h>
#include <DevStudio/datatypes/ActiveStatusEnumEncoder.h>
#include <DevStudio/datatypes/AdministerTreatmentTaskStruct.h>
#include <DevStudio/datatypes/AdministerTreatmentTaskStructEncoder.h>
#include <DevStudio/datatypes/AgentClassEnum.h>
#include <DevStudio/datatypes/AgentClassEnumEncoder.h>
#include <DevStudio/datatypes/AgentConcentrationStruct.h>
#include <DevStudio/datatypes/AgentConcentrationStructEncoder.h>
#include <DevStudio/datatypes/AgentMassStruct.h>
#include <DevStudio/datatypes/AgentMassStructEncoder.h>
#include <DevStudio/datatypes/AgentTypeEnum.h>
#include <DevStudio/datatypes/AgentTypeEnumEncoder.h>
#include <DevStudio/datatypes/AggregateMarkingStruct.h>
#include <DevStudio/datatypes/AggregateMarkingStructEncoder.h>
#include <DevStudio/datatypes/AggregateMissionEnum.h>
#include <DevStudio/datatypes/AggregateMissionEnumEncoder.h>
#include <DevStudio/datatypes/AggregateStateEnum.h>
#include <DevStudio/datatypes/AggregateStateEnumEncoder.h>
#include <DevStudio/datatypes/AggregateStateFormationEnum.h>
#include <DevStudio/datatypes/AggregateStateFormationEnumEncoder.h>
#include <DevStudio/datatypes/AidTypeEnum.h>
#include <DevStudio/datatypes/AidTypeEnumEncoder.h>
#include <DevStudio/datatypes/AirFormationTypeEnum.h>
#include <DevStudio/datatypes/AirFormationTypeEnumEncoder.h>
#include <DevStudio/datatypes/AmplitudeAngleModulationTypeEnum.h>
#include <DevStudio/datatypes/AmplitudeAngleModulationTypeEnumEncoder.h>
#include <DevStudio/datatypes/AmplitudeModulationTypeEnum.h>
#include <DevStudio/datatypes/AmplitudeModulationTypeEnumEncoder.h>
#include <DevStudio/datatypes/AngleModulationTypeEnum.h>
#include <DevStudio/datatypes/AngleModulationTypeEnumEncoder.h>
#include <DevStudio/datatypes/AngularVelocityVectorStruct.h>
#include <DevStudio/datatypes/AngularVelocityVectorStructEncoder.h>
#include <DevStudio/datatypes/AntennaPatternTypeEnum.h>
#include <DevStudio/datatypes/AntennaPatternTypeEnumEncoder.h>
#include <DevStudio/datatypes/AntennaPatternVariantStruct.h>
#include <DevStudio/datatypes/AntennaPatternVariantStructEncoder.h>
#include <DevStudio/datatypes/AntennaPatternVariantStructLengthlessArray.h>
#include <DevStudio/datatypes/AntennaPatternVariantStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/AntiCollisionLightFlashingEnum.h>
#include <DevStudio/datatypes/AntiCollisionLightFlashingEnumEncoder.h>
#include <DevStudio/datatypes/AppearanceAntiCollisionDayNightEnum.h>
#include <DevStudio/datatypes/AppearanceAntiCollisionDayNightEnumEncoder.h>
#include <DevStudio/datatypes/AppearanceNVGModeEnum.h>
#include <DevStudio/datatypes/AppearanceNVGModeEnumEncoder.h>
#include <DevStudio/datatypes/ApplyIPETaskStruct.h>
#include <DevStudio/datatypes/ApplyIPETaskStructEncoder.h>
#include <DevStudio/datatypes/AppointmentStruct.h>
#include <DevStudio/datatypes/AppointmentStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfAgentConcentrationStruct.h>
#include <DevStudio/datatypes/ArrayOfAgentConcentrationStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfAgentMassStruct.h>
#include <DevStudio/datatypes/ArrayOfAgentMassStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfAgentTypeEnum.h>
#include <DevStudio/datatypes/ArrayOfAgentTypeEnumEncoder.h>
#include <DevStudio/datatypes/ArrayOfCBRNExposureStruct.h>
#include <DevStudio/datatypes/ArrayOfCBRNExposureStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfCBRNSensorReadingStruct.h>
#include <DevStudio/datatypes/ArrayOfCBRNSensorReadingStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfCommunicationNetworks.h>
#include <DevStudio/datatypes/ArrayOfCommunicationNetworksEncoder.h>
#include <DevStudio/datatypes/ArrayOfContourStruct.h>
#include <DevStudio/datatypes/ArrayOfContourStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfDetectedEquipment.h>
#include <DevStudio/datatypes/ArrayOfDetectedEquipmentEncoder.h>
#include <DevStudio/datatypes/ArrayOfHoldings.h>
#include <DevStudio/datatypes/ArrayOfHoldingsEncoder.h>
#include <DevStudio/datatypes/ArrayOfObservedEquipment.h>
#include <DevStudio/datatypes/ArrayOfObservedEquipmentEncoder.h>
#include <DevStudio/datatypes/ArrayOfProtectionEffectivenessStruct.h>
#include <DevStudio/datatypes/ArrayOfProtectionEffectivenessStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfRelationship.h>
#include <DevStudio/datatypes/ArrayOfRelationshipEncoder.h>
#include <DevStudio/datatypes/ArrayOfResourceStatus.h>
#include <DevStudio/datatypes/ArrayOfResourceStatusEncoder.h>
#include <DevStudio/datatypes/ArrayOfSensor.h>
#include <DevStudio/datatypes/ArrayOfSensorEncoder.h>
#include <DevStudio/datatypes/ArrayOfSigmas6.h>
#include <DevStudio/datatypes/ArrayOfSigmas6Encoder.h>
#include <DevStudio/datatypes/ArrayOfSignature.h>
#include <DevStudio/datatypes/ArrayOfSignatureEncoder.h>
#include <DevStudio/datatypes/ArrayOfSupplyStatus.h>
#include <DevStudio/datatypes/ArrayOfSupplyStatusEncoder.h>
#include <DevStudio/datatypes/ArrayOfTaskDefinitions.h>
#include <DevStudio/datatypes/ArrayOfTaskDefinitionsEncoder.h>
#include <DevStudio/datatypes/ArrayOfTaskProgress.h>
#include <DevStudio/datatypes/ArrayOfTaskProgressEncoder.h>
#include <DevStudio/datatypes/ArrayOfTrack.h>
#include <DevStudio/datatypes/ArrayOfTrackEncoder.h>
#include <DevStudio/datatypes/ArrayOfTreatmentStruct.h>
#include <DevStudio/datatypes/ArrayOfTreatmentStructEncoder.h>
#include <DevStudio/datatypes/ArrayOfUuid.h>
#include <DevStudio/datatypes/ArrayOfUuidEncoder.h>
#include <DevStudio/datatypes/ArrayOfWaypoints.h>
#include <DevStudio/datatypes/ArrayOfWaypointsEncoder.h>
#include <DevStudio/datatypes/ArticulatedParameterStruct.h>
#include <DevStudio/datatypes/ArticulatedParameterStructEncoder.h>
#include <DevStudio/datatypes/ArticulatedParameterStructLengthlessArray.h>
#include <DevStudio/datatypes/ArticulatedParameterStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/ArticulatedPartsStruct.h>
#include <DevStudio/datatypes/ArticulatedPartsStructEncoder.h>
#include <DevStudio/datatypes/ArticulatedPartsTypeEnum.h>
#include <DevStudio/datatypes/ArticulatedPartsTypeEnumEncoder.h>
#include <DevStudio/datatypes/ArticulatedTypeMetricEnum.h>
#include <DevStudio/datatypes/ArticulatedTypeMetricEnumEncoder.h>
#include <DevStudio/datatypes/AttachTaskStruct.h>
#include <DevStudio/datatypes/AttachTaskStructEncoder.h>
#include <DevStudio/datatypes/AttachedPartsStruct.h>
#include <DevStudio/datatypes/AttachedPartsStructEncoder.h>
#include <DevStudio/datatypes/AttackTypeEnum.h>
#include <DevStudio/datatypes/AttackTypeEnumEncoder.h>
#include <DevStudio/datatypes/AttributeValuePairStruct.h>
#include <DevStudio/datatypes/AttributeValuePairStructArray1Plus.h>
#include <DevStudio/datatypes/AttributeValuePairStructArray1PlusEncoder.h>
#include <DevStudio/datatypes/AttributeValuePairStructEncoder.h>
#include <DevStudio/datatypes/AudioDataTypeStruct.h>
#include <DevStudio/datatypes/AudioDataTypeStructEncoder.h>
#include <DevStudio/datatypes/BaseEncoder.h>
#include <DevStudio/datatypes/BeamAntennaStruct.h>
#include <DevStudio/datatypes/BeamAntennaStructEncoder.h>
#include <DevStudio/datatypes/BeamFunctionCodeEnum.h>
#include <DevStudio/datatypes/BeamFunctionCodeEnumEncoder.h>
#include <DevStudio/datatypes/BinArray.h>
#include <DevStudio/datatypes/BinArrayEncoder.h>
#include <DevStudio/datatypes/BreachStruct.h>
#include <DevStudio/datatypes/BreachStructEncoder.h>
#include <DevStudio/datatypes/BreachStructLengthlessArray.h>
#include <DevStudio/datatypes/BreachStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/BreachableSegmentStruct.h>
#include <DevStudio/datatypes/BreachableSegmentStructEncoder.h>
#include <DevStudio/datatypes/BreachableSegmentStructLengthlessArray.h>
#include <DevStudio/datatypes/BreachableSegmentStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/BreachedStatusArray8.h>
#include <DevStudio/datatypes/BreachedStatusArray8Encoder.h>
#include <DevStudio/datatypes/BreachedStatusEnum.h>
#include <DevStudio/datatypes/BreachedStatusEnumEncoder.h>
#include <DevStudio/datatypes/CBRNAlarmStruct.h>
#include <DevStudio/datatypes/CBRNAlarmStructEncoder.h>
#include <DevStudio/datatypes/CBRNDamageEnum.h>
#include <DevStudio/datatypes/CBRNDamageEnumEncoder.h>
#include <DevStudio/datatypes/CBRNExposureStruct.h>
#include <DevStudio/datatypes/CBRNExposureStructEncoder.h>
#include <DevStudio/datatypes/CBRNSensorReadingStruct.h>
#include <DevStudio/datatypes/CBRNSensorReadingStructEncoder.h>
#include <DevStudio/datatypes/COMBICStateRecStruct.h>
#include <DevStudio/datatypes/COMBICStateRecStructEncoder.h>
#include <DevStudio/datatypes/CamouflageEnum.h>
#include <DevStudio/datatypes/CamouflageEnumEncoder.h>
#include <DevStudio/datatypes/CaptureStatusEnum.h>
#include <DevStudio/datatypes/CaptureStatusEnumEncoder.h>
#include <DevStudio/datatypes/ChangeAltitudeTaskStruct.h>
#include <DevStudio/datatypes/ChangeAltitudeTaskStructEncoder.h>
#include <DevStudio/datatypes/ChangeHeadingTaskStruct.h>
#include <DevStudio/datatypes/ChangeHeadingTaskStructEncoder.h>
#include <DevStudio/datatypes/ChangeRulesOfEngagementTaskStruct.h>
#include <DevStudio/datatypes/ChangeRulesOfEngagementTaskStructEncoder.h>
#include <DevStudio/datatypes/ChangeSpeedTaskStruct.h>
#include <DevStudio/datatypes/ChangeSpeedTaskStructEncoder.h>
#include <DevStudio/datatypes/ChemicalContentEnum.h>
#include <DevStudio/datatypes/ChemicalContentEnumEncoder.h>
#include <DevStudio/datatypes/ClearEngineeringTaskStruct.h>
#include <DevStudio/datatypes/ClearEngineeringTaskStructEncoder.h>
#include <DevStudio/datatypes/ClockTimeStruct.h>
#include <DevStudio/datatypes/ClockTimeStructEncoder.h>
#include <DevStudio/datatypes/ClockTimeStructLengthlessArray.h>
#include <DevStudio/datatypes/ClockTimeStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/CloudStruct.h>
#include <DevStudio/datatypes/CloudStructEncoder.h>
#include <DevStudio/datatypes/CloudTypeEnum.h>
#include <DevStudio/datatypes/CloudTypeEnumEncoder.h>
#include <DevStudio/datatypes/CoefficientsLengthlessArray1Plus.h>
#include <DevStudio/datatypes/CoefficientsLengthlessArray1PlusEncoder.h>
#include <DevStudio/datatypes/CollisionTypeEnum.h>
#include <DevStudio/datatypes/CollisionTypeEnumEncoder.h>
#include <DevStudio/datatypes/CombinationModulationTypeEnum.h>
#include <DevStudio/datatypes/CombinationModulationTypeEnumEncoder.h>
#include <DevStudio/datatypes/CommunicationNetworkArray.h>
#include <DevStudio/datatypes/CommunicationNetworkArrayEncoder.h>
#include <DevStudio/datatypes/CommunicationNetworkTypeEnum.h>
#include <DevStudio/datatypes/CommunicationNetworkTypeEnumEncoder.h>
#include <DevStudio/datatypes/CommunicationServiceTypeEnum.h>
#include <DevStudio/datatypes/CommunicationServiceTypeEnumEncoder.h>
#include <DevStudio/datatypes/ComplianceStateEnum.h>
#include <DevStudio/datatypes/ComplianceStateEnumEncoder.h>
#include <DevStudio/datatypes/Cone1GeomRecStruct.h>
#include <DevStudio/datatypes/Cone1GeomRecStructEncoder.h>
#include <DevStudio/datatypes/Cone2GeomRecStruct.h>
#include <DevStudio/datatypes/Cone2GeomRecStructEncoder.h>
#include <DevStudio/datatypes/ConnectionReceiverArray.h>
#include <DevStudio/datatypes/ConnectionReceiverArrayEncoder.h>
#include <DevStudio/datatypes/ConnectionReceiverStruct.h>
#include <DevStudio/datatypes/ConnectionReceiverStructEncoder.h>
#include <DevStudio/datatypes/ConnectionTypeEnum.h>
#include <DevStudio/datatypes/ConnectionTypeEnumEncoder.h>
#include <DevStudio/datatypes/ConstituentPartNatureEnum.h>
#include <DevStudio/datatypes/ConstituentPartNatureEnumEncoder.h>
#include <DevStudio/datatypes/ConstituentPartPositionEnum.h>
#include <DevStudio/datatypes/ConstituentPartPositionEnumEncoder.h>
#include <DevStudio/datatypes/ConstituentPartRelationshipStruct.h>
#include <DevStudio/datatypes/ConstituentPartRelationshipStructEncoder.h>
#include <DevStudio/datatypes/ConstituentPartStationNameEnum.h>
#include <DevStudio/datatypes/ConstituentPartStationNameEnumEncoder.h>
#include <DevStudio/datatypes/ContourStruct.h>
#include <DevStudio/datatypes/ContourStructEncoder.h>
#include <DevStudio/datatypes/CreateBreachTaskStruct.h>
#include <DevStudio/datatypes/CreateBreachTaskStructEncoder.h>
#include <DevStudio/datatypes/CreateFireBreakTaskStruct.h>
#include <DevStudio/datatypes/CreateFireBreakTaskStructEncoder.h>
#include <DevStudio/datatypes/CreateLeveeTaskStruct.h>
#include <DevStudio/datatypes/CreateLeveeTaskStructEncoder.h>
#include <DevStudio/datatypes/CreateObstacleTaskStruct.h>
#include <DevStudio/datatypes/CreateObstacleTaskStructEncoder.h>
#include <DevStudio/datatypes/CryptographicModeEnum.h>
#include <DevStudio/datatypes/CryptographicModeEnumEncoder.h>
#include <DevStudio/datatypes/CryptographicSystemTypeEnum.h>
#include <DevStudio/datatypes/CryptographicSystemTypeEnumEncoder.h>
#include <DevStudio/datatypes/CurrentStruct.h>
#include <DevStudio/datatypes/CurrentStructEncoder.h>
#include <DevStudio/datatypes/DamageStatusEnhancedEnum.h>
#include <DevStudio/datatypes/DamageStatusEnhancedEnumEncoder.h>
#include <DevStudio/datatypes/DamageStatusEnum.h>
#include <DevStudio/datatypes/DamageStatusEnumEncoder.h>
#include <DevStudio/datatypes/DatumIdentifierEnum.h>
#include <DevStudio/datatypes/DatumIdentifierEnumEncoder.h>
#include <DevStudio/datatypes/DatumIdentifierLengthlessArray.h>
#include <DevStudio/datatypes/DatumIdentifierLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/DeadReckoningAlgorithmEnum.h>
#include <DevStudio/datatypes/DeadReckoningAlgorithmEnumEncoder.h>
#include <DevStudio/datatypes/DepthMeterFloat32LengthlessArray.h>
#include <DevStudio/datatypes/DepthMeterFloat32LengthlessArrayEncoder.h>
#include <DevStudio/datatypes/DesignatorCodeEnum.h>
#include <DevStudio/datatypes/DesignatorCodeEnumEncoder.h>
#include <DevStudio/datatypes/DesignatorCodeNameEnum.h>
#include <DevStudio/datatypes/DesignatorCodeNameEnumEncoder.h>
#include <DevStudio/datatypes/DetectedEquipment.h>
#include <DevStudio/datatypes/DetectedEquipmentEncoder.h>
#include <DevStudio/datatypes/DetonationResultCodeEnum.h>
#include <DevStudio/datatypes/DetonationResultCodeEnumEncoder.h>
#include <DevStudio/datatypes/DimensionRateStruct.h>
#include <DevStudio/datatypes/DimensionRateStructEncoder.h>
#include <DevStudio/datatypes/DimensionStruct.h>
#include <DevStudio/datatypes/DimensionStructEncoder.h>
#include <DevStudio/datatypes/DirectFireTaskStruct.h>
#include <DevStudio/datatypes/DirectFireTaskStructEncoder.h>
#include <DevStudio/datatypes/DisruptCommunicationTaskStruct.h>
#include <DevStudio/datatypes/DisruptCommunicationTaskStructEncoder.h>
#include <DevStudio/datatypes/EchelonEnum.h>
#include <DevStudio/datatypes/EchelonEnumEncoder.h>
#include <DevStudio/datatypes/ElapsedTimeProgress.h>
#include <DevStudio/datatypes/ElapsedTimeProgressEncoder.h>
#include <DevStudio/datatypes/Ellipsoid1GeomRecStruct.h>
#include <DevStudio/datatypes/Ellipsoid1GeomRecStructEncoder.h>
#include <DevStudio/datatypes/Ellipsoid2GeomRecStruct.h>
#include <DevStudio/datatypes/Ellipsoid2GeomRecStructEncoder.h>
#include <DevStudio/datatypes/EmitterFunctionEnum.h>
#include <DevStudio/datatypes/EmitterFunctionEnumEncoder.h>
#include <DevStudio/datatypes/EmitterTypeEnum.h>
#include <DevStudio/datatypes/EmitterTypeEnumEncoder.h>
#include <DevStudio/datatypes/EmptyTaskStruct.h>
#include <DevStudio/datatypes/EmptyTaskStructEncoder.h>
#include <DevStudio/datatypes/EncodingTypeEnum.h>
#include <DevStudio/datatypes/EncodingTypeEnumEncoder.h>
#include <DevStudio/datatypes/EngineeringTaskStruct.h>
#include <DevStudio/datatypes/EngineeringTaskStructEncoder.h>
#include <DevStudio/datatypes/EntityControlActionEnum.h>
#include <DevStudio/datatypes/EntityControlActionEnumEncoder.h>
#include <DevStudio/datatypes/EntityControlActions.h>
#include <DevStudio/datatypes/EntityControlActionsEncoder.h>
#include <DevStudio/datatypes/EntityCoordinateVectorStruct.h>
#include <DevStudio/datatypes/EntityCoordinateVectorStructEncoder.h>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>
#include <DevStudio/datatypes/EntityIdentifierStructEncoder.h>
#include <DevStudio/datatypes/EntityTypeStruct.h>
#include <DevStudio/datatypes/EntityTypeStructEncoder.h>
#include <DevStudio/datatypes/EntityTypeStructLengthlessArray.h>
#include <DevStudio/datatypes/EntityTypeStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/EnvironmentConditionModelTypeEnum.h>
#include <DevStudio/datatypes/EnvironmentConditionModelTypeEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentDataCoordinateSystemEnum.h>
#include <DevStudio/datatypes/EnvironmentDataCoordinateSystemEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentDataRepresentationEnum.h>
#include <DevStudio/datatypes/EnvironmentDataRepresentationEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentDataSampleTypeEnum.h>
#include <DevStudio/datatypes/EnvironmentDataSampleTypeEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentGridAxisTypeEnum.h>
#include <DevStudio/datatypes/EnvironmentGridAxisTypeEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentGridTypeEnum.h>
#include <DevStudio/datatypes/EnvironmentGridTypeEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentModelTypeEnum.h>
#include <DevStudio/datatypes/EnvironmentModelTypeEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentObjectTypeStruct.h>
#include <DevStudio/datatypes/EnvironmentObjectTypeStructEncoder.h>
#include <DevStudio/datatypes/EnvironmentRecStruct.h>
#include <DevStudio/datatypes/EnvironmentRecStructArray.h>
#include <DevStudio/datatypes/EnvironmentRecStructArrayEncoder.h>
#include <DevStudio/datatypes/EnvironmentRecStructEncoder.h>
#include <DevStudio/datatypes/EnvironmentRecVariantStruct.h>
#include <DevStudio/datatypes/EnvironmentRecVariantStructEncoder.h>
#include <DevStudio/datatypes/EnvironmentRecordTypeEnum.h>
#include <DevStudio/datatypes/EnvironmentRecordTypeEnumEncoder.h>
#include <DevStudio/datatypes/EnvironmentTypeStruct.h>
#include <DevStudio/datatypes/EnvironmentTypeStructEncoder.h>
#include <DevStudio/datatypes/EquipmentSymbolAmplificationStruct.h>
#include <DevStudio/datatypes/EquipmentSymbolAmplificationStructEncoder.h>
#include <DevStudio/datatypes/EstablishCheckpointTaskStruct.h>
#include <DevStudio/datatypes/EstablishCheckpointTaskStructEncoder.h>
#include <DevStudio/datatypes/EstablishObservationPostTaskStruct.h>
#include <DevStudio/datatypes/EstablishObservationPostTaskStructEncoder.h>
#include <DevStudio/datatypes/EvacuateProgressStruct.h>
#include <DevStudio/datatypes/EvacuateProgressStructEncoder.h>
#include <DevStudio/datatypes/EvacuateTaskStruct.h>
#include <DevStudio/datatypes/EvacuateTaskStructEncoder.h>
#include <DevStudio/datatypes/EventIdentifierStruct.h>
#include <DevStudio/datatypes/EventIdentifierStructEncoder.h>
#include <DevStudio/datatypes/EventTypeEnum.h>
#include <DevStudio/datatypes/EventTypeEnumEncoder.h>
#include <DevStudio/datatypes/ExhaustSmokeStruct.h>
#include <DevStudio/datatypes/ExhaustSmokeStructEncoder.h>
#include <DevStudio/datatypes/ExhaustSmokeStructLengthlessArray.h>
#include <DevStudio/datatypes/ExhaustSmokeStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/ExposureTypeEnum.h>
#include <DevStudio/datatypes/ExposureTypeEnumEncoder.h>
#include <DevStudio/datatypes/ExtinguishWildfireTaskStruct.h>
#include <DevStudio/datatypes/ExtinguishWildfireTaskStructEncoder.h>
#include <DevStudio/datatypes/FederateControlActionEnum.h>
#include <DevStudio/datatypes/FederateControlActionEnumEncoder.h>
#include <DevStudio/datatypes/FederateControlActions.h>
#include <DevStudio/datatypes/FederateControlActionsEncoder.h>
#include <DevStudio/datatypes/FederateIdentifierStruct.h>
#include <DevStudio/datatypes/FederateIdentifierStructEncoder.h>
#include <DevStudio/datatypes/FireTaskProgress.h>
#include <DevStudio/datatypes/FireTaskProgressEncoder.h>
#include <DevStudio/datatypes/FixedDatumStruct.h>
#include <DevStudio/datatypes/FixedDatumStructEncoder.h>
#include <DevStudio/datatypes/FixedDatumStructLengthlessArray.h>
#include <DevStudio/datatypes/FixedDatumStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/FlareStateRecStruct.h>
#include <DevStudio/datatypes/FlareStateRecStructEncoder.h>
#include <DevStudio/datatypes/Float32Array1Plus.h>
#include <DevStudio/datatypes/Float32Array1PlusEncoder.h>
#include <DevStudio/datatypes/FollowEntityTaskStruct.h>
#include <DevStudio/datatypes/FollowEntityTaskStructEncoder.h>
#include <DevStudio/datatypes/ForceIdentifierEnum.h>
#include <DevStudio/datatypes/ForceIdentifierEnumEncoder.h>
#include <DevStudio/datatypes/FormationDataStruct.h>
#include <DevStudio/datatypes/FormationDataStructEncoder.h>
#include <DevStudio/datatypes/FormationEnum.h>
#include <DevStudio/datatypes/FormationEnumEncoder.h>
#include <DevStudio/datatypes/FormationLocationTypeEnum.h>
#include <DevStudio/datatypes/FormationLocationTypeEnumEncoder.h>
#include <DevStudio/datatypes/FormationStruct.h>
#include <DevStudio/datatypes/FormationStructEncoder.h>
#include <DevStudio/datatypes/FormationTypeEnum.h>
#include <DevStudio/datatypes/FormationTypeEnumEncoder.h>
#include <DevStudio/datatypes/FundamentalParameterDataStruct.h>
#include <DevStudio/datatypes/FundamentalParameterDataStructEncoder.h>
#include <DevStudio/datatypes/FundamentalParameterDataStructLengthlessArray.h>
#include <DevStudio/datatypes/FundamentalParameterDataStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/FuseTypeEnum.h>
#include <DevStudio/datatypes/FuseTypeEnumEncoder.h>
#include <DevStudio/datatypes/GaussPlumeGeomRecStruct.h>
#include <DevStudio/datatypes/GaussPlumeGeomRecStructEncoder.h>
#include <DevStudio/datatypes/GaussPuffGeomRecStruct.h>
#include <DevStudio/datatypes/GaussPuffGeomRecStructEncoder.h>
#include <DevStudio/datatypes/GeoLocationTypeEnum.h>
#include <DevStudio/datatypes/GeoLocationTypeEnumEncoder.h>
#include <DevStudio/datatypes/GeoReferenceVariant.h>
#include <DevStudio/datatypes/GeoReferenceVariantEncoder.h>
#include <DevStudio/datatypes/GeodeticCircle.h>
#include <DevStudio/datatypes/GeodeticCircleEncoder.h>
#include <DevStudio/datatypes/GeodeticLocation.h>
#include <DevStudio/datatypes/GeodeticLocationEncoder.h>
#include <DevStudio/datatypes/GeodeticPath.h>
#include <DevStudio/datatypes/GeodeticPathEncoder.h>
#include <DevStudio/datatypes/GeodeticPoint.h>
#include <DevStudio/datatypes/GeodeticPointEncoder.h>
#include <DevStudio/datatypes/GeodeticPolygon.h>
#include <DevStudio/datatypes/GeodeticPolygonEncoder.h>
#include <DevStudio/datatypes/GeodeticQuadrangle.h>
#include <DevStudio/datatypes/GeodeticQuadrangleEncoder.h>
#include <DevStudio/datatypes/GridAxisStruct.h>
#include <DevStudio/datatypes/GridAxisStructEncoder.h>
#include <DevStudio/datatypes/GridAxisStructLengthlessArray.h>
#include <DevStudio/datatypes/GridAxisStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/GridAxisTypeVariantStruct.h>
#include <DevStudio/datatypes/GridAxisTypeVariantStructEncoder.h>
#include <DevStudio/datatypes/GridDataRepresentationVariantStruct.h>
#include <DevStudio/datatypes/GridDataRepresentationVariantStructEncoder.h>
#include <DevStudio/datatypes/GridDataStruct.h>
#include <DevStudio/datatypes/GridDataStructEncoder.h>
#include <DevStudio/datatypes/GridDataStructLengthlessArray.h>
#include <DevStudio/datatypes/GridDataStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/GridValueType0Struct.h>
#include <DevStudio/datatypes/GridValueType0StructEncoder.h>
#include <DevStudio/datatypes/GridValueType1Struct.h>
#include <DevStudio/datatypes/GridValueType1StructEncoder.h>
#include <DevStudio/datatypes/GridValueType2Struct.h>
#include <DevStudio/datatypes/GridValueType2StructEncoder.h>
#include <DevStudio/datatypes/GroundFormationTypeEnum.h>
#include <DevStudio/datatypes/GroundFormationTypeEnumEncoder.h>
#include <DevStudio/datatypes/HLAargumentList.h>
#include <DevStudio/datatypes/HLAargumentListEncoder.h>
#include <DevStudio/datatypes/HLAfederateState.h>
#include <DevStudio/datatypes/HLAfederateStateEncoder.h>
#include <DevStudio/datatypes/HLAhandleList.h>
#include <DevStudio/datatypes/HLAhandleListEncoder.h>
#include <DevStudio/datatypes/HLAinteractionCount.h>
#include <DevStudio/datatypes/HLAinteractionCountEncoder.h>
#include <DevStudio/datatypes/HLAinteractionCounts.h>
#include <DevStudio/datatypes/HLAinteractionCountsEncoder.h>
#include <DevStudio/datatypes/HLAinteractionSubList.h>
#include <DevStudio/datatypes/HLAinteractionSubListEncoder.h>
#include <DevStudio/datatypes/HLAinteractionSubscription.h>
#include <DevStudio/datatypes/HLAinteractionSubscriptionEncoder.h>
#include <DevStudio/datatypes/HLAmoduleDesignatorList.h>
#include <DevStudio/datatypes/HLAmoduleDesignatorListEncoder.h>
#include <DevStudio/datatypes/HLAobjectClassBasedCount.h>
#include <DevStudio/datatypes/HLAobjectClassBasedCountEncoder.h>
#include <DevStudio/datatypes/HLAobjectClassBasedCounts.h>
#include <DevStudio/datatypes/HLAobjectClassBasedCountsEncoder.h>
#include <DevStudio/datatypes/HLAorderType.h>
#include <DevStudio/datatypes/HLAorderTypeEncoder.h>
#include <DevStudio/datatypes/HLAownership.h>
#include <DevStudio/datatypes/HLAownershipEncoder.h>
#include <DevStudio/datatypes/HLAresignAction.h>
#include <DevStudio/datatypes/HLAresignActionEncoder.h>
#include <DevStudio/datatypes/HLAswitch.h>
#include <DevStudio/datatypes/HLAswitchEncoder.h>
#include <DevStudio/datatypes/HLAsynchPointFederate.h>
#include <DevStudio/datatypes/HLAsynchPointFederateEncoder.h>
#include <DevStudio/datatypes/HLAsynchPointFederateList.h>
#include <DevStudio/datatypes/HLAsynchPointFederateListEncoder.h>
#include <DevStudio/datatypes/HLAsynchPointList.h>
#include <DevStudio/datatypes/HLAsynchPointListEncoder.h>
#include <DevStudio/datatypes/HLAsynchPointStatus.h>
#include <DevStudio/datatypes/HLAsynchPointStatusEncoder.h>
#include <DevStudio/datatypes/HLAtimeState.h>
#include <DevStudio/datatypes/HLAtimeStateEncoder.h>
#include <DevStudio/datatypes/HatchStateEnum.h>
#include <DevStudio/datatypes/HatchStateEnumEncoder.h>
#include <DevStudio/datatypes/HazardTypeEnum.h>
#include <DevStudio/datatypes/HazardTypeEnum2.h>
#include <DevStudio/datatypes/HazardTypeEnum2Encoder.h>
#include <DevStudio/datatypes/HazardTypeEnumEncoder.h>
#include <DevStudio/datatypes/HazeStruct.h>
#include <DevStudio/datatypes/HazeStructEncoder.h>
#include <DevStudio/datatypes/HazeTypeEnum.h>
#include <DevStudio/datatypes/HazeTypeEnumEncoder.h>
#include <DevStudio/datatypes/Holding.h>
#include <DevStudio/datatypes/HoldingEncoder.h>
#include <DevStudio/datatypes/HostilityStatusCodeEnum.h>
#include <DevStudio/datatypes/HostilityStatusCodeEnumEncoder.h>
#include <DevStudio/datatypes/IPETypeEnum.h>
#include <DevStudio/datatypes/IPETypeEnumEncoder.h>
#include <DevStudio/datatypes/IceStruct.h>
#include <DevStudio/datatypes/IceStructEncoder.h>
#include <DevStudio/datatypes/IceTypeEnum.h>
#include <DevStudio/datatypes/IceTypeEnumEncoder.h>
#include <DevStudio/datatypes/IdentificationLevelEnum.h>
#include <DevStudio/datatypes/IdentificationLevelEnumEncoder.h>
#include <DevStudio/datatypes/IffAlternateMode4Enum.h>
#include <DevStudio/datatypes/IffAlternateMode4EnumEncoder.h>
#include <DevStudio/datatypes/IffApplicableModesEnum.h>
#include <DevStudio/datatypes/IffApplicableModesEnumEncoder.h>
#include <DevStudio/datatypes/IffOperationalParameter1Enum.h>
#include <DevStudio/datatypes/IffOperationalParameter1EnumEncoder.h>
#include <DevStudio/datatypes/IffOperationalParameter2Enum.h>
#include <DevStudio/datatypes/IffOperationalParameter2EnumEncoder.h>
#include <DevStudio/datatypes/IffSystemModeEnum.h>
#include <DevStudio/datatypes/IffSystemModeEnumEncoder.h>
#include <DevStudio/datatypes/IffSystemNameEnum.h>
#include <DevStudio/datatypes/IffSystemNameEnumEncoder.h>
#include <DevStudio/datatypes/IffSystemTypeEnum.h>
#include <DevStudio/datatypes/IffSystemTypeEnumEncoder.h>
#include <DevStudio/datatypes/IncomingConnectionArray.h>
#include <DevStudio/datatypes/IncomingConnectionArrayEncoder.h>
#include <DevStudio/datatypes/IncomingConnectionStruct.h>
#include <DevStudio/datatypes/IncomingConnectionStructEncoder.h>
#include <DevStudio/datatypes/IndirectFireTaskStruct.h>
#include <DevStudio/datatypes/IndirectFireTaskStructEncoder.h>
#include <DevStudio/datatypes/InstallationSymbolAmplificationStruct.h>
#include <DevStudio/datatypes/InstallationSymbolAmplificationStructEncoder.h>
#include <DevStudio/datatypes/Integer16Array1Plus.h>
#include <DevStudio/datatypes/Integer16Array1PlusEncoder.h>
#include <DevStudio/datatypes/IrregularGridAxisStruct.h>
#include <DevStudio/datatypes/IrregularGridAxisStructEncoder.h>
#include <DevStudio/datatypes/IsPartOfStruct.h>
#include <DevStudio/datatypes/IsPartOfStructEncoder.h>
#include <DevStudio/datatypes/LandslideTypeEnum.h>
#include <DevStudio/datatypes/LandslideTypeEnumEncoder.h>
#include <DevStudio/datatypes/LayMinefieldTaskStruct.h>
#include <DevStudio/datatypes/LayMinefieldTaskStructEncoder.h>
#include <DevStudio/datatypes/LayerStruct.h>
#include <DevStudio/datatypes/LayerStructEncoder.h>
#include <DevStudio/datatypes/LightningTypeEnum.h>
#include <DevStudio/datatypes/LightningTypeEnumEncoder.h>
#include <DevStudio/datatypes/Line1GeomRecStruct.h>
#include <DevStudio/datatypes/Line1GeomRecStructEncoder.h>
#include <DevStudio/datatypes/Line2GeomRecStruct.h>
#include <DevStudio/datatypes/Line2GeomRecStructEncoder.h>
#include <DevStudio/datatypes/LinearSegmentStruct.h>
#include <DevStudio/datatypes/LinearSegmentStructEncoder.h>
#include <DevStudio/datatypes/LinkStatusArray.h>
#include <DevStudio/datatypes/LinkStatusArrayEncoder.h>
#include <DevStudio/datatypes/LinkStatusStruct.h>
#include <DevStudio/datatypes/LinkStatusStructEncoder.h>
#include <DevStudio/datatypes/LocationStruct.h>
#include <DevStudio/datatypes/LocationStructArray.h>
#include <DevStudio/datatypes/LocationStructArrayEncoder.h>
#include <DevStudio/datatypes/LocationStructEncoder.h>
#include <DevStudio/datatypes/MagicMoveTaskStruct.h>
#include <DevStudio/datatypes/MagicMoveTaskStructEncoder.h>
#include <DevStudio/datatypes/MajorRFModulationTypeEnum.h>
#include <DevStudio/datatypes/MajorRFModulationTypeEnumEncoder.h>
#include <DevStudio/datatypes/ManeuverIndicatorEnum.h>
#include <DevStudio/datatypes/ManeuverIndicatorEnumEncoder.h>
#include <DevStudio/datatypes/ManufacturerIdArray3.h>
#include <DevStudio/datatypes/ManufacturerIdArray3Encoder.h>
#include <DevStudio/datatypes/MarkingArray11.h>
#include <DevStudio/datatypes/MarkingArray11Encoder.h>
#include <DevStudio/datatypes/MarkingArray31.h>
#include <DevStudio/datatypes/MarkingArray31Encoder.h>
#include <DevStudio/datatypes/MarkingEncodingEnum.h>
#include <DevStudio/datatypes/MarkingEncodingEnumEncoder.h>
#include <DevStudio/datatypes/MarkingStruct.h>
#include <DevStudio/datatypes/MarkingStructEncoder.h>
#include <DevStudio/datatypes/MineDielectricDifferenceLengthlessArray.h>
#include <DevStudio/datatypes/MineDielectricDifferenceLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/MineFusingStruct.h>
#include <DevStudio/datatypes/MineFusingStructEncoder.h>
#include <DevStudio/datatypes/MineFusingStructLengthlessArray.h>
#include <DevStudio/datatypes/MineFusingStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/MineIdentifierLengthlessArray.h>
#include <DevStudio/datatypes/MineIdentifierLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/MinefieldFusingEnum.h>
#include <DevStudio/datatypes/MinefieldFusingEnumEncoder.h>
#include <DevStudio/datatypes/MinefieldLaneEnum.h>
#include <DevStudio/datatypes/MinefieldLaneEnumEncoder.h>
#include <DevStudio/datatypes/MinefieldLaneMarkerStruct.h>
#include <DevStudio/datatypes/MinefieldLaneMarkerStructEncoder.h>
#include <DevStudio/datatypes/MinefieldLaneMarkerStructLengthlessArray.h>
#include <DevStudio/datatypes/MinefieldLaneMarkerStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/MinefieldPaintSchemeEnum.h>
#include <DevStudio/datatypes/MinefieldPaintSchemeEnumEncoder.h>
#include <DevStudio/datatypes/MinefieldPaintSchemeLengthlessArray.h>
#include <DevStudio/datatypes/MinefieldPaintSchemeLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/MinefieldProtocolEnum.h>
#include <DevStudio/datatypes/MinefieldProtocolEnumEncoder.h>
#include <DevStudio/datatypes/MinefieldSensorTypeEnum.h>
#include <DevStudio/datatypes/MinefieldSensorTypeEnumEncoder.h>
#include <DevStudio/datatypes/MinefieldSensorTypeLengthlessArray.h>
#include <DevStudio/datatypes/MinefieldSensorTypeLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/MinefieldStatusEnum.h>
#include <DevStudio/datatypes/MinefieldStatusEnumEncoder.h>
#include <DevStudio/datatypes/MinefieldTypeEnum.h>
#include <DevStudio/datatypes/MinefieldTypeEnumEncoder.h>
#include <DevStudio/datatypes/MissingRecordNumbersLengthlessArray1Plus.h>
#include <DevStudio/datatypes/MissingRecordNumbersLengthlessArray1PlusEncoder.h>
#include <DevStudio/datatypes/MoveByRouteTaskStruct.h>
#include <DevStudio/datatypes/MoveByRouteTaskStructEncoder.h>
#include <DevStudio/datatypes/MoveInDirectionTaskStruct.h>
#include <DevStudio/datatypes/MoveInDirectionTaskStructEncoder.h>
#include <DevStudio/datatypes/MoveIntoFormationTaskStruct.h>
#include <DevStudio/datatypes/MoveIntoFormationTaskStructEncoder.h>
#include <DevStudio/datatypes/MoveTaskProgress.h>
#include <DevStudio/datatypes/MoveTaskProgressEncoder.h>
#include <DevStudio/datatypes/MoveToLocationTaskStruct.h>
#include <DevStudio/datatypes/MoveToLocationTaskStructEncoder.h>
#include <DevStudio/datatypes/MoveTypeEnum.h>
#include <DevStudio/datatypes/MoveTypeEnumEncoder.h>
#include <DevStudio/datatypes/MsgIdEnum.h>
#include <DevStudio/datatypes/MsgIdEnumEncoder.h>
#include <DevStudio/datatypes/NamedLocationStruct.h>
#include <DevStudio/datatypes/NamedLocationStructEncoder.h>
#include <DevStudio/datatypes/NavigationLightFlashingEnum.h>
#include <DevStudio/datatypes/NavigationLightFlashingEnumEncoder.h>
#include <DevStudio/datatypes/NavigationStatusEnum.h>
#include <DevStudio/datatypes/NavigationStatusEnumEncoder.h>
#include <DevStudio/datatypes/NetworkDeviceArray.h>
#include <DevStudio/datatypes/NetworkDeviceArrayEncoder.h>
#include <DevStudio/datatypes/NetworkDeviceEmptyCharactersticsStruct.h>
#include <DevStudio/datatypes/NetworkDeviceEmptyCharactersticsStructEncoder.h>
#include <DevStudio/datatypes/NetworkDeviceGenericTransmitterCharacteristicsStruct.h>
#include <DevStudio/datatypes/NetworkDeviceGenericTransmitterCharacteristicsStructEncoder.h>
#include <DevStudio/datatypes/NetworkDeviceReceiverCharacteristicsVariant.h>
#include <DevStudio/datatypes/NetworkDeviceReceiverCharacteristicsVariantEncoder.h>
#include <DevStudio/datatypes/NetworkDeviceStruct.h>
#include <DevStudio/datatypes/NetworkDeviceStructEncoder.h>
#include <DevStudio/datatypes/NetworkDeviceTransmitterCharacteristicsVariant.h>
#include <DevStudio/datatypes/NetworkDeviceTransmitterCharacteristicsVariantEncoder.h>
#include <DevStudio/datatypes/NomenclatureEnum.h>
#include <DevStudio/datatypes/NomenclatureEnumEncoder.h>
#include <DevStudio/datatypes/NomenclatureVersionEnum.h>
#include <DevStudio/datatypes/NomenclatureVersionEnumEncoder.h>
#include <DevStudio/datatypes/ObserveTaskStruct.h>
#include <DevStudio/datatypes/ObserveTaskStructEncoder.h>
#include <DevStudio/datatypes/ObservedEquipment.h>
#include <DevStudio/datatypes/ObservedEquipmentEncoder.h>
#include <DevStudio/datatypes/OctetArray.h>
#include <DevStudio/datatypes/OctetArray1Plus.h>
#include <DevStudio/datatypes/OctetArray1PlusEncoder.h>
#include <DevStudio/datatypes/OctetArray2.h>
#include <DevStudio/datatypes/OctetArray2Encoder.h>
#include <DevStudio/datatypes/OctetArray3.h>
#include <DevStudio/datatypes/OctetArray3Encoder.h>
#include <DevStudio/datatypes/OctetArray4.h>
#include <DevStudio/datatypes/OctetArray4Encoder.h>
#include <DevStudio/datatypes/OctetArray7.h>
#include <DevStudio/datatypes/OctetArray7Encoder.h>
#include <DevStudio/datatypes/OctetArray8.h>
#include <DevStudio/datatypes/OctetArray8Encoder.h>
#include <DevStudio/datatypes/OctetArrayEncoder.h>
#include <DevStudio/datatypes/OctetPadding32Array.h>
#include <DevStudio/datatypes/OctetPadding32ArrayEncoder.h>
#include <DevStudio/datatypes/OctetPadding64Array.h>
#include <DevStudio/datatypes/OctetPadding64ArrayEncoder.h>
#include <DevStudio/datatypes/OpacityCodeEnum.h>
#include <DevStudio/datatypes/OpacityCodeEnumEncoder.h>
#include <DevStudio/datatypes/OperateCheckpointTaskStruct.h>
#include <DevStudio/datatypes/OperateCheckpointTaskStructEncoder.h>
#include <DevStudio/datatypes/OperateObservationPostTaskStruct.h>
#include <DevStudio/datatypes/OperateObservationPostTaskStructEncoder.h>
#include <DevStudio/datatypes/OrientationStruct.h>
#include <DevStudio/datatypes/OrientationStructEncoder.h>
#include <DevStudio/datatypes/OrientationStructLengthlessArray.h>
#include <DevStudio/datatypes/OrientationStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/OtherActivityTaskStruct.h>
#include <DevStudio/datatypes/OtherActivityTaskStructEncoder.h>
#include <DevStudio/datatypes/ParameterTypeEnum.h>
#include <DevStudio/datatypes/ParameterTypeEnumEncoder.h>
#include <DevStudio/datatypes/ParameterValueVariantStruct.h>
#include <DevStudio/datatypes/ParameterValueVariantStructEncoder.h>
#include <DevStudio/datatypes/PartNumberEnum.h>
#include <DevStudio/datatypes/PartNumberEnumEncoder.h>
#include <DevStudio/datatypes/PatrolMoveTypeEnum.h>
#include <DevStudio/datatypes/PatrolMoveTypeEnumEncoder.h>
#include <DevStudio/datatypes/PatrolTaskProgress.h>
#include <DevStudio/datatypes/PatrolTaskProgressEncoder.h>
#include <DevStudio/datatypes/PatrolTaskStruct.h>
#include <DevStudio/datatypes/PatrolTaskStructEncoder.h>
#include <DevStudio/datatypes/PatrolTypeEnum.h>
#include <DevStudio/datatypes/PatrolTypeEnumEncoder.h>
#include <DevStudio/datatypes/PerimeterPointStruct.h>
#include <DevStudio/datatypes/PerimeterPointStructEncoder.h>
#include <DevStudio/datatypes/PerimeterPointStructLengthlessArray.h>
#include <DevStudio/datatypes/PerimeterPointStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/PhysicalGenericNetworkStruct.h>
#include <DevStudio/datatypes/PhysicalGenericNetworkStructEncoder.h>
#include <DevStudio/datatypes/PhysicalNetworkDescriptionVariant.h>
#include <DevStudio/datatypes/PhysicalNetworkDescriptionVariantEncoder.h>
#include <DevStudio/datatypes/PhysicalNetworkTypeEnum.h>
#include <DevStudio/datatypes/PhysicalNetworkTypeEnumEncoder.h>
#include <DevStudio/datatypes/PhysicalUndefinedNetworkStruct.h>
#include <DevStudio/datatypes/PhysicalUndefinedNetworkStructEncoder.h>
#include <DevStudio/datatypes/PlumeDimensionRateStruct.h>
#include <DevStudio/datatypes/PlumeDimensionRateStructEncoder.h>
#include <DevStudio/datatypes/PlumeDimensionStruct.h>
#include <DevStudio/datatypes/PlumeDimensionStructEncoder.h>
#include <DevStudio/datatypes/Point2GeomRecStruct.h>
#include <DevStudio/datatypes/Point2GeomRecStructEncoder.h>
#include <DevStudio/datatypes/PrecipitationStruct.h>
#include <DevStudio/datatypes/PrecipitationStructEncoder.h>
#include <DevStudio/datatypes/PrecipitationTypeEnum.h>
#include <DevStudio/datatypes/PrecipitationTypeEnumEncoder.h>
#include <DevStudio/datatypes/PropulsionPlantEnum.h>
#include <DevStudio/datatypes/PropulsionPlantEnumEncoder.h>
#include <DevStudio/datatypes/PropulsionSystemDataStruct.h>
#include <DevStudio/datatypes/PropulsionSystemDataStructEncoder.h>
#include <DevStudio/datatypes/PropulsionSystemDataStructLengthlessArray.h>
#include <DevStudio/datatypes/PropulsionSystemDataStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/ProtectionEffectivenessStruct.h>
#include <DevStudio/datatypes/ProtectionEffectivenessStructEncoder.h>
#include <DevStudio/datatypes/PulseModulationTypeEnum.h>
#include <DevStudio/datatypes/PulseModulationTypeEnumEncoder.h>
#include <DevStudio/datatypes/PumpFloodingTaskStruct.h>
#include <DevStudio/datatypes/PumpFloodingTaskStructEncoder.h>
#include <DevStudio/datatypes/RFModulationSystemTypeEnum.h>
#include <DevStudio/datatypes/RFModulationSystemTypeEnumEncoder.h>
#include <DevStudio/datatypes/RFModulationTypeVariantStruct.h>
#include <DevStudio/datatypes/RFModulationTypeVariantStructEncoder.h>
#include <DevStudio/datatypes/RTIobjectIdArray.h>
#include <DevStudio/datatypes/RTIobjectIdArrayEncoder.h>
#include <DevStudio/datatypes/RadioInputSourceEnum.h>
#include <DevStudio/datatypes/RadioInputSourceEnumEncoder.h>
#include <DevStudio/datatypes/RadioTypeStruct.h>
#include <DevStudio/datatypes/RadioTypeStructEncoder.h>
#include <DevStudio/datatypes/ReceiverOperationalStatusEnum.h>
#include <DevStudio/datatypes/ReceiverOperationalStatusEnumEncoder.h>
#include <DevStudio/datatypes/RecordSetStruct.h>
#include <DevStudio/datatypes/RecordSetStructArray1Plus.h>
#include <DevStudio/datatypes/RecordSetStructArray1PlusEncoder.h>
#include <DevStudio/datatypes/RecordSetStructEncoder.h>
#include <DevStudio/datatypes/RecordStruct.h>
#include <DevStudio/datatypes/RecordStructArray.h>
#include <DevStudio/datatypes/RecordStructArrayEncoder.h>
#include <DevStudio/datatypes/RecordStructEncoder.h>
#include <DevStudio/datatypes/RectVol1GeomRecStruct.h>
#include <DevStudio/datatypes/RectVol1GeomRecStructEncoder.h>
#include <DevStudio/datatypes/RectVol2GeomRecStruct.h>
#include <DevStudio/datatypes/RectVol2GeomRecStructEncoder.h>
#include <DevStudio/datatypes/RectVol3GeomRecStruct.h>
#include <DevStudio/datatypes/RectVol3GeomRecStructEncoder.h>
#include <DevStudio/datatypes/ReferenceSystemEnum.h>
#include <DevStudio/datatypes/ReferenceSystemEnumEncoder.h>
#include <DevStudio/datatypes/RefuelingStationLightColorEnum.h>
#include <DevStudio/datatypes/RefuelingStationLightColorEnumEncoder.h>
#include <DevStudio/datatypes/RefuelingStationLightDayNightEnum.h>
#include <DevStudio/datatypes/RefuelingStationLightDayNightEnumEncoder.h>
#include <DevStudio/datatypes/Relation.h>
#include <DevStudio/datatypes/RelationEncoder.h>
#include <DevStudio/datatypes/RelativePositionStruct.h>
#include <DevStudio/datatypes/RelativePositionStructEncoder.h>
#include <DevStudio/datatypes/RelativeRangeBearingStruct.h>
#include <DevStudio/datatypes/RelativeRangeBearingStructEncoder.h>
#include <DevStudio/datatypes/ReleaseDistributionStruct.h>
#include <DevStudio/datatypes/ReleaseDistributionStructEncoder.h>
#include <DevStudio/datatypes/ReleaseDynamicsStruct.h>
#include <DevStudio/datatypes/ReleaseDynamicsStructEncoder.h>
#include <DevStudio/datatypes/ReleaseSizeStruct.h>
#include <DevStudio/datatypes/ReleaseSizeStructEncoder.h>
#include <DevStudio/datatypes/RepairResultEnum.h>
#include <DevStudio/datatypes/RepairResultEnumEncoder.h>
#include <DevStudio/datatypes/RepairTaskStruct.h>
#include <DevStudio/datatypes/RepairTaskStructEncoder.h>
#include <DevStudio/datatypes/RepairTypeEnum.h>
#include <DevStudio/datatypes/RepairTypeEnumEncoder.h>
#include <DevStudio/datatypes/RequestStatusEnum.h>
#include <DevStudio/datatypes/RequestStatusEnumEncoder.h>
#include <DevStudio/datatypes/RequestedConnection.h>
#include <DevStudio/datatypes/RequestedConnectionArray.h>
#include <DevStudio/datatypes/RequestedConnectionArrayEncoder.h>
#include <DevStudio/datatypes/RequestedConnectionEncoder.h>
#include <DevStudio/datatypes/ResourceStatusStruct.h>
#include <DevStudio/datatypes/ResourceStatusStructEncoder.h>
#include <DevStudio/datatypes/ResponseFlagEnum.h>
#include <DevStudio/datatypes/ResponseFlagEnumEncoder.h>
#include <DevStudio/datatypes/ResupplyTaskStruct.h>
#include <DevStudio/datatypes/ResupplyTaskStructEncoder.h>
#include <DevStudio/datatypes/RoadIceConditionEnum.h>
#include <DevStudio/datatypes/RoadIceConditionEnumEncoder.h>
#include <DevStudio/datatypes/RoundStruct.h>
#include <DevStudio/datatypes/RoundStructEncoder.h>
#include <DevStudio/datatypes/RulesOfEngagementEnum.h>
#include <DevStudio/datatypes/RulesOfEngagementEnumEncoder.h>
#include <DevStudio/datatypes/SINCGARSModulationStruct.h>
#include <DevStudio/datatypes/SINCGARSModulationStructEncoder.h>
#include <DevStudio/datatypes/SeaStateEnum.h>
#include <DevStudio/datatypes/SeaStateEnumEncoder.h>
#include <DevStudio/datatypes/SedimentTypeEnum.h>
#include <DevStudio/datatypes/SedimentTypeEnumEncoder.h>
#include <DevStudio/datatypes/SendSafetyRelatedBroadcastMessageTaskStruct.h>
#include <DevStudio/datatypes/SendSafetyRelatedBroadcastMessageTaskStructEncoder.h>
#include <DevStudio/datatypes/SendSafetyRelatedMessageTaskStruct.h>
#include <DevStudio/datatypes/SendSafetyRelatedMessageTaskStructEncoder.h>
#include <DevStudio/datatypes/SensorStateEnum.h>
#include <DevStudio/datatypes/SensorStateEnumEncoder.h>
#include <DevStudio/datatypes/SensorStruct.h>
#include <DevStudio/datatypes/SensorStructEncoder.h>
#include <DevStudio/datatypes/ServiceTypeEnum.h>
#include <DevStudio/datatypes/ServiceTypeEnumEncoder.h>
#include <DevStudio/datatypes/SetTransmitterStatusTaskStruct.h>
#include <DevStudio/datatypes/SetTransmitterStatusTaskStructEncoder.h>
#include <DevStudio/datatypes/ShaftDataStruct.h>
#include <DevStudio/datatypes/ShaftDataStructEncoder.h>
#include <DevStudio/datatypes/ShaftDataStructLengthlessArray1Plus.h>
#include <DevStudio/datatypes/ShaftDataStructLengthlessArray1PlusEncoder.h>
#include <DevStudio/datatypes/ShipTypeEnum.h>
#include <DevStudio/datatypes/ShipTypeEnumEncoder.h>
#include <DevStudio/datatypes/SignalDataLengthlessArray1Plus.h>
#include <DevStudio/datatypes/SignalDataLengthlessArray1PlusEncoder.h>
#include <DevStudio/datatypes/SignatureTypeEnum.h>
#include <DevStudio/datatypes/SignatureTypeEnumEncoder.h>
#include <DevStudio/datatypes/SignatureVariant.h>
#include <DevStudio/datatypes/SignatureVariantEncoder.h>
#include <DevStudio/datatypes/SilentAggregateStruct.h>
#include <DevStudio/datatypes/SilentAggregateStructEncoder.h>
#include <DevStudio/datatypes/SilentAggregateStructLengthlessArray.h>
#include <DevStudio/datatypes/SilentAggregateStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/SilentEntityStruct.h>
#include <DevStudio/datatypes/SilentEntityStructEncoder.h>
#include <DevStudio/datatypes/SilentEntityStructLengthlessArray.h>
#include <DevStudio/datatypes/SilentEntityStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/SnowStruct.h>
#include <DevStudio/datatypes/SnowStructEncoder.h>
#include <DevStudio/datatypes/SpatialFPStruct.h>
#include <DevStudio/datatypes/SpatialFPStructEncoder.h>
#include <DevStudio/datatypes/SpatialFVStruct.h>
#include <DevStudio/datatypes/SpatialFVStructEncoder.h>
#include <DevStudio/datatypes/SpatialRPStruct.h>
#include <DevStudio/datatypes/SpatialRPStructEncoder.h>
#include <DevStudio/datatypes/SpatialRVStruct.h>
#include <DevStudio/datatypes/SpatialRVStructEncoder.h>
#include <DevStudio/datatypes/SpatialStaticStruct.h>
#include <DevStudio/datatypes/SpatialStaticStructEncoder.h>
#include <DevStudio/datatypes/SpatialVariantStruct.h>
#include <DevStudio/datatypes/SpatialVariantStructEncoder.h>
#include <DevStudio/datatypes/Sphere1GeomRecStruct.h>
#include <DevStudio/datatypes/Sphere1GeomRecStructEncoder.h>
#include <DevStudio/datatypes/Sphere2GeomRecStruct.h>
#include <DevStudio/datatypes/Sphere2GeomRecStructEncoder.h>
#include <DevStudio/datatypes/SphericalHarmonicAntennaStruct.h>
#include <DevStudio/datatypes/SphericalHarmonicAntennaStructEncoder.h>
#include <DevStudio/datatypes/SpreadSpectrumEnum.h>
#include <DevStudio/datatypes/SpreadSpectrumEnumEncoder.h>
#include <DevStudio/datatypes/SpreadSpectrumVariantStruct.h>
#include <DevStudio/datatypes/SpreadSpectrumVariantStructEncoder.h>
#include <DevStudio/datatypes/StanceCodeEnum.h>
#include <DevStudio/datatypes/StanceCodeEnumEncoder.h>
#include <DevStudio/datatypes/StationEnum.h>
#include <DevStudio/datatypes/StationEnumEncoder.h>
#include <DevStudio/datatypes/StationNameLocationVariantStruct.h>
#include <DevStudio/datatypes/StationNameLocationVariantStructEncoder.h>
#include <DevStudio/datatypes/StopFreezeReasonEnum.h>
#include <DevStudio/datatypes/StopFreezeReasonEnumEncoder.h>
#include <DevStudio/datatypes/SubsurfaceFormationTypeEnum.h>
#include <DevStudio/datatypes/SubsurfaceFormationTypeEnumEncoder.h>
#include <DevStudio/datatypes/SupplyStatusStruct.h>
#include <DevStudio/datatypes/SupplyStatusStructEncoder.h>
#include <DevStudio/datatypes/SupplyStruct.h>
#include <DevStudio/datatypes/SupplyStructEncoder.h>
#include <DevStudio/datatypes/SupplyStructLengthlessArray.h>
#include <DevStudio/datatypes/SupplyStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/SurfaceFormationTypeEnum.h>
#include <DevStudio/datatypes/SurfaceFormationTypeEnumEncoder.h>
#include <DevStudio/datatypes/SurfaceMoistureEnum.h>
#include <DevStudio/datatypes/SurfaceMoistureEnumEncoder.h>
#include <DevStudio/datatypes/SymbolAmplificationVariant.h>
#include <DevStudio/datatypes/SymbolAmplificationVariantEncoder.h>
#include <DevStudio/datatypes/SymbolStruct.h>
#include <DevStudio/datatypes/SymbolStructEncoder.h>
#include <DevStudio/datatypes/SymbolTypeEnum.h>
#include <DevStudio/datatypes/SymbolTypeEnumEncoder.h>
#include <DevStudio/datatypes/TacticalDataLinkTypeEnum.h>
#include <DevStudio/datatypes/TacticalDataLinkTypeEnumEncoder.h>
#include <DevStudio/datatypes/TaskDefinition.h>
#include <DevStudio/datatypes/TaskDefinitionEncoder.h>
#include <DevStudio/datatypes/TaskDefinitionVariantRecord.h>
#include <DevStudio/datatypes/TaskDefinitionVariantRecordEncoder.h>
#include <DevStudio/datatypes/TaskModeEnum.h>
#include <DevStudio/datatypes/TaskModeEnumEncoder.h>
#include <DevStudio/datatypes/TaskProgress.h>
#include <DevStudio/datatypes/TaskProgressEncoder.h>
#include <DevStudio/datatypes/TaskProgressVariantRecord.h>
#include <DevStudio/datatypes/TaskProgressVariantRecordEncoder.h>
#include <DevStudio/datatypes/TaskStatusEnum.h>
#include <DevStudio/datatypes/TaskStatusEnumEncoder.h>
#include <DevStudio/datatypes/TemperatureDegreeCelsiusFloat32LengthlessArray.h>
#include <DevStudio/datatypes/TemperatureDegreeCelsiusFloat32LengthlessArrayEncoder.h>
#include <DevStudio/datatypes/TrackStruct.h>
#include <DevStudio/datatypes/TrackStructEncoder.h>
#include <DevStudio/datatypes/TrailingEffectsCodeEnum.h>
#include <DevStudio/datatypes/TrailingEffectsCodeEnumEncoder.h>
#include <DevStudio/datatypes/TransferTypeEnum.h>
#include <DevStudio/datatypes/TransferTypeEnumEncoder.h>
#include <DevStudio/datatypes/TransmitterOperationalStatusEnum.h>
#include <DevStudio/datatypes/TransmitterOperationalStatusEnumEncoder.h>
#include <DevStudio/datatypes/TransportTaskStruct.h>
#include <DevStudio/datatypes/TransportTaskStructEncoder.h>
#include <DevStudio/datatypes/TreatmentStruct.h>
#include <DevStudio/datatypes/TreatmentStructEncoder.h>
#include <DevStudio/datatypes/UUID.h>
#include <DevStudio/datatypes/UUIDEncoder.h>
#include <DevStudio/datatypes/UniformGeomRecStruct.h>
#include <DevStudio/datatypes/UniformGeomRecStructEncoder.h>
#include <DevStudio/datatypes/UnitSymbolAmplificationStruct.h>
#include <DevStudio/datatypes/UnitSymbolAmplificationStructEncoder.h>
#include <DevStudio/datatypes/UnmodulatedTypeEnum.h>
#include <DevStudio/datatypes/UnmodulatedTypeEnumEncoder.h>
#include <DevStudio/datatypes/UnsignedInteger16Array1Plus.h>
#include <DevStudio/datatypes/UnsignedInteger16Array1PlusEncoder.h>
#include <DevStudio/datatypes/UnsignedInteger32LengthlessArray.h>
#include <DevStudio/datatypes/UnsignedInteger32LengthlessArrayEncoder.h>
#include <DevStudio/datatypes/UnsignedInteger64Array1Plus.h>
#include <DevStudio/datatypes/UnsignedInteger64Array1PlusEncoder.h>
#include <DevStudio/datatypes/UnsignedInteger8LengthlessArray.h>
#include <DevStudio/datatypes/UnsignedInteger8LengthlessArrayEncoder.h>
#include <DevStudio/datatypes/UserProtocolEnum.h>
#include <DevStudio/datatypes/UserProtocolEnumEncoder.h>
#include <DevStudio/datatypes/VariableDatumStruct.h>
#include <DevStudio/datatypes/VariableDatumStructArray.h>
#include <DevStudio/datatypes/VariableDatumStructArrayEncoder.h>
#include <DevStudio/datatypes/VariableDatumStructEncoder.h>
#include <DevStudio/datatypes/VariableDatumStructLengthlessArray.h>
#include <DevStudio/datatypes/VariableDatumStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/VectoringNozzleSystemDataStruct.h>
#include <DevStudio/datatypes/VectoringNozzleSystemDataStructEncoder.h>
#include <DevStudio/datatypes/VectoringNozzleSystemDataStructLengthlessArray.h>
#include <DevStudio/datatypes/VectoringNozzleSystemDataStructLengthlessArrayEncoder.h>
#include <DevStudio/datatypes/VelocityVectorStruct.h>
#include <DevStudio/datatypes/VelocityVectorStructEncoder.h>
#include <DevStudio/datatypes/VisibleSideLocationEnum.h>
#include <DevStudio/datatypes/VisibleSideLocationEnumEncoder.h>
#include <DevStudio/datatypes/WarheadTypeEnum.h>
#include <DevStudio/datatypes/WarheadTypeEnumEncoder.h>
#include <DevStudio/datatypes/WaveStruct.h>
#include <DevStudio/datatypes/WaveStructEncoder.h>
#include <DevStudio/datatypes/Waypoint.h>
#include <DevStudio/datatypes/WaypointEncoder.h>
#include <DevStudio/datatypes/WeaponControlOrderEnum.h>
#include <DevStudio/datatypes/WeaponControlOrderEnumEncoder.h>
#include <DevStudio/datatypes/WeaponStateEnum.h>
#include <DevStudio/datatypes/WeaponStateEnumEncoder.h>
#include <DevStudio/datatypes/WindStruct.h>
#include <DevStudio/datatypes/WindStructEncoder.h>
#include <DevStudio/datatypes/WorldLocationStruct.h>
#include <DevStudio/datatypes/WorldLocationStructEncoder.h>
#include <DevStudio/datatypes/WorldLocationStructLengthlessArray.h>
#include <DevStudio/datatypes/WorldLocationStructLengthlessArrayEncoder.h>
// === AUTO-GENERATED INCLUDES END ===
//pragma endregion

namespace DevStudio
{
    class Decoders
    {
    private:
        mutable std::mutex _decodersLock;

//pragma region Auto-generated Variables
// === AUTO-GENERATED MEMBERS BEGIN ===
        ATP45HazardAreaTypeEnumEncoder _aTP45HazardAreaTypeEnumDecoder;
        AccelerationVectorStructEncoder _accelerationVectorStructDecoder;
        AcknowledgeFlagEnumEncoder _acknowledgeFlagEnumDecoder;
        AcknowledgementProtocolEnumEncoder _acknowledgementProtocolEnumDecoder;
        ActionEnumEncoder _actionEnumDecoder;
        ActionResultEnumEncoder _actionResultEnumDecoder;
        ActiveSonarEnumEncoder _activeSonarEnumDecoder;
        ActiveSonarFunctionCodeEnumEncoder _activeSonarFunctionCodeEnumDecoder;
        ActiveSonarScanPatternEnumEncoder _activeSonarScanPatternEnumDecoder;
        ActiveStatusEnumEncoder _activeStatusEnumDecoder;
        AdministerTreatmentTaskStructEncoder _administerTreatmentTaskStructDecoder;
        AgentClassEnumEncoder _agentClassEnumDecoder;
        AgentConcentrationStructEncoder _agentConcentrationStructDecoder;
        AgentMassStructEncoder _agentMassStructDecoder;
        AgentTypeEnumEncoder _agentTypeEnumDecoder;
        AggregateMarkingStructEncoder _aggregateMarkingStructDecoder;
        AggregateMissionEnumEncoder _aggregateMissionEnumDecoder;
        AggregateStateEnumEncoder _aggregateStateEnumDecoder;
        AggregateStateFormationEnumEncoder _aggregateStateFormationEnumDecoder;
        AidTypeEnumEncoder _aidTypeEnumDecoder;
        AirFormationTypeEnumEncoder _airFormationTypeEnumDecoder;
        AmplitudeAngleModulationTypeEnumEncoder _amplitudeAngleModulationTypeEnumDecoder;
        AmplitudeModulationTypeEnumEncoder _amplitudeModulationTypeEnumDecoder;
        AngleModulationTypeEnumEncoder _angleModulationTypeEnumDecoder;
        AngularVelocityVectorStructEncoder _angularVelocityVectorStructDecoder;
        AntennaPatternTypeEnumEncoder _antennaPatternTypeEnumDecoder;
        AntennaPatternVariantStructEncoder _antennaPatternVariantStructDecoder;
        AntennaPatternVariantStructLengthlessArrayEncoder _antennaPatternVariantStructLengthlessArrayDecoder;
        AntiCollisionLightFlashingEnumEncoder _antiCollisionLightFlashingEnumDecoder;
        AppearanceAntiCollisionDayNightEnumEncoder _appearanceAntiCollisionDayNightEnumDecoder;
        AppearanceNVGModeEnumEncoder _appearanceNVGModeEnumDecoder;
        ApplyIPETaskStructEncoder _applyIPETaskStructDecoder;
        AppointmentStructEncoder _appointmentStructDecoder;
        ArrayOfAgentConcentrationStructEncoder _arrayOfAgentConcentrationStructDecoder;
        ArrayOfAgentMassStructEncoder _arrayOfAgentMassStructDecoder;
        ArrayOfAgentTypeEnumEncoder _arrayOfAgentTypeEnumDecoder;
        ArrayOfCBRNExposureStructEncoder _arrayOfCBRNExposureStructDecoder;
        ArrayOfCBRNSensorReadingStructEncoder _arrayOfCBRNSensorReadingStructDecoder;
        ArrayOfCommunicationNetworksEncoder _arrayOfCommunicationNetworksDecoder;
        ArrayOfContourStructEncoder _arrayOfContourStructDecoder;
        ArrayOfDetectedEquipmentEncoder _arrayOfDetectedEquipmentDecoder;
        ArrayOfHoldingsEncoder _arrayOfHoldingsDecoder;
        ArrayOfObservedEquipmentEncoder _arrayOfObservedEquipmentDecoder;
        ArrayOfProtectionEffectivenessStructEncoder _arrayOfProtectionEffectivenessStructDecoder;
        ArrayOfRelationshipEncoder _arrayOfRelationshipDecoder;
        ArrayOfResourceStatusEncoder _arrayOfResourceStatusDecoder;
        ArrayOfSensorEncoder _arrayOfSensorDecoder;
        ArrayOfSigmas6Encoder _arrayOfSigmas6Decoder;
        ArrayOfSignatureEncoder _arrayOfSignatureDecoder;
        ArrayOfSupplyStatusEncoder _arrayOfSupplyStatusDecoder;
        ArrayOfTaskDefinitionsEncoder _arrayOfTaskDefinitionsDecoder;
        ArrayOfTaskProgressEncoder _arrayOfTaskProgressDecoder;
        ArrayOfTrackEncoder _arrayOfTrackDecoder;
        ArrayOfTreatmentStructEncoder _arrayOfTreatmentStructDecoder;
        ArrayOfUuidEncoder _arrayOfUuidDecoder;
        ArrayOfWaypointsEncoder _arrayOfWaypointsDecoder;
        ArticulatedParameterStructEncoder _articulatedParameterStructDecoder;
        ArticulatedParameterStructLengthlessArrayEncoder _articulatedParameterStructLengthlessArrayDecoder;
        ArticulatedPartsStructEncoder _articulatedPartsStructDecoder;
        ArticulatedPartsTypeEnumEncoder _articulatedPartsTypeEnumDecoder;
        ArticulatedTypeMetricEnumEncoder _articulatedTypeMetricEnumDecoder;
        AttachTaskStructEncoder _attachTaskStructDecoder;
        AttachedPartsStructEncoder _attachedPartsStructDecoder;
        AttackTypeEnumEncoder _attackTypeEnumDecoder;
        AttributeValuePairStructArray1PlusEncoder _attributeValuePairStructArray1PlusDecoder;
        AttributeValuePairStructEncoder _attributeValuePairStructDecoder;
        AudioDataTypeStructEncoder _audioDataTypeStructDecoder;
        BaseEncoder _baseDecoder;
        BeamAntennaStructEncoder _beamAntennaStructDecoder;
        BeamFunctionCodeEnumEncoder _beamFunctionCodeEnumDecoder;
        BinArrayEncoder _binArrayDecoder;
        BreachStructEncoder _breachStructDecoder;
        BreachStructLengthlessArrayEncoder _breachStructLengthlessArrayDecoder;
        BreachableSegmentStructEncoder _breachableSegmentStructDecoder;
        BreachableSegmentStructLengthlessArrayEncoder _breachableSegmentStructLengthlessArrayDecoder;
        BreachedStatusArray8Encoder _breachedStatusArray8Decoder;
        BreachedStatusEnumEncoder _breachedStatusEnumDecoder;
        CBRNAlarmStructEncoder _cBRNAlarmStructDecoder;
        CBRNDamageEnumEncoder _cBRNDamageEnumDecoder;
        CBRNExposureStructEncoder _cBRNExposureStructDecoder;
        CBRNSensorReadingStructEncoder _cBRNSensorReadingStructDecoder;
        COMBICStateRecStructEncoder _cOMBICStateRecStructDecoder;
        CamouflageEnumEncoder _camouflageEnumDecoder;
        CaptureStatusEnumEncoder _captureStatusEnumDecoder;
        ChangeAltitudeTaskStructEncoder _changeAltitudeTaskStructDecoder;
        ChangeHeadingTaskStructEncoder _changeHeadingTaskStructDecoder;
        ChangeRulesOfEngagementTaskStructEncoder _changeRulesOfEngagementTaskStructDecoder;
        ChangeSpeedTaskStructEncoder _changeSpeedTaskStructDecoder;
        ChemicalContentEnumEncoder _chemicalContentEnumDecoder;
        ClearEngineeringTaskStructEncoder _clearEngineeringTaskStructDecoder;
        ClockTimeStructEncoder _clockTimeStructDecoder;
        ClockTimeStructLengthlessArrayEncoder _clockTimeStructLengthlessArrayDecoder;
        CloudStructEncoder _cloudStructDecoder;
        CloudTypeEnumEncoder _cloudTypeEnumDecoder;
        CoefficientsLengthlessArray1PlusEncoder _coefficientsLengthlessArray1PlusDecoder;
        CollisionTypeEnumEncoder _collisionTypeEnumDecoder;
        CombinationModulationTypeEnumEncoder _combinationModulationTypeEnumDecoder;
        CommunicationNetworkArrayEncoder _communicationNetworkArrayDecoder;
        CommunicationNetworkTypeEnumEncoder _communicationNetworkTypeEnumDecoder;
        CommunicationServiceTypeEnumEncoder _communicationServiceTypeEnumDecoder;
        ComplianceStateEnumEncoder _complianceStateEnumDecoder;
        Cone1GeomRecStructEncoder _cone1GeomRecStructDecoder;
        Cone2GeomRecStructEncoder _cone2GeomRecStructDecoder;
        ConnectionReceiverArrayEncoder _connectionReceiverArrayDecoder;
        ConnectionReceiverStructEncoder _connectionReceiverStructDecoder;
        ConnectionTypeEnumEncoder _connectionTypeEnumDecoder;
        ConstituentPartNatureEnumEncoder _constituentPartNatureEnumDecoder;
        ConstituentPartPositionEnumEncoder _constituentPartPositionEnumDecoder;
        ConstituentPartRelationshipStructEncoder _constituentPartRelationshipStructDecoder;
        ConstituentPartStationNameEnumEncoder _constituentPartStationNameEnumDecoder;
        ContourStructEncoder _contourStructDecoder;
        CreateBreachTaskStructEncoder _createBreachTaskStructDecoder;
        CreateFireBreakTaskStructEncoder _createFireBreakTaskStructDecoder;
        CreateLeveeTaskStructEncoder _createLeveeTaskStructDecoder;
        CreateObstacleTaskStructEncoder _createObstacleTaskStructDecoder;
        CryptographicModeEnumEncoder _cryptographicModeEnumDecoder;
        CryptographicSystemTypeEnumEncoder _cryptographicSystemTypeEnumDecoder;
        CurrentStructEncoder _currentStructDecoder;
        DamageStatusEnhancedEnumEncoder _damageStatusEnhancedEnumDecoder;
        DamageStatusEnumEncoder _damageStatusEnumDecoder;
        DatumIdentifierEnumEncoder _datumIdentifierEnumDecoder;
        DatumIdentifierLengthlessArrayEncoder _datumIdentifierLengthlessArrayDecoder;
        DeadReckoningAlgorithmEnumEncoder _deadReckoningAlgorithmEnumDecoder;
        DepthMeterFloat32LengthlessArrayEncoder _depthMeterFloat32LengthlessArrayDecoder;
        DesignatorCodeEnumEncoder _designatorCodeEnumDecoder;
        DesignatorCodeNameEnumEncoder _designatorCodeNameEnumDecoder;
        DetectedEquipmentEncoder _detectedEquipmentDecoder;
        DetonationResultCodeEnumEncoder _detonationResultCodeEnumDecoder;
        DimensionRateStructEncoder _dimensionRateStructDecoder;
        DimensionStructEncoder _dimensionStructDecoder;
        DirectFireTaskStructEncoder _directFireTaskStructDecoder;
        DisruptCommunicationTaskStructEncoder _disruptCommunicationTaskStructDecoder;
        EchelonEnumEncoder _echelonEnumDecoder;
        ElapsedTimeProgressEncoder _elapsedTimeProgressDecoder;
        Ellipsoid1GeomRecStructEncoder _ellipsoid1GeomRecStructDecoder;
        Ellipsoid2GeomRecStructEncoder _ellipsoid2GeomRecStructDecoder;
        EmitterFunctionEnumEncoder _emitterFunctionEnumDecoder;
        EmitterTypeEnumEncoder _emitterTypeEnumDecoder;
        EmptyTaskStructEncoder _emptyTaskStructDecoder;
        EncodingTypeEnumEncoder _encodingTypeEnumDecoder;
        EngineeringTaskStructEncoder _engineeringTaskStructDecoder;
        EntityControlActionEnumEncoder _entityControlActionEnumDecoder;
        EntityControlActionsEncoder _entityControlActionsDecoder;
        EntityCoordinateVectorStructEncoder _entityCoordinateVectorStructDecoder;
        EntityIdentifierStructEncoder _entityIdentifierStructDecoder;
        EntityTypeStructEncoder _entityTypeStructDecoder;
        EntityTypeStructLengthlessArrayEncoder _entityTypeStructLengthlessArrayDecoder;
        EnvironmentConditionModelTypeEnumEncoder _environmentConditionModelTypeEnumDecoder;
        EnvironmentDataCoordinateSystemEnumEncoder _environmentDataCoordinateSystemEnumDecoder;
        EnvironmentDataRepresentationEnumEncoder _environmentDataRepresentationEnumDecoder;
        EnvironmentDataSampleTypeEnumEncoder _environmentDataSampleTypeEnumDecoder;
        EnvironmentGridAxisTypeEnumEncoder _environmentGridAxisTypeEnumDecoder;
        EnvironmentGridTypeEnumEncoder _environmentGridTypeEnumDecoder;
        EnvironmentModelTypeEnumEncoder _environmentModelTypeEnumDecoder;
        EnvironmentObjectTypeStructEncoder _environmentObjectTypeStructDecoder;
        EnvironmentRecStructArrayEncoder _environmentRecStructArrayDecoder;
        EnvironmentRecStructEncoder _environmentRecStructDecoder;
        EnvironmentRecVariantStructEncoder _environmentRecVariantStructDecoder;
        EnvironmentRecordTypeEnumEncoder _environmentRecordTypeEnumDecoder;
        EnvironmentTypeStructEncoder _environmentTypeStructDecoder;
        EquipmentSymbolAmplificationStructEncoder _equipmentSymbolAmplificationStructDecoder;
        EstablishCheckpointTaskStructEncoder _establishCheckpointTaskStructDecoder;
        EstablishObservationPostTaskStructEncoder _establishObservationPostTaskStructDecoder;
        EvacuateProgressStructEncoder _evacuateProgressStructDecoder;
        EvacuateTaskStructEncoder _evacuateTaskStructDecoder;
        EventIdentifierStructEncoder _eventIdentifierStructDecoder;
        EventTypeEnumEncoder _eventTypeEnumDecoder;
        ExhaustSmokeStructEncoder _exhaustSmokeStructDecoder;
        ExhaustSmokeStructLengthlessArrayEncoder _exhaustSmokeStructLengthlessArrayDecoder;
        ExposureTypeEnumEncoder _exposureTypeEnumDecoder;
        ExtinguishWildfireTaskStructEncoder _extinguishWildfireTaskStructDecoder;
        FederateControlActionEnumEncoder _federateControlActionEnumDecoder;
        FederateControlActionsEncoder _federateControlActionsDecoder;
        FederateIdentifierStructEncoder _federateIdentifierStructDecoder;
        FireTaskProgressEncoder _fireTaskProgressDecoder;
        FixedDatumStructEncoder _fixedDatumStructDecoder;
        FixedDatumStructLengthlessArrayEncoder _fixedDatumStructLengthlessArrayDecoder;
        FlareStateRecStructEncoder _flareStateRecStructDecoder;
        Float32Array1PlusEncoder _float32Array1PlusDecoder;
        FollowEntityTaskStructEncoder _followEntityTaskStructDecoder;
        ForceIdentifierEnumEncoder _forceIdentifierEnumDecoder;
        FormationDataStructEncoder _formationDataStructDecoder;
        FormationEnumEncoder _formationEnumDecoder;
        FormationLocationTypeEnumEncoder _formationLocationTypeEnumDecoder;
        FormationStructEncoder _formationStructDecoder;
        FormationTypeEnumEncoder _formationTypeEnumDecoder;
        FundamentalParameterDataStructEncoder _fundamentalParameterDataStructDecoder;
        FundamentalParameterDataStructLengthlessArrayEncoder _fundamentalParameterDataStructLengthlessArrayDecoder;
        FuseTypeEnumEncoder _fuseTypeEnumDecoder;
        GaussPlumeGeomRecStructEncoder _gaussPlumeGeomRecStructDecoder;
        GaussPuffGeomRecStructEncoder _gaussPuffGeomRecStructDecoder;
        GeoLocationTypeEnumEncoder _geoLocationTypeEnumDecoder;
        GeoReferenceVariantEncoder _geoReferenceVariantDecoder;
        GeodeticCircleEncoder _geodeticCircleDecoder;
        GeodeticLocationEncoder _geodeticLocationDecoder;
        GeodeticPathEncoder _geodeticPathDecoder;
        GeodeticPointEncoder _geodeticPointDecoder;
        GeodeticPolygonEncoder _geodeticPolygonDecoder;
        GeodeticQuadrangleEncoder _geodeticQuadrangleDecoder;
        GridAxisStructEncoder _gridAxisStructDecoder;
        GridAxisStructLengthlessArrayEncoder _gridAxisStructLengthlessArrayDecoder;
        GridAxisTypeVariantStructEncoder _gridAxisTypeVariantStructDecoder;
        GridDataRepresentationVariantStructEncoder _gridDataRepresentationVariantStructDecoder;
        GridDataStructEncoder _gridDataStructDecoder;
        GridDataStructLengthlessArrayEncoder _gridDataStructLengthlessArrayDecoder;
        GridValueType0StructEncoder _gridValueType0StructDecoder;
        GridValueType1StructEncoder _gridValueType1StructDecoder;
        GridValueType2StructEncoder _gridValueType2StructDecoder;
        GroundFormationTypeEnumEncoder _groundFormationTypeEnumDecoder;
        HLAargumentListEncoder _hLAargumentListDecoder;
        HLAfederateStateEncoder _hLAfederateStateDecoder;
        HLAhandleListEncoder _hLAhandleListDecoder;
        HLAinteractionCountEncoder _hLAinteractionCountDecoder;
        HLAinteractionCountsEncoder _hLAinteractionCountsDecoder;
        HLAinteractionSubListEncoder _hLAinteractionSubListDecoder;
        HLAinteractionSubscriptionEncoder _hLAinteractionSubscriptionDecoder;
        HLAmoduleDesignatorListEncoder _hLAmoduleDesignatorListDecoder;
        HLAobjectClassBasedCountEncoder _hLAobjectClassBasedCountDecoder;
        HLAobjectClassBasedCountsEncoder _hLAobjectClassBasedCountsDecoder;
        HLAorderTypeEncoder _hLAorderTypeDecoder;
        HLAownershipEncoder _hLAownershipDecoder;
        HLAresignActionEncoder _hLAresignActionDecoder;
        HLAswitchEncoder _hLAswitchDecoder;
        HLAsynchPointFederateEncoder _hLAsynchPointFederateDecoder;
        HLAsynchPointFederateListEncoder _hLAsynchPointFederateListDecoder;
        HLAsynchPointListEncoder _hLAsynchPointListDecoder;
        HLAsynchPointStatusEncoder _hLAsynchPointStatusDecoder;
        HLAtimeStateEncoder _hLAtimeStateDecoder;
        HatchStateEnumEncoder _hatchStateEnumDecoder;
        HazardTypeEnum2Encoder _hazardTypeEnum2Decoder;
        HazardTypeEnumEncoder _hazardTypeEnumDecoder;
        HazeStructEncoder _hazeStructDecoder;
        HazeTypeEnumEncoder _hazeTypeEnumDecoder;
        HoldingEncoder _holdingDecoder;
        HostilityStatusCodeEnumEncoder _hostilityStatusCodeEnumDecoder;
        IPETypeEnumEncoder _iPETypeEnumDecoder;
        IceStructEncoder _iceStructDecoder;
        IceTypeEnumEncoder _iceTypeEnumDecoder;
        IdentificationLevelEnumEncoder _identificationLevelEnumDecoder;
        IffAlternateMode4EnumEncoder _iffAlternateMode4EnumDecoder;
        IffApplicableModesEnumEncoder _iffApplicableModesEnumDecoder;
        IffOperationalParameter1EnumEncoder _iffOperationalParameter1EnumDecoder;
        IffOperationalParameter2EnumEncoder _iffOperationalParameter2EnumDecoder;
        IffSystemModeEnumEncoder _iffSystemModeEnumDecoder;
        IffSystemNameEnumEncoder _iffSystemNameEnumDecoder;
        IffSystemTypeEnumEncoder _iffSystemTypeEnumDecoder;
        IncomingConnectionArrayEncoder _incomingConnectionArrayDecoder;
        IncomingConnectionStructEncoder _incomingConnectionStructDecoder;
        IndirectFireTaskStructEncoder _indirectFireTaskStructDecoder;
        InstallationSymbolAmplificationStructEncoder _installationSymbolAmplificationStructDecoder;
        Integer16Array1PlusEncoder _integer16Array1PlusDecoder;
        IrregularGridAxisStructEncoder _irregularGridAxisStructDecoder;
        IsPartOfStructEncoder _isPartOfStructDecoder;
        LandslideTypeEnumEncoder _landslideTypeEnumDecoder;
        LayMinefieldTaskStructEncoder _layMinefieldTaskStructDecoder;
        LayerStructEncoder _layerStructDecoder;
        LightningTypeEnumEncoder _lightningTypeEnumDecoder;
        Line1GeomRecStructEncoder _line1GeomRecStructDecoder;
        Line2GeomRecStructEncoder _line2GeomRecStructDecoder;
        LinearSegmentStructEncoder _linearSegmentStructDecoder;
        LinkStatusArrayEncoder _linkStatusArrayDecoder;
        LinkStatusStructEncoder _linkStatusStructDecoder;
        LocationStructArrayEncoder _locationStructArrayDecoder;
        LocationStructEncoder _locationStructDecoder;
        MagicMoveTaskStructEncoder _magicMoveTaskStructDecoder;
        MajorRFModulationTypeEnumEncoder _majorRFModulationTypeEnumDecoder;
        ManeuverIndicatorEnumEncoder _maneuverIndicatorEnumDecoder;
        ManufacturerIdArray3Encoder _manufacturerIdArray3Decoder;
        MarkingArray11Encoder _markingArray11Decoder;
        MarkingArray31Encoder _markingArray31Decoder;
        MarkingEncodingEnumEncoder _markingEncodingEnumDecoder;
        MarkingStructEncoder _markingStructDecoder;
        MineDielectricDifferenceLengthlessArrayEncoder _mineDielectricDifferenceLengthlessArrayDecoder;
        MineFusingStructEncoder _mineFusingStructDecoder;
        MineFusingStructLengthlessArrayEncoder _mineFusingStructLengthlessArrayDecoder;
        MineIdentifierLengthlessArrayEncoder _mineIdentifierLengthlessArrayDecoder;
        MinefieldFusingEnumEncoder _minefieldFusingEnumDecoder;
        MinefieldLaneEnumEncoder _minefieldLaneEnumDecoder;
        MinefieldLaneMarkerStructEncoder _minefieldLaneMarkerStructDecoder;
        MinefieldLaneMarkerStructLengthlessArrayEncoder _minefieldLaneMarkerStructLengthlessArrayDecoder;
        MinefieldPaintSchemeEnumEncoder _minefieldPaintSchemeEnumDecoder;
        MinefieldPaintSchemeLengthlessArrayEncoder _minefieldPaintSchemeLengthlessArrayDecoder;
        MinefieldProtocolEnumEncoder _minefieldProtocolEnumDecoder;
        MinefieldSensorTypeEnumEncoder _minefieldSensorTypeEnumDecoder;
        MinefieldSensorTypeLengthlessArrayEncoder _minefieldSensorTypeLengthlessArrayDecoder;
        MinefieldStatusEnumEncoder _minefieldStatusEnumDecoder;
        MinefieldTypeEnumEncoder _minefieldTypeEnumDecoder;
        MissingRecordNumbersLengthlessArray1PlusEncoder _missingRecordNumbersLengthlessArray1PlusDecoder;
        MoveByRouteTaskStructEncoder _moveByRouteTaskStructDecoder;
        MoveInDirectionTaskStructEncoder _moveInDirectionTaskStructDecoder;
        MoveIntoFormationTaskStructEncoder _moveIntoFormationTaskStructDecoder;
        MoveTaskProgressEncoder _moveTaskProgressDecoder;
        MoveToLocationTaskStructEncoder _moveToLocationTaskStructDecoder;
        MoveTypeEnumEncoder _moveTypeEnumDecoder;
        MsgIdEnumEncoder _msgIdEnumDecoder;
        NamedLocationStructEncoder _namedLocationStructDecoder;
        NavigationLightFlashingEnumEncoder _navigationLightFlashingEnumDecoder;
        NavigationStatusEnumEncoder _navigationStatusEnumDecoder;
        NetworkDeviceArrayEncoder _networkDeviceArrayDecoder;
        NetworkDeviceEmptyCharactersticsStructEncoder _networkDeviceEmptyCharactersticsStructDecoder;
        NetworkDeviceGenericTransmitterCharacteristicsStructEncoder _networkDeviceGenericTransmitterCharacteristicsStructDecoder;
        NetworkDeviceReceiverCharacteristicsVariantEncoder _networkDeviceReceiverCharacteristicsVariantDecoder;
        NetworkDeviceStructEncoder _networkDeviceStructDecoder;
        NetworkDeviceTransmitterCharacteristicsVariantEncoder _networkDeviceTransmitterCharacteristicsVariantDecoder;
        NomenclatureEnumEncoder _nomenclatureEnumDecoder;
        NomenclatureVersionEnumEncoder _nomenclatureVersionEnumDecoder;
        ObserveTaskStructEncoder _observeTaskStructDecoder;
        ObservedEquipmentEncoder _observedEquipmentDecoder;
        OctetArray1PlusEncoder _octetArray1PlusDecoder;
        OctetArray2Encoder _octetArray2Decoder;
        OctetArray3Encoder _octetArray3Decoder;
        OctetArray4Encoder _octetArray4Decoder;
        OctetArray7Encoder _octetArray7Decoder;
        OctetArray8Encoder _octetArray8Decoder;
        OctetArrayEncoder _octetArrayDecoder;
        OctetPadding32ArrayEncoder _octetPadding32ArrayDecoder;
        OctetPadding64ArrayEncoder _octetPadding64ArrayDecoder;
        OpacityCodeEnumEncoder _opacityCodeEnumDecoder;
        OperateCheckpointTaskStructEncoder _operateCheckpointTaskStructDecoder;
        OperateObservationPostTaskStructEncoder _operateObservationPostTaskStructDecoder;
        OrientationStructEncoder _orientationStructDecoder;
        OrientationStructLengthlessArrayEncoder _orientationStructLengthlessArrayDecoder;
        OtherActivityTaskStructEncoder _otherActivityTaskStructDecoder;
        ParameterTypeEnumEncoder _parameterTypeEnumDecoder;
        ParameterValueVariantStructEncoder _parameterValueVariantStructDecoder;
        PartNumberEnumEncoder _partNumberEnumDecoder;
        PatrolMoveTypeEnumEncoder _patrolMoveTypeEnumDecoder;
        PatrolTaskProgressEncoder _patrolTaskProgressDecoder;
        PatrolTaskStructEncoder _patrolTaskStructDecoder;
        PatrolTypeEnumEncoder _patrolTypeEnumDecoder;
        PerimeterPointStructEncoder _perimeterPointStructDecoder;
        PerimeterPointStructLengthlessArrayEncoder _perimeterPointStructLengthlessArrayDecoder;
        PhysicalGenericNetworkStructEncoder _physicalGenericNetworkStructDecoder;
        PhysicalNetworkDescriptionVariantEncoder _physicalNetworkDescriptionVariantDecoder;
        PhysicalNetworkTypeEnumEncoder _physicalNetworkTypeEnumDecoder;
        PhysicalUndefinedNetworkStructEncoder _physicalUndefinedNetworkStructDecoder;
        PlumeDimensionRateStructEncoder _plumeDimensionRateStructDecoder;
        PlumeDimensionStructEncoder _plumeDimensionStructDecoder;
        Point2GeomRecStructEncoder _point2GeomRecStructDecoder;
        PrecipitationStructEncoder _precipitationStructDecoder;
        PrecipitationTypeEnumEncoder _precipitationTypeEnumDecoder;
        PropulsionPlantEnumEncoder _propulsionPlantEnumDecoder;
        PropulsionSystemDataStructEncoder _propulsionSystemDataStructDecoder;
        PropulsionSystemDataStructLengthlessArrayEncoder _propulsionSystemDataStructLengthlessArrayDecoder;
        ProtectionEffectivenessStructEncoder _protectionEffectivenessStructDecoder;
        PulseModulationTypeEnumEncoder _pulseModulationTypeEnumDecoder;
        PumpFloodingTaskStructEncoder _pumpFloodingTaskStructDecoder;
        RFModulationSystemTypeEnumEncoder _rFModulationSystemTypeEnumDecoder;
        RFModulationTypeVariantStructEncoder _rFModulationTypeVariantStructDecoder;
        RTIobjectIdArrayEncoder _rTIobjectIdArrayDecoder;
        RadioInputSourceEnumEncoder _radioInputSourceEnumDecoder;
        RadioTypeStructEncoder _radioTypeStructDecoder;
        ReceiverOperationalStatusEnumEncoder _receiverOperationalStatusEnumDecoder;
        RecordSetStructArray1PlusEncoder _recordSetStructArray1PlusDecoder;
        RecordSetStructEncoder _recordSetStructDecoder;
        RecordStructArrayEncoder _recordStructArrayDecoder;
        RecordStructEncoder _recordStructDecoder;
        RectVol1GeomRecStructEncoder _rectVol1GeomRecStructDecoder;
        RectVol2GeomRecStructEncoder _rectVol2GeomRecStructDecoder;
        RectVol3GeomRecStructEncoder _rectVol3GeomRecStructDecoder;
        ReferenceSystemEnumEncoder _referenceSystemEnumDecoder;
        RefuelingStationLightColorEnumEncoder _refuelingStationLightColorEnumDecoder;
        RefuelingStationLightDayNightEnumEncoder _refuelingStationLightDayNightEnumDecoder;
        RelationEncoder _relationDecoder;
        RelativePositionStructEncoder _relativePositionStructDecoder;
        RelativeRangeBearingStructEncoder _relativeRangeBearingStructDecoder;
        ReleaseDistributionStructEncoder _releaseDistributionStructDecoder;
        ReleaseDynamicsStructEncoder _releaseDynamicsStructDecoder;
        ReleaseSizeStructEncoder _releaseSizeStructDecoder;
        RepairResultEnumEncoder _repairResultEnumDecoder;
        RepairTaskStructEncoder _repairTaskStructDecoder;
        RepairTypeEnumEncoder _repairTypeEnumDecoder;
        RequestStatusEnumEncoder _requestStatusEnumDecoder;
        RequestedConnectionArrayEncoder _requestedConnectionArrayDecoder;
        RequestedConnectionEncoder _requestedConnectionDecoder;
        ResourceStatusStructEncoder _resourceStatusStructDecoder;
        ResponseFlagEnumEncoder _responseFlagEnumDecoder;
        ResupplyTaskStructEncoder _resupplyTaskStructDecoder;
        RoadIceConditionEnumEncoder _roadIceConditionEnumDecoder;
        RoundStructEncoder _roundStructDecoder;
        RulesOfEngagementEnumEncoder _rulesOfEngagementEnumDecoder;
        SINCGARSModulationStructEncoder _sINCGARSModulationStructDecoder;
        SeaStateEnumEncoder _seaStateEnumDecoder;
        SedimentTypeEnumEncoder _sedimentTypeEnumDecoder;
        SendSafetyRelatedBroadcastMessageTaskStructEncoder _sendSafetyRelatedBroadcastMessageTaskStructDecoder;
        SendSafetyRelatedMessageTaskStructEncoder _sendSafetyRelatedMessageTaskStructDecoder;
        SensorStateEnumEncoder _sensorStateEnumDecoder;
        SensorStructEncoder _sensorStructDecoder;
        ServiceTypeEnumEncoder _serviceTypeEnumDecoder;
        SetTransmitterStatusTaskStructEncoder _setTransmitterStatusTaskStructDecoder;
        ShaftDataStructEncoder _shaftDataStructDecoder;
        ShaftDataStructLengthlessArray1PlusEncoder _shaftDataStructLengthlessArray1PlusDecoder;
        ShipTypeEnumEncoder _shipTypeEnumDecoder;
        SignalDataLengthlessArray1PlusEncoder _signalDataLengthlessArray1PlusDecoder;
        SignatureTypeEnumEncoder _signatureTypeEnumDecoder;
        SignatureVariantEncoder _signatureVariantDecoder;
        SilentAggregateStructEncoder _silentAggregateStructDecoder;
        SilentAggregateStructLengthlessArrayEncoder _silentAggregateStructLengthlessArrayDecoder;
        SilentEntityStructEncoder _silentEntityStructDecoder;
        SilentEntityStructLengthlessArrayEncoder _silentEntityStructLengthlessArrayDecoder;
        SnowStructEncoder _snowStructDecoder;
        SpatialFPStructEncoder _spatialFPStructDecoder;
        SpatialFVStructEncoder _spatialFVStructDecoder;
        SpatialRPStructEncoder _spatialRPStructDecoder;
        SpatialRVStructEncoder _spatialRVStructDecoder;
        SpatialStaticStructEncoder _spatialStaticStructDecoder;
        SpatialVariantStructEncoder _spatialVariantStructDecoder;
        Sphere1GeomRecStructEncoder _sphere1GeomRecStructDecoder;
        Sphere2GeomRecStructEncoder _sphere2GeomRecStructDecoder;
        SphericalHarmonicAntennaStructEncoder _sphericalHarmonicAntennaStructDecoder;
        SpreadSpectrumEnumEncoder _spreadSpectrumEnumDecoder;
        SpreadSpectrumVariantStructEncoder _spreadSpectrumVariantStructDecoder;
        StanceCodeEnumEncoder _stanceCodeEnumDecoder;
        StationEnumEncoder _stationEnumDecoder;
        StationNameLocationVariantStructEncoder _stationNameLocationVariantStructDecoder;
        StopFreezeReasonEnumEncoder _stopFreezeReasonEnumDecoder;
        SubsurfaceFormationTypeEnumEncoder _subsurfaceFormationTypeEnumDecoder;
        SupplyStatusStructEncoder _supplyStatusStructDecoder;
        SupplyStructEncoder _supplyStructDecoder;
        SupplyStructLengthlessArrayEncoder _supplyStructLengthlessArrayDecoder;
        SurfaceFormationTypeEnumEncoder _surfaceFormationTypeEnumDecoder;
        SurfaceMoistureEnumEncoder _surfaceMoistureEnumDecoder;
        SymbolAmplificationVariantEncoder _symbolAmplificationVariantDecoder;
        SymbolStructEncoder _symbolStructDecoder;
        SymbolTypeEnumEncoder _symbolTypeEnumDecoder;
        TacticalDataLinkTypeEnumEncoder _tacticalDataLinkTypeEnumDecoder;
        TaskDefinitionEncoder _taskDefinitionDecoder;
        TaskDefinitionVariantRecordEncoder _taskDefinitionVariantRecordDecoder;
        TaskModeEnumEncoder _taskModeEnumDecoder;
        TaskProgressEncoder _taskProgressDecoder;
        TaskProgressVariantRecordEncoder _taskProgressVariantRecordDecoder;
        TaskStatusEnumEncoder _taskStatusEnumDecoder;
        TemperatureDegreeCelsiusFloat32LengthlessArrayEncoder _temperatureDegreeCelsiusFloat32LengthlessArrayDecoder;
        TrackStructEncoder _trackStructDecoder;
        TrailingEffectsCodeEnumEncoder _trailingEffectsCodeEnumDecoder;
        TransferTypeEnumEncoder _transferTypeEnumDecoder;
        TransmitterOperationalStatusEnumEncoder _transmitterOperationalStatusEnumDecoder;
        TransportTaskStructEncoder _transportTaskStructDecoder;
        TreatmentStructEncoder _treatmentStructDecoder;
        UUIDEncoder _uUIDDecoder;
        UniformGeomRecStructEncoder _uniformGeomRecStructDecoder;
        UnitSymbolAmplificationStructEncoder _unitSymbolAmplificationStructDecoder;
        UnmodulatedTypeEnumEncoder _unmodulatedTypeEnumDecoder;
        UnsignedInteger16Array1PlusEncoder _unsignedInteger16Array1PlusDecoder;
        UnsignedInteger32LengthlessArrayEncoder _unsignedInteger32LengthlessArrayDecoder;
        UnsignedInteger64Array1PlusEncoder _unsignedInteger64Array1PlusDecoder;
        UnsignedInteger8LengthlessArrayEncoder _unsignedInteger8LengthlessArrayDecoder;
        UserProtocolEnumEncoder _userProtocolEnumDecoder;
        VariableDatumStructArrayEncoder _variableDatumStructArrayDecoder;
        VariableDatumStructEncoder _variableDatumStructDecoder;
        VariableDatumStructLengthlessArrayEncoder _variableDatumStructLengthlessArrayDecoder;
        VectoringNozzleSystemDataStructEncoder _vectoringNozzleSystemDataStructDecoder;
        VectoringNozzleSystemDataStructLengthlessArrayEncoder _vectoringNozzleSystemDataStructLengthlessArrayDecoder;
        VelocityVectorStructEncoder _velocityVectorStructDecoder;
        VisibleSideLocationEnumEncoder _visibleSideLocationEnumDecoder;
        WarheadTypeEnumEncoder _warheadTypeEnumDecoder;
        WaveStructEncoder _waveStructDecoder;
        WaypointEncoder _waypointDecoder;
        WeaponControlOrderEnumEncoder _weaponControlOrderEnumDecoder;
        WeaponStateEnumEncoder _weaponStateEnumDecoder;
        WindStructEncoder _windStructDecoder;
        WorldLocationStructEncoder _worldLocationStructDecoder;
        WorldLocationStructLengthlessArrayEncoder _worldLocationStructLengthlessArrayDecoder;
// === AUTO-GENERATED MEMBERS END ===
//pragma endregion
        EncodingHelpers::HLAboolean _boolDecoder;
        EncodingHelpers::HLAoctet _charDecoder;
        EncodingHelpers::HLAinteger16BE _shortDecoder;
        EncodingHelpers::UnsignedShort _unsignedshortDecoder;
        EncodingHelpers::HLAinteger32BE _intDecoder;
        EncodingHelpers::UnsignedInteger32BE _unsignedintDecoder;
        EncodingHelpers::HLAinteger64BE _longDecoder;
        EncodingHelpers::UnsignedInteger64BE _unsignedlongDecoder;
        EncodingHelpers::HLAinteger64BE _longlongDecoder;
        EncodingHelpers::UnsignedInteger64BE _unsignedlonglongDecoder;
        EncodingHelpers::HLAfloat32BE _floatDecoder;
        EncodingHelpers::HLAfloat64BE _doubleDecoder;
        EncodingHelpers::OMT13string _stringDecoder;
        EncodingHelpers::HLAunicodeString _wstringDecoder;

        Decoders();

    public:

        ~Decoders();

        static Decoders& getInstance()
        {
            static std::unique_ptr<Decoders> theInstance{ new Decoders() };
            return *theInstance;
        }

//pragma region Auto-generated Functions
        // === AUTO-GENERATED FUNCTIONS BEGIN ===
        DevStudio::ATP45HazardAreaTypeEnum::ATP45HazardAreaTypeEnum decodeATP45HazardAreaTypeEnum(const std::vector<char>& data);
        DevStudio::AccelerationVectorStruct decodeAccelerationVectorStruct(const std::vector<char>& data);
        DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum decodeAcknowledgeFlagEnum(const std::vector<char>& data);
        DevStudio::AcknowledgementProtocolEnum::AcknowledgementProtocolEnum decodeAcknowledgementProtocolEnum(const std::vector<char>& data);
        DevStudio::ActionEnum::ActionEnum decodeActionEnum(const std::vector<char>& data);
        DevStudio::ActionResultEnum::ActionResultEnum decodeActionResultEnum(const std::vector<char>& data);
        DevStudio::ActiveSonarEnum::ActiveSonarEnum decodeActiveSonarEnum(const std::vector<char>& data);
        DevStudio::ActiveSonarFunctionCodeEnum::ActiveSonarFunctionCodeEnum decodeActiveSonarFunctionCodeEnum(const std::vector<char>& data);
        DevStudio::ActiveSonarScanPatternEnum::ActiveSonarScanPatternEnum decodeActiveSonarScanPatternEnum(const std::vector<char>& data);
        DevStudio::ActiveStatusEnum::ActiveStatusEnum decodeActiveStatusEnum(const std::vector<char>& data);
        DevStudio::AdministerTreatmentTaskStruct decodeAdministerTreatmentTaskStruct(const std::vector<char>& data);
        DevStudio::AgentClassEnum::AgentClassEnum decodeAgentClassEnum(const std::vector<char>& data);
        DevStudio::AgentConcentrationStruct decodeAgentConcentrationStruct(const std::vector<char>& data);
        DevStudio::AgentMassStruct decodeAgentMassStruct(const std::vector<char>& data);
        DevStudio::AgentTypeEnum::AgentTypeEnum decodeAgentTypeEnum(const std::vector<char>& data);
        DevStudio::AggregateMarkingStruct decodeAggregateMarkingStruct(const std::vector<char>& data);
        DevStudio::AggregateMissionEnum::AggregateMissionEnum decodeAggregateMissionEnum(const std::vector<char>& data);
        DevStudio::AggregateStateEnum::AggregateStateEnum decodeAggregateStateEnum(const std::vector<char>& data);
        DevStudio::AggregateStateFormationEnum::AggregateStateFormationEnum decodeAggregateStateFormationEnum(const std::vector<char>& data);
        DevStudio::AidTypeEnum::AidTypeEnum decodeAidTypeEnum(const std::vector<char>& data);
        DevStudio::AirFormationTypeEnum::AirFormationTypeEnum decodeAirFormationTypeEnum(const std::vector<char>& data);
        DevStudio::AmplitudeAngleModulationTypeEnum::AmplitudeAngleModulationTypeEnum decodeAmplitudeAngleModulationTypeEnum(const std::vector<char>& data);
        DevStudio::AmplitudeModulationTypeEnum::AmplitudeModulationTypeEnum decodeAmplitudeModulationTypeEnum(const std::vector<char>& data);
        DevStudio::AngleModulationTypeEnum::AngleModulationTypeEnum decodeAngleModulationTypeEnum(const std::vector<char>& data);
        DevStudio::AngularVelocityVectorStruct decodeAngularVelocityVectorStruct(const std::vector<char>& data);
        DevStudio::AntennaPatternTypeEnum::AntennaPatternTypeEnum decodeAntennaPatternTypeEnum(const std::vector<char>& data);
        DevStudio::AntennaPatternVariantStruct decodeAntennaPatternVariantStruct(const std::vector<char>& data);
        std::vector< DevStudio::AntennaPatternVariantStruct > decodeAntennaPatternVariantStructLengthlessArray(const std::vector<char>& data);
        DevStudio::AntiCollisionLightFlashingEnum::AntiCollisionLightFlashingEnum decodeAntiCollisionLightFlashingEnum(const std::vector<char>& data);
        DevStudio::AppearanceAntiCollisionDayNightEnum::AppearanceAntiCollisionDayNightEnum decodeAppearanceAntiCollisionDayNightEnum(const std::vector<char>& data);
        DevStudio::AppearanceNVGModeEnum::AppearanceNVGModeEnum decodeAppearanceNVGModeEnum(const std::vector<char>& data);
        DevStudio::ApplyIPETaskStruct decodeApplyIPETaskStruct(const std::vector<char>& data);
        DevStudio::AppointmentStruct decodeAppointmentStruct(const std::vector<char>& data);
        std::vector< DevStudio::AgentConcentrationStruct > decodeArrayOfAgentConcentrationStruct(const std::vector<char>& data);
        std::vector< DevStudio::AgentMassStruct > decodeArrayOfAgentMassStruct(const std::vector<char>& data);
        std::vector< DevStudio::AgentTypeEnum::AgentTypeEnum > decodeArrayOfAgentTypeEnum(const std::vector<char>& data);
        std::vector< DevStudio::CBRNExposureStruct > decodeArrayOfCBRNExposureStruct(const std::vector<char>& data);
        std::vector< DevStudio::CBRNSensorReadingStruct > decodeArrayOfCBRNSensorReadingStruct(const std::vector<char>& data);
        std::vector< std::vector< char > > decodeArrayOfCommunicationNetworks(const std::vector<char>& data);
        std::vector< DevStudio::ContourStruct > decodeArrayOfContourStruct(const std::vector<char>& data);
        std::vector< DevStudio::DetectedEquipment > decodeArrayOfDetectedEquipment(const std::vector<char>& data);
        std::vector< DevStudio::Holding > decodeArrayOfHoldings(const std::vector<char>& data);
        std::vector< DevStudio::ObservedEquipment > decodeArrayOfObservedEquipment(const std::vector<char>& data);
        std::vector< DevStudio::ProtectionEffectivenessStruct > decodeArrayOfProtectionEffectivenessStruct(const std::vector<char>& data);
        std::vector< DevStudio::Relation > decodeArrayOfRelationship(const std::vector<char>& data);
        std::vector< DevStudio::ResourceStatusStruct > decodeArrayOfResourceStatus(const std::vector<char>& data);
        std::vector< DevStudio::SensorStruct > decodeArrayOfSensor(const std::vector<char>& data);
        std::vector< float > decodeArrayOfSigmas6(const std::vector<char>& data);
        std::vector< DevStudio::SignatureVariant > decodeArrayOfSignature(const std::vector<char>& data);
        std::vector< DevStudio::SupplyStatusStruct > decodeArrayOfSupplyStatus(const std::vector<char>& data);
        std::vector< DevStudio::TaskDefinition > decodeArrayOfTaskDefinitions(const std::vector<char>& data);
        std::vector< DevStudio::TaskProgress > decodeArrayOfTaskProgress(const std::vector<char>& data);
        std::vector< DevStudio::TrackStruct > decodeArrayOfTrack(const std::vector<char>& data);
        std::vector< DevStudio::TreatmentStruct > decodeArrayOfTreatmentStruct(const std::vector<char>& data);
        std::vector< std::vector< char > > decodeArrayOfUuid(const std::vector<char>& data);
        std::vector< DevStudio::Waypoint > decodeArrayOfWaypoints(const std::vector<char>& data);
        DevStudio::ArticulatedParameterStruct decodeArticulatedParameterStruct(const std::vector<char>& data);
        std::vector< DevStudio::ArticulatedParameterStruct > decodeArticulatedParameterStructLengthlessArray(const std::vector<char>& data);
        DevStudio::ArticulatedPartsStruct decodeArticulatedPartsStruct(const std::vector<char>& data);
        DevStudio::ArticulatedPartsTypeEnum::ArticulatedPartsTypeEnum decodeArticulatedPartsTypeEnum(const std::vector<char>& data);
        DevStudio::ArticulatedTypeMetricEnum::ArticulatedTypeMetricEnum decodeArticulatedTypeMetricEnum(const std::vector<char>& data);
        DevStudio::AttachTaskStruct decodeAttachTaskStruct(const std::vector<char>& data);
        DevStudio::AttachedPartsStruct decodeAttachedPartsStruct(const std::vector<char>& data);
        DevStudio::AttackTypeEnum::AttackTypeEnum decodeAttackTypeEnum(const std::vector<char>& data);
        DevStudio::AttributeValuePairStruct decodeAttributeValuePairStruct(const std::vector<char>& data);
        std::vector< DevStudio::AttributeValuePairStruct > decodeAttributeValuePairStructArray1Plus(const std::vector<char>& data);
        DevStudio::AudioDataTypeStruct decodeAudioDataTypeStruct(const std::vector<char>& data);
        DevStudio::BeamAntennaStruct decodeBeamAntennaStruct(const std::vector<char>& data);
        DevStudio::BeamFunctionCodeEnum::BeamFunctionCodeEnum decodeBeamFunctionCodeEnum(const std::vector<char>& data);
        std::vector< bool > decodeBinArray(const std::vector<char>& data);
        DevStudio::BreachStruct decodeBreachStruct(const std::vector<char>& data);
        std::vector< DevStudio::BreachStruct > decodeBreachStructLengthlessArray(const std::vector<char>& data);
        DevStudio::BreachableSegmentStruct decodeBreachableSegmentStruct(const std::vector<char>& data);
        std::vector< DevStudio::BreachableSegmentStruct > decodeBreachableSegmentStructLengthlessArray(const std::vector<char>& data);
        std::vector< DevStudio::BreachedStatusEnum::BreachedStatusEnum > decodeBreachedStatusArray8(const std::vector<char>& data);
        DevStudio::BreachedStatusEnum::BreachedStatusEnum decodeBreachedStatusEnum(const std::vector<char>& data);
        DevStudio::CBRNAlarmStruct decodeCBRNAlarmStruct(const std::vector<char>& data);
        DevStudio::CBRNDamageEnum::CBRNDamageEnum decodeCBRNDamageEnum(const std::vector<char>& data);
        DevStudio::CBRNExposureStruct decodeCBRNExposureStruct(const std::vector<char>& data);
        DevStudio::CBRNSensorReadingStruct decodeCBRNSensorReadingStruct(const std::vector<char>& data);
        DevStudio::COMBICStateRecStruct decodeCOMBICStateRecStruct(const std::vector<char>& data);
        DevStudio::CamouflageEnum::CamouflageEnum decodeCamouflageEnum(const std::vector<char>& data);
        DevStudio::CaptureStatusEnum::CaptureStatusEnum decodeCaptureStatusEnum(const std::vector<char>& data);
        DevStudio::ChangeAltitudeTaskStruct decodeChangeAltitudeTaskStruct(const std::vector<char>& data);
        DevStudio::ChangeHeadingTaskStruct decodeChangeHeadingTaskStruct(const std::vector<char>& data);
        DevStudio::ChangeRulesOfEngagementTaskStruct decodeChangeRulesOfEngagementTaskStruct(const std::vector<char>& data);
        DevStudio::ChangeSpeedTaskStruct decodeChangeSpeedTaskStruct(const std::vector<char>& data);
        DevStudio::ChemicalContentEnum::ChemicalContentEnum decodeChemicalContentEnum(const std::vector<char>& data);
        DevStudio::ClearEngineeringTaskStruct decodeClearEngineeringTaskStruct(const std::vector<char>& data);
        DevStudio::ClockTimeStruct decodeClockTimeStruct(const std::vector<char>& data);
        std::vector< DevStudio::ClockTimeStruct > decodeClockTimeStructLengthlessArray(const std::vector<char>& data);
        DevStudio::CloudStruct decodeCloudStruct(const std::vector<char>& data);
        DevStudio::CloudTypeEnum::CloudTypeEnum decodeCloudTypeEnum(const std::vector<char>& data);
        std::vector< float > decodeCoefficientsLengthlessArray1Plus(const std::vector<char>& data);
        DevStudio::CollisionTypeEnum::CollisionTypeEnum decodeCollisionTypeEnum(const std::vector<char>& data);
        DevStudio::CombinationModulationTypeEnum::CombinationModulationTypeEnum decodeCombinationModulationTypeEnum(const std::vector<char>& data);
        DevStudio::CommunicationNetworkTypeEnum::CommunicationNetworkTypeEnum decodeCommunicationNetworkTypeEnum(const std::vector<char>& data);
        DevStudio::CommunicationServiceTypeEnum::CommunicationServiceTypeEnum decodeCommunicationServiceTypeEnum(const std::vector<char>& data);
        DevStudio::ComplianceStateEnum::ComplianceStateEnum decodeComplianceStateEnum(const std::vector<char>& data);
        DevStudio::Cone1GeomRecStruct decodeCone1GeomRecStruct(const std::vector<char>& data);
        DevStudio::Cone2GeomRecStruct decodeCone2GeomRecStruct(const std::vector<char>& data);
        std::vector< DevStudio::ConnectionReceiverStruct > decodeConnectionReceiverArray(const std::vector<char>& data);
        DevStudio::ConnectionReceiverStruct decodeConnectionReceiverStruct(const std::vector<char>& data);
        DevStudio::ConnectionTypeEnum::ConnectionTypeEnum decodeConnectionTypeEnum(const std::vector<char>& data);
        DevStudio::ConstituentPartNatureEnum::ConstituentPartNatureEnum decodeConstituentPartNatureEnum(const std::vector<char>& data);
        DevStudio::ConstituentPartPositionEnum::ConstituentPartPositionEnum decodeConstituentPartPositionEnum(const std::vector<char>& data);
        DevStudio::ConstituentPartRelationshipStruct decodeConstituentPartRelationshipStruct(const std::vector<char>& data);
        DevStudio::ConstituentPartStationNameEnum::ConstituentPartStationNameEnum decodeConstituentPartStationNameEnum(const std::vector<char>& data);
        DevStudio::ContourStruct decodeContourStruct(const std::vector<char>& data);
        DevStudio::CreateBreachTaskStruct decodeCreateBreachTaskStruct(const std::vector<char>& data);
        DevStudio::CreateFireBreakTaskStruct decodeCreateFireBreakTaskStruct(const std::vector<char>& data);
        DevStudio::CreateLeveeTaskStruct decodeCreateLeveeTaskStruct(const std::vector<char>& data);
        DevStudio::CreateObstacleTaskStruct decodeCreateObstacleTaskStruct(const std::vector<char>& data);
        DevStudio::CryptographicModeEnum::CryptographicModeEnum decodeCryptographicModeEnum(const std::vector<char>& data);
        DevStudio::CryptographicSystemTypeEnum::CryptographicSystemTypeEnum decodeCryptographicSystemTypeEnum(const std::vector<char>& data);
        DevStudio::CurrentStruct decodeCurrentStruct(const std::vector<char>& data);
        DevStudio::DamageStatusEnhancedEnum::DamageStatusEnhancedEnum decodeDamageStatusEnhancedEnum(const std::vector<char>& data);
        DevStudio::DamageStatusEnum::DamageStatusEnum decodeDamageStatusEnum(const std::vector<char>& data);
        DevStudio::DatumIdentifierEnum::DatumIdentifierEnum decodeDatumIdentifierEnum(const std::vector<char>& data);
        std::vector< DevStudio::DatumIdentifierEnum::DatumIdentifierEnum > decodeDatumIdentifierLengthlessArray(const std::vector<char>& data);
        DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum decodeDeadReckoningAlgorithmEnum(const std::vector<char>& data);
        std::vector< float > decodeDepthMeterFloat32LengthlessArray(const std::vector<char>& data);
        DevStudio::DesignatorCodeEnum::DesignatorCodeEnum decodeDesignatorCodeEnum(const std::vector<char>& data);
        DevStudio::DesignatorCodeNameEnum::DesignatorCodeNameEnum decodeDesignatorCodeNameEnum(const std::vector<char>& data);
        DevStudio::DetectedEquipment decodeDetectedEquipment(const std::vector<char>& data);
        DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum decodeDetonationResultCodeEnum(const std::vector<char>& data);
        DevStudio::DimensionRateStruct decodeDimensionRateStruct(const std::vector<char>& data);
        DevStudio::DimensionStruct decodeDimensionStruct(const std::vector<char>& data);
        DevStudio::DirectFireTaskStruct decodeDirectFireTaskStruct(const std::vector<char>& data);
        DevStudio::DisruptCommunicationTaskStruct decodeDisruptCommunicationTaskStruct(const std::vector<char>& data);
        DevStudio::EchelonEnum::EchelonEnum decodeEchelonEnum(const std::vector<char>& data);
        DevStudio::ElapsedTimeProgress decodeElapsedTimeProgress(const std::vector<char>& data);
        DevStudio::Ellipsoid1GeomRecStruct decodeEllipsoid1GeomRecStruct(const std::vector<char>& data);
        DevStudio::Ellipsoid2GeomRecStruct decodeEllipsoid2GeomRecStruct(const std::vector<char>& data);
        DevStudio::EmitterFunctionEnum::EmitterFunctionEnum decodeEmitterFunctionEnum(const std::vector<char>& data);
        DevStudio::EmitterTypeEnum::EmitterTypeEnum decodeEmitterTypeEnum(const std::vector<char>& data);
        DevStudio::EmptyTaskStruct decodeEmptyTaskStruct(const std::vector<char>& data);
        DevStudio::EncodingTypeEnum::EncodingTypeEnum decodeEncodingTypeEnum(const std::vector<char>& data);
        DevStudio::EngineeringTaskStruct decodeEngineeringTaskStruct(const std::vector<char>& data);
        DevStudio::EntityControlActionEnum::EntityControlActionEnum decodeEntityControlActionEnum(const std::vector<char>& data);
        std::vector< DevStudio::EntityControlActionEnum::EntityControlActionEnum > decodeEntityControlActions(const std::vector<char>& data);
        DevStudio::EntityCoordinateVectorStruct decodeEntityCoordinateVectorStruct(const std::vector<char>& data);
        DevStudio::EntityIdentifierStruct decodeEntityIdentifierStruct(const std::vector<char>& data);
        DevStudio::EntityTypeStruct decodeEntityTypeStruct(const std::vector<char>& data);
        std::vector< DevStudio::EntityTypeStruct > decodeEntityTypeStructLengthlessArray(const std::vector<char>& data);
        DevStudio::EnvironmentConditionModelTypeEnum::EnvironmentConditionModelTypeEnum decodeEnvironmentConditionModelTypeEnum(const std::vector<char>& data);
        DevStudio::EnvironmentDataCoordinateSystemEnum::EnvironmentDataCoordinateSystemEnum decodeEnvironmentDataCoordinateSystemEnum(const std::vector<char>& data);
        DevStudio::EnvironmentDataRepresentationEnum::EnvironmentDataRepresentationEnum decodeEnvironmentDataRepresentationEnum(const std::vector<char>& data);
        DevStudio::EnvironmentDataSampleTypeEnum::EnvironmentDataSampleTypeEnum decodeEnvironmentDataSampleTypeEnum(const std::vector<char>& data);
        DevStudio::EnvironmentGridAxisTypeEnum::EnvironmentGridAxisTypeEnum decodeEnvironmentGridAxisTypeEnum(const std::vector<char>& data);
        DevStudio::EnvironmentGridTypeEnum::EnvironmentGridTypeEnum decodeEnvironmentGridTypeEnum(const std::vector<char>& data);
        DevStudio::EnvironmentModelTypeEnum::EnvironmentModelTypeEnum decodeEnvironmentModelTypeEnum(const std::vector<char>& data);
        DevStudio::EnvironmentObjectTypeStruct decodeEnvironmentObjectTypeStruct(const std::vector<char>& data);
        DevStudio::EnvironmentRecStruct decodeEnvironmentRecStruct(const std::vector<char>& data);
        std::vector< DevStudio::EnvironmentRecStruct > decodeEnvironmentRecStructArray(const std::vector<char>& data);
        DevStudio::EnvironmentRecVariantStruct decodeEnvironmentRecVariantStruct(const std::vector<char>& data);
        DevStudio::EnvironmentRecordTypeEnum::EnvironmentRecordTypeEnum decodeEnvironmentRecordTypeEnum(const std::vector<char>& data);
        DevStudio::EnvironmentTypeStruct decodeEnvironmentTypeStruct(const std::vector<char>& data);
        DevStudio::EquipmentSymbolAmplificationStruct decodeEquipmentSymbolAmplificationStruct(const std::vector<char>& data);
        DevStudio::EstablishCheckpointTaskStruct decodeEstablishCheckpointTaskStruct(const std::vector<char>& data);
        DevStudio::EstablishObservationPostTaskStruct decodeEstablishObservationPostTaskStruct(const std::vector<char>& data);
        DevStudio::EvacuateProgressStruct decodeEvacuateProgressStruct(const std::vector<char>& data);
        DevStudio::EvacuateTaskStruct decodeEvacuateTaskStruct(const std::vector<char>& data);
        DevStudio::EventIdentifierStruct decodeEventIdentifierStruct(const std::vector<char>& data);
        DevStudio::EventTypeEnum::EventTypeEnum decodeEventTypeEnum(const std::vector<char>& data);
        DevStudio::ExhaustSmokeStruct decodeExhaustSmokeStruct(const std::vector<char>& data);
        std::vector< DevStudio::ExhaustSmokeStruct > decodeExhaustSmokeStructLengthlessArray(const std::vector<char>& data);
        DevStudio::ExposureTypeEnum::ExposureTypeEnum decodeExposureTypeEnum(const std::vector<char>& data);
        DevStudio::ExtinguishWildfireTaskStruct decodeExtinguishWildfireTaskStruct(const std::vector<char>& data);
        DevStudio::FederateControlActionEnum::FederateControlActionEnum decodeFederateControlActionEnum(const std::vector<char>& data);
        std::vector< DevStudio::FederateControlActionEnum::FederateControlActionEnum > decodeFederateControlActions(const std::vector<char>& data);
        DevStudio::FederateIdentifierStruct decodeFederateIdentifierStruct(const std::vector<char>& data);
        DevStudio::FireTaskProgress decodeFireTaskProgress(const std::vector<char>& data);
        DevStudio::FixedDatumStruct decodeFixedDatumStruct(const std::vector<char>& data);
        std::vector< DevStudio::FixedDatumStruct > decodeFixedDatumStructLengthlessArray(const std::vector<char>& data);
        DevStudio::FlareStateRecStruct decodeFlareStateRecStruct(const std::vector<char>& data);
        std::vector< float > decodeFloat32Array1Plus(const std::vector<char>& data);
        DevStudio::FollowEntityTaskStruct decodeFollowEntityTaskStruct(const std::vector<char>& data);
        DevStudio::ForceIdentifierEnum::ForceIdentifierEnum decodeForceIdentifierEnum(const std::vector<char>& data);
        DevStudio::FormationDataStruct decodeFormationDataStruct(const std::vector<char>& data);
        DevStudio::FormationEnum::FormationEnum decodeFormationEnum(const std::vector<char>& data);
        DevStudio::FormationLocationTypeEnum::FormationLocationTypeEnum decodeFormationLocationTypeEnum(const std::vector<char>& data);
        DevStudio::FormationStruct decodeFormationStruct(const std::vector<char>& data);
        DevStudio::FormationTypeEnum::FormationTypeEnum decodeFormationTypeEnum(const std::vector<char>& data);
        DevStudio::FundamentalParameterDataStruct decodeFundamentalParameterDataStruct(const std::vector<char>& data);
        std::vector< DevStudio::FundamentalParameterDataStruct > decodeFundamentalParameterDataStructLengthlessArray(const std::vector<char>& data);
        DevStudio::FuseTypeEnum::FuseTypeEnum decodeFuseTypeEnum(const std::vector<char>& data);
        DevStudio::GaussPlumeGeomRecStruct decodeGaussPlumeGeomRecStruct(const std::vector<char>& data);
        DevStudio::GaussPuffGeomRecStruct decodeGaussPuffGeomRecStruct(const std::vector<char>& data);
        DevStudio::GeoLocationTypeEnum::GeoLocationTypeEnum decodeGeoLocationTypeEnum(const std::vector<char>& data);
        DevStudio::GeoReferenceVariant decodeGeoReferenceVariant(const std::vector<char>& data);
        DevStudio::GeodeticCircle decodeGeodeticCircle(const std::vector<char>& data);
        DevStudio::GeodeticLocation decodeGeodeticLocation(const std::vector<char>& data);
        std::vector< DevStudio::GeodeticPoint > decodeGeodeticPath(const std::vector<char>& data);
        DevStudio::GeodeticPoint decodeGeodeticPoint(const std::vector<char>& data);
        std::vector< DevStudio::GeodeticLocation > decodeGeodeticPolygon(const std::vector<char>& data);
        DevStudio::GeodeticQuadrangle decodeGeodeticQuadrangle(const std::vector<char>& data);
        DevStudio::GridAxisStruct decodeGridAxisStruct(const std::vector<char>& data);
        std::vector< DevStudio::GridAxisStruct > decodeGridAxisStructLengthlessArray(const std::vector<char>& data);
        DevStudio::GridAxisTypeVariantStruct decodeGridAxisTypeVariantStruct(const std::vector<char>& data);
        DevStudio::GridDataRepresentationVariantStruct decodeGridDataRepresentationVariantStruct(const std::vector<char>& data);
        DevStudio::GridDataStruct decodeGridDataStruct(const std::vector<char>& data);
        std::vector< DevStudio::GridDataStruct > decodeGridDataStructLengthlessArray(const std::vector<char>& data);
        DevStudio::GridValueType0Struct decodeGridValueType0Struct(const std::vector<char>& data);
        DevStudio::GridValueType1Struct decodeGridValueType1Struct(const std::vector<char>& data);
        DevStudio::GridValueType2Struct decodeGridValueType2Struct(const std::vector<char>& data);
        DevStudio::GroundFormationTypeEnum::GroundFormationTypeEnum decodeGroundFormationTypeEnum(const std::vector<char>& data);
        std::vector< std::wstring > decodeHLAargumentList(const std::vector<char>& data);
        DevStudio::HLAfederateState::HLAfederateState decodeHLAfederateState(const std::vector<char>& data);
        std::vector< std::vector<char > > decodeHLAhandleList(const std::vector<char>& data);
        DevStudio::HLAinteractionCount decodeHLAinteractionCount(const std::vector<char>& data);
        std::vector< DevStudio::HLAinteractionCount > decodeHLAinteractionCounts(const std::vector<char>& data);
        std::vector< DevStudio::HLAinteractionSubscription > decodeHLAinteractionSubList(const std::vector<char>& data);
        DevStudio::HLAinteractionSubscription decodeHLAinteractionSubscription(const std::vector<char>& data);
        std::vector< std::wstring > decodeHLAmoduleDesignatorList(const std::vector<char>& data);
        DevStudio::HLAobjectClassBasedCount decodeHLAobjectClassBasedCount(const std::vector<char>& data);
        std::vector< DevStudio::HLAobjectClassBasedCount > decodeHLAobjectClassBasedCounts(const std::vector<char>& data);
        DevStudio::HLAorderType::HLAorderType decodeHLAorderType(const std::vector<char>& data);
        DevStudio::HLAownership::HLAownership decodeHLAownership(const std::vector<char>& data);
        DevStudio::HLAresignAction::HLAresignAction decodeHLAresignAction(const std::vector<char>& data);
        DevStudio::HLAswitch::HLAswitch decodeHLAswitch(const std::vector<char>& data);
        DevStudio::HLAsynchPointFederate decodeHLAsynchPointFederate(const std::vector<char>& data);
        std::vector< DevStudio::HLAsynchPointFederate > decodeHLAsynchPointFederateList(const std::vector<char>& data);
        std::vector< std::wstring > decodeHLAsynchPointList(const std::vector<char>& data);
        DevStudio::HLAsynchPointStatus::HLAsynchPointStatus decodeHLAsynchPointStatus(const std::vector<char>& data);
        DevStudio::HLAtimeState::HLAtimeState decodeHLAtimeState(const std::vector<char>& data);
        DevStudio::HatchStateEnum::HatchStateEnum decodeHatchStateEnum(const std::vector<char>& data);
        DevStudio::HazardTypeEnum::HazardTypeEnum decodeHazardTypeEnum(const std::vector<char>& data);
        DevStudio::HazardTypeEnum2::HazardTypeEnum2 decodeHazardTypeEnum2(const std::vector<char>& data);
        DevStudio::HazeStruct decodeHazeStruct(const std::vector<char>& data);
        DevStudio::HazeTypeEnum::HazeTypeEnum decodeHazeTypeEnum(const std::vector<char>& data);
        DevStudio::Holding decodeHolding(const std::vector<char>& data);
        DevStudio::HostilityStatusCodeEnum::HostilityStatusCodeEnum decodeHostilityStatusCodeEnum(const std::vector<char>& data);
        DevStudio::IPETypeEnum::IPETypeEnum decodeIPETypeEnum(const std::vector<char>& data);
        DevStudio::IceStruct decodeIceStruct(const std::vector<char>& data);
        DevStudio::IceTypeEnum::IceTypeEnum decodeIceTypeEnum(const std::vector<char>& data);
        DevStudio::IdentificationLevelEnum::IdentificationLevelEnum decodeIdentificationLevelEnum(const std::vector<char>& data);
        DevStudio::IffAlternateMode4Enum::IffAlternateMode4Enum decodeIffAlternateMode4Enum(const std::vector<char>& data);
        DevStudio::IffApplicableModesEnum::IffApplicableModesEnum decodeIffApplicableModesEnum(const std::vector<char>& data);
        DevStudio::IffOperationalParameter1Enum::IffOperationalParameter1Enum decodeIffOperationalParameter1Enum(const std::vector<char>& data);
        DevStudio::IffOperationalParameter2Enum::IffOperationalParameter2Enum decodeIffOperationalParameter2Enum(const std::vector<char>& data);
        DevStudio::IffSystemModeEnum::IffSystemModeEnum decodeIffSystemModeEnum(const std::vector<char>& data);
        DevStudio::IffSystemNameEnum::IffSystemNameEnum decodeIffSystemNameEnum(const std::vector<char>& data);
        DevStudio::IffSystemTypeEnum::IffSystemTypeEnum decodeIffSystemTypeEnum(const std::vector<char>& data);
        std::vector< DevStudio::IncomingConnectionStruct > decodeIncomingConnectionArray(const std::vector<char>& data);
        DevStudio::IncomingConnectionStruct decodeIncomingConnectionStruct(const std::vector<char>& data);
        DevStudio::IndirectFireTaskStruct decodeIndirectFireTaskStruct(const std::vector<char>& data);
        DevStudio::InstallationSymbolAmplificationStruct decodeInstallationSymbolAmplificationStruct(const std::vector<char>& data);
        std::vector< short > decodeInteger16Array1Plus(const std::vector<char>& data);
        DevStudio::IrregularGridAxisStruct decodeIrregularGridAxisStruct(const std::vector<char>& data);
        DevStudio::IsPartOfStruct decodeIsPartOfStruct(const std::vector<char>& data);
        DevStudio::LandslideTypeEnum::LandslideTypeEnum decodeLandslideTypeEnum(const std::vector<char>& data);
        DevStudio::LayMinefieldTaskStruct decodeLayMinefieldTaskStruct(const std::vector<char>& data);
        DevStudio::LayerStruct decodeLayerStruct(const std::vector<char>& data);
        DevStudio::LightningTypeEnum::LightningTypeEnum decodeLightningTypeEnum(const std::vector<char>& data);
        DevStudio::Line1GeomRecStruct decodeLine1GeomRecStruct(const std::vector<char>& data);
        DevStudio::Line2GeomRecStruct decodeLine2GeomRecStruct(const std::vector<char>& data);
        DevStudio::LinearSegmentStruct decodeLinearSegmentStruct(const std::vector<char>& data);
        std::vector< DevStudio::LinkStatusStruct > decodeLinkStatusArray(const std::vector<char>& data);
        DevStudio::LinkStatusStruct decodeLinkStatusStruct(const std::vector<char>& data);
        DevStudio::LocationStruct decodeLocationStruct(const std::vector<char>& data);
        std::vector< DevStudio::LocationStruct > decodeLocationStructArray(const std::vector<char>& data);
        DevStudio::MagicMoveTaskStruct decodeMagicMoveTaskStruct(const std::vector<char>& data);
        DevStudio::MajorRFModulationTypeEnum::MajorRFModulationTypeEnum decodeMajorRFModulationTypeEnum(const std::vector<char>& data);
        DevStudio::ManeuverIndicatorEnum::ManeuverIndicatorEnum decodeManeuverIndicatorEnum(const std::vector<char>& data);
        std::vector< char > decodeManufacturerIdArray3(const std::vector<char>& data);
        std::vector< char > decodeMarkingArray11(const std::vector<char>& data);
        std::vector< char > decodeMarkingArray31(const std::vector<char>& data);
        DevStudio::MarkingEncodingEnum::MarkingEncodingEnum decodeMarkingEncodingEnum(const std::vector<char>& data);
        DevStudio::MarkingStruct decodeMarkingStruct(const std::vector<char>& data);
        std::vector< float > decodeMineDielectricDifferenceLengthlessArray(const std::vector<char>& data);
        DevStudio::MineFusingStruct decodeMineFusingStruct(const std::vector<char>& data);
        std::vector< DevStudio::MineFusingStruct > decodeMineFusingStructLengthlessArray(const std::vector<char>& data);
        std::vector< unsigned short > decodeMineIdentifierLengthlessArray(const std::vector<char>& data);
        DevStudio::MinefieldFusingEnum::MinefieldFusingEnum decodeMinefieldFusingEnum(const std::vector<char>& data);
        DevStudio::MinefieldLaneEnum::MinefieldLaneEnum decodeMinefieldLaneEnum(const std::vector<char>& data);
        DevStudio::MinefieldLaneMarkerStruct decodeMinefieldLaneMarkerStruct(const std::vector<char>& data);
        std::vector< DevStudio::MinefieldLaneMarkerStruct > decodeMinefieldLaneMarkerStructLengthlessArray(const std::vector<char>& data);
        DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum decodeMinefieldPaintSchemeEnum(const std::vector<char>& data);
        std::vector< DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum > decodeMinefieldPaintSchemeLengthlessArray(const std::vector<char>& data);
        DevStudio::MinefieldProtocolEnum::MinefieldProtocolEnum decodeMinefieldProtocolEnum(const std::vector<char>& data);
        DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum decodeMinefieldSensorTypeEnum(const std::vector<char>& data);
        std::vector< DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum > decodeMinefieldSensorTypeLengthlessArray(const std::vector<char>& data);
        DevStudio::MinefieldStatusEnum::MinefieldStatusEnum decodeMinefieldStatusEnum(const std::vector<char>& data);
        DevStudio::MinefieldTypeEnum::MinefieldTypeEnum decodeMinefieldTypeEnum(const std::vector<char>& data);
        std::vector< char > decodeMissingRecordNumbersLengthlessArray1Plus(const std::vector<char>& data);
        DevStudio::MoveByRouteTaskStruct decodeMoveByRouteTaskStruct(const std::vector<char>& data);
        DevStudio::MoveInDirectionTaskStruct decodeMoveInDirectionTaskStruct(const std::vector<char>& data);
        DevStudio::MoveIntoFormationTaskStruct decodeMoveIntoFormationTaskStruct(const std::vector<char>& data);
        DevStudio::MoveTaskProgress decodeMoveTaskProgress(const std::vector<char>& data);
        DevStudio::MoveToLocationTaskStruct decodeMoveToLocationTaskStruct(const std::vector<char>& data);
        DevStudio::MoveTypeEnum::MoveTypeEnum decodeMoveTypeEnum(const std::vector<char>& data);
        DevStudio::MsgIdEnum::MsgIdEnum decodeMsgIdEnum(const std::vector<char>& data);
        DevStudio::NamedLocationStruct decodeNamedLocationStruct(const std::vector<char>& data);
        DevStudio::NavigationLightFlashingEnum::NavigationLightFlashingEnum decodeNavigationLightFlashingEnum(const std::vector<char>& data);
        DevStudio::NavigationStatusEnum::NavigationStatusEnum decodeNavigationStatusEnum(const std::vector<char>& data);
        std::vector< DevStudio::NetworkDeviceStruct > decodeNetworkDeviceArray(const std::vector<char>& data);
        DevStudio::NetworkDeviceEmptyCharactersticsStruct decodeNetworkDeviceEmptyCharactersticsStruct(const std::vector<char>& data);
        DevStudio::NetworkDeviceGenericTransmitterCharacteristicsStruct decodeNetworkDeviceGenericTransmitterCharacteristicsStruct(const std::vector<char>& data);
        DevStudio::NetworkDeviceReceiverCharacteristicsVariant decodeNetworkDeviceReceiverCharacteristicsVariant(const std::vector<char>& data);
        DevStudio::NetworkDeviceStruct decodeNetworkDeviceStruct(const std::vector<char>& data);
        DevStudio::NetworkDeviceTransmitterCharacteristicsVariant decodeNetworkDeviceTransmitterCharacteristicsVariant(const std::vector<char>& data);
        DevStudio::NomenclatureEnum::NomenclatureEnum decodeNomenclatureEnum(const std::vector<char>& data);
        DevStudio::NomenclatureVersionEnum::NomenclatureVersionEnum decodeNomenclatureVersionEnum(const std::vector<char>& data);
        DevStudio::ObserveTaskStruct decodeObserveTaskStruct(const std::vector<char>& data);
        DevStudio::ObservedEquipment decodeObservedEquipment(const std::vector<char>& data);
        std::vector< char > decodeOctetArray(const std::vector<char>& data);
        std::vector< char > decodeOctetArray1Plus(const std::vector<char>& data);
        std::vector< char > decodeOctetArray2(const std::vector<char>& data);
        std::vector< char > decodeOctetArray3(const std::vector<char>& data);
        std::vector< char > decodeOctetArray4(const std::vector<char>& data);
        std::vector< char > decodeOctetArray7(const std::vector<char>& data);
        std::vector< char > decodeOctetArray8(const std::vector<char>& data);
        std::vector< char > decodeOctetPadding32Array(const std::vector<char>& data);
        std::vector< char > decodeOctetPadding64Array(const std::vector<char>& data);
        DevStudio::OpacityCodeEnum::OpacityCodeEnum decodeOpacityCodeEnum(const std::vector<char>& data);
        DevStudio::OperateCheckpointTaskStruct decodeOperateCheckpointTaskStruct(const std::vector<char>& data);
        DevStudio::OperateObservationPostTaskStruct decodeOperateObservationPostTaskStruct(const std::vector<char>& data);
        DevStudio::OrientationStruct decodeOrientationStruct(const std::vector<char>& data);
        std::vector< DevStudio::OrientationStruct > decodeOrientationStructLengthlessArray(const std::vector<char>& data);
        DevStudio::OtherActivityTaskStruct decodeOtherActivityTaskStruct(const std::vector<char>& data);
        DevStudio::ParameterTypeEnum::ParameterTypeEnum decodeParameterTypeEnum(const std::vector<char>& data);
        DevStudio::ParameterValueVariantStruct decodeParameterValueVariantStruct(const std::vector<char>& data);
        DevStudio::PartNumberEnum::PartNumberEnum decodePartNumberEnum(const std::vector<char>& data);
        DevStudio::PatrolMoveTypeEnum::PatrolMoveTypeEnum decodePatrolMoveTypeEnum(const std::vector<char>& data);
        DevStudio::PatrolTaskProgress decodePatrolTaskProgress(const std::vector<char>& data);
        DevStudio::PatrolTaskStruct decodePatrolTaskStruct(const std::vector<char>& data);
        DevStudio::PatrolTypeEnum::PatrolTypeEnum decodePatrolTypeEnum(const std::vector<char>& data);
        DevStudio::PerimeterPointStruct decodePerimeterPointStruct(const std::vector<char>& data);
        std::vector< DevStudio::PerimeterPointStruct > decodePerimeterPointStructLengthlessArray(const std::vector<char>& data);
        DevStudio::PhysicalGenericNetworkStruct decodePhysicalGenericNetworkStruct(const std::vector<char>& data);
        DevStudio::PhysicalNetworkDescriptionVariant decodePhysicalNetworkDescriptionVariant(const std::vector<char>& data);
        DevStudio::PhysicalNetworkTypeEnum::PhysicalNetworkTypeEnum decodePhysicalNetworkTypeEnum(const std::vector<char>& data);
        DevStudio::PhysicalUndefinedNetworkStruct decodePhysicalUndefinedNetworkStruct(const std::vector<char>& data);
        DevStudio::PlumeDimensionRateStruct decodePlumeDimensionRateStruct(const std::vector<char>& data);
        DevStudio::PlumeDimensionStruct decodePlumeDimensionStruct(const std::vector<char>& data);
        DevStudio::Point2GeomRecStruct decodePoint2GeomRecStruct(const std::vector<char>& data);
        DevStudio::PrecipitationStruct decodePrecipitationStruct(const std::vector<char>& data);
        DevStudio::PrecipitationTypeEnum::PrecipitationTypeEnum decodePrecipitationTypeEnum(const std::vector<char>& data);
        DevStudio::PropulsionPlantEnum::PropulsionPlantEnum decodePropulsionPlantEnum(const std::vector<char>& data);
        DevStudio::PropulsionSystemDataStruct decodePropulsionSystemDataStruct(const std::vector<char>& data);
        std::vector< DevStudio::PropulsionSystemDataStruct > decodePropulsionSystemDataStructLengthlessArray(const std::vector<char>& data);
        DevStudio::ProtectionEffectivenessStruct decodeProtectionEffectivenessStruct(const std::vector<char>& data);
        DevStudio::PulseModulationTypeEnum::PulseModulationTypeEnum decodePulseModulationTypeEnum(const std::vector<char>& data);
        DevStudio::PumpFloodingTaskStruct decodePumpFloodingTaskStruct(const std::vector<char>& data);
        DevStudio::RFModulationSystemTypeEnum::RFModulationSystemTypeEnum decodeRFModulationSystemTypeEnum(const std::vector<char>& data);
        DevStudio::RFModulationTypeVariantStruct decodeRFModulationTypeVariantStruct(const std::vector<char>& data);
        std::vector< std::string > decodeRTIobjectIdArray(const std::vector<char>& data);
        DevStudio::RadioInputSourceEnum::RadioInputSourceEnum decodeRadioInputSourceEnum(const std::vector<char>& data);
        DevStudio::RadioTypeStruct decodeRadioTypeStruct(const std::vector<char>& data);
        DevStudio::ReceiverOperationalStatusEnum::ReceiverOperationalStatusEnum decodeReceiverOperationalStatusEnum(const std::vector<char>& data);
        DevStudio::RecordSetStruct decodeRecordSetStruct(const std::vector<char>& data);
        std::vector< DevStudio::RecordSetStruct > decodeRecordSetStructArray1Plus(const std::vector<char>& data);
        DevStudio::RecordStruct decodeRecordStruct(const std::vector<char>& data);
        std::vector< DevStudio::RecordStruct > decodeRecordStructArray(const std::vector<char>& data);
        DevStudio::RectVol1GeomRecStruct decodeRectVol1GeomRecStruct(const std::vector<char>& data);
        DevStudio::RectVol2GeomRecStruct decodeRectVol2GeomRecStruct(const std::vector<char>& data);
        DevStudio::RectVol3GeomRecStruct decodeRectVol3GeomRecStruct(const std::vector<char>& data);
        DevStudio::ReferenceSystemEnum::ReferenceSystemEnum decodeReferenceSystemEnum(const std::vector<char>& data);
        DevStudio::RefuelingStationLightColorEnum::RefuelingStationLightColorEnum decodeRefuelingStationLightColorEnum(const std::vector<char>& data);
        DevStudio::RefuelingStationLightDayNightEnum::RefuelingStationLightDayNightEnum decodeRefuelingStationLightDayNightEnum(const std::vector<char>& data);
        DevStudio::Relation decodeRelation(const std::vector<char>& data);
        DevStudio::RelativePositionStruct decodeRelativePositionStruct(const std::vector<char>& data);
        DevStudio::RelativeRangeBearingStruct decodeRelativeRangeBearingStruct(const std::vector<char>& data);
        DevStudio::ReleaseDistributionStruct decodeReleaseDistributionStruct(const std::vector<char>& data);
        DevStudio::ReleaseDynamicsStruct decodeReleaseDynamicsStruct(const std::vector<char>& data);
        DevStudio::ReleaseSizeStruct decodeReleaseSizeStruct(const std::vector<char>& data);
        DevStudio::RepairResultEnum::RepairResultEnum decodeRepairResultEnum(const std::vector<char>& data);
        DevStudio::RepairTaskStruct decodeRepairTaskStruct(const std::vector<char>& data);
        DevStudio::RepairTypeEnum::RepairTypeEnum decodeRepairTypeEnum(const std::vector<char>& data);
        DevStudio::RequestStatusEnum::RequestStatusEnum decodeRequestStatusEnum(const std::vector<char>& data);
        DevStudio::RequestedConnection decodeRequestedConnection(const std::vector<char>& data);
        std::vector< DevStudio::RequestedConnection > decodeRequestedConnectionArray(const std::vector<char>& data);
        DevStudio::ResourceStatusStruct decodeResourceStatusStruct(const std::vector<char>& data);
        DevStudio::ResponseFlagEnum::ResponseFlagEnum decodeResponseFlagEnum(const std::vector<char>& data);
        DevStudio::ResupplyTaskStruct decodeResupplyTaskStruct(const std::vector<char>& data);
        DevStudio::RoadIceConditionEnum::RoadIceConditionEnum decodeRoadIceConditionEnum(const std::vector<char>& data);
        DevStudio::RoundStruct decodeRoundStruct(const std::vector<char>& data);
        DevStudio::RulesOfEngagementEnum::RulesOfEngagementEnum decodeRulesOfEngagementEnum(const std::vector<char>& data);
        DevStudio::SINCGARSModulationStruct decodeSINCGARSModulationStruct(const std::vector<char>& data);
        DevStudio::SeaStateEnum::SeaStateEnum decodeSeaStateEnum(const std::vector<char>& data);
        DevStudio::SedimentTypeEnum::SedimentTypeEnum decodeSedimentTypeEnum(const std::vector<char>& data);
        DevStudio::SendSafetyRelatedBroadcastMessageTaskStruct decodeSendSafetyRelatedBroadcastMessageTaskStruct(const std::vector<char>& data);
        DevStudio::SendSafetyRelatedMessageTaskStruct decodeSendSafetyRelatedMessageTaskStruct(const std::vector<char>& data);
        DevStudio::SensorStateEnum::SensorStateEnum decodeSensorStateEnum(const std::vector<char>& data);
        DevStudio::SensorStruct decodeSensorStruct(const std::vector<char>& data);
        DevStudio::ServiceTypeEnum::ServiceTypeEnum decodeServiceTypeEnum(const std::vector<char>& data);
        DevStudio::SetTransmitterStatusTaskStruct decodeSetTransmitterStatusTaskStruct(const std::vector<char>& data);
        DevStudio::ShaftDataStruct decodeShaftDataStruct(const std::vector<char>& data);
        std::vector< DevStudio::ShaftDataStruct > decodeShaftDataStructLengthlessArray1Plus(const std::vector<char>& data);
        DevStudio::ShipTypeEnum::ShipTypeEnum decodeShipTypeEnum(const std::vector<char>& data);
        std::vector< char > decodeSignalDataLengthlessArray1Plus(const std::vector<char>& data);
        DevStudio::SignatureTypeEnum::SignatureTypeEnum decodeSignatureTypeEnum(const std::vector<char>& data);
        DevStudio::SignatureVariant decodeSignatureVariant(const std::vector<char>& data);
        DevStudio::SilentAggregateStruct decodeSilentAggregateStruct(const std::vector<char>& data);
        std::vector< DevStudio::SilentAggregateStruct > decodeSilentAggregateStructLengthlessArray(const std::vector<char>& data);
        DevStudio::SilentEntityStruct decodeSilentEntityStruct(const std::vector<char>& data);
        std::vector< DevStudio::SilentEntityStruct > decodeSilentEntityStructLengthlessArray(const std::vector<char>& data);
        DevStudio::SnowStruct decodeSnowStruct(const std::vector<char>& data);
        DevStudio::SpatialFPStruct decodeSpatialFPStruct(const std::vector<char>& data);
        DevStudio::SpatialFVStruct decodeSpatialFVStruct(const std::vector<char>& data);
        DevStudio::SpatialRPStruct decodeSpatialRPStruct(const std::vector<char>& data);
        DevStudio::SpatialRVStruct decodeSpatialRVStruct(const std::vector<char>& data);
        DevStudio::SpatialStaticStruct decodeSpatialStaticStruct(const std::vector<char>& data);
        DevStudio::SpatialVariantStruct decodeSpatialVariantStruct(const std::vector<char>& data);
        DevStudio::Sphere1GeomRecStruct decodeSphere1GeomRecStruct(const std::vector<char>& data);
        DevStudio::Sphere2GeomRecStruct decodeSphere2GeomRecStruct(const std::vector<char>& data);
        DevStudio::SphericalHarmonicAntennaStruct decodeSphericalHarmonicAntennaStruct(const std::vector<char>& data);
        DevStudio::SpreadSpectrumEnum::SpreadSpectrumEnum decodeSpreadSpectrumEnum(const std::vector<char>& data);
        DevStudio::SpreadSpectrumVariantStruct decodeSpreadSpectrumVariantStruct(const std::vector<char>& data);
        DevStudio::StanceCodeEnum::StanceCodeEnum decodeStanceCodeEnum(const std::vector<char>& data);
        DevStudio::StationEnum::StationEnum decodeStationEnum(const std::vector<char>& data);
        DevStudio::StationNameLocationVariantStruct decodeStationNameLocationVariantStruct(const std::vector<char>& data);
        DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum decodeStopFreezeReasonEnum(const std::vector<char>& data);
        DevStudio::SubsurfaceFormationTypeEnum::SubsurfaceFormationTypeEnum decodeSubsurfaceFormationTypeEnum(const std::vector<char>& data);
        DevStudio::SupplyStatusStruct decodeSupplyStatusStruct(const std::vector<char>& data);
        DevStudio::SupplyStruct decodeSupplyStruct(const std::vector<char>& data);
        std::vector< DevStudio::SupplyStruct > decodeSupplyStructLengthlessArray(const std::vector<char>& data);
        DevStudio::SurfaceFormationTypeEnum::SurfaceFormationTypeEnum decodeSurfaceFormationTypeEnum(const std::vector<char>& data);
        DevStudio::SurfaceMoistureEnum::SurfaceMoistureEnum decodeSurfaceMoistureEnum(const std::vector<char>& data);
        DevStudio::SymbolAmplificationVariant decodeSymbolAmplificationVariant(const std::vector<char>& data);
        DevStudio::SymbolStruct decodeSymbolStruct(const std::vector<char>& data);
        DevStudio::SymbolTypeEnum::SymbolTypeEnum decodeSymbolTypeEnum(const std::vector<char>& data);
        DevStudio::TacticalDataLinkTypeEnum::TacticalDataLinkTypeEnum decodeTacticalDataLinkTypeEnum(const std::vector<char>& data);
        DevStudio::TaskDefinition decodeTaskDefinition(const std::vector<char>& data);
        DevStudio::TaskDefinitionVariantRecord decodeTaskDefinitionVariantRecord(const std::vector<char>& data);
        DevStudio::TaskModeEnum::TaskModeEnum decodeTaskModeEnum(const std::vector<char>& data);
        DevStudio::TaskProgress decodeTaskProgress(const std::vector<char>& data);
        DevStudio::TaskProgressVariantRecord decodeTaskProgressVariantRecord(const std::vector<char>& data);
        DevStudio::TaskStatusEnum::TaskStatusEnum decodeTaskStatusEnum(const std::vector<char>& data);
        std::vector< float > decodeTemperatureDegreeCelsiusFloat32LengthlessArray(const std::vector<char>& data);
        DevStudio::TrackStruct decodeTrackStruct(const std::vector<char>& data);
        DevStudio::TrailingEffectsCodeEnum::TrailingEffectsCodeEnum decodeTrailingEffectsCodeEnum(const std::vector<char>& data);
        DevStudio::TransferTypeEnum::TransferTypeEnum decodeTransferTypeEnum(const std::vector<char>& data);
        DevStudio::TransmitterOperationalStatusEnum::TransmitterOperationalStatusEnum decodeTransmitterOperationalStatusEnum(const std::vector<char>& data);
        DevStudio::TransportTaskStruct decodeTransportTaskStruct(const std::vector<char>& data);
        DevStudio::TreatmentStruct decodeTreatmentStruct(const std::vector<char>& data);
        std::vector< char > decodeUUID(const std::vector<char>& data);
        DevStudio::UniformGeomRecStruct decodeUniformGeomRecStruct(const std::vector<char>& data);
        DevStudio::UnitSymbolAmplificationStruct decodeUnitSymbolAmplificationStruct(const std::vector<char>& data);
        DevStudio::UnmodulatedTypeEnum::UnmodulatedTypeEnum decodeUnmodulatedTypeEnum(const std::vector<char>& data);
        std::vector< unsigned short > decodeUnsignedInteger16Array1Plus(const std::vector<char>& data);
        std::vector< unsigned int > decodeUnsignedInteger32LengthlessArray(const std::vector<char>& data);
        std::vector< unsigned long long > decodeUnsignedInteger64Array1Plus(const std::vector<char>& data);
        std::vector< char > decodeUnsignedInteger8LengthlessArray(const std::vector<char>& data);
        DevStudio::UserProtocolEnum::UserProtocolEnum decodeUserProtocolEnum(const std::vector<char>& data);
        DevStudio::VariableDatumStruct decodeVariableDatumStruct(const std::vector<char>& data);
        std::vector< DevStudio::VariableDatumStruct > decodeVariableDatumStructArray(const std::vector<char>& data);
        std::vector< DevStudio::VariableDatumStruct > decodeVariableDatumStructLengthlessArray(const std::vector<char>& data);
        DevStudio::VectoringNozzleSystemDataStruct decodeVectoringNozzleSystemDataStruct(const std::vector<char>& data);
        std::vector< DevStudio::VectoringNozzleSystemDataStruct > decodeVectoringNozzleSystemDataStructLengthlessArray(const std::vector<char>& data);
        DevStudio::VelocityVectorStruct decodeVelocityVectorStruct(const std::vector<char>& data);
        DevStudio::VisibleSideLocationEnum::VisibleSideLocationEnum decodeVisibleSideLocationEnum(const std::vector<char>& data);
        DevStudio::WarheadTypeEnum::WarheadTypeEnum decodeWarheadTypeEnum(const std::vector<char>& data);
        DevStudio::WaveStruct decodeWaveStruct(const std::vector<char>& data);
        DevStudio::Waypoint decodeWaypoint(const std::vector<char>& data);
        DevStudio::WeaponControlOrderEnum::WeaponControlOrderEnum decodeWeaponControlOrderEnum(const std::vector<char>& data);
        DevStudio::WeaponStateEnum::WeaponStateEnum decodeWeaponStateEnum(const std::vector<char>& data);
        DevStudio::WindStruct decodeWindStruct(const std::vector<char>& data);
        DevStudio::WorldLocationStruct decodeWorldLocationStruct(const std::vector<char>& data);
        std::vector< DevStudio::WorldLocationStruct > decodeWorldLocationStructLengthlessArray(const std::vector<char>& data);
// === AUTO-GENERATED FUNCTIONS END ===
//pragma endregion

        bool decodebool(const std::vector<char>& data);
		char decodechar(const std::vector<char>& data);
		short decodeshort(const std::vector<char>& data);
		unsigned short decodeunsignedshort(const std::vector<char>& data);
		int decodeint(const std::vector<char>& data);
		unsigned int decodeunsignedint(const std::vector<char>& data);
		long decodelong(const std::vector<char>& data);
        unsigned long decodeunsignedlong(const std::vector<char>& data);
		long long decodelonglong(const std::vector<char>& data);
		unsigned long long decodeunsignedlonglong(const std::vector<char>& data); 
		float decodefloat(const std::vector<char>& data);
		double decodedouble(const std::vector<char>& data);
        std::string decodestring(const std::vector<char>& data);
		std::wstring decodewstring(const std::vector<char>& data);
		
    };
}

#endif
