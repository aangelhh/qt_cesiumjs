#pragma once
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <exception>
#include <codecvt>
#include <map>
#include <memory>
#include <cmath>

#define NOMINMAX
#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#elif defined(__linux__)
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

#include "include/Websocketserver.h"

#include <DevStudio/HlaWorldListener.h>
#include <DevStudio/HlaAircraftManagerListener.h>
#include <DevStudio/HlaPlatformManagerListener.h>
#include <RtiDriver/RprUtility/RprUtility.h>
#include <RtiDriver/RtiAmbassador.h>

#include <DevStudio/FederateManager.h>
#include <DevStudio/HlaInteractionListener.h>
#include <DevStudio/HlaSettings.h>

#include <DevStudio/HlaBaseEntityAttributes.h>
#include <DevStudio/HlaBaseEntityAttributesImpl.h>
#include <DevStudio/HlaHLAobjectRootAttributesImpl.h>
#include <DevStudio/HlaPhysicalEntityAttributesImpl.h>
#include <DevStudio/HlaPlatformAttributesImpl.h>
#include <DevStudio/HlaAircraftAttributesImpl.h>
#include <DevStudio/HlaEmitterSystemAttributesImpl.h>
#include <DevStudio/HlaRadarBeamAttributesImpl.h>
#include <DevStudio/HlaJammerBeamAttributesImpl.h>

#include <DevStudio/HlaPointers.h>
#include "include/SimlabSettings.h"
#include "include/events/EventListener.h"
#include "include/events/EventOnDataAvailableInteraction.h"
#include "include/events/EventOnDataAvailableObject.h"

#include "include/ObjectManagerListeners.h"
#include "include/enumsCommon/EnumsCommons.h"

#include "include/SDLDevStudio/TaskParams.h"

#include "include/InteractionManagerListener.h"
#include "include/DataTypes/EmitterBeamConverter.h"
#include "include/DataTypes/Converters.h"
#include "include/ObjectManager.h"
#include "ProtoGeneration.h"

#include "../protobuf/c++/entity.pb.h"

#include "include/DataTypes/ObjectIdentifier.h"
#include "include/DataTypes/AttributeValuePair.h"
#include "include/PitchListener.h"

#include <memory>


