#include <DevStudio/HlaLibSettings.h>
#include "DevStudio/HlaAllHeaders.h"

#include "Decoders.h"

namespace DevStudio
{
//pragma region Decoder Builder
// === AUTO-GENERATED BUILDER BEGIN ===
    Decoders::Decoders() :
        _aTP45HazardAreaTypeEnumDecoder(),
        _accelerationVectorStructDecoder(),
        _acknowledgeFlagEnumDecoder(),
        _acknowledgementProtocolEnumDecoder(),
        _actionEnumDecoder(),
        _actionResultEnumDecoder(),
        _activeSonarEnumDecoder(),
        _activeSonarFunctionCodeEnumDecoder(),
        _activeSonarScanPatternEnumDecoder(),
        _activeStatusEnumDecoder(),
        _administerTreatmentTaskStructDecoder(),
        _agentClassEnumDecoder(),
        _agentConcentrationStructDecoder(),
        _agentMassStructDecoder(),
        _agentTypeEnumDecoder(),
        _aggregateMarkingStructDecoder(),
        _aggregateMissionEnumDecoder(),
        _aggregateStateEnumDecoder(),
        _aggregateStateFormationEnumDecoder(),
        _aidTypeEnumDecoder(),
        _airFormationTypeEnumDecoder(),
        _amplitudeAngleModulationTypeEnumDecoder(),
        _amplitudeModulationTypeEnumDecoder(),
        _angleModulationTypeEnumDecoder(),
        _angularVelocityVectorStructDecoder(),
        _antennaPatternTypeEnumDecoder(),
        _antennaPatternVariantStructDecoder(),
        _antennaPatternVariantStructLengthlessArrayDecoder(),
        _antiCollisionLightFlashingEnumDecoder(),
        _appearanceAntiCollisionDayNightEnumDecoder(),
        _appearanceNVGModeEnumDecoder(),
        _applyIPETaskStructDecoder(),
        _appointmentStructDecoder(),
        _arrayOfAgentConcentrationStructDecoder(),
        _arrayOfAgentMassStructDecoder(),
        _arrayOfAgentTypeEnumDecoder(),
        _arrayOfCBRNExposureStructDecoder(),
        _arrayOfCBRNSensorReadingStructDecoder(),
        _arrayOfCommunicationNetworksDecoder(),
        _arrayOfContourStructDecoder(),
        _arrayOfDetectedEquipmentDecoder(),
        _arrayOfHoldingsDecoder(),
        _arrayOfObservedEquipmentDecoder(),
        _arrayOfProtectionEffectivenessStructDecoder(),
        _arrayOfRelationshipDecoder(),
        _arrayOfResourceStatusDecoder(),
        _arrayOfSensorDecoder(),
        _arrayOfSigmas6Decoder(),
        _arrayOfSignatureDecoder(),
        _arrayOfSupplyStatusDecoder(),
        _arrayOfTaskDefinitionsDecoder(),
        _arrayOfTaskProgressDecoder(),
        _arrayOfTrackDecoder(),
        _arrayOfTreatmentStructDecoder(),
        _arrayOfUuidDecoder(),
        _arrayOfWaypointsDecoder(),
        _articulatedParameterStructDecoder(),
        _articulatedParameterStructLengthlessArrayDecoder(),
        _articulatedPartsStructDecoder(),
        _articulatedPartsTypeEnumDecoder(),
        _articulatedTypeMetricEnumDecoder(),
        _attachTaskStructDecoder(),
        _attachedPartsStructDecoder(),
        _attackTypeEnumDecoder(),
        _attributeValuePairStructArray1PlusDecoder(),
        _attributeValuePairStructDecoder(),
        _audioDataTypeStructDecoder(),
        _baseDecoder(),
        _beamAntennaStructDecoder(),
        _beamFunctionCodeEnumDecoder(),
        _binArrayDecoder(),
        _breachStructDecoder(),
        _breachStructLengthlessArrayDecoder(),
        _breachableSegmentStructDecoder(),
        _breachableSegmentStructLengthlessArrayDecoder(),
        _breachedStatusArray8Decoder(),
        _breachedStatusEnumDecoder(),
        _cBRNAlarmStructDecoder(),
        _cBRNDamageEnumDecoder(),
        _cBRNExposureStructDecoder(),
        _cBRNSensorReadingStructDecoder(),
        _cOMBICStateRecStructDecoder(),
        _camouflageEnumDecoder(),
        _captureStatusEnumDecoder(),
        _changeAltitudeTaskStructDecoder(),
        _changeHeadingTaskStructDecoder(),
        _changeRulesOfEngagementTaskStructDecoder(),
        _changeSpeedTaskStructDecoder(),
        _chemicalContentEnumDecoder(),
        _clearEngineeringTaskStructDecoder(),
        _clockTimeStructDecoder(),
        _clockTimeStructLengthlessArrayDecoder(),
        _cloudStructDecoder(),
        _cloudTypeEnumDecoder(),
        _coefficientsLengthlessArray1PlusDecoder(),
        _collisionTypeEnumDecoder(),
        _combinationModulationTypeEnumDecoder(),
        _communicationNetworkArrayDecoder(),
        _communicationNetworkTypeEnumDecoder(),
        _communicationServiceTypeEnumDecoder(),
        _complianceStateEnumDecoder(),
        _cone1GeomRecStructDecoder(),
        _cone2GeomRecStructDecoder(),
        _connectionReceiverArrayDecoder(),
        _connectionReceiverStructDecoder(),
        _connectionTypeEnumDecoder(),
        _constituentPartNatureEnumDecoder(),
        _constituentPartPositionEnumDecoder(),
        _constituentPartRelationshipStructDecoder(),
        _constituentPartStationNameEnumDecoder(),
        _contourStructDecoder(),
        _createBreachTaskStructDecoder(),
        _createFireBreakTaskStructDecoder(),
        _createLeveeTaskStructDecoder(),
        _createObstacleTaskStructDecoder(),
        _cryptographicModeEnumDecoder(),
        _cryptographicSystemTypeEnumDecoder(),
        _currentStructDecoder(),
        _damageStatusEnhancedEnumDecoder(),
        _damageStatusEnumDecoder(),
        _datumIdentifierEnumDecoder(),
        _datumIdentifierLengthlessArrayDecoder(),
        _deadReckoningAlgorithmEnumDecoder(),
        _depthMeterFloat32LengthlessArrayDecoder(),
        _designatorCodeEnumDecoder(),
        _designatorCodeNameEnumDecoder(),
        _detectedEquipmentDecoder(),
        _detonationResultCodeEnumDecoder(),
        _dimensionRateStructDecoder(),
        _dimensionStructDecoder(),
        _directFireTaskStructDecoder(),
        _disruptCommunicationTaskStructDecoder(),
        _echelonEnumDecoder(),
        _elapsedTimeProgressDecoder(),
        _ellipsoid1GeomRecStructDecoder(),
        _ellipsoid2GeomRecStructDecoder(),
        _emitterFunctionEnumDecoder(),
        _emitterTypeEnumDecoder(),
        _emptyTaskStructDecoder(),
        _encodingTypeEnumDecoder(),
        _engineeringTaskStructDecoder(),
        _entityControlActionEnumDecoder(),
        _entityControlActionsDecoder(),
        _entityCoordinateVectorStructDecoder(),
        _entityIdentifierStructDecoder(),
        _entityTypeStructDecoder(),
        _entityTypeStructLengthlessArrayDecoder(),
        _environmentConditionModelTypeEnumDecoder(),
        _environmentDataCoordinateSystemEnumDecoder(),
        _environmentDataRepresentationEnumDecoder(),
        _environmentDataSampleTypeEnumDecoder(),
        _environmentGridAxisTypeEnumDecoder(),
        _environmentGridTypeEnumDecoder(),
        _environmentModelTypeEnumDecoder(),
        _environmentObjectTypeStructDecoder(),
        _environmentRecStructArrayDecoder(),
        _environmentRecStructDecoder(),
        _environmentRecVariantStructDecoder(),
        _environmentRecordTypeEnumDecoder(),
        _environmentTypeStructDecoder(),
        _equipmentSymbolAmplificationStructDecoder(),
        _establishCheckpointTaskStructDecoder(),
        _establishObservationPostTaskStructDecoder(),
        _evacuateProgressStructDecoder(),
        _evacuateTaskStructDecoder(),
        _eventIdentifierStructDecoder(),
        _eventTypeEnumDecoder(),
        _exhaustSmokeStructDecoder(),
        _exhaustSmokeStructLengthlessArrayDecoder(),
        _exposureTypeEnumDecoder(),
        _extinguishWildfireTaskStructDecoder(),
        _federateControlActionEnumDecoder(),
        _federateControlActionsDecoder(),
        _federateIdentifierStructDecoder(),
        _fireTaskProgressDecoder(),
        _fixedDatumStructDecoder(),
        _fixedDatumStructLengthlessArrayDecoder(),
        _flareStateRecStructDecoder(),
        _float32Array1PlusDecoder(),
        _followEntityTaskStructDecoder(),
        _forceIdentifierEnumDecoder(),
        _formationDataStructDecoder(),
        _formationEnumDecoder(),
        _formationLocationTypeEnumDecoder(),
        _formationStructDecoder(),
        _formationTypeEnumDecoder(),
        _fundamentalParameterDataStructDecoder(),
        _fundamentalParameterDataStructLengthlessArrayDecoder(),
        _fuseTypeEnumDecoder(),
        _gaussPlumeGeomRecStructDecoder(),
        _gaussPuffGeomRecStructDecoder(),
        _geoLocationTypeEnumDecoder(),
        _geoReferenceVariantDecoder(),
        _geodeticCircleDecoder(),
        _geodeticLocationDecoder(),
        _geodeticPathDecoder(),
        _geodeticPointDecoder(),
        _geodeticPolygonDecoder(),
        _geodeticQuadrangleDecoder(),
        _gridAxisStructDecoder(),
        _gridAxisStructLengthlessArrayDecoder(),
        _gridAxisTypeVariantStructDecoder(),
        _gridDataRepresentationVariantStructDecoder(),
        _gridDataStructDecoder(),
        _gridDataStructLengthlessArrayDecoder(),
        _gridValueType0StructDecoder(),
        _gridValueType1StructDecoder(),
        _gridValueType2StructDecoder(),
        _groundFormationTypeEnumDecoder(),
        _hLAargumentListDecoder(),
        _hLAfederateStateDecoder(),
        _hLAhandleListDecoder(),
        _hLAinteractionCountDecoder(),
        _hLAinteractionCountsDecoder(),
        _hLAinteractionSubListDecoder(),
        _hLAinteractionSubscriptionDecoder(),
        _hLAmoduleDesignatorListDecoder(),
        _hLAobjectClassBasedCountDecoder(),
        _hLAobjectClassBasedCountsDecoder(),
        _hLAorderTypeDecoder(),
        _hLAownershipDecoder(),
        _hLAresignActionDecoder(),
        _hLAswitchDecoder(),
        _hLAsynchPointFederateDecoder(),
        _hLAsynchPointFederateListDecoder(),
        _hLAsynchPointListDecoder(),
        _hLAsynchPointStatusDecoder(),
        _hLAtimeStateDecoder(),
        _hatchStateEnumDecoder(),
        _hazardTypeEnum2Decoder(),
        _hazardTypeEnumDecoder(),
        _hazeStructDecoder(),
        _hazeTypeEnumDecoder(),
        _holdingDecoder(),
        _hostilityStatusCodeEnumDecoder(),
        _iPETypeEnumDecoder(),
        _iceStructDecoder(),
        _iceTypeEnumDecoder(),
        _identificationLevelEnumDecoder(),
        _iffAlternateMode4EnumDecoder(),
        _iffApplicableModesEnumDecoder(),
        _iffOperationalParameter1EnumDecoder(),
        _iffOperationalParameter2EnumDecoder(),
        _iffSystemModeEnumDecoder(),
        _iffSystemNameEnumDecoder(),
        _iffSystemTypeEnumDecoder(),
        _incomingConnectionArrayDecoder(),
        _incomingConnectionStructDecoder(),
        _indirectFireTaskStructDecoder(),
        _installationSymbolAmplificationStructDecoder(),
        _integer16Array1PlusDecoder(),
        _irregularGridAxisStructDecoder(),
        _isPartOfStructDecoder(),
        _landslideTypeEnumDecoder(),
        _layMinefieldTaskStructDecoder(),
        _layerStructDecoder(),
        _lightningTypeEnumDecoder(),
        _line1GeomRecStructDecoder(),
        _line2GeomRecStructDecoder(),
        _linearSegmentStructDecoder(),
        _linkStatusArrayDecoder(),
        _linkStatusStructDecoder(),
        _locationStructArrayDecoder(),
        _locationStructDecoder(),
        _magicMoveTaskStructDecoder(),
        _majorRFModulationTypeEnumDecoder(),
        _maneuverIndicatorEnumDecoder(),
        _manufacturerIdArray3Decoder(),
        _markingArray11Decoder(),
        _markingArray31Decoder(),
        _markingEncodingEnumDecoder(),
        _markingStructDecoder(),
        _mineDielectricDifferenceLengthlessArrayDecoder(),
        _mineFusingStructDecoder(),
        _mineFusingStructLengthlessArrayDecoder(),
        _mineIdentifierLengthlessArrayDecoder(),
        _minefieldFusingEnumDecoder(),
        _minefieldLaneEnumDecoder(),
        _minefieldLaneMarkerStructDecoder(),
        _minefieldLaneMarkerStructLengthlessArrayDecoder(),
        _minefieldPaintSchemeEnumDecoder(),
        _minefieldPaintSchemeLengthlessArrayDecoder(),
        _minefieldProtocolEnumDecoder(),
        _minefieldSensorTypeEnumDecoder(),
        _minefieldSensorTypeLengthlessArrayDecoder(),
        _minefieldStatusEnumDecoder(),
        _minefieldTypeEnumDecoder(),
        _missingRecordNumbersLengthlessArray1PlusDecoder(),
        _moveByRouteTaskStructDecoder(),
        _moveInDirectionTaskStructDecoder(),
        _moveIntoFormationTaskStructDecoder(),
        _moveTaskProgressDecoder(),
        _moveToLocationTaskStructDecoder(),
        _moveTypeEnumDecoder(),
        _msgIdEnumDecoder(),
        _namedLocationStructDecoder(),
        _navigationLightFlashingEnumDecoder(),
        _navigationStatusEnumDecoder(),
        _networkDeviceArrayDecoder(),
        _networkDeviceEmptyCharactersticsStructDecoder(),
        _networkDeviceGenericTransmitterCharacteristicsStructDecoder(),
        _networkDeviceReceiverCharacteristicsVariantDecoder(),
        _networkDeviceStructDecoder(),
        _networkDeviceTransmitterCharacteristicsVariantDecoder(),
        _nomenclatureEnumDecoder(),
        _nomenclatureVersionEnumDecoder(),
        _observeTaskStructDecoder(),
        _observedEquipmentDecoder(),
        _octetArray1PlusDecoder(),
        _octetArray2Decoder(),
        _octetArray3Decoder(),
        _octetArray4Decoder(),
        _octetArray7Decoder(),
        _octetArray8Decoder(),
        _octetArrayDecoder(),
        _octetPadding32ArrayDecoder(),
        _octetPadding64ArrayDecoder(),
        _opacityCodeEnumDecoder(),
        _operateCheckpointTaskStructDecoder(),
        _operateObservationPostTaskStructDecoder(),
        _orientationStructDecoder(),
        _orientationStructLengthlessArrayDecoder(),
        _otherActivityTaskStructDecoder(),
        _parameterTypeEnumDecoder(),
        _parameterValueVariantStructDecoder(),
        _partNumberEnumDecoder(),
        _patrolMoveTypeEnumDecoder(),
        _patrolTaskProgressDecoder(),
        _patrolTaskStructDecoder(),
        _patrolTypeEnumDecoder(),
        _perimeterPointStructDecoder(),
        _perimeterPointStructLengthlessArrayDecoder(),
        _physicalGenericNetworkStructDecoder(),
        _physicalNetworkDescriptionVariantDecoder(),
        _physicalNetworkTypeEnumDecoder(),
        _physicalUndefinedNetworkStructDecoder(),
        _plumeDimensionRateStructDecoder(),
        _plumeDimensionStructDecoder(),
        _point2GeomRecStructDecoder(),
        _precipitationStructDecoder(),
        _precipitationTypeEnumDecoder(),
        _propulsionPlantEnumDecoder(),
        _propulsionSystemDataStructDecoder(),
        _propulsionSystemDataStructLengthlessArrayDecoder(),
        _protectionEffectivenessStructDecoder(),
        _pulseModulationTypeEnumDecoder(),
        _pumpFloodingTaskStructDecoder(),
        _rFModulationSystemTypeEnumDecoder(),
        _rFModulationTypeVariantStructDecoder(),
        _rTIobjectIdArrayDecoder(),
        _radioInputSourceEnumDecoder(),
        _radioTypeStructDecoder(),
        _receiverOperationalStatusEnumDecoder(),
        _recordSetStructArray1PlusDecoder(),
        _recordSetStructDecoder(),
        _recordStructArrayDecoder(),
        _recordStructDecoder(),
        _rectVol1GeomRecStructDecoder(),
        _rectVol2GeomRecStructDecoder(),
        _rectVol3GeomRecStructDecoder(),
        _referenceSystemEnumDecoder(),
        _refuelingStationLightColorEnumDecoder(),
        _refuelingStationLightDayNightEnumDecoder(),
        _relationDecoder(),
        _relativePositionStructDecoder(),
        _relativeRangeBearingStructDecoder(),
        _releaseDistributionStructDecoder(),
        _releaseDynamicsStructDecoder(),
        _releaseSizeStructDecoder(),
        _repairResultEnumDecoder(),
        _repairTaskStructDecoder(),
        _repairTypeEnumDecoder(),
        _requestStatusEnumDecoder(),
        _requestedConnectionArrayDecoder(),
        _requestedConnectionDecoder(),
        _resourceStatusStructDecoder(),
        _responseFlagEnumDecoder(),
        _resupplyTaskStructDecoder(),
        _roadIceConditionEnumDecoder(),
        _roundStructDecoder(),
        _rulesOfEngagementEnumDecoder(),
        _sINCGARSModulationStructDecoder(),
        _seaStateEnumDecoder(),
        _sedimentTypeEnumDecoder(),
        _sendSafetyRelatedBroadcastMessageTaskStructDecoder(),
        _sendSafetyRelatedMessageTaskStructDecoder(),
        _sensorStateEnumDecoder(),
        _sensorStructDecoder(),
        _serviceTypeEnumDecoder(),
        _setTransmitterStatusTaskStructDecoder(),
        _shaftDataStructDecoder(),
        _shaftDataStructLengthlessArray1PlusDecoder(),
        _shipTypeEnumDecoder(),
        _signalDataLengthlessArray1PlusDecoder(),
        _signatureTypeEnumDecoder(),
        _signatureVariantDecoder(),
        _silentAggregateStructDecoder(),
        _silentAggregateStructLengthlessArrayDecoder(),
        _silentEntityStructDecoder(),
        _silentEntityStructLengthlessArrayDecoder(),
        _snowStructDecoder(),
        _spatialFPStructDecoder(),
        _spatialFVStructDecoder(),
        _spatialRPStructDecoder(),
        _spatialRVStructDecoder(),
        _spatialStaticStructDecoder(),
        _spatialVariantStructDecoder(),
        _sphere1GeomRecStructDecoder(),
        _sphere2GeomRecStructDecoder(),
        _sphericalHarmonicAntennaStructDecoder(),
        _spreadSpectrumEnumDecoder(),
        _spreadSpectrumVariantStructDecoder(),
        _stanceCodeEnumDecoder(),
        _stationEnumDecoder(),
        _stationNameLocationVariantStructDecoder(),
        _stopFreezeReasonEnumDecoder(),
        _subsurfaceFormationTypeEnumDecoder(),
        _supplyStatusStructDecoder(),
        _supplyStructDecoder(),
        _supplyStructLengthlessArrayDecoder(),
        _surfaceFormationTypeEnumDecoder(),
        _surfaceMoistureEnumDecoder(),
        _symbolAmplificationVariantDecoder(),
        _symbolStructDecoder(),
        _symbolTypeEnumDecoder(),
        _tacticalDataLinkTypeEnumDecoder(),
        _taskDefinitionDecoder(),
        _taskDefinitionVariantRecordDecoder(),
        _taskModeEnumDecoder(),
        _taskProgressDecoder(),
        _taskProgressVariantRecordDecoder(),
        _taskStatusEnumDecoder(),
        _temperatureDegreeCelsiusFloat32LengthlessArrayDecoder(),
        _trackStructDecoder(),
        _trailingEffectsCodeEnumDecoder(),
        _transferTypeEnumDecoder(),
        _transmitterOperationalStatusEnumDecoder(),
        _transportTaskStructDecoder(),
        _treatmentStructDecoder(),
        _uUIDDecoder(),
        _uniformGeomRecStructDecoder(),
        _unitSymbolAmplificationStructDecoder(),
        _unmodulatedTypeEnumDecoder(),
        _unsignedInteger16Array1PlusDecoder(),
        _unsignedInteger32LengthlessArrayDecoder(),
        _unsignedInteger64Array1PlusDecoder(),
        _unsignedInteger8LengthlessArrayDecoder(),
        _userProtocolEnumDecoder(),
        _variableDatumStructArrayDecoder(),
        _variableDatumStructDecoder(),
        _variableDatumStructLengthlessArrayDecoder(),
        _vectoringNozzleSystemDataStructDecoder(),
        _vectoringNozzleSystemDataStructLengthlessArrayDecoder(),
        _velocityVectorStructDecoder(),
        _visibleSideLocationEnumDecoder(),
        _warheadTypeEnumDecoder(),
        _waveStructDecoder(),
        _waypointDecoder(),
        _weaponControlOrderEnumDecoder(),
        _weaponStateEnumDecoder(),
        _windStructDecoder(),
        _worldLocationStructDecoder(),
        _worldLocationStructLengthlessArrayDecoder()
// === AUTO-GENERATED BUILDER END ===
//pragma endregion
	{
	}

