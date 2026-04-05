#ifndef DEVELOPER_STUDIO_ENCODERS_H
#define DEVELOPER_STUDIO_ENCODERS_H

#include <mutex>

#include <RtiDriver/Encoding/BasicDataElements.h>
#include <DevStudio/HlaLibSettings.h>
#include <RtiDriver/Encoding/AbstractDataElement.h>
#include <RtiDriver/Encoding/HLAfixedRecord.h>

#include <DevStudio/HlaAllHeaders.h>

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
    class Encoders
    {
    private:
		mutable std::mutex _encodersLock;

//pragma region Auto-generated Variables
// === AUTO-GENERATED MEMBERS BEGIN ===
        ATP45HazardAreaTypeEnumEncoder _aTP45HazardAreaTypeEnumEncoder;
        AccelerationVectorStructEncoder _accelerationVectorStructEncoder;
        AcknowledgeFlagEnumEncoder _acknowledgeFlagEnumEncoder;
        AcknowledgementProtocolEnumEncoder _acknowledgementProtocolEnumEncoder;
        ActionEnumEncoder _actionEnumEncoder;
        ActionResultEnumEncoder _actionResultEnumEncoder;
        ActiveSonarEnumEncoder _activeSonarEnumEncoder;
        ActiveSonarFunctionCodeEnumEncoder _activeSonarFunctionCodeEnumEncoder;
        ActiveSonarScanPatternEnumEncoder _activeSonarScanPatternEnumEncoder;
        ActiveStatusEnumEncoder _activeStatusEnumEncoder;
        AdministerTreatmentTaskStructEncoder _administerTreatmentTaskStructEncoder;
        AgentClassEnumEncoder _agentClassEnumEncoder;
        AgentConcentrationStructEncoder _agentConcentrationStructEncoder;
        AgentMassStructEncoder _agentMassStructEncoder;
        AgentTypeEnumEncoder _agentTypeEnumEncoder;
        AggregateMarkingStructEncoder _aggregateMarkingStructEncoder;
        AggregateMissionEnumEncoder _aggregateMissionEnumEncoder;
        AggregateStateEnumEncoder _aggregateStateEnumEncoder;
        AggregateStateFormationEnumEncoder _aggregateStateFormationEnumEncoder;
        AidTypeEnumEncoder _aidTypeEnumEncoder;
        AirFormationTypeEnumEncoder _airFormationTypeEnumEncoder;
        AmplitudeAngleModulationTypeEnumEncoder _amplitudeAngleModulationTypeEnumEncoder;
        AmplitudeModulationTypeEnumEncoder _amplitudeModulationTypeEnumEncoder;
        AngleModulationTypeEnumEncoder _angleModulationTypeEnumEncoder;
        AngularVelocityVectorStructEncoder _angularVelocityVectorStructEncoder;
        AntennaPatternTypeEnumEncoder _antennaPatternTypeEnumEncoder;
        AntennaPatternVariantStructEncoder _antennaPatternVariantStructEncoder;
        AntennaPatternVariantStructLengthlessArrayEncoder _antennaPatternVariantStructLengthlessArrayEncoder;
        AntiCollisionLightFlashingEnumEncoder _antiCollisionLightFlashingEnumEncoder;
        AppearanceAntiCollisionDayNightEnumEncoder _appearanceAntiCollisionDayNightEnumEncoder;
        AppearanceNVGModeEnumEncoder _appearanceNVGModeEnumEncoder;
        ApplyIPETaskStructEncoder _applyIPETaskStructEncoder;
        AppointmentStructEncoder _appointmentStructEncoder;
        ArrayOfAgentConcentrationStructEncoder _arrayOfAgentConcentrationStructEncoder;
        ArrayOfAgentMassStructEncoder _arrayOfAgentMassStructEncoder;
        ArrayOfAgentTypeEnumEncoder _arrayOfAgentTypeEnumEncoder;
        ArrayOfCBRNExposureStructEncoder _arrayOfCBRNExposureStructEncoder;
        ArrayOfCBRNSensorReadingStructEncoder _arrayOfCBRNSensorReadingStructEncoder;
        ArrayOfCommunicationNetworksEncoder _arrayOfCommunicationNetworksEncoder;
        ArrayOfContourStructEncoder _arrayOfContourStructEncoder;
        ArrayOfDetectedEquipmentEncoder _arrayOfDetectedEquipmentEncoder;
        ArrayOfHoldingsEncoder _arrayOfHoldingsEncoder;
        ArrayOfObservedEquipmentEncoder _arrayOfObservedEquipmentEncoder;
        ArrayOfProtectionEffectivenessStructEncoder _arrayOfProtectionEffectivenessStructEncoder;
        ArrayOfRelationshipEncoder _arrayOfRelationshipEncoder;
        ArrayOfResourceStatusEncoder _arrayOfResourceStatusEncoder;
        ArrayOfSensorEncoder _arrayOfSensorEncoder;
        ArrayOfSigmas6Encoder _arrayOfSigmas6Encoder;
        ArrayOfSignatureEncoder _arrayOfSignatureEncoder;
        ArrayOfSupplyStatusEncoder _arrayOfSupplyStatusEncoder;
        ArrayOfTaskDefinitionsEncoder _arrayOfTaskDefinitionsEncoder;
        ArrayOfTaskProgressEncoder _arrayOfTaskProgressEncoder;
        ArrayOfTrackEncoder _arrayOfTrackEncoder;
        ArrayOfTreatmentStructEncoder _arrayOfTreatmentStructEncoder;
        ArrayOfUuidEncoder _arrayOfUuidEncoder;
        ArrayOfWaypointsEncoder _arrayOfWaypointsEncoder;
        ArticulatedParameterStructEncoder _articulatedParameterStructEncoder;
        ArticulatedParameterStructLengthlessArrayEncoder _articulatedParameterStructLengthlessArrayEncoder;
        ArticulatedPartsStructEncoder _articulatedPartsStructEncoder;
        ArticulatedPartsTypeEnumEncoder _articulatedPartsTypeEnumEncoder;
        ArticulatedTypeMetricEnumEncoder _articulatedTypeMetricEnumEncoder;
        AttachTaskStructEncoder _attachTaskStructEncoder;
        AttachedPartsStructEncoder _attachedPartsStructEncoder;
        AttackTypeEnumEncoder _attackTypeEnumEncoder;
        AttributeValuePairStructArray1PlusEncoder _attributeValuePairStructArray1PlusEncoder;
        AttributeValuePairStructEncoder _attributeValuePairStructEncoder;
        AudioDataTypeStructEncoder _audioDataTypeStructEncoder;
        BaseEncoder _baseEncoder;
        BeamAntennaStructEncoder _beamAntennaStructEncoder;
        BeamFunctionCodeEnumEncoder _beamFunctionCodeEnumEncoder;
        BinArrayEncoder _binArrayEncoder;
        BreachStructEncoder _breachStructEncoder;
        BreachStructLengthlessArrayEncoder _breachStructLengthlessArrayEncoder;
        BreachableSegmentStructEncoder _breachableSegmentStructEncoder;
        BreachableSegmentStructLengthlessArrayEncoder _breachableSegmentStructLengthlessArrayEncoder;
        BreachedStatusArray8Encoder _breachedStatusArray8Encoder;
        BreachedStatusEnumEncoder _breachedStatusEnumEncoder;
        CBRNAlarmStructEncoder _cBRNAlarmStructEncoder;
        CBRNDamageEnumEncoder _cBRNDamageEnumEncoder;
        CBRNExposureStructEncoder _cBRNExposureStructEncoder;
        CBRNSensorReadingStructEncoder _cBRNSensorReadingStructEncoder;
        COMBICStateRecStructEncoder _cOMBICStateRecStructEncoder;
        CamouflageEnumEncoder _camouflageEnumEncoder;
        CaptureStatusEnumEncoder _captureStatusEnumEncoder;
        ChangeAltitudeTaskStructEncoder _changeAltitudeTaskStructEncoder;
        ChangeHeadingTaskStructEncoder _changeHeadingTaskStructEncoder;
        ChangeRulesOfEngagementTaskStructEncoder _changeRulesOfEngagementTaskStructEncoder;
        ChangeSpeedTaskStructEncoder _changeSpeedTaskStructEncoder;
        ChemicalContentEnumEncoder _chemicalContentEnumEncoder;
        ClearEngineeringTaskStructEncoder _clearEngineeringTaskStructEncoder;
        ClockTimeStructEncoder _clockTimeStructEncoder;
        ClockTimeStructLengthlessArrayEncoder _clockTimeStructLengthlessArrayEncoder;
        CloudStructEncoder _cloudStructEncoder;
        CloudTypeEnumEncoder _cloudTypeEnumEncoder;
        CoefficientsLengthlessArray1PlusEncoder _coefficientsLengthlessArray1PlusEncoder;
        CollisionTypeEnumEncoder _collisionTypeEnumEncoder;
        CombinationModulationTypeEnumEncoder _combinationModulationTypeEnumEncoder;
        CommunicationNetworkArrayEncoder _communicationNetworkArrayEncoder;
        CommunicationNetworkTypeEnumEncoder _communicationNetworkTypeEnumEncoder;
        CommunicationServiceTypeEnumEncoder _communicationServiceTypeEnumEncoder;
        ComplianceStateEnumEncoder _complianceStateEnumEncoder;
        Cone1GeomRecStructEncoder _cone1GeomRecStructEncoder;
        Cone2GeomRecStructEncoder _cone2GeomRecStructEncoder;
        ConnectionReceiverArrayEncoder _connectionReceiverArrayEncoder;
        ConnectionReceiverStructEncoder _connectionReceiverStructEncoder;
        ConnectionTypeEnumEncoder _connectionTypeEnumEncoder;
        ConstituentPartNatureEnumEncoder _constituentPartNatureEnumEncoder;
        ConstituentPartPositionEnumEncoder _constituentPartPositionEnumEncoder;
        ConstituentPartRelationshipStructEncoder _constituentPartRelationshipStructEncoder;
        ConstituentPartStationNameEnumEncoder _constituentPartStationNameEnumEncoder;
        ContourStructEncoder _contourStructEncoder;
        CreateBreachTaskStructEncoder _createBreachTaskStructEncoder;
        CreateFireBreakTaskStructEncoder _createFireBreakTaskStructEncoder;
        CreateLeveeTaskStructEncoder _createLeveeTaskStructEncoder;
        CreateObstacleTaskStructEncoder _createObstacleTaskStructEncoder;
        CryptographicModeEnumEncoder _cryptographicModeEnumEncoder;
        CryptographicSystemTypeEnumEncoder _cryptographicSystemTypeEnumEncoder;
        CurrentStructEncoder _currentStructEncoder;
        DamageStatusEnhancedEnumEncoder _damageStatusEnhancedEnumEncoder;
        DamageStatusEnumEncoder _damageStatusEnumEncoder;
        DatumIdentifierEnumEncoder _datumIdentifierEnumEncoder;
        DatumIdentifierLengthlessArrayEncoder _datumIdentifierLengthlessArrayEncoder;
        DeadReckoningAlgorithmEnumEncoder _deadReckoningAlgorithmEnumEncoder;
        DepthMeterFloat32LengthlessArrayEncoder _depthMeterFloat32LengthlessArrayEncoder;
        DesignatorCodeEnumEncoder _designatorCodeEnumEncoder;
        DesignatorCodeNameEnumEncoder _designatorCodeNameEnumEncoder;
        DetectedEquipmentEncoder _detectedEquipmentEncoder;
        DetonationResultCodeEnumEncoder _detonationResultCodeEnumEncoder;
        DimensionRateStructEncoder _dimensionRateStructEncoder;
        DimensionStructEncoder _dimensionStructEncoder;
        DirectFireTaskStructEncoder _directFireTaskStructEncoder;
        DisruptCommunicationTaskStructEncoder _disruptCommunicationTaskStructEncoder;
        EchelonEnumEncoder _echelonEnumEncoder;
        ElapsedTimeProgressEncoder _elapsedTimeProgressEncoder;
        Ellipsoid1GeomRecStructEncoder _ellipsoid1GeomRecStructEncoder;
        Ellipsoid2GeomRecStructEncoder _ellipsoid2GeomRecStructEncoder;
        EmitterFunctionEnumEncoder _emitterFunctionEnumEncoder;
        EmitterTypeEnumEncoder _emitterTypeEnumEncoder;
        EmptyTaskStructEncoder _emptyTaskStructEncoder;
        EncodingTypeEnumEncoder _encodingTypeEnumEncoder;
        EngineeringTaskStructEncoder _engineeringTaskStructEncoder;
        EntityControlActionEnumEncoder _entityControlActionEnumEncoder;
        EntityControlActionsEncoder _entityControlActionsEncoder;
        EntityCoordinateVectorStructEncoder _entityCoordinateVectorStructEncoder;
        EntityIdentifierStructEncoder _entityIdentifierStructEncoder;
        EntityTypeStructEncoder _entityTypeStructEncoder;
        EntityTypeStructLengthlessArrayEncoder _entityTypeStructLengthlessArrayEncoder;
        EnvironmentConditionModelTypeEnumEncoder _environmentConditionModelTypeEnumEncoder;
        EnvironmentDataCoordinateSystemEnumEncoder _environmentDataCoordinateSystemEnumEncoder;
        EnvironmentDataRepresentationEnumEncoder _environmentDataRepresentationEnumEncoder;
        EnvironmentDataSampleTypeEnumEncoder _environmentDataSampleTypeEnumEncoder;
        EnvironmentGridAxisTypeEnumEncoder _environmentGridAxisTypeEnumEncoder;
        EnvironmentGridTypeEnumEncoder _environmentGridTypeEnumEncoder;
        EnvironmentModelTypeEnumEncoder _environmentModelTypeEnumEncoder;
        EnvironmentObjectTypeStructEncoder _environmentObjectTypeStructEncoder;
        EnvironmentRecStructArrayEncoder _environmentRecStructArrayEncoder;
        EnvironmentRecStructEncoder _environmentRecStructEncoder;
        EnvironmentRecVariantStructEncoder _environmentRecVariantStructEncoder;
        EnvironmentRecordTypeEnumEncoder _environmentRecordTypeEnumEncoder;
        EnvironmentTypeStructEncoder _environmentTypeStructEncoder;
        EquipmentSymbolAmplificationStructEncoder _equipmentSymbolAmplificationStructEncoder;
        EstablishCheckpointTaskStructEncoder _establishCheckpointTaskStructEncoder;
        EstablishObservationPostTaskStructEncoder _establishObservationPostTaskStructEncoder;
        EvacuateProgressStructEncoder _evacuateProgressStructEncoder;
        EvacuateTaskStructEncoder _evacuateTaskStructEncoder;
        EventIdentifierStructEncoder _eventIdentifierStructEncoder;
        EventTypeEnumEncoder _eventTypeEnumEncoder;
        ExhaustSmokeStructEncoder _exhaustSmokeStructEncoder;
        ExhaustSmokeStructLengthlessArrayEncoder _exhaustSmokeStructLengthlessArrayEncoder;
        ExposureTypeEnumEncoder _exposureTypeEnumEncoder;
        ExtinguishWildfireTaskStructEncoder _extinguishWildfireTaskStructEncoder;
        FederateControlActionEnumEncoder _federateControlActionEnumEncoder;
        FederateControlActionsEncoder _federateControlActionsEncoder;
        FederateIdentifierStructEncoder _federateIdentifierStructEncoder;
        FireTaskProgressEncoder _fireTaskProgressEncoder;
        FixedDatumStructEncoder _fixedDatumStructEncoder;
        FixedDatumStructLengthlessArrayEncoder _fixedDatumStructLengthlessArrayEncoder;
        FlareStateRecStructEncoder _flareStateRecStructEncoder;
        Float32Array1PlusEncoder _float32Array1PlusEncoder;
        FollowEntityTaskStructEncoder _followEntityTaskStructEncoder;
        ForceIdentifierEnumEncoder _forceIdentifierEnumEncoder;
        FormationDataStructEncoder _formationDataStructEncoder;
        FormationEnumEncoder _formationEnumEncoder;
        FormationLocationTypeEnumEncoder _formationLocationTypeEnumEncoder;
        FormationStructEncoder _formationStructEncoder;
        FormationTypeEnumEncoder _formationTypeEnumEncoder;
        FundamentalParameterDataStructEncoder _fundamentalParameterDataStructEncoder;
        FundamentalParameterDataStructLengthlessArrayEncoder _fundamentalParameterDataStructLengthlessArrayEncoder;
        FuseTypeEnumEncoder _fuseTypeEnumEncoder;
        GaussPlumeGeomRecStructEncoder _gaussPlumeGeomRecStructEncoder;
        GaussPuffGeomRecStructEncoder _gaussPuffGeomRecStructEncoder;
        GeoLocationTypeEnumEncoder _geoLocationTypeEnumEncoder;
        GeoReferenceVariantEncoder _geoReferenceVariantEncoder;
        GeodeticCircleEncoder _geodeticCircleEncoder;
        GeodeticLocationEncoder _geodeticLocationEncoder;
        GeodeticPathEncoder _geodeticPathEncoder;
        GeodeticPointEncoder _geodeticPointEncoder;
        GeodeticPolygonEncoder _geodeticPolygonEncoder;
        GeodeticQuadrangleEncoder _geodeticQuadrangleEncoder;
        GridAxisStructEncoder _gridAxisStructEncoder;
        GridAxisStructLengthlessArrayEncoder _gridAxisStructLengthlessArrayEncoder;
        GridAxisTypeVariantStructEncoder _gridAxisTypeVariantStructEncoder;
        GridDataRepresentationVariantStructEncoder _gridDataRepresentationVariantStructEncoder;
        GridDataStructEncoder _gridDataStructEncoder;
        GridDataStructLengthlessArrayEncoder _gridDataStructLengthlessArrayEncoder;
        GridValueType0StructEncoder _gridValueType0StructEncoder;
        GridValueType1StructEncoder _gridValueType1StructEncoder;
        GridValueType2StructEncoder _gridValueType2StructEncoder;
        GroundFormationTypeEnumEncoder _groundFormationTypeEnumEncoder;
        HLAargumentListEncoder _hLAargumentListEncoder;
        HLAfederateStateEncoder _hLAfederateStateEncoder;
        HLAhandleListEncoder _hLAhandleListEncoder;
        HLAinteractionCountEncoder _hLAinteractionCountEncoder;
        HLAinteractionCountsEncoder _hLAinteractionCountsEncoder;
        HLAinteractionSubListEncoder _hLAinteractionSubListEncoder;
        HLAinteractionSubscriptionEncoder _hLAinteractionSubscriptionEncoder;
        HLAmoduleDesignatorListEncoder _hLAmoduleDesignatorListEncoder;
        HLAobjectClassBasedCountEncoder _hLAobjectClassBasedCountEncoder;
        HLAobjectClassBasedCountsEncoder _hLAobjectClassBasedCountsEncoder;
        HLAorderTypeEncoder _hLAorderTypeEncoder;
        HLAownershipEncoder _hLAownershipEncoder;
        HLAresignActionEncoder _hLAresignActionEncoder;
        HLAswitchEncoder _hLAswitchEncoder;
        HLAsynchPointFederateEncoder _hLAsynchPointFederateEncoder;
        HLAsynchPointFederateListEncoder _hLAsynchPointFederateListEncoder;
        HLAsynchPointListEncoder _hLAsynchPointListEncoder;
        HLAsynchPointStatusEncoder _hLAsynchPointStatusEncoder;
        HLAtimeStateEncoder _hLAtimeStateEncoder;
        HatchStateEnumEncoder _hatchStateEnumEncoder;
        HazardTypeEnum2Encoder _hazardTypeEnum2Encoder;
        HazardTypeEnumEncoder _hazardTypeEnumEncoder;
        HazeStructEncoder _hazeStructEncoder;
        HazeTypeEnumEncoder _hazeTypeEnumEncoder;
        HoldingEncoder _holdingEncoder;
        HostilityStatusCodeEnumEncoder _hostilityStatusCodeEnumEncoder;
        IPETypeEnumEncoder _iPETypeEnumEncoder;
        IceStructEncoder _iceStructEncoder;
        IceTypeEnumEncoder _iceTypeEnumEncoder;
        IdentificationLevelEnumEncoder _identificationLevelEnumEncoder;
        IffAlternateMode4EnumEncoder _iffAlternateMode4EnumEncoder;
        IffApplicableModesEnumEncoder _iffApplicableModesEnumEncoder;
        IffOperationalParameter1EnumEncoder _iffOperationalParameter1EnumEncoder;
        IffOperationalParameter2EnumEncoder _iffOperationalParameter2EnumEncoder;
        IffSystemModeEnumEncoder _iffSystemModeEnumEncoder;
        IffSystemNameEnumEncoder _iffSystemNameEnumEncoder;
        IffSystemTypeEnumEncoder _iffSystemTypeEnumEncoder;
        IncomingConnectionArrayEncoder _incomingConnectionArrayEncoder;
        IncomingConnectionStructEncoder _incomingConnectionStructEncoder;
        IndirectFireTaskStructEncoder _indirectFireTaskStructEncoder;
        InstallationSymbolAmplificationStructEncoder _installationSymbolAmplificationStructEncoder;
        Integer16Array1PlusEncoder _integer16Array1PlusEncoder;
        IrregularGridAxisStructEncoder _irregularGridAxisStructEncoder;
        IsPartOfStructEncoder _isPartOfStructEncoder;
        LandslideTypeEnumEncoder _landslideTypeEnumEncoder;
        LayMinefieldTaskStructEncoder _layMinefieldTaskStructEncoder;
        LayerStructEncoder _layerStructEncoder;
        LightningTypeEnumEncoder _lightningTypeEnumEncoder;
        Line1GeomRecStructEncoder _line1GeomRecStructEncoder;
        Line2GeomRecStructEncoder _line2GeomRecStructEncoder;
        LinearSegmentStructEncoder _linearSegmentStructEncoder;
        LinkStatusArrayEncoder _linkStatusArrayEncoder;
        LinkStatusStructEncoder _linkStatusStructEncoder;
        LocationStructArrayEncoder _locationStructArrayEncoder;
        LocationStructEncoder _locationStructEncoder;
        MagicMoveTaskStructEncoder _magicMoveTaskStructEncoder;
        MajorRFModulationTypeEnumEncoder _majorRFModulationTypeEnumEncoder;
        ManeuverIndicatorEnumEncoder _maneuverIndicatorEnumEncoder;
        ManufacturerIdArray3Encoder _manufacturerIdArray3Encoder;
        MarkingArray11Encoder _markingArray11Encoder;
        MarkingArray31Encoder _markingArray31Encoder;
        MarkingEncodingEnumEncoder _markingEncodingEnumEncoder;
        MarkingStructEncoder _markingStructEncoder;
        MineDielectricDifferenceLengthlessArrayEncoder _mineDielectricDifferenceLengthlessArrayEncoder;
        MineFusingStructEncoder _mineFusingStructEncoder;
        MineFusingStructLengthlessArrayEncoder _mineFusingStructLengthlessArrayEncoder;
        MineIdentifierLengthlessArrayEncoder _mineIdentifierLengthlessArrayEncoder;
        MinefieldFusingEnumEncoder _minefieldFusingEnumEncoder;
        MinefieldLaneEnumEncoder _minefieldLaneEnumEncoder;
        MinefieldLaneMarkerStructEncoder _minefieldLaneMarkerStructEncoder;
        MinefieldLaneMarkerStructLengthlessArrayEncoder _minefieldLaneMarkerStructLengthlessArrayEncoder;
        MinefieldPaintSchemeEnumEncoder _minefieldPaintSchemeEnumEncoder;
        MinefieldPaintSchemeLengthlessArrayEncoder _minefieldPaintSchemeLengthlessArrayEncoder;
        MinefieldProtocolEnumEncoder _minefieldProtocolEnumEncoder;
        MinefieldSensorTypeEnumEncoder _minefieldSensorTypeEnumEncoder;
        MinefieldSensorTypeLengthlessArrayEncoder _minefieldSensorTypeLengthlessArrayEncoder;
        MinefieldStatusEnumEncoder _minefieldStatusEnumEncoder;
        MinefieldTypeEnumEncoder _minefieldTypeEnumEncoder;
        MissingRecordNumbersLengthlessArray1PlusEncoder _missingRecordNumbersLengthlessArray1PlusEncoder;
        MoveByRouteTaskStructEncoder _moveByRouteTaskStructEncoder;
        MoveInDirectionTaskStructEncoder _moveInDirectionTaskStructEncoder;
        MoveIntoFormationTaskStructEncoder _moveIntoFormationTaskStructEncoder;
        MoveTaskProgressEncoder _moveTaskProgressEncoder;
        MoveToLocationTaskStructEncoder _moveToLocationTaskStructEncoder;
        MoveTypeEnumEncoder _moveTypeEnumEncoder;
        MsgIdEnumEncoder _msgIdEnumEncoder;
        NamedLocationStructEncoder _namedLocationStructEncoder;
        NavigationLightFlashingEnumEncoder _navigationLightFlashingEnumEncoder;
        NavigationStatusEnumEncoder _navigationStatusEnumEncoder;
        NetworkDeviceArrayEncoder _networkDeviceArrayEncoder;
        NetworkDeviceEmptyCharactersticsStructEncoder _networkDeviceEmptyCharactersticsStructEncoder;
        NetworkDeviceGenericTransmitterCharacteristicsStructEncoder _networkDeviceGenericTransmitterCharacteristicsStructEncoder;
        NetworkDeviceReceiverCharacteristicsVariantEncoder _networkDeviceReceiverCharacteristicsVariantEncoder;
        NetworkDeviceStructEncoder _networkDeviceStructEncoder;
        NetworkDeviceTransmitterCharacteristicsVariantEncoder _networkDeviceTransmitterCharacteristicsVariantEncoder;
        NomenclatureEnumEncoder _nomenclatureEnumEncoder;
        NomenclatureVersionEnumEncoder _nomenclatureVersionEnumEncoder;
        ObserveTaskStructEncoder _observeTaskStructEncoder;
        ObservedEquipmentEncoder _observedEquipmentEncoder;
        OctetArray1PlusEncoder _octetArray1PlusEncoder;
        OctetArray2Encoder _octetArray2Encoder;
        OctetArray3Encoder _octetArray3Encoder;
        OctetArray4Encoder _octetArray4Encoder;
        OctetArray7Encoder _octetArray7Encoder;
        OctetArray8Encoder _octetArray8Encoder;
        OctetArrayEncoder _octetArrayEncoder;
        OctetPadding32ArrayEncoder _octetPadding32ArrayEncoder;
        OctetPadding64ArrayEncoder _octetPadding64ArrayEncoder;
        OpacityCodeEnumEncoder _opacityCodeEnumEncoder;
        OperateCheckpointTaskStructEncoder _operateCheckpointTaskStructEncoder;
        OperateObservationPostTaskStructEncoder _operateObservationPostTaskStructEncoder;
        OrientationStructEncoder _orientationStructEncoder;
        OrientationStructLengthlessArrayEncoder _orientationStructLengthlessArrayEncoder;
        OtherActivityTaskStructEncoder _otherActivityTaskStructEncoder;
        ParameterTypeEnumEncoder _parameterTypeEnumEncoder;
        ParameterValueVariantStructEncoder _parameterValueVariantStructEncoder;
        PartNumberEnumEncoder _partNumberEnumEncoder;
        PatrolMoveTypeEnumEncoder _patrolMoveTypeEnumEncoder;
        PatrolTaskProgressEncoder _patrolTaskProgressEncoder;
        PatrolTaskStructEncoder _patrolTaskStructEncoder;
        PatrolTypeEnumEncoder _patrolTypeEnumEncoder;
        PerimeterPointStructEncoder _perimeterPointStructEncoder;
        PerimeterPointStructLengthlessArrayEncoder _perimeterPointStructLengthlessArrayEncoder;
        PhysicalGenericNetworkStructEncoder _physicalGenericNetworkStructEncoder;
        PhysicalNetworkDescriptionVariantEncoder _physicalNetworkDescriptionVariantEncoder;
        PhysicalNetworkTypeEnumEncoder _physicalNetworkTypeEnumEncoder;
        PhysicalUndefinedNetworkStructEncoder _physicalUndefinedNetworkStructEncoder;
        PlumeDimensionRateStructEncoder _plumeDimensionRateStructEncoder;
        PlumeDimensionStructEncoder _plumeDimensionStructEncoder;
        Point2GeomRecStructEncoder _point2GeomRecStructEncoder;
        PrecipitationStructEncoder _precipitationStructEncoder;
        PrecipitationTypeEnumEncoder _precipitationTypeEnumEncoder;
        PropulsionPlantEnumEncoder _propulsionPlantEnumEncoder;
        PropulsionSystemDataStructEncoder _propulsionSystemDataStructEncoder;
        PropulsionSystemDataStructLengthlessArrayEncoder _propulsionSystemDataStructLengthlessArrayEncoder;
        ProtectionEffectivenessStructEncoder _protectionEffectivenessStructEncoder;
        PulseModulationTypeEnumEncoder _pulseModulationTypeEnumEncoder;
        PumpFloodingTaskStructEncoder _pumpFloodingTaskStructEncoder;
        RFModulationSystemTypeEnumEncoder _rFModulationSystemTypeEnumEncoder;
        RFModulationTypeVariantStructEncoder _rFModulationTypeVariantStructEncoder;
        RTIobjectIdArrayEncoder _rTIobjectIdArrayEncoder;
        RadioInputSourceEnumEncoder _radioInputSourceEnumEncoder;
        RadioTypeStructEncoder _radioTypeStructEncoder;
        ReceiverOperationalStatusEnumEncoder _receiverOperationalStatusEnumEncoder;
        RecordSetStructArray1PlusEncoder _recordSetStructArray1PlusEncoder;
        RecordSetStructEncoder _recordSetStructEncoder;
        RecordStructArrayEncoder _recordStructArrayEncoder;
        RecordStructEncoder _recordStructEncoder;
        RectVol1GeomRecStructEncoder _rectVol1GeomRecStructEncoder;
        RectVol2GeomRecStructEncoder _rectVol2GeomRecStructEncoder;
        RectVol3GeomRecStructEncoder _rectVol3GeomRecStructEncoder;
        ReferenceSystemEnumEncoder _referenceSystemEnumEncoder;
        RefuelingStationLightColorEnumEncoder _refuelingStationLightColorEnumEncoder;
        RefuelingStationLightDayNightEnumEncoder _refuelingStationLightDayNightEnumEncoder;
        RelationEncoder _relationEncoder;
        RelativePositionStructEncoder _relativePositionStructEncoder;
        RelativeRangeBearingStructEncoder _relativeRangeBearingStructEncoder;
        ReleaseDistributionStructEncoder _releaseDistributionStructEncoder;
        ReleaseDynamicsStructEncoder _releaseDynamicsStructEncoder;
        ReleaseSizeStructEncoder _releaseSizeStructEncoder;
        RepairResultEnumEncoder _repairResultEnumEncoder;
        RepairTaskStructEncoder _repairTaskStructEncoder;
        RepairTypeEnumEncoder _repairTypeEnumEncoder;
        RequestStatusEnumEncoder _requestStatusEnumEncoder;
        RequestedConnectionArrayEncoder _requestedConnectionArrayEncoder;
        RequestedConnectionEncoder _requestedConnectionEncoder;
        ResourceStatusStructEncoder _resourceStatusStructEncoder;
        ResponseFlagEnumEncoder _responseFlagEnumEncoder;
        ResupplyTaskStructEncoder _resupplyTaskStructEncoder;
        RoadIceConditionEnumEncoder _roadIceConditionEnumEncoder;
        RoundStructEncoder _roundStructEncoder;
        RulesOfEngagementEnumEncoder _rulesOfEngagementEnumEncoder;
        SINCGARSModulationStructEncoder _sINCGARSModulationStructEncoder;
        SeaStateEnumEncoder _seaStateEnumEncoder;
        SedimentTypeEnumEncoder _sedimentTypeEnumEncoder;
        SendSafetyRelatedBroadcastMessageTaskStructEncoder _sendSafetyRelatedBroadcastMessageTaskStructEncoder;
        SendSafetyRelatedMessageTaskStructEncoder _sendSafetyRelatedMessageTaskStructEncoder;
        SensorStateEnumEncoder _sensorStateEnumEncoder;
        SensorStructEncoder _sensorStructEncoder;
        ServiceTypeEnumEncoder _serviceTypeEnumEncoder;
        SetTransmitterStatusTaskStructEncoder _setTransmitterStatusTaskStructEncoder;
        ShaftDataStructEncoder _shaftDataStructEncoder;
        ShaftDataStructLengthlessArray1PlusEncoder _shaftDataStructLengthlessArray1PlusEncoder;
        ShipTypeEnumEncoder _shipTypeEnumEncoder;
        SignalDataLengthlessArray1PlusEncoder _signalDataLengthlessArray1PlusEncoder;
        SignatureTypeEnumEncoder _signatureTypeEnumEncoder;
        SignatureVariantEncoder _signatureVariantEncoder;
        SilentAggregateStructEncoder _silentAggregateStructEncoder;
        SilentAggregateStructLengthlessArrayEncoder _silentAggregateStructLengthlessArrayEncoder;
        SilentEntityStructEncoder _silentEntityStructEncoder;
        SilentEntityStructLengthlessArrayEncoder _silentEntityStructLengthlessArrayEncoder;
        SnowStructEncoder _snowStructEncoder;
        SpatialFPStructEncoder _spatialFPStructEncoder;
        SpatialFVStructEncoder _spatialFVStructEncoder;
        SpatialRPStructEncoder _spatialRPStructEncoder;
        SpatialRVStructEncoder _spatialRVStructEncoder;
        SpatialStaticStructEncoder _spatialStaticStructEncoder;
        SpatialVariantStructEncoder _spatialVariantStructEncoder;
        Sphere1GeomRecStructEncoder _sphere1GeomRecStructEncoder;
        Sphere2GeomRecStructEncoder _sphere2GeomRecStructEncoder;
        SphericalHarmonicAntennaStructEncoder _sphericalHarmonicAntennaStructEncoder;
        SpreadSpectrumEnumEncoder _spreadSpectrumEnumEncoder;
        SpreadSpectrumVariantStructEncoder _spreadSpectrumVariantStructEncoder;
        StanceCodeEnumEncoder _stanceCodeEnumEncoder;
        StationEnumEncoder _stationEnumEncoder;
        StationNameLocationVariantStructEncoder _stationNameLocationVariantStructEncoder;
        StopFreezeReasonEnumEncoder _stopFreezeReasonEnumEncoder;
        SubsurfaceFormationTypeEnumEncoder _subsurfaceFormationTypeEnumEncoder;
        SupplyStatusStructEncoder _supplyStatusStructEncoder;
        SupplyStructEncoder _supplyStructEncoder;
        SupplyStructLengthlessArrayEncoder _supplyStructLengthlessArrayEncoder;
        SurfaceFormationTypeEnumEncoder _surfaceFormationTypeEnumEncoder;
        SurfaceMoistureEnumEncoder _surfaceMoistureEnumEncoder;
        SymbolAmplificationVariantEncoder _symbolAmplificationVariantEncoder;
        SymbolStructEncoder _symbolStructEncoder;
        SymbolTypeEnumEncoder _symbolTypeEnumEncoder;
        TacticalDataLinkTypeEnumEncoder _tacticalDataLinkTypeEnumEncoder;
        TaskDefinitionEncoder _taskDefinitionEncoder;
        TaskDefinitionVariantRecordEncoder _taskDefinitionVariantRecordEncoder;
        TaskModeEnumEncoder _taskModeEnumEncoder;
        TaskProgressEncoder _taskProgressEncoder;
        TaskProgressVariantRecordEncoder _taskProgressVariantRecordEncoder;
        TaskStatusEnumEncoder _taskStatusEnumEncoder;
        TemperatureDegreeCelsiusFloat32LengthlessArrayEncoder _temperatureDegreeCelsiusFloat32LengthlessArrayEncoder;
        TrackStructEncoder _trackStructEncoder;
        TrailingEffectsCodeEnumEncoder _trailingEffectsCodeEnumEncoder;
        TransferTypeEnumEncoder _transferTypeEnumEncoder;
        TransmitterOperationalStatusEnumEncoder _transmitterOperationalStatusEnumEncoder;
        TransportTaskStructEncoder _transportTaskStructEncoder;
        TreatmentStructEncoder _treatmentStructEncoder;
        UUIDEncoder _uUIDEncoder;
        UniformGeomRecStructEncoder _uniformGeomRecStructEncoder;
        UnitSymbolAmplificationStructEncoder _unitSymbolAmplificationStructEncoder;
        UnmodulatedTypeEnumEncoder _unmodulatedTypeEnumEncoder;
        UnsignedInteger16Array1PlusEncoder _unsignedInteger16Array1PlusEncoder;
        UnsignedInteger32LengthlessArrayEncoder _unsignedInteger32LengthlessArrayEncoder;
        UnsignedInteger64Array1PlusEncoder _unsignedInteger64Array1PlusEncoder;
        UnsignedInteger8LengthlessArrayEncoder _unsignedInteger8LengthlessArrayEncoder;
        UserProtocolEnumEncoder _userProtocolEnumEncoder;
        VariableDatumStructArrayEncoder _variableDatumStructArrayEncoder;
        VariableDatumStructEncoder _variableDatumStructEncoder;
        VariableDatumStructLengthlessArrayEncoder _variableDatumStructLengthlessArrayEncoder;
        VectoringNozzleSystemDataStructEncoder _vectoringNozzleSystemDataStructEncoder;
        VectoringNozzleSystemDataStructLengthlessArrayEncoder _vectoringNozzleSystemDataStructLengthlessArrayEncoder;
        VelocityVectorStructEncoder _velocityVectorStructEncoder;
        VisibleSideLocationEnumEncoder _visibleSideLocationEnumEncoder;
        WarheadTypeEnumEncoder _warheadTypeEnumEncoder;
        WaveStructEncoder _waveStructEncoder;
        WaypointEncoder _waypointEncoder;
        WeaponControlOrderEnumEncoder _weaponControlOrderEnumEncoder;
        WeaponStateEnumEncoder _weaponStateEnumEncoder;
        WindStructEncoder _windStructEncoder;
        WorldLocationStructEncoder _worldLocationStructEncoder;
        WorldLocationStructLengthlessArrayEncoder _worldLocationStructLengthlessArrayEncoder;
// === AUTO-GENERATED MEMBERS END ===
//pragma endregion

        EncodingHelpers::HLAboolean _boolEncoder;
        EncodingHelpers::HLAoctet _charEncoder;
        EncodingHelpers::HLAinteger16BE _shortEncoder;
        EncodingHelpers::UnsignedShort _unsignedshortEncoder;
		EncodingHelpers::HLAinteger32BE _intEncoder;
		EncodingHelpers::UnsignedInteger32BE _unsignedintEncoder;
		EncodingHelpers::HLAinteger64BE _longEncoder;
		EncodingHelpers::UnsignedInteger64BE _unsignedlongEncoder;
        EncodingHelpers::HLAinteger64BE _longlongEncoder;
        EncodingHelpers::UnsignedInteger64BE _unsignedlonglongEncoder;
		EncodingHelpers::HLAfloat32BE _floatEncoder;
		EncodingHelpers::HLAfloat64BE _doubleEncoder;
		EncodingHelpers::OMT13string _stringEncoder;
		EncodingHelpers::HLAunicodeString _wstringEncoder;

        Encoders();

    public:
        ~Encoders();

		// Singleton instance retrieval
        static Encoders& getInstance()
        {
            static std::unique_ptr<Encoders> theInstance{ new Encoders() };
            return *theInstance;
        }

//pragma region Auto-generated Functions
        // === AUTO-GENERATED FUNCTIONS BEGIN ===
        std::vector<char> encodeATP45HazardAreaTypeEnum(const DevStudio::ATP45HazardAreaTypeEnum::ATP45HazardAreaTypeEnum& aTP45HazardAreaTypeEnum);
        std::vector<char> encodeAccelerationVectorStruct(const DevStudio::AccelerationVectorStruct& accelerationVectorStruct);
        std::vector<char> encodeAcknowledgeFlagEnum(const DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum& acknowledgeFlagEnum);
        std::vector<char> encodeAcknowledgementProtocolEnum(const DevStudio::AcknowledgementProtocolEnum::AcknowledgementProtocolEnum& acknowledgementProtocolEnum);
        std::vector<char> encodeActionEnum(const DevStudio::ActionEnum::ActionEnum& actionEnum);
        std::vector<char> encodeActionResultEnum(const DevStudio::ActionResultEnum::ActionResultEnum& actionResultEnum);
        std::vector<char> encodeActiveSonarEnum(const DevStudio::ActiveSonarEnum::ActiveSonarEnum& activeSonarEnum);
        std::vector<char> encodeActiveSonarFunctionCodeEnum(const DevStudio::ActiveSonarFunctionCodeEnum::ActiveSonarFunctionCodeEnum& activeSonarFunctionCodeEnum);
        std::vector<char> encodeActiveSonarScanPatternEnum(const DevStudio::ActiveSonarScanPatternEnum::ActiveSonarScanPatternEnum& activeSonarScanPatternEnum);
        std::vector<char> encodeActiveStatusEnum(const DevStudio::ActiveStatusEnum::ActiveStatusEnum& activeStatusEnum);
        std::vector<char> encodeAdministerTreatmentTaskStruct(const DevStudio::AdministerTreatmentTaskStruct& administerTreatmentTaskStruct);
        std::vector<char> encodeAgentClassEnum(const DevStudio::AgentClassEnum::AgentClassEnum& agentClassEnum);
        std::vector<char> encodeAgentConcentrationStruct(const DevStudio::AgentConcentrationStruct& agentConcentrationStruct);
        std::vector<char> encodeAgentMassStruct(const DevStudio::AgentMassStruct& agentMassStruct);
        std::vector<char> encodeAgentTypeEnum(const DevStudio::AgentTypeEnum::AgentTypeEnum& agentTypeEnum);
        std::vector<char> encodeAggregateMarkingStruct(const DevStudio::AggregateMarkingStruct& aggregateMarkingStruct);
        std::vector<char> encodeAggregateMissionEnum(const DevStudio::AggregateMissionEnum::AggregateMissionEnum& aggregateMissionEnum);
        std::vector<char> encodeAggregateStateEnum(const DevStudio::AggregateStateEnum::AggregateStateEnum& aggregateStateEnum);
        std::vector<char> encodeAggregateStateFormationEnum(const DevStudio::AggregateStateFormationEnum::AggregateStateFormationEnum& aggregateStateFormationEnum);
        std::vector<char> encodeAidTypeEnum(const DevStudio::AidTypeEnum::AidTypeEnum& aidTypeEnum);
        std::vector<char> encodeAirFormationTypeEnum(const DevStudio::AirFormationTypeEnum::AirFormationTypeEnum& airFormationTypeEnum);
        std::vector<char> encodeAmplitudeAngleModulationTypeEnum(const DevStudio::AmplitudeAngleModulationTypeEnum::AmplitudeAngleModulationTypeEnum& amplitudeAngleModulationTypeEnum);
        std::vector<char> encodeAmplitudeModulationTypeEnum(const DevStudio::AmplitudeModulationTypeEnum::AmplitudeModulationTypeEnum& amplitudeModulationTypeEnum);
        std::vector<char> encodeAngleModulationTypeEnum(const DevStudio::AngleModulationTypeEnum::AngleModulationTypeEnum& angleModulationTypeEnum);
        std::vector<char> encodeAngularVelocityVectorStruct(const DevStudio::AngularVelocityVectorStruct& angularVelocityVectorStruct);
        std::vector<char> encodeAntennaPatternTypeEnum(const DevStudio::AntennaPatternTypeEnum::AntennaPatternTypeEnum& antennaPatternTypeEnum);
        std::vector<char> encodeAntennaPatternVariantStruct(const DevStudio::AntennaPatternVariantStruct& antennaPatternVariantStruct);
        std::vector<char> encodeAntennaPatternVariantStructLengthlessArray(const std::vector< DevStudio::AntennaPatternVariantStruct >& antennaPatternVariantStructLengthlessArray);
        std::vector<char> encodeAntiCollisionLightFlashingEnum(const DevStudio::AntiCollisionLightFlashingEnum::AntiCollisionLightFlashingEnum& antiCollisionLightFlashingEnum);
        std::vector<char> encodeAppearanceAntiCollisionDayNightEnum(const DevStudio::AppearanceAntiCollisionDayNightEnum::AppearanceAntiCollisionDayNightEnum& appearanceAntiCollisionDayNightEnum);
        std::vector<char> encodeAppearanceNVGModeEnum(const DevStudio::AppearanceNVGModeEnum::AppearanceNVGModeEnum& appearanceNVGModeEnum);
        std::vector<char> encodeApplyIPETaskStruct(const DevStudio::ApplyIPETaskStruct& applyIPETaskStruct);
        std::vector<char> encodeAppointmentStruct(const DevStudio::AppointmentStruct& appointmentStruct);
        std::vector<char> encodeArrayOfAgentConcentrationStruct(const std::vector< DevStudio::AgentConcentrationStruct >& arrayOfAgentConcentrationStruct);
        std::vector<char> encodeArrayOfAgentMassStruct(const std::vector< DevStudio::AgentMassStruct >& arrayOfAgentMassStruct);
        std::vector<char> encodeArrayOfAgentTypeEnum(const std::vector< DevStudio::AgentTypeEnum::AgentTypeEnum >& arrayOfAgentTypeEnum);
        std::vector<char> encodeArrayOfCBRNExposureStruct(const std::vector< DevStudio::CBRNExposureStruct >& arrayOfCBRNExposureStruct);
        std::vector<char> encodeArrayOfCBRNSensorReadingStruct(const std::vector< DevStudio::CBRNSensorReadingStruct >& arrayOfCBRNSensorReadingStruct);
        std::vector<char> encodeArrayOfCommunicationNetworks(const std::vector< std::vector< char > >& arrayOfCommunicationNetworks);
        std::vector<char> encodeArrayOfContourStruct(const std::vector< DevStudio::ContourStruct >& arrayOfContourStruct);
        std::vector<char> encodeArrayOfDetectedEquipment(const std::vector< DevStudio::DetectedEquipment >& arrayOfDetectedEquipment);
        std::vector<char> encodeArrayOfHoldings(const std::vector< DevStudio::Holding >& arrayOfHoldings);
        std::vector<char> encodeArrayOfObservedEquipment(const std::vector< DevStudio::ObservedEquipment >& arrayOfObservedEquipment);
        std::vector<char> encodeArrayOfProtectionEffectivenessStruct(const std::vector< DevStudio::ProtectionEffectivenessStruct >& arrayOfProtectionEffectivenessStruct);
        std::vector<char> encodeArrayOfRelationship(const std::vector< DevStudio::Relation >& arrayOfRelationship);
        std::vector<char> encodeArrayOfResourceStatus(const std::vector< DevStudio::ResourceStatusStruct >& arrayOfResourceStatus);
        std::vector<char> encodeArrayOfSensor(const std::vector< DevStudio::SensorStruct >& arrayOfSensor);
        std::vector<char> encodeArrayOfSigmas6(const std::vector< float >& arrayOfSigmas6);
        std::vector<char> encodeArrayOfSignature(const std::vector< DevStudio::SignatureVariant >& arrayOfSignature);
        std::vector<char> encodeArrayOfSupplyStatus(const std::vector< DevStudio::SupplyStatusStruct >& arrayOfSupplyStatus);
        std::vector<char> encodeArrayOfTaskDefinitions(const std::vector< DevStudio::TaskDefinition >& arrayOfTaskDefinitions);
        std::vector<char> encodeArrayOfTaskProgress(const std::vector< DevStudio::TaskProgress >& arrayOfTaskProgress);
        std::vector<char> encodeArrayOfTrack(const std::vector< DevStudio::TrackStruct >& arrayOfTrack);
        std::vector<char> encodeArrayOfTreatmentStruct(const std::vector< DevStudio::TreatmentStruct >& arrayOfTreatmentStruct);
        std::vector<char> encodeArrayOfUuid(const std::vector< std::vector< char > >& arrayOfUuid);
        std::vector<char> encodeArrayOfWaypoints(const std::vector< DevStudio::Waypoint >& arrayOfWaypoints);
        std::vector<char> encodeArticulatedParameterStruct(const DevStudio::ArticulatedParameterStruct& articulatedParameterStruct);
        std::vector<char> encodeArticulatedParameterStructLengthlessArray(const std::vector< DevStudio::ArticulatedParameterStruct >& articulatedParameterStructLengthlessArray);
        std::vector<char> encodeArticulatedPartsStruct(const DevStudio::ArticulatedPartsStruct& articulatedPartsStruct);
        std::vector<char> encodeArticulatedPartsTypeEnum(const DevStudio::ArticulatedPartsTypeEnum::ArticulatedPartsTypeEnum& articulatedPartsTypeEnum);
        std::vector<char> encodeArticulatedTypeMetricEnum(const DevStudio::ArticulatedTypeMetricEnum::ArticulatedTypeMetricEnum& articulatedTypeMetricEnum);
        std::vector<char> encodeAttachTaskStruct(const DevStudio::AttachTaskStruct& attachTaskStruct);
        std::vector<char> encodeAttachedPartsStruct(const DevStudio::AttachedPartsStruct& attachedPartsStruct);
        std::vector<char> encodeAttackTypeEnum(const DevStudio::AttackTypeEnum::AttackTypeEnum& attackTypeEnum);
        std::vector<char> encodeAttributeValuePairStruct(const DevStudio::AttributeValuePairStruct& attributeValuePairStruct);
        std::vector<char> encodeAttributeValuePairStructArray1Plus(const std::vector< DevStudio::AttributeValuePairStruct >& attributeValuePairStructArray1Plus);
        std::vector<char> encodeAudioDataTypeStruct(const DevStudio::AudioDataTypeStruct& audioDataTypeStruct);
        std::vector<char> encodeBeamAntennaStruct(const DevStudio::BeamAntennaStruct& beamAntennaStruct);
        std::vector<char> encodeBeamFunctionCodeEnum(const DevStudio::BeamFunctionCodeEnum::BeamFunctionCodeEnum& beamFunctionCodeEnum);
        std::vector<char> encodeBinArray(const std::vector< bool >& binArray);
        std::vector<char> encodeBreachStruct(const DevStudio::BreachStruct& breachStruct);
        std::vector<char> encodeBreachStructLengthlessArray(const std::vector< DevStudio::BreachStruct >& breachStructLengthlessArray);
        std::vector<char> encodeBreachableSegmentStruct(const DevStudio::BreachableSegmentStruct& breachableSegmentStruct);
        std::vector<char> encodeBreachableSegmentStructLengthlessArray(const std::vector< DevStudio::BreachableSegmentStruct >& breachableSegmentStructLengthlessArray);
        std::vector<char> encodeBreachedStatusArray8(const std::vector< DevStudio::BreachedStatusEnum::BreachedStatusEnum >& breachedStatusArray8);
        std::vector<char> encodeBreachedStatusEnum(const DevStudio::BreachedStatusEnum::BreachedStatusEnum& breachedStatusEnum);
        std::vector<char> encodeCBRNAlarmStruct(const DevStudio::CBRNAlarmStruct& cBRNAlarmStruct);
        std::vector<char> encodeCBRNDamageEnum(const DevStudio::CBRNDamageEnum::CBRNDamageEnum& cBRNDamageEnum);
        std::vector<char> encodeCBRNExposureStruct(const DevStudio::CBRNExposureStruct& cBRNExposureStruct);
        std::vector<char> encodeCBRNSensorReadingStruct(const DevStudio::CBRNSensorReadingStruct& cBRNSensorReadingStruct);
        std::vector<char> encodeCOMBICStateRecStruct(const DevStudio::COMBICStateRecStruct& cOMBICStateRecStruct);
        std::vector<char> encodeCamouflageEnum(const DevStudio::CamouflageEnum::CamouflageEnum& camouflageEnum);
        std::vector<char> encodeCaptureStatusEnum(const DevStudio::CaptureStatusEnum::CaptureStatusEnum& captureStatusEnum);
        std::vector<char> encodeChangeAltitudeTaskStruct(const DevStudio::ChangeAltitudeTaskStruct& changeAltitudeTaskStruct);
        std::vector<char> encodeChangeHeadingTaskStruct(const DevStudio::ChangeHeadingTaskStruct& changeHeadingTaskStruct);
        std::vector<char> encodeChangeRulesOfEngagementTaskStruct(const DevStudio::ChangeRulesOfEngagementTaskStruct& changeRulesOfEngagementTaskStruct);
        std::vector<char> encodeChangeSpeedTaskStruct(const DevStudio::ChangeSpeedTaskStruct& changeSpeedTaskStruct);
        std::vector<char> encodeChemicalContentEnum(const DevStudio::ChemicalContentEnum::ChemicalContentEnum& chemicalContentEnum);
        std::vector<char> encodeClearEngineeringTaskStruct(const DevStudio::ClearEngineeringTaskStruct& clearEngineeringTaskStruct);
        std::vector<char> encodeClockTimeStruct(const DevStudio::ClockTimeStruct& clockTimeStruct);
        std::vector<char> encodeClockTimeStructLengthlessArray(const std::vector< DevStudio::ClockTimeStruct >& clockTimeStructLengthlessArray);
        std::vector<char> encodeCloudStruct(const DevStudio::CloudStruct& cloudStruct);
        std::vector<char> encodeCloudTypeEnum(const DevStudio::CloudTypeEnum::CloudTypeEnum& cloudTypeEnum);
        std::vector<char> encodeCoefficientsLengthlessArray1Plus(const std::vector< float >& coefficientsLengthlessArray1Plus);
        std::vector<char> encodeCollisionTypeEnum(const DevStudio::CollisionTypeEnum::CollisionTypeEnum& collisionTypeEnum);
        std::vector<char> encodeCombinationModulationTypeEnum(const DevStudio::CombinationModulationTypeEnum::CombinationModulationTypeEnum& combinationModulationTypeEnum);
        std::vector<char> encodeCommunicationNetworkTypeEnum(const DevStudio::CommunicationNetworkTypeEnum::CommunicationNetworkTypeEnum& communicationNetworkTypeEnum);
        std::vector<char> encodeCommunicationServiceTypeEnum(const DevStudio::CommunicationServiceTypeEnum::CommunicationServiceTypeEnum& communicationServiceTypeEnum);
        std::vector<char> encodeComplianceStateEnum(const DevStudio::ComplianceStateEnum::ComplianceStateEnum& complianceStateEnum);
        std::vector<char> encodeCone1GeomRecStruct(const DevStudio::Cone1GeomRecStruct& cone1GeomRecStruct);
        std::vector<char> encodeCone2GeomRecStruct(const DevStudio::Cone2GeomRecStruct& cone2GeomRecStruct);
        std::vector<char> encodeConnectionReceiverArray(const std::vector< DevStudio::ConnectionReceiverStruct >& connectionReceiverArray);
        std::vector<char> encodeConnectionReceiverStruct(const DevStudio::ConnectionReceiverStruct& connectionReceiverStruct);
        std::vector<char> encodeConnectionTypeEnum(const DevStudio::ConnectionTypeEnum::ConnectionTypeEnum& connectionTypeEnum);
        std::vector<char> encodeConstituentPartNatureEnum(const DevStudio::ConstituentPartNatureEnum::ConstituentPartNatureEnum& constituentPartNatureEnum);
        std::vector<char> encodeConstituentPartPositionEnum(const DevStudio::ConstituentPartPositionEnum::ConstituentPartPositionEnum& constituentPartPositionEnum);
        std::vector<char> encodeConstituentPartRelationshipStruct(const DevStudio::ConstituentPartRelationshipStruct& constituentPartRelationshipStruct);
        std::vector<char> encodeConstituentPartStationNameEnum(const DevStudio::ConstituentPartStationNameEnum::ConstituentPartStationNameEnum& constituentPartStationNameEnum);
        std::vector<char> encodeContourStruct(const DevStudio::ContourStruct& contourStruct);
        std::vector<char> encodeCreateBreachTaskStruct(const DevStudio::CreateBreachTaskStruct& createBreachTaskStruct);
        std::vector<char> encodeCreateFireBreakTaskStruct(const DevStudio::CreateFireBreakTaskStruct& createFireBreakTaskStruct);
        std::vector<char> encodeCreateLeveeTaskStruct(const DevStudio::CreateLeveeTaskStruct& createLeveeTaskStruct);
        std::vector<char> encodeCreateObstacleTaskStruct(const DevStudio::CreateObstacleTaskStruct& createObstacleTaskStruct);
        std::vector<char> encodeCryptographicModeEnum(const DevStudio::CryptographicModeEnum::CryptographicModeEnum& cryptographicModeEnum);
        std::vector<char> encodeCryptographicSystemTypeEnum(const DevStudio::CryptographicSystemTypeEnum::CryptographicSystemTypeEnum& cryptographicSystemTypeEnum);
        std::vector<char> encodeCurrentStruct(const DevStudio::CurrentStruct& currentStruct);
        std::vector<char> encodeDamageStatusEnhancedEnum(const DevStudio::DamageStatusEnhancedEnum::DamageStatusEnhancedEnum& damageStatusEnhancedEnum);
        std::vector<char> encodeDamageStatusEnum(const DevStudio::DamageStatusEnum::DamageStatusEnum& damageStatusEnum);
        std::vector<char> encodeDatumIdentifierEnum(const DevStudio::DatumIdentifierEnum::DatumIdentifierEnum& datumIdentifierEnum);
        std::vector<char> encodeDatumIdentifierLengthlessArray(const std::vector< DevStudio::DatumIdentifierEnum::DatumIdentifierEnum >& datumIdentifierLengthlessArray);
        std::vector<char> encodeDeadReckoningAlgorithmEnum(const DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum& deadReckoningAlgorithmEnum);
        std::vector<char> encodeDepthMeterFloat32LengthlessArray(const std::vector< float >& depthMeterFloat32LengthlessArray);
        std::vector<char> encodeDesignatorCodeEnum(const DevStudio::DesignatorCodeEnum::DesignatorCodeEnum& designatorCodeEnum);
        std::vector<char> encodeDesignatorCodeNameEnum(const DevStudio::DesignatorCodeNameEnum::DesignatorCodeNameEnum& designatorCodeNameEnum);
        std::vector<char> encodeDetectedEquipment(const DevStudio::DetectedEquipment& detectedEquipment);
        std::vector<char> encodeDetonationResultCodeEnum(const DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum& detonationResultCodeEnum);
        std::vector<char> encodeDimensionRateStruct(const DevStudio::DimensionRateStruct& dimensionRateStruct);
        std::vector<char> encodeDimensionStruct(const DevStudio::DimensionStruct& dimensionStruct);
        std::vector<char> encodeDirectFireTaskStruct(const DevStudio::DirectFireTaskStruct& directFireTaskStruct);
        std::vector<char> encodeDisruptCommunicationTaskStruct(const DevStudio::DisruptCommunicationTaskStruct& disruptCommunicationTaskStruct);
        std::vector<char> encodeEchelonEnum(const DevStudio::EchelonEnum::EchelonEnum& echelonEnum);
        std::vector<char> encodeElapsedTimeProgress(const DevStudio::ElapsedTimeProgress& elapsedTimeProgress);
        std::vector<char> encodeEllipsoid1GeomRecStruct(const DevStudio::Ellipsoid1GeomRecStruct& ellipsoid1GeomRecStruct);
        std::vector<char> encodeEllipsoid2GeomRecStruct(const DevStudio::Ellipsoid2GeomRecStruct& ellipsoid2GeomRecStruct);
        std::vector<char> encodeEmitterFunctionEnum(const DevStudio::EmitterFunctionEnum::EmitterFunctionEnum& emitterFunctionEnum);
        std::vector<char> encodeEmitterTypeEnum(const DevStudio::EmitterTypeEnum::EmitterTypeEnum& emitterTypeEnum);
        std::vector<char> encodeEmptyTaskStruct(const DevStudio::EmptyTaskStruct& emptyTaskStruct);
        std::vector<char> encodeEncodingTypeEnum(const DevStudio::EncodingTypeEnum::EncodingTypeEnum& encodingTypeEnum);
        std::vector<char> encodeEngineeringTaskStruct(const DevStudio::EngineeringTaskStruct& engineeringTaskStruct);
        std::vector<char> encodeEntityControlActionEnum(const DevStudio::EntityControlActionEnum::EntityControlActionEnum& entityControlActionEnum);
        std::vector<char> encodeEntityControlActions(const std::vector< DevStudio::EntityControlActionEnum::EntityControlActionEnum >& entityControlActions);
        std::vector<char> encodeEntityCoordinateVectorStruct(const DevStudio::EntityCoordinateVectorStruct& entityCoordinateVectorStruct);
        std::vector<char> encodeEntityIdentifierStruct(const DevStudio::EntityIdentifierStruct& entityIdentifierStruct);
        std::vector<char> encodeEntityTypeStruct(const DevStudio::EntityTypeStruct& entityTypeStruct);
        std::vector<char> encodeEntityTypeStructLengthlessArray(const std::vector< DevStudio::EntityTypeStruct >& entityTypeStructLengthlessArray);
        std::vector<char> encodeEnvironmentConditionModelTypeEnum(const DevStudio::EnvironmentConditionModelTypeEnum::EnvironmentConditionModelTypeEnum& environmentConditionModelTypeEnum);
        std::vector<char> encodeEnvironmentDataCoordinateSystemEnum(const DevStudio::EnvironmentDataCoordinateSystemEnum::EnvironmentDataCoordinateSystemEnum& environmentDataCoordinateSystemEnum);
        std::vector<char> encodeEnvironmentDataRepresentationEnum(const DevStudio::EnvironmentDataRepresentationEnum::EnvironmentDataRepresentationEnum& environmentDataRepresentationEnum);
        std::vector<char> encodeEnvironmentDataSampleTypeEnum(const DevStudio::EnvironmentDataSampleTypeEnum::EnvironmentDataSampleTypeEnum& environmentDataSampleTypeEnum);
        std::vector<char> encodeEnvironmentGridAxisTypeEnum(const DevStudio::EnvironmentGridAxisTypeEnum::EnvironmentGridAxisTypeEnum& environmentGridAxisTypeEnum);
        std::vector<char> encodeEnvironmentGridTypeEnum(const DevStudio::EnvironmentGridTypeEnum::EnvironmentGridTypeEnum& environmentGridTypeEnum);
        std::vector<char> encodeEnvironmentModelTypeEnum(const DevStudio::EnvironmentModelTypeEnum::EnvironmentModelTypeEnum& environmentModelTypeEnum);
        std::vector<char> encodeEnvironmentObjectTypeStruct(const DevStudio::EnvironmentObjectTypeStruct& environmentObjectTypeStruct);
        std::vector<char> encodeEnvironmentRecStruct(const DevStudio::EnvironmentRecStruct& environmentRecStruct);
        std::vector<char> encodeEnvironmentRecStructArray(const std::vector< DevStudio::EnvironmentRecStruct >& environmentRecStructArray);
        std::vector<char> encodeEnvironmentRecVariantStruct(const DevStudio::EnvironmentRecVariantStruct& environmentRecVariantStruct);
        std::vector<char> encodeEnvironmentRecordTypeEnum(const DevStudio::EnvironmentRecordTypeEnum::EnvironmentRecordTypeEnum& environmentRecordTypeEnum);
        std::vector<char> encodeEnvironmentTypeStruct(const DevStudio::EnvironmentTypeStruct& environmentTypeStruct);
        std::vector<char> encodeEquipmentSymbolAmplificationStruct(const DevStudio::EquipmentSymbolAmplificationStruct& equipmentSymbolAmplificationStruct);
        std::vector<char> encodeEstablishCheckpointTaskStruct(const DevStudio::EstablishCheckpointTaskStruct& establishCheckpointTaskStruct);
        std::vector<char> encodeEstablishObservationPostTaskStruct(const DevStudio::EstablishObservationPostTaskStruct& establishObservationPostTaskStruct);
        std::vector<char> encodeEvacuateProgressStruct(const DevStudio::EvacuateProgressStruct& evacuateProgressStruct);
        std::vector<char> encodeEvacuateTaskStruct(const DevStudio::EvacuateTaskStruct& evacuateTaskStruct);
        std::vector<char> encodeEventIdentifierStruct(const DevStudio::EventIdentifierStruct& eventIdentifierStruct);
        std::vector<char> encodeEventTypeEnum(const DevStudio::EventTypeEnum::EventTypeEnum& eventTypeEnum);
        std::vector<char> encodeExhaustSmokeStruct(const DevStudio::ExhaustSmokeStruct& exhaustSmokeStruct);
        std::vector<char> encodeExhaustSmokeStructLengthlessArray(const std::vector< DevStudio::ExhaustSmokeStruct >& exhaustSmokeStructLengthlessArray);
        std::vector<char> encodeExposureTypeEnum(const DevStudio::ExposureTypeEnum::ExposureTypeEnum& exposureTypeEnum);
        std::vector<char> encodeExtinguishWildfireTaskStruct(const DevStudio::ExtinguishWildfireTaskStruct& extinguishWildfireTaskStruct);
        std::vector<char> encodeFederateControlActionEnum(const DevStudio::FederateControlActionEnum::FederateControlActionEnum& federateControlActionEnum);
        std::vector<char> encodeFederateControlActions(const std::vector< DevStudio::FederateControlActionEnum::FederateControlActionEnum >& federateControlActions);
        std::vector<char> encodeFederateIdentifierStruct(const DevStudio::FederateIdentifierStruct& federateIdentifierStruct);
        std::vector<char> encodeFireTaskProgress(const DevStudio::FireTaskProgress& fireTaskProgress);
        std::vector<char> encodeFixedDatumStruct(const DevStudio::FixedDatumStruct& fixedDatumStruct);
        std::vector<char> encodeFixedDatumStructLengthlessArray(const std::vector< DevStudio::FixedDatumStruct >& fixedDatumStructLengthlessArray);
        std::vector<char> encodeFlareStateRecStruct(const DevStudio::FlareStateRecStruct& flareStateRecStruct);
        std::vector<char> encodeFloat32Array1Plus(const std::vector< float >& float32Array1Plus);
        std::vector<char> encodeFollowEntityTaskStruct(const DevStudio::FollowEntityTaskStruct& followEntityTaskStruct);
        std::vector<char> encodeForceIdentifierEnum(const DevStudio::ForceIdentifierEnum::ForceIdentifierEnum& forceIdentifierEnum);
        std::vector<char> encodeFormationDataStruct(const DevStudio::FormationDataStruct& formationDataStruct);
        std::vector<char> encodeFormationEnum(const DevStudio::FormationEnum::FormationEnum& formationEnum);
        std::vector<char> encodeFormationLocationTypeEnum(const DevStudio::FormationLocationTypeEnum::FormationLocationTypeEnum& formationLocationTypeEnum);
        std::vector<char> encodeFormationStruct(const DevStudio::FormationStruct& formationStruct);
        std::vector<char> encodeFormationTypeEnum(const DevStudio::FormationTypeEnum::FormationTypeEnum& formationTypeEnum);
        std::vector<char> encodeFundamentalParameterDataStruct(const DevStudio::FundamentalParameterDataStruct& fundamentalParameterDataStruct);
        std::vector<char> encodeFundamentalParameterDataStructLengthlessArray(const std::vector< DevStudio::FundamentalParameterDataStruct >& fundamentalParameterDataStructLengthlessArray);
        std::vector<char> encodeFuseTypeEnum(const DevStudio::FuseTypeEnum::FuseTypeEnum& fuseTypeEnum);
        std::vector<char> encodeGaussPlumeGeomRecStruct(const DevStudio::GaussPlumeGeomRecStruct& gaussPlumeGeomRecStruct);
        std::vector<char> encodeGaussPuffGeomRecStruct(const DevStudio::GaussPuffGeomRecStruct& gaussPuffGeomRecStruct);
        std::vector<char> encodeGeoLocationTypeEnum(const DevStudio::GeoLocationTypeEnum::GeoLocationTypeEnum& geoLocationTypeEnum);
        std::vector<char> encodeGeoReferenceVariant(const DevStudio::GeoReferenceVariant& geoReferenceVariant);
        std::vector<char> encodeGeodeticCircle(const DevStudio::GeodeticCircle& geodeticCircle);
        std::vector<char> encodeGeodeticLocation(const DevStudio::GeodeticLocation& geodeticLocation);
        std::vector<char> encodeGeodeticPath(const std::vector< DevStudio::GeodeticPoint >& geodeticPath);
        std::vector<char> encodeGeodeticPoint(const DevStudio::GeodeticPoint& geodeticPoint);
        std::vector<char> encodeGeodeticPolygon(const std::vector< DevStudio::GeodeticLocation >& geodeticPolygon);
        std::vector<char> encodeGeodeticQuadrangle(const DevStudio::GeodeticQuadrangle& geodeticQuadrangle);
        std::vector<char> encodeGridAxisStruct(const DevStudio::GridAxisStruct& gridAxisStruct);
        std::vector<char> encodeGridAxisStructLengthlessArray(const std::vector< DevStudio::GridAxisStruct >& gridAxisStructLengthlessArray);
        std::vector<char> encodeGridAxisTypeVariantStruct(const DevStudio::GridAxisTypeVariantStruct& gridAxisTypeVariantStruct);
        std::vector<char> encodeGridDataRepresentationVariantStruct(const DevStudio::GridDataRepresentationVariantStruct& gridDataRepresentationVariantStruct);
        std::vector<char> encodeGridDataStruct(const DevStudio::GridDataStruct& gridDataStruct);
        std::vector<char> encodeGridDataStructLengthlessArray(const std::vector< DevStudio::GridDataStruct >& gridDataStructLengthlessArray);
        std::vector<char> encodeGridValueType0Struct(const DevStudio::GridValueType0Struct& gridValueType0Struct);
        std::vector<char> encodeGridValueType1Struct(const DevStudio::GridValueType1Struct& gridValueType1Struct);
        std::vector<char> encodeGridValueType2Struct(const DevStudio::GridValueType2Struct& gridValueType2Struct);
        std::vector<char> encodeGroundFormationTypeEnum(const DevStudio::GroundFormationTypeEnum::GroundFormationTypeEnum& groundFormationTypeEnum);
        std::vector<char> encodeHLAargumentList(const std::vector< std::wstring >& hLAargumentList);
        std::vector<char> encodeHLAfederateState(const DevStudio::HLAfederateState::HLAfederateState& hLAfederateState);
        std::vector<char> encodeHLAhandleList(const std::vector< std::vector<char > >& hLAhandleList);
        std::vector<char> encodeHLAinteractionCount(const DevStudio::HLAinteractionCount& hLAinteractionCount);
        std::vector<char> encodeHLAinteractionCounts(const std::vector< DevStudio::HLAinteractionCount >& hLAinteractionCounts);
        std::vector<char> encodeHLAinteractionSubList(const std::vector< DevStudio::HLAinteractionSubscription >& hLAinteractionSubList);
        std::vector<char> encodeHLAinteractionSubscription(const DevStudio::HLAinteractionSubscription& hLAinteractionSubscription);
        std::vector<char> encodeHLAmoduleDesignatorList(const std::vector< std::wstring >& hLAmoduleDesignatorList);
        std::vector<char> encodeHLAobjectClassBasedCount(const DevStudio::HLAobjectClassBasedCount& hLAobjectClassBasedCount);
        std::vector<char> encodeHLAobjectClassBasedCounts(const std::vector< DevStudio::HLAobjectClassBasedCount >& hLAobjectClassBasedCounts);
        std::vector<char> encodeHLAorderType(const DevStudio::HLAorderType::HLAorderType& hLAorderType);
        std::vector<char> encodeHLAownership(const DevStudio::HLAownership::HLAownership& hLAownership);
        std::vector<char> encodeHLAresignAction(const DevStudio::HLAresignAction::HLAresignAction& hLAresignAction);
        std::vector<char> encodeHLAswitch(const DevStudio::HLAswitch::HLAswitch& hLAswitch);
        std::vector<char> encodeHLAsynchPointFederate(const DevStudio::HLAsynchPointFederate& hLAsynchPointFederate);
        std::vector<char> encodeHLAsynchPointFederateList(const std::vector< DevStudio::HLAsynchPointFederate >& hLAsynchPointFederateList);
        std::vector<char> encodeHLAsynchPointList(const std::vector< std::wstring >& hLAsynchPointList);
        std::vector<char> encodeHLAsynchPointStatus(const DevStudio::HLAsynchPointStatus::HLAsynchPointStatus& hLAsynchPointStatus);
        std::vector<char> encodeHLAtimeState(const DevStudio::HLAtimeState::HLAtimeState& hLAtimeState);
        std::vector<char> encodeHatchStateEnum(const DevStudio::HatchStateEnum::HatchStateEnum& hatchStateEnum);
        std::vector<char> encodeHazardTypeEnum(const DevStudio::HazardTypeEnum::HazardTypeEnum& hazardTypeEnum);
        std::vector<char> encodeHazardTypeEnum2(const DevStudio::HazardTypeEnum2::HazardTypeEnum2& hazardTypeEnum2);
        std::vector<char> encodeHazeStruct(const DevStudio::HazeStruct& hazeStruct);
        std::vector<char> encodeHazeTypeEnum(const DevStudio::HazeTypeEnum::HazeTypeEnum& hazeTypeEnum);
        std::vector<char> encodeHolding(const DevStudio::Holding& holding);
        std::vector<char> encodeHostilityStatusCodeEnum(const DevStudio::HostilityStatusCodeEnum::HostilityStatusCodeEnum& hostilityStatusCodeEnum);
        std::vector<char> encodeIPETypeEnum(const DevStudio::IPETypeEnum::IPETypeEnum& iPETypeEnum);
        std::vector<char> encodeIceStruct(const DevStudio::IceStruct& iceStruct);
        std::vector<char> encodeIceTypeEnum(const DevStudio::IceTypeEnum::IceTypeEnum& iceTypeEnum);
        std::vector<char> encodeIdentificationLevelEnum(const DevStudio::IdentificationLevelEnum::IdentificationLevelEnum& identificationLevelEnum);
        std::vector<char> encodeIffAlternateMode4Enum(const DevStudio::IffAlternateMode4Enum::IffAlternateMode4Enum& iffAlternateMode4Enum);
        std::vector<char> encodeIffApplicableModesEnum(const DevStudio::IffApplicableModesEnum::IffApplicableModesEnum& iffApplicableModesEnum);
        std::vector<char> encodeIffOperationalParameter1Enum(const DevStudio::IffOperationalParameter1Enum::IffOperationalParameter1Enum& iffOperationalParameter1Enum);
        std::vector<char> encodeIffOperationalParameter2Enum(const DevStudio::IffOperationalParameter2Enum::IffOperationalParameter2Enum& iffOperationalParameter2Enum);
        std::vector<char> encodeIffSystemModeEnum(const DevStudio::IffSystemModeEnum::IffSystemModeEnum& iffSystemModeEnum);
        std::vector<char> encodeIffSystemNameEnum(const DevStudio::IffSystemNameEnum::IffSystemNameEnum& iffSystemNameEnum);
        std::vector<char> encodeIffSystemTypeEnum(const DevStudio::IffSystemTypeEnum::IffSystemTypeEnum& iffSystemTypeEnum);
        std::vector<char> encodeIncomingConnectionArray(const std::vector< DevStudio::IncomingConnectionStruct >& incomingConnectionArray);
        std::vector<char> encodeIncomingConnectionStruct(const DevStudio::IncomingConnectionStruct& incomingConnectionStruct);
        std::vector<char> encodeIndirectFireTaskStruct(const DevStudio::IndirectFireTaskStruct& indirectFireTaskStruct);
        std::vector<char> encodeInstallationSymbolAmplificationStruct(const DevStudio::InstallationSymbolAmplificationStruct& installationSymbolAmplificationStruct);
        std::vector<char> encodeInteger16Array1Plus(const std::vector< short >& integer16Array1Plus);
        std::vector<char> encodeIrregularGridAxisStruct(const DevStudio::IrregularGridAxisStruct& irregularGridAxisStruct);
        std::vector<char> encodeIsPartOfStruct(const DevStudio::IsPartOfStruct& isPartOfStruct);
        std::vector<char> encodeLandslideTypeEnum(const DevStudio::LandslideTypeEnum::LandslideTypeEnum& landslideTypeEnum);
        std::vector<char> encodeLayMinefieldTaskStruct(const DevStudio::LayMinefieldTaskStruct& layMinefieldTaskStruct);
        std::vector<char> encodeLayerStruct(const DevStudio::LayerStruct& layerStruct);
        std::vector<char> encodeLightningTypeEnum(const DevStudio::LightningTypeEnum::LightningTypeEnum& lightningTypeEnum);
        std::vector<char> encodeLine1GeomRecStruct(const DevStudio::Line1GeomRecStruct& line1GeomRecStruct);
        std::vector<char> encodeLine2GeomRecStruct(const DevStudio::Line2GeomRecStruct& line2GeomRecStruct);
        std::vector<char> encodeLinearSegmentStruct(const DevStudio::LinearSegmentStruct& linearSegmentStruct);
        std::vector<char> encodeLinkStatusArray(const std::vector< DevStudio::LinkStatusStruct >& linkStatusArray);
        std::vector<char> encodeLinkStatusStruct(const DevStudio::LinkStatusStruct& linkStatusStruct);
        std::vector<char> encodeLocationStruct(const DevStudio::LocationStruct& locationStruct);
        std::vector<char> encodeLocationStructArray(const std::vector< DevStudio::LocationStruct >& locationStructArray);
        std::vector<char> encodeMagicMoveTaskStruct(const DevStudio::MagicMoveTaskStruct& magicMoveTaskStruct);
        std::vector<char> encodeMajorRFModulationTypeEnum(const DevStudio::MajorRFModulationTypeEnum::MajorRFModulationTypeEnum& majorRFModulationTypeEnum);
        std::vector<char> encodeManeuverIndicatorEnum(const DevStudio::ManeuverIndicatorEnum::ManeuverIndicatorEnum& maneuverIndicatorEnum);
        std::vector<char> encodeManufacturerIdArray3(const std::vector< char >& manufacturerIdArray3);
        std::vector<char> encodeMarkingArray11(const std::vector< char >& markingArray11);
        std::vector<char> encodeMarkingArray31(const std::vector< char >& markingArray31);
        std::vector<char> encodeMarkingEncodingEnum(const DevStudio::MarkingEncodingEnum::MarkingEncodingEnum& markingEncodingEnum);
        std::vector<char> encodeMarkingStruct(const DevStudio::MarkingStruct& markingStruct);
        std::vector<char> encodeMineDielectricDifferenceLengthlessArray(const std::vector< float >& mineDielectricDifferenceLengthlessArray);
        std::vector<char> encodeMineFusingStruct(const DevStudio::MineFusingStruct& mineFusingStruct);
        std::vector<char> encodeMineFusingStructLengthlessArray(const std::vector< DevStudio::MineFusingStruct >& mineFusingStructLengthlessArray);
        std::vector<char> encodeMineIdentifierLengthlessArray(const std::vector< unsigned short >& mineIdentifierLengthlessArray);
        std::vector<char> encodeMinefieldFusingEnum(const DevStudio::MinefieldFusingEnum::MinefieldFusingEnum& minefieldFusingEnum);
        std::vector<char> encodeMinefieldLaneEnum(const DevStudio::MinefieldLaneEnum::MinefieldLaneEnum& minefieldLaneEnum);
        std::vector<char> encodeMinefieldLaneMarkerStruct(const DevStudio::MinefieldLaneMarkerStruct& minefieldLaneMarkerStruct);
        std::vector<char> encodeMinefieldLaneMarkerStructLengthlessArray(const std::vector< DevStudio::MinefieldLaneMarkerStruct >& minefieldLaneMarkerStructLengthlessArray);
        std::vector<char> encodeMinefieldPaintSchemeEnum(const DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum& minefieldPaintSchemeEnum);
        std::vector<char> encodeMinefieldPaintSchemeLengthlessArray(const std::vector< DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum >& minefieldPaintSchemeLengthlessArray);
        std::vector<char> encodeMinefieldProtocolEnum(const DevStudio::MinefieldProtocolEnum::MinefieldProtocolEnum& minefieldProtocolEnum);
        std::vector<char> encodeMinefieldSensorTypeEnum(const DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum& minefieldSensorTypeEnum);
        std::vector<char> encodeMinefieldSensorTypeLengthlessArray(const std::vector< DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum >& minefieldSensorTypeLengthlessArray);
        std::vector<char> encodeMinefieldStatusEnum(const DevStudio::MinefieldStatusEnum::MinefieldStatusEnum& minefieldStatusEnum);
        std::vector<char> encodeMinefieldTypeEnum(const DevStudio::MinefieldTypeEnum::MinefieldTypeEnum& minefieldTypeEnum);
        std::vector<char> encodeMissingRecordNumbersLengthlessArray1Plus(const std::vector< char >& missingRecordNumbersLengthlessArray1Plus);
        std::vector<char> encodeMoveByRouteTaskStruct(const DevStudio::MoveByRouteTaskStruct& moveByRouteTaskStruct);
        std::vector<char> encodeMoveInDirectionTaskStruct(const DevStudio::MoveInDirectionTaskStruct& moveInDirectionTaskStruct);
        std::vector<char> encodeMoveIntoFormationTaskStruct(const DevStudio::MoveIntoFormationTaskStruct& moveIntoFormationTaskStruct);
        std::vector<char> encodeMoveTaskProgress(const DevStudio::MoveTaskProgress& moveTaskProgress);
        std::vector<char> encodeMoveToLocationTaskStruct(const DevStudio::MoveToLocationTaskStruct& moveToLocationTaskStruct);
        std::vector<char> encodeMoveTypeEnum(const DevStudio::MoveTypeEnum::MoveTypeEnum& moveTypeEnum);
        std::vector<char> encodeMsgIdEnum(const DevStudio::MsgIdEnum::MsgIdEnum& msgIdEnum);
        std::vector<char> encodeNamedLocationStruct(const DevStudio::NamedLocationStruct& namedLocationStruct);
        std::vector<char> encodeNavigationLightFlashingEnum(const DevStudio::NavigationLightFlashingEnum::NavigationLightFlashingEnum& navigationLightFlashingEnum);
        std::vector<char> encodeNavigationStatusEnum(const DevStudio::NavigationStatusEnum::NavigationStatusEnum& navigationStatusEnum);
        std::vector<char> encodeNetworkDeviceArray(const std::vector< DevStudio::NetworkDeviceStruct >& networkDeviceArray);
        std::vector<char> encodeNetworkDeviceEmptyCharactersticsStruct(const DevStudio::NetworkDeviceEmptyCharactersticsStruct& networkDeviceEmptyCharactersticsStruct);
        std::vector<char> encodeNetworkDeviceGenericTransmitterCharacteristicsStruct(const DevStudio::NetworkDeviceGenericTransmitterCharacteristicsStruct& networkDeviceGenericTransmitterCharacteristicsStruct);
        std::vector<char> encodeNetworkDeviceReceiverCharacteristicsVariant(const DevStudio::NetworkDeviceReceiverCharacteristicsVariant& networkDeviceReceiverCharacteristicsVariant);
        std::vector<char> encodeNetworkDeviceStruct(const DevStudio::NetworkDeviceStruct& networkDeviceStruct);
        std::vector<char> encodeNetworkDeviceTransmitterCharacteristicsVariant(const DevStudio::NetworkDeviceTransmitterCharacteristicsVariant& networkDeviceTransmitterCharacteristicsVariant);
        std::vector<char> encodeNomenclatureEnum(const DevStudio::NomenclatureEnum::NomenclatureEnum& nomenclatureEnum);
        std::vector<char> encodeNomenclatureVersionEnum(const DevStudio::NomenclatureVersionEnum::NomenclatureVersionEnum& nomenclatureVersionEnum);
        std::vector<char> encodeObserveTaskStruct(const DevStudio::ObserveTaskStruct& observeTaskStruct);
        std::vector<char> encodeObservedEquipment(const DevStudio::ObservedEquipment& observedEquipment);
        std::vector<char> encodeOctetArray(const std::vector< char >& octetArray);
        std::vector<char> encodeOctetArray1Plus(const std::vector< char >& octetArray1Plus);
        std::vector<char> encodeOctetArray2(const std::vector< char >& octetArray2);
        std::vector<char> encodeOctetArray3(const std::vector< char >& octetArray3);
        std::vector<char> encodeOctetArray4(const std::vector< char >& octetArray4);
        std::vector<char> encodeOctetArray7(const std::vector< char >& octetArray7);
        std::vector<char> encodeOctetArray8(const std::vector< char >& octetArray8);
        std::vector<char> encodeOctetPadding32Array(const std::vector< char >& octetPadding32Array);
        std::vector<char> encodeOctetPadding64Array(const std::vector< char >& octetPadding64Array);
        std::vector<char> encodeOpacityCodeEnum(const DevStudio::OpacityCodeEnum::OpacityCodeEnum& opacityCodeEnum);
        std::vector<char> encodeOperateCheckpointTaskStruct(const DevStudio::OperateCheckpointTaskStruct& operateCheckpointTaskStruct);
        std::vector<char> encodeOperateObservationPostTaskStruct(const DevStudio::OperateObservationPostTaskStruct& operateObservationPostTaskStruct);
        std::vector<char> encodeOrientationStruct(const DevStudio::OrientationStruct& orientationStruct);
        std::vector<char> encodeOrientationStructLengthlessArray(const std::vector< DevStudio::OrientationStruct >& orientationStructLengthlessArray);
        std::vector<char> encodeOtherActivityTaskStruct(const DevStudio::OtherActivityTaskStruct& otherActivityTaskStruct);
        std::vector<char> encodeParameterTypeEnum(const DevStudio::ParameterTypeEnum::ParameterTypeEnum& parameterTypeEnum);
        std::vector<char> encodeParameterValueVariantStruct(const DevStudio::ParameterValueVariantStruct& parameterValueVariantStruct);
        std::vector<char> encodePartNumberEnum(const DevStudio::PartNumberEnum::PartNumberEnum& partNumberEnum);
        std::vector<char> encodePatrolMoveTypeEnum(const DevStudio::PatrolMoveTypeEnum::PatrolMoveTypeEnum& patrolMoveTypeEnum);
        std::vector<char> encodePatrolTaskProgress(const DevStudio::PatrolTaskProgress& patrolTaskProgress);
        std::vector<char> encodePatrolTaskStruct(const DevStudio::PatrolTaskStruct& patrolTaskStruct);
        std::vector<char> encodePatrolTypeEnum(const DevStudio::PatrolTypeEnum::PatrolTypeEnum& patrolTypeEnum);
        std::vector<char> encodePerimeterPointStruct(const DevStudio::PerimeterPointStruct& perimeterPointStruct);
        std::vector<char> encodePerimeterPointStructLengthlessArray(const std::vector< DevStudio::PerimeterPointStruct >& perimeterPointStructLengthlessArray);
        std::vector<char> encodePhysicalGenericNetworkStruct(const DevStudio::PhysicalGenericNetworkStruct& physicalGenericNetworkStruct);
        std::vector<char> encodePhysicalNetworkDescriptionVariant(const DevStudio::PhysicalNetworkDescriptionVariant& physicalNetworkDescriptionVariant);
        std::vector<char> encodePhysicalNetworkTypeEnum(const DevStudio::PhysicalNetworkTypeEnum::PhysicalNetworkTypeEnum& physicalNetworkTypeEnum);
        std::vector<char> encodePhysicalUndefinedNetworkStruct(const DevStudio::PhysicalUndefinedNetworkStruct& physicalUndefinedNetworkStruct);
        std::vector<char> encodePlumeDimensionRateStruct(const DevStudio::PlumeDimensionRateStruct& plumeDimensionRateStruct);
        std::vector<char> encodePlumeDimensionStruct(const DevStudio::PlumeDimensionStruct& plumeDimensionStruct);
        std::vector<char> encodePoint2GeomRecStruct(const DevStudio::Point2GeomRecStruct& point2GeomRecStruct);
        std::vector<char> encodePrecipitationStruct(const DevStudio::PrecipitationStruct& precipitationStruct);
        std::vector<char> encodePrecipitationTypeEnum(const DevStudio::PrecipitationTypeEnum::PrecipitationTypeEnum& precipitationTypeEnum);
        std::vector<char> encodePropulsionPlantEnum(const DevStudio::PropulsionPlantEnum::PropulsionPlantEnum& propulsionPlantEnum);
        std::vector<char> encodePropulsionSystemDataStruct(const DevStudio::PropulsionSystemDataStruct& propulsionSystemDataStruct);
        std::vector<char> encodePropulsionSystemDataStructLengthlessArray(const std::vector< DevStudio::PropulsionSystemDataStruct >& propulsionSystemDataStructLengthlessArray);
        std::vector<char> encodeProtectionEffectivenessStruct(const DevStudio::ProtectionEffectivenessStruct& protectionEffectivenessStruct);
        std::vector<char> encodePulseModulationTypeEnum(const DevStudio::PulseModulationTypeEnum::PulseModulationTypeEnum& pulseModulationTypeEnum);
        std::vector<char> encodePumpFloodingTaskStruct(const DevStudio::PumpFloodingTaskStruct& pumpFloodingTaskStruct);
        std::vector<char> encodeRFModulationSystemTypeEnum(const DevStudio::RFModulationSystemTypeEnum::RFModulationSystemTypeEnum& rFModulationSystemTypeEnum);
        std::vector<char> encodeRFModulationTypeVariantStruct(const DevStudio::RFModulationTypeVariantStruct& rFModulationTypeVariantStruct);
        std::vector<char> encodeRTIobjectIdArray(const std::vector< std::string >& rTIobjectIdArray);
        std::vector<char> encodeRadioInputSourceEnum(const DevStudio::RadioInputSourceEnum::RadioInputSourceEnum& radioInputSourceEnum);
        std::vector<char> encodeRadioTypeStruct(const DevStudio::RadioTypeStruct& radioTypeStruct);
        std::vector<char> encodeReceiverOperationalStatusEnum(const DevStudio::ReceiverOperationalStatusEnum::ReceiverOperationalStatusEnum& receiverOperationalStatusEnum);
        std::vector<char> encodeRecordSetStruct(const DevStudio::RecordSetStruct& recordSetStruct);
        std::vector<char> encodeRecordSetStructArray1Plus(const std::vector< DevStudio::RecordSetStruct >& recordSetStructArray1Plus);
        std::vector<char> encodeRecordStruct(const DevStudio::RecordStruct& recordStruct);
        std::vector<char> encodeRecordStructArray(const std::vector< DevStudio::RecordStruct >& recordStructArray);
        std::vector<char> encodeRectVol1GeomRecStruct(const DevStudio::RectVol1GeomRecStruct& rectVol1GeomRecStruct);
        std::vector<char> encodeRectVol2GeomRecStruct(const DevStudio::RectVol2GeomRecStruct& rectVol2GeomRecStruct);
        std::vector<char> encodeRectVol3GeomRecStruct(const DevStudio::RectVol3GeomRecStruct& rectVol3GeomRecStruct);
        std::vector<char> encodeReferenceSystemEnum(const DevStudio::ReferenceSystemEnum::ReferenceSystemEnum& referenceSystemEnum);
        std::vector<char> encodeRefuelingStationLightColorEnum(const DevStudio::RefuelingStationLightColorEnum::RefuelingStationLightColorEnum& refuelingStationLightColorEnum);
        std::vector<char> encodeRefuelingStationLightDayNightEnum(const DevStudio::RefuelingStationLightDayNightEnum::RefuelingStationLightDayNightEnum& refuelingStationLightDayNightEnum);
        std::vector<char> encodeRelation(const DevStudio::Relation& relation);
        std::vector<char> encodeRelativePositionStruct(const DevStudio::RelativePositionStruct& relativePositionStruct);
        std::vector<char> encodeRelativeRangeBearingStruct(const DevStudio::RelativeRangeBearingStruct& relativeRangeBearingStruct);
        std::vector<char> encodeReleaseDistributionStruct(const DevStudio::ReleaseDistributionStruct& releaseDistributionStruct);
        std::vector<char> encodeReleaseDynamicsStruct(const DevStudio::ReleaseDynamicsStruct& releaseDynamicsStruct);
        std::vector<char> encodeReleaseSizeStruct(const DevStudio::ReleaseSizeStruct& releaseSizeStruct);
        std::vector<char> encodeRepairResultEnum(const DevStudio::RepairResultEnum::RepairResultEnum& repairResultEnum);
        std::vector<char> encodeRepairTaskStruct(const DevStudio::RepairTaskStruct& repairTaskStruct);
        std::vector<char> encodeRepairTypeEnum(const DevStudio::RepairTypeEnum::RepairTypeEnum& repairTypeEnum);
        std::vector<char> encodeRequestStatusEnum(const DevStudio::RequestStatusEnum::RequestStatusEnum& requestStatusEnum);
        std::vector<char> encodeRequestedConnection(const DevStudio::RequestedConnection& requestedConnection);
        std::vector<char> encodeRequestedConnectionArray(const std::vector< DevStudio::RequestedConnection >& requestedConnectionArray);
        std::vector<char> encodeResourceStatusStruct(const DevStudio::ResourceStatusStruct& resourceStatusStruct);
        std::vector<char> encodeResponseFlagEnum(const DevStudio::ResponseFlagEnum::ResponseFlagEnum& responseFlagEnum);
        std::vector<char> encodeResupplyTaskStruct(const DevStudio::ResupplyTaskStruct& resupplyTaskStruct);
        std::vector<char> encodeRoadIceConditionEnum(const DevStudio::RoadIceConditionEnum::RoadIceConditionEnum& roadIceConditionEnum);
        std::vector<char> encodeRoundStruct(const DevStudio::RoundStruct& roundStruct);
        std::vector<char> encodeRulesOfEngagementEnum(const DevStudio::RulesOfEngagementEnum::RulesOfEngagementEnum& rulesOfEngagementEnum);
        std::vector<char> encodeSINCGARSModulationStruct(const DevStudio::SINCGARSModulationStruct& sINCGARSModulationStruct);
        std::vector<char> encodeSeaStateEnum(const DevStudio::SeaStateEnum::SeaStateEnum& seaStateEnum);
        std::vector<char> encodeSedimentTypeEnum(const DevStudio::SedimentTypeEnum::SedimentTypeEnum& sedimentTypeEnum);
        std::vector<char> encodeSendSafetyRelatedBroadcastMessageTaskStruct(const DevStudio::SendSafetyRelatedBroadcastMessageTaskStruct& sendSafetyRelatedBroadcastMessageTaskStruct);
        std::vector<char> encodeSendSafetyRelatedMessageTaskStruct(const DevStudio::SendSafetyRelatedMessageTaskStruct& sendSafetyRelatedMessageTaskStruct);
        std::vector<char> encodeSensorStateEnum(const DevStudio::SensorStateEnum::SensorStateEnum& sensorStateEnum);
        std::vector<char> encodeSensorStruct(const DevStudio::SensorStruct& sensorStruct);
        std::vector<char> encodeServiceTypeEnum(const DevStudio::ServiceTypeEnum::ServiceTypeEnum& serviceTypeEnum);
        std::vector<char> encodeSetTransmitterStatusTaskStruct(const DevStudio::SetTransmitterStatusTaskStruct& setTransmitterStatusTaskStruct);
        std::vector<char> encodeShaftDataStruct(const DevStudio::ShaftDataStruct& shaftDataStruct);
        std::vector<char> encodeShaftDataStructLengthlessArray1Plus(const std::vector< DevStudio::ShaftDataStruct >& shaftDataStructLengthlessArray1Plus);
        std::vector<char> encodeShipTypeEnum(const DevStudio::ShipTypeEnum::ShipTypeEnum& shipTypeEnum);
        std::vector<char> encodeSignalDataLengthlessArray1Plus(const std::vector< char >& signalDataLengthlessArray1Plus);
        std::vector<char> encodeSignatureTypeEnum(const DevStudio::SignatureTypeEnum::SignatureTypeEnum& signatureTypeEnum);
        std::vector<char> encodeSignatureVariant(const DevStudio::SignatureVariant& signatureVariant);
        std::vector<char> encodeSilentAggregateStruct(const DevStudio::SilentAggregateStruct& silentAggregateStruct);
        std::vector<char> encodeSilentAggregateStructLengthlessArray(const std::vector< DevStudio::SilentAggregateStruct >& silentAggregateStructLengthlessArray);
        std::vector<char> encodeSilentEntityStruct(const DevStudio::SilentEntityStruct& silentEntityStruct);
        std::vector<char> encodeSilentEntityStructLengthlessArray(const std::vector< DevStudio::SilentEntityStruct >& silentEntityStructLengthlessArray);
        std::vector<char> encodeSnowStruct(const DevStudio::SnowStruct& snowStruct);
        std::vector<char> encodeSpatialFPStruct(const DevStudio::SpatialFPStruct& spatialFPStruct);
        std::vector<char> encodeSpatialFVStruct(const DevStudio::SpatialFVStruct& spatialFVStruct);
        std::vector<char> encodeSpatialRPStruct(const DevStudio::SpatialRPStruct& spatialRPStruct);
        std::vector<char> encodeSpatialRVStruct(const DevStudio::SpatialRVStruct& spatialRVStruct);
        std::vector<char> encodeSpatialStaticStruct(const DevStudio::SpatialStaticStruct& spatialStaticStruct);
        std::vector<char> encodeSpatialVariantStruct(const DevStudio::SpatialVariantStruct& spatialVariantStruct);
        std::vector<char> encodeSphere1GeomRecStruct(const DevStudio::Sphere1GeomRecStruct& sphere1GeomRecStruct);
        std::vector<char> encodeSphere2GeomRecStruct(const DevStudio::Sphere2GeomRecStruct& sphere2GeomRecStruct);
        std::vector<char> encodeSphericalHarmonicAntennaStruct(const DevStudio::SphericalHarmonicAntennaStruct& sphericalHarmonicAntennaStruct);
        std::vector<char> encodeSpreadSpectrumEnum(const DevStudio::SpreadSpectrumEnum::SpreadSpectrumEnum& spreadSpectrumEnum);
        std::vector<char> encodeSpreadSpectrumVariantStruct(const DevStudio::SpreadSpectrumVariantStruct& spreadSpectrumVariantStruct);
        std::vector<char> encodeStanceCodeEnum(const DevStudio::StanceCodeEnum::StanceCodeEnum& stanceCodeEnum);
        std::vector<char> encodeStationEnum(const DevStudio::StationEnum::StationEnum& stationEnum);
        std::vector<char> encodeStationNameLocationVariantStruct(const DevStudio::StationNameLocationVariantStruct& stationNameLocationVariantStruct);
        std::vector<char> encodeStopFreezeReasonEnum(const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& stopFreezeReasonEnum);
        std::vector<char> encodeSubsurfaceFormationTypeEnum(const DevStudio::SubsurfaceFormationTypeEnum::SubsurfaceFormationTypeEnum& subsurfaceFormationTypeEnum);
        std::vector<char> encodeSupplyStatusStruct(const DevStudio::SupplyStatusStruct& supplyStatusStruct);
        std::vector<char> encodeSupplyStruct(const DevStudio::SupplyStruct& supplyStruct);
        std::vector<char> encodeSupplyStructLengthlessArray(const std::vector< DevStudio::SupplyStruct >& supplyStructLengthlessArray);
        std::vector<char> encodeSurfaceFormationTypeEnum(const DevStudio::SurfaceFormationTypeEnum::SurfaceFormationTypeEnum& surfaceFormationTypeEnum);
        std::vector<char> encodeSurfaceMoistureEnum(const DevStudio::SurfaceMoistureEnum::SurfaceMoistureEnum& surfaceMoistureEnum);
        std::vector<char> encodeSymbolAmplificationVariant(const DevStudio::SymbolAmplificationVariant& symbolAmplificationVariant);
        std::vector<char> encodeSymbolStruct(const DevStudio::SymbolStruct& symbolStruct);
        std::vector<char> encodeSymbolTypeEnum(const DevStudio::SymbolTypeEnum::SymbolTypeEnum& symbolTypeEnum);
        std::vector<char> encodeTacticalDataLinkTypeEnum(const DevStudio::TacticalDataLinkTypeEnum::TacticalDataLinkTypeEnum& tacticalDataLinkTypeEnum);
        std::vector<char> encodeTaskDefinition(const DevStudio::TaskDefinition& taskDefinition);
        std::vector<char> encodeTaskDefinitionVariantRecord(const DevStudio::TaskDefinitionVariantRecord& taskDefinitionVariantRecord);
        std::vector<char> encodeTaskModeEnum(const DevStudio::TaskModeEnum::TaskModeEnum& taskModeEnum);
        std::vector<char> encodeTaskProgress(const DevStudio::TaskProgress& taskProgress);
        std::vector<char> encodeTaskProgressVariantRecord(const DevStudio::TaskProgressVariantRecord& taskProgressVariantRecord);
        std::vector<char> encodeTaskStatusEnum(const DevStudio::TaskStatusEnum::TaskStatusEnum& taskStatusEnum);
        std::vector<char> encodeTemperatureDegreeCelsiusFloat32LengthlessArray(const std::vector< float >& temperatureDegreeCelsiusFloat32LengthlessArray);
        std::vector<char> encodeTrackStruct(const DevStudio::TrackStruct& trackStruct);
        std::vector<char> encodeTrailingEffectsCodeEnum(const DevStudio::TrailingEffectsCodeEnum::TrailingEffectsCodeEnum& trailingEffectsCodeEnum);
        std::vector<char> encodeTransferTypeEnum(const DevStudio::TransferTypeEnum::TransferTypeEnum& transferTypeEnum);
        std::vector<char> encodeTransmitterOperationalStatusEnum(const DevStudio::TransmitterOperationalStatusEnum::TransmitterOperationalStatusEnum& transmitterOperationalStatusEnum);
        std::vector<char> encodeTransportTaskStruct(const DevStudio::TransportTaskStruct& transportTaskStruct);
        std::vector<char> encodeTreatmentStruct(const DevStudio::TreatmentStruct& treatmentStruct);
        std::vector<char> encodeUUID(const std::vector< char >& uUID);
        std::vector<char> encodeUniformGeomRecStruct(const DevStudio::UniformGeomRecStruct& uniformGeomRecStruct);
        std::vector<char> encodeUnitSymbolAmplificationStruct(const DevStudio::UnitSymbolAmplificationStruct& unitSymbolAmplificationStruct);
        std::vector<char> encodeUnmodulatedTypeEnum(const DevStudio::UnmodulatedTypeEnum::UnmodulatedTypeEnum& unmodulatedTypeEnum);
        std::vector<char> encodeUnsignedInteger16Array1Plus(const std::vector< unsigned short >& unsignedInteger16Array1Plus);
        std::vector<char> encodeUnsignedInteger32LengthlessArray(const std::vector< unsigned int >& unsignedInteger32LengthlessArray);
        std::vector<char> encodeUnsignedInteger64Array1Plus(const std::vector< unsigned long long >& unsignedInteger64Array1Plus);
        std::vector<char> encodeUnsignedInteger8LengthlessArray(const std::vector< char >& unsignedInteger8LengthlessArray);
        std::vector<char> encodeUserProtocolEnum(const DevStudio::UserProtocolEnum::UserProtocolEnum& userProtocolEnum);
        std::vector<char> encodeVariableDatumStruct(const DevStudio::VariableDatumStruct& variableDatumStruct);
        std::vector<char> encodeVariableDatumStructArray(const std::vector< DevStudio::VariableDatumStruct >& variableDatumStructArray);
        std::vector<char> encodeVariableDatumStructLengthlessArray(const std::vector< DevStudio::VariableDatumStruct >& variableDatumStructLengthlessArray);
        std::vector<char> encodeVectoringNozzleSystemDataStruct(const DevStudio::VectoringNozzleSystemDataStruct& vectoringNozzleSystemDataStruct);
        std::vector<char> encodeVectoringNozzleSystemDataStructLengthlessArray(const std::vector< DevStudio::VectoringNozzleSystemDataStruct >& vectoringNozzleSystemDataStructLengthlessArray);
        std::vector<char> encodeVelocityVectorStruct(const DevStudio::VelocityVectorStruct& velocityVectorStruct);
        std::vector<char> encodeVisibleSideLocationEnum(const DevStudio::VisibleSideLocationEnum::VisibleSideLocationEnum& visibleSideLocationEnum);
        std::vector<char> encodeWarheadTypeEnum(const DevStudio::WarheadTypeEnum::WarheadTypeEnum& warheadTypeEnum);
        std::vector<char> encodeWaveStruct(const DevStudio::WaveStruct& waveStruct);
        std::vector<char> encodeWaypoint(const DevStudio::Waypoint& waypoint);
        std::vector<char> encodeWeaponControlOrderEnum(const DevStudio::WeaponControlOrderEnum::WeaponControlOrderEnum& weaponControlOrderEnum);
        std::vector<char> encodeWeaponStateEnum(const DevStudio::WeaponStateEnum::WeaponStateEnum& weaponStateEnum);
        std::vector<char> encodeWindStruct(const DevStudio::WindStruct& windStruct);
        std::vector<char> encodeWorldLocationStruct(const DevStudio::WorldLocationStruct& worldLocationStruct);
        std::vector<char> encodeWorldLocationStructLengthlessArray(const std::vector< DevStudio::WorldLocationStruct >& worldLocationStructLengthlessArray);
// === AUTO-GENERATED FUNCTIONS END ===
//pragma endregion

        std::vector<char> encodebool(const bool& _bool);
        std::vector<char> encodechar(const char& _char);
        std::vector<char> encodeshort(const short& _short);
        std::vector<char> encodeunsignedshort(const unsigned short& _unsignedShort);
        std::vector<char> encodeint(const int& _int);
		std::vector<char> encodeunsignedint(const unsigned int& _unsignedInt);
        std::vector<char> encodelong(const long& _long);
		std::vector<char> encodeunsignedlong(const unsigned long& _unsignedLong);
        std::vector<char> encodelonglong(const long long& _long);
        std::vector<char> encodeunsignedlonglong(const unsigned long long& _unsignedLong);
        std::vector<char> encodefloat(const float& _float);
        std::vector<char> encodedouble(const double& _double);
        std::vector<char> encodestring(const std::string& _string);
		std::vector<char> encodewstring(const std::wstring& _wstring);
    };
}

#endif


