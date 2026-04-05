#include <DevStudio/HlaLibSettings.h>
#include "DevStudio/HlaAllHeaders.h"

#include "Encoders.h"

namespace DevStudio
{
// === AUTO-GENERATED BUILDER BEGIN ===
    Encoders::Encoders() :
        _aTP45HazardAreaTypeEnumEncoder(),
        _accelerationVectorStructEncoder(),
        _acknowledgeFlagEnumEncoder(),
        _acknowledgementProtocolEnumEncoder(),
        _actionEnumEncoder(),
        _actionResultEnumEncoder(),
        _activeSonarEnumEncoder(),
        _activeSonarFunctionCodeEnumEncoder(),
        _activeSonarScanPatternEnumEncoder(),
        _activeStatusEnumEncoder(),
        _administerTreatmentTaskStructEncoder(),
        _agentClassEnumEncoder(),
        _agentConcentrationStructEncoder(),
        _agentMassStructEncoder(),
        _agentTypeEnumEncoder(),
        _aggregateMarkingStructEncoder(),
        _aggregateMissionEnumEncoder(),
        _aggregateStateEnumEncoder(),
        _aggregateStateFormationEnumEncoder(),
        _aidTypeEnumEncoder(),
        _airFormationTypeEnumEncoder(),
        _amplitudeAngleModulationTypeEnumEncoder(),
        _amplitudeModulationTypeEnumEncoder(),
        _angleModulationTypeEnumEncoder(),
        _angularVelocityVectorStructEncoder(),
        _antennaPatternTypeEnumEncoder(),
        _antennaPatternVariantStructEncoder(),
        _antennaPatternVariantStructLengthlessArrayEncoder(),
        _antiCollisionLightFlashingEnumEncoder(),
        _appearanceAntiCollisionDayNightEnumEncoder(),
        _appearanceNVGModeEnumEncoder(),
        _applyIPETaskStructEncoder(),
        _appointmentStructEncoder(),
        _arrayOfAgentConcentrationStructEncoder(),
        _arrayOfAgentMassStructEncoder(),
        _arrayOfAgentTypeEnumEncoder(),
        _arrayOfCBRNExposureStructEncoder(),
        _arrayOfCBRNSensorReadingStructEncoder(),
        _arrayOfCommunicationNetworksEncoder(),
        _arrayOfContourStructEncoder(),
        _arrayOfDetectedEquipmentEncoder(),
        _arrayOfHoldingsEncoder(),
        _arrayOfObservedEquipmentEncoder(),
        _arrayOfProtectionEffectivenessStructEncoder(),
        _arrayOfRelationshipEncoder(),
        _arrayOfResourceStatusEncoder(),
        _arrayOfSensorEncoder(),
        _arrayOfSigmas6Encoder(),
        _arrayOfSignatureEncoder(),
        _arrayOfSupplyStatusEncoder(),
        _arrayOfTaskDefinitionsEncoder(),
        _arrayOfTaskProgressEncoder(),
        _arrayOfTrackEncoder(),
        _arrayOfTreatmentStructEncoder(),
        _arrayOfUuidEncoder(),
        _arrayOfWaypointsEncoder(),
        _articulatedParameterStructEncoder(),
        _articulatedParameterStructLengthlessArrayEncoder(),
        _articulatedPartsStructEncoder(),
        _articulatedPartsTypeEnumEncoder(),
        _articulatedTypeMetricEnumEncoder(),
        _attachTaskStructEncoder(),
        _attachedPartsStructEncoder(),
        _attackTypeEnumEncoder(),
        _attributeValuePairStructArray1PlusEncoder(),
        _attributeValuePairStructEncoder(),
        _audioDataTypeStructEncoder(),
        _baseEncoder(),
        _beamAntennaStructEncoder(),
        _beamFunctionCodeEnumEncoder(),
        _binArrayEncoder(),
        _breachStructEncoder(),
        _breachStructLengthlessArrayEncoder(),
        _breachableSegmentStructEncoder(),
        _breachableSegmentStructLengthlessArrayEncoder(),
        _breachedStatusArray8Encoder(),
        _breachedStatusEnumEncoder(),
        _cBRNAlarmStructEncoder(),
        _cBRNDamageEnumEncoder(),
        _cBRNExposureStructEncoder(),
        _cBRNSensorReadingStructEncoder(),
        _cOMBICStateRecStructEncoder(),
        _camouflageEnumEncoder(),
        _captureStatusEnumEncoder(),
        _changeAltitudeTaskStructEncoder(),
        _changeHeadingTaskStructEncoder(),
        _changeRulesOfEngagementTaskStructEncoder(),
        _changeSpeedTaskStructEncoder(),
        _chemicalContentEnumEncoder(),
        _clearEngineeringTaskStructEncoder(),
        _clockTimeStructEncoder(),
        _clockTimeStructLengthlessArrayEncoder(),
        _cloudStructEncoder(),
        _cloudTypeEnumEncoder(),
        _coefficientsLengthlessArray1PlusEncoder(),
        _collisionTypeEnumEncoder(),
        _combinationModulationTypeEnumEncoder(),
        _communicationNetworkArrayEncoder(),
        _communicationNetworkTypeEnumEncoder(),
        _communicationServiceTypeEnumEncoder(),
        _complianceStateEnumEncoder(),
        _cone1GeomRecStructEncoder(),
        _cone2GeomRecStructEncoder(),
        _connectionReceiverArrayEncoder(),
        _connectionReceiverStructEncoder(),
        _connectionTypeEnumEncoder(),
        _constituentPartNatureEnumEncoder(),
        _constituentPartPositionEnumEncoder(),
        _constituentPartRelationshipStructEncoder(),
        _constituentPartStationNameEnumEncoder(),
        _contourStructEncoder(),
        _createBreachTaskStructEncoder(),
        _createFireBreakTaskStructEncoder(),
        _createLeveeTaskStructEncoder(),
        _createObstacleTaskStructEncoder(),
        _cryptographicModeEnumEncoder(),
        _cryptographicSystemTypeEnumEncoder(),
        _currentStructEncoder(),
        _damageStatusEnhancedEnumEncoder(),
        _damageStatusEnumEncoder(),
        _datumIdentifierEnumEncoder(),
        _datumIdentifierLengthlessArrayEncoder(),
        _deadReckoningAlgorithmEnumEncoder(),
        _depthMeterFloat32LengthlessArrayEncoder(),
        _designatorCodeEnumEncoder(),
        _designatorCodeNameEnumEncoder(),
        _detectedEquipmentEncoder(),
        _detonationResultCodeEnumEncoder(),
        _dimensionRateStructEncoder(),
        _dimensionStructEncoder(),
        _directFireTaskStructEncoder(),
        _disruptCommunicationTaskStructEncoder(),
        _echelonEnumEncoder(),
        _elapsedTimeProgressEncoder(),
        _ellipsoid1GeomRecStructEncoder(),
        _ellipsoid2GeomRecStructEncoder(),
        _emitterFunctionEnumEncoder(),
        _emitterTypeEnumEncoder(),
        _emptyTaskStructEncoder(),
        _encodingTypeEnumEncoder(),
        _engineeringTaskStructEncoder(),
        _entityControlActionEnumEncoder(),
        _entityControlActionsEncoder(),
        _entityCoordinateVectorStructEncoder(),
        _entityIdentifierStructEncoder(),
        _entityTypeStructEncoder(),
        _entityTypeStructLengthlessArrayEncoder(),
        _environmentConditionModelTypeEnumEncoder(),
        _environmentDataCoordinateSystemEnumEncoder(),
        _environmentDataRepresentationEnumEncoder(),
        _environmentDataSampleTypeEnumEncoder(),
        _environmentGridAxisTypeEnumEncoder(),
        _environmentGridTypeEnumEncoder(),
        _environmentModelTypeEnumEncoder(),
        _environmentObjectTypeStructEncoder(),
        _environmentRecStructArrayEncoder(),
        _environmentRecStructEncoder(),
        _environmentRecVariantStructEncoder(),
        _environmentRecordTypeEnumEncoder(),
        _environmentTypeStructEncoder(),
        _equipmentSymbolAmplificationStructEncoder(),
        _establishCheckpointTaskStructEncoder(),
        _establishObservationPostTaskStructEncoder(),
        _evacuateProgressStructEncoder(),
        _evacuateTaskStructEncoder(),
        _eventIdentifierStructEncoder(),
        _eventTypeEnumEncoder(),
        _exhaustSmokeStructEncoder(),
        _exhaustSmokeStructLengthlessArrayEncoder(),
        _exposureTypeEnumEncoder(),
        _extinguishWildfireTaskStructEncoder(),
        _federateControlActionEnumEncoder(),
        _federateControlActionsEncoder(),
        _federateIdentifierStructEncoder(),
        _fireTaskProgressEncoder(),
        _fixedDatumStructEncoder(),
        _fixedDatumStructLengthlessArrayEncoder(),
        _flareStateRecStructEncoder(),
        _float32Array1PlusEncoder(),
        _followEntityTaskStructEncoder(),
        _forceIdentifierEnumEncoder(),
        _formationDataStructEncoder(),
        _formationEnumEncoder(),
        _formationLocationTypeEnumEncoder(),
        _formationStructEncoder(),
        _formationTypeEnumEncoder(),
        _fundamentalParameterDataStructEncoder(),
        _fundamentalParameterDataStructLengthlessArrayEncoder(),
        _fuseTypeEnumEncoder(),
        _gaussPlumeGeomRecStructEncoder(),
        _gaussPuffGeomRecStructEncoder(),
        _geoLocationTypeEnumEncoder(),
        _geoReferenceVariantEncoder(),
        _geodeticCircleEncoder(),
        _geodeticLocationEncoder(),
        _geodeticPathEncoder(),
        _geodeticPointEncoder(),
        _geodeticPolygonEncoder(),
        _geodeticQuadrangleEncoder(),
        _gridAxisStructEncoder(),
        _gridAxisStructLengthlessArrayEncoder(),
        _gridAxisTypeVariantStructEncoder(),
        _gridDataRepresentationVariantStructEncoder(),
        _gridDataStructEncoder(),
        _gridDataStructLengthlessArrayEncoder(),
        _gridValueType0StructEncoder(),
        _gridValueType1StructEncoder(),
        _gridValueType2StructEncoder(),
        _groundFormationTypeEnumEncoder(),
        _hLAargumentListEncoder(),
        _hLAfederateStateEncoder(),
        _hLAhandleListEncoder(),
        _hLAinteractionCountEncoder(),
        _hLAinteractionCountsEncoder(),
        _hLAinteractionSubListEncoder(),
        _hLAinteractionSubscriptionEncoder(),
        _hLAmoduleDesignatorListEncoder(),
        _hLAobjectClassBasedCountEncoder(),
        _hLAobjectClassBasedCountsEncoder(),
        _hLAorderTypeEncoder(),
        _hLAownershipEncoder(),
        _hLAresignActionEncoder(),
        _hLAswitchEncoder(),
        _hLAsynchPointFederateEncoder(),
        _hLAsynchPointFederateListEncoder(),
        _hLAsynchPointListEncoder(),
        _hLAsynchPointStatusEncoder(),
        _hLAtimeStateEncoder(),
        _hatchStateEnumEncoder(),
        _hazardTypeEnum2Encoder(),
        _hazardTypeEnumEncoder(),
        _hazeStructEncoder(),
        _hazeTypeEnumEncoder(),
        _holdingEncoder(),
        _hostilityStatusCodeEnumEncoder(),
        _iPETypeEnumEncoder(),
        _iceStructEncoder(),
        _iceTypeEnumEncoder(),
        _identificationLevelEnumEncoder(),
        _iffAlternateMode4EnumEncoder(),
        _iffApplicableModesEnumEncoder(),
        _iffOperationalParameter1EnumEncoder(),
        _iffOperationalParameter2EnumEncoder(),
        _iffSystemModeEnumEncoder(),
        _iffSystemNameEnumEncoder(),
        _iffSystemTypeEnumEncoder(),
        _incomingConnectionArrayEncoder(),
        _incomingConnectionStructEncoder(),
        _indirectFireTaskStructEncoder(),
        _installationSymbolAmplificationStructEncoder(),
        _integer16Array1PlusEncoder(),
        _irregularGridAxisStructEncoder(),
        _isPartOfStructEncoder(),
        _landslideTypeEnumEncoder(),
        _layMinefieldTaskStructEncoder(),
        _layerStructEncoder(),
        _lightningTypeEnumEncoder(),
        _line1GeomRecStructEncoder(),
        _line2GeomRecStructEncoder(),
        _linearSegmentStructEncoder(),
        _linkStatusArrayEncoder(),
        _linkStatusStructEncoder(),
        _locationStructArrayEncoder(),
        _locationStructEncoder(),
        _magicMoveTaskStructEncoder(),
        _majorRFModulationTypeEnumEncoder(),
        _maneuverIndicatorEnumEncoder(),
        _manufacturerIdArray3Encoder(),
        _markingArray11Encoder(),
        _markingArray31Encoder(),
        _markingEncodingEnumEncoder(),
        _markingStructEncoder(),
        _mineDielectricDifferenceLengthlessArrayEncoder(),
        _mineFusingStructEncoder(),
        _mineFusingStructLengthlessArrayEncoder(),
        _mineIdentifierLengthlessArrayEncoder(),
        _minefieldFusingEnumEncoder(),
        _minefieldLaneEnumEncoder(),
        _minefieldLaneMarkerStructEncoder(),
        _minefieldLaneMarkerStructLengthlessArrayEncoder(),
        _minefieldPaintSchemeEnumEncoder(),
        _minefieldPaintSchemeLengthlessArrayEncoder(),
        _minefieldProtocolEnumEncoder(),
        _minefieldSensorTypeEnumEncoder(),
        _minefieldSensorTypeLengthlessArrayEncoder(),
        _minefieldStatusEnumEncoder(),
        _minefieldTypeEnumEncoder(),
        _missingRecordNumbersLengthlessArray1PlusEncoder(),
        _moveByRouteTaskStructEncoder(),
        _moveInDirectionTaskStructEncoder(),
        _moveIntoFormationTaskStructEncoder(),
        _moveTaskProgressEncoder(),
        _moveToLocationTaskStructEncoder(),
        _moveTypeEnumEncoder(),
        _msgIdEnumEncoder(),
        _namedLocationStructEncoder(),
        _navigationLightFlashingEnumEncoder(),
        _navigationStatusEnumEncoder(),
        _networkDeviceArrayEncoder(),
        _networkDeviceEmptyCharactersticsStructEncoder(),
        _networkDeviceGenericTransmitterCharacteristicsStructEncoder(),
        _networkDeviceReceiverCharacteristicsVariantEncoder(),
        _networkDeviceStructEncoder(),
        _networkDeviceTransmitterCharacteristicsVariantEncoder(),
        _nomenclatureEnumEncoder(),
        _nomenclatureVersionEnumEncoder(),
        _observeTaskStructEncoder(),
        _observedEquipmentEncoder(),
        _octetArray1PlusEncoder(),
        _octetArray2Encoder(),
        _octetArray3Encoder(),
        _octetArray4Encoder(),
        _octetArray7Encoder(),
        _octetArray8Encoder(),
        _octetArrayEncoder(),
        _octetPadding32ArrayEncoder(),
        _octetPadding64ArrayEncoder(),
        _opacityCodeEnumEncoder(),
        _operateCheckpointTaskStructEncoder(),
        _operateObservationPostTaskStructEncoder(),
        _orientationStructEncoder(),
        _orientationStructLengthlessArrayEncoder(),
        _otherActivityTaskStructEncoder(),
        _parameterTypeEnumEncoder(),
        _parameterValueVariantStructEncoder(),
        _partNumberEnumEncoder(),
        _patrolMoveTypeEnumEncoder(),
        _patrolTaskProgressEncoder(),
        _patrolTaskStructEncoder(),
        _patrolTypeEnumEncoder(),
        _perimeterPointStructEncoder(),
        _perimeterPointStructLengthlessArrayEncoder(),
        _physicalGenericNetworkStructEncoder(),
        _physicalNetworkDescriptionVariantEncoder(),
        _physicalNetworkTypeEnumEncoder(),
        _physicalUndefinedNetworkStructEncoder(),
        _plumeDimensionRateStructEncoder(),
        _plumeDimensionStructEncoder(),
        _point2GeomRecStructEncoder(),
        _precipitationStructEncoder(),
        _precipitationTypeEnumEncoder(),
        _propulsionPlantEnumEncoder(),
        _propulsionSystemDataStructEncoder(),
        _propulsionSystemDataStructLengthlessArrayEncoder(),
        _protectionEffectivenessStructEncoder(),
        _pulseModulationTypeEnumEncoder(),
        _pumpFloodingTaskStructEncoder(),
        _rFModulationSystemTypeEnumEncoder(),
        _rFModulationTypeVariantStructEncoder(),
        _rTIobjectIdArrayEncoder(),
        _radioInputSourceEnumEncoder(),
        _radioTypeStructEncoder(),
        _receiverOperationalStatusEnumEncoder(),
        _recordSetStructArray1PlusEncoder(),
        _recordSetStructEncoder(),
        _recordStructArrayEncoder(),
        _recordStructEncoder(),
        _rectVol1GeomRecStructEncoder(),
        _rectVol2GeomRecStructEncoder(),
        _rectVol3GeomRecStructEncoder(),
        _referenceSystemEnumEncoder(),
        _refuelingStationLightColorEnumEncoder(),
        _refuelingStationLightDayNightEnumEncoder(),
        _relationEncoder(),
        _relativePositionStructEncoder(),
        _relativeRangeBearingStructEncoder(),
        _releaseDistributionStructEncoder(),
        _releaseDynamicsStructEncoder(),
        _releaseSizeStructEncoder(),
        _repairResultEnumEncoder(),
        _repairTaskStructEncoder(),
        _repairTypeEnumEncoder(),
        _requestStatusEnumEncoder(),
        _requestedConnectionArrayEncoder(),
        _requestedConnectionEncoder(),
        _resourceStatusStructEncoder(),
        _responseFlagEnumEncoder(),
        _resupplyTaskStructEncoder(),
        _roadIceConditionEnumEncoder(),
        _roundStructEncoder(),
        _rulesOfEngagementEnumEncoder(),
        _sINCGARSModulationStructEncoder(),
        _seaStateEnumEncoder(),
        _sedimentTypeEnumEncoder(),
        _sendSafetyRelatedBroadcastMessageTaskStructEncoder(),
        _sendSafetyRelatedMessageTaskStructEncoder(),
        _sensorStateEnumEncoder(),
        _sensorStructEncoder(),
        _serviceTypeEnumEncoder(),
        _setTransmitterStatusTaskStructEncoder(),
        _shaftDataStructEncoder(),
        _shaftDataStructLengthlessArray1PlusEncoder(),
        _shipTypeEnumEncoder(),
        _signalDataLengthlessArray1PlusEncoder(),
        _signatureTypeEnumEncoder(),
        _signatureVariantEncoder(),
        _silentAggregateStructEncoder(),
        _silentAggregateStructLengthlessArrayEncoder(),
        _silentEntityStructEncoder(),
        _silentEntityStructLengthlessArrayEncoder(),
        _snowStructEncoder(),
        _spatialFPStructEncoder(),
        _spatialFVStructEncoder(),
        _spatialRPStructEncoder(),
        _spatialRVStructEncoder(),
        _spatialStaticStructEncoder(),
        _spatialVariantStructEncoder(),
        _sphere1GeomRecStructEncoder(),
        _sphere2GeomRecStructEncoder(),
        _sphericalHarmonicAntennaStructEncoder(),
        _spreadSpectrumEnumEncoder(),
        _spreadSpectrumVariantStructEncoder(),
        _stanceCodeEnumEncoder(),
        _stationEnumEncoder(),
        _stationNameLocationVariantStructEncoder(),
        _stopFreezeReasonEnumEncoder(),
        _subsurfaceFormationTypeEnumEncoder(),
        _supplyStatusStructEncoder(),
        _supplyStructEncoder(),
        _supplyStructLengthlessArrayEncoder(),
        _surfaceFormationTypeEnumEncoder(),
        _surfaceMoistureEnumEncoder(),
        _symbolAmplificationVariantEncoder(),
        _symbolStructEncoder(),
        _symbolTypeEnumEncoder(),
        _tacticalDataLinkTypeEnumEncoder(),
        _taskDefinitionEncoder(),
        _taskDefinitionVariantRecordEncoder(),
        _taskModeEnumEncoder(),
        _taskProgressEncoder(),
        _taskProgressVariantRecordEncoder(),
        _taskStatusEnumEncoder(),
        _temperatureDegreeCelsiusFloat32LengthlessArrayEncoder(),
        _trackStructEncoder(),
        _trailingEffectsCodeEnumEncoder(),
        _transferTypeEnumEncoder(),
        _transmitterOperationalStatusEnumEncoder(),
        _transportTaskStructEncoder(),
        _treatmentStructEncoder(),
        _uUIDEncoder(),
        _uniformGeomRecStructEncoder(),
        _unitSymbolAmplificationStructEncoder(),
        _unmodulatedTypeEnumEncoder(),
        _unsignedInteger16Array1PlusEncoder(),
        _unsignedInteger32LengthlessArrayEncoder(),
        _unsignedInteger64Array1PlusEncoder(),
        _unsignedInteger8LengthlessArrayEncoder(),
        _userProtocolEnumEncoder(),
        _variableDatumStructArrayEncoder(),
        _variableDatumStructEncoder(),
        _variableDatumStructLengthlessArrayEncoder(),
        _vectoringNozzleSystemDataStructEncoder(),
        _vectoringNozzleSystemDataStructLengthlessArrayEncoder(),
        _velocityVectorStructEncoder(),
        _visibleSideLocationEnumEncoder(),
        _warheadTypeEnumEncoder(),
        _waveStructEncoder(),
        _waypointEncoder(),
        _weaponControlOrderEnumEncoder(),
        _weaponStateEnumEncoder(),
        _windStructEncoder(),
        _worldLocationStructEncoder(),
        _worldLocationStructLengthlessArrayEncoder()
// === AUTO-GENERATED BUILDER END ===
    {}
    Encoders::~Encoders()
    {
    }
//pragma region Auto-generated Functions
    // === AUTO-GENERATED FUNCTIONS BEGIN ===
    std::vector<char> Encoders::encodeATP45HazardAreaTypeEnum(const DevStudio::ATP45HazardAreaTypeEnum::ATP45HazardAreaTypeEnum& aTP45HazardAreaTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _aTP45HazardAreaTypeEnumEncoder.set(aTP45HazardAreaTypeEnum);
        buffer.reserve(_aTP45HazardAreaTypeEnumEncoder.getEncodedLength());
        _aTP45HazardAreaTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAccelerationVectorStruct(const DevStudio::AccelerationVectorStruct& accelerationVectorStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _accelerationVectorStructEncoder.set(accelerationVectorStruct);
        buffer.reserve(_accelerationVectorStructEncoder.getEncodedLength());
        _accelerationVectorStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAcknowledgeFlagEnum(const DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum& acknowledgeFlagEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _acknowledgeFlagEnumEncoder.set(acknowledgeFlagEnum);
        buffer.reserve(_acknowledgeFlagEnumEncoder.getEncodedLength());
        _acknowledgeFlagEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAcknowledgementProtocolEnum(const DevStudio::AcknowledgementProtocolEnum::AcknowledgementProtocolEnum& acknowledgementProtocolEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _acknowledgementProtocolEnumEncoder.set(acknowledgementProtocolEnum);
        buffer.reserve(_acknowledgementProtocolEnumEncoder.getEncodedLength());
        _acknowledgementProtocolEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeActionEnum(const DevStudio::ActionEnum::ActionEnum& actionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _actionEnumEncoder.set(actionEnum);
        buffer.reserve(_actionEnumEncoder.getEncodedLength());
        _actionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeActionResultEnum(const DevStudio::ActionResultEnum::ActionResultEnum& actionResultEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _actionResultEnumEncoder.set(actionResultEnum);
        buffer.reserve(_actionResultEnumEncoder.getEncodedLength());
        _actionResultEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeActiveSonarEnum(const DevStudio::ActiveSonarEnum::ActiveSonarEnum& activeSonarEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _activeSonarEnumEncoder.set(activeSonarEnum);
        buffer.reserve(_activeSonarEnumEncoder.getEncodedLength());
        _activeSonarEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeActiveSonarFunctionCodeEnum(const DevStudio::ActiveSonarFunctionCodeEnum::ActiveSonarFunctionCodeEnum& activeSonarFunctionCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _activeSonarFunctionCodeEnumEncoder.set(activeSonarFunctionCodeEnum);
        buffer.reserve(_activeSonarFunctionCodeEnumEncoder.getEncodedLength());
        _activeSonarFunctionCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeActiveSonarScanPatternEnum(const DevStudio::ActiveSonarScanPatternEnum::ActiveSonarScanPatternEnum& activeSonarScanPatternEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _activeSonarScanPatternEnumEncoder.set(activeSonarScanPatternEnum);
        buffer.reserve(_activeSonarScanPatternEnumEncoder.getEncodedLength());
        _activeSonarScanPatternEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeActiveStatusEnum(const DevStudio::ActiveStatusEnum::ActiveStatusEnum& activeStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _activeStatusEnumEncoder.set(activeStatusEnum);
        buffer.reserve(_activeStatusEnumEncoder.getEncodedLength());
        _activeStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAdministerTreatmentTaskStruct(const DevStudio::AdministerTreatmentTaskStruct& administerTreatmentTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _administerTreatmentTaskStructEncoder.set(administerTreatmentTaskStruct);
        buffer.reserve(_administerTreatmentTaskStructEncoder.getEncodedLength());
        _administerTreatmentTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAgentClassEnum(const DevStudio::AgentClassEnum::AgentClassEnum& agentClassEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _agentClassEnumEncoder.set(agentClassEnum);
        buffer.reserve(_agentClassEnumEncoder.getEncodedLength());
        _agentClassEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAgentConcentrationStruct(const DevStudio::AgentConcentrationStruct& agentConcentrationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _agentConcentrationStructEncoder.set(agentConcentrationStruct);
        buffer.reserve(_agentConcentrationStructEncoder.getEncodedLength());
        _agentConcentrationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAgentMassStruct(const DevStudio::AgentMassStruct& agentMassStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _agentMassStructEncoder.set(agentMassStruct);
        buffer.reserve(_agentMassStructEncoder.getEncodedLength());
        _agentMassStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAgentTypeEnum(const DevStudio::AgentTypeEnum::AgentTypeEnum& agentTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _agentTypeEnumEncoder.set(agentTypeEnum);
        buffer.reserve(_agentTypeEnumEncoder.getEncodedLength());
        _agentTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAggregateMarkingStruct(const DevStudio::AggregateMarkingStruct& aggregateMarkingStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _aggregateMarkingStructEncoder.set(aggregateMarkingStruct);
        buffer.reserve(_aggregateMarkingStructEncoder.getEncodedLength());
        _aggregateMarkingStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAggregateMissionEnum(const DevStudio::AggregateMissionEnum::AggregateMissionEnum& aggregateMissionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _aggregateMissionEnumEncoder.set(aggregateMissionEnum);
        buffer.reserve(_aggregateMissionEnumEncoder.getEncodedLength());
        _aggregateMissionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAggregateStateEnum(const DevStudio::AggregateStateEnum::AggregateStateEnum& aggregateStateEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _aggregateStateEnumEncoder.set(aggregateStateEnum);
        buffer.reserve(_aggregateStateEnumEncoder.getEncodedLength());
        _aggregateStateEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAggregateStateFormationEnum(const DevStudio::AggregateStateFormationEnum::AggregateStateFormationEnum& aggregateStateFormationEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _aggregateStateFormationEnumEncoder.set(aggregateStateFormationEnum);
        buffer.reserve(_aggregateStateFormationEnumEncoder.getEncodedLength());
        _aggregateStateFormationEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAidTypeEnum(const DevStudio::AidTypeEnum::AidTypeEnum& aidTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _aidTypeEnumEncoder.set(aidTypeEnum);
        buffer.reserve(_aidTypeEnumEncoder.getEncodedLength());
        _aidTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAirFormationTypeEnum(const DevStudio::AirFormationTypeEnum::AirFormationTypeEnum& airFormationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _airFormationTypeEnumEncoder.set(airFormationTypeEnum);
        buffer.reserve(_airFormationTypeEnumEncoder.getEncodedLength());
        _airFormationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAmplitudeAngleModulationTypeEnum(const DevStudio::AmplitudeAngleModulationTypeEnum::AmplitudeAngleModulationTypeEnum& amplitudeAngleModulationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _amplitudeAngleModulationTypeEnumEncoder.set(amplitudeAngleModulationTypeEnum);
        buffer.reserve(_amplitudeAngleModulationTypeEnumEncoder.getEncodedLength());
        _amplitudeAngleModulationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAmplitudeModulationTypeEnum(const DevStudio::AmplitudeModulationTypeEnum::AmplitudeModulationTypeEnum& amplitudeModulationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _amplitudeModulationTypeEnumEncoder.set(amplitudeModulationTypeEnum);
        buffer.reserve(_amplitudeModulationTypeEnumEncoder.getEncodedLength());
        _amplitudeModulationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAngleModulationTypeEnum(const DevStudio::AngleModulationTypeEnum::AngleModulationTypeEnum& angleModulationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _angleModulationTypeEnumEncoder.set(angleModulationTypeEnum);
        buffer.reserve(_angleModulationTypeEnumEncoder.getEncodedLength());
        _angleModulationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAngularVelocityVectorStruct(const DevStudio::AngularVelocityVectorStruct& angularVelocityVectorStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _angularVelocityVectorStructEncoder.set(angularVelocityVectorStruct);
        buffer.reserve(_angularVelocityVectorStructEncoder.getEncodedLength());
        _angularVelocityVectorStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAntennaPatternTypeEnum(const DevStudio::AntennaPatternTypeEnum::AntennaPatternTypeEnum& antennaPatternTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _antennaPatternTypeEnumEncoder.set(antennaPatternTypeEnum);
        buffer.reserve(_antennaPatternTypeEnumEncoder.getEncodedLength());
        _antennaPatternTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAntennaPatternVariantStruct(const DevStudio::AntennaPatternVariantStruct& antennaPatternVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _antennaPatternVariantStructEncoder.set(antennaPatternVariantStruct);
        buffer.reserve(_antennaPatternVariantStructEncoder.getEncodedLength());
        _antennaPatternVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAntennaPatternVariantStructLengthlessArray(const std::vector< DevStudio::AntennaPatternVariantStruct >& antennaPatternVariantStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _antennaPatternVariantStructLengthlessArrayEncoder.set(antennaPatternVariantStructLengthlessArray);
        buffer.reserve(_antennaPatternVariantStructLengthlessArrayEncoder.getEncodedLength());
        _antennaPatternVariantStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAntiCollisionLightFlashingEnum(const DevStudio::AntiCollisionLightFlashingEnum::AntiCollisionLightFlashingEnum& antiCollisionLightFlashingEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _antiCollisionLightFlashingEnumEncoder.set(antiCollisionLightFlashingEnum);
        buffer.reserve(_antiCollisionLightFlashingEnumEncoder.getEncodedLength());
        _antiCollisionLightFlashingEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAppearanceAntiCollisionDayNightEnum(const DevStudio::AppearanceAntiCollisionDayNightEnum::AppearanceAntiCollisionDayNightEnum& appearanceAntiCollisionDayNightEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _appearanceAntiCollisionDayNightEnumEncoder.set(appearanceAntiCollisionDayNightEnum);
        buffer.reserve(_appearanceAntiCollisionDayNightEnumEncoder.getEncodedLength());
        _appearanceAntiCollisionDayNightEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAppearanceNVGModeEnum(const DevStudio::AppearanceNVGModeEnum::AppearanceNVGModeEnum& appearanceNVGModeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _appearanceNVGModeEnumEncoder.set(appearanceNVGModeEnum);
        buffer.reserve(_appearanceNVGModeEnumEncoder.getEncodedLength());
        _appearanceNVGModeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeApplyIPETaskStruct(const DevStudio::ApplyIPETaskStruct& applyIPETaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _applyIPETaskStructEncoder.set(applyIPETaskStruct);
        buffer.reserve(_applyIPETaskStructEncoder.getEncodedLength());
        _applyIPETaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAppointmentStruct(const DevStudio::AppointmentStruct& appointmentStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _appointmentStructEncoder.set(appointmentStruct);
        buffer.reserve(_appointmentStructEncoder.getEncodedLength());
        _appointmentStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfAgentConcentrationStruct(const std::vector< DevStudio::AgentConcentrationStruct >& arrayOfAgentConcentrationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfAgentConcentrationStructEncoder.set(arrayOfAgentConcentrationStruct);
        buffer.reserve(_arrayOfAgentConcentrationStructEncoder.getEncodedLength());
        _arrayOfAgentConcentrationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfAgentMassStruct(const std::vector< DevStudio::AgentMassStruct >& arrayOfAgentMassStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfAgentMassStructEncoder.set(arrayOfAgentMassStruct);
        buffer.reserve(_arrayOfAgentMassStructEncoder.getEncodedLength());
        _arrayOfAgentMassStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfAgentTypeEnum(const std::vector< DevStudio::AgentTypeEnum::AgentTypeEnum >& arrayOfAgentTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfAgentTypeEnumEncoder.set(arrayOfAgentTypeEnum);
        buffer.reserve(_arrayOfAgentTypeEnumEncoder.getEncodedLength());
        _arrayOfAgentTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfCBRNExposureStruct(const std::vector< DevStudio::CBRNExposureStruct >& arrayOfCBRNExposureStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfCBRNExposureStructEncoder.set(arrayOfCBRNExposureStruct);
        buffer.reserve(_arrayOfCBRNExposureStructEncoder.getEncodedLength());
        _arrayOfCBRNExposureStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfCBRNSensorReadingStruct(const std::vector< DevStudio::CBRNSensorReadingStruct >& arrayOfCBRNSensorReadingStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfCBRNSensorReadingStructEncoder.set(arrayOfCBRNSensorReadingStruct);
        buffer.reserve(_arrayOfCBRNSensorReadingStructEncoder.getEncodedLength());
        _arrayOfCBRNSensorReadingStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfCommunicationNetworks(const std::vector< std::vector< char > >& arrayOfCommunicationNetworks)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfCommunicationNetworksEncoder.set(arrayOfCommunicationNetworks);
        buffer.reserve(_arrayOfCommunicationNetworksEncoder.getEncodedLength());
        _arrayOfCommunicationNetworksEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfContourStruct(const std::vector< DevStudio::ContourStruct >& arrayOfContourStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfContourStructEncoder.set(arrayOfContourStruct);
        buffer.reserve(_arrayOfContourStructEncoder.getEncodedLength());
        _arrayOfContourStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfDetectedEquipment(const std::vector< DevStudio::DetectedEquipment >& arrayOfDetectedEquipment)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfDetectedEquipmentEncoder.set(arrayOfDetectedEquipment);
        buffer.reserve(_arrayOfDetectedEquipmentEncoder.getEncodedLength());
        _arrayOfDetectedEquipmentEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfHoldings(const std::vector< DevStudio::Holding >& arrayOfHoldings)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfHoldingsEncoder.set(arrayOfHoldings);
        buffer.reserve(_arrayOfHoldingsEncoder.getEncodedLength());
        _arrayOfHoldingsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfObservedEquipment(const std::vector< DevStudio::ObservedEquipment >& arrayOfObservedEquipment)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfObservedEquipmentEncoder.set(arrayOfObservedEquipment);
        buffer.reserve(_arrayOfObservedEquipmentEncoder.getEncodedLength());
        _arrayOfObservedEquipmentEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfProtectionEffectivenessStruct(const std::vector< DevStudio::ProtectionEffectivenessStruct >& arrayOfProtectionEffectivenessStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfProtectionEffectivenessStructEncoder.set(arrayOfProtectionEffectivenessStruct);
        buffer.reserve(_arrayOfProtectionEffectivenessStructEncoder.getEncodedLength());
        _arrayOfProtectionEffectivenessStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfRelationship(const std::vector< DevStudio::Relation >& arrayOfRelationship)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfRelationshipEncoder.set(arrayOfRelationship);
        buffer.reserve(_arrayOfRelationshipEncoder.getEncodedLength());
        _arrayOfRelationshipEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfResourceStatus(const std::vector< DevStudio::ResourceStatusStruct >& arrayOfResourceStatus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfResourceStatusEncoder.set(arrayOfResourceStatus);
        buffer.reserve(_arrayOfResourceStatusEncoder.getEncodedLength());
        _arrayOfResourceStatusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfSensor(const std::vector< DevStudio::SensorStruct >& arrayOfSensor)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfSensorEncoder.set(arrayOfSensor);
        buffer.reserve(_arrayOfSensorEncoder.getEncodedLength());
        _arrayOfSensorEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfSigmas6(const std::vector< float >& arrayOfSigmas6)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfSigmas6Encoder.set(arrayOfSigmas6);
        buffer.reserve(_arrayOfSigmas6Encoder.getEncodedLength());
        _arrayOfSigmas6Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfSignature(const std::vector< DevStudio::SignatureVariant >& arrayOfSignature)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfSignatureEncoder.set(arrayOfSignature);
        buffer.reserve(_arrayOfSignatureEncoder.getEncodedLength());
        _arrayOfSignatureEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfSupplyStatus(const std::vector< DevStudio::SupplyStatusStruct >& arrayOfSupplyStatus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfSupplyStatusEncoder.set(arrayOfSupplyStatus);
        buffer.reserve(_arrayOfSupplyStatusEncoder.getEncodedLength());
        _arrayOfSupplyStatusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfTaskDefinitions(const std::vector< DevStudio::TaskDefinition >& arrayOfTaskDefinitions)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfTaskDefinitionsEncoder.set(arrayOfTaskDefinitions);
        buffer.reserve(_arrayOfTaskDefinitionsEncoder.getEncodedLength());
        _arrayOfTaskDefinitionsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfTaskProgress(const std::vector< DevStudio::TaskProgress >& arrayOfTaskProgress)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfTaskProgressEncoder.set(arrayOfTaskProgress);
        buffer.reserve(_arrayOfTaskProgressEncoder.getEncodedLength());
        _arrayOfTaskProgressEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfTrack(const std::vector< DevStudio::TrackStruct >& arrayOfTrack)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfTrackEncoder.set(arrayOfTrack);
        buffer.reserve(_arrayOfTrackEncoder.getEncodedLength());
        _arrayOfTrackEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfTreatmentStruct(const std::vector< DevStudio::TreatmentStruct >& arrayOfTreatmentStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfTreatmentStructEncoder.set(arrayOfTreatmentStruct);
        buffer.reserve(_arrayOfTreatmentStructEncoder.getEncodedLength());
        _arrayOfTreatmentStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfUuid(const std::vector< std::vector< char > >& arrayOfUuid)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfUuidEncoder.set(arrayOfUuid);
        buffer.reserve(_arrayOfUuidEncoder.getEncodedLength());
        _arrayOfUuidEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArrayOfWaypoints(const std::vector< DevStudio::Waypoint >& arrayOfWaypoints)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _arrayOfWaypointsEncoder.set(arrayOfWaypoints);
        buffer.reserve(_arrayOfWaypointsEncoder.getEncodedLength());
        _arrayOfWaypointsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArticulatedParameterStruct(const DevStudio::ArticulatedParameterStruct& articulatedParameterStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _articulatedParameterStructEncoder.set(articulatedParameterStruct);
        buffer.reserve(_articulatedParameterStructEncoder.getEncodedLength());
        _articulatedParameterStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArticulatedParameterStructLengthlessArray(const std::vector< DevStudio::ArticulatedParameterStruct >& articulatedParameterStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _articulatedParameterStructLengthlessArrayEncoder.set(articulatedParameterStructLengthlessArray);
        buffer.reserve(_articulatedParameterStructLengthlessArrayEncoder.getEncodedLength());
        _articulatedParameterStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArticulatedPartsStruct(const DevStudio::ArticulatedPartsStruct& articulatedPartsStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _articulatedPartsStructEncoder.set(articulatedPartsStruct);
        buffer.reserve(_articulatedPartsStructEncoder.getEncodedLength());
        _articulatedPartsStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArticulatedPartsTypeEnum(const DevStudio::ArticulatedPartsTypeEnum::ArticulatedPartsTypeEnum& articulatedPartsTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _articulatedPartsTypeEnumEncoder.set(articulatedPartsTypeEnum);
        buffer.reserve(_articulatedPartsTypeEnumEncoder.getEncodedLength());
        _articulatedPartsTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeArticulatedTypeMetricEnum(const DevStudio::ArticulatedTypeMetricEnum::ArticulatedTypeMetricEnum& articulatedTypeMetricEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _articulatedTypeMetricEnumEncoder.set(articulatedTypeMetricEnum);
        buffer.reserve(_articulatedTypeMetricEnumEncoder.getEncodedLength());
        _articulatedTypeMetricEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAttachTaskStruct(const DevStudio::AttachTaskStruct& attachTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _attachTaskStructEncoder.set(attachTaskStruct);
        buffer.reserve(_attachTaskStructEncoder.getEncodedLength());
        _attachTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAttachedPartsStruct(const DevStudio::AttachedPartsStruct& attachedPartsStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _attachedPartsStructEncoder.set(attachedPartsStruct);
        buffer.reserve(_attachedPartsStructEncoder.getEncodedLength());
        _attachedPartsStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAttackTypeEnum(const DevStudio::AttackTypeEnum::AttackTypeEnum& attackTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _attackTypeEnumEncoder.set(attackTypeEnum);
        buffer.reserve(_attackTypeEnumEncoder.getEncodedLength());
        _attackTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAttributeValuePairStruct(const DevStudio::AttributeValuePairStruct& attributeValuePairStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _attributeValuePairStructEncoder.set(attributeValuePairStruct);
        buffer.reserve(_attributeValuePairStructEncoder.getEncodedLength());
        _attributeValuePairStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAttributeValuePairStructArray1Plus(const std::vector< DevStudio::AttributeValuePairStruct >& attributeValuePairStructArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _attributeValuePairStructArray1PlusEncoder.set(attributeValuePairStructArray1Plus);
        buffer.reserve(_attributeValuePairStructArray1PlusEncoder.getEncodedLength());
        _attributeValuePairStructArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeAudioDataTypeStruct(const DevStudio::AudioDataTypeStruct& audioDataTypeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _audioDataTypeStructEncoder.set(audioDataTypeStruct);
        buffer.reserve(_audioDataTypeStructEncoder.getEncodedLength());
        _audioDataTypeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBeamAntennaStruct(const DevStudio::BeamAntennaStruct& beamAntennaStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _beamAntennaStructEncoder.set(beamAntennaStruct);
        buffer.reserve(_beamAntennaStructEncoder.getEncodedLength());
        _beamAntennaStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBeamFunctionCodeEnum(const DevStudio::BeamFunctionCodeEnum::BeamFunctionCodeEnum& beamFunctionCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _beamFunctionCodeEnumEncoder.set(beamFunctionCodeEnum);
        buffer.reserve(_beamFunctionCodeEnumEncoder.getEncodedLength());
        _beamFunctionCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBinArray(const std::vector< bool >& binArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _binArrayEncoder.set(binArray);
        buffer.reserve(_binArrayEncoder.getEncodedLength());
        _binArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBreachStruct(const DevStudio::BreachStruct& breachStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _breachStructEncoder.set(breachStruct);
        buffer.reserve(_breachStructEncoder.getEncodedLength());
        _breachStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBreachStructLengthlessArray(const std::vector< DevStudio::BreachStruct >& breachStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _breachStructLengthlessArrayEncoder.set(breachStructLengthlessArray);
        buffer.reserve(_breachStructLengthlessArrayEncoder.getEncodedLength());
        _breachStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBreachableSegmentStruct(const DevStudio::BreachableSegmentStruct& breachableSegmentStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _breachableSegmentStructEncoder.set(breachableSegmentStruct);
        buffer.reserve(_breachableSegmentStructEncoder.getEncodedLength());
        _breachableSegmentStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBreachableSegmentStructLengthlessArray(const std::vector< DevStudio::BreachableSegmentStruct >& breachableSegmentStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _breachableSegmentStructLengthlessArrayEncoder.set(breachableSegmentStructLengthlessArray);
        buffer.reserve(_breachableSegmentStructLengthlessArrayEncoder.getEncodedLength());
        _breachableSegmentStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBreachedStatusArray8(const std::vector< DevStudio::BreachedStatusEnum::BreachedStatusEnum >& breachedStatusArray8)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _breachedStatusArray8Encoder.set(breachedStatusArray8);
        buffer.reserve(_breachedStatusArray8Encoder.getEncodedLength());
        _breachedStatusArray8Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeBreachedStatusEnum(const DevStudio::BreachedStatusEnum::BreachedStatusEnum& breachedStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _breachedStatusEnumEncoder.set(breachedStatusEnum);
        buffer.reserve(_breachedStatusEnumEncoder.getEncodedLength());
        _breachedStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCBRNAlarmStruct(const DevStudio::CBRNAlarmStruct& cBRNAlarmStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cBRNAlarmStructEncoder.set(cBRNAlarmStruct);
        buffer.reserve(_cBRNAlarmStructEncoder.getEncodedLength());
        _cBRNAlarmStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCBRNDamageEnum(const DevStudio::CBRNDamageEnum::CBRNDamageEnum& cBRNDamageEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cBRNDamageEnumEncoder.set(cBRNDamageEnum);
        buffer.reserve(_cBRNDamageEnumEncoder.getEncodedLength());
        _cBRNDamageEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCBRNExposureStruct(const DevStudio::CBRNExposureStruct& cBRNExposureStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cBRNExposureStructEncoder.set(cBRNExposureStruct);
        buffer.reserve(_cBRNExposureStructEncoder.getEncodedLength());
        _cBRNExposureStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCBRNSensorReadingStruct(const DevStudio::CBRNSensorReadingStruct& cBRNSensorReadingStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cBRNSensorReadingStructEncoder.set(cBRNSensorReadingStruct);
        buffer.reserve(_cBRNSensorReadingStructEncoder.getEncodedLength());
        _cBRNSensorReadingStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCOMBICStateRecStruct(const DevStudio::COMBICStateRecStruct& cOMBICStateRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cOMBICStateRecStructEncoder.set(cOMBICStateRecStruct);
        buffer.reserve(_cOMBICStateRecStructEncoder.getEncodedLength());
        _cOMBICStateRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCamouflageEnum(const DevStudio::CamouflageEnum::CamouflageEnum& camouflageEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _camouflageEnumEncoder.set(camouflageEnum);
        buffer.reserve(_camouflageEnumEncoder.getEncodedLength());
        _camouflageEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCaptureStatusEnum(const DevStudio::CaptureStatusEnum::CaptureStatusEnum& captureStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _captureStatusEnumEncoder.set(captureStatusEnum);
        buffer.reserve(_captureStatusEnumEncoder.getEncodedLength());
        _captureStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeChangeAltitudeTaskStruct(const DevStudio::ChangeAltitudeTaskStruct& changeAltitudeTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _changeAltitudeTaskStructEncoder.set(changeAltitudeTaskStruct);
        buffer.reserve(_changeAltitudeTaskStructEncoder.getEncodedLength());
        _changeAltitudeTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeChangeHeadingTaskStruct(const DevStudio::ChangeHeadingTaskStruct& changeHeadingTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _changeHeadingTaskStructEncoder.set(changeHeadingTaskStruct);
        buffer.reserve(_changeHeadingTaskStructEncoder.getEncodedLength());
        _changeHeadingTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeChangeRulesOfEngagementTaskStruct(const DevStudio::ChangeRulesOfEngagementTaskStruct& changeRulesOfEngagementTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _changeRulesOfEngagementTaskStructEncoder.set(changeRulesOfEngagementTaskStruct);
        buffer.reserve(_changeRulesOfEngagementTaskStructEncoder.getEncodedLength());
        _changeRulesOfEngagementTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeChangeSpeedTaskStruct(const DevStudio::ChangeSpeedTaskStruct& changeSpeedTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _changeSpeedTaskStructEncoder.set(changeSpeedTaskStruct);
        buffer.reserve(_changeSpeedTaskStructEncoder.getEncodedLength());
        _changeSpeedTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeChemicalContentEnum(const DevStudio::ChemicalContentEnum::ChemicalContentEnum& chemicalContentEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _chemicalContentEnumEncoder.set(chemicalContentEnum);
        buffer.reserve(_chemicalContentEnumEncoder.getEncodedLength());
        _chemicalContentEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeClearEngineeringTaskStruct(const DevStudio::ClearEngineeringTaskStruct& clearEngineeringTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _clearEngineeringTaskStructEncoder.set(clearEngineeringTaskStruct);
        buffer.reserve(_clearEngineeringTaskStructEncoder.getEncodedLength());
        _clearEngineeringTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeClockTimeStruct(const DevStudio::ClockTimeStruct& clockTimeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _clockTimeStructEncoder.set(clockTimeStruct);
        buffer.reserve(_clockTimeStructEncoder.getEncodedLength());
        _clockTimeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeClockTimeStructLengthlessArray(const std::vector< DevStudio::ClockTimeStruct >& clockTimeStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _clockTimeStructLengthlessArrayEncoder.set(clockTimeStructLengthlessArray);
        buffer.reserve(_clockTimeStructLengthlessArrayEncoder.getEncodedLength());
        _clockTimeStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCloudStruct(const DevStudio::CloudStruct& cloudStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cloudStructEncoder.set(cloudStruct);
        buffer.reserve(_cloudStructEncoder.getEncodedLength());
        _cloudStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCloudTypeEnum(const DevStudio::CloudTypeEnum::CloudTypeEnum& cloudTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cloudTypeEnumEncoder.set(cloudTypeEnum);
        buffer.reserve(_cloudTypeEnumEncoder.getEncodedLength());
        _cloudTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCoefficientsLengthlessArray1Plus(const std::vector< float >& coefficientsLengthlessArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _coefficientsLengthlessArray1PlusEncoder.set(coefficientsLengthlessArray1Plus);
        buffer.reserve(_coefficientsLengthlessArray1PlusEncoder.getEncodedLength());
        _coefficientsLengthlessArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCollisionTypeEnum(const DevStudio::CollisionTypeEnum::CollisionTypeEnum& collisionTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _collisionTypeEnumEncoder.set(collisionTypeEnum);
        buffer.reserve(_collisionTypeEnumEncoder.getEncodedLength());
        _collisionTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCombinationModulationTypeEnum(const DevStudio::CombinationModulationTypeEnum::CombinationModulationTypeEnum& combinationModulationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _combinationModulationTypeEnumEncoder.set(combinationModulationTypeEnum);
        buffer.reserve(_combinationModulationTypeEnumEncoder.getEncodedLength());
        _combinationModulationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCommunicationNetworkTypeEnum(const DevStudio::CommunicationNetworkTypeEnum::CommunicationNetworkTypeEnum& communicationNetworkTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _communicationNetworkTypeEnumEncoder.set(communicationNetworkTypeEnum);
        buffer.reserve(_communicationNetworkTypeEnumEncoder.getEncodedLength());
        _communicationNetworkTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCommunicationServiceTypeEnum(const DevStudio::CommunicationServiceTypeEnum::CommunicationServiceTypeEnum& communicationServiceTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _communicationServiceTypeEnumEncoder.set(communicationServiceTypeEnum);
        buffer.reserve(_communicationServiceTypeEnumEncoder.getEncodedLength());
        _communicationServiceTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeComplianceStateEnum(const DevStudio::ComplianceStateEnum::ComplianceStateEnum& complianceStateEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _complianceStateEnumEncoder.set(complianceStateEnum);
        buffer.reserve(_complianceStateEnumEncoder.getEncodedLength());
        _complianceStateEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCone1GeomRecStruct(const DevStudio::Cone1GeomRecStruct& cone1GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cone1GeomRecStructEncoder.set(cone1GeomRecStruct);
        buffer.reserve(_cone1GeomRecStructEncoder.getEncodedLength());
        _cone1GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCone2GeomRecStruct(const DevStudio::Cone2GeomRecStruct& cone2GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cone2GeomRecStructEncoder.set(cone2GeomRecStruct);
        buffer.reserve(_cone2GeomRecStructEncoder.getEncodedLength());
        _cone2GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConnectionReceiverArray(const std::vector< DevStudio::ConnectionReceiverStruct >& connectionReceiverArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _connectionReceiverArrayEncoder.set(connectionReceiverArray);
        buffer.reserve(_connectionReceiverArrayEncoder.getEncodedLength());
        _connectionReceiverArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConnectionReceiverStruct(const DevStudio::ConnectionReceiverStruct& connectionReceiverStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _connectionReceiverStructEncoder.set(connectionReceiverStruct);
        buffer.reserve(_connectionReceiverStructEncoder.getEncodedLength());
        _connectionReceiverStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConnectionTypeEnum(const DevStudio::ConnectionTypeEnum::ConnectionTypeEnum& connectionTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _connectionTypeEnumEncoder.set(connectionTypeEnum);
        buffer.reserve(_connectionTypeEnumEncoder.getEncodedLength());
        _connectionTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConstituentPartNatureEnum(const DevStudio::ConstituentPartNatureEnum::ConstituentPartNatureEnum& constituentPartNatureEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _constituentPartNatureEnumEncoder.set(constituentPartNatureEnum);
        buffer.reserve(_constituentPartNatureEnumEncoder.getEncodedLength());
        _constituentPartNatureEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConstituentPartPositionEnum(const DevStudio::ConstituentPartPositionEnum::ConstituentPartPositionEnum& constituentPartPositionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _constituentPartPositionEnumEncoder.set(constituentPartPositionEnum);
        buffer.reserve(_constituentPartPositionEnumEncoder.getEncodedLength());
        _constituentPartPositionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConstituentPartRelationshipStruct(const DevStudio::ConstituentPartRelationshipStruct& constituentPartRelationshipStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _constituentPartRelationshipStructEncoder.set(constituentPartRelationshipStruct);
        buffer.reserve(_constituentPartRelationshipStructEncoder.getEncodedLength());
        _constituentPartRelationshipStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeConstituentPartStationNameEnum(const DevStudio::ConstituentPartStationNameEnum::ConstituentPartStationNameEnum& constituentPartStationNameEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _constituentPartStationNameEnumEncoder.set(constituentPartStationNameEnum);
        buffer.reserve(_constituentPartStationNameEnumEncoder.getEncodedLength());
        _constituentPartStationNameEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeContourStruct(const DevStudio::ContourStruct& contourStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _contourStructEncoder.set(contourStruct);
        buffer.reserve(_contourStructEncoder.getEncodedLength());
        _contourStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCreateBreachTaskStruct(const DevStudio::CreateBreachTaskStruct& createBreachTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _createBreachTaskStructEncoder.set(createBreachTaskStruct);
        buffer.reserve(_createBreachTaskStructEncoder.getEncodedLength());
        _createBreachTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCreateFireBreakTaskStruct(const DevStudio::CreateFireBreakTaskStruct& createFireBreakTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _createFireBreakTaskStructEncoder.set(createFireBreakTaskStruct);
        buffer.reserve(_createFireBreakTaskStructEncoder.getEncodedLength());
        _createFireBreakTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCreateLeveeTaskStruct(const DevStudio::CreateLeveeTaskStruct& createLeveeTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _createLeveeTaskStructEncoder.set(createLeveeTaskStruct);
        buffer.reserve(_createLeveeTaskStructEncoder.getEncodedLength());
        _createLeveeTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCreateObstacleTaskStruct(const DevStudio::CreateObstacleTaskStruct& createObstacleTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _createObstacleTaskStructEncoder.set(createObstacleTaskStruct);
        buffer.reserve(_createObstacleTaskStructEncoder.getEncodedLength());
        _createObstacleTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCryptographicModeEnum(const DevStudio::CryptographicModeEnum::CryptographicModeEnum& cryptographicModeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cryptographicModeEnumEncoder.set(cryptographicModeEnum);
        buffer.reserve(_cryptographicModeEnumEncoder.getEncodedLength());
        _cryptographicModeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCryptographicSystemTypeEnum(const DevStudio::CryptographicSystemTypeEnum::CryptographicSystemTypeEnum& cryptographicSystemTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _cryptographicSystemTypeEnumEncoder.set(cryptographicSystemTypeEnum);
        buffer.reserve(_cryptographicSystemTypeEnumEncoder.getEncodedLength());
        _cryptographicSystemTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeCurrentStruct(const DevStudio::CurrentStruct& currentStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _currentStructEncoder.set(currentStruct);
        buffer.reserve(_currentStructEncoder.getEncodedLength());
        _currentStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDamageStatusEnhancedEnum(const DevStudio::DamageStatusEnhancedEnum::DamageStatusEnhancedEnum& damageStatusEnhancedEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _damageStatusEnhancedEnumEncoder.set(damageStatusEnhancedEnum);
        buffer.reserve(_damageStatusEnhancedEnumEncoder.getEncodedLength());
        _damageStatusEnhancedEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDamageStatusEnum(const DevStudio::DamageStatusEnum::DamageStatusEnum& damageStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _damageStatusEnumEncoder.set(damageStatusEnum);
        buffer.reserve(_damageStatusEnumEncoder.getEncodedLength());
        _damageStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDatumIdentifierEnum(const DevStudio::DatumIdentifierEnum::DatumIdentifierEnum& datumIdentifierEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _datumIdentifierEnumEncoder.set(datumIdentifierEnum);
        buffer.reserve(_datumIdentifierEnumEncoder.getEncodedLength());
        _datumIdentifierEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDatumIdentifierLengthlessArray(const std::vector< DevStudio::DatumIdentifierEnum::DatumIdentifierEnum >& datumIdentifierLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _datumIdentifierLengthlessArrayEncoder.set(datumIdentifierLengthlessArray);
        buffer.reserve(_datumIdentifierLengthlessArrayEncoder.getEncodedLength());
        _datumIdentifierLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDeadReckoningAlgorithmEnum(const DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum& deadReckoningAlgorithmEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _deadReckoningAlgorithmEnumEncoder.set(deadReckoningAlgorithmEnum);
        buffer.reserve(_deadReckoningAlgorithmEnumEncoder.getEncodedLength());
        _deadReckoningAlgorithmEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDepthMeterFloat32LengthlessArray(const std::vector< float >& depthMeterFloat32LengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _depthMeterFloat32LengthlessArrayEncoder.set(depthMeterFloat32LengthlessArray);
        buffer.reserve(_depthMeterFloat32LengthlessArrayEncoder.getEncodedLength());
        _depthMeterFloat32LengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDesignatorCodeEnum(const DevStudio::DesignatorCodeEnum::DesignatorCodeEnum& designatorCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _designatorCodeEnumEncoder.set(designatorCodeEnum);
        buffer.reserve(_designatorCodeEnumEncoder.getEncodedLength());
        _designatorCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDesignatorCodeNameEnum(const DevStudio::DesignatorCodeNameEnum::DesignatorCodeNameEnum& designatorCodeNameEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _designatorCodeNameEnumEncoder.set(designatorCodeNameEnum);
        buffer.reserve(_designatorCodeNameEnumEncoder.getEncodedLength());
        _designatorCodeNameEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDetectedEquipment(const DevStudio::DetectedEquipment& detectedEquipment)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _detectedEquipmentEncoder.set(detectedEquipment);
        buffer.reserve(_detectedEquipmentEncoder.getEncodedLength());
        _detectedEquipmentEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDetonationResultCodeEnum(const DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum& detonationResultCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _detonationResultCodeEnumEncoder.set(detonationResultCodeEnum);
        buffer.reserve(_detonationResultCodeEnumEncoder.getEncodedLength());
        _detonationResultCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDimensionRateStruct(const DevStudio::DimensionRateStruct& dimensionRateStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _dimensionRateStructEncoder.set(dimensionRateStruct);
        buffer.reserve(_dimensionRateStructEncoder.getEncodedLength());
        _dimensionRateStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDimensionStruct(const DevStudio::DimensionStruct& dimensionStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _dimensionStructEncoder.set(dimensionStruct);
        buffer.reserve(_dimensionStructEncoder.getEncodedLength());
        _dimensionStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDirectFireTaskStruct(const DevStudio::DirectFireTaskStruct& directFireTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _directFireTaskStructEncoder.set(directFireTaskStruct);
        buffer.reserve(_directFireTaskStructEncoder.getEncodedLength());
        _directFireTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeDisruptCommunicationTaskStruct(const DevStudio::DisruptCommunicationTaskStruct& disruptCommunicationTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _disruptCommunicationTaskStructEncoder.set(disruptCommunicationTaskStruct);
        buffer.reserve(_disruptCommunicationTaskStructEncoder.getEncodedLength());
        _disruptCommunicationTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEchelonEnum(const DevStudio::EchelonEnum::EchelonEnum& echelonEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _echelonEnumEncoder.set(echelonEnum);
        buffer.reserve(_echelonEnumEncoder.getEncodedLength());
        _echelonEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeElapsedTimeProgress(const DevStudio::ElapsedTimeProgress& elapsedTimeProgress)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _elapsedTimeProgressEncoder.set(elapsedTimeProgress);
        buffer.reserve(_elapsedTimeProgressEncoder.getEncodedLength());
        _elapsedTimeProgressEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEllipsoid1GeomRecStruct(const DevStudio::Ellipsoid1GeomRecStruct& ellipsoid1GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _ellipsoid1GeomRecStructEncoder.set(ellipsoid1GeomRecStruct);
        buffer.reserve(_ellipsoid1GeomRecStructEncoder.getEncodedLength());
        _ellipsoid1GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEllipsoid2GeomRecStruct(const DevStudio::Ellipsoid2GeomRecStruct& ellipsoid2GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _ellipsoid2GeomRecStructEncoder.set(ellipsoid2GeomRecStruct);
        buffer.reserve(_ellipsoid2GeomRecStructEncoder.getEncodedLength());
        _ellipsoid2GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEmitterFunctionEnum(const DevStudio::EmitterFunctionEnum::EmitterFunctionEnum& emitterFunctionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _emitterFunctionEnumEncoder.set(emitterFunctionEnum);
        buffer.reserve(_emitterFunctionEnumEncoder.getEncodedLength());
        _emitterFunctionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEmitterTypeEnum(const DevStudio::EmitterTypeEnum::EmitterTypeEnum& emitterTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _emitterTypeEnumEncoder.set(emitterTypeEnum);
        buffer.reserve(_emitterTypeEnumEncoder.getEncodedLength());
        _emitterTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEmptyTaskStruct(const DevStudio::EmptyTaskStruct& emptyTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _emptyTaskStructEncoder.set(emptyTaskStruct);
        buffer.reserve(_emptyTaskStructEncoder.getEncodedLength());
        _emptyTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEncodingTypeEnum(const DevStudio::EncodingTypeEnum::EncodingTypeEnum& encodingTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _encodingTypeEnumEncoder.set(encodingTypeEnum);
        buffer.reserve(_encodingTypeEnumEncoder.getEncodedLength());
        _encodingTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEngineeringTaskStruct(const DevStudio::EngineeringTaskStruct& engineeringTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _engineeringTaskStructEncoder.set(engineeringTaskStruct);
        buffer.reserve(_engineeringTaskStructEncoder.getEncodedLength());
        _engineeringTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEntityControlActionEnum(const DevStudio::EntityControlActionEnum::EntityControlActionEnum& entityControlActionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _entityControlActionEnumEncoder.set(entityControlActionEnum);
        buffer.reserve(_entityControlActionEnumEncoder.getEncodedLength());
        _entityControlActionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEntityControlActions(const std::vector< DevStudio::EntityControlActionEnum::EntityControlActionEnum >& entityControlActions)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _entityControlActionsEncoder.set(entityControlActions);
        buffer.reserve(_entityControlActionsEncoder.getEncodedLength());
        _entityControlActionsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEntityCoordinateVectorStruct(const DevStudio::EntityCoordinateVectorStruct& entityCoordinateVectorStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _entityCoordinateVectorStructEncoder.set(entityCoordinateVectorStruct);
        buffer.reserve(_entityCoordinateVectorStructEncoder.getEncodedLength());
        _entityCoordinateVectorStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEntityIdentifierStruct(const DevStudio::EntityIdentifierStruct& entityIdentifierStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _entityIdentifierStructEncoder.set(entityIdentifierStruct);
        buffer.reserve(_entityIdentifierStructEncoder.getEncodedLength());
        _entityIdentifierStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEntityTypeStruct(const DevStudio::EntityTypeStruct& entityTypeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _entityTypeStructEncoder.set(entityTypeStruct);
        buffer.reserve(_entityTypeStructEncoder.getEncodedLength());
        _entityTypeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEntityTypeStructLengthlessArray(const std::vector< DevStudio::EntityTypeStruct >& entityTypeStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _entityTypeStructLengthlessArrayEncoder.set(entityTypeStructLengthlessArray);
        buffer.reserve(_entityTypeStructLengthlessArrayEncoder.getEncodedLength());
        _entityTypeStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentConditionModelTypeEnum(const DevStudio::EnvironmentConditionModelTypeEnum::EnvironmentConditionModelTypeEnum& environmentConditionModelTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentConditionModelTypeEnumEncoder.set(environmentConditionModelTypeEnum);
        buffer.reserve(_environmentConditionModelTypeEnumEncoder.getEncodedLength());
        _environmentConditionModelTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentDataCoordinateSystemEnum(const DevStudio::EnvironmentDataCoordinateSystemEnum::EnvironmentDataCoordinateSystemEnum& environmentDataCoordinateSystemEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentDataCoordinateSystemEnumEncoder.set(environmentDataCoordinateSystemEnum);
        buffer.reserve(_environmentDataCoordinateSystemEnumEncoder.getEncodedLength());
        _environmentDataCoordinateSystemEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentDataRepresentationEnum(const DevStudio::EnvironmentDataRepresentationEnum::EnvironmentDataRepresentationEnum& environmentDataRepresentationEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentDataRepresentationEnumEncoder.set(environmentDataRepresentationEnum);
        buffer.reserve(_environmentDataRepresentationEnumEncoder.getEncodedLength());
        _environmentDataRepresentationEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentDataSampleTypeEnum(const DevStudio::EnvironmentDataSampleTypeEnum::EnvironmentDataSampleTypeEnum& environmentDataSampleTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentDataSampleTypeEnumEncoder.set(environmentDataSampleTypeEnum);
        buffer.reserve(_environmentDataSampleTypeEnumEncoder.getEncodedLength());
        _environmentDataSampleTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentGridAxisTypeEnum(const DevStudio::EnvironmentGridAxisTypeEnum::EnvironmentGridAxisTypeEnum& environmentGridAxisTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentGridAxisTypeEnumEncoder.set(environmentGridAxisTypeEnum);
        buffer.reserve(_environmentGridAxisTypeEnumEncoder.getEncodedLength());
        _environmentGridAxisTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentGridTypeEnum(const DevStudio::EnvironmentGridTypeEnum::EnvironmentGridTypeEnum& environmentGridTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentGridTypeEnumEncoder.set(environmentGridTypeEnum);
        buffer.reserve(_environmentGridTypeEnumEncoder.getEncodedLength());
        _environmentGridTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentModelTypeEnum(const DevStudio::EnvironmentModelTypeEnum::EnvironmentModelTypeEnum& environmentModelTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentModelTypeEnumEncoder.set(environmentModelTypeEnum);
        buffer.reserve(_environmentModelTypeEnumEncoder.getEncodedLength());
        _environmentModelTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentObjectTypeStruct(const DevStudio::EnvironmentObjectTypeStruct& environmentObjectTypeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentObjectTypeStructEncoder.set(environmentObjectTypeStruct);
        buffer.reserve(_environmentObjectTypeStructEncoder.getEncodedLength());
        _environmentObjectTypeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentRecStruct(const DevStudio::EnvironmentRecStruct& environmentRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentRecStructEncoder.set(environmentRecStruct);
        buffer.reserve(_environmentRecStructEncoder.getEncodedLength());
        _environmentRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentRecStructArray(const std::vector< DevStudio::EnvironmentRecStruct >& environmentRecStructArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentRecStructArrayEncoder.set(environmentRecStructArray);
        buffer.reserve(_environmentRecStructArrayEncoder.getEncodedLength());
        _environmentRecStructArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentRecVariantStruct(const DevStudio::EnvironmentRecVariantStruct& environmentRecVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentRecVariantStructEncoder.set(environmentRecVariantStruct);
        buffer.reserve(_environmentRecVariantStructEncoder.getEncodedLength());
        _environmentRecVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentRecordTypeEnum(const DevStudio::EnvironmentRecordTypeEnum::EnvironmentRecordTypeEnum& environmentRecordTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentRecordTypeEnumEncoder.set(environmentRecordTypeEnum);
        buffer.reserve(_environmentRecordTypeEnumEncoder.getEncodedLength());
        _environmentRecordTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEnvironmentTypeStruct(const DevStudio::EnvironmentTypeStruct& environmentTypeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _environmentTypeStructEncoder.set(environmentTypeStruct);
        buffer.reserve(_environmentTypeStructEncoder.getEncodedLength());
        _environmentTypeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEquipmentSymbolAmplificationStruct(const DevStudio::EquipmentSymbolAmplificationStruct& equipmentSymbolAmplificationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _equipmentSymbolAmplificationStructEncoder.set(equipmentSymbolAmplificationStruct);
        buffer.reserve(_equipmentSymbolAmplificationStructEncoder.getEncodedLength());
        _equipmentSymbolAmplificationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEstablishCheckpointTaskStruct(const DevStudio::EstablishCheckpointTaskStruct& establishCheckpointTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _establishCheckpointTaskStructEncoder.set(establishCheckpointTaskStruct);
        buffer.reserve(_establishCheckpointTaskStructEncoder.getEncodedLength());
        _establishCheckpointTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEstablishObservationPostTaskStruct(const DevStudio::EstablishObservationPostTaskStruct& establishObservationPostTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _establishObservationPostTaskStructEncoder.set(establishObservationPostTaskStruct);
        buffer.reserve(_establishObservationPostTaskStructEncoder.getEncodedLength());
        _establishObservationPostTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEvacuateProgressStruct(const DevStudio::EvacuateProgressStruct& evacuateProgressStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _evacuateProgressStructEncoder.set(evacuateProgressStruct);
        buffer.reserve(_evacuateProgressStructEncoder.getEncodedLength());
        _evacuateProgressStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEvacuateTaskStruct(const DevStudio::EvacuateTaskStruct& evacuateTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _evacuateTaskStructEncoder.set(evacuateTaskStruct);
        buffer.reserve(_evacuateTaskStructEncoder.getEncodedLength());
        _evacuateTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEventIdentifierStruct(const DevStudio::EventIdentifierStruct& eventIdentifierStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _eventIdentifierStructEncoder.set(eventIdentifierStruct);
        buffer.reserve(_eventIdentifierStructEncoder.getEncodedLength());
        _eventIdentifierStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeEventTypeEnum(const DevStudio::EventTypeEnum::EventTypeEnum& eventTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _eventTypeEnumEncoder.set(eventTypeEnum);
        buffer.reserve(_eventTypeEnumEncoder.getEncodedLength());
        _eventTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeExhaustSmokeStruct(const DevStudio::ExhaustSmokeStruct& exhaustSmokeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _exhaustSmokeStructEncoder.set(exhaustSmokeStruct);
        buffer.reserve(_exhaustSmokeStructEncoder.getEncodedLength());
        _exhaustSmokeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeExhaustSmokeStructLengthlessArray(const std::vector< DevStudio::ExhaustSmokeStruct >& exhaustSmokeStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _exhaustSmokeStructLengthlessArrayEncoder.set(exhaustSmokeStructLengthlessArray);
        buffer.reserve(_exhaustSmokeStructLengthlessArrayEncoder.getEncodedLength());
        _exhaustSmokeStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeExposureTypeEnum(const DevStudio::ExposureTypeEnum::ExposureTypeEnum& exposureTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _exposureTypeEnumEncoder.set(exposureTypeEnum);
        buffer.reserve(_exposureTypeEnumEncoder.getEncodedLength());
        _exposureTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeExtinguishWildfireTaskStruct(const DevStudio::ExtinguishWildfireTaskStruct& extinguishWildfireTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _extinguishWildfireTaskStructEncoder.set(extinguishWildfireTaskStruct);
        buffer.reserve(_extinguishWildfireTaskStructEncoder.getEncodedLength());
        _extinguishWildfireTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFederateControlActionEnum(const DevStudio::FederateControlActionEnum::FederateControlActionEnum& federateControlActionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _federateControlActionEnumEncoder.set(federateControlActionEnum);
        buffer.reserve(_federateControlActionEnumEncoder.getEncodedLength());
        _federateControlActionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFederateControlActions(const std::vector< DevStudio::FederateControlActionEnum::FederateControlActionEnum >& federateControlActions)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _federateControlActionsEncoder.set(federateControlActions);
        buffer.reserve(_federateControlActionsEncoder.getEncodedLength());
        _federateControlActionsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFederateIdentifierStruct(const DevStudio::FederateIdentifierStruct& federateIdentifierStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _federateIdentifierStructEncoder.set(federateIdentifierStruct);
        buffer.reserve(_federateIdentifierStructEncoder.getEncodedLength());
        _federateIdentifierStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFireTaskProgress(const DevStudio::FireTaskProgress& fireTaskProgress)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _fireTaskProgressEncoder.set(fireTaskProgress);
        buffer.reserve(_fireTaskProgressEncoder.getEncodedLength());
        _fireTaskProgressEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFixedDatumStruct(const DevStudio::FixedDatumStruct& fixedDatumStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _fixedDatumStructEncoder.set(fixedDatumStruct);
        buffer.reserve(_fixedDatumStructEncoder.getEncodedLength());
        _fixedDatumStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFixedDatumStructLengthlessArray(const std::vector< DevStudio::FixedDatumStruct >& fixedDatumStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _fixedDatumStructLengthlessArrayEncoder.set(fixedDatumStructLengthlessArray);
        buffer.reserve(_fixedDatumStructLengthlessArrayEncoder.getEncodedLength());
        _fixedDatumStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFlareStateRecStruct(const DevStudio::FlareStateRecStruct& flareStateRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _flareStateRecStructEncoder.set(flareStateRecStruct);
        buffer.reserve(_flareStateRecStructEncoder.getEncodedLength());
        _flareStateRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFloat32Array1Plus(const std::vector< float >& float32Array1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _float32Array1PlusEncoder.set(float32Array1Plus);
        buffer.reserve(_float32Array1PlusEncoder.getEncodedLength());
        _float32Array1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFollowEntityTaskStruct(const DevStudio::FollowEntityTaskStruct& followEntityTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _followEntityTaskStructEncoder.set(followEntityTaskStruct);
        buffer.reserve(_followEntityTaskStructEncoder.getEncodedLength());
        _followEntityTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeForceIdentifierEnum(const DevStudio::ForceIdentifierEnum::ForceIdentifierEnum& forceIdentifierEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _forceIdentifierEnumEncoder.set(forceIdentifierEnum);
        buffer.reserve(_forceIdentifierEnumEncoder.getEncodedLength());
        _forceIdentifierEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFormationDataStruct(const DevStudio::FormationDataStruct& formationDataStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _formationDataStructEncoder.set(formationDataStruct);
        buffer.reserve(_formationDataStructEncoder.getEncodedLength());
        _formationDataStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFormationEnum(const DevStudio::FormationEnum::FormationEnum& formationEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _formationEnumEncoder.set(formationEnum);
        buffer.reserve(_formationEnumEncoder.getEncodedLength());
        _formationEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFormationLocationTypeEnum(const DevStudio::FormationLocationTypeEnum::FormationLocationTypeEnum& formationLocationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _formationLocationTypeEnumEncoder.set(formationLocationTypeEnum);
        buffer.reserve(_formationLocationTypeEnumEncoder.getEncodedLength());
        _formationLocationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFormationStruct(const DevStudio::FormationStruct& formationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _formationStructEncoder.set(formationStruct);
        buffer.reserve(_formationStructEncoder.getEncodedLength());
        _formationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFormationTypeEnum(const DevStudio::FormationTypeEnum::FormationTypeEnum& formationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _formationTypeEnumEncoder.set(formationTypeEnum);
        buffer.reserve(_formationTypeEnumEncoder.getEncodedLength());
        _formationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFundamentalParameterDataStruct(const DevStudio::FundamentalParameterDataStruct& fundamentalParameterDataStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _fundamentalParameterDataStructEncoder.set(fundamentalParameterDataStruct);
        buffer.reserve(_fundamentalParameterDataStructEncoder.getEncodedLength());
        _fundamentalParameterDataStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFundamentalParameterDataStructLengthlessArray(const std::vector< DevStudio::FundamentalParameterDataStruct >& fundamentalParameterDataStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _fundamentalParameterDataStructLengthlessArrayEncoder.set(fundamentalParameterDataStructLengthlessArray);
        buffer.reserve(_fundamentalParameterDataStructLengthlessArrayEncoder.getEncodedLength());
        _fundamentalParameterDataStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeFuseTypeEnum(const DevStudio::FuseTypeEnum::FuseTypeEnum& fuseTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _fuseTypeEnumEncoder.set(fuseTypeEnum);
        buffer.reserve(_fuseTypeEnumEncoder.getEncodedLength());
        _fuseTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGaussPlumeGeomRecStruct(const DevStudio::GaussPlumeGeomRecStruct& gaussPlumeGeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gaussPlumeGeomRecStructEncoder.set(gaussPlumeGeomRecStruct);
        buffer.reserve(_gaussPlumeGeomRecStructEncoder.getEncodedLength());
        _gaussPlumeGeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGaussPuffGeomRecStruct(const DevStudio::GaussPuffGeomRecStruct& gaussPuffGeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gaussPuffGeomRecStructEncoder.set(gaussPuffGeomRecStruct);
        buffer.reserve(_gaussPuffGeomRecStructEncoder.getEncodedLength());
        _gaussPuffGeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeoLocationTypeEnum(const DevStudio::GeoLocationTypeEnum::GeoLocationTypeEnum& geoLocationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geoLocationTypeEnumEncoder.set(geoLocationTypeEnum);
        buffer.reserve(_geoLocationTypeEnumEncoder.getEncodedLength());
        _geoLocationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeoReferenceVariant(const DevStudio::GeoReferenceVariant& geoReferenceVariant)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geoReferenceVariantEncoder.set(geoReferenceVariant);
        buffer.reserve(_geoReferenceVariantEncoder.getEncodedLength());
        _geoReferenceVariantEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeodeticCircle(const DevStudio::GeodeticCircle& geodeticCircle)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geodeticCircleEncoder.set(geodeticCircle);
        buffer.reserve(_geodeticCircleEncoder.getEncodedLength());
        _geodeticCircleEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeodeticLocation(const DevStudio::GeodeticLocation& geodeticLocation)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geodeticLocationEncoder.set(geodeticLocation);
        buffer.reserve(_geodeticLocationEncoder.getEncodedLength());
        _geodeticLocationEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeodeticPath(const std::vector< DevStudio::GeodeticPoint >& geodeticPath)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geodeticPathEncoder.set(geodeticPath);
        buffer.reserve(_geodeticPathEncoder.getEncodedLength());
        _geodeticPathEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeodeticPoint(const DevStudio::GeodeticPoint& geodeticPoint)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geodeticPointEncoder.set(geodeticPoint);
        buffer.reserve(_geodeticPointEncoder.getEncodedLength());
        _geodeticPointEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeodeticPolygon(const std::vector< DevStudio::GeodeticLocation >& geodeticPolygon)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geodeticPolygonEncoder.set(geodeticPolygon);
        buffer.reserve(_geodeticPolygonEncoder.getEncodedLength());
        _geodeticPolygonEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGeodeticQuadrangle(const DevStudio::GeodeticQuadrangle& geodeticQuadrangle)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _geodeticQuadrangleEncoder.set(geodeticQuadrangle);
        buffer.reserve(_geodeticQuadrangleEncoder.getEncodedLength());
        _geodeticQuadrangleEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridAxisStruct(const DevStudio::GridAxisStruct& gridAxisStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridAxisStructEncoder.set(gridAxisStruct);
        buffer.reserve(_gridAxisStructEncoder.getEncodedLength());
        _gridAxisStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridAxisStructLengthlessArray(const std::vector< DevStudio::GridAxisStruct >& gridAxisStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridAxisStructLengthlessArrayEncoder.set(gridAxisStructLengthlessArray);
        buffer.reserve(_gridAxisStructLengthlessArrayEncoder.getEncodedLength());
        _gridAxisStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridAxisTypeVariantStruct(const DevStudio::GridAxisTypeVariantStruct& gridAxisTypeVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridAxisTypeVariantStructEncoder.set(gridAxisTypeVariantStruct);
        buffer.reserve(_gridAxisTypeVariantStructEncoder.getEncodedLength());
        _gridAxisTypeVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridDataRepresentationVariantStruct(const DevStudio::GridDataRepresentationVariantStruct& gridDataRepresentationVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridDataRepresentationVariantStructEncoder.set(gridDataRepresentationVariantStruct);
        buffer.reserve(_gridDataRepresentationVariantStructEncoder.getEncodedLength());
        _gridDataRepresentationVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridDataStruct(const DevStudio::GridDataStruct& gridDataStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridDataStructEncoder.set(gridDataStruct);
        buffer.reserve(_gridDataStructEncoder.getEncodedLength());
        _gridDataStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridDataStructLengthlessArray(const std::vector< DevStudio::GridDataStruct >& gridDataStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridDataStructLengthlessArrayEncoder.set(gridDataStructLengthlessArray);
        buffer.reserve(_gridDataStructLengthlessArrayEncoder.getEncodedLength());
        _gridDataStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridValueType0Struct(const DevStudio::GridValueType0Struct& gridValueType0Struct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridValueType0StructEncoder.set(gridValueType0Struct);
        buffer.reserve(_gridValueType0StructEncoder.getEncodedLength());
        _gridValueType0StructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridValueType1Struct(const DevStudio::GridValueType1Struct& gridValueType1Struct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridValueType1StructEncoder.set(gridValueType1Struct);
        buffer.reserve(_gridValueType1StructEncoder.getEncodedLength());
        _gridValueType1StructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGridValueType2Struct(const DevStudio::GridValueType2Struct& gridValueType2Struct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _gridValueType2StructEncoder.set(gridValueType2Struct);
        buffer.reserve(_gridValueType2StructEncoder.getEncodedLength());
        _gridValueType2StructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeGroundFormationTypeEnum(const DevStudio::GroundFormationTypeEnum::GroundFormationTypeEnum& groundFormationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _groundFormationTypeEnumEncoder.set(groundFormationTypeEnum);
        buffer.reserve(_groundFormationTypeEnumEncoder.getEncodedLength());
        _groundFormationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAargumentList(const std::vector< std::wstring >& hLAargumentList)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAargumentListEncoder.set(hLAargumentList);
        buffer.reserve(_hLAargumentListEncoder.getEncodedLength());
        _hLAargumentListEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAfederateState(const DevStudio::HLAfederateState::HLAfederateState& hLAfederateState)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAfederateStateEncoder.set(hLAfederateState);
        buffer.reserve(_hLAfederateStateEncoder.getEncodedLength());
        _hLAfederateStateEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAhandleList(const std::vector< std::vector<char > >& hLAhandleList)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAhandleListEncoder.set(hLAhandleList);
        buffer.reserve(_hLAhandleListEncoder.getEncodedLength());
        _hLAhandleListEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAinteractionCount(const DevStudio::HLAinteractionCount& hLAinteractionCount)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAinteractionCountEncoder.set(hLAinteractionCount);
        buffer.reserve(_hLAinteractionCountEncoder.getEncodedLength());
        _hLAinteractionCountEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAinteractionCounts(const std::vector< DevStudio::HLAinteractionCount >& hLAinteractionCounts)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAinteractionCountsEncoder.set(hLAinteractionCounts);
        buffer.reserve(_hLAinteractionCountsEncoder.getEncodedLength());
        _hLAinteractionCountsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAinteractionSubList(const std::vector< DevStudio::HLAinteractionSubscription >& hLAinteractionSubList)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAinteractionSubListEncoder.set(hLAinteractionSubList);
        buffer.reserve(_hLAinteractionSubListEncoder.getEncodedLength());
        _hLAinteractionSubListEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAinteractionSubscription(const DevStudio::HLAinteractionSubscription& hLAinteractionSubscription)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAinteractionSubscriptionEncoder.set(hLAinteractionSubscription);
        buffer.reserve(_hLAinteractionSubscriptionEncoder.getEncodedLength());
        _hLAinteractionSubscriptionEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAmoduleDesignatorList(const std::vector< std::wstring >& hLAmoduleDesignatorList)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAmoduleDesignatorListEncoder.set(hLAmoduleDesignatorList);
        buffer.reserve(_hLAmoduleDesignatorListEncoder.getEncodedLength());
        _hLAmoduleDesignatorListEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAobjectClassBasedCount(const DevStudio::HLAobjectClassBasedCount& hLAobjectClassBasedCount)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAobjectClassBasedCountEncoder.set(hLAobjectClassBasedCount);
        buffer.reserve(_hLAobjectClassBasedCountEncoder.getEncodedLength());
        _hLAobjectClassBasedCountEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAobjectClassBasedCounts(const std::vector< DevStudio::HLAobjectClassBasedCount >& hLAobjectClassBasedCounts)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAobjectClassBasedCountsEncoder.set(hLAobjectClassBasedCounts);
        buffer.reserve(_hLAobjectClassBasedCountsEncoder.getEncodedLength());
        _hLAobjectClassBasedCountsEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAorderType(const DevStudio::HLAorderType::HLAorderType& hLAorderType)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAorderTypeEncoder.set(hLAorderType);
        buffer.reserve(_hLAorderTypeEncoder.getEncodedLength());
        _hLAorderTypeEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAownership(const DevStudio::HLAownership::HLAownership& hLAownership)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAownershipEncoder.set(hLAownership);
        buffer.reserve(_hLAownershipEncoder.getEncodedLength());
        _hLAownershipEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAresignAction(const DevStudio::HLAresignAction::HLAresignAction& hLAresignAction)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAresignActionEncoder.set(hLAresignAction);
        buffer.reserve(_hLAresignActionEncoder.getEncodedLength());
        _hLAresignActionEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAswitch(const DevStudio::HLAswitch::HLAswitch& hLAswitch)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAswitchEncoder.set(hLAswitch);
        buffer.reserve(_hLAswitchEncoder.getEncodedLength());
        _hLAswitchEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAsynchPointFederate(const DevStudio::HLAsynchPointFederate& hLAsynchPointFederate)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAsynchPointFederateEncoder.set(hLAsynchPointFederate);
        buffer.reserve(_hLAsynchPointFederateEncoder.getEncodedLength());
        _hLAsynchPointFederateEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAsynchPointFederateList(const std::vector< DevStudio::HLAsynchPointFederate >& hLAsynchPointFederateList)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAsynchPointFederateListEncoder.set(hLAsynchPointFederateList);
        buffer.reserve(_hLAsynchPointFederateListEncoder.getEncodedLength());
        _hLAsynchPointFederateListEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAsynchPointList(const std::vector< std::wstring >& hLAsynchPointList)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAsynchPointListEncoder.set(hLAsynchPointList);
        buffer.reserve(_hLAsynchPointListEncoder.getEncodedLength());
        _hLAsynchPointListEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAsynchPointStatus(const DevStudio::HLAsynchPointStatus::HLAsynchPointStatus& hLAsynchPointStatus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAsynchPointStatusEncoder.set(hLAsynchPointStatus);
        buffer.reserve(_hLAsynchPointStatusEncoder.getEncodedLength());
        _hLAsynchPointStatusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHLAtimeState(const DevStudio::HLAtimeState::HLAtimeState& hLAtimeState)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hLAtimeStateEncoder.set(hLAtimeState);
        buffer.reserve(_hLAtimeStateEncoder.getEncodedLength());
        _hLAtimeStateEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHatchStateEnum(const DevStudio::HatchStateEnum::HatchStateEnum& hatchStateEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hatchStateEnumEncoder.set(hatchStateEnum);
        buffer.reserve(_hatchStateEnumEncoder.getEncodedLength());
        _hatchStateEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHazardTypeEnum(const DevStudio::HazardTypeEnum::HazardTypeEnum& hazardTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hazardTypeEnumEncoder.set(hazardTypeEnum);
        buffer.reserve(_hazardTypeEnumEncoder.getEncodedLength());
        _hazardTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHazardTypeEnum2(const DevStudio::HazardTypeEnum2::HazardTypeEnum2& hazardTypeEnum2)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hazardTypeEnum2Encoder.set(hazardTypeEnum2);
        buffer.reserve(_hazardTypeEnum2Encoder.getEncodedLength());
        _hazardTypeEnum2Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHazeStruct(const DevStudio::HazeStruct& hazeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hazeStructEncoder.set(hazeStruct);
        buffer.reserve(_hazeStructEncoder.getEncodedLength());
        _hazeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHazeTypeEnum(const DevStudio::HazeTypeEnum::HazeTypeEnum& hazeTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hazeTypeEnumEncoder.set(hazeTypeEnum);
        buffer.reserve(_hazeTypeEnumEncoder.getEncodedLength());
        _hazeTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHolding(const DevStudio::Holding& holding)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _holdingEncoder.set(holding);
        buffer.reserve(_holdingEncoder.getEncodedLength());
        _holdingEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeHostilityStatusCodeEnum(const DevStudio::HostilityStatusCodeEnum::HostilityStatusCodeEnum& hostilityStatusCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _hostilityStatusCodeEnumEncoder.set(hostilityStatusCodeEnum);
        buffer.reserve(_hostilityStatusCodeEnumEncoder.getEncodedLength());
        _hostilityStatusCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIPETypeEnum(const DevStudio::IPETypeEnum::IPETypeEnum& iPETypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iPETypeEnumEncoder.set(iPETypeEnum);
        buffer.reserve(_iPETypeEnumEncoder.getEncodedLength());
        _iPETypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIceStruct(const DevStudio::IceStruct& iceStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iceStructEncoder.set(iceStruct);
        buffer.reserve(_iceStructEncoder.getEncodedLength());
        _iceStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIceTypeEnum(const DevStudio::IceTypeEnum::IceTypeEnum& iceTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iceTypeEnumEncoder.set(iceTypeEnum);
        buffer.reserve(_iceTypeEnumEncoder.getEncodedLength());
        _iceTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIdentificationLevelEnum(const DevStudio::IdentificationLevelEnum::IdentificationLevelEnum& identificationLevelEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _identificationLevelEnumEncoder.set(identificationLevelEnum);
        buffer.reserve(_identificationLevelEnumEncoder.getEncodedLength());
        _identificationLevelEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffAlternateMode4Enum(const DevStudio::IffAlternateMode4Enum::IffAlternateMode4Enum& iffAlternateMode4Enum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffAlternateMode4EnumEncoder.set(iffAlternateMode4Enum);
        buffer.reserve(_iffAlternateMode4EnumEncoder.getEncodedLength());
        _iffAlternateMode4EnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffApplicableModesEnum(const DevStudio::IffApplicableModesEnum::IffApplicableModesEnum& iffApplicableModesEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffApplicableModesEnumEncoder.set(iffApplicableModesEnum);
        buffer.reserve(_iffApplicableModesEnumEncoder.getEncodedLength());
        _iffApplicableModesEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffOperationalParameter1Enum(const DevStudio::IffOperationalParameter1Enum::IffOperationalParameter1Enum& iffOperationalParameter1Enum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffOperationalParameter1EnumEncoder.set(iffOperationalParameter1Enum);
        buffer.reserve(_iffOperationalParameter1EnumEncoder.getEncodedLength());
        _iffOperationalParameter1EnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffOperationalParameter2Enum(const DevStudio::IffOperationalParameter2Enum::IffOperationalParameter2Enum& iffOperationalParameter2Enum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffOperationalParameter2EnumEncoder.set(iffOperationalParameter2Enum);
        buffer.reserve(_iffOperationalParameter2EnumEncoder.getEncodedLength());
        _iffOperationalParameter2EnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffSystemModeEnum(const DevStudio::IffSystemModeEnum::IffSystemModeEnum& iffSystemModeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffSystemModeEnumEncoder.set(iffSystemModeEnum);
        buffer.reserve(_iffSystemModeEnumEncoder.getEncodedLength());
        _iffSystemModeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffSystemNameEnum(const DevStudio::IffSystemNameEnum::IffSystemNameEnum& iffSystemNameEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffSystemNameEnumEncoder.set(iffSystemNameEnum);
        buffer.reserve(_iffSystemNameEnumEncoder.getEncodedLength());
        _iffSystemNameEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIffSystemTypeEnum(const DevStudio::IffSystemTypeEnum::IffSystemTypeEnum& iffSystemTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _iffSystemTypeEnumEncoder.set(iffSystemTypeEnum);
        buffer.reserve(_iffSystemTypeEnumEncoder.getEncodedLength());
        _iffSystemTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIncomingConnectionArray(const std::vector< DevStudio::IncomingConnectionStruct >& incomingConnectionArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _incomingConnectionArrayEncoder.set(incomingConnectionArray);
        buffer.reserve(_incomingConnectionArrayEncoder.getEncodedLength());
        _incomingConnectionArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIncomingConnectionStruct(const DevStudio::IncomingConnectionStruct& incomingConnectionStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _incomingConnectionStructEncoder.set(incomingConnectionStruct);
        buffer.reserve(_incomingConnectionStructEncoder.getEncodedLength());
        _incomingConnectionStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIndirectFireTaskStruct(const DevStudio::IndirectFireTaskStruct& indirectFireTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _indirectFireTaskStructEncoder.set(indirectFireTaskStruct);
        buffer.reserve(_indirectFireTaskStructEncoder.getEncodedLength());
        _indirectFireTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeInstallationSymbolAmplificationStruct(const DevStudio::InstallationSymbolAmplificationStruct& installationSymbolAmplificationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _installationSymbolAmplificationStructEncoder.set(installationSymbolAmplificationStruct);
        buffer.reserve(_installationSymbolAmplificationStructEncoder.getEncodedLength());
        _installationSymbolAmplificationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeInteger16Array1Plus(const std::vector< short >& integer16Array1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _integer16Array1PlusEncoder.set(integer16Array1Plus);
        buffer.reserve(_integer16Array1PlusEncoder.getEncodedLength());
        _integer16Array1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIrregularGridAxisStruct(const DevStudio::IrregularGridAxisStruct& irregularGridAxisStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _irregularGridAxisStructEncoder.set(irregularGridAxisStruct);
        buffer.reserve(_irregularGridAxisStructEncoder.getEncodedLength());
        _irregularGridAxisStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeIsPartOfStruct(const DevStudio::IsPartOfStruct& isPartOfStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _isPartOfStructEncoder.set(isPartOfStruct);
        buffer.reserve(_isPartOfStructEncoder.getEncodedLength());
        _isPartOfStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLandslideTypeEnum(const DevStudio::LandslideTypeEnum::LandslideTypeEnum& landslideTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _landslideTypeEnumEncoder.set(landslideTypeEnum);
        buffer.reserve(_landslideTypeEnumEncoder.getEncodedLength());
        _landslideTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLayMinefieldTaskStruct(const DevStudio::LayMinefieldTaskStruct& layMinefieldTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _layMinefieldTaskStructEncoder.set(layMinefieldTaskStruct);
        buffer.reserve(_layMinefieldTaskStructEncoder.getEncodedLength());
        _layMinefieldTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLayerStruct(const DevStudio::LayerStruct& layerStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _layerStructEncoder.set(layerStruct);
        buffer.reserve(_layerStructEncoder.getEncodedLength());
        _layerStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLightningTypeEnum(const DevStudio::LightningTypeEnum::LightningTypeEnum& lightningTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _lightningTypeEnumEncoder.set(lightningTypeEnum);
        buffer.reserve(_lightningTypeEnumEncoder.getEncodedLength());
        _lightningTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLine1GeomRecStruct(const DevStudio::Line1GeomRecStruct& line1GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _line1GeomRecStructEncoder.set(line1GeomRecStruct);
        buffer.reserve(_line1GeomRecStructEncoder.getEncodedLength());
        _line1GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLine2GeomRecStruct(const DevStudio::Line2GeomRecStruct& line2GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _line2GeomRecStructEncoder.set(line2GeomRecStruct);
        buffer.reserve(_line2GeomRecStructEncoder.getEncodedLength());
        _line2GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLinearSegmentStruct(const DevStudio::LinearSegmentStruct& linearSegmentStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _linearSegmentStructEncoder.set(linearSegmentStruct);
        buffer.reserve(_linearSegmentStructEncoder.getEncodedLength());
        _linearSegmentStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLinkStatusArray(const std::vector< DevStudio::LinkStatusStruct >& linkStatusArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _linkStatusArrayEncoder.set(linkStatusArray);
        buffer.reserve(_linkStatusArrayEncoder.getEncodedLength());
        _linkStatusArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLinkStatusStruct(const DevStudio::LinkStatusStruct& linkStatusStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _linkStatusStructEncoder.set(linkStatusStruct);
        buffer.reserve(_linkStatusStructEncoder.getEncodedLength());
        _linkStatusStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLocationStruct(const DevStudio::LocationStruct& locationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _locationStructEncoder.set(locationStruct);
        buffer.reserve(_locationStructEncoder.getEncodedLength());
        _locationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeLocationStructArray(const std::vector< DevStudio::LocationStruct >& locationStructArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _locationStructArrayEncoder.set(locationStructArray);
        buffer.reserve(_locationStructArrayEncoder.getEncodedLength());
        _locationStructArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMagicMoveTaskStruct(const DevStudio::MagicMoveTaskStruct& magicMoveTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _magicMoveTaskStructEncoder.set(magicMoveTaskStruct);
        buffer.reserve(_magicMoveTaskStructEncoder.getEncodedLength());
        _magicMoveTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMajorRFModulationTypeEnum(const DevStudio::MajorRFModulationTypeEnum::MajorRFModulationTypeEnum& majorRFModulationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _majorRFModulationTypeEnumEncoder.set(majorRFModulationTypeEnum);
        buffer.reserve(_majorRFModulationTypeEnumEncoder.getEncodedLength());
        _majorRFModulationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeManeuverIndicatorEnum(const DevStudio::ManeuverIndicatorEnum::ManeuverIndicatorEnum& maneuverIndicatorEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _maneuverIndicatorEnumEncoder.set(maneuverIndicatorEnum);
        buffer.reserve(_maneuverIndicatorEnumEncoder.getEncodedLength());
        _maneuverIndicatorEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeManufacturerIdArray3(const std::vector< char >& manufacturerIdArray3)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _manufacturerIdArray3Encoder.set(manufacturerIdArray3);
        buffer.reserve(_manufacturerIdArray3Encoder.getEncodedLength());
        _manufacturerIdArray3Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMarkingArray11(const std::vector< char >& markingArray11)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _markingArray11Encoder.set(markingArray11);
        buffer.reserve(_markingArray11Encoder.getEncodedLength());
        _markingArray11Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMarkingArray31(const std::vector< char >& markingArray31)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _markingArray31Encoder.set(markingArray31);
        buffer.reserve(_markingArray31Encoder.getEncodedLength());
        _markingArray31Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMarkingEncodingEnum(const DevStudio::MarkingEncodingEnum::MarkingEncodingEnum& markingEncodingEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _markingEncodingEnumEncoder.set(markingEncodingEnum);
        buffer.reserve(_markingEncodingEnumEncoder.getEncodedLength());
        _markingEncodingEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMarkingStruct(const DevStudio::MarkingStruct& markingStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _markingStructEncoder.set(markingStruct);
        buffer.reserve(_markingStructEncoder.getEncodedLength());
        _markingStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMineDielectricDifferenceLengthlessArray(const std::vector< float >& mineDielectricDifferenceLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _mineDielectricDifferenceLengthlessArrayEncoder.set(mineDielectricDifferenceLengthlessArray);
        buffer.reserve(_mineDielectricDifferenceLengthlessArrayEncoder.getEncodedLength());
        _mineDielectricDifferenceLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMineFusingStruct(const DevStudio::MineFusingStruct& mineFusingStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _mineFusingStructEncoder.set(mineFusingStruct);
        buffer.reserve(_mineFusingStructEncoder.getEncodedLength());
        _mineFusingStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMineFusingStructLengthlessArray(const std::vector< DevStudio::MineFusingStruct >& mineFusingStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _mineFusingStructLengthlessArrayEncoder.set(mineFusingStructLengthlessArray);
        buffer.reserve(_mineFusingStructLengthlessArrayEncoder.getEncodedLength());
        _mineFusingStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMineIdentifierLengthlessArray(const std::vector< unsigned short >& mineIdentifierLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _mineIdentifierLengthlessArrayEncoder.set(mineIdentifierLengthlessArray);
        buffer.reserve(_mineIdentifierLengthlessArrayEncoder.getEncodedLength());
        _mineIdentifierLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldFusingEnum(const DevStudio::MinefieldFusingEnum::MinefieldFusingEnum& minefieldFusingEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldFusingEnumEncoder.set(minefieldFusingEnum);
        buffer.reserve(_minefieldFusingEnumEncoder.getEncodedLength());
        _minefieldFusingEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldLaneEnum(const DevStudio::MinefieldLaneEnum::MinefieldLaneEnum& minefieldLaneEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldLaneEnumEncoder.set(minefieldLaneEnum);
        buffer.reserve(_minefieldLaneEnumEncoder.getEncodedLength());
        _minefieldLaneEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldLaneMarkerStruct(const DevStudio::MinefieldLaneMarkerStruct& minefieldLaneMarkerStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldLaneMarkerStructEncoder.set(minefieldLaneMarkerStruct);
        buffer.reserve(_minefieldLaneMarkerStructEncoder.getEncodedLength());
        _minefieldLaneMarkerStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldLaneMarkerStructLengthlessArray(const std::vector< DevStudio::MinefieldLaneMarkerStruct >& minefieldLaneMarkerStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldLaneMarkerStructLengthlessArrayEncoder.set(minefieldLaneMarkerStructLengthlessArray);
        buffer.reserve(_minefieldLaneMarkerStructLengthlessArrayEncoder.getEncodedLength());
        _minefieldLaneMarkerStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldPaintSchemeEnum(const DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum& minefieldPaintSchemeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldPaintSchemeEnumEncoder.set(minefieldPaintSchemeEnum);
        buffer.reserve(_minefieldPaintSchemeEnumEncoder.getEncodedLength());
        _minefieldPaintSchemeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldPaintSchemeLengthlessArray(const std::vector< DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum >& minefieldPaintSchemeLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldPaintSchemeLengthlessArrayEncoder.set(minefieldPaintSchemeLengthlessArray);
        buffer.reserve(_minefieldPaintSchemeLengthlessArrayEncoder.getEncodedLength());
        _minefieldPaintSchemeLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldProtocolEnum(const DevStudio::MinefieldProtocolEnum::MinefieldProtocolEnum& minefieldProtocolEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldProtocolEnumEncoder.set(minefieldProtocolEnum);
        buffer.reserve(_minefieldProtocolEnumEncoder.getEncodedLength());
        _minefieldProtocolEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldSensorTypeEnum(const DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum& minefieldSensorTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldSensorTypeEnumEncoder.set(minefieldSensorTypeEnum);
        buffer.reserve(_minefieldSensorTypeEnumEncoder.getEncodedLength());
        _minefieldSensorTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldSensorTypeLengthlessArray(const std::vector< DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum >& minefieldSensorTypeLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldSensorTypeLengthlessArrayEncoder.set(minefieldSensorTypeLengthlessArray);
        buffer.reserve(_minefieldSensorTypeLengthlessArrayEncoder.getEncodedLength());
        _minefieldSensorTypeLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldStatusEnum(const DevStudio::MinefieldStatusEnum::MinefieldStatusEnum& minefieldStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldStatusEnumEncoder.set(minefieldStatusEnum);
        buffer.reserve(_minefieldStatusEnumEncoder.getEncodedLength());
        _minefieldStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMinefieldTypeEnum(const DevStudio::MinefieldTypeEnum::MinefieldTypeEnum& minefieldTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _minefieldTypeEnumEncoder.set(minefieldTypeEnum);
        buffer.reserve(_minefieldTypeEnumEncoder.getEncodedLength());
        _minefieldTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMissingRecordNumbersLengthlessArray1Plus(const std::vector< char >& missingRecordNumbersLengthlessArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _missingRecordNumbersLengthlessArray1PlusEncoder.set(missingRecordNumbersLengthlessArray1Plus);
        buffer.reserve(_missingRecordNumbersLengthlessArray1PlusEncoder.getEncodedLength());
        _missingRecordNumbersLengthlessArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMoveByRouteTaskStruct(const DevStudio::MoveByRouteTaskStruct& moveByRouteTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _moveByRouteTaskStructEncoder.set(moveByRouteTaskStruct);
        buffer.reserve(_moveByRouteTaskStructEncoder.getEncodedLength());
        _moveByRouteTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMoveInDirectionTaskStruct(const DevStudio::MoveInDirectionTaskStruct& moveInDirectionTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _moveInDirectionTaskStructEncoder.set(moveInDirectionTaskStruct);
        buffer.reserve(_moveInDirectionTaskStructEncoder.getEncodedLength());
        _moveInDirectionTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMoveIntoFormationTaskStruct(const DevStudio::MoveIntoFormationTaskStruct& moveIntoFormationTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _moveIntoFormationTaskStructEncoder.set(moveIntoFormationTaskStruct);
        buffer.reserve(_moveIntoFormationTaskStructEncoder.getEncodedLength());
        _moveIntoFormationTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMoveTaskProgress(const DevStudio::MoveTaskProgress& moveTaskProgress)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _moveTaskProgressEncoder.set(moveTaskProgress);
        buffer.reserve(_moveTaskProgressEncoder.getEncodedLength());
        _moveTaskProgressEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMoveToLocationTaskStruct(const DevStudio::MoveToLocationTaskStruct& moveToLocationTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _moveToLocationTaskStructEncoder.set(moveToLocationTaskStruct);
        buffer.reserve(_moveToLocationTaskStructEncoder.getEncodedLength());
        _moveToLocationTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMoveTypeEnum(const DevStudio::MoveTypeEnum::MoveTypeEnum& moveTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _moveTypeEnumEncoder.set(moveTypeEnum);
        buffer.reserve(_moveTypeEnumEncoder.getEncodedLength());
        _moveTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeMsgIdEnum(const DevStudio::MsgIdEnum::MsgIdEnum& msgIdEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _msgIdEnumEncoder.set(msgIdEnum);
        buffer.reserve(_msgIdEnumEncoder.getEncodedLength());
        _msgIdEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNamedLocationStruct(const DevStudio::NamedLocationStruct& namedLocationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _namedLocationStructEncoder.set(namedLocationStruct);
        buffer.reserve(_namedLocationStructEncoder.getEncodedLength());
        _namedLocationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNavigationLightFlashingEnum(const DevStudio::NavigationLightFlashingEnum::NavigationLightFlashingEnum& navigationLightFlashingEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _navigationLightFlashingEnumEncoder.set(navigationLightFlashingEnum);
        buffer.reserve(_navigationLightFlashingEnumEncoder.getEncodedLength());
        _navigationLightFlashingEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNavigationStatusEnum(const DevStudio::NavigationStatusEnum::NavigationStatusEnum& navigationStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _navigationStatusEnumEncoder.set(navigationStatusEnum);
        buffer.reserve(_navigationStatusEnumEncoder.getEncodedLength());
        _navigationStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNetworkDeviceArray(const std::vector< DevStudio::NetworkDeviceStruct >& networkDeviceArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _networkDeviceArrayEncoder.set(networkDeviceArray);
        buffer.reserve(_networkDeviceArrayEncoder.getEncodedLength());
        _networkDeviceArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNetworkDeviceEmptyCharactersticsStruct(const DevStudio::NetworkDeviceEmptyCharactersticsStruct& networkDeviceEmptyCharactersticsStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _networkDeviceEmptyCharactersticsStructEncoder.set(networkDeviceEmptyCharactersticsStruct);
        buffer.reserve(_networkDeviceEmptyCharactersticsStructEncoder.getEncodedLength());
        _networkDeviceEmptyCharactersticsStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNetworkDeviceGenericTransmitterCharacteristicsStruct(const DevStudio::NetworkDeviceGenericTransmitterCharacteristicsStruct& networkDeviceGenericTransmitterCharacteristicsStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _networkDeviceGenericTransmitterCharacteristicsStructEncoder.set(networkDeviceGenericTransmitterCharacteristicsStruct);
        buffer.reserve(_networkDeviceGenericTransmitterCharacteristicsStructEncoder.getEncodedLength());
        _networkDeviceGenericTransmitterCharacteristicsStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNetworkDeviceReceiverCharacteristicsVariant(const DevStudio::NetworkDeviceReceiverCharacteristicsVariant& networkDeviceReceiverCharacteristicsVariant)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _networkDeviceReceiverCharacteristicsVariantEncoder.set(networkDeviceReceiverCharacteristicsVariant);
        buffer.reserve(_networkDeviceReceiverCharacteristicsVariantEncoder.getEncodedLength());
        _networkDeviceReceiverCharacteristicsVariantEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNetworkDeviceStruct(const DevStudio::NetworkDeviceStruct& networkDeviceStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _networkDeviceStructEncoder.set(networkDeviceStruct);
        buffer.reserve(_networkDeviceStructEncoder.getEncodedLength());
        _networkDeviceStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNetworkDeviceTransmitterCharacteristicsVariant(const DevStudio::NetworkDeviceTransmitterCharacteristicsVariant& networkDeviceTransmitterCharacteristicsVariant)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _networkDeviceTransmitterCharacteristicsVariantEncoder.set(networkDeviceTransmitterCharacteristicsVariant);
        buffer.reserve(_networkDeviceTransmitterCharacteristicsVariantEncoder.getEncodedLength());
        _networkDeviceTransmitterCharacteristicsVariantEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNomenclatureEnum(const DevStudio::NomenclatureEnum::NomenclatureEnum& nomenclatureEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _nomenclatureEnumEncoder.set(nomenclatureEnum);
        buffer.reserve(_nomenclatureEnumEncoder.getEncodedLength());
        _nomenclatureEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeNomenclatureVersionEnum(const DevStudio::NomenclatureVersionEnum::NomenclatureVersionEnum& nomenclatureVersionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _nomenclatureVersionEnumEncoder.set(nomenclatureVersionEnum);
        buffer.reserve(_nomenclatureVersionEnumEncoder.getEncodedLength());
        _nomenclatureVersionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeObserveTaskStruct(const DevStudio::ObserveTaskStruct& observeTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _observeTaskStructEncoder.set(observeTaskStruct);
        buffer.reserve(_observeTaskStructEncoder.getEncodedLength());
        _observeTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeObservedEquipment(const DevStudio::ObservedEquipment& observedEquipment)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _observedEquipmentEncoder.set(observedEquipment);
        buffer.reserve(_observedEquipmentEncoder.getEncodedLength());
        _observedEquipmentEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray(const std::vector< char >& octetArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArrayEncoder.set(octetArray);
        buffer.reserve(_octetArrayEncoder.getEncodedLength());
        _octetArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray1Plus(const std::vector< char >& octetArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArray1PlusEncoder.set(octetArray1Plus);
        buffer.reserve(_octetArray1PlusEncoder.getEncodedLength());
        _octetArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray2(const std::vector< char >& octetArray2)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArray2Encoder.set(octetArray2);
        buffer.reserve(_octetArray2Encoder.getEncodedLength());
        _octetArray2Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray3(const std::vector< char >& octetArray3)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArray3Encoder.set(octetArray3);
        buffer.reserve(_octetArray3Encoder.getEncodedLength());
        _octetArray3Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray4(const std::vector< char >& octetArray4)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArray4Encoder.set(octetArray4);
        buffer.reserve(_octetArray4Encoder.getEncodedLength());
        _octetArray4Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray7(const std::vector< char >& octetArray7)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArray7Encoder.set(octetArray7);
        buffer.reserve(_octetArray7Encoder.getEncodedLength());
        _octetArray7Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetArray8(const std::vector< char >& octetArray8)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetArray8Encoder.set(octetArray8);
        buffer.reserve(_octetArray8Encoder.getEncodedLength());
        _octetArray8Encoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetPadding32Array(const std::vector< char >& octetPadding32Array)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetPadding32ArrayEncoder.set(octetPadding32Array);
        buffer.reserve(_octetPadding32ArrayEncoder.getEncodedLength());
        _octetPadding32ArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOctetPadding64Array(const std::vector< char >& octetPadding64Array)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _octetPadding64ArrayEncoder.set(octetPadding64Array);
        buffer.reserve(_octetPadding64ArrayEncoder.getEncodedLength());
        _octetPadding64ArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOpacityCodeEnum(const DevStudio::OpacityCodeEnum::OpacityCodeEnum& opacityCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _opacityCodeEnumEncoder.set(opacityCodeEnum);
        buffer.reserve(_opacityCodeEnumEncoder.getEncodedLength());
        _opacityCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOperateCheckpointTaskStruct(const DevStudio::OperateCheckpointTaskStruct& operateCheckpointTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _operateCheckpointTaskStructEncoder.set(operateCheckpointTaskStruct);
        buffer.reserve(_operateCheckpointTaskStructEncoder.getEncodedLength());
        _operateCheckpointTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOperateObservationPostTaskStruct(const DevStudio::OperateObservationPostTaskStruct& operateObservationPostTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _operateObservationPostTaskStructEncoder.set(operateObservationPostTaskStruct);
        buffer.reserve(_operateObservationPostTaskStructEncoder.getEncodedLength());
        _operateObservationPostTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOrientationStruct(const DevStudio::OrientationStruct& orientationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _orientationStructEncoder.set(orientationStruct);
        buffer.reserve(_orientationStructEncoder.getEncodedLength());
        _orientationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOrientationStructLengthlessArray(const std::vector< DevStudio::OrientationStruct >& orientationStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _orientationStructLengthlessArrayEncoder.set(orientationStructLengthlessArray);
        buffer.reserve(_orientationStructLengthlessArrayEncoder.getEncodedLength());
        _orientationStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeOtherActivityTaskStruct(const DevStudio::OtherActivityTaskStruct& otherActivityTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _otherActivityTaskStructEncoder.set(otherActivityTaskStruct);
        buffer.reserve(_otherActivityTaskStructEncoder.getEncodedLength());
        _otherActivityTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeParameterTypeEnum(const DevStudio::ParameterTypeEnum::ParameterTypeEnum& parameterTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _parameterTypeEnumEncoder.set(parameterTypeEnum);
        buffer.reserve(_parameterTypeEnumEncoder.getEncodedLength());
        _parameterTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeParameterValueVariantStruct(const DevStudio::ParameterValueVariantStruct& parameterValueVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _parameterValueVariantStructEncoder.set(parameterValueVariantStruct);
        buffer.reserve(_parameterValueVariantStructEncoder.getEncodedLength());
        _parameterValueVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePartNumberEnum(const DevStudio::PartNumberEnum::PartNumberEnum& partNumberEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _partNumberEnumEncoder.set(partNumberEnum);
        buffer.reserve(_partNumberEnumEncoder.getEncodedLength());
        _partNumberEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePatrolMoveTypeEnum(const DevStudio::PatrolMoveTypeEnum::PatrolMoveTypeEnum& patrolMoveTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _patrolMoveTypeEnumEncoder.set(patrolMoveTypeEnum);
        buffer.reserve(_patrolMoveTypeEnumEncoder.getEncodedLength());
        _patrolMoveTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePatrolTaskProgress(const DevStudio::PatrolTaskProgress& patrolTaskProgress)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _patrolTaskProgressEncoder.set(patrolTaskProgress);
        buffer.reserve(_patrolTaskProgressEncoder.getEncodedLength());
        _patrolTaskProgressEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePatrolTaskStruct(const DevStudio::PatrolTaskStruct& patrolTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _patrolTaskStructEncoder.set(patrolTaskStruct);
        buffer.reserve(_patrolTaskStructEncoder.getEncodedLength());
        _patrolTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePatrolTypeEnum(const DevStudio::PatrolTypeEnum::PatrolTypeEnum& patrolTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _patrolTypeEnumEncoder.set(patrolTypeEnum);
        buffer.reserve(_patrolTypeEnumEncoder.getEncodedLength());
        _patrolTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePerimeterPointStruct(const DevStudio::PerimeterPointStruct& perimeterPointStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _perimeterPointStructEncoder.set(perimeterPointStruct);
        buffer.reserve(_perimeterPointStructEncoder.getEncodedLength());
        _perimeterPointStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePerimeterPointStructLengthlessArray(const std::vector< DevStudio::PerimeterPointStruct >& perimeterPointStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _perimeterPointStructLengthlessArrayEncoder.set(perimeterPointStructLengthlessArray);
        buffer.reserve(_perimeterPointStructLengthlessArrayEncoder.getEncodedLength());
        _perimeterPointStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePhysicalGenericNetworkStruct(const DevStudio::PhysicalGenericNetworkStruct& physicalGenericNetworkStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _physicalGenericNetworkStructEncoder.set(physicalGenericNetworkStruct);
        buffer.reserve(_physicalGenericNetworkStructEncoder.getEncodedLength());
        _physicalGenericNetworkStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePhysicalNetworkDescriptionVariant(const DevStudio::PhysicalNetworkDescriptionVariant& physicalNetworkDescriptionVariant)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _physicalNetworkDescriptionVariantEncoder.set(physicalNetworkDescriptionVariant);
        buffer.reserve(_physicalNetworkDescriptionVariantEncoder.getEncodedLength());
        _physicalNetworkDescriptionVariantEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePhysicalNetworkTypeEnum(const DevStudio::PhysicalNetworkTypeEnum::PhysicalNetworkTypeEnum& physicalNetworkTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _physicalNetworkTypeEnumEncoder.set(physicalNetworkTypeEnum);
        buffer.reserve(_physicalNetworkTypeEnumEncoder.getEncodedLength());
        _physicalNetworkTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePhysicalUndefinedNetworkStruct(const DevStudio::PhysicalUndefinedNetworkStruct& physicalUndefinedNetworkStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _physicalUndefinedNetworkStructEncoder.set(physicalUndefinedNetworkStruct);
        buffer.reserve(_physicalUndefinedNetworkStructEncoder.getEncodedLength());
        _physicalUndefinedNetworkStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePlumeDimensionRateStruct(const DevStudio::PlumeDimensionRateStruct& plumeDimensionRateStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _plumeDimensionRateStructEncoder.set(plumeDimensionRateStruct);
        buffer.reserve(_plumeDimensionRateStructEncoder.getEncodedLength());
        _plumeDimensionRateStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePlumeDimensionStruct(const DevStudio::PlumeDimensionStruct& plumeDimensionStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _plumeDimensionStructEncoder.set(plumeDimensionStruct);
        buffer.reserve(_plumeDimensionStructEncoder.getEncodedLength());
        _plumeDimensionStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePoint2GeomRecStruct(const DevStudio::Point2GeomRecStruct& point2GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _point2GeomRecStructEncoder.set(point2GeomRecStruct);
        buffer.reserve(_point2GeomRecStructEncoder.getEncodedLength());
        _point2GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePrecipitationStruct(const DevStudio::PrecipitationStruct& precipitationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _precipitationStructEncoder.set(precipitationStruct);
        buffer.reserve(_precipitationStructEncoder.getEncodedLength());
        _precipitationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePrecipitationTypeEnum(const DevStudio::PrecipitationTypeEnum::PrecipitationTypeEnum& precipitationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _precipitationTypeEnumEncoder.set(precipitationTypeEnum);
        buffer.reserve(_precipitationTypeEnumEncoder.getEncodedLength());
        _precipitationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePropulsionPlantEnum(const DevStudio::PropulsionPlantEnum::PropulsionPlantEnum& propulsionPlantEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _propulsionPlantEnumEncoder.set(propulsionPlantEnum);
        buffer.reserve(_propulsionPlantEnumEncoder.getEncodedLength());
        _propulsionPlantEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePropulsionSystemDataStruct(const DevStudio::PropulsionSystemDataStruct& propulsionSystemDataStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _propulsionSystemDataStructEncoder.set(propulsionSystemDataStruct);
        buffer.reserve(_propulsionSystemDataStructEncoder.getEncodedLength());
        _propulsionSystemDataStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePropulsionSystemDataStructLengthlessArray(const std::vector< DevStudio::PropulsionSystemDataStruct >& propulsionSystemDataStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _propulsionSystemDataStructLengthlessArrayEncoder.set(propulsionSystemDataStructLengthlessArray);
        buffer.reserve(_propulsionSystemDataStructLengthlessArrayEncoder.getEncodedLength());
        _propulsionSystemDataStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeProtectionEffectivenessStruct(const DevStudio::ProtectionEffectivenessStruct& protectionEffectivenessStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _protectionEffectivenessStructEncoder.set(protectionEffectivenessStruct);
        buffer.reserve(_protectionEffectivenessStructEncoder.getEncodedLength());
        _protectionEffectivenessStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePulseModulationTypeEnum(const DevStudio::PulseModulationTypeEnum::PulseModulationTypeEnum& pulseModulationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _pulseModulationTypeEnumEncoder.set(pulseModulationTypeEnum);
        buffer.reserve(_pulseModulationTypeEnumEncoder.getEncodedLength());
        _pulseModulationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodePumpFloodingTaskStruct(const DevStudio::PumpFloodingTaskStruct& pumpFloodingTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _pumpFloodingTaskStructEncoder.set(pumpFloodingTaskStruct);
        buffer.reserve(_pumpFloodingTaskStructEncoder.getEncodedLength());
        _pumpFloodingTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRFModulationSystemTypeEnum(const DevStudio::RFModulationSystemTypeEnum::RFModulationSystemTypeEnum& rFModulationSystemTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rFModulationSystemTypeEnumEncoder.set(rFModulationSystemTypeEnum);
        buffer.reserve(_rFModulationSystemTypeEnumEncoder.getEncodedLength());
        _rFModulationSystemTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRFModulationTypeVariantStruct(const DevStudio::RFModulationTypeVariantStruct& rFModulationTypeVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rFModulationTypeVariantStructEncoder.set(rFModulationTypeVariantStruct);
        buffer.reserve(_rFModulationTypeVariantStructEncoder.getEncodedLength());
        _rFModulationTypeVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRTIobjectIdArray(const std::vector< std::string >& rTIobjectIdArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rTIobjectIdArrayEncoder.set(rTIobjectIdArray);
        buffer.reserve(_rTIobjectIdArrayEncoder.getEncodedLength());
        _rTIobjectIdArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRadioInputSourceEnum(const DevStudio::RadioInputSourceEnum::RadioInputSourceEnum& radioInputSourceEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _radioInputSourceEnumEncoder.set(radioInputSourceEnum);
        buffer.reserve(_radioInputSourceEnumEncoder.getEncodedLength());
        _radioInputSourceEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRadioTypeStruct(const DevStudio::RadioTypeStruct& radioTypeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _radioTypeStructEncoder.set(radioTypeStruct);
        buffer.reserve(_radioTypeStructEncoder.getEncodedLength());
        _radioTypeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeReceiverOperationalStatusEnum(const DevStudio::ReceiverOperationalStatusEnum::ReceiverOperationalStatusEnum& receiverOperationalStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _receiverOperationalStatusEnumEncoder.set(receiverOperationalStatusEnum);
        buffer.reserve(_receiverOperationalStatusEnumEncoder.getEncodedLength());
        _receiverOperationalStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRecordSetStruct(const DevStudio::RecordSetStruct& recordSetStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _recordSetStructEncoder.set(recordSetStruct);
        buffer.reserve(_recordSetStructEncoder.getEncodedLength());
        _recordSetStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRecordSetStructArray1Plus(const std::vector< DevStudio::RecordSetStruct >& recordSetStructArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _recordSetStructArray1PlusEncoder.set(recordSetStructArray1Plus);
        buffer.reserve(_recordSetStructArray1PlusEncoder.getEncodedLength());
        _recordSetStructArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRecordStruct(const DevStudio::RecordStruct& recordStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _recordStructEncoder.set(recordStruct);
        buffer.reserve(_recordStructEncoder.getEncodedLength());
        _recordStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRecordStructArray(const std::vector< DevStudio::RecordStruct >& recordStructArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _recordStructArrayEncoder.set(recordStructArray);
        buffer.reserve(_recordStructArrayEncoder.getEncodedLength());
        _recordStructArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRectVol1GeomRecStruct(const DevStudio::RectVol1GeomRecStruct& rectVol1GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rectVol1GeomRecStructEncoder.set(rectVol1GeomRecStruct);
        buffer.reserve(_rectVol1GeomRecStructEncoder.getEncodedLength());
        _rectVol1GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRectVol2GeomRecStruct(const DevStudio::RectVol2GeomRecStruct& rectVol2GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rectVol2GeomRecStructEncoder.set(rectVol2GeomRecStruct);
        buffer.reserve(_rectVol2GeomRecStructEncoder.getEncodedLength());
        _rectVol2GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRectVol3GeomRecStruct(const DevStudio::RectVol3GeomRecStruct& rectVol3GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rectVol3GeomRecStructEncoder.set(rectVol3GeomRecStruct);
        buffer.reserve(_rectVol3GeomRecStructEncoder.getEncodedLength());
        _rectVol3GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeReferenceSystemEnum(const DevStudio::ReferenceSystemEnum::ReferenceSystemEnum& referenceSystemEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _referenceSystemEnumEncoder.set(referenceSystemEnum);
        buffer.reserve(_referenceSystemEnumEncoder.getEncodedLength());
        _referenceSystemEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRefuelingStationLightColorEnum(const DevStudio::RefuelingStationLightColorEnum::RefuelingStationLightColorEnum& refuelingStationLightColorEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _refuelingStationLightColorEnumEncoder.set(refuelingStationLightColorEnum);
        buffer.reserve(_refuelingStationLightColorEnumEncoder.getEncodedLength());
        _refuelingStationLightColorEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRefuelingStationLightDayNightEnum(const DevStudio::RefuelingStationLightDayNightEnum::RefuelingStationLightDayNightEnum& refuelingStationLightDayNightEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _refuelingStationLightDayNightEnumEncoder.set(refuelingStationLightDayNightEnum);
        buffer.reserve(_refuelingStationLightDayNightEnumEncoder.getEncodedLength());
        _refuelingStationLightDayNightEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRelation(const DevStudio::Relation& relation)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _relationEncoder.set(relation);
        buffer.reserve(_relationEncoder.getEncodedLength());
        _relationEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRelativePositionStruct(const DevStudio::RelativePositionStruct& relativePositionStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _relativePositionStructEncoder.set(relativePositionStruct);
        buffer.reserve(_relativePositionStructEncoder.getEncodedLength());
        _relativePositionStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRelativeRangeBearingStruct(const DevStudio::RelativeRangeBearingStruct& relativeRangeBearingStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _relativeRangeBearingStructEncoder.set(relativeRangeBearingStruct);
        buffer.reserve(_relativeRangeBearingStructEncoder.getEncodedLength());
        _relativeRangeBearingStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeReleaseDistributionStruct(const DevStudio::ReleaseDistributionStruct& releaseDistributionStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _releaseDistributionStructEncoder.set(releaseDistributionStruct);
        buffer.reserve(_releaseDistributionStructEncoder.getEncodedLength());
        _releaseDistributionStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeReleaseDynamicsStruct(const DevStudio::ReleaseDynamicsStruct& releaseDynamicsStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _releaseDynamicsStructEncoder.set(releaseDynamicsStruct);
        buffer.reserve(_releaseDynamicsStructEncoder.getEncodedLength());
        _releaseDynamicsStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeReleaseSizeStruct(const DevStudio::ReleaseSizeStruct& releaseSizeStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _releaseSizeStructEncoder.set(releaseSizeStruct);
        buffer.reserve(_releaseSizeStructEncoder.getEncodedLength());
        _releaseSizeStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRepairResultEnum(const DevStudio::RepairResultEnum::RepairResultEnum& repairResultEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _repairResultEnumEncoder.set(repairResultEnum);
        buffer.reserve(_repairResultEnumEncoder.getEncodedLength());
        _repairResultEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRepairTaskStruct(const DevStudio::RepairTaskStruct& repairTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _repairTaskStructEncoder.set(repairTaskStruct);
        buffer.reserve(_repairTaskStructEncoder.getEncodedLength());
        _repairTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRepairTypeEnum(const DevStudio::RepairTypeEnum::RepairTypeEnum& repairTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _repairTypeEnumEncoder.set(repairTypeEnum);
        buffer.reserve(_repairTypeEnumEncoder.getEncodedLength());
        _repairTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRequestStatusEnum(const DevStudio::RequestStatusEnum::RequestStatusEnum& requestStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _requestStatusEnumEncoder.set(requestStatusEnum);
        buffer.reserve(_requestStatusEnumEncoder.getEncodedLength());
        _requestStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRequestedConnection(const DevStudio::RequestedConnection& requestedConnection)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _requestedConnectionEncoder.set(requestedConnection);
        buffer.reserve(_requestedConnectionEncoder.getEncodedLength());
        _requestedConnectionEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRequestedConnectionArray(const std::vector< DevStudio::RequestedConnection >& requestedConnectionArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _requestedConnectionArrayEncoder.set(requestedConnectionArray);
        buffer.reserve(_requestedConnectionArrayEncoder.getEncodedLength());
        _requestedConnectionArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeResourceStatusStruct(const DevStudio::ResourceStatusStruct& resourceStatusStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _resourceStatusStructEncoder.set(resourceStatusStruct);
        buffer.reserve(_resourceStatusStructEncoder.getEncodedLength());
        _resourceStatusStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeResponseFlagEnum(const DevStudio::ResponseFlagEnum::ResponseFlagEnum& responseFlagEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _responseFlagEnumEncoder.set(responseFlagEnum);
        buffer.reserve(_responseFlagEnumEncoder.getEncodedLength());
        _responseFlagEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeResupplyTaskStruct(const DevStudio::ResupplyTaskStruct& resupplyTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _resupplyTaskStructEncoder.set(resupplyTaskStruct);
        buffer.reserve(_resupplyTaskStructEncoder.getEncodedLength());
        _resupplyTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRoadIceConditionEnum(const DevStudio::RoadIceConditionEnum::RoadIceConditionEnum& roadIceConditionEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _roadIceConditionEnumEncoder.set(roadIceConditionEnum);
        buffer.reserve(_roadIceConditionEnumEncoder.getEncodedLength());
        _roadIceConditionEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRoundStruct(const DevStudio::RoundStruct& roundStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _roundStructEncoder.set(roundStruct);
        buffer.reserve(_roundStructEncoder.getEncodedLength());
        _roundStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeRulesOfEngagementEnum(const DevStudio::RulesOfEngagementEnum::RulesOfEngagementEnum& rulesOfEngagementEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _rulesOfEngagementEnumEncoder.set(rulesOfEngagementEnum);
        buffer.reserve(_rulesOfEngagementEnumEncoder.getEncodedLength());
        _rulesOfEngagementEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSINCGARSModulationStruct(const DevStudio::SINCGARSModulationStruct& sINCGARSModulationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sINCGARSModulationStructEncoder.set(sINCGARSModulationStruct);
        buffer.reserve(_sINCGARSModulationStructEncoder.getEncodedLength());
        _sINCGARSModulationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSeaStateEnum(const DevStudio::SeaStateEnum::SeaStateEnum& seaStateEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _seaStateEnumEncoder.set(seaStateEnum);
        buffer.reserve(_seaStateEnumEncoder.getEncodedLength());
        _seaStateEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSedimentTypeEnum(const DevStudio::SedimentTypeEnum::SedimentTypeEnum& sedimentTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sedimentTypeEnumEncoder.set(sedimentTypeEnum);
        buffer.reserve(_sedimentTypeEnumEncoder.getEncodedLength());
        _sedimentTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSendSafetyRelatedBroadcastMessageTaskStruct(const DevStudio::SendSafetyRelatedBroadcastMessageTaskStruct& sendSafetyRelatedBroadcastMessageTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sendSafetyRelatedBroadcastMessageTaskStructEncoder.set(sendSafetyRelatedBroadcastMessageTaskStruct);
        buffer.reserve(_sendSafetyRelatedBroadcastMessageTaskStructEncoder.getEncodedLength());
        _sendSafetyRelatedBroadcastMessageTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSendSafetyRelatedMessageTaskStruct(const DevStudio::SendSafetyRelatedMessageTaskStruct& sendSafetyRelatedMessageTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sendSafetyRelatedMessageTaskStructEncoder.set(sendSafetyRelatedMessageTaskStruct);
        buffer.reserve(_sendSafetyRelatedMessageTaskStructEncoder.getEncodedLength());
        _sendSafetyRelatedMessageTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSensorStateEnum(const DevStudio::SensorStateEnum::SensorStateEnum& sensorStateEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sensorStateEnumEncoder.set(sensorStateEnum);
        buffer.reserve(_sensorStateEnumEncoder.getEncodedLength());
        _sensorStateEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSensorStruct(const DevStudio::SensorStruct& sensorStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sensorStructEncoder.set(sensorStruct);
        buffer.reserve(_sensorStructEncoder.getEncodedLength());
        _sensorStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeServiceTypeEnum(const DevStudio::ServiceTypeEnum::ServiceTypeEnum& serviceTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _serviceTypeEnumEncoder.set(serviceTypeEnum);
        buffer.reserve(_serviceTypeEnumEncoder.getEncodedLength());
        _serviceTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSetTransmitterStatusTaskStruct(const DevStudio::SetTransmitterStatusTaskStruct& setTransmitterStatusTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _setTransmitterStatusTaskStructEncoder.set(setTransmitterStatusTaskStruct);
        buffer.reserve(_setTransmitterStatusTaskStructEncoder.getEncodedLength());
        _setTransmitterStatusTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeShaftDataStruct(const DevStudio::ShaftDataStruct& shaftDataStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _shaftDataStructEncoder.set(shaftDataStruct);
        buffer.reserve(_shaftDataStructEncoder.getEncodedLength());
        _shaftDataStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeShaftDataStructLengthlessArray1Plus(const std::vector< DevStudio::ShaftDataStruct >& shaftDataStructLengthlessArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _shaftDataStructLengthlessArray1PlusEncoder.set(shaftDataStructLengthlessArray1Plus);
        buffer.reserve(_shaftDataStructLengthlessArray1PlusEncoder.getEncodedLength());
        _shaftDataStructLengthlessArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeShipTypeEnum(const DevStudio::ShipTypeEnum::ShipTypeEnum& shipTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _shipTypeEnumEncoder.set(shipTypeEnum);
        buffer.reserve(_shipTypeEnumEncoder.getEncodedLength());
        _shipTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSignalDataLengthlessArray1Plus(const std::vector< char >& signalDataLengthlessArray1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _signalDataLengthlessArray1PlusEncoder.set(signalDataLengthlessArray1Plus);
        buffer.reserve(_signalDataLengthlessArray1PlusEncoder.getEncodedLength());
        _signalDataLengthlessArray1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSignatureTypeEnum(const DevStudio::SignatureTypeEnum::SignatureTypeEnum& signatureTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _signatureTypeEnumEncoder.set(signatureTypeEnum);
        buffer.reserve(_signatureTypeEnumEncoder.getEncodedLength());
        _signatureTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSignatureVariant(const DevStudio::SignatureVariant& signatureVariant)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _signatureVariantEncoder.set(signatureVariant);
        buffer.reserve(_signatureVariantEncoder.getEncodedLength());
        _signatureVariantEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSilentAggregateStruct(const DevStudio::SilentAggregateStruct& silentAggregateStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _silentAggregateStructEncoder.set(silentAggregateStruct);
        buffer.reserve(_silentAggregateStructEncoder.getEncodedLength());
        _silentAggregateStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSilentAggregateStructLengthlessArray(const std::vector< DevStudio::SilentAggregateStruct >& silentAggregateStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _silentAggregateStructLengthlessArrayEncoder.set(silentAggregateStructLengthlessArray);
        buffer.reserve(_silentAggregateStructLengthlessArrayEncoder.getEncodedLength());
        _silentAggregateStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSilentEntityStruct(const DevStudio::SilentEntityStruct& silentEntityStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _silentEntityStructEncoder.set(silentEntityStruct);
        buffer.reserve(_silentEntityStructEncoder.getEncodedLength());
        _silentEntityStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSilentEntityStructLengthlessArray(const std::vector< DevStudio::SilentEntityStruct >& silentEntityStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _silentEntityStructLengthlessArrayEncoder.set(silentEntityStructLengthlessArray);
        buffer.reserve(_silentEntityStructLengthlessArrayEncoder.getEncodedLength());
        _silentEntityStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSnowStruct(const DevStudio::SnowStruct& snowStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _snowStructEncoder.set(snowStruct);
        buffer.reserve(_snowStructEncoder.getEncodedLength());
        _snowStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpatialFPStruct(const DevStudio::SpatialFPStruct& spatialFPStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spatialFPStructEncoder.set(spatialFPStruct);
        buffer.reserve(_spatialFPStructEncoder.getEncodedLength());
        _spatialFPStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpatialFVStruct(const DevStudio::SpatialFVStruct& spatialFVStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spatialFVStructEncoder.set(spatialFVStruct);
        buffer.reserve(_spatialFVStructEncoder.getEncodedLength());
        _spatialFVStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpatialRPStruct(const DevStudio::SpatialRPStruct& spatialRPStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spatialRPStructEncoder.set(spatialRPStruct);
        buffer.reserve(_spatialRPStructEncoder.getEncodedLength());
        _spatialRPStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpatialRVStruct(const DevStudio::SpatialRVStruct& spatialRVStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spatialRVStructEncoder.set(spatialRVStruct);
        buffer.reserve(_spatialRVStructEncoder.getEncodedLength());
        _spatialRVStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpatialStaticStruct(const DevStudio::SpatialStaticStruct& spatialStaticStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spatialStaticStructEncoder.set(spatialStaticStruct);
        buffer.reserve(_spatialStaticStructEncoder.getEncodedLength());
        _spatialStaticStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpatialVariantStruct(const DevStudio::SpatialVariantStruct& spatialVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spatialVariantStructEncoder.set(spatialVariantStruct);
        buffer.reserve(_spatialVariantStructEncoder.getEncodedLength());
        _spatialVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSphere1GeomRecStruct(const DevStudio::Sphere1GeomRecStruct& sphere1GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sphere1GeomRecStructEncoder.set(sphere1GeomRecStruct);
        buffer.reserve(_sphere1GeomRecStructEncoder.getEncodedLength());
        _sphere1GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSphere2GeomRecStruct(const DevStudio::Sphere2GeomRecStruct& sphere2GeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sphere2GeomRecStructEncoder.set(sphere2GeomRecStruct);
        buffer.reserve(_sphere2GeomRecStructEncoder.getEncodedLength());
        _sphere2GeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSphericalHarmonicAntennaStruct(const DevStudio::SphericalHarmonicAntennaStruct& sphericalHarmonicAntennaStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _sphericalHarmonicAntennaStructEncoder.set(sphericalHarmonicAntennaStruct);
        buffer.reserve(_sphericalHarmonicAntennaStructEncoder.getEncodedLength());
        _sphericalHarmonicAntennaStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpreadSpectrumEnum(const DevStudio::SpreadSpectrumEnum::SpreadSpectrumEnum& spreadSpectrumEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spreadSpectrumEnumEncoder.set(spreadSpectrumEnum);
        buffer.reserve(_spreadSpectrumEnumEncoder.getEncodedLength());
        _spreadSpectrumEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSpreadSpectrumVariantStruct(const DevStudio::SpreadSpectrumVariantStruct& spreadSpectrumVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _spreadSpectrumVariantStructEncoder.set(spreadSpectrumVariantStruct);
        buffer.reserve(_spreadSpectrumVariantStructEncoder.getEncodedLength());
        _spreadSpectrumVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeStanceCodeEnum(const DevStudio::StanceCodeEnum::StanceCodeEnum& stanceCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _stanceCodeEnumEncoder.set(stanceCodeEnum);
        buffer.reserve(_stanceCodeEnumEncoder.getEncodedLength());
        _stanceCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeStationEnum(const DevStudio::StationEnum::StationEnum& stationEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _stationEnumEncoder.set(stationEnum);
        buffer.reserve(_stationEnumEncoder.getEncodedLength());
        _stationEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeStationNameLocationVariantStruct(const DevStudio::StationNameLocationVariantStruct& stationNameLocationVariantStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _stationNameLocationVariantStructEncoder.set(stationNameLocationVariantStruct);
        buffer.reserve(_stationNameLocationVariantStructEncoder.getEncodedLength());
        _stationNameLocationVariantStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeStopFreezeReasonEnum(const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& stopFreezeReasonEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _stopFreezeReasonEnumEncoder.set(stopFreezeReasonEnum);
        buffer.reserve(_stopFreezeReasonEnumEncoder.getEncodedLength());
        _stopFreezeReasonEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSubsurfaceFormationTypeEnum(const DevStudio::SubsurfaceFormationTypeEnum::SubsurfaceFormationTypeEnum& subsurfaceFormationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _subsurfaceFormationTypeEnumEncoder.set(subsurfaceFormationTypeEnum);
        buffer.reserve(_subsurfaceFormationTypeEnumEncoder.getEncodedLength());
        _subsurfaceFormationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSupplyStatusStruct(const DevStudio::SupplyStatusStruct& supplyStatusStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _supplyStatusStructEncoder.set(supplyStatusStruct);
        buffer.reserve(_supplyStatusStructEncoder.getEncodedLength());
        _supplyStatusStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSupplyStruct(const DevStudio::SupplyStruct& supplyStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _supplyStructEncoder.set(supplyStruct);
        buffer.reserve(_supplyStructEncoder.getEncodedLength());
        _supplyStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSupplyStructLengthlessArray(const std::vector< DevStudio::SupplyStruct >& supplyStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _supplyStructLengthlessArrayEncoder.set(supplyStructLengthlessArray);
        buffer.reserve(_supplyStructLengthlessArrayEncoder.getEncodedLength());
        _supplyStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSurfaceFormationTypeEnum(const DevStudio::SurfaceFormationTypeEnum::SurfaceFormationTypeEnum& surfaceFormationTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _surfaceFormationTypeEnumEncoder.set(surfaceFormationTypeEnum);
        buffer.reserve(_surfaceFormationTypeEnumEncoder.getEncodedLength());
        _surfaceFormationTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSurfaceMoistureEnum(const DevStudio::SurfaceMoistureEnum::SurfaceMoistureEnum& surfaceMoistureEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _surfaceMoistureEnumEncoder.set(surfaceMoistureEnum);
        buffer.reserve(_surfaceMoistureEnumEncoder.getEncodedLength());
        _surfaceMoistureEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSymbolAmplificationVariant(const DevStudio::SymbolAmplificationVariant& symbolAmplificationVariant)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _symbolAmplificationVariantEncoder.set(symbolAmplificationVariant);
        buffer.reserve(_symbolAmplificationVariantEncoder.getEncodedLength());
        _symbolAmplificationVariantEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSymbolStruct(const DevStudio::SymbolStruct& symbolStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _symbolStructEncoder.set(symbolStruct);
        buffer.reserve(_symbolStructEncoder.getEncodedLength());
        _symbolStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeSymbolTypeEnum(const DevStudio::SymbolTypeEnum::SymbolTypeEnum& symbolTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _symbolTypeEnumEncoder.set(symbolTypeEnum);
        buffer.reserve(_symbolTypeEnumEncoder.getEncodedLength());
        _symbolTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTacticalDataLinkTypeEnum(const DevStudio::TacticalDataLinkTypeEnum::TacticalDataLinkTypeEnum& tacticalDataLinkTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _tacticalDataLinkTypeEnumEncoder.set(tacticalDataLinkTypeEnum);
        buffer.reserve(_tacticalDataLinkTypeEnumEncoder.getEncodedLength());
        _tacticalDataLinkTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTaskDefinition(const DevStudio::TaskDefinition& taskDefinition)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _taskDefinitionEncoder.set(taskDefinition);
        buffer.reserve(_taskDefinitionEncoder.getEncodedLength());
        _taskDefinitionEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTaskDefinitionVariantRecord(const DevStudio::TaskDefinitionVariantRecord& taskDefinitionVariantRecord)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _taskDefinitionVariantRecordEncoder.set(taskDefinitionVariantRecord);
        buffer.reserve(_taskDefinitionVariantRecordEncoder.getEncodedLength());
        _taskDefinitionVariantRecordEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTaskModeEnum(const DevStudio::TaskModeEnum::TaskModeEnum& taskModeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _taskModeEnumEncoder.set(taskModeEnum);
        buffer.reserve(_taskModeEnumEncoder.getEncodedLength());
        _taskModeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTaskProgress(const DevStudio::TaskProgress& taskProgress)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _taskProgressEncoder.set(taskProgress);
        buffer.reserve(_taskProgressEncoder.getEncodedLength());
        _taskProgressEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTaskProgressVariantRecord(const DevStudio::TaskProgressVariantRecord& taskProgressVariantRecord)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _taskProgressVariantRecordEncoder.set(taskProgressVariantRecord);
        buffer.reserve(_taskProgressVariantRecordEncoder.getEncodedLength());
        _taskProgressVariantRecordEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTaskStatusEnum(const DevStudio::TaskStatusEnum::TaskStatusEnum& taskStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _taskStatusEnumEncoder.set(taskStatusEnum);
        buffer.reserve(_taskStatusEnumEncoder.getEncodedLength());
        _taskStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTemperatureDegreeCelsiusFloat32LengthlessArray(const std::vector< float >& temperatureDegreeCelsiusFloat32LengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _temperatureDegreeCelsiusFloat32LengthlessArrayEncoder.set(temperatureDegreeCelsiusFloat32LengthlessArray);
        buffer.reserve(_temperatureDegreeCelsiusFloat32LengthlessArrayEncoder.getEncodedLength());
        _temperatureDegreeCelsiusFloat32LengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTrackStruct(const DevStudio::TrackStruct& trackStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _trackStructEncoder.set(trackStruct);
        buffer.reserve(_trackStructEncoder.getEncodedLength());
        _trackStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTrailingEffectsCodeEnum(const DevStudio::TrailingEffectsCodeEnum::TrailingEffectsCodeEnum& trailingEffectsCodeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _trailingEffectsCodeEnumEncoder.set(trailingEffectsCodeEnum);
        buffer.reserve(_trailingEffectsCodeEnumEncoder.getEncodedLength());
        _trailingEffectsCodeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTransferTypeEnum(const DevStudio::TransferTypeEnum::TransferTypeEnum& transferTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _transferTypeEnumEncoder.set(transferTypeEnum);
        buffer.reserve(_transferTypeEnumEncoder.getEncodedLength());
        _transferTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTransmitterOperationalStatusEnum(const DevStudio::TransmitterOperationalStatusEnum::TransmitterOperationalStatusEnum& transmitterOperationalStatusEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _transmitterOperationalStatusEnumEncoder.set(transmitterOperationalStatusEnum);
        buffer.reserve(_transmitterOperationalStatusEnumEncoder.getEncodedLength());
        _transmitterOperationalStatusEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTransportTaskStruct(const DevStudio::TransportTaskStruct& transportTaskStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _transportTaskStructEncoder.set(transportTaskStruct);
        buffer.reserve(_transportTaskStructEncoder.getEncodedLength());
        _transportTaskStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeTreatmentStruct(const DevStudio::TreatmentStruct& treatmentStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _treatmentStructEncoder.set(treatmentStruct);
        buffer.reserve(_treatmentStructEncoder.getEncodedLength());
        _treatmentStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUUID(const std::vector< char >& uUID)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _uUIDEncoder.set(uUID);
        buffer.reserve(_uUIDEncoder.getEncodedLength());
        _uUIDEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUniformGeomRecStruct(const DevStudio::UniformGeomRecStruct& uniformGeomRecStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _uniformGeomRecStructEncoder.set(uniformGeomRecStruct);
        buffer.reserve(_uniformGeomRecStructEncoder.getEncodedLength());
        _uniformGeomRecStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUnitSymbolAmplificationStruct(const DevStudio::UnitSymbolAmplificationStruct& unitSymbolAmplificationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _unitSymbolAmplificationStructEncoder.set(unitSymbolAmplificationStruct);
        buffer.reserve(_unitSymbolAmplificationStructEncoder.getEncodedLength());
        _unitSymbolAmplificationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUnmodulatedTypeEnum(const DevStudio::UnmodulatedTypeEnum::UnmodulatedTypeEnum& unmodulatedTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _unmodulatedTypeEnumEncoder.set(unmodulatedTypeEnum);
        buffer.reserve(_unmodulatedTypeEnumEncoder.getEncodedLength());
        _unmodulatedTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUnsignedInteger16Array1Plus(const std::vector< unsigned short >& unsignedInteger16Array1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _unsignedInteger16Array1PlusEncoder.set(unsignedInteger16Array1Plus);
        buffer.reserve(_unsignedInteger16Array1PlusEncoder.getEncodedLength());
        _unsignedInteger16Array1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUnsignedInteger32LengthlessArray(const std::vector< unsigned int >& unsignedInteger32LengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _unsignedInteger32LengthlessArrayEncoder.set(unsignedInteger32LengthlessArray);
        buffer.reserve(_unsignedInteger32LengthlessArrayEncoder.getEncodedLength());
        _unsignedInteger32LengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUnsignedInteger64Array1Plus(const std::vector< unsigned long long >& unsignedInteger64Array1Plus)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _unsignedInteger64Array1PlusEncoder.set(unsignedInteger64Array1Plus);
        buffer.reserve(_unsignedInteger64Array1PlusEncoder.getEncodedLength());
        _unsignedInteger64Array1PlusEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUnsignedInteger8LengthlessArray(const std::vector< char >& unsignedInteger8LengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _unsignedInteger8LengthlessArrayEncoder.set(unsignedInteger8LengthlessArray);
        buffer.reserve(_unsignedInteger8LengthlessArrayEncoder.getEncodedLength());
        _unsignedInteger8LengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeUserProtocolEnum(const DevStudio::UserProtocolEnum::UserProtocolEnum& userProtocolEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _userProtocolEnumEncoder.set(userProtocolEnum);
        buffer.reserve(_userProtocolEnumEncoder.getEncodedLength());
        _userProtocolEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVariableDatumStruct(const DevStudio::VariableDatumStruct& variableDatumStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _variableDatumStructEncoder.set(variableDatumStruct);
        buffer.reserve(_variableDatumStructEncoder.getEncodedLength());
        _variableDatumStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVariableDatumStructArray(const std::vector< DevStudio::VariableDatumStruct >& variableDatumStructArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _variableDatumStructArrayEncoder.set(variableDatumStructArray);
        buffer.reserve(_variableDatumStructArrayEncoder.getEncodedLength());
        _variableDatumStructArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVariableDatumStructLengthlessArray(const std::vector< DevStudio::VariableDatumStruct >& variableDatumStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _variableDatumStructLengthlessArrayEncoder.set(variableDatumStructLengthlessArray);
        buffer.reserve(_variableDatumStructLengthlessArrayEncoder.getEncodedLength());
        _variableDatumStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVectoringNozzleSystemDataStruct(const DevStudio::VectoringNozzleSystemDataStruct& vectoringNozzleSystemDataStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _vectoringNozzleSystemDataStructEncoder.set(vectoringNozzleSystemDataStruct);
        buffer.reserve(_vectoringNozzleSystemDataStructEncoder.getEncodedLength());
        _vectoringNozzleSystemDataStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVectoringNozzleSystemDataStructLengthlessArray(const std::vector< DevStudio::VectoringNozzleSystemDataStruct >& vectoringNozzleSystemDataStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _vectoringNozzleSystemDataStructLengthlessArrayEncoder.set(vectoringNozzleSystemDataStructLengthlessArray);
        buffer.reserve(_vectoringNozzleSystemDataStructLengthlessArrayEncoder.getEncodedLength());
        _vectoringNozzleSystemDataStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVelocityVectorStruct(const DevStudio::VelocityVectorStruct& velocityVectorStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _velocityVectorStructEncoder.set(velocityVectorStruct);
        buffer.reserve(_velocityVectorStructEncoder.getEncodedLength());
        _velocityVectorStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeVisibleSideLocationEnum(const DevStudio::VisibleSideLocationEnum::VisibleSideLocationEnum& visibleSideLocationEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _visibleSideLocationEnumEncoder.set(visibleSideLocationEnum);
        buffer.reserve(_visibleSideLocationEnumEncoder.getEncodedLength());
        _visibleSideLocationEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWarheadTypeEnum(const DevStudio::WarheadTypeEnum::WarheadTypeEnum& warheadTypeEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _warheadTypeEnumEncoder.set(warheadTypeEnum);
        buffer.reserve(_warheadTypeEnumEncoder.getEncodedLength());
        _warheadTypeEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWaveStruct(const DevStudio::WaveStruct& waveStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _waveStructEncoder.set(waveStruct);
        buffer.reserve(_waveStructEncoder.getEncodedLength());
        _waveStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWaypoint(const DevStudio::Waypoint& waypoint)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _waypointEncoder.set(waypoint);
        buffer.reserve(_waypointEncoder.getEncodedLength());
        _waypointEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWeaponControlOrderEnum(const DevStudio::WeaponControlOrderEnum::WeaponControlOrderEnum& weaponControlOrderEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _weaponControlOrderEnumEncoder.set(weaponControlOrderEnum);
        buffer.reserve(_weaponControlOrderEnumEncoder.getEncodedLength());
        _weaponControlOrderEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWeaponStateEnum(const DevStudio::WeaponStateEnum::WeaponStateEnum& weaponStateEnum)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _weaponStateEnumEncoder.set(weaponStateEnum);
        buffer.reserve(_weaponStateEnumEncoder.getEncodedLength());
        _weaponStateEnumEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWindStruct(const DevStudio::WindStruct& windStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _windStructEncoder.set(windStruct);
        buffer.reserve(_windStructEncoder.getEncodedLength());
        _windStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWorldLocationStruct(const DevStudio::WorldLocationStruct& worldLocationStruct)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _worldLocationStructEncoder.set(worldLocationStruct);
        buffer.reserve(_worldLocationStructEncoder.getEncodedLength());
        _worldLocationStructEncoder.encodeInto(buffer);
        return buffer;
    }
    std::vector<char> Encoders::encodeWorldLocationStructLengthlessArray(const std::vector< DevStudio::WorldLocationStruct >& worldLocationStructLengthlessArray)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_encodersLock);
        _worldLocationStructLengthlessArrayEncoder.set(worldLocationStructLengthlessArray);
        buffer.reserve(_worldLocationStructLengthlessArrayEncoder.getEncodedLength());
        _worldLocationStructLengthlessArrayEncoder.encodeInto(buffer);
        return buffer;
    }
// === AUTO-GENERATED FUNCTIONS END ===

        std::vector<char> Encoders::encodebool(const bool& _bool)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _boolEncoder.set(_bool);
            buffer.reserve(_boolEncoder.getEncodedLength());
            _boolEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodechar(const char& _char)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _charEncoder.set(_char);
            buffer.reserve(_charEncoder.getEncodedLength());
            _charEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodeshort(const short& _short)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _shortEncoder.set(_short);
            buffer.reserve(_shortEncoder.getEncodedLength());
            _shortEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodeunsignedshort(const unsigned short& _unsignedShort)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _unsignedshortEncoder.set(_unsignedShort);
            buffer.reserve(_unsignedshortEncoder.getEncodedLength());
            _unsignedshortEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodeint(const int& _int)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _intEncoder.set(_int);
            buffer.reserve(_intEncoder.getEncodedLength());
            _intEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodeunsignedint(const unsigned int& _unsignedInt)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _unsignedintEncoder.set(_unsignedInt);
            buffer.reserve(_unsignedintEncoder.getEncodedLength());
            _unsignedintEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodelong(const long& _long)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _longEncoder.set(_long);
            buffer.reserve(_longEncoder.getEncodedLength());
            _longEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodeunsignedlong(const unsigned long& _unsignedLong)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _unsignedlongEncoder.set(_unsignedLong);
            buffer.reserve(_unsignedlongEncoder.getEncodedLength());
            _unsignedlongEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodelonglong(const long long& _longLong)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _longlongEncoder.set(_longLong);
            buffer.reserve(_longlongEncoder.getEncodedLength());
            _longlongEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodeunsignedlonglong(const unsigned long long& _unsignedLongLong)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _unsignedlonglongEncoder.set(_unsignedLongLong);
            buffer.reserve(_unsignedlonglongEncoder.getEncodedLength());
            _unsignedlonglongEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodefloat(const float& _float)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _floatEncoder.set(_float);
            buffer.reserve(_floatEncoder.getEncodedLength());
            _floatEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodedouble(const double& _double)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _doubleEncoder.set(_double);
            buffer.reserve(_doubleEncoder.getEncodedLength());
            _doubleEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodestring(const std::string& _string)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _stringEncoder.set(_string);
            buffer.reserve(_stringEncoder.getEncodedLength());
            _stringEncoder.encodeInto(buffer);
            return buffer;
        }
        std::vector<char> Encoders::encodewstring(const std::wstring& _wstring)
        {
            std::vector<char> buffer;
            std::unique_lock<std::mutex> lock(_encodersLock);
            _wstringEncoder.set(_wstring);
            buffer.reserve(_wstringEncoder.getEncodedLength());
            _wstringEncoder.encodeInto(buffer);
            return buffer;
        }
}