	Decoders::~Decoders() {}

//pragma region Auto-generated Functions
// === AUTO-GENERATED FUNCTIONS BEGIN ===
    DevStudio::ATP45HazardAreaTypeEnum::ATP45HazardAreaTypeEnum Decoders::decodeATP45HazardAreaTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _aTP45HazardAreaTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_aTP45HazardAreaTypeEnumDecoder.getEncodedLength());
        _aTP45HazardAreaTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ATP45HazardAreaTypeEnum::ATP45HazardAreaTypeEnum(_aTP45HazardAreaTypeEnumDecoder);
    }
    DevStudio::AccelerationVectorStruct Decoders::decodeAccelerationVectorStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _accelerationVectorStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_accelerationVectorStructDecoder.getEncodedLength());
        _accelerationVectorStructDecoder.encodeInto(buffer);
        return DevStudio::AccelerationVectorStruct(_accelerationVectorStructDecoder);
    }
    DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum Decoders::decodeAcknowledgeFlagEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _acknowledgeFlagEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_acknowledgeFlagEnumDecoder.getEncodedLength());
        _acknowledgeFlagEnumDecoder.encodeInto(buffer);
        return DevStudio::AcknowledgeFlagEnum::AcknowledgeFlagEnum(_acknowledgeFlagEnumDecoder);
    }
    DevStudio::AcknowledgementProtocolEnum::AcknowledgementProtocolEnum Decoders::decodeAcknowledgementProtocolEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _acknowledgementProtocolEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_acknowledgementProtocolEnumDecoder.getEncodedLength());
        _acknowledgementProtocolEnumDecoder.encodeInto(buffer);
        return DevStudio::AcknowledgementProtocolEnum::AcknowledgementProtocolEnum(_acknowledgementProtocolEnumDecoder);
    }
    DevStudio::ActionEnum::ActionEnum Decoders::decodeActionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _actionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_actionEnumDecoder.getEncodedLength());
        _actionEnumDecoder.encodeInto(buffer);
        return DevStudio::ActionEnum::ActionEnum(_actionEnumDecoder);
    }
    DevStudio::ActionResultEnum::ActionResultEnum Decoders::decodeActionResultEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _actionResultEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_actionResultEnumDecoder.getEncodedLength());
        _actionResultEnumDecoder.encodeInto(buffer);
        return DevStudio::ActionResultEnum::ActionResultEnum(_actionResultEnumDecoder);
    }
    DevStudio::ActiveSonarEnum::ActiveSonarEnum Decoders::decodeActiveSonarEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _activeSonarEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_activeSonarEnumDecoder.getEncodedLength());
        _activeSonarEnumDecoder.encodeInto(buffer);
        return DevStudio::ActiveSonarEnum::ActiveSonarEnum(_activeSonarEnumDecoder);
    }
    DevStudio::ActiveSonarFunctionCodeEnum::ActiveSonarFunctionCodeEnum Decoders::decodeActiveSonarFunctionCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _activeSonarFunctionCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_activeSonarFunctionCodeEnumDecoder.getEncodedLength());
        _activeSonarFunctionCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::ActiveSonarFunctionCodeEnum::ActiveSonarFunctionCodeEnum(_activeSonarFunctionCodeEnumDecoder);
    }
    DevStudio::ActiveSonarScanPatternEnum::ActiveSonarScanPatternEnum Decoders::decodeActiveSonarScanPatternEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _activeSonarScanPatternEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_activeSonarScanPatternEnumDecoder.getEncodedLength());
        _activeSonarScanPatternEnumDecoder.encodeInto(buffer);
        return DevStudio::ActiveSonarScanPatternEnum::ActiveSonarScanPatternEnum(_activeSonarScanPatternEnumDecoder);
    }
    DevStudio::ActiveStatusEnum::ActiveStatusEnum Decoders::decodeActiveStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _activeStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_activeStatusEnumDecoder.getEncodedLength());
        _activeStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::ActiveStatusEnum::ActiveStatusEnum(_activeStatusEnumDecoder);
    }
    DevStudio::AdministerTreatmentTaskStruct Decoders::decodeAdministerTreatmentTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _administerTreatmentTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_administerTreatmentTaskStructDecoder.getEncodedLength());
        _administerTreatmentTaskStructDecoder.encodeInto(buffer);
        return DevStudio::AdministerTreatmentTaskStruct(_administerTreatmentTaskStructDecoder);
    }
    DevStudio::AgentClassEnum::AgentClassEnum Decoders::decodeAgentClassEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _agentClassEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_agentClassEnumDecoder.getEncodedLength());
        _agentClassEnumDecoder.encodeInto(buffer);
        return DevStudio::AgentClassEnum::AgentClassEnum(_agentClassEnumDecoder);
    }
    DevStudio::AgentConcentrationStruct Decoders::decodeAgentConcentrationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _agentConcentrationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_agentConcentrationStructDecoder.getEncodedLength());
        _agentConcentrationStructDecoder.encodeInto(buffer);
        return DevStudio::AgentConcentrationStruct(_agentConcentrationStructDecoder);
    }
    DevStudio::AgentMassStruct Decoders::decodeAgentMassStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _agentMassStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_agentMassStructDecoder.getEncodedLength());
        _agentMassStructDecoder.encodeInto(buffer);
        return DevStudio::AgentMassStruct(_agentMassStructDecoder);
    }
    DevStudio::AgentTypeEnum::AgentTypeEnum Decoders::decodeAgentTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _agentTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_agentTypeEnumDecoder.getEncodedLength());
        _agentTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AgentTypeEnum::AgentTypeEnum(_agentTypeEnumDecoder);
    }
    DevStudio::AggregateMarkingStruct Decoders::decodeAggregateMarkingStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _aggregateMarkingStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_aggregateMarkingStructDecoder.getEncodedLength());
        _aggregateMarkingStructDecoder.encodeInto(buffer);
        return DevStudio::AggregateMarkingStruct(_aggregateMarkingStructDecoder);
    }
    DevStudio::AggregateMissionEnum::AggregateMissionEnum Decoders::decodeAggregateMissionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _aggregateMissionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_aggregateMissionEnumDecoder.getEncodedLength());
        _aggregateMissionEnumDecoder.encodeInto(buffer);
        return DevStudio::AggregateMissionEnum::AggregateMissionEnum(_aggregateMissionEnumDecoder);
    }
    DevStudio::AggregateStateEnum::AggregateStateEnum Decoders::decodeAggregateStateEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _aggregateStateEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_aggregateStateEnumDecoder.getEncodedLength());
        _aggregateStateEnumDecoder.encodeInto(buffer);
        return DevStudio::AggregateStateEnum::AggregateStateEnum(_aggregateStateEnumDecoder);
    }
    DevStudio::AggregateStateFormationEnum::AggregateStateFormationEnum Decoders::decodeAggregateStateFormationEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _aggregateStateFormationEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_aggregateStateFormationEnumDecoder.getEncodedLength());
        _aggregateStateFormationEnumDecoder.encodeInto(buffer);
        return DevStudio::AggregateStateFormationEnum::AggregateStateFormationEnum(_aggregateStateFormationEnumDecoder);
    }
    DevStudio::AidTypeEnum::AidTypeEnum Decoders::decodeAidTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _aidTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_aidTypeEnumDecoder.getEncodedLength());
        _aidTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AidTypeEnum::AidTypeEnum(_aidTypeEnumDecoder);
    }
    DevStudio::AirFormationTypeEnum::AirFormationTypeEnum Decoders::decodeAirFormationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _airFormationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_airFormationTypeEnumDecoder.getEncodedLength());
        _airFormationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AirFormationTypeEnum::AirFormationTypeEnum(_airFormationTypeEnumDecoder);
    }
    DevStudio::AmplitudeAngleModulationTypeEnum::AmplitudeAngleModulationTypeEnum Decoders::decodeAmplitudeAngleModulationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _amplitudeAngleModulationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_amplitudeAngleModulationTypeEnumDecoder.getEncodedLength());
        _amplitudeAngleModulationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AmplitudeAngleModulationTypeEnum::AmplitudeAngleModulationTypeEnum(_amplitudeAngleModulationTypeEnumDecoder);
    }
    DevStudio::AmplitudeModulationTypeEnum::AmplitudeModulationTypeEnum Decoders::decodeAmplitudeModulationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _amplitudeModulationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_amplitudeModulationTypeEnumDecoder.getEncodedLength());
        _amplitudeModulationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AmplitudeModulationTypeEnum::AmplitudeModulationTypeEnum(_amplitudeModulationTypeEnumDecoder);
    }
    DevStudio::AngleModulationTypeEnum::AngleModulationTypeEnum Decoders::decodeAngleModulationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _angleModulationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_angleModulationTypeEnumDecoder.getEncodedLength());
        _angleModulationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AngleModulationTypeEnum::AngleModulationTypeEnum(_angleModulationTypeEnumDecoder);
    }
    DevStudio::AngularVelocityVectorStruct Decoders::decodeAngularVelocityVectorStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _angularVelocityVectorStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_angularVelocityVectorStructDecoder.getEncodedLength());
        _angularVelocityVectorStructDecoder.encodeInto(buffer);
        return DevStudio::AngularVelocityVectorStruct(_angularVelocityVectorStructDecoder);
    }
    DevStudio::AntennaPatternTypeEnum::AntennaPatternTypeEnum Decoders::decodeAntennaPatternTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _antennaPatternTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_antennaPatternTypeEnumDecoder.getEncodedLength());
        _antennaPatternTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AntennaPatternTypeEnum::AntennaPatternTypeEnum(_antennaPatternTypeEnumDecoder);
    }
    DevStudio::AntennaPatternVariantStruct Decoders::decodeAntennaPatternVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _antennaPatternVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_antennaPatternVariantStructDecoder.getEncodedLength());
        _antennaPatternVariantStructDecoder.encodeInto(buffer);
        return DevStudio::AntennaPatternVariantStruct(_antennaPatternVariantStructDecoder);
    }
    std::vector< DevStudio::AntennaPatternVariantStruct > Decoders::decodeAntennaPatternVariantStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _antennaPatternVariantStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_antennaPatternVariantStructLengthlessArrayDecoder.getEncodedLength());
        _antennaPatternVariantStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::AntennaPatternVariantStruct >(_antennaPatternVariantStructLengthlessArrayDecoder);
    }
    DevStudio::AntiCollisionLightFlashingEnum::AntiCollisionLightFlashingEnum Decoders::decodeAntiCollisionLightFlashingEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _antiCollisionLightFlashingEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_antiCollisionLightFlashingEnumDecoder.getEncodedLength());
        _antiCollisionLightFlashingEnumDecoder.encodeInto(buffer);
        return DevStudio::AntiCollisionLightFlashingEnum::AntiCollisionLightFlashingEnum(_antiCollisionLightFlashingEnumDecoder);
    }
    DevStudio::AppearanceAntiCollisionDayNightEnum::AppearanceAntiCollisionDayNightEnum Decoders::decodeAppearanceAntiCollisionDayNightEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _appearanceAntiCollisionDayNightEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_appearanceAntiCollisionDayNightEnumDecoder.getEncodedLength());
        _appearanceAntiCollisionDayNightEnumDecoder.encodeInto(buffer);
        return DevStudio::AppearanceAntiCollisionDayNightEnum::AppearanceAntiCollisionDayNightEnum(_appearanceAntiCollisionDayNightEnumDecoder);
    }
    DevStudio::AppearanceNVGModeEnum::AppearanceNVGModeEnum Decoders::decodeAppearanceNVGModeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _appearanceNVGModeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_appearanceNVGModeEnumDecoder.getEncodedLength());
        _appearanceNVGModeEnumDecoder.encodeInto(buffer);
        return DevStudio::AppearanceNVGModeEnum::AppearanceNVGModeEnum(_appearanceNVGModeEnumDecoder);
    }
    DevStudio::ApplyIPETaskStruct Decoders::decodeApplyIPETaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _applyIPETaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_applyIPETaskStructDecoder.getEncodedLength());
        _applyIPETaskStructDecoder.encodeInto(buffer);
        return DevStudio::ApplyIPETaskStruct(_applyIPETaskStructDecoder);
    }
    DevStudio::AppointmentStruct Decoders::decodeAppointmentStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _appointmentStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_appointmentStructDecoder.getEncodedLength());
        _appointmentStructDecoder.encodeInto(buffer);
        return DevStudio::AppointmentStruct(_appointmentStructDecoder);
    }
    std::vector< DevStudio::AgentConcentrationStruct > Decoders::decodeArrayOfAgentConcentrationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfAgentConcentrationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfAgentConcentrationStructDecoder.getEncodedLength());
        _arrayOfAgentConcentrationStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::AgentConcentrationStruct >(_arrayOfAgentConcentrationStructDecoder);
    }
    std::vector< DevStudio::AgentMassStruct > Decoders::decodeArrayOfAgentMassStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfAgentMassStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfAgentMassStructDecoder.getEncodedLength());
        _arrayOfAgentMassStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::AgentMassStruct >(_arrayOfAgentMassStructDecoder);
    }
    std::vector< DevStudio::AgentTypeEnum::AgentTypeEnum > Decoders::decodeArrayOfAgentTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfAgentTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfAgentTypeEnumDecoder.getEncodedLength());
        _arrayOfAgentTypeEnumDecoder.encodeInto(buffer);
        return std::vector< DevStudio::AgentTypeEnum::AgentTypeEnum >(_arrayOfAgentTypeEnumDecoder);
    }
    std::vector< DevStudio::CBRNExposureStruct > Decoders::decodeArrayOfCBRNExposureStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfCBRNExposureStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfCBRNExposureStructDecoder.getEncodedLength());
        _arrayOfCBRNExposureStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::CBRNExposureStruct >(_arrayOfCBRNExposureStructDecoder);
    }
    std::vector< DevStudio::CBRNSensorReadingStruct > Decoders::decodeArrayOfCBRNSensorReadingStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfCBRNSensorReadingStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfCBRNSensorReadingStructDecoder.getEncodedLength());
        _arrayOfCBRNSensorReadingStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::CBRNSensorReadingStruct >(_arrayOfCBRNSensorReadingStructDecoder);
    }
    std::vector< std::vector< char > > Decoders::decodeArrayOfCommunicationNetworks(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfCommunicationNetworksDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfCommunicationNetworksDecoder.getEncodedLength());
        _arrayOfCommunicationNetworksDecoder.encodeInto(buffer);
        return std::vector< std::vector< char > >(_arrayOfCommunicationNetworksDecoder);
    }
    std::vector< DevStudio::ContourStruct > Decoders::decodeArrayOfContourStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfContourStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfContourStructDecoder.getEncodedLength());
        _arrayOfContourStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ContourStruct >(_arrayOfContourStructDecoder);
    }
    std::vector< DevStudio::DetectedEquipment > Decoders::decodeArrayOfDetectedEquipment(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfDetectedEquipmentDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfDetectedEquipmentDecoder.getEncodedLength());
        _arrayOfDetectedEquipmentDecoder.encodeInto(buffer);
        return std::vector< DevStudio::DetectedEquipment >(_arrayOfDetectedEquipmentDecoder);
    }
    std::vector< DevStudio::Holding > Decoders::decodeArrayOfHoldings(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfHoldingsDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfHoldingsDecoder.getEncodedLength());
        _arrayOfHoldingsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::Holding >(_arrayOfHoldingsDecoder);
    }
    std::vector< DevStudio::ObservedEquipment > Decoders::decodeArrayOfObservedEquipment(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfObservedEquipmentDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfObservedEquipmentDecoder.getEncodedLength());
        _arrayOfObservedEquipmentDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ObservedEquipment >(_arrayOfObservedEquipmentDecoder);
    }
    std::vector< DevStudio::ProtectionEffectivenessStruct > Decoders::decodeArrayOfProtectionEffectivenessStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfProtectionEffectivenessStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfProtectionEffectivenessStructDecoder.getEncodedLength());
        _arrayOfProtectionEffectivenessStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ProtectionEffectivenessStruct >(_arrayOfProtectionEffectivenessStructDecoder);
    }
    std::vector< DevStudio::Relation > Decoders::decodeArrayOfRelationship(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfRelationshipDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfRelationshipDecoder.getEncodedLength());
        _arrayOfRelationshipDecoder.encodeInto(buffer);
        return std::vector< DevStudio::Relation >(_arrayOfRelationshipDecoder);
    }
    std::vector< DevStudio::ResourceStatusStruct > Decoders::decodeArrayOfResourceStatus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfResourceStatusDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfResourceStatusDecoder.getEncodedLength());
        _arrayOfResourceStatusDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ResourceStatusStruct >(_arrayOfResourceStatusDecoder);
    }
    std::vector< DevStudio::SensorStruct > Decoders::decodeArrayOfSensor(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfSensorDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfSensorDecoder.getEncodedLength());
        _arrayOfSensorDecoder.encodeInto(buffer);
        return std::vector< DevStudio::SensorStruct >(_arrayOfSensorDecoder);
    }
    std::vector< float > Decoders::decodeArrayOfSigmas6(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfSigmas6Decoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfSigmas6Decoder.getEncodedLength());
        _arrayOfSigmas6Decoder.encodeInto(buffer);
        return std::vector< float >(_arrayOfSigmas6Decoder);
    }
    std::vector< DevStudio::SignatureVariant > Decoders::decodeArrayOfSignature(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfSignatureDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfSignatureDecoder.getEncodedLength());
        _arrayOfSignatureDecoder.encodeInto(buffer);
        return std::vector< DevStudio::SignatureVariant >(_arrayOfSignatureDecoder);
    }
    std::vector< DevStudio::SupplyStatusStruct > Decoders::decodeArrayOfSupplyStatus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfSupplyStatusDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfSupplyStatusDecoder.getEncodedLength());
        _arrayOfSupplyStatusDecoder.encodeInto(buffer);
        return std::vector< DevStudio::SupplyStatusStruct >(_arrayOfSupplyStatusDecoder);
    }
    std::vector< DevStudio::TaskDefinition > Decoders::decodeArrayOfTaskDefinitions(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfTaskDefinitionsDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfTaskDefinitionsDecoder.getEncodedLength());
        _arrayOfTaskDefinitionsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::TaskDefinition >(_arrayOfTaskDefinitionsDecoder);
    }
    std::vector< DevStudio::TaskProgress > Decoders::decodeArrayOfTaskProgress(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfTaskProgressDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfTaskProgressDecoder.getEncodedLength());
        _arrayOfTaskProgressDecoder.encodeInto(buffer);
        return std::vector< DevStudio::TaskProgress >(_arrayOfTaskProgressDecoder);
    }
    std::vector< DevStudio::TrackStruct > Decoders::decodeArrayOfTrack(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfTrackDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfTrackDecoder.getEncodedLength());
        _arrayOfTrackDecoder.encodeInto(buffer);
        return std::vector< DevStudio::TrackStruct >(_arrayOfTrackDecoder);
    }
    std::vector< DevStudio::TreatmentStruct > Decoders::decodeArrayOfTreatmentStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfTreatmentStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfTreatmentStructDecoder.getEncodedLength());
        _arrayOfTreatmentStructDecoder.encodeInto(buffer);
        return std::vector< DevStudio::TreatmentStruct >(_arrayOfTreatmentStructDecoder);
    }
    std::vector< std::vector< char > > Decoders::decodeArrayOfUuid(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfUuidDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfUuidDecoder.getEncodedLength());
        _arrayOfUuidDecoder.encodeInto(buffer);
        return std::vector< std::vector< char > >(_arrayOfUuidDecoder);
    }
    std::vector< DevStudio::Waypoint > Decoders::decodeArrayOfWaypoints(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _arrayOfWaypointsDecoder.decodeFrom(data, 0);
        buffer.reserve(_arrayOfWaypointsDecoder.getEncodedLength());
        _arrayOfWaypointsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::Waypoint >(_arrayOfWaypointsDecoder);
    }
    DevStudio::ArticulatedParameterStruct Decoders::decodeArticulatedParameterStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _articulatedParameterStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_articulatedParameterStructDecoder.getEncodedLength());
        _articulatedParameterStructDecoder.encodeInto(buffer);
        return DevStudio::ArticulatedParameterStruct(_articulatedParameterStructDecoder);
    }
    std::vector< DevStudio::ArticulatedParameterStruct > Decoders::decodeArticulatedParameterStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _articulatedParameterStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_articulatedParameterStructLengthlessArrayDecoder.getEncodedLength());
        _articulatedParameterStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ArticulatedParameterStruct >(_articulatedParameterStructLengthlessArrayDecoder);
    }
    DevStudio::ArticulatedPartsStruct Decoders::decodeArticulatedPartsStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _articulatedPartsStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_articulatedPartsStructDecoder.getEncodedLength());
        _articulatedPartsStructDecoder.encodeInto(buffer);
        return DevStudio::ArticulatedPartsStruct(_articulatedPartsStructDecoder);
    }
    DevStudio::ArticulatedPartsTypeEnum::ArticulatedPartsTypeEnum Decoders::decodeArticulatedPartsTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _articulatedPartsTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_articulatedPartsTypeEnumDecoder.getEncodedLength());
        _articulatedPartsTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ArticulatedPartsTypeEnum::ArticulatedPartsTypeEnum(_articulatedPartsTypeEnumDecoder);
    }
    DevStudio::ArticulatedTypeMetricEnum::ArticulatedTypeMetricEnum Decoders::decodeArticulatedTypeMetricEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _articulatedTypeMetricEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_articulatedTypeMetricEnumDecoder.getEncodedLength());
        _articulatedTypeMetricEnumDecoder.encodeInto(buffer);
        return DevStudio::ArticulatedTypeMetricEnum::ArticulatedTypeMetricEnum(_articulatedTypeMetricEnumDecoder);
    }
    DevStudio::AttachTaskStruct Decoders::decodeAttachTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _attachTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_attachTaskStructDecoder.getEncodedLength());
        _attachTaskStructDecoder.encodeInto(buffer);
        return DevStudio::AttachTaskStruct(_attachTaskStructDecoder);
    }
    DevStudio::AttachedPartsStruct Decoders::decodeAttachedPartsStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _attachedPartsStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_attachedPartsStructDecoder.getEncodedLength());
        _attachedPartsStructDecoder.encodeInto(buffer);
        return DevStudio::AttachedPartsStruct(_attachedPartsStructDecoder);
    }
    DevStudio::AttackTypeEnum::AttackTypeEnum Decoders::decodeAttackTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _attackTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_attackTypeEnumDecoder.getEncodedLength());
        _attackTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::AttackTypeEnum::AttackTypeEnum(_attackTypeEnumDecoder);
    }
    DevStudio::AttributeValuePairStruct Decoders::decodeAttributeValuePairStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _attributeValuePairStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_attributeValuePairStructDecoder.getEncodedLength());
        _attributeValuePairStructDecoder.encodeInto(buffer);
        return DevStudio::AttributeValuePairStruct(_attributeValuePairStructDecoder);
    }
    std::vector< DevStudio::AttributeValuePairStruct > Decoders::decodeAttributeValuePairStructArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _attributeValuePairStructArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_attributeValuePairStructArray1PlusDecoder.getEncodedLength());
        _attributeValuePairStructArray1PlusDecoder.encodeInto(buffer);
        return std::vector< DevStudio::AttributeValuePairStruct >(_attributeValuePairStructArray1PlusDecoder);
    }
    DevStudio::AudioDataTypeStruct Decoders::decodeAudioDataTypeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _audioDataTypeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_audioDataTypeStructDecoder.getEncodedLength());
        _audioDataTypeStructDecoder.encodeInto(buffer);
        return DevStudio::AudioDataTypeStruct(_audioDataTypeStructDecoder);
    }
    DevStudio::BeamAntennaStruct Decoders::decodeBeamAntennaStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _beamAntennaStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_beamAntennaStructDecoder.getEncodedLength());
        _beamAntennaStructDecoder.encodeInto(buffer);
        return DevStudio::BeamAntennaStruct(_beamAntennaStructDecoder);
    }
    DevStudio::BeamFunctionCodeEnum::BeamFunctionCodeEnum Decoders::decodeBeamFunctionCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _beamFunctionCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_beamFunctionCodeEnumDecoder.getEncodedLength());
        _beamFunctionCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::BeamFunctionCodeEnum::BeamFunctionCodeEnum(_beamFunctionCodeEnumDecoder);
    }
    std::vector< bool > Decoders::decodeBinArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _binArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_binArrayDecoder.getEncodedLength());
        _binArrayDecoder.encodeInto(buffer);
        return std::vector< bool >(_binArrayDecoder);
    }
    DevStudio::BreachStruct Decoders::decodeBreachStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _breachStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_breachStructDecoder.getEncodedLength());
        _breachStructDecoder.encodeInto(buffer);
        return DevStudio::BreachStruct(_breachStructDecoder);
    }
    std::vector< DevStudio::BreachStruct > Decoders::decodeBreachStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _breachStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_breachStructLengthlessArrayDecoder.getEncodedLength());
        _breachStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::BreachStruct >(_breachStructLengthlessArrayDecoder);
    }
    DevStudio::BreachableSegmentStruct Decoders::decodeBreachableSegmentStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _breachableSegmentStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_breachableSegmentStructDecoder.getEncodedLength());
        _breachableSegmentStructDecoder.encodeInto(buffer);
        return DevStudio::BreachableSegmentStruct(_breachableSegmentStructDecoder);
    }
    std::vector< DevStudio::BreachableSegmentStruct > Decoders::decodeBreachableSegmentStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _breachableSegmentStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_breachableSegmentStructLengthlessArrayDecoder.getEncodedLength());
        _breachableSegmentStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::BreachableSegmentStruct >(_breachableSegmentStructLengthlessArrayDecoder);
    }
    std::vector< DevStudio::BreachedStatusEnum::BreachedStatusEnum > Decoders::decodeBreachedStatusArray8(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _breachedStatusArray8Decoder.decodeFrom(data, 0);
        buffer.reserve(_breachedStatusArray8Decoder.getEncodedLength());
        _breachedStatusArray8Decoder.encodeInto(buffer);
        return std::vector< DevStudio::BreachedStatusEnum::BreachedStatusEnum >(_breachedStatusArray8Decoder);
    }
    DevStudio::BreachedStatusEnum::BreachedStatusEnum Decoders::decodeBreachedStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _breachedStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_breachedStatusEnumDecoder.getEncodedLength());
        _breachedStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::BreachedStatusEnum::BreachedStatusEnum(_breachedStatusEnumDecoder);
    }
    DevStudio::CBRNAlarmStruct Decoders::decodeCBRNAlarmStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cBRNAlarmStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cBRNAlarmStructDecoder.getEncodedLength());
        _cBRNAlarmStructDecoder.encodeInto(buffer);
        return DevStudio::CBRNAlarmStruct(_cBRNAlarmStructDecoder);
    }
    DevStudio::CBRNDamageEnum::CBRNDamageEnum Decoders::decodeCBRNDamageEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cBRNDamageEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_cBRNDamageEnumDecoder.getEncodedLength());
        _cBRNDamageEnumDecoder.encodeInto(buffer);
        return DevStudio::CBRNDamageEnum::CBRNDamageEnum(_cBRNDamageEnumDecoder);
    }
    DevStudio::CBRNExposureStruct Decoders::decodeCBRNExposureStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cBRNExposureStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cBRNExposureStructDecoder.getEncodedLength());
        _cBRNExposureStructDecoder.encodeInto(buffer);
        return DevStudio::CBRNExposureStruct(_cBRNExposureStructDecoder);
    }
    DevStudio::CBRNSensorReadingStruct Decoders::decodeCBRNSensorReadingStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cBRNSensorReadingStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cBRNSensorReadingStructDecoder.getEncodedLength());
        _cBRNSensorReadingStructDecoder.encodeInto(buffer);
        return DevStudio::CBRNSensorReadingStruct(_cBRNSensorReadingStructDecoder);
    }
    DevStudio::COMBICStateRecStruct Decoders::decodeCOMBICStateRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cOMBICStateRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cOMBICStateRecStructDecoder.getEncodedLength());
        _cOMBICStateRecStructDecoder.encodeInto(buffer);
        return DevStudio::COMBICStateRecStruct(_cOMBICStateRecStructDecoder);
    }
    DevStudio::CamouflageEnum::CamouflageEnum Decoders::decodeCamouflageEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _camouflageEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_camouflageEnumDecoder.getEncodedLength());
        _camouflageEnumDecoder.encodeInto(buffer);
        return DevStudio::CamouflageEnum::CamouflageEnum(_camouflageEnumDecoder);
    }
    DevStudio::CaptureStatusEnum::CaptureStatusEnum Decoders::decodeCaptureStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _captureStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_captureStatusEnumDecoder.getEncodedLength());
        _captureStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::CaptureStatusEnum::CaptureStatusEnum(_captureStatusEnumDecoder);
    }
    DevStudio::ChangeAltitudeTaskStruct Decoders::decodeChangeAltitudeTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _changeAltitudeTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_changeAltitudeTaskStructDecoder.getEncodedLength());
        _changeAltitudeTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ChangeAltitudeTaskStruct(_changeAltitudeTaskStructDecoder);
    }
    DevStudio::ChangeHeadingTaskStruct Decoders::decodeChangeHeadingTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _changeHeadingTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_changeHeadingTaskStructDecoder.getEncodedLength());
        _changeHeadingTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ChangeHeadingTaskStruct(_changeHeadingTaskStructDecoder);
    }
    DevStudio::ChangeRulesOfEngagementTaskStruct Decoders::decodeChangeRulesOfEngagementTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _changeRulesOfEngagementTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_changeRulesOfEngagementTaskStructDecoder.getEncodedLength());
        _changeRulesOfEngagementTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ChangeRulesOfEngagementTaskStruct(_changeRulesOfEngagementTaskStructDecoder);
    }
    DevStudio::ChangeSpeedTaskStruct Decoders::decodeChangeSpeedTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _changeSpeedTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_changeSpeedTaskStructDecoder.getEncodedLength());
        _changeSpeedTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ChangeSpeedTaskStruct(_changeSpeedTaskStructDecoder);
    }
    DevStudio::ChemicalContentEnum::ChemicalContentEnum Decoders::decodeChemicalContentEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _chemicalContentEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_chemicalContentEnumDecoder.getEncodedLength());
        _chemicalContentEnumDecoder.encodeInto(buffer);
        return DevStudio::ChemicalContentEnum::ChemicalContentEnum(_chemicalContentEnumDecoder);
    }
    DevStudio::ClearEngineeringTaskStruct Decoders::decodeClearEngineeringTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _clearEngineeringTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_clearEngineeringTaskStructDecoder.getEncodedLength());
        _clearEngineeringTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ClearEngineeringTaskStruct(_clearEngineeringTaskStructDecoder);
    }
    DevStudio::ClockTimeStruct Decoders::decodeClockTimeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _clockTimeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_clockTimeStructDecoder.getEncodedLength());
        _clockTimeStructDecoder.encodeInto(buffer);
        return DevStudio::ClockTimeStruct(_clockTimeStructDecoder);
    }
    std::vector< DevStudio::ClockTimeStruct > Decoders::decodeClockTimeStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _clockTimeStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_clockTimeStructLengthlessArrayDecoder.getEncodedLength());
        _clockTimeStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ClockTimeStruct >(_clockTimeStructLengthlessArrayDecoder);
    }
    DevStudio::CloudStruct Decoders::decodeCloudStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cloudStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cloudStructDecoder.getEncodedLength());
        _cloudStructDecoder.encodeInto(buffer);
        return DevStudio::CloudStruct(_cloudStructDecoder);
    }
    DevStudio::CloudTypeEnum::CloudTypeEnum Decoders::decodeCloudTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cloudTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_cloudTypeEnumDecoder.getEncodedLength());
        _cloudTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::CloudTypeEnum::CloudTypeEnum(_cloudTypeEnumDecoder);
    }
    std::vector< float > Decoders::decodeCoefficientsLengthlessArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _coefficientsLengthlessArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_coefficientsLengthlessArray1PlusDecoder.getEncodedLength());
        _coefficientsLengthlessArray1PlusDecoder.encodeInto(buffer);
        return std::vector< float >(_coefficientsLengthlessArray1PlusDecoder);
    }
    DevStudio::CollisionTypeEnum::CollisionTypeEnum Decoders::decodeCollisionTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _collisionTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_collisionTypeEnumDecoder.getEncodedLength());
        _collisionTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::CollisionTypeEnum::CollisionTypeEnum(_collisionTypeEnumDecoder);
    }
    DevStudio::CombinationModulationTypeEnum::CombinationModulationTypeEnum Decoders::decodeCombinationModulationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _combinationModulationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_combinationModulationTypeEnumDecoder.getEncodedLength());
        _combinationModulationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::CombinationModulationTypeEnum::CombinationModulationTypeEnum(_combinationModulationTypeEnumDecoder);
    }
    DevStudio::CommunicationNetworkTypeEnum::CommunicationNetworkTypeEnum Decoders::decodeCommunicationNetworkTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _communicationNetworkTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_communicationNetworkTypeEnumDecoder.getEncodedLength());
        _communicationNetworkTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::CommunicationNetworkTypeEnum::CommunicationNetworkTypeEnum(_communicationNetworkTypeEnumDecoder);
    }
    DevStudio::CommunicationServiceTypeEnum::CommunicationServiceTypeEnum Decoders::decodeCommunicationServiceTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _communicationServiceTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_communicationServiceTypeEnumDecoder.getEncodedLength());
        _communicationServiceTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::CommunicationServiceTypeEnum::CommunicationServiceTypeEnum(_communicationServiceTypeEnumDecoder);
    }
    DevStudio::ComplianceStateEnum::ComplianceStateEnum Decoders::decodeComplianceStateEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _complianceStateEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_complianceStateEnumDecoder.getEncodedLength());
        _complianceStateEnumDecoder.encodeInto(buffer);
        return DevStudio::ComplianceStateEnum::ComplianceStateEnum(_complianceStateEnumDecoder);
    }
    DevStudio::Cone1GeomRecStruct Decoders::decodeCone1GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cone1GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cone1GeomRecStructDecoder.getEncodedLength());
        _cone1GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Cone1GeomRecStruct(_cone1GeomRecStructDecoder);
    }
    DevStudio::Cone2GeomRecStruct Decoders::decodeCone2GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cone2GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_cone2GeomRecStructDecoder.getEncodedLength());
        _cone2GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Cone2GeomRecStruct(_cone2GeomRecStructDecoder);
    }
    std::vector< DevStudio::ConnectionReceiverStruct > Decoders::decodeConnectionReceiverArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _connectionReceiverArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_connectionReceiverArrayDecoder.getEncodedLength());
        _connectionReceiverArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ConnectionReceiverStruct >(_connectionReceiverArrayDecoder);
    }
    DevStudio::ConnectionReceiverStruct Decoders::decodeConnectionReceiverStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _connectionReceiverStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_connectionReceiverStructDecoder.getEncodedLength());
        _connectionReceiverStructDecoder.encodeInto(buffer);
        return DevStudio::ConnectionReceiverStruct(_connectionReceiverStructDecoder);
    }
    DevStudio::ConnectionTypeEnum::ConnectionTypeEnum Decoders::decodeConnectionTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _connectionTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_connectionTypeEnumDecoder.getEncodedLength());
        _connectionTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ConnectionTypeEnum::ConnectionTypeEnum(_connectionTypeEnumDecoder);
    }
    DevStudio::ConstituentPartNatureEnum::ConstituentPartNatureEnum Decoders::decodeConstituentPartNatureEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _constituentPartNatureEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_constituentPartNatureEnumDecoder.getEncodedLength());
        _constituentPartNatureEnumDecoder.encodeInto(buffer);
        return DevStudio::ConstituentPartNatureEnum::ConstituentPartNatureEnum(_constituentPartNatureEnumDecoder);
    }
    DevStudio::ConstituentPartPositionEnum::ConstituentPartPositionEnum Decoders::decodeConstituentPartPositionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _constituentPartPositionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_constituentPartPositionEnumDecoder.getEncodedLength());
        _constituentPartPositionEnumDecoder.encodeInto(buffer);
        return DevStudio::ConstituentPartPositionEnum::ConstituentPartPositionEnum(_constituentPartPositionEnumDecoder);
    }
    DevStudio::ConstituentPartRelationshipStruct Decoders::decodeConstituentPartRelationshipStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _constituentPartRelationshipStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_constituentPartRelationshipStructDecoder.getEncodedLength());
        _constituentPartRelationshipStructDecoder.encodeInto(buffer);
        return DevStudio::ConstituentPartRelationshipStruct(_constituentPartRelationshipStructDecoder);
    }
    DevStudio::ConstituentPartStationNameEnum::ConstituentPartStationNameEnum Decoders::decodeConstituentPartStationNameEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _constituentPartStationNameEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_constituentPartStationNameEnumDecoder.getEncodedLength());
        _constituentPartStationNameEnumDecoder.encodeInto(buffer);
        return DevStudio::ConstituentPartStationNameEnum::ConstituentPartStationNameEnum(_constituentPartStationNameEnumDecoder);
    }
    DevStudio::ContourStruct Decoders::decodeContourStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _contourStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_contourStructDecoder.getEncodedLength());
        _contourStructDecoder.encodeInto(buffer);
        return DevStudio::ContourStruct(_contourStructDecoder);
    }
    DevStudio::CreateBreachTaskStruct Decoders::decodeCreateBreachTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _createBreachTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_createBreachTaskStructDecoder.getEncodedLength());
        _createBreachTaskStructDecoder.encodeInto(buffer);
        return DevStudio::CreateBreachTaskStruct(_createBreachTaskStructDecoder);
    }
    DevStudio::CreateFireBreakTaskStruct Decoders::decodeCreateFireBreakTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _createFireBreakTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_createFireBreakTaskStructDecoder.getEncodedLength());
        _createFireBreakTaskStructDecoder.encodeInto(buffer);
        return DevStudio::CreateFireBreakTaskStruct(_createFireBreakTaskStructDecoder);
    }
    DevStudio::CreateLeveeTaskStruct Decoders::decodeCreateLeveeTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _createLeveeTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_createLeveeTaskStructDecoder.getEncodedLength());
        _createLeveeTaskStructDecoder.encodeInto(buffer);
        return DevStudio::CreateLeveeTaskStruct(_createLeveeTaskStructDecoder);
    }
    DevStudio::CreateObstacleTaskStruct Decoders::decodeCreateObstacleTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _createObstacleTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_createObstacleTaskStructDecoder.getEncodedLength());
        _createObstacleTaskStructDecoder.encodeInto(buffer);
        return DevStudio::CreateObstacleTaskStruct(_createObstacleTaskStructDecoder);
    }
    DevStudio::CryptographicModeEnum::CryptographicModeEnum Decoders::decodeCryptographicModeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cryptographicModeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_cryptographicModeEnumDecoder.getEncodedLength());
        _cryptographicModeEnumDecoder.encodeInto(buffer);
        return DevStudio::CryptographicModeEnum::CryptographicModeEnum(_cryptographicModeEnumDecoder);
    }
    DevStudio::CryptographicSystemTypeEnum::CryptographicSystemTypeEnum Decoders::decodeCryptographicSystemTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _cryptographicSystemTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_cryptographicSystemTypeEnumDecoder.getEncodedLength());
        _cryptographicSystemTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::CryptographicSystemTypeEnum::CryptographicSystemTypeEnum(_cryptographicSystemTypeEnumDecoder);
    }
    DevStudio::CurrentStruct Decoders::decodeCurrentStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _currentStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_currentStructDecoder.getEncodedLength());
        _currentStructDecoder.encodeInto(buffer);
        return DevStudio::CurrentStruct(_currentStructDecoder);
    }
    DevStudio::DamageStatusEnhancedEnum::DamageStatusEnhancedEnum Decoders::decodeDamageStatusEnhancedEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _damageStatusEnhancedEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_damageStatusEnhancedEnumDecoder.getEncodedLength());
        _damageStatusEnhancedEnumDecoder.encodeInto(buffer);
        return DevStudio::DamageStatusEnhancedEnum::DamageStatusEnhancedEnum(_damageStatusEnhancedEnumDecoder);
    }
    DevStudio::DamageStatusEnum::DamageStatusEnum Decoders::decodeDamageStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _damageStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_damageStatusEnumDecoder.getEncodedLength());
        _damageStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::DamageStatusEnum::DamageStatusEnum(_damageStatusEnumDecoder);
    }
    DevStudio::DatumIdentifierEnum::DatumIdentifierEnum Decoders::decodeDatumIdentifierEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _datumIdentifierEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_datumIdentifierEnumDecoder.getEncodedLength());
        _datumIdentifierEnumDecoder.encodeInto(buffer);
        return DevStudio::DatumIdentifierEnum::DatumIdentifierEnum(_datumIdentifierEnumDecoder);
    }
    std::vector< DevStudio::DatumIdentifierEnum::DatumIdentifierEnum > Decoders::decodeDatumIdentifierLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _datumIdentifierLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_datumIdentifierLengthlessArrayDecoder.getEncodedLength());
        _datumIdentifierLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::DatumIdentifierEnum::DatumIdentifierEnum >(_datumIdentifierLengthlessArrayDecoder);
    }
    DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum Decoders::decodeDeadReckoningAlgorithmEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _deadReckoningAlgorithmEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_deadReckoningAlgorithmEnumDecoder.getEncodedLength());
        _deadReckoningAlgorithmEnumDecoder.encodeInto(buffer);
        return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum(_deadReckoningAlgorithmEnumDecoder);
    }
    std::vector< float > Decoders::decodeDepthMeterFloat32LengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _depthMeterFloat32LengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_depthMeterFloat32LengthlessArrayDecoder.getEncodedLength());
        _depthMeterFloat32LengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< float >(_depthMeterFloat32LengthlessArrayDecoder);
    }
    DevStudio::DesignatorCodeEnum::DesignatorCodeEnum Decoders::decodeDesignatorCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _designatorCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_designatorCodeEnumDecoder.getEncodedLength());
        _designatorCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::DesignatorCodeEnum::DesignatorCodeEnum(_designatorCodeEnumDecoder);
    }
    DevStudio::DesignatorCodeNameEnum::DesignatorCodeNameEnum Decoders::decodeDesignatorCodeNameEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _designatorCodeNameEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_designatorCodeNameEnumDecoder.getEncodedLength());
        _designatorCodeNameEnumDecoder.encodeInto(buffer);
        return DevStudio::DesignatorCodeNameEnum::DesignatorCodeNameEnum(_designatorCodeNameEnumDecoder);
    }
    DevStudio::DetectedEquipment Decoders::decodeDetectedEquipment(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _detectedEquipmentDecoder.decodeFrom(data, 0);
        buffer.reserve(_detectedEquipmentDecoder.getEncodedLength());
        _detectedEquipmentDecoder.encodeInto(buffer);
        return DevStudio::DetectedEquipment(_detectedEquipmentDecoder);
    }
    DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum Decoders::decodeDetonationResultCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _detonationResultCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_detonationResultCodeEnumDecoder.getEncodedLength());
        _detonationResultCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum(_detonationResultCodeEnumDecoder);
    }
    DevStudio::DimensionRateStruct Decoders::decodeDimensionRateStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _dimensionRateStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_dimensionRateStructDecoder.getEncodedLength());
        _dimensionRateStructDecoder.encodeInto(buffer);
        return DevStudio::DimensionRateStruct(_dimensionRateStructDecoder);
    }
    DevStudio::DimensionStruct Decoders::decodeDimensionStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _dimensionStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_dimensionStructDecoder.getEncodedLength());
        _dimensionStructDecoder.encodeInto(buffer);
        return DevStudio::DimensionStruct(_dimensionStructDecoder);
    }
    DevStudio::DirectFireTaskStruct Decoders::decodeDirectFireTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _directFireTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_directFireTaskStructDecoder.getEncodedLength());
        _directFireTaskStructDecoder.encodeInto(buffer);
        return DevStudio::DirectFireTaskStruct(_directFireTaskStructDecoder);
    }
    DevStudio::DisruptCommunicationTaskStruct Decoders::decodeDisruptCommunicationTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _disruptCommunicationTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_disruptCommunicationTaskStructDecoder.getEncodedLength());
        _disruptCommunicationTaskStructDecoder.encodeInto(buffer);
        return DevStudio::DisruptCommunicationTaskStruct(_disruptCommunicationTaskStructDecoder);
    }
    DevStudio::EchelonEnum::EchelonEnum Decoders::decodeEchelonEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _echelonEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_echelonEnumDecoder.getEncodedLength());
        _echelonEnumDecoder.encodeInto(buffer);
        return DevStudio::EchelonEnum::EchelonEnum(_echelonEnumDecoder);
    }
    DevStudio::ElapsedTimeProgress Decoders::decodeElapsedTimeProgress(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _elapsedTimeProgressDecoder.decodeFrom(data, 0);
        buffer.reserve(_elapsedTimeProgressDecoder.getEncodedLength());
        _elapsedTimeProgressDecoder.encodeInto(buffer);
        return DevStudio::ElapsedTimeProgress(_elapsedTimeProgressDecoder);
    }
    DevStudio::Ellipsoid1GeomRecStruct Decoders::decodeEllipsoid1GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _ellipsoid1GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_ellipsoid1GeomRecStructDecoder.getEncodedLength());
        _ellipsoid1GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Ellipsoid1GeomRecStruct(_ellipsoid1GeomRecStructDecoder);
    }
    DevStudio::Ellipsoid2GeomRecStruct Decoders::decodeEllipsoid2GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _ellipsoid2GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_ellipsoid2GeomRecStructDecoder.getEncodedLength());
        _ellipsoid2GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Ellipsoid2GeomRecStruct(_ellipsoid2GeomRecStructDecoder);
    }
    DevStudio::EmitterFunctionEnum::EmitterFunctionEnum Decoders::decodeEmitterFunctionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _emitterFunctionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_emitterFunctionEnumDecoder.getEncodedLength());
        _emitterFunctionEnumDecoder.encodeInto(buffer);
        return DevStudio::EmitterFunctionEnum::EmitterFunctionEnum(_emitterFunctionEnumDecoder);
    }
    DevStudio::EmitterTypeEnum::EmitterTypeEnum Decoders::decodeEmitterTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _emitterTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_emitterTypeEnumDecoder.getEncodedLength());
        _emitterTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EmitterTypeEnum::EmitterTypeEnum(_emitterTypeEnumDecoder);
    }
    DevStudio::EmptyTaskStruct Decoders::decodeEmptyTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _emptyTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_emptyTaskStructDecoder.getEncodedLength());
        _emptyTaskStructDecoder.encodeInto(buffer);
        return DevStudio::EmptyTaskStruct(_emptyTaskStructDecoder);
    }
    DevStudio::EncodingTypeEnum::EncodingTypeEnum Decoders::decodeEncodingTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _encodingTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_encodingTypeEnumDecoder.getEncodedLength());
        _encodingTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EncodingTypeEnum::EncodingTypeEnum(_encodingTypeEnumDecoder);
    }
    DevStudio::EngineeringTaskStruct Decoders::decodeEngineeringTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _engineeringTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_engineeringTaskStructDecoder.getEncodedLength());
        _engineeringTaskStructDecoder.encodeInto(buffer);
        return DevStudio::EngineeringTaskStruct(_engineeringTaskStructDecoder);
    }
    DevStudio::EntityControlActionEnum::EntityControlActionEnum Decoders::decodeEntityControlActionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _entityControlActionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_entityControlActionEnumDecoder.getEncodedLength());
        _entityControlActionEnumDecoder.encodeInto(buffer);
        return DevStudio::EntityControlActionEnum::EntityControlActionEnum(_entityControlActionEnumDecoder);
    }
    std::vector< DevStudio::EntityControlActionEnum::EntityControlActionEnum > Decoders::decodeEntityControlActions(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _entityControlActionsDecoder.decodeFrom(data, 0);
        buffer.reserve(_entityControlActionsDecoder.getEncodedLength());
        _entityControlActionsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::EntityControlActionEnum::EntityControlActionEnum >(_entityControlActionsDecoder);
    }
    DevStudio::EntityCoordinateVectorStruct Decoders::decodeEntityCoordinateVectorStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _entityCoordinateVectorStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_entityCoordinateVectorStructDecoder.getEncodedLength());
        _entityCoordinateVectorStructDecoder.encodeInto(buffer);
        return DevStudio::EntityCoordinateVectorStruct(_entityCoordinateVectorStructDecoder);
    }
    DevStudio::EntityIdentifierStruct Decoders::decodeEntityIdentifierStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _entityIdentifierStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_entityIdentifierStructDecoder.getEncodedLength());
        _entityIdentifierStructDecoder.encodeInto(buffer);
        return DevStudio::EntityIdentifierStruct(_entityIdentifierStructDecoder);
    }
    DevStudio::EntityTypeStruct Decoders::decodeEntityTypeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _entityTypeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_entityTypeStructDecoder.getEncodedLength());
        _entityTypeStructDecoder.encodeInto(buffer);
        return DevStudio::EntityTypeStruct(_entityTypeStructDecoder);
    }
    std::vector< DevStudio::EntityTypeStruct > Decoders::decodeEntityTypeStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _entityTypeStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_entityTypeStructLengthlessArrayDecoder.getEncodedLength());
        _entityTypeStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::EntityTypeStruct >(_entityTypeStructLengthlessArrayDecoder);
    }
    DevStudio::EnvironmentConditionModelTypeEnum::EnvironmentConditionModelTypeEnum Decoders::decodeEnvironmentConditionModelTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentConditionModelTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentConditionModelTypeEnumDecoder.getEncodedLength());
        _environmentConditionModelTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentConditionModelTypeEnum::EnvironmentConditionModelTypeEnum(_environmentConditionModelTypeEnumDecoder);
    }
    DevStudio::EnvironmentDataCoordinateSystemEnum::EnvironmentDataCoordinateSystemEnum Decoders::decodeEnvironmentDataCoordinateSystemEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentDataCoordinateSystemEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentDataCoordinateSystemEnumDecoder.getEncodedLength());
        _environmentDataCoordinateSystemEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentDataCoordinateSystemEnum::EnvironmentDataCoordinateSystemEnum(_environmentDataCoordinateSystemEnumDecoder);
    }
    DevStudio::EnvironmentDataRepresentationEnum::EnvironmentDataRepresentationEnum Decoders::decodeEnvironmentDataRepresentationEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentDataRepresentationEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentDataRepresentationEnumDecoder.getEncodedLength());
        _environmentDataRepresentationEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentDataRepresentationEnum::EnvironmentDataRepresentationEnum(_environmentDataRepresentationEnumDecoder);
    }
    DevStudio::EnvironmentDataSampleTypeEnum::EnvironmentDataSampleTypeEnum Decoders::decodeEnvironmentDataSampleTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentDataSampleTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentDataSampleTypeEnumDecoder.getEncodedLength());
        _environmentDataSampleTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentDataSampleTypeEnum::EnvironmentDataSampleTypeEnum(_environmentDataSampleTypeEnumDecoder);
    }
    DevStudio::EnvironmentGridAxisTypeEnum::EnvironmentGridAxisTypeEnum Decoders::decodeEnvironmentGridAxisTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentGridAxisTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentGridAxisTypeEnumDecoder.getEncodedLength());
        _environmentGridAxisTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentGridAxisTypeEnum::EnvironmentGridAxisTypeEnum(_environmentGridAxisTypeEnumDecoder);
    }
    DevStudio::EnvironmentGridTypeEnum::EnvironmentGridTypeEnum Decoders::decodeEnvironmentGridTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentGridTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentGridTypeEnumDecoder.getEncodedLength());
        _environmentGridTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentGridTypeEnum::EnvironmentGridTypeEnum(_environmentGridTypeEnumDecoder);
    }
    DevStudio::EnvironmentModelTypeEnum::EnvironmentModelTypeEnum Decoders::decodeEnvironmentModelTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentModelTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentModelTypeEnumDecoder.getEncodedLength());
        _environmentModelTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentModelTypeEnum::EnvironmentModelTypeEnum(_environmentModelTypeEnumDecoder);
    }
    DevStudio::EnvironmentObjectTypeStruct Decoders::decodeEnvironmentObjectTypeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentObjectTypeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentObjectTypeStructDecoder.getEncodedLength());
        _environmentObjectTypeStructDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentObjectTypeStruct(_environmentObjectTypeStructDecoder);
    }
    DevStudio::EnvironmentRecStruct Decoders::decodeEnvironmentRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentRecStructDecoder.getEncodedLength());
        _environmentRecStructDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentRecStruct(_environmentRecStructDecoder);
    }
    std::vector< DevStudio::EnvironmentRecStruct > Decoders::decodeEnvironmentRecStructArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentRecStructArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentRecStructArrayDecoder.getEncodedLength());
        _environmentRecStructArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::EnvironmentRecStruct >(_environmentRecStructArrayDecoder);
    }
    DevStudio::EnvironmentRecVariantStruct Decoders::decodeEnvironmentRecVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentRecVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentRecVariantStructDecoder.getEncodedLength());
        _environmentRecVariantStructDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentRecVariantStruct(_environmentRecVariantStructDecoder);
    }
    DevStudio::EnvironmentRecordTypeEnum::EnvironmentRecordTypeEnum Decoders::decodeEnvironmentRecordTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentRecordTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentRecordTypeEnumDecoder.getEncodedLength());
        _environmentRecordTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentRecordTypeEnum::EnvironmentRecordTypeEnum(_environmentRecordTypeEnumDecoder);
    }
    DevStudio::EnvironmentTypeStruct Decoders::decodeEnvironmentTypeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _environmentTypeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_environmentTypeStructDecoder.getEncodedLength());
        _environmentTypeStructDecoder.encodeInto(buffer);
        return DevStudio::EnvironmentTypeStruct(_environmentTypeStructDecoder);
    }
    DevStudio::EquipmentSymbolAmplificationStruct Decoders::decodeEquipmentSymbolAmplificationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _equipmentSymbolAmplificationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_equipmentSymbolAmplificationStructDecoder.getEncodedLength());
        _equipmentSymbolAmplificationStructDecoder.encodeInto(buffer);
        return DevStudio::EquipmentSymbolAmplificationStruct(_equipmentSymbolAmplificationStructDecoder);
    }
    DevStudio::EstablishCheckpointTaskStruct Decoders::decodeEstablishCheckpointTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _establishCheckpointTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_establishCheckpointTaskStructDecoder.getEncodedLength());
        _establishCheckpointTaskStructDecoder.encodeInto(buffer);
        return DevStudio::EstablishCheckpointTaskStruct(_establishCheckpointTaskStructDecoder);
    }
    DevStudio::EstablishObservationPostTaskStruct Decoders::decodeEstablishObservationPostTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _establishObservationPostTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_establishObservationPostTaskStructDecoder.getEncodedLength());
        _establishObservationPostTaskStructDecoder.encodeInto(buffer);
        return DevStudio::EstablishObservationPostTaskStruct(_establishObservationPostTaskStructDecoder);
    }
    DevStudio::EvacuateProgressStruct Decoders::decodeEvacuateProgressStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _evacuateProgressStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_evacuateProgressStructDecoder.getEncodedLength());
        _evacuateProgressStructDecoder.encodeInto(buffer);
        return DevStudio::EvacuateProgressStruct(_evacuateProgressStructDecoder);
    }
    DevStudio::EvacuateTaskStruct Decoders::decodeEvacuateTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _evacuateTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_evacuateTaskStructDecoder.getEncodedLength());
        _evacuateTaskStructDecoder.encodeInto(buffer);
        return DevStudio::EvacuateTaskStruct(_evacuateTaskStructDecoder);
    }
    DevStudio::EventIdentifierStruct Decoders::decodeEventIdentifierStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _eventIdentifierStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_eventIdentifierStructDecoder.getEncodedLength());
        _eventIdentifierStructDecoder.encodeInto(buffer);
        return DevStudio::EventIdentifierStruct(_eventIdentifierStructDecoder);
    }
    DevStudio::EventTypeEnum::EventTypeEnum Decoders::decodeEventTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _eventTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_eventTypeEnumDecoder.getEncodedLength());
        _eventTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::EventTypeEnum::EventTypeEnum(_eventTypeEnumDecoder);
    }
    DevStudio::ExhaustSmokeStruct Decoders::decodeExhaustSmokeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _exhaustSmokeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_exhaustSmokeStructDecoder.getEncodedLength());
        _exhaustSmokeStructDecoder.encodeInto(buffer);
        return DevStudio::ExhaustSmokeStruct(_exhaustSmokeStructDecoder);
    }
    std::vector< DevStudio::ExhaustSmokeStruct > Decoders::decodeExhaustSmokeStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _exhaustSmokeStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_exhaustSmokeStructLengthlessArrayDecoder.getEncodedLength());
        _exhaustSmokeStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ExhaustSmokeStruct >(_exhaustSmokeStructLengthlessArrayDecoder);
    }
    DevStudio::ExposureTypeEnum::ExposureTypeEnum Decoders::decodeExposureTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _exposureTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_exposureTypeEnumDecoder.getEncodedLength());
        _exposureTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ExposureTypeEnum::ExposureTypeEnum(_exposureTypeEnumDecoder);
    }
    DevStudio::ExtinguishWildfireTaskStruct Decoders::decodeExtinguishWildfireTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _extinguishWildfireTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_extinguishWildfireTaskStructDecoder.getEncodedLength());
        _extinguishWildfireTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ExtinguishWildfireTaskStruct(_extinguishWildfireTaskStructDecoder);
    }
    DevStudio::FederateControlActionEnum::FederateControlActionEnum Decoders::decodeFederateControlActionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _federateControlActionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_federateControlActionEnumDecoder.getEncodedLength());
        _federateControlActionEnumDecoder.encodeInto(buffer);
        return DevStudio::FederateControlActionEnum::FederateControlActionEnum(_federateControlActionEnumDecoder);
    }
    std::vector< DevStudio::FederateControlActionEnum::FederateControlActionEnum > Decoders::decodeFederateControlActions(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _federateControlActionsDecoder.decodeFrom(data, 0);
        buffer.reserve(_federateControlActionsDecoder.getEncodedLength());
        _federateControlActionsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::FederateControlActionEnum::FederateControlActionEnum >(_federateControlActionsDecoder);
    }
    DevStudio::FederateIdentifierStruct Decoders::decodeFederateIdentifierStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _federateIdentifierStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_federateIdentifierStructDecoder.getEncodedLength());
        _federateIdentifierStructDecoder.encodeInto(buffer);
        return DevStudio::FederateIdentifierStruct(_federateIdentifierStructDecoder);
    }
    DevStudio::FireTaskProgress Decoders::decodeFireTaskProgress(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _fireTaskProgressDecoder.decodeFrom(data, 0);
        buffer.reserve(_fireTaskProgressDecoder.getEncodedLength());
        _fireTaskProgressDecoder.encodeInto(buffer);
        return DevStudio::FireTaskProgress(_fireTaskProgressDecoder);
    }
    DevStudio::FixedDatumStruct Decoders::decodeFixedDatumStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _fixedDatumStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_fixedDatumStructDecoder.getEncodedLength());
        _fixedDatumStructDecoder.encodeInto(buffer);
        return DevStudio::FixedDatumStruct(_fixedDatumStructDecoder);
    }
    std::vector< DevStudio::FixedDatumStruct > Decoders::decodeFixedDatumStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _fixedDatumStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_fixedDatumStructLengthlessArrayDecoder.getEncodedLength());
        _fixedDatumStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::FixedDatumStruct >(_fixedDatumStructLengthlessArrayDecoder);
    }
    DevStudio::FlareStateRecStruct Decoders::decodeFlareStateRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _flareStateRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_flareStateRecStructDecoder.getEncodedLength());
        _flareStateRecStructDecoder.encodeInto(buffer);
        return DevStudio::FlareStateRecStruct(_flareStateRecStructDecoder);
    }
    std::vector< float > Decoders::decodeFloat32Array1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _float32Array1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_float32Array1PlusDecoder.getEncodedLength());
        _float32Array1PlusDecoder.encodeInto(buffer);
        return std::vector< float >(_float32Array1PlusDecoder);
    }
    DevStudio::FollowEntityTaskStruct Decoders::decodeFollowEntityTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _followEntityTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_followEntityTaskStructDecoder.getEncodedLength());
        _followEntityTaskStructDecoder.encodeInto(buffer);
        return DevStudio::FollowEntityTaskStruct(_followEntityTaskStructDecoder);
    }
    DevStudio::ForceIdentifierEnum::ForceIdentifierEnum Decoders::decodeForceIdentifierEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _forceIdentifierEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_forceIdentifierEnumDecoder.getEncodedLength());
        _forceIdentifierEnumDecoder.encodeInto(buffer);
        return DevStudio::ForceIdentifierEnum::ForceIdentifierEnum(_forceIdentifierEnumDecoder);
    }
    DevStudio::FormationDataStruct Decoders::decodeFormationDataStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _formationDataStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_formationDataStructDecoder.getEncodedLength());
        _formationDataStructDecoder.encodeInto(buffer);
        return DevStudio::FormationDataStruct(_formationDataStructDecoder);
    }
    DevStudio::FormationEnum::FormationEnum Decoders::decodeFormationEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _formationEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_formationEnumDecoder.getEncodedLength());
        _formationEnumDecoder.encodeInto(buffer);
        return DevStudio::FormationEnum::FormationEnum(_formationEnumDecoder);
    }
    DevStudio::FormationLocationTypeEnum::FormationLocationTypeEnum Decoders::decodeFormationLocationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _formationLocationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_formationLocationTypeEnumDecoder.getEncodedLength());
        _formationLocationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::FormationLocationTypeEnum::FormationLocationTypeEnum(_formationLocationTypeEnumDecoder);
    }
    DevStudio::FormationStruct Decoders::decodeFormationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _formationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_formationStructDecoder.getEncodedLength());
        _formationStructDecoder.encodeInto(buffer);
        return DevStudio::FormationStruct(_formationStructDecoder);
    }
    DevStudio::FormationTypeEnum::FormationTypeEnum Decoders::decodeFormationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _formationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_formationTypeEnumDecoder.getEncodedLength());
        _formationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::FormationTypeEnum::FormationTypeEnum(_formationTypeEnumDecoder);
    }
    DevStudio::FundamentalParameterDataStruct Decoders::decodeFundamentalParameterDataStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _fundamentalParameterDataStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_fundamentalParameterDataStructDecoder.getEncodedLength());
        _fundamentalParameterDataStructDecoder.encodeInto(buffer);
        return DevStudio::FundamentalParameterDataStruct(_fundamentalParameterDataStructDecoder);
    }
    std::vector< DevStudio::FundamentalParameterDataStruct > Decoders::decodeFundamentalParameterDataStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _fundamentalParameterDataStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_fundamentalParameterDataStructLengthlessArrayDecoder.getEncodedLength());
        _fundamentalParameterDataStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::FundamentalParameterDataStruct >(_fundamentalParameterDataStructLengthlessArrayDecoder);
    }
    DevStudio::FuseTypeEnum::FuseTypeEnum Decoders::decodeFuseTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _fuseTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_fuseTypeEnumDecoder.getEncodedLength());
        _fuseTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::FuseTypeEnum::FuseTypeEnum(_fuseTypeEnumDecoder);
    }
    DevStudio::GaussPlumeGeomRecStruct Decoders::decodeGaussPlumeGeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gaussPlumeGeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gaussPlumeGeomRecStructDecoder.getEncodedLength());
        _gaussPlumeGeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::GaussPlumeGeomRecStruct(_gaussPlumeGeomRecStructDecoder);
    }
    DevStudio::GaussPuffGeomRecStruct Decoders::decodeGaussPuffGeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gaussPuffGeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gaussPuffGeomRecStructDecoder.getEncodedLength());
        _gaussPuffGeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::GaussPuffGeomRecStruct(_gaussPuffGeomRecStructDecoder);
    }
    DevStudio::GeoLocationTypeEnum::GeoLocationTypeEnum Decoders::decodeGeoLocationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geoLocationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_geoLocationTypeEnumDecoder.getEncodedLength());
        _geoLocationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::GeoLocationTypeEnum::GeoLocationTypeEnum(_geoLocationTypeEnumDecoder);
    }
    DevStudio::GeoReferenceVariant Decoders::decodeGeoReferenceVariant(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geoReferenceVariantDecoder.decodeFrom(data, 0);
        buffer.reserve(_geoReferenceVariantDecoder.getEncodedLength());
        _geoReferenceVariantDecoder.encodeInto(buffer);
        return DevStudio::GeoReferenceVariant(_geoReferenceVariantDecoder);
    }
    DevStudio::GeodeticCircle Decoders::decodeGeodeticCircle(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geodeticCircleDecoder.decodeFrom(data, 0);
        buffer.reserve(_geodeticCircleDecoder.getEncodedLength());
        _geodeticCircleDecoder.encodeInto(buffer);
        return DevStudio::GeodeticCircle(_geodeticCircleDecoder);
    }
    DevStudio::GeodeticLocation Decoders::decodeGeodeticLocation(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geodeticLocationDecoder.decodeFrom(data, 0);
        buffer.reserve(_geodeticLocationDecoder.getEncodedLength());
        _geodeticLocationDecoder.encodeInto(buffer);
        return DevStudio::GeodeticLocation(_geodeticLocationDecoder);
    }
    std::vector< DevStudio::GeodeticPoint > Decoders::decodeGeodeticPath(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geodeticPathDecoder.decodeFrom(data, 0);
        buffer.reserve(_geodeticPathDecoder.getEncodedLength());
        _geodeticPathDecoder.encodeInto(buffer);
        return std::vector< DevStudio::GeodeticPoint >(_geodeticPathDecoder);
    }
    DevStudio::GeodeticPoint Decoders::decodeGeodeticPoint(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geodeticPointDecoder.decodeFrom(data, 0);
        buffer.reserve(_geodeticPointDecoder.getEncodedLength());
        _geodeticPointDecoder.encodeInto(buffer);
        return DevStudio::GeodeticPoint(_geodeticPointDecoder);
    }
    std::vector< DevStudio::GeodeticLocation > Decoders::decodeGeodeticPolygon(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geodeticPolygonDecoder.decodeFrom(data, 0);
        buffer.reserve(_geodeticPolygonDecoder.getEncodedLength());
        _geodeticPolygonDecoder.encodeInto(buffer);
        return std::vector< DevStudio::GeodeticLocation >(_geodeticPolygonDecoder);
    }
    DevStudio::GeodeticQuadrangle Decoders::decodeGeodeticQuadrangle(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _geodeticQuadrangleDecoder.decodeFrom(data, 0);
        buffer.reserve(_geodeticQuadrangleDecoder.getEncodedLength());
        _geodeticQuadrangleDecoder.encodeInto(buffer);
        return DevStudio::GeodeticQuadrangle(_geodeticQuadrangleDecoder);
    }
    DevStudio::GridAxisStruct Decoders::decodeGridAxisStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridAxisStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridAxisStructDecoder.getEncodedLength());
        _gridAxisStructDecoder.encodeInto(buffer);
        return DevStudio::GridAxisStruct(_gridAxisStructDecoder);
    }
    std::vector< DevStudio::GridAxisStruct > Decoders::decodeGridAxisStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridAxisStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridAxisStructLengthlessArrayDecoder.getEncodedLength());
        _gridAxisStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::GridAxisStruct >(_gridAxisStructLengthlessArrayDecoder);
    }
    DevStudio::GridAxisTypeVariantStruct Decoders::decodeGridAxisTypeVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridAxisTypeVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridAxisTypeVariantStructDecoder.getEncodedLength());
        _gridAxisTypeVariantStructDecoder.encodeInto(buffer);
        return DevStudio::GridAxisTypeVariantStruct(_gridAxisTypeVariantStructDecoder);
    }
    DevStudio::GridDataRepresentationVariantStruct Decoders::decodeGridDataRepresentationVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridDataRepresentationVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridDataRepresentationVariantStructDecoder.getEncodedLength());
        _gridDataRepresentationVariantStructDecoder.encodeInto(buffer);
        return DevStudio::GridDataRepresentationVariantStruct(_gridDataRepresentationVariantStructDecoder);
    }
    DevStudio::GridDataStruct Decoders::decodeGridDataStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridDataStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridDataStructDecoder.getEncodedLength());
        _gridDataStructDecoder.encodeInto(buffer);
        return DevStudio::GridDataStruct(_gridDataStructDecoder);
    }
    std::vector< DevStudio::GridDataStruct > Decoders::decodeGridDataStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridDataStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridDataStructLengthlessArrayDecoder.getEncodedLength());
        _gridDataStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::GridDataStruct >(_gridDataStructLengthlessArrayDecoder);
    }
    DevStudio::GridValueType0Struct Decoders::decodeGridValueType0Struct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridValueType0StructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridValueType0StructDecoder.getEncodedLength());
        _gridValueType0StructDecoder.encodeInto(buffer);
        return DevStudio::GridValueType0Struct(_gridValueType0StructDecoder);
    }
    DevStudio::GridValueType1Struct Decoders::decodeGridValueType1Struct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridValueType1StructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridValueType1StructDecoder.getEncodedLength());
        _gridValueType1StructDecoder.encodeInto(buffer);
        return DevStudio::GridValueType1Struct(_gridValueType1StructDecoder);
    }
    DevStudio::GridValueType2Struct Decoders::decodeGridValueType2Struct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _gridValueType2StructDecoder.decodeFrom(data, 0);
        buffer.reserve(_gridValueType2StructDecoder.getEncodedLength());
        _gridValueType2StructDecoder.encodeInto(buffer);
        return DevStudio::GridValueType2Struct(_gridValueType2StructDecoder);
    }
    DevStudio::GroundFormationTypeEnum::GroundFormationTypeEnum Decoders::decodeGroundFormationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _groundFormationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_groundFormationTypeEnumDecoder.getEncodedLength());
        _groundFormationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::GroundFormationTypeEnum::GroundFormationTypeEnum(_groundFormationTypeEnumDecoder);
    }
    std::vector< std::wstring > Decoders::decodeHLAargumentList(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAargumentListDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAargumentListDecoder.getEncodedLength());
        _hLAargumentListDecoder.encodeInto(buffer);
        return std::vector< std::wstring >(_hLAargumentListDecoder);
    }
    DevStudio::HLAfederateState::HLAfederateState Decoders::decodeHLAfederateState(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAfederateStateDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAfederateStateDecoder.getEncodedLength());
        _hLAfederateStateDecoder.encodeInto(buffer);
        return DevStudio::HLAfederateState::HLAfederateState(_hLAfederateStateDecoder);
    }
    std::vector< std::vector<char > > Decoders::decodeHLAhandleList(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAhandleListDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAhandleListDecoder.getEncodedLength());
        _hLAhandleListDecoder.encodeInto(buffer);
        return std::vector< std::vector<char > >(_hLAhandleListDecoder);
    }
    DevStudio::HLAinteractionCount Decoders::decodeHLAinteractionCount(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAinteractionCountDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAinteractionCountDecoder.getEncodedLength());
        _hLAinteractionCountDecoder.encodeInto(buffer);
        return DevStudio::HLAinteractionCount(_hLAinteractionCountDecoder);
    }
    std::vector< DevStudio::HLAinteractionCount > Decoders::decodeHLAinteractionCounts(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAinteractionCountsDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAinteractionCountsDecoder.getEncodedLength());
        _hLAinteractionCountsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::HLAinteractionCount >(_hLAinteractionCountsDecoder);
    }
    std::vector< DevStudio::HLAinteractionSubscription > Decoders::decodeHLAinteractionSubList(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAinteractionSubListDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAinteractionSubListDecoder.getEncodedLength());
        _hLAinteractionSubListDecoder.encodeInto(buffer);
        return std::vector< DevStudio::HLAinteractionSubscription >(_hLAinteractionSubListDecoder);
    }
    DevStudio::HLAinteractionSubscription Decoders::decodeHLAinteractionSubscription(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAinteractionSubscriptionDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAinteractionSubscriptionDecoder.getEncodedLength());
        _hLAinteractionSubscriptionDecoder.encodeInto(buffer);
        return DevStudio::HLAinteractionSubscription(_hLAinteractionSubscriptionDecoder);
    }
    std::vector< std::wstring > Decoders::decodeHLAmoduleDesignatorList(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAmoduleDesignatorListDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAmoduleDesignatorListDecoder.getEncodedLength());
        _hLAmoduleDesignatorListDecoder.encodeInto(buffer);
        return std::vector< std::wstring >(_hLAmoduleDesignatorListDecoder);
    }
    DevStudio::HLAobjectClassBasedCount Decoders::decodeHLAobjectClassBasedCount(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAobjectClassBasedCountDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAobjectClassBasedCountDecoder.getEncodedLength());
        _hLAobjectClassBasedCountDecoder.encodeInto(buffer);
        return DevStudio::HLAobjectClassBasedCount(_hLAobjectClassBasedCountDecoder);
    }
    std::vector< DevStudio::HLAobjectClassBasedCount > Decoders::decodeHLAobjectClassBasedCounts(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAobjectClassBasedCountsDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAobjectClassBasedCountsDecoder.getEncodedLength());
        _hLAobjectClassBasedCountsDecoder.encodeInto(buffer);
        return std::vector< DevStudio::HLAobjectClassBasedCount >(_hLAobjectClassBasedCountsDecoder);
    }
    DevStudio::HLAorderType::HLAorderType Decoders::decodeHLAorderType(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAorderTypeDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAorderTypeDecoder.getEncodedLength());
        _hLAorderTypeDecoder.encodeInto(buffer);
        return DevStudio::HLAorderType::HLAorderType(_hLAorderTypeDecoder);
    }
    DevStudio::HLAownership::HLAownership Decoders::decodeHLAownership(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAownershipDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAownershipDecoder.getEncodedLength());
        _hLAownershipDecoder.encodeInto(buffer);
        return DevStudio::HLAownership::HLAownership(_hLAownershipDecoder);
    }
    DevStudio::HLAresignAction::HLAresignAction Decoders::decodeHLAresignAction(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAresignActionDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAresignActionDecoder.getEncodedLength());
        _hLAresignActionDecoder.encodeInto(buffer);
        return DevStudio::HLAresignAction::HLAresignAction(_hLAresignActionDecoder);
    }
    DevStudio::HLAswitch::HLAswitch Decoders::decodeHLAswitch(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAswitchDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAswitchDecoder.getEncodedLength());
        _hLAswitchDecoder.encodeInto(buffer);
        return DevStudio::HLAswitch::HLAswitch(_hLAswitchDecoder);
    }
    DevStudio::HLAsynchPointFederate Decoders::decodeHLAsynchPointFederate(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAsynchPointFederateDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAsynchPointFederateDecoder.getEncodedLength());
        _hLAsynchPointFederateDecoder.encodeInto(buffer);
        return DevStudio::HLAsynchPointFederate(_hLAsynchPointFederateDecoder);
    }
    std::vector< DevStudio::HLAsynchPointFederate > Decoders::decodeHLAsynchPointFederateList(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAsynchPointFederateListDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAsynchPointFederateListDecoder.getEncodedLength());
        _hLAsynchPointFederateListDecoder.encodeInto(buffer);
        return std::vector< DevStudio::HLAsynchPointFederate >(_hLAsynchPointFederateListDecoder);
    }
    std::vector< std::wstring > Decoders::decodeHLAsynchPointList(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAsynchPointListDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAsynchPointListDecoder.getEncodedLength());
        _hLAsynchPointListDecoder.encodeInto(buffer);
        return std::vector< std::wstring >(_hLAsynchPointListDecoder);
    }
    DevStudio::HLAsynchPointStatus::HLAsynchPointStatus Decoders::decodeHLAsynchPointStatus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAsynchPointStatusDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAsynchPointStatusDecoder.getEncodedLength());
        _hLAsynchPointStatusDecoder.encodeInto(buffer);
        return DevStudio::HLAsynchPointStatus::HLAsynchPointStatus(_hLAsynchPointStatusDecoder);
    }
    DevStudio::HLAtimeState::HLAtimeState Decoders::decodeHLAtimeState(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hLAtimeStateDecoder.decodeFrom(data, 0);
        buffer.reserve(_hLAtimeStateDecoder.getEncodedLength());
        _hLAtimeStateDecoder.encodeInto(buffer);
        return DevStudio::HLAtimeState::HLAtimeState(_hLAtimeStateDecoder);
    }
    DevStudio::HatchStateEnum::HatchStateEnum Decoders::decodeHatchStateEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hatchStateEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_hatchStateEnumDecoder.getEncodedLength());
        _hatchStateEnumDecoder.encodeInto(buffer);
        return DevStudio::HatchStateEnum::HatchStateEnum(_hatchStateEnumDecoder);
    }
    DevStudio::HazardTypeEnum::HazardTypeEnum Decoders::decodeHazardTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hazardTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_hazardTypeEnumDecoder.getEncodedLength());
        _hazardTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::HazardTypeEnum::HazardTypeEnum(_hazardTypeEnumDecoder);
    }
    DevStudio::HazardTypeEnum2::HazardTypeEnum2 Decoders::decodeHazardTypeEnum2(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hazardTypeEnum2Decoder.decodeFrom(data, 0);
        buffer.reserve(_hazardTypeEnum2Decoder.getEncodedLength());
        _hazardTypeEnum2Decoder.encodeInto(buffer);
        return DevStudio::HazardTypeEnum2::HazardTypeEnum2(_hazardTypeEnum2Decoder);
    }
    DevStudio::HazeStruct Decoders::decodeHazeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hazeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_hazeStructDecoder.getEncodedLength());
        _hazeStructDecoder.encodeInto(buffer);
        return DevStudio::HazeStruct(_hazeStructDecoder);
    }
    DevStudio::HazeTypeEnum::HazeTypeEnum Decoders::decodeHazeTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hazeTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_hazeTypeEnumDecoder.getEncodedLength());
        _hazeTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::HazeTypeEnum::HazeTypeEnum(_hazeTypeEnumDecoder);
    }
    DevStudio::Holding Decoders::decodeHolding(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _holdingDecoder.decodeFrom(data, 0);
        buffer.reserve(_holdingDecoder.getEncodedLength());
        _holdingDecoder.encodeInto(buffer);
        return DevStudio::Holding(_holdingDecoder);
    }
    DevStudio::HostilityStatusCodeEnum::HostilityStatusCodeEnum Decoders::decodeHostilityStatusCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _hostilityStatusCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_hostilityStatusCodeEnumDecoder.getEncodedLength());
        _hostilityStatusCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::HostilityStatusCodeEnum::HostilityStatusCodeEnum(_hostilityStatusCodeEnumDecoder);
    }
    DevStudio::IPETypeEnum::IPETypeEnum Decoders::decodeIPETypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iPETypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iPETypeEnumDecoder.getEncodedLength());
        _iPETypeEnumDecoder.encodeInto(buffer);
        return DevStudio::IPETypeEnum::IPETypeEnum(_iPETypeEnumDecoder);
    }
    DevStudio::IceStruct Decoders::decodeIceStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iceStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_iceStructDecoder.getEncodedLength());
        _iceStructDecoder.encodeInto(buffer);
        return DevStudio::IceStruct(_iceStructDecoder);
    }
    DevStudio::IceTypeEnum::IceTypeEnum Decoders::decodeIceTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iceTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iceTypeEnumDecoder.getEncodedLength());
        _iceTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::IceTypeEnum::IceTypeEnum(_iceTypeEnumDecoder);
    }
    DevStudio::IdentificationLevelEnum::IdentificationLevelEnum Decoders::decodeIdentificationLevelEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _identificationLevelEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_identificationLevelEnumDecoder.getEncodedLength());
        _identificationLevelEnumDecoder.encodeInto(buffer);
        return DevStudio::IdentificationLevelEnum::IdentificationLevelEnum(_identificationLevelEnumDecoder);
    }
    DevStudio::IffAlternateMode4Enum::IffAlternateMode4Enum Decoders::decodeIffAlternateMode4Enum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffAlternateMode4EnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffAlternateMode4EnumDecoder.getEncodedLength());
        _iffAlternateMode4EnumDecoder.encodeInto(buffer);
        return DevStudio::IffAlternateMode4Enum::IffAlternateMode4Enum(_iffAlternateMode4EnumDecoder);
    }
    DevStudio::IffApplicableModesEnum::IffApplicableModesEnum Decoders::decodeIffApplicableModesEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffApplicableModesEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffApplicableModesEnumDecoder.getEncodedLength());
        _iffApplicableModesEnumDecoder.encodeInto(buffer);
        return DevStudio::IffApplicableModesEnum::IffApplicableModesEnum(_iffApplicableModesEnumDecoder);
    }
    DevStudio::IffOperationalParameter1Enum::IffOperationalParameter1Enum Decoders::decodeIffOperationalParameter1Enum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffOperationalParameter1EnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffOperationalParameter1EnumDecoder.getEncodedLength());
        _iffOperationalParameter1EnumDecoder.encodeInto(buffer);
        return DevStudio::IffOperationalParameter1Enum::IffOperationalParameter1Enum(_iffOperationalParameter1EnumDecoder);
    }
    DevStudio::IffOperationalParameter2Enum::IffOperationalParameter2Enum Decoders::decodeIffOperationalParameter2Enum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffOperationalParameter2EnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffOperationalParameter2EnumDecoder.getEncodedLength());
        _iffOperationalParameter2EnumDecoder.encodeInto(buffer);
        return DevStudio::IffOperationalParameter2Enum::IffOperationalParameter2Enum(_iffOperationalParameter2EnumDecoder);
    }
    DevStudio::IffSystemModeEnum::IffSystemModeEnum Decoders::decodeIffSystemModeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffSystemModeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffSystemModeEnumDecoder.getEncodedLength());
        _iffSystemModeEnumDecoder.encodeInto(buffer);
        return DevStudio::IffSystemModeEnum::IffSystemModeEnum(_iffSystemModeEnumDecoder);
    }
    DevStudio::IffSystemNameEnum::IffSystemNameEnum Decoders::decodeIffSystemNameEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffSystemNameEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffSystemNameEnumDecoder.getEncodedLength());
        _iffSystemNameEnumDecoder.encodeInto(buffer);
        return DevStudio::IffSystemNameEnum::IffSystemNameEnum(_iffSystemNameEnumDecoder);
    }
    DevStudio::IffSystemTypeEnum::IffSystemTypeEnum Decoders::decodeIffSystemTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _iffSystemTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_iffSystemTypeEnumDecoder.getEncodedLength());
        _iffSystemTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::IffSystemTypeEnum::IffSystemTypeEnum(_iffSystemTypeEnumDecoder);
    }
    std::vector< DevStudio::IncomingConnectionStruct > Decoders::decodeIncomingConnectionArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _incomingConnectionArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_incomingConnectionArrayDecoder.getEncodedLength());
        _incomingConnectionArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::IncomingConnectionStruct >(_incomingConnectionArrayDecoder);
    }
    DevStudio::IncomingConnectionStruct Decoders::decodeIncomingConnectionStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _incomingConnectionStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_incomingConnectionStructDecoder.getEncodedLength());
        _incomingConnectionStructDecoder.encodeInto(buffer);
        return DevStudio::IncomingConnectionStruct(_incomingConnectionStructDecoder);
    }
    DevStudio::IndirectFireTaskStruct Decoders::decodeIndirectFireTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _indirectFireTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_indirectFireTaskStructDecoder.getEncodedLength());
        _indirectFireTaskStructDecoder.encodeInto(buffer);
        return DevStudio::IndirectFireTaskStruct(_indirectFireTaskStructDecoder);
    }
    DevStudio::InstallationSymbolAmplificationStruct Decoders::decodeInstallationSymbolAmplificationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _installationSymbolAmplificationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_installationSymbolAmplificationStructDecoder.getEncodedLength());
        _installationSymbolAmplificationStructDecoder.encodeInto(buffer);
        return DevStudio::InstallationSymbolAmplificationStruct(_installationSymbolAmplificationStructDecoder);
    }
    std::vector< short > Decoders::decodeInteger16Array1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _integer16Array1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_integer16Array1PlusDecoder.getEncodedLength());
        _integer16Array1PlusDecoder.encodeInto(buffer);
        return std::vector< short >(_integer16Array1PlusDecoder);
    }
    DevStudio::IrregularGridAxisStruct Decoders::decodeIrregularGridAxisStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _irregularGridAxisStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_irregularGridAxisStructDecoder.getEncodedLength());
        _irregularGridAxisStructDecoder.encodeInto(buffer);
        return DevStudio::IrregularGridAxisStruct(_irregularGridAxisStructDecoder);
    }
    DevStudio::IsPartOfStruct Decoders::decodeIsPartOfStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _isPartOfStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_isPartOfStructDecoder.getEncodedLength());
        _isPartOfStructDecoder.encodeInto(buffer);
        return DevStudio::IsPartOfStruct(_isPartOfStructDecoder);
    }
    DevStudio::LandslideTypeEnum::LandslideTypeEnum Decoders::decodeLandslideTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _landslideTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_landslideTypeEnumDecoder.getEncodedLength());
        _landslideTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::LandslideTypeEnum::LandslideTypeEnum(_landslideTypeEnumDecoder);
    }
    DevStudio::LayMinefieldTaskStruct Decoders::decodeLayMinefieldTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _layMinefieldTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_layMinefieldTaskStructDecoder.getEncodedLength());
        _layMinefieldTaskStructDecoder.encodeInto(buffer);
        return DevStudio::LayMinefieldTaskStruct(_layMinefieldTaskStructDecoder);
    }
    DevStudio::LayerStruct Decoders::decodeLayerStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _layerStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_layerStructDecoder.getEncodedLength());
        _layerStructDecoder.encodeInto(buffer);
        return DevStudio::LayerStruct(_layerStructDecoder);
    }
    DevStudio::LightningTypeEnum::LightningTypeEnum Decoders::decodeLightningTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _lightningTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_lightningTypeEnumDecoder.getEncodedLength());
        _lightningTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::LightningTypeEnum::LightningTypeEnum(_lightningTypeEnumDecoder);
    }
    DevStudio::Line1GeomRecStruct Decoders::decodeLine1GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _line1GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_line1GeomRecStructDecoder.getEncodedLength());
        _line1GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Line1GeomRecStruct(_line1GeomRecStructDecoder);
    }
    DevStudio::Line2GeomRecStruct Decoders::decodeLine2GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _line2GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_line2GeomRecStructDecoder.getEncodedLength());
        _line2GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Line2GeomRecStruct(_line2GeomRecStructDecoder);
    }
    DevStudio::LinearSegmentStruct Decoders::decodeLinearSegmentStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _linearSegmentStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_linearSegmentStructDecoder.getEncodedLength());
        _linearSegmentStructDecoder.encodeInto(buffer);
        return DevStudio::LinearSegmentStruct(_linearSegmentStructDecoder);
    }
    std::vector< DevStudio::LinkStatusStruct > Decoders::decodeLinkStatusArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _linkStatusArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_linkStatusArrayDecoder.getEncodedLength());
        _linkStatusArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::LinkStatusStruct >(_linkStatusArrayDecoder);
    }
    DevStudio::LinkStatusStruct Decoders::decodeLinkStatusStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _linkStatusStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_linkStatusStructDecoder.getEncodedLength());
        _linkStatusStructDecoder.encodeInto(buffer);
        return DevStudio::LinkStatusStruct(_linkStatusStructDecoder);
    }
    DevStudio::LocationStruct Decoders::decodeLocationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _locationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_locationStructDecoder.getEncodedLength());
        _locationStructDecoder.encodeInto(buffer);
        return DevStudio::LocationStruct(_locationStructDecoder);
    }
    std::vector< DevStudio::LocationStruct > Decoders::decodeLocationStructArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _locationStructArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_locationStructArrayDecoder.getEncodedLength());
        _locationStructArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::LocationStruct >(_locationStructArrayDecoder);
    }
    DevStudio::MagicMoveTaskStruct Decoders::decodeMagicMoveTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _magicMoveTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_magicMoveTaskStructDecoder.getEncodedLength());
        _magicMoveTaskStructDecoder.encodeInto(buffer);
        return DevStudio::MagicMoveTaskStruct(_magicMoveTaskStructDecoder);
    }
    DevStudio::MajorRFModulationTypeEnum::MajorRFModulationTypeEnum Decoders::decodeMajorRFModulationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _majorRFModulationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_majorRFModulationTypeEnumDecoder.getEncodedLength());
        _majorRFModulationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::MajorRFModulationTypeEnum::MajorRFModulationTypeEnum(_majorRFModulationTypeEnumDecoder);
    }
    DevStudio::ManeuverIndicatorEnum::ManeuverIndicatorEnum Decoders::decodeManeuverIndicatorEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _maneuverIndicatorEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_maneuverIndicatorEnumDecoder.getEncodedLength());
        _maneuverIndicatorEnumDecoder.encodeInto(buffer);
        return DevStudio::ManeuverIndicatorEnum::ManeuverIndicatorEnum(_maneuverIndicatorEnumDecoder);
    }
    std::vector< char > Decoders::decodeManufacturerIdArray3(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _manufacturerIdArray3Decoder.decodeFrom(data, 0);
        buffer.reserve(_manufacturerIdArray3Decoder.getEncodedLength());
        _manufacturerIdArray3Decoder.encodeInto(buffer);
        return std::vector< char >(_manufacturerIdArray3Decoder);
    }
    std::vector< char > Decoders::decodeMarkingArray11(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _markingArray11Decoder.decodeFrom(data, 0);
        buffer.reserve(_markingArray11Decoder.getEncodedLength());
        _markingArray11Decoder.encodeInto(buffer);
        return std::vector< char >(_markingArray11Decoder);
    }
    std::vector< char > Decoders::decodeMarkingArray31(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _markingArray31Decoder.decodeFrom(data, 0);
        buffer.reserve(_markingArray31Decoder.getEncodedLength());
        _markingArray31Decoder.encodeInto(buffer);
        return std::vector< char >(_markingArray31Decoder);
    }
    DevStudio::MarkingEncodingEnum::MarkingEncodingEnum Decoders::decodeMarkingEncodingEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _markingEncodingEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_markingEncodingEnumDecoder.getEncodedLength());
        _markingEncodingEnumDecoder.encodeInto(buffer);
        return DevStudio::MarkingEncodingEnum::MarkingEncodingEnum(_markingEncodingEnumDecoder);
    }
    DevStudio::MarkingStruct Decoders::decodeMarkingStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _markingStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_markingStructDecoder.getEncodedLength());
        _markingStructDecoder.encodeInto(buffer);
        return DevStudio::MarkingStruct(_markingStructDecoder);
    }
    std::vector< float > Decoders::decodeMineDielectricDifferenceLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _mineDielectricDifferenceLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_mineDielectricDifferenceLengthlessArrayDecoder.getEncodedLength());
        _mineDielectricDifferenceLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< float >(_mineDielectricDifferenceLengthlessArrayDecoder);
    }
    DevStudio::MineFusingStruct Decoders::decodeMineFusingStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _mineFusingStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_mineFusingStructDecoder.getEncodedLength());
        _mineFusingStructDecoder.encodeInto(buffer);
        return DevStudio::MineFusingStruct(_mineFusingStructDecoder);
    }
    std::vector< DevStudio::MineFusingStruct > Decoders::decodeMineFusingStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _mineFusingStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_mineFusingStructLengthlessArrayDecoder.getEncodedLength());
        _mineFusingStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::MineFusingStruct >(_mineFusingStructLengthlessArrayDecoder);
    }
    std::vector< unsigned short > Decoders::decodeMineIdentifierLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _mineIdentifierLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_mineIdentifierLengthlessArrayDecoder.getEncodedLength());
        _mineIdentifierLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< unsigned short >(_mineIdentifierLengthlessArrayDecoder);
    }
    DevStudio::MinefieldFusingEnum::MinefieldFusingEnum Decoders::decodeMinefieldFusingEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldFusingEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldFusingEnumDecoder.getEncodedLength());
        _minefieldFusingEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldFusingEnum::MinefieldFusingEnum(_minefieldFusingEnumDecoder);
    }
    DevStudio::MinefieldLaneEnum::MinefieldLaneEnum Decoders::decodeMinefieldLaneEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldLaneEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldLaneEnumDecoder.getEncodedLength());
        _minefieldLaneEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldLaneEnum::MinefieldLaneEnum(_minefieldLaneEnumDecoder);
    }
    DevStudio::MinefieldLaneMarkerStruct Decoders::decodeMinefieldLaneMarkerStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldLaneMarkerStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldLaneMarkerStructDecoder.getEncodedLength());
        _minefieldLaneMarkerStructDecoder.encodeInto(buffer);
        return DevStudio::MinefieldLaneMarkerStruct(_minefieldLaneMarkerStructDecoder);
    }
    std::vector< DevStudio::MinefieldLaneMarkerStruct > Decoders::decodeMinefieldLaneMarkerStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldLaneMarkerStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldLaneMarkerStructLengthlessArrayDecoder.getEncodedLength());
        _minefieldLaneMarkerStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::MinefieldLaneMarkerStruct >(_minefieldLaneMarkerStructLengthlessArrayDecoder);
    }
    DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum Decoders::decodeMinefieldPaintSchemeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldPaintSchemeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldPaintSchemeEnumDecoder.getEncodedLength());
        _minefieldPaintSchemeEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum(_minefieldPaintSchemeEnumDecoder);
    }
    std::vector< DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum > Decoders::decodeMinefieldPaintSchemeLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldPaintSchemeLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldPaintSchemeLengthlessArrayDecoder.getEncodedLength());
        _minefieldPaintSchemeLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::MinefieldPaintSchemeEnum::MinefieldPaintSchemeEnum >(_minefieldPaintSchemeLengthlessArrayDecoder);
    }
    DevStudio::MinefieldProtocolEnum::MinefieldProtocolEnum Decoders::decodeMinefieldProtocolEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldProtocolEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldProtocolEnumDecoder.getEncodedLength());
        _minefieldProtocolEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldProtocolEnum::MinefieldProtocolEnum(_minefieldProtocolEnumDecoder);
    }
    DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum Decoders::decodeMinefieldSensorTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldSensorTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldSensorTypeEnumDecoder.getEncodedLength());
        _minefieldSensorTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum(_minefieldSensorTypeEnumDecoder);
    }
    std::vector< DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum > Decoders::decodeMinefieldSensorTypeLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldSensorTypeLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldSensorTypeLengthlessArrayDecoder.getEncodedLength());
        _minefieldSensorTypeLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::MinefieldSensorTypeEnum::MinefieldSensorTypeEnum >(_minefieldSensorTypeLengthlessArrayDecoder);
    }
    DevStudio::MinefieldStatusEnum::MinefieldStatusEnum Decoders::decodeMinefieldStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldStatusEnumDecoder.getEncodedLength());
        _minefieldStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldStatusEnum::MinefieldStatusEnum(_minefieldStatusEnumDecoder);
    }
    DevStudio::MinefieldTypeEnum::MinefieldTypeEnum Decoders::decodeMinefieldTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _minefieldTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_minefieldTypeEnumDecoder.getEncodedLength());
        _minefieldTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::MinefieldTypeEnum::MinefieldTypeEnum(_minefieldTypeEnumDecoder);
    }
    std::vector< char > Decoders::decodeMissingRecordNumbersLengthlessArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _missingRecordNumbersLengthlessArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_missingRecordNumbersLengthlessArray1PlusDecoder.getEncodedLength());
        _missingRecordNumbersLengthlessArray1PlusDecoder.encodeInto(buffer);
        return std::vector< char >(_missingRecordNumbersLengthlessArray1PlusDecoder);
    }
    DevStudio::MoveByRouteTaskStruct Decoders::decodeMoveByRouteTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _moveByRouteTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_moveByRouteTaskStructDecoder.getEncodedLength());
        _moveByRouteTaskStructDecoder.encodeInto(buffer);
        return DevStudio::MoveByRouteTaskStruct(_moveByRouteTaskStructDecoder);
    }
    DevStudio::MoveInDirectionTaskStruct Decoders::decodeMoveInDirectionTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _moveInDirectionTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_moveInDirectionTaskStructDecoder.getEncodedLength());
        _moveInDirectionTaskStructDecoder.encodeInto(buffer);
        return DevStudio::MoveInDirectionTaskStruct(_moveInDirectionTaskStructDecoder);
    }
    DevStudio::MoveIntoFormationTaskStruct Decoders::decodeMoveIntoFormationTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _moveIntoFormationTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_moveIntoFormationTaskStructDecoder.getEncodedLength());
        _moveIntoFormationTaskStructDecoder.encodeInto(buffer);
        return DevStudio::MoveIntoFormationTaskStruct(_moveIntoFormationTaskStructDecoder);
    }
    DevStudio::MoveTaskProgress Decoders::decodeMoveTaskProgress(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _moveTaskProgressDecoder.decodeFrom(data, 0);
        buffer.reserve(_moveTaskProgressDecoder.getEncodedLength());
        _moveTaskProgressDecoder.encodeInto(buffer);
        return DevStudio::MoveTaskProgress(_moveTaskProgressDecoder);
    }
    DevStudio::MoveToLocationTaskStruct Decoders::decodeMoveToLocationTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _moveToLocationTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_moveToLocationTaskStructDecoder.getEncodedLength());
        _moveToLocationTaskStructDecoder.encodeInto(buffer);
        return DevStudio::MoveToLocationTaskStruct(_moveToLocationTaskStructDecoder);
    }
    DevStudio::MoveTypeEnum::MoveTypeEnum Decoders::decodeMoveTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _moveTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_moveTypeEnumDecoder.getEncodedLength());
        _moveTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::MoveTypeEnum::MoveTypeEnum(_moveTypeEnumDecoder);
    }
    DevStudio::MsgIdEnum::MsgIdEnum Decoders::decodeMsgIdEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _msgIdEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_msgIdEnumDecoder.getEncodedLength());
        _msgIdEnumDecoder.encodeInto(buffer);
        return DevStudio::MsgIdEnum::MsgIdEnum(_msgIdEnumDecoder);
    }
    DevStudio::NamedLocationStruct Decoders::decodeNamedLocationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _namedLocationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_namedLocationStructDecoder.getEncodedLength());
        _namedLocationStructDecoder.encodeInto(buffer);
        return DevStudio::NamedLocationStruct(_namedLocationStructDecoder);
    }
    DevStudio::NavigationLightFlashingEnum::NavigationLightFlashingEnum Decoders::decodeNavigationLightFlashingEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _navigationLightFlashingEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_navigationLightFlashingEnumDecoder.getEncodedLength());
        _navigationLightFlashingEnumDecoder.encodeInto(buffer);
        return DevStudio::NavigationLightFlashingEnum::NavigationLightFlashingEnum(_navigationLightFlashingEnumDecoder);
    }
    DevStudio::NavigationStatusEnum::NavigationStatusEnum Decoders::decodeNavigationStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _navigationStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_navigationStatusEnumDecoder.getEncodedLength());
        _navigationStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::NavigationStatusEnum::NavigationStatusEnum(_navigationStatusEnumDecoder);
    }
    std::vector< DevStudio::NetworkDeviceStruct > Decoders::decodeNetworkDeviceArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _networkDeviceArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_networkDeviceArrayDecoder.getEncodedLength());
        _networkDeviceArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::NetworkDeviceStruct >(_networkDeviceArrayDecoder);
    }
    DevStudio::NetworkDeviceEmptyCharactersticsStruct Decoders::decodeNetworkDeviceEmptyCharactersticsStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _networkDeviceEmptyCharactersticsStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_networkDeviceEmptyCharactersticsStructDecoder.getEncodedLength());
        _networkDeviceEmptyCharactersticsStructDecoder.encodeInto(buffer);
        return DevStudio::NetworkDeviceEmptyCharactersticsStruct(_networkDeviceEmptyCharactersticsStructDecoder);
    }
    DevStudio::NetworkDeviceGenericTransmitterCharacteristicsStruct Decoders::decodeNetworkDeviceGenericTransmitterCharacteristicsStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _networkDeviceGenericTransmitterCharacteristicsStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_networkDeviceGenericTransmitterCharacteristicsStructDecoder.getEncodedLength());
        _networkDeviceGenericTransmitterCharacteristicsStructDecoder.encodeInto(buffer);
        return DevStudio::NetworkDeviceGenericTransmitterCharacteristicsStruct(_networkDeviceGenericTransmitterCharacteristicsStructDecoder);
    }
    DevStudio::NetworkDeviceReceiverCharacteristicsVariant Decoders::decodeNetworkDeviceReceiverCharacteristicsVariant(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _networkDeviceReceiverCharacteristicsVariantDecoder.decodeFrom(data, 0);
        buffer.reserve(_networkDeviceReceiverCharacteristicsVariantDecoder.getEncodedLength());
        _networkDeviceReceiverCharacteristicsVariantDecoder.encodeInto(buffer);
        return DevStudio::NetworkDeviceReceiverCharacteristicsVariant(_networkDeviceReceiverCharacteristicsVariantDecoder);
    }
    DevStudio::NetworkDeviceStruct Decoders::decodeNetworkDeviceStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _networkDeviceStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_networkDeviceStructDecoder.getEncodedLength());
        _networkDeviceStructDecoder.encodeInto(buffer);
        return DevStudio::NetworkDeviceStruct(_networkDeviceStructDecoder);
    }
    DevStudio::NetworkDeviceTransmitterCharacteristicsVariant Decoders::decodeNetworkDeviceTransmitterCharacteristicsVariant(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _networkDeviceTransmitterCharacteristicsVariantDecoder.decodeFrom(data, 0);
        buffer.reserve(_networkDeviceTransmitterCharacteristicsVariantDecoder.getEncodedLength());
        _networkDeviceTransmitterCharacteristicsVariantDecoder.encodeInto(buffer);
        return DevStudio::NetworkDeviceTransmitterCharacteristicsVariant(_networkDeviceTransmitterCharacteristicsVariantDecoder);
    }
    DevStudio::NomenclatureEnum::NomenclatureEnum Decoders::decodeNomenclatureEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _nomenclatureEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_nomenclatureEnumDecoder.getEncodedLength());
        _nomenclatureEnumDecoder.encodeInto(buffer);
        return DevStudio::NomenclatureEnum::NomenclatureEnum(_nomenclatureEnumDecoder);
    }
    DevStudio::NomenclatureVersionEnum::NomenclatureVersionEnum Decoders::decodeNomenclatureVersionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _nomenclatureVersionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_nomenclatureVersionEnumDecoder.getEncodedLength());
        _nomenclatureVersionEnumDecoder.encodeInto(buffer);
        return DevStudio::NomenclatureVersionEnum::NomenclatureVersionEnum(_nomenclatureVersionEnumDecoder);
    }
    DevStudio::ObserveTaskStruct Decoders::decodeObserveTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _observeTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_observeTaskStructDecoder.getEncodedLength());
        _observeTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ObserveTaskStruct(_observeTaskStructDecoder);
    }
    DevStudio::ObservedEquipment Decoders::decodeObservedEquipment(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _observedEquipmentDecoder.decodeFrom(data, 0);
        buffer.reserve(_observedEquipmentDecoder.getEncodedLength());
        _observedEquipmentDecoder.encodeInto(buffer);
        return DevStudio::ObservedEquipment(_observedEquipmentDecoder);
    }
    std::vector< char > Decoders::decodeOctetArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_octetArrayDecoder.getEncodedLength());
        _octetArrayDecoder.encodeInto(buffer);
        return std::vector< char >(_octetArrayDecoder);
    }
    std::vector< char > Decoders::decodeOctetArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_octetArray1PlusDecoder.getEncodedLength());
        _octetArray1PlusDecoder.encodeInto(buffer);
        return std::vector< char >(_octetArray1PlusDecoder);
    }
    std::vector< char > Decoders::decodeOctetArray2(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArray2Decoder.decodeFrom(data, 0);
        buffer.reserve(_octetArray2Decoder.getEncodedLength());
        _octetArray2Decoder.encodeInto(buffer);
        return std::vector< char >(_octetArray2Decoder);
    }
    std::vector< char > Decoders::decodeOctetArray3(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArray3Decoder.decodeFrom(data, 0);
        buffer.reserve(_octetArray3Decoder.getEncodedLength());
        _octetArray3Decoder.encodeInto(buffer);
        return std::vector< char >(_octetArray3Decoder);
    }
    std::vector< char > Decoders::decodeOctetArray4(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArray4Decoder.decodeFrom(data, 0);
        buffer.reserve(_octetArray4Decoder.getEncodedLength());
        _octetArray4Decoder.encodeInto(buffer);
        return std::vector< char >(_octetArray4Decoder);
    }
    std::vector< char > Decoders::decodeOctetArray7(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArray7Decoder.decodeFrom(data, 0);
        buffer.reserve(_octetArray7Decoder.getEncodedLength());
        _octetArray7Decoder.encodeInto(buffer);
        return std::vector< char >(_octetArray7Decoder);
    }
    std::vector< char > Decoders::decodeOctetArray8(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetArray8Decoder.decodeFrom(data, 0);
        buffer.reserve(_octetArray8Decoder.getEncodedLength());
        _octetArray8Decoder.encodeInto(buffer);
        return std::vector< char >(_octetArray8Decoder);
    }
    std::vector< char > Decoders::decodeOctetPadding32Array(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetPadding32ArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_octetPadding32ArrayDecoder.getEncodedLength());
        _octetPadding32ArrayDecoder.encodeInto(buffer);
        return std::vector< char >(_octetPadding32ArrayDecoder);
    }
    std::vector< char > Decoders::decodeOctetPadding64Array(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _octetPadding64ArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_octetPadding64ArrayDecoder.getEncodedLength());
        _octetPadding64ArrayDecoder.encodeInto(buffer);
        return std::vector< char >(_octetPadding64ArrayDecoder);
    }
    DevStudio::OpacityCodeEnum::OpacityCodeEnum Decoders::decodeOpacityCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _opacityCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_opacityCodeEnumDecoder.getEncodedLength());
        _opacityCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::OpacityCodeEnum::OpacityCodeEnum(_opacityCodeEnumDecoder);
    }
    DevStudio::OperateCheckpointTaskStruct Decoders::decodeOperateCheckpointTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _operateCheckpointTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_operateCheckpointTaskStructDecoder.getEncodedLength());
        _operateCheckpointTaskStructDecoder.encodeInto(buffer);
        return DevStudio::OperateCheckpointTaskStruct(_operateCheckpointTaskStructDecoder);
    }
    DevStudio::OperateObservationPostTaskStruct Decoders::decodeOperateObservationPostTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _operateObservationPostTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_operateObservationPostTaskStructDecoder.getEncodedLength());
        _operateObservationPostTaskStructDecoder.encodeInto(buffer);
        return DevStudio::OperateObservationPostTaskStruct(_operateObservationPostTaskStructDecoder);
    }
    DevStudio::OrientationStruct Decoders::decodeOrientationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _orientationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_orientationStructDecoder.getEncodedLength());
        _orientationStructDecoder.encodeInto(buffer);
        return DevStudio::OrientationStruct(_orientationStructDecoder);
    }
    std::vector< DevStudio::OrientationStruct > Decoders::decodeOrientationStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _orientationStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_orientationStructLengthlessArrayDecoder.getEncodedLength());
        _orientationStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::OrientationStruct >(_orientationStructLengthlessArrayDecoder);
    }
    DevStudio::OtherActivityTaskStruct Decoders::decodeOtherActivityTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _otherActivityTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_otherActivityTaskStructDecoder.getEncodedLength());
        _otherActivityTaskStructDecoder.encodeInto(buffer);
        return DevStudio::OtherActivityTaskStruct(_otherActivityTaskStructDecoder);
    }
    DevStudio::ParameterTypeEnum::ParameterTypeEnum Decoders::decodeParameterTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _parameterTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_parameterTypeEnumDecoder.getEncodedLength());
        _parameterTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ParameterTypeEnum::ParameterTypeEnum(_parameterTypeEnumDecoder);
    }
    DevStudio::ParameterValueVariantStruct Decoders::decodeParameterValueVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _parameterValueVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_parameterValueVariantStructDecoder.getEncodedLength());
        _parameterValueVariantStructDecoder.encodeInto(buffer);
        return DevStudio::ParameterValueVariantStruct(_parameterValueVariantStructDecoder);
    }
    DevStudio::PartNumberEnum::PartNumberEnum Decoders::decodePartNumberEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _partNumberEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_partNumberEnumDecoder.getEncodedLength());
        _partNumberEnumDecoder.encodeInto(buffer);
        return DevStudio::PartNumberEnum::PartNumberEnum(_partNumberEnumDecoder);
    }
    DevStudio::PatrolMoveTypeEnum::PatrolMoveTypeEnum Decoders::decodePatrolMoveTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _patrolMoveTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_patrolMoveTypeEnumDecoder.getEncodedLength());
        _patrolMoveTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::PatrolMoveTypeEnum::PatrolMoveTypeEnum(_patrolMoveTypeEnumDecoder);
    }
    DevStudio::PatrolTaskProgress Decoders::decodePatrolTaskProgress(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _patrolTaskProgressDecoder.decodeFrom(data, 0);
        buffer.reserve(_patrolTaskProgressDecoder.getEncodedLength());
        _patrolTaskProgressDecoder.encodeInto(buffer);
        return DevStudio::PatrolTaskProgress(_patrolTaskProgressDecoder);
    }
    DevStudio::PatrolTaskStruct Decoders::decodePatrolTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _patrolTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_patrolTaskStructDecoder.getEncodedLength());
        _patrolTaskStructDecoder.encodeInto(buffer);
        return DevStudio::PatrolTaskStruct(_patrolTaskStructDecoder);
    }
    DevStudio::PatrolTypeEnum::PatrolTypeEnum Decoders::decodePatrolTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _patrolTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_patrolTypeEnumDecoder.getEncodedLength());
        _patrolTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::PatrolTypeEnum::PatrolTypeEnum(_patrolTypeEnumDecoder);
    }
    DevStudio::PerimeterPointStruct Decoders::decodePerimeterPointStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _perimeterPointStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_perimeterPointStructDecoder.getEncodedLength());
        _perimeterPointStructDecoder.encodeInto(buffer);
        return DevStudio::PerimeterPointStruct(_perimeterPointStructDecoder);
    }
    std::vector< DevStudio::PerimeterPointStruct > Decoders::decodePerimeterPointStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _perimeterPointStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_perimeterPointStructLengthlessArrayDecoder.getEncodedLength());
        _perimeterPointStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::PerimeterPointStruct >(_perimeterPointStructLengthlessArrayDecoder);
    }
    DevStudio::PhysicalGenericNetworkStruct Decoders::decodePhysicalGenericNetworkStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _physicalGenericNetworkStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_physicalGenericNetworkStructDecoder.getEncodedLength());
        _physicalGenericNetworkStructDecoder.encodeInto(buffer);
        return DevStudio::PhysicalGenericNetworkStruct(_physicalGenericNetworkStructDecoder);
    }
    DevStudio::PhysicalNetworkDescriptionVariant Decoders::decodePhysicalNetworkDescriptionVariant(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _physicalNetworkDescriptionVariantDecoder.decodeFrom(data, 0);
        buffer.reserve(_physicalNetworkDescriptionVariantDecoder.getEncodedLength());
        _physicalNetworkDescriptionVariantDecoder.encodeInto(buffer);
        return DevStudio::PhysicalNetworkDescriptionVariant(_physicalNetworkDescriptionVariantDecoder);
    }
    DevStudio::PhysicalNetworkTypeEnum::PhysicalNetworkTypeEnum Decoders::decodePhysicalNetworkTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _physicalNetworkTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_physicalNetworkTypeEnumDecoder.getEncodedLength());
        _physicalNetworkTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::PhysicalNetworkTypeEnum::PhysicalNetworkTypeEnum(_physicalNetworkTypeEnumDecoder);
    }
    DevStudio::PhysicalUndefinedNetworkStruct Decoders::decodePhysicalUndefinedNetworkStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _physicalUndefinedNetworkStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_physicalUndefinedNetworkStructDecoder.getEncodedLength());
        _physicalUndefinedNetworkStructDecoder.encodeInto(buffer);
        return DevStudio::PhysicalUndefinedNetworkStruct(_physicalUndefinedNetworkStructDecoder);
    }
    DevStudio::PlumeDimensionRateStruct Decoders::decodePlumeDimensionRateStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _plumeDimensionRateStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_plumeDimensionRateStructDecoder.getEncodedLength());
        _plumeDimensionRateStructDecoder.encodeInto(buffer);
        return DevStudio::PlumeDimensionRateStruct(_plumeDimensionRateStructDecoder);
    }
    DevStudio::PlumeDimensionStruct Decoders::decodePlumeDimensionStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _plumeDimensionStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_plumeDimensionStructDecoder.getEncodedLength());
        _plumeDimensionStructDecoder.encodeInto(buffer);
        return DevStudio::PlumeDimensionStruct(_plumeDimensionStructDecoder);
    }
    DevStudio::Point2GeomRecStruct Decoders::decodePoint2GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _point2GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_point2GeomRecStructDecoder.getEncodedLength());
        _point2GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Point2GeomRecStruct(_point2GeomRecStructDecoder);
    }
    DevStudio::PrecipitationStruct Decoders::decodePrecipitationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _precipitationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_precipitationStructDecoder.getEncodedLength());
        _precipitationStructDecoder.encodeInto(buffer);
        return DevStudio::PrecipitationStruct(_precipitationStructDecoder);
    }
    DevStudio::PrecipitationTypeEnum::PrecipitationTypeEnum Decoders::decodePrecipitationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _precipitationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_precipitationTypeEnumDecoder.getEncodedLength());
        _precipitationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::PrecipitationTypeEnum::PrecipitationTypeEnum(_precipitationTypeEnumDecoder);
    }
    DevStudio::PropulsionPlantEnum::PropulsionPlantEnum Decoders::decodePropulsionPlantEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _propulsionPlantEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_propulsionPlantEnumDecoder.getEncodedLength());
        _propulsionPlantEnumDecoder.encodeInto(buffer);
        return DevStudio::PropulsionPlantEnum::PropulsionPlantEnum(_propulsionPlantEnumDecoder);
    }
    DevStudio::PropulsionSystemDataStruct Decoders::decodePropulsionSystemDataStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _propulsionSystemDataStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_propulsionSystemDataStructDecoder.getEncodedLength());
        _propulsionSystemDataStructDecoder.encodeInto(buffer);
        return DevStudio::PropulsionSystemDataStruct(_propulsionSystemDataStructDecoder);
    }
    std::vector< DevStudio::PropulsionSystemDataStruct > Decoders::decodePropulsionSystemDataStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _propulsionSystemDataStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_propulsionSystemDataStructLengthlessArrayDecoder.getEncodedLength());
        _propulsionSystemDataStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::PropulsionSystemDataStruct >(_propulsionSystemDataStructLengthlessArrayDecoder);
    }
    DevStudio::ProtectionEffectivenessStruct Decoders::decodeProtectionEffectivenessStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _protectionEffectivenessStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_protectionEffectivenessStructDecoder.getEncodedLength());
        _protectionEffectivenessStructDecoder.encodeInto(buffer);
        return DevStudio::ProtectionEffectivenessStruct(_protectionEffectivenessStructDecoder);
    }
    DevStudio::PulseModulationTypeEnum::PulseModulationTypeEnum Decoders::decodePulseModulationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _pulseModulationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_pulseModulationTypeEnumDecoder.getEncodedLength());
        _pulseModulationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::PulseModulationTypeEnum::PulseModulationTypeEnum(_pulseModulationTypeEnumDecoder);
    }
    DevStudio::PumpFloodingTaskStruct Decoders::decodePumpFloodingTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _pumpFloodingTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_pumpFloodingTaskStructDecoder.getEncodedLength());
        _pumpFloodingTaskStructDecoder.encodeInto(buffer);
        return DevStudio::PumpFloodingTaskStruct(_pumpFloodingTaskStructDecoder);
    }
    DevStudio::RFModulationSystemTypeEnum::RFModulationSystemTypeEnum Decoders::decodeRFModulationSystemTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rFModulationSystemTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_rFModulationSystemTypeEnumDecoder.getEncodedLength());
        _rFModulationSystemTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::RFModulationSystemTypeEnum::RFModulationSystemTypeEnum(_rFModulationSystemTypeEnumDecoder);
    }
    DevStudio::RFModulationTypeVariantStruct Decoders::decodeRFModulationTypeVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rFModulationTypeVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_rFModulationTypeVariantStructDecoder.getEncodedLength());
        _rFModulationTypeVariantStructDecoder.encodeInto(buffer);
        return DevStudio::RFModulationTypeVariantStruct(_rFModulationTypeVariantStructDecoder);
    }
    std::vector< std::string > Decoders::decodeRTIobjectIdArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rTIobjectIdArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_rTIobjectIdArrayDecoder.getEncodedLength());
        _rTIobjectIdArrayDecoder.encodeInto(buffer);
        return std::vector< std::string >(_rTIobjectIdArrayDecoder);
    }
    DevStudio::RadioInputSourceEnum::RadioInputSourceEnum Decoders::decodeRadioInputSourceEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _radioInputSourceEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_radioInputSourceEnumDecoder.getEncodedLength());
        _radioInputSourceEnumDecoder.encodeInto(buffer);
        return DevStudio::RadioInputSourceEnum::RadioInputSourceEnum(_radioInputSourceEnumDecoder);
    }
    DevStudio::RadioTypeStruct Decoders::decodeRadioTypeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _radioTypeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_radioTypeStructDecoder.getEncodedLength());
        _radioTypeStructDecoder.encodeInto(buffer);
        return DevStudio::RadioTypeStruct(_radioTypeStructDecoder);
    }
    DevStudio::ReceiverOperationalStatusEnum::ReceiverOperationalStatusEnum Decoders::decodeReceiverOperationalStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _receiverOperationalStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_receiverOperationalStatusEnumDecoder.getEncodedLength());
        _receiverOperationalStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::ReceiverOperationalStatusEnum::ReceiverOperationalStatusEnum(_receiverOperationalStatusEnumDecoder);
    }
    DevStudio::RecordSetStruct Decoders::decodeRecordSetStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _recordSetStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_recordSetStructDecoder.getEncodedLength());
        _recordSetStructDecoder.encodeInto(buffer);
        return DevStudio::RecordSetStruct(_recordSetStructDecoder);
    }
    std::vector< DevStudio::RecordSetStruct > Decoders::decodeRecordSetStructArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _recordSetStructArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_recordSetStructArray1PlusDecoder.getEncodedLength());
        _recordSetStructArray1PlusDecoder.encodeInto(buffer);
        return std::vector< DevStudio::RecordSetStruct >(_recordSetStructArray1PlusDecoder);
    }
    DevStudio::RecordStruct Decoders::decodeRecordStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _recordStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_recordStructDecoder.getEncodedLength());
        _recordStructDecoder.encodeInto(buffer);
        return DevStudio::RecordStruct(_recordStructDecoder);
    }
    std::vector< DevStudio::RecordStruct > Decoders::decodeRecordStructArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _recordStructArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_recordStructArrayDecoder.getEncodedLength());
        _recordStructArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::RecordStruct >(_recordStructArrayDecoder);
    }
    DevStudio::RectVol1GeomRecStruct Decoders::decodeRectVol1GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rectVol1GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_rectVol1GeomRecStructDecoder.getEncodedLength());
        _rectVol1GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::RectVol1GeomRecStruct(_rectVol1GeomRecStructDecoder);
    }
    DevStudio::RectVol2GeomRecStruct Decoders::decodeRectVol2GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rectVol2GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_rectVol2GeomRecStructDecoder.getEncodedLength());
        _rectVol2GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::RectVol2GeomRecStruct(_rectVol2GeomRecStructDecoder);
    }
    DevStudio::RectVol3GeomRecStruct Decoders::decodeRectVol3GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rectVol3GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_rectVol3GeomRecStructDecoder.getEncodedLength());
        _rectVol3GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::RectVol3GeomRecStruct(_rectVol3GeomRecStructDecoder);
    }
    DevStudio::ReferenceSystemEnum::ReferenceSystemEnum Decoders::decodeReferenceSystemEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _referenceSystemEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_referenceSystemEnumDecoder.getEncodedLength());
        _referenceSystemEnumDecoder.encodeInto(buffer);
        return DevStudio::ReferenceSystemEnum::ReferenceSystemEnum(_referenceSystemEnumDecoder);
    }
    DevStudio::RefuelingStationLightColorEnum::RefuelingStationLightColorEnum Decoders::decodeRefuelingStationLightColorEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _refuelingStationLightColorEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_refuelingStationLightColorEnumDecoder.getEncodedLength());
        _refuelingStationLightColorEnumDecoder.encodeInto(buffer);
        return DevStudio::RefuelingStationLightColorEnum::RefuelingStationLightColorEnum(_refuelingStationLightColorEnumDecoder);
    }
    DevStudio::RefuelingStationLightDayNightEnum::RefuelingStationLightDayNightEnum Decoders::decodeRefuelingStationLightDayNightEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _refuelingStationLightDayNightEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_refuelingStationLightDayNightEnumDecoder.getEncodedLength());
        _refuelingStationLightDayNightEnumDecoder.encodeInto(buffer);
        return DevStudio::RefuelingStationLightDayNightEnum::RefuelingStationLightDayNightEnum(_refuelingStationLightDayNightEnumDecoder);
    }
    DevStudio::Relation Decoders::decodeRelation(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _relationDecoder.decodeFrom(data, 0);
        buffer.reserve(_relationDecoder.getEncodedLength());
        _relationDecoder.encodeInto(buffer);
        return DevStudio::Relation(_relationDecoder);
    }
    DevStudio::RelativePositionStruct Decoders::decodeRelativePositionStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _relativePositionStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_relativePositionStructDecoder.getEncodedLength());
        _relativePositionStructDecoder.encodeInto(buffer);
        return DevStudio::RelativePositionStruct(_relativePositionStructDecoder);
    }
    DevStudio::RelativeRangeBearingStruct Decoders::decodeRelativeRangeBearingStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _relativeRangeBearingStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_relativeRangeBearingStructDecoder.getEncodedLength());
        _relativeRangeBearingStructDecoder.encodeInto(buffer);
        return DevStudio::RelativeRangeBearingStruct(_relativeRangeBearingStructDecoder);
    }
    DevStudio::ReleaseDistributionStruct Decoders::decodeReleaseDistributionStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _releaseDistributionStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_releaseDistributionStructDecoder.getEncodedLength());
        _releaseDistributionStructDecoder.encodeInto(buffer);
        return DevStudio::ReleaseDistributionStruct(_releaseDistributionStructDecoder);
    }
    DevStudio::ReleaseDynamicsStruct Decoders::decodeReleaseDynamicsStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _releaseDynamicsStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_releaseDynamicsStructDecoder.getEncodedLength());
        _releaseDynamicsStructDecoder.encodeInto(buffer);
        return DevStudio::ReleaseDynamicsStruct(_releaseDynamicsStructDecoder);
    }
    DevStudio::ReleaseSizeStruct Decoders::decodeReleaseSizeStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _releaseSizeStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_releaseSizeStructDecoder.getEncodedLength());
        _releaseSizeStructDecoder.encodeInto(buffer);
        return DevStudio::ReleaseSizeStruct(_releaseSizeStructDecoder);
    }
    DevStudio::RepairResultEnum::RepairResultEnum Decoders::decodeRepairResultEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _repairResultEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_repairResultEnumDecoder.getEncodedLength());
        _repairResultEnumDecoder.encodeInto(buffer);
        return DevStudio::RepairResultEnum::RepairResultEnum(_repairResultEnumDecoder);
    }
    DevStudio::RepairTaskStruct Decoders::decodeRepairTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _repairTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_repairTaskStructDecoder.getEncodedLength());
        _repairTaskStructDecoder.encodeInto(buffer);
        return DevStudio::RepairTaskStruct(_repairTaskStructDecoder);
    }
    DevStudio::RepairTypeEnum::RepairTypeEnum Decoders::decodeRepairTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _repairTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_repairTypeEnumDecoder.getEncodedLength());
        _repairTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::RepairTypeEnum::RepairTypeEnum(_repairTypeEnumDecoder);
    }
    DevStudio::RequestStatusEnum::RequestStatusEnum Decoders::decodeRequestStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _requestStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_requestStatusEnumDecoder.getEncodedLength());
        _requestStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::RequestStatusEnum::RequestStatusEnum(_requestStatusEnumDecoder);
    }
    DevStudio::RequestedConnection Decoders::decodeRequestedConnection(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _requestedConnectionDecoder.decodeFrom(data, 0);
        buffer.reserve(_requestedConnectionDecoder.getEncodedLength());
        _requestedConnectionDecoder.encodeInto(buffer);
        return DevStudio::RequestedConnection(_requestedConnectionDecoder);
    }
    std::vector< DevStudio::RequestedConnection > Decoders::decodeRequestedConnectionArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _requestedConnectionArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_requestedConnectionArrayDecoder.getEncodedLength());
        _requestedConnectionArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::RequestedConnection >(_requestedConnectionArrayDecoder);
    }
    DevStudio::ResourceStatusStruct Decoders::decodeResourceStatusStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _resourceStatusStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_resourceStatusStructDecoder.getEncodedLength());
        _resourceStatusStructDecoder.encodeInto(buffer);
        return DevStudio::ResourceStatusStruct(_resourceStatusStructDecoder);
    }
    DevStudio::ResponseFlagEnum::ResponseFlagEnum Decoders::decodeResponseFlagEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _responseFlagEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_responseFlagEnumDecoder.getEncodedLength());
        _responseFlagEnumDecoder.encodeInto(buffer);
        return DevStudio::ResponseFlagEnum::ResponseFlagEnum(_responseFlagEnumDecoder);
    }
    DevStudio::ResupplyTaskStruct Decoders::decodeResupplyTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _resupplyTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_resupplyTaskStructDecoder.getEncodedLength());
        _resupplyTaskStructDecoder.encodeInto(buffer);
        return DevStudio::ResupplyTaskStruct(_resupplyTaskStructDecoder);
    }
    DevStudio::RoadIceConditionEnum::RoadIceConditionEnum Decoders::decodeRoadIceConditionEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _roadIceConditionEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_roadIceConditionEnumDecoder.getEncodedLength());
        _roadIceConditionEnumDecoder.encodeInto(buffer);
        return DevStudio::RoadIceConditionEnum::RoadIceConditionEnum(_roadIceConditionEnumDecoder);
    }
    DevStudio::RoundStruct Decoders::decodeRoundStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _roundStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_roundStructDecoder.getEncodedLength());
        _roundStructDecoder.encodeInto(buffer);
        return DevStudio::RoundStruct(_roundStructDecoder);
    }
    DevStudio::RulesOfEngagementEnum::RulesOfEngagementEnum Decoders::decodeRulesOfEngagementEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _rulesOfEngagementEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_rulesOfEngagementEnumDecoder.getEncodedLength());
        _rulesOfEngagementEnumDecoder.encodeInto(buffer);
        return DevStudio::RulesOfEngagementEnum::RulesOfEngagementEnum(_rulesOfEngagementEnumDecoder);
    }
    DevStudio::SINCGARSModulationStruct Decoders::decodeSINCGARSModulationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sINCGARSModulationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sINCGARSModulationStructDecoder.getEncodedLength());
        _sINCGARSModulationStructDecoder.encodeInto(buffer);
        return DevStudio::SINCGARSModulationStruct(_sINCGARSModulationStructDecoder);
    }
    DevStudio::SeaStateEnum::SeaStateEnum Decoders::decodeSeaStateEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _seaStateEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_seaStateEnumDecoder.getEncodedLength());
        _seaStateEnumDecoder.encodeInto(buffer);
        return DevStudio::SeaStateEnum::SeaStateEnum(_seaStateEnumDecoder);
    }
    DevStudio::SedimentTypeEnum::SedimentTypeEnum Decoders::decodeSedimentTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sedimentTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_sedimentTypeEnumDecoder.getEncodedLength());
        _sedimentTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::SedimentTypeEnum::SedimentTypeEnum(_sedimentTypeEnumDecoder);
    }
    DevStudio::SendSafetyRelatedBroadcastMessageTaskStruct Decoders::decodeSendSafetyRelatedBroadcastMessageTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sendSafetyRelatedBroadcastMessageTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sendSafetyRelatedBroadcastMessageTaskStructDecoder.getEncodedLength());
        _sendSafetyRelatedBroadcastMessageTaskStructDecoder.encodeInto(buffer);
        return DevStudio::SendSafetyRelatedBroadcastMessageTaskStruct(_sendSafetyRelatedBroadcastMessageTaskStructDecoder);
    }
    DevStudio::SendSafetyRelatedMessageTaskStruct Decoders::decodeSendSafetyRelatedMessageTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sendSafetyRelatedMessageTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sendSafetyRelatedMessageTaskStructDecoder.getEncodedLength());
        _sendSafetyRelatedMessageTaskStructDecoder.encodeInto(buffer);
        return DevStudio::SendSafetyRelatedMessageTaskStruct(_sendSafetyRelatedMessageTaskStructDecoder);
    }
    DevStudio::SensorStateEnum::SensorStateEnum Decoders::decodeSensorStateEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sensorStateEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_sensorStateEnumDecoder.getEncodedLength());
        _sensorStateEnumDecoder.encodeInto(buffer);
        return DevStudio::SensorStateEnum::SensorStateEnum(_sensorStateEnumDecoder);
    }
    DevStudio::SensorStruct Decoders::decodeSensorStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sensorStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sensorStructDecoder.getEncodedLength());
        _sensorStructDecoder.encodeInto(buffer);
        return DevStudio::SensorStruct(_sensorStructDecoder);
    }
    DevStudio::ServiceTypeEnum::ServiceTypeEnum Decoders::decodeServiceTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _serviceTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_serviceTypeEnumDecoder.getEncodedLength());
        _serviceTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ServiceTypeEnum::ServiceTypeEnum(_serviceTypeEnumDecoder);
    }
    DevStudio::SetTransmitterStatusTaskStruct Decoders::decodeSetTransmitterStatusTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _setTransmitterStatusTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_setTransmitterStatusTaskStructDecoder.getEncodedLength());
        _setTransmitterStatusTaskStructDecoder.encodeInto(buffer);
        return DevStudio::SetTransmitterStatusTaskStruct(_setTransmitterStatusTaskStructDecoder);
    }
    DevStudio::ShaftDataStruct Decoders::decodeShaftDataStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _shaftDataStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_shaftDataStructDecoder.getEncodedLength());
        _shaftDataStructDecoder.encodeInto(buffer);
        return DevStudio::ShaftDataStruct(_shaftDataStructDecoder);
    }
    std::vector< DevStudio::ShaftDataStruct > Decoders::decodeShaftDataStructLengthlessArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _shaftDataStructLengthlessArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_shaftDataStructLengthlessArray1PlusDecoder.getEncodedLength());
        _shaftDataStructLengthlessArray1PlusDecoder.encodeInto(buffer);
        return std::vector< DevStudio::ShaftDataStruct >(_shaftDataStructLengthlessArray1PlusDecoder);
    }
    DevStudio::ShipTypeEnum::ShipTypeEnum Decoders::decodeShipTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _shipTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_shipTypeEnumDecoder.getEncodedLength());
        _shipTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::ShipTypeEnum::ShipTypeEnum(_shipTypeEnumDecoder);
    }
    std::vector< char > Decoders::decodeSignalDataLengthlessArray1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _signalDataLengthlessArray1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_signalDataLengthlessArray1PlusDecoder.getEncodedLength());
        _signalDataLengthlessArray1PlusDecoder.encodeInto(buffer);
        return std::vector< char >(_signalDataLengthlessArray1PlusDecoder);
    }
    DevStudio::SignatureTypeEnum::SignatureTypeEnum Decoders::decodeSignatureTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _signatureTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_signatureTypeEnumDecoder.getEncodedLength());
        _signatureTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::SignatureTypeEnum::SignatureTypeEnum(_signatureTypeEnumDecoder);
    }
    DevStudio::SignatureVariant Decoders::decodeSignatureVariant(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _signatureVariantDecoder.decodeFrom(data, 0);
        buffer.reserve(_signatureVariantDecoder.getEncodedLength());
        _signatureVariantDecoder.encodeInto(buffer);
        return DevStudio::SignatureVariant(_signatureVariantDecoder);
    }
    DevStudio::SilentAggregateStruct Decoders::decodeSilentAggregateStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _silentAggregateStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_silentAggregateStructDecoder.getEncodedLength());
        _silentAggregateStructDecoder.encodeInto(buffer);
        return DevStudio::SilentAggregateStruct(_silentAggregateStructDecoder);
    }
    std::vector< DevStudio::SilentAggregateStruct > Decoders::decodeSilentAggregateStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _silentAggregateStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_silentAggregateStructLengthlessArrayDecoder.getEncodedLength());
        _silentAggregateStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::SilentAggregateStruct >(_silentAggregateStructLengthlessArrayDecoder);
    }
    DevStudio::SilentEntityStruct Decoders::decodeSilentEntityStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _silentEntityStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_silentEntityStructDecoder.getEncodedLength());
        _silentEntityStructDecoder.encodeInto(buffer);
        return DevStudio::SilentEntityStruct(_silentEntityStructDecoder);
    }
    std::vector< DevStudio::SilentEntityStruct > Decoders::decodeSilentEntityStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _silentEntityStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_silentEntityStructLengthlessArrayDecoder.getEncodedLength());
        _silentEntityStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::SilentEntityStruct >(_silentEntityStructLengthlessArrayDecoder);
    }
    DevStudio::SnowStruct Decoders::decodeSnowStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _snowStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_snowStructDecoder.getEncodedLength());
        _snowStructDecoder.encodeInto(buffer);
        return DevStudio::SnowStruct(_snowStructDecoder);
    }
    DevStudio::SpatialFPStruct Decoders::decodeSpatialFPStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spatialFPStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spatialFPStructDecoder.getEncodedLength());
        _spatialFPStructDecoder.encodeInto(buffer);
        return DevStudio::SpatialFPStruct(_spatialFPStructDecoder);
    }
    DevStudio::SpatialFVStruct Decoders::decodeSpatialFVStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spatialFVStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spatialFVStructDecoder.getEncodedLength());
        _spatialFVStructDecoder.encodeInto(buffer);
        return DevStudio::SpatialFVStruct(_spatialFVStructDecoder);
    }
    DevStudio::SpatialRPStruct Decoders::decodeSpatialRPStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spatialRPStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spatialRPStructDecoder.getEncodedLength());
        _spatialRPStructDecoder.encodeInto(buffer);
        return DevStudio::SpatialRPStruct(_spatialRPStructDecoder);
    }
    DevStudio::SpatialRVStruct Decoders::decodeSpatialRVStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spatialRVStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spatialRVStructDecoder.getEncodedLength());
        _spatialRVStructDecoder.encodeInto(buffer);
        return DevStudio::SpatialRVStruct(_spatialRVStructDecoder);
    }
    DevStudio::SpatialStaticStruct Decoders::decodeSpatialStaticStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spatialStaticStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spatialStaticStructDecoder.getEncodedLength());
        _spatialStaticStructDecoder.encodeInto(buffer);
        return DevStudio::SpatialStaticStruct(_spatialStaticStructDecoder);
    }
    DevStudio::SpatialVariantStruct Decoders::decodeSpatialVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spatialVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spatialVariantStructDecoder.getEncodedLength());
        _spatialVariantStructDecoder.encodeInto(buffer);
        return DevStudio::SpatialVariantStruct(_spatialVariantStructDecoder);
    }
    DevStudio::Sphere1GeomRecStruct Decoders::decodeSphere1GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sphere1GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sphere1GeomRecStructDecoder.getEncodedLength());
        _sphere1GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Sphere1GeomRecStruct(_sphere1GeomRecStructDecoder);
    }
    DevStudio::Sphere2GeomRecStruct Decoders::decodeSphere2GeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sphere2GeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sphere2GeomRecStructDecoder.getEncodedLength());
        _sphere2GeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::Sphere2GeomRecStruct(_sphere2GeomRecStructDecoder);
    }
    DevStudio::SphericalHarmonicAntennaStruct Decoders::decodeSphericalHarmonicAntennaStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _sphericalHarmonicAntennaStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_sphericalHarmonicAntennaStructDecoder.getEncodedLength());
        _sphericalHarmonicAntennaStructDecoder.encodeInto(buffer);
        return DevStudio::SphericalHarmonicAntennaStruct(_sphericalHarmonicAntennaStructDecoder);
    }
    DevStudio::SpreadSpectrumEnum::SpreadSpectrumEnum Decoders::decodeSpreadSpectrumEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spreadSpectrumEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_spreadSpectrumEnumDecoder.getEncodedLength());
        _spreadSpectrumEnumDecoder.encodeInto(buffer);
        return DevStudio::SpreadSpectrumEnum::SpreadSpectrumEnum(_spreadSpectrumEnumDecoder);
    }
    DevStudio::SpreadSpectrumVariantStruct Decoders::decodeSpreadSpectrumVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _spreadSpectrumVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_spreadSpectrumVariantStructDecoder.getEncodedLength());
        _spreadSpectrumVariantStructDecoder.encodeInto(buffer);
        return DevStudio::SpreadSpectrumVariantStruct(_spreadSpectrumVariantStructDecoder);
    }
    DevStudio::StanceCodeEnum::StanceCodeEnum Decoders::decodeStanceCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _stanceCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_stanceCodeEnumDecoder.getEncodedLength());
        _stanceCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::StanceCodeEnum::StanceCodeEnum(_stanceCodeEnumDecoder);
    }
    DevStudio::StationEnum::StationEnum Decoders::decodeStationEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _stationEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_stationEnumDecoder.getEncodedLength());
        _stationEnumDecoder.encodeInto(buffer);
        return DevStudio::StationEnum::StationEnum(_stationEnumDecoder);
    }
    DevStudio::StationNameLocationVariantStruct Decoders::decodeStationNameLocationVariantStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _stationNameLocationVariantStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_stationNameLocationVariantStructDecoder.getEncodedLength());
        _stationNameLocationVariantStructDecoder.encodeInto(buffer);
        return DevStudio::StationNameLocationVariantStruct(_stationNameLocationVariantStructDecoder);
    }
    DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum Decoders::decodeStopFreezeReasonEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _stopFreezeReasonEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_stopFreezeReasonEnumDecoder.getEncodedLength());
        _stopFreezeReasonEnumDecoder.encodeInto(buffer);
        return DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum(_stopFreezeReasonEnumDecoder);
    }
    DevStudio::SubsurfaceFormationTypeEnum::SubsurfaceFormationTypeEnum Decoders::decodeSubsurfaceFormationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _subsurfaceFormationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_subsurfaceFormationTypeEnumDecoder.getEncodedLength());
        _subsurfaceFormationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::SubsurfaceFormationTypeEnum::SubsurfaceFormationTypeEnum(_subsurfaceFormationTypeEnumDecoder);
    }
    DevStudio::SupplyStatusStruct Decoders::decodeSupplyStatusStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _supplyStatusStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_supplyStatusStructDecoder.getEncodedLength());
        _supplyStatusStructDecoder.encodeInto(buffer);
        return DevStudio::SupplyStatusStruct(_supplyStatusStructDecoder);
    }
    DevStudio::SupplyStruct Decoders::decodeSupplyStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _supplyStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_supplyStructDecoder.getEncodedLength());
        _supplyStructDecoder.encodeInto(buffer);
        return DevStudio::SupplyStruct(_supplyStructDecoder);
    }
    std::vector< DevStudio::SupplyStruct > Decoders::decodeSupplyStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _supplyStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_supplyStructLengthlessArrayDecoder.getEncodedLength());
        _supplyStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::SupplyStruct >(_supplyStructLengthlessArrayDecoder);
    }
    DevStudio::SurfaceFormationTypeEnum::SurfaceFormationTypeEnum Decoders::decodeSurfaceFormationTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _surfaceFormationTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_surfaceFormationTypeEnumDecoder.getEncodedLength());
        _surfaceFormationTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::SurfaceFormationTypeEnum::SurfaceFormationTypeEnum(_surfaceFormationTypeEnumDecoder);
    }
    DevStudio::SurfaceMoistureEnum::SurfaceMoistureEnum Decoders::decodeSurfaceMoistureEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _surfaceMoistureEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_surfaceMoistureEnumDecoder.getEncodedLength());
        _surfaceMoistureEnumDecoder.encodeInto(buffer);
        return DevStudio::SurfaceMoistureEnum::SurfaceMoistureEnum(_surfaceMoistureEnumDecoder);
    }
    DevStudio::SymbolAmplificationVariant Decoders::decodeSymbolAmplificationVariant(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _symbolAmplificationVariantDecoder.decodeFrom(data, 0);
        buffer.reserve(_symbolAmplificationVariantDecoder.getEncodedLength());
        _symbolAmplificationVariantDecoder.encodeInto(buffer);
        return DevStudio::SymbolAmplificationVariant(_symbolAmplificationVariantDecoder);
    }
    DevStudio::SymbolStruct Decoders::decodeSymbolStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _symbolStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_symbolStructDecoder.getEncodedLength());
        _symbolStructDecoder.encodeInto(buffer);
        return DevStudio::SymbolStruct(_symbolStructDecoder);
    }
    DevStudio::SymbolTypeEnum::SymbolTypeEnum Decoders::decodeSymbolTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _symbolTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_symbolTypeEnumDecoder.getEncodedLength());
        _symbolTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::SymbolTypeEnum::SymbolTypeEnum(_symbolTypeEnumDecoder);
    }
    DevStudio::TacticalDataLinkTypeEnum::TacticalDataLinkTypeEnum Decoders::decodeTacticalDataLinkTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _tacticalDataLinkTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_tacticalDataLinkTypeEnumDecoder.getEncodedLength());
        _tacticalDataLinkTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::TacticalDataLinkTypeEnum::TacticalDataLinkTypeEnum(_tacticalDataLinkTypeEnumDecoder);
    }
    DevStudio::TaskDefinition Decoders::decodeTaskDefinition(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _taskDefinitionDecoder.decodeFrom(data, 0);
        buffer.reserve(_taskDefinitionDecoder.getEncodedLength());
        _taskDefinitionDecoder.encodeInto(buffer);
        return DevStudio::TaskDefinition(_taskDefinitionDecoder);
    }
    DevStudio::TaskDefinitionVariantRecord Decoders::decodeTaskDefinitionVariantRecord(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _taskDefinitionVariantRecordDecoder.decodeFrom(data, 0);
        buffer.reserve(_taskDefinitionVariantRecordDecoder.getEncodedLength());
        _taskDefinitionVariantRecordDecoder.encodeInto(buffer);
        return DevStudio::TaskDefinitionVariantRecord(_taskDefinitionVariantRecordDecoder);
    }
    DevStudio::TaskModeEnum::TaskModeEnum Decoders::decodeTaskModeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _taskModeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_taskModeEnumDecoder.getEncodedLength());
        _taskModeEnumDecoder.encodeInto(buffer);
        return DevStudio::TaskModeEnum::TaskModeEnum(_taskModeEnumDecoder);
    }
    DevStudio::TaskProgress Decoders::decodeTaskProgress(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _taskProgressDecoder.decodeFrom(data, 0);
        buffer.reserve(_taskProgressDecoder.getEncodedLength());
        _taskProgressDecoder.encodeInto(buffer);
        return DevStudio::TaskProgress(_taskProgressDecoder);
    }
    DevStudio::TaskProgressVariantRecord Decoders::decodeTaskProgressVariantRecord(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _taskProgressVariantRecordDecoder.decodeFrom(data, 0);
        buffer.reserve(_taskProgressVariantRecordDecoder.getEncodedLength());
        _taskProgressVariantRecordDecoder.encodeInto(buffer);
        return DevStudio::TaskProgressVariantRecord(_taskProgressVariantRecordDecoder);
    }
    DevStudio::TaskStatusEnum::TaskStatusEnum Decoders::decodeTaskStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _taskStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_taskStatusEnumDecoder.getEncodedLength());
        _taskStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::TaskStatusEnum::TaskStatusEnum(_taskStatusEnumDecoder);
    }
    std::vector< float > Decoders::decodeTemperatureDegreeCelsiusFloat32LengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _temperatureDegreeCelsiusFloat32LengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_temperatureDegreeCelsiusFloat32LengthlessArrayDecoder.getEncodedLength());
        _temperatureDegreeCelsiusFloat32LengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< float >(_temperatureDegreeCelsiusFloat32LengthlessArrayDecoder);
    }
    DevStudio::TrackStruct Decoders::decodeTrackStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _trackStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_trackStructDecoder.getEncodedLength());
        _trackStructDecoder.encodeInto(buffer);
        return DevStudio::TrackStruct(_trackStructDecoder);
    }
    DevStudio::TrailingEffectsCodeEnum::TrailingEffectsCodeEnum Decoders::decodeTrailingEffectsCodeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _trailingEffectsCodeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_trailingEffectsCodeEnumDecoder.getEncodedLength());
        _trailingEffectsCodeEnumDecoder.encodeInto(buffer);
        return DevStudio::TrailingEffectsCodeEnum::TrailingEffectsCodeEnum(_trailingEffectsCodeEnumDecoder);
    }
    DevStudio::TransferTypeEnum::TransferTypeEnum Decoders::decodeTransferTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _transferTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_transferTypeEnumDecoder.getEncodedLength());
        _transferTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::TransferTypeEnum::TransferTypeEnum(_transferTypeEnumDecoder);
    }
    DevStudio::TransmitterOperationalStatusEnum::TransmitterOperationalStatusEnum Decoders::decodeTransmitterOperationalStatusEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _transmitterOperationalStatusEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_transmitterOperationalStatusEnumDecoder.getEncodedLength());
        _transmitterOperationalStatusEnumDecoder.encodeInto(buffer);
        return DevStudio::TransmitterOperationalStatusEnum::TransmitterOperationalStatusEnum(_transmitterOperationalStatusEnumDecoder);
    }
    DevStudio::TransportTaskStruct Decoders::decodeTransportTaskStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _transportTaskStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_transportTaskStructDecoder.getEncodedLength());
        _transportTaskStructDecoder.encodeInto(buffer);
        return DevStudio::TransportTaskStruct(_transportTaskStructDecoder);
    }
    DevStudio::TreatmentStruct Decoders::decodeTreatmentStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _treatmentStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_treatmentStructDecoder.getEncodedLength());
        _treatmentStructDecoder.encodeInto(buffer);
        return DevStudio::TreatmentStruct(_treatmentStructDecoder);
    }
    std::vector< char > Decoders::decodeUUID(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _uUIDDecoder.decodeFrom(data, 0);
        buffer.reserve(_uUIDDecoder.getEncodedLength());
        _uUIDDecoder.encodeInto(buffer);
        return std::vector< char >(_uUIDDecoder);
    }
    DevStudio::UniformGeomRecStruct Decoders::decodeUniformGeomRecStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _uniformGeomRecStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_uniformGeomRecStructDecoder.getEncodedLength());
        _uniformGeomRecStructDecoder.encodeInto(buffer);
        return DevStudio::UniformGeomRecStruct(_uniformGeomRecStructDecoder);
    }
    DevStudio::UnitSymbolAmplificationStruct Decoders::decodeUnitSymbolAmplificationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _unitSymbolAmplificationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_unitSymbolAmplificationStructDecoder.getEncodedLength());
        _unitSymbolAmplificationStructDecoder.encodeInto(buffer);
        return DevStudio::UnitSymbolAmplificationStruct(_unitSymbolAmplificationStructDecoder);
    }
    DevStudio::UnmodulatedTypeEnum::UnmodulatedTypeEnum Decoders::decodeUnmodulatedTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _unmodulatedTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_unmodulatedTypeEnumDecoder.getEncodedLength());
        _unmodulatedTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::UnmodulatedTypeEnum::UnmodulatedTypeEnum(_unmodulatedTypeEnumDecoder);
    }
    std::vector< unsigned short > Decoders::decodeUnsignedInteger16Array1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _unsignedInteger16Array1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_unsignedInteger16Array1PlusDecoder.getEncodedLength());
        _unsignedInteger16Array1PlusDecoder.encodeInto(buffer);
        return std::vector< unsigned short >(_unsignedInteger16Array1PlusDecoder);
    }
    std::vector< unsigned int > Decoders::decodeUnsignedInteger32LengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _unsignedInteger32LengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_unsignedInteger32LengthlessArrayDecoder.getEncodedLength());
        _unsignedInteger32LengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< unsigned int >(_unsignedInteger32LengthlessArrayDecoder);
    }
    std::vector< unsigned long long > Decoders::decodeUnsignedInteger64Array1Plus(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _unsignedInteger64Array1PlusDecoder.decodeFrom(data, 0);
        buffer.reserve(_unsignedInteger64Array1PlusDecoder.getEncodedLength());
        _unsignedInteger64Array1PlusDecoder.encodeInto(buffer);
        return std::vector< unsigned long long >(_unsignedInteger64Array1PlusDecoder);
    }
    std::vector< char > Decoders::decodeUnsignedInteger8LengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _unsignedInteger8LengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_unsignedInteger8LengthlessArrayDecoder.getEncodedLength());
        _unsignedInteger8LengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< char >(_unsignedInteger8LengthlessArrayDecoder);
    }
    DevStudio::UserProtocolEnum::UserProtocolEnum Decoders::decodeUserProtocolEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _userProtocolEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_userProtocolEnumDecoder.getEncodedLength());
        _userProtocolEnumDecoder.encodeInto(buffer);
        return DevStudio::UserProtocolEnum::UserProtocolEnum(_userProtocolEnumDecoder);
    }
    DevStudio::VariableDatumStruct Decoders::decodeVariableDatumStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _variableDatumStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_variableDatumStructDecoder.getEncodedLength());
        _variableDatumStructDecoder.encodeInto(buffer);
        return DevStudio::VariableDatumStruct(_variableDatumStructDecoder);
    }
    std::vector< DevStudio::VariableDatumStruct > Decoders::decodeVariableDatumStructArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _variableDatumStructArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_variableDatumStructArrayDecoder.getEncodedLength());
        _variableDatumStructArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::VariableDatumStruct >(_variableDatumStructArrayDecoder);
    }
    std::vector< DevStudio::VariableDatumStruct > Decoders::decodeVariableDatumStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _variableDatumStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_variableDatumStructLengthlessArrayDecoder.getEncodedLength());
        _variableDatumStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::VariableDatumStruct >(_variableDatumStructLengthlessArrayDecoder);
    }
    DevStudio::VectoringNozzleSystemDataStruct Decoders::decodeVectoringNozzleSystemDataStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _vectoringNozzleSystemDataStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_vectoringNozzleSystemDataStructDecoder.getEncodedLength());
        _vectoringNozzleSystemDataStructDecoder.encodeInto(buffer);
        return DevStudio::VectoringNozzleSystemDataStruct(_vectoringNozzleSystemDataStructDecoder);
    }
    std::vector< DevStudio::VectoringNozzleSystemDataStruct > Decoders::decodeVectoringNozzleSystemDataStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _vectoringNozzleSystemDataStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_vectoringNozzleSystemDataStructLengthlessArrayDecoder.getEncodedLength());
        _vectoringNozzleSystemDataStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::VectoringNozzleSystemDataStruct >(_vectoringNozzleSystemDataStructLengthlessArrayDecoder);
    }
    DevStudio::VelocityVectorStruct Decoders::decodeVelocityVectorStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _velocityVectorStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_velocityVectorStructDecoder.getEncodedLength());
        _velocityVectorStructDecoder.encodeInto(buffer);
        return DevStudio::VelocityVectorStruct(_velocityVectorStructDecoder);
    }
    DevStudio::VisibleSideLocationEnum::VisibleSideLocationEnum Decoders::decodeVisibleSideLocationEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _visibleSideLocationEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_visibleSideLocationEnumDecoder.getEncodedLength());
        _visibleSideLocationEnumDecoder.encodeInto(buffer);
        return DevStudio::VisibleSideLocationEnum::VisibleSideLocationEnum(_visibleSideLocationEnumDecoder);
    }
    DevStudio::WarheadTypeEnum::WarheadTypeEnum Decoders::decodeWarheadTypeEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _warheadTypeEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_warheadTypeEnumDecoder.getEncodedLength());
        _warheadTypeEnumDecoder.encodeInto(buffer);
        return DevStudio::WarheadTypeEnum::WarheadTypeEnum(_warheadTypeEnumDecoder);
    }
    DevStudio::WaveStruct Decoders::decodeWaveStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _waveStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_waveStructDecoder.getEncodedLength());
        _waveStructDecoder.encodeInto(buffer);
        return DevStudio::WaveStruct(_waveStructDecoder);
    }
    DevStudio::Waypoint Decoders::decodeWaypoint(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _waypointDecoder.decodeFrom(data, 0);
        buffer.reserve(_waypointDecoder.getEncodedLength());
        _waypointDecoder.encodeInto(buffer);
        return DevStudio::Waypoint(_waypointDecoder);
    }
    DevStudio::WeaponControlOrderEnum::WeaponControlOrderEnum Decoders::decodeWeaponControlOrderEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _weaponControlOrderEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_weaponControlOrderEnumDecoder.getEncodedLength());
        _weaponControlOrderEnumDecoder.encodeInto(buffer);
        return DevStudio::WeaponControlOrderEnum::WeaponControlOrderEnum(_weaponControlOrderEnumDecoder);
    }
    DevStudio::WeaponStateEnum::WeaponStateEnum Decoders::decodeWeaponStateEnum(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _weaponStateEnumDecoder.decodeFrom(data, 0);
        buffer.reserve(_weaponStateEnumDecoder.getEncodedLength());
        _weaponStateEnumDecoder.encodeInto(buffer);
        return DevStudio::WeaponStateEnum::WeaponStateEnum(_weaponStateEnumDecoder);
    }
    DevStudio::WindStruct Decoders::decodeWindStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _windStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_windStructDecoder.getEncodedLength());
        _windStructDecoder.encodeInto(buffer);
        return DevStudio::WindStruct(_windStructDecoder);
    }
    DevStudio::WorldLocationStruct Decoders::decodeWorldLocationStruct(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _worldLocationStructDecoder.decodeFrom(data, 0);
        buffer.reserve(_worldLocationStructDecoder.getEncodedLength());
        _worldLocationStructDecoder.encodeInto(buffer);
        return DevStudio::WorldLocationStruct(_worldLocationStructDecoder);
    }
    std::vector< DevStudio::WorldLocationStruct > Decoders::decodeWorldLocationStructLengthlessArray(const std::vector<char>& data)
    {
        std::vector<char> buffer;
        std::unique_lock<std::mutex> lock(_decodersLock);
        _worldLocationStructLengthlessArrayDecoder.decodeFrom(data, 0);
        buffer.reserve(_worldLocationStructLengthlessArrayDecoder.getEncodedLength());
        _worldLocationStructLengthlessArrayDecoder.encodeInto(buffer);
        return std::vector< DevStudio::WorldLocationStruct >(_worldLocationStructLengthlessArrayDecoder);
    }