namespace SDL_RTI
{
	class ClientPitch: public events::EventListener<EventOnDataAvailableInteraction>
		, public events::EventListener<EventOnDataAvailableObject>
	{
	public:
		ClientPitch();
		~ClientPitch();

		//TODO: void initializeManager();
		void initializeManagerListener();

		void addBaseEntityObjectListener();
		void addEmbeddedSystemObjectListener();
		void addEnvironmentObjectListener();
		void addEmitterBeamObjectListener();
		void addOtherObjectListener();
		void addAllObjectListeners(void);
		void addAllInteractionListeners(void);
		DevStudio::HlaWorldPtr getWorld(void);

		void sendStartResumeInteractionHLA(
			int originatingEntityId,
			int originatingApplicationId,
			int originatingSiteId,
			int receivingEntityId,
			int receivingApplicationId,
			int receivingSiteId,
			double realWorldTime,
			double simulationTime,
			unsigned int requestIdentifier);


		void initConnection(void);
		void initSetting(const std::string& federationName, const std::string& federateName, const std::string& federateType, int siteId, int applicationId, const std::string& ip, int port);
		bool initFederation(const std::string& federationName, const std::string& federateName, const std::string& federateType, int siteId, int applicationId, const std::string& ip, int port);
		bool createWorld(std::shared_ptr<SimlabSettings> setting);

		void runTest();
			
		void startSimulation(void);
		bool getConnectionHlaStatus(void);
		void setPitchListener(void* listenerInstance);

		void finishedFederation(void);
		void notifyFinishedFederation(const std::string& description);

		void connectCallbackInteraction(void);
		void disconnectCallbackInteraction(void);

		void connectCallbackObject(void);
		void disconnectCallbackObject(void);

		//void checkDisconnectedFederates();
		
		//********************************************
		// SEND 
		void sendStartResumeInteraction(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::ClockTimeStruct& simulationTime);
		void sendStartResumeInteraction(const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::ClockTimeStruct& simulationTime);
		void sendStartResumeAnswerInteraction(const AcknowledgementProtocolEnum::AcknowledgementProtocolEnum& acknowledgementProtocol, const ClockTimeStruct& realWorldTime, const unsigned int& requestIdentifier, const ClockTimeStruct& simulationTime);
		void sendStopFreezeInteraction(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& reason, const bool& reflectValues, const bool& runInternalSimulationClock, const bool& updateAttributes);
		void sendStopFreezeInteraction(const DevStudio::EntityIdentifierStruct& receivingEntity, const DevStudio::ClockTimeStruct& realWorldTime, const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& reason, const bool& reflectValues, const bool& runInternalSimulationClock, const bool& updateAttributes);
		void sendStopFreezeAnswerInteraction(const AcknowledgementProtocolEnum::AcknowledgementProtocolEnum& acknowledgementProtocol, const unsigned int& requestIdentifier, const ClockTimeStruct& realWorldTime, const StopFreezeReasonEnum::StopFreezeReasonEnum& reason, const bool& reflectValues, const bool& runInternalSimulationClock, const bool& updateAttributes);
		void sendCreateObjectRequest(unsigned int classObject, std::vector<DataTypes::AttributeValuePair> &attributes);
		void sendRemoveObjectsRequest(const std::vector<DataTypes::ObjectIdentifier>& objectIdentifiers);
		void sendAttributeChangeRequest(const std::vector<DataTypes::ObjectIdentifier>& objectIdentifiers, const std::vector<DataTypes::AttributeValuePair>& attributeValueSet);
		void sendChangeSpeedMagicActionChangeSpeedObjectRequest(const std::vector<std::string >& objectIdentifiers, const std::vector<DevStudio::AttributeValuePairStruct >& attributeValueSet);
		void sendWeaponFire(const DevStudio::EventIdentifierStruct& eventIdentifier, const float& fireControlSolutionRange, const unsigned int& fireMissionIndex, const DevStudio::WorldLocationStruct& firingLocation, DataTypes::ObjectIdentifier& firingObjectIdentifier, const DevStudio::FuseTypeEnum::FuseTypeEnum& fuseType, const DevStudio::VelocityVectorStruct& initialVelocityVector, DataTypes::ObjectIdentifier& munitionObjectIdentifier, const DevStudio::EntityTypeStruct& munitionType, const unsigned short& quantityFired, const unsigned short& rateOfFire, DataTypes::ObjectIdentifier& targetObjectIdentifier, const DevStudio::WarheadTypeEnum::WarheadTypeEnum& warheadType);
		void sendMunitionDetonation(const std::vector<DevStudio::ArticulatedParameterStruct>& articulatedPartData, const DevStudio::WorldLocationStruct& detonationLocation, const DevStudio::DetonationResultCodeEnum::DetonationResultCodeEnum& detonationResultCode, const DevStudio::EventIdentifierStruct& eventIdentifier, DataTypes::ObjectIdentifier& firingObjectIdentifier, const DevStudio::VelocityVectorStruct& finalVelocityVector, const DevStudio::FuseTypeEnum::FuseTypeEnum& fuseType, DataTypes::ObjectIdentifier& munitionObjectIdentifier, const DevStudio::EntityTypeStruct& munitionType, const unsigned short& quantityFired, const unsigned short& rateOfFire, const DevStudio::RelativePositionStruct& relativeDetonationLocation, DataTypes::ObjectIdentifier& targetObjectIdentifier, const DevStudio::WarheadTypeEnum::WarheadTypeEnum& warheadType);
		
		// Tasks
		void sendMagicMove(TaskParams& task, const DevStudio::MagicMoveTaskStruct& magicMoveParams);
		
		//*********************************************

		//********************************************
		// RECEIVE
		void onEventReceived(const EventOnDataAvailableInteraction& event);
		void onEventReceived(const EventOnDataAvailableObject& event);

		void receivedStartResumeInteraction(DevStudio::HlaStartResumeParametersImplPtr* val);
		void receivedStartResumeAnswerInteraction(DevStudio::HlaStartResumeRParametersImplPtr* val);
		void receivedStopFreezeInteraction(DevStudio::HlaStopFreezeParametersImplPtr* val);
		void receivedStopFreezeAnswerInteraction(DevStudio::HlaStopFreezeRParametersImplPtr* val);
		void receivedAcknowledgeInteraction(DevStudio::HlaAcknowledgeParametersImplPtr* val);
		void receivedCollisionInteraction(DevStudio::HlaCollisionParametersImplPtr* val);
		void receivedMunitionDetonationInteraction(DevStudio::HlaMunitionDetonationParametersImplPtr* munition);
		void receivedResupplyCancelInteraction(DevStudio::HlaResupplyCancelParametersImplPtr* resupplyCancel);
		void receivedResupplyOfferInteraction(DevStudio::HlaResupplyOfferParametersImplPtr* resupplyOffer);
		void receivedResupplyReceivedInteraction(DevStudio::HlaResupplyReceivedParametersImplPtr* resupplyReceived);
		void receivedWeaponFireInteraction(DevStudio::HlaWeaponFireParametersImplPtr* weaponFire);
		void receivedCreateInteraction(DevStudio::HlaCreateEntityParametersImplPtr* create);
		void receivedRemoveInteraction(DevStudio::HlaRemoveEntityParametersImplPtr* remove);
		void receivedCreateObjectRequestInteraction(DevStudio::HlaCreateObjectRequestParametersImplPtr* val);
		void receivedRemoveObjectRequestInteraction(DevStudio::HlaRemoveObjectRequestParametersImplPtr* val);
		void receivedAttributeChangeRequestInteraction(DevStudio::HlaAttributeChangeRequestParametersImplPtr* val);

		void handleMagicAction(const unsigned int& objectClass, const std::vector<DevStudio::AttributeValuePairStruct >& attributeValueSet, const unsigned int& requestIdentifier);

		void handlePlayResume(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, 
			const DevStudio::ClockTimeStruct& realWorldTimeStruct, const DevStudio::ClockTimeStruct& simulationTimeStruct, const int& requestID);

		void handlePauseFreeze(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, 
			const DevStudio::ClockTimeStruct& realWorldTimeStruct, const DevStudio::ClockTimeStruct& simulationTimeStruct, const int& requestID, 
			const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& devStudioReason, bool reflectValues, bool runInternalSimulationClock, bool updateAttributes);

		void handleStop(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, 
			const DevStudio::ClockTimeStruct& realWorldTimeStruct, const DevStudio::ClockTimeStruct& simulationTimeStruct, const int& requestID,
			const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& devStudioReason, bool reflectValues, bool runInternalSimulationClock, bool updateAttributes);
			
		template <typename T>
		void addAttributeToVector(std::vector<DevStudio::AttributeValuePairStruct>& attributeVector, unsigned int attributeHandle, T attributeValue)
		{
			DevStudio::AttributeValuePairStruct attribute;
			attribute.attributeHandle = attributeHandle;

			std::vector<char> encodedValue(sizeof(T));
			std::memcpy(encodedValue.data(), &attributeValue, sizeof(T));

			size_t remainder = encodedValue.size() % 4;
			if (remainder != 0) {
				size_t paddingSize = 4 - remainder;
				encodedValue.insert(encodedValue.end(), paddingSize, 0);
			}

			attribute.numberOfBytesAValue = encodedValue;
			attributeVector.push_back(attribute);
		}
		void serializeMarkingStruct(std::vector<DevStudio::AttributeValuePairStruct>& attributeVector, unsigned int attributeHandle, const DevStudio::MarkingStruct& marking) 
		{
			DevStudio::AttributeValuePairStruct attribute;
			attribute.attributeHandle = attributeHandle;
			
			std::vector<char> buffer;

			buffer.push_back(static_cast<char>(marking.markingEncodingType));

			for (char c : marking.markingData) {
				buffer.push_back(c);
			}

			attribute.numberOfBytesAValue = buffer;
			attributeVector.push_back(attribute);
		}
		void setFederationManagerWrapper(void* wrapperInstance);
		void getRemoteAircrafts(void);
		void getRemoteRadars(void);
		void getRemoteJammers(void);
		void getRemoteEmitterSystems(void);

		std::shared_ptr<SDL_RTI::ObjectManager> getObjectManager();

		void evokeAll();
		void evokeAll(unsigned long long millis);

	protected:
		DevStudio::HlaWorldPtr m_World;
		std::shared_ptr<SimlabSettings> m_SettingsPtr;
		DevStudio::HlaTimeStampPtr m_time;
		//SDL_RTI::FederateManagerSDL* m_FederateManagerSDL;
		bool m_Status;
		int m_IdMagicAction;
		
		std::map<std::string, int> m_MapObjects;
		SDL_RTI::EmitterBeamConverter* m_EmitterConverter;

		//std::map<std::wstring, SDL_RTI::FederateManagerSDL> m_federates;
		//std::thread m_federateThread;
		
		std::shared_ptr<SDL_RTI::ObjectManager> m_ObjectManager;
		std::shared_ptr<PitchListener>* m_PitchListener;

		std::shared_ptr<SDLPhysicalEntityManagerListener> m_PhysicalEntityListener;
		std::shared_ptr<SDLBaseEntityManagerListener> m_BaseEntityListener;
		std::shared_ptr<SDLPlatformManagerListener> m_PlatformEntityListener;
		std::shared_ptr<SDLAircraftManagerListener> m_AircraftEntityListener;
		std::shared_ptr<SDLAmphibiousVehicleManagerListener> m_AmphibiousVehicleListener;
		std::shared_ptr<SDLGroundVehicleManagerListener> m_GroundVehicleListener;
		std::shared_ptr<SDLMultiDomainPlatformManagerListener> m_MultiDomainPlatformListener;
		std::shared_ptr<SDLSpacecraftManagerListener> m_SpacecraftListener;
		std::shared_ptr<SDLSubmersibleVesselManagerListener> m_SubmersibleVesselListener;
		std::shared_ptr<SDLSurfaceVesselManagerListener> m_SurfaceVesselListener;
		std::shared_ptr<SDLLifeformManagerListener> m_LifeformListener;
		std::shared_ptr<SDLHumanManagerListener> m_HumanListener;
		std::shared_ptr<SDLNonHumanManagerListener> m_NonHumanListener;
		std::shared_ptr<SDLCulturalFeatureManagerListener> m_CulturalFeatureListener;
		std::shared_ptr<SDLMunitionManagerListener> m_MunitionListener;
		std::shared_ptr<SDLExpendablesManagerListener> m_ExpendablesListener;
		std::shared_ptr<SDLRadioManagerListener> m_RadioListener;
		std::shared_ptr<SDLSensorManagerListener> m_SensorListener;
		std::shared_ptr<SDLSuppliesManagerListener> m_SuppliesListener;
		std::shared_ptr<SDLEmbeddedSystemManagerListener> m_EmbeddedSystemListener;
		std::shared_ptr<SDLRadioTransmitterManagerListener> m_RadioTransmitterListener;
		std::shared_ptr<SDLRadioReceiverManagerListener> m_RadioReceiverListener;
		std::shared_ptr<SDLDesignatorManagerListener> m_DesignatorListener;
		std::shared_ptr<SDLEmitterSystemManagerListener> m_EmitterSystemListener;
		std::shared_ptr<SDLIFFManagerListener> m_IffListener;
		std::shared_ptr<SDLRRBManagerListener> m_RrbListener;
		std::shared_ptr<SDLMinefieldDataManagerListener> m_MinefieldDataListener;
		std::shared_ptr<SDLEnvironmentObjectManagerListener> m_EnvironmentObjectListener;
		std::shared_ptr<SDLArealObjectManagerListener> m_ArealObjectListener;
		std::shared_ptr<SDLMinefieldObjectManagerListener> m_MinefieldObjectListener;
		std::shared_ptr<SDLLinearObjectManagerListener> m_LinearObjectListener;
		std::shared_ptr<SDLPointObjectManagerListener> m_PointObjectListener;
		std::shared_ptr<SDLRibbonBridgeObjectManagerListener> m_RibbonBridgeObjectListener;
		std::shared_ptr<SDLStructureObjectManagerListener> m_StructureObjectListener;
		std::shared_ptr<SDLEnvironmentProcessManagerListener> m_EnvironmentProcessListener;
		std::shared_ptr<SDLGriddedDataManagerListener> m_GriddedDataListener;
		std::shared_ptr<SDLEmitterBeamManagerListener> m_EmitterBeamListener;
		std::shared_ptr<SDLRadarBeamManagerListener> m_RadarBeamListener;
		std::shared_ptr<SDLJammerBeamManagerListener> m_JammerBeamListener;
		std::shared_ptr<SDLMinefieldManagerListener> m_MinefieldListener;
		std::shared_ptr<SDLFederateManagerListener> m_FederateListener;

		SDL_RTI::Converters converter;

		std::string m_FederationName;
	};
}