// === AUTO-GENERATED FUNCTIONS END ===
//pragma endregion

	bool Decoders::decodebool(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_boolDecoder.decodeFrom(data, 0);
		return  _boolDecoder.get();
	}
	char Decoders::decodechar(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_charDecoder.decodeFrom(data, 0);
		return _charDecoder.get();
	}
	short Decoders::decodeshort(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_shortDecoder.decodeFrom(data, 0);
		return _shortDecoder.get();
	}
	unsigned short Decoders::decodeunsignedshort(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_unsignedshortDecoder.decodeFrom(data, 0);
		return _unsignedshortDecoder.get();
	}
	int Decoders::decodeint(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_intDecoder.decodeFrom(data, 0);
		return _intDecoder.get();
	}
	unsigned int Decoders::decodeunsignedint(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_unsignedintDecoder.decodeFrom(data, 0);
		return _unsignedintDecoder.get();
	}
	long Decoders::decodelong(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_longDecoder.decodeFrom(data, 0);
		return static_cast<long>(_longDecoder.get());
	}
	unsigned long Decoders::decodeunsignedlong(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_unsignedlongDecoder.decodeFrom(data, 0);
		return static_cast<unsigned long>(_unsignedlongDecoder.get());
	}
	long long Decoders::decodelonglong(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_longlongDecoder.decodeFrom(data, 0);
		return _longlongDecoder.get();
	}
	unsigned long long Decoders::decodeunsignedlonglong(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_unsignedlonglongDecoder.decodeFrom(data, 0);
		return _unsignedlonglongDecoder.get();
	}
	float Decoders::decodefloat(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_floatDecoder.decodeFrom(data, 0);
		return _floatDecoder.get();
	}
	double Decoders::decodedouble(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_doubleDecoder.decodeFrom(data, 0);
		return _doubleDecoder.get();
	}
	std::string Decoders::decodestring(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_stringDecoder.decodeFrom(data, 0);
		return _stringDecoder.get();
	}
	std::wstring Decoders::decodewstring(const std::vector<char>& data)
	{
		std::unique_lock<std::mutex> lock(_decodersLock);
		_wstringDecoder.decodeFrom(data, 0);
		return _wstringDecoder.get();
	}

}


