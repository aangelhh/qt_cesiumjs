#include "ClientPitch.h"

#include <DevStudio/HlaPointers.h>
#include <DevStudio/HlaWorld.h>
#include <DevStudio/HlaWorldImpl.h>
#include <DevStudio/ListenerSet.h>
#include <DevStudio/HlaAircraftManager.h>
#include <DevStudio/HlaInteractionManager.h>
#include <DevStudio/HlaAircraftImpl.h>
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/ImplPointers.h>
#include <DevStudio/HlaInteractionManagerImpl.h>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>
#include <DevStudio/FederateManager.h>
#include <DevStudio/HlaPhysicalEntityAttributesImpl.h>
#include <DevStudio/HlaBaseEntityAttributesImpl.h>
#include <DevStudio/HlaBaseEntityAttributes.h>

#include <DevStudio/HlaPhysicalEntityImpl.h>
#include <DevStudio/HlaPlatformAttributesImpl.h>
#include <DevStudio/HlaAircraftAttributesImpl.h>
#include <DevStudio/HlaRadarBeamAttributesImpl.h>
#include <DevStudio/HlaJammerBeamAttributesImpl.h>
#include <DevStudio/HlaHLAfederateAttributesImpl.h>
#include <DevStudio/HlaHLAfederateManager.h>

#include "include/ObjectManagerListeners.h"
#include "include/events/EventBus.h"
#include "include/events/EventSource.h"
#include "include/utils/LoggerManager.h"
#include "include/utils/Singleton.h"
#include "include/events/EventOnDataAvailableObject.h"
#include "include/utils/StringConverter.h"

#include <memory>

using namespace SDL_RTI;

ClientPitch::ClientPitch() 
    : m_World(nullptr), m_SettingsPtr(nullptr), m_Status(false), m_IdMagicAction(1)
#ifdef SEOS
    , m_FederationManagerWrapper(nullptr) 
#endif
{
    initializeManagerListener();

    m_FederationName = "";
    m_EmitterConverter = new(SDL_RTI::EmitterBeamConverter);
    //m_FederateManagerSDL=new(SDL_RTI::FederateManagerSDL);

    // m_federateThread = std::thread([this]() {
    //     while (true) {
    //         checkDisconnectedFederates();
    //         std::this_thread::sleep_for(std::chrono::seconds(1)); // Espera 1 segundo antes de la próxima comprobación
    //     }
    // });

}

ClientPitch::~ClientPitch()
{
    // if (m_federateThread.joinable()) {
    //     m_federateThread.join();
    // }

    delete(m_EmitterConverter);
    //delete(m_FederateManagerSDL);
    m_Status = false;
}

void SDL_RTI::ClientPitch::initializeManagerListener()
{
    m_PhysicalEntityListener = std::make_shared<SDLPhysicalEntityManagerListener>();
    m_BaseEntityListener = std::make_shared<SDLBaseEntityManagerListener>();
    m_PlatformEntityListener = std::make_shared<SDLPlatformManagerListener>();
    m_AircraftEntityListener = std::make_shared<SDLAircraftManagerListener>();
    m_AmphibiousVehicleListener = std::make_shared<SDLAmphibiousVehicleManagerListener>();
    m_GroundVehicleListener = std::make_shared<SDLGroundVehicleManagerListener>();
    m_MultiDomainPlatformListener = std::make_shared<SDLMultiDomainPlatformManagerListener>();
    m_SpacecraftListener = std::make_shared<SDLSpacecraftManagerListener>();
    m_SubmersibleVesselListener = std::make_shared<SDLSubmersibleVesselManagerListener>();
    m_SurfaceVesselListener = std::make_shared<SDLSurfaceVesselManagerListener>();
    m_LifeformListener = std::make_shared<SDLLifeformManagerListener>();
    m_HumanListener = std::make_shared<SDLHumanManagerListener>();
    m_NonHumanListener = std::make_shared<SDLNonHumanManagerListener>();
    m_CulturalFeatureListener = std::make_shared<SDLCulturalFeatureManagerListener>();
    m_MunitionListener = std::make_shared<SDLMunitionManagerListener>();
    m_ExpendablesListener = std::make_shared<SDLExpendablesManagerListener>();
    m_RadioListener = std::make_shared<SDLRadioManagerListener>();
    m_SensorListener = std::make_shared<SDLSensorManagerListener>();
    m_SuppliesListener = std::make_shared<SDLSuppliesManagerListener>();
    m_EmbeddedSystemListener = std::make_shared<SDLEmbeddedSystemManagerListener>();
    m_RadioTransmitterListener = std::make_shared<SDLRadioTransmitterManagerListener>();
    m_RadioReceiverListener = std::make_shared<SDLRadioReceiverManagerListener>();
    m_DesignatorListener = std::make_shared<SDLDesignatorManagerListener>();
    m_EmitterSystemListener = std::make_shared<SDLEmitterSystemManagerListener>();
    m_IffListener = std::make_shared<SDLIFFManagerListener>();
    m_RrbListener = std::make_shared<SDLRRBManagerListener>();
    m_MinefieldDataListener = std::make_shared<SDLMinefieldDataManagerListener>();
    m_EnvironmentObjectListener = std::make_shared<SDLEnvironmentObjectManagerListener>();
    m_ArealObjectListener = std::make_shared<SDLArealObjectManagerListener>();
    m_MinefieldObjectListener = std::make_shared<SDLMinefieldObjectManagerListener>();
    m_LinearObjectListener = std::make_shared<SDLLinearObjectManagerListener>();
    m_PointObjectListener = std::make_shared<SDLPointObjectManagerListener>();
    m_RibbonBridgeObjectListener = std::make_shared<SDLRibbonBridgeObjectManagerListener>();
    m_StructureObjectListener = std::make_shared<SDLStructureObjectManagerListener>();
    m_EnvironmentProcessListener = std::make_shared<SDLEnvironmentProcessManagerListener>();
    m_GriddedDataListener = std::make_shared<SDLGriddedDataManagerListener>();
    m_EmitterBeamListener = std::make_shared<SDLEmitterBeamManagerListener>();
    m_RadarBeamListener = std::make_shared<SDLRadarBeamManagerListener>();
    m_JammerBeamListener = std::make_shared<SDLJammerBeamManagerListener>();
    m_MinefieldListener = std::make_shared<SDLMinefieldManagerListener>();
    m_FederateListener = std::make_shared<SDLFederateManagerListener>();
}


void SDL_RTI::ClientPitch::addBaseEntityObjectListener()
{
    m_World->getHlaPhysicalEntityManager()->addHlaPhysicalEntityManagerListener(m_PhysicalEntityListener);
    m_World->getHlaBaseEntityManager()->addHlaBaseEntityManagerListener(m_BaseEntityListener);
    m_World->getHlaPlatformManager()->addHlaPlatformManagerListener(m_PlatformEntityListener);
    m_World->getHlaAircraftManager()->addHlaAircraftManagerListener(m_AircraftEntityListener);
    m_World->getHlaAmphibiousVehicleManager()->addHlaAmphibiousVehicleManagerListener(m_AmphibiousVehicleListener);
    m_World->getHlaGroundVehicleManager()->addHlaGroundVehicleManagerListener(m_GroundVehicleListener);
    m_World->getHlaMultiDomainPlatformManager()->addHlaMultiDomainPlatformManagerListener(m_MultiDomainPlatformListener);
    m_World->getHlaSpacecraftManager()->addHlaSpacecraftManagerListener(m_SpacecraftListener);
    m_World->getHlaSubmersibleVesselManager()->addHlaSubmersibleVesselManagerListener(m_SubmersibleVesselListener);
    m_World->getHlaSurfaceVesselManager()->addHlaSurfaceVesselManagerListener(m_SurfaceVesselListener);
    m_World->getHlaLifeformManager()->addHlaLifeformManagerListener(m_LifeformListener);
    m_World->getHlaHumanManager()->addHlaHumanManagerListener(m_HumanListener);
    m_World->getHlaNonHumanManager()->addHlaNonHumanManagerListener(m_NonHumanListener);
    m_World->getHlaCulturalFeatureManager()->addHlaCulturalFeatureManagerListener(m_CulturalFeatureListener);
    m_World->getHlaMunitionManager()->addHlaMunitionManagerListener(m_MunitionListener);
    m_World->getHlaExpendablesManager()->addHlaExpendablesManagerListener(m_ExpendablesListener);
    m_World->getHlaRadioManager()->addHlaRadioManagerListener(m_RadioListener);
    m_World->getHlaSensorManager()->addHlaSensorManagerListener(m_SensorListener);
    m_World->getHlaSuppliesManager()->addHlaSuppliesManagerListener(m_SuppliesListener);
}

void SDL_RTI::ClientPitch::addEmbeddedSystemObjectListener()
{
    m_World->getHlaEmbeddedSystemManager()->addHlaEmbeddedSystemManagerListener(m_EmbeddedSystemListener);
    m_World->getHlaRadioTransmitterManager()->addHlaRadioTransmitterManagerListener(m_RadioTransmitterListener);
    m_World->getHlaRadioReceiverManager()->addHlaRadioReceiverManagerListener(m_RadioReceiverListener);
    m_World->getHlaDesignatorManager()->addHlaDesignatorManagerListener(m_DesignatorListener);
    m_World->getHlaEmitterSystemManager()->addHlaEmitterSystemManagerListener(m_EmitterSystemListener);
    m_World->getHlaIFFManager()->addHlaIFFManagerListener(m_IffListener);
    m_World->getHlaRRBManager()->addHlaRRBManagerListener(m_RrbListener);
    m_World->getHlaMinefieldDataManager()->addHlaMinefieldDataManagerListener(m_MinefieldDataListener);
}


void SDL_RTI::ClientPitch::addEnvironmentObjectListener(void)
{
    m_World->getHlaEnvironmentObjectManager()->addHlaEnvironmentObjectManagerListener(m_EnvironmentObjectListener);
    m_World->getHlaArealObjectManager()->addHlaArealObjectManagerListener(m_ArealObjectListener);
    m_World->getHlaMinefieldObjectManager()->addHlaMinefieldObjectManagerListener(m_MinefieldObjectListener);
    m_World->getHlaLinearObjectManager()->addHlaLinearObjectManagerListener(m_LinearObjectListener);
    m_World->getHlaPointObjectManager()->addHlaPointObjectManagerListener(m_PointObjectListener);
    m_World->getHlaRibbonBridgeObjectManager()->addHlaRibbonBridgeObjectManagerListener(m_RibbonBridgeObjectListener);
    m_World->getHlaStructureObjectManager()->addHlaStructureObjectManagerListener(m_StructureObjectListener);
}

void SDL_RTI::ClientPitch::addEmitterBeamObjectListener()
{
    m_World->getHlaEmitterBeamManager()->addHlaEmitterBeamManagerListener(m_EmitterBeamListener);
    m_World->getHlaRadarBeamManager()->addHlaRadarBeamManagerListener(m_RadarBeamListener);
    m_World->getHlaJammerBeamManager()->addHlaJammerBeamManagerListener(m_JammerBeamListener);
}

void SDL_RTI::ClientPitch::addOtherObjectListener()
{
    m_World->getHlaEnvironmentProcessManager()->addHlaEnvironmentProcessManagerListener(m_EnvironmentProcessListener);
    m_World->getHlaGriddedDataManager()->addHlaGriddedDataManagerListener(m_GriddedDataListener);
    m_World->getHlaMinefieldManager()->addHlaMinefieldManagerListener(m_MinefieldListener);
    m_World->getHlaHLAfederateManager()->addHlaHLAfederateManagerListener(m_FederateListener);
}

void ClientPitch::addAllObjectListeners(void)
{

    addBaseEntityObjectListener();
    addEmbeddedSystemObjectListener();
    addEnvironmentObjectListener();
    addEmitterBeamObjectListener();
    addOtherObjectListener();
}

void ClientPitch::addAllInteractionListeners(void)
{
    auto interactionManager = m_World->getHlaInteractionManager();
    auto m_InteractionListener = std::make_shared<InteractionManagerListener>();
    interactionManager->addHlaInteractionListener(m_InteractionListener);
}

DevStudio::HlaWorldPtr SDL_RTI::ClientPitch::getWorld(void)
{
    return m_World;
}


void ClientPitch::initSetting(const std::string& federationName, const std::string& federateName, const std::string& federateType, int siteId, int applicationId, const std::string& ip, int port)
{
    m_SettingsPtr = std::make_shared<SimlabSettings>();

    std::string crcHost = ip;
    int crcPort = port;
    bool createFederation = true;
    bool destroyFederation = false;
    std::string localSettingsDesignator = "crcAddress="+ip+":"+std::to_string(port);

    m_SettingsPtr->setCrcHost(crcHost);
    m_SettingsPtr->setCrcPort(port);
    m_SettingsPtr->setCreateFederation(true);
    m_SettingsPtr->setDestroyFederation(false);
    m_SettingsPtr->setFederationName(federationName);

    m_SettingsPtr->setLocalSettingsDesignator(localSettingsDesignator);
    m_SettingsPtr->setFederateName(federateName);

    m_FederationName = federationName;
}

void SDL_RTI::ClientPitch::initConnection(void)
{
    connectCallbackInteraction();
    connectCallbackObject();
}

bool ClientPitch::initFederation(const std::string& federationName, const std::string& federateName, const std::string& federateType, int siteId, int applicationId, const std::string& ip, int port)
{
    initSetting(federationName, federateName, federateType, siteId, applicationId, ip, port);

    if (m_SettingsPtr)
    {
        m_Status = createWorld(m_SettingsPtr);

        std::cout << "Status: " << m_Status << std::endl;
        if (m_Status)
        {
            addAllInteractionListeners();
            addAllObjectListeners();

            m_World->enableNotifications();
			initConnection();
            startSimulation();

            return m_Status;
        }
    }

    return false;
}

bool ClientPitch::createWorld(std::shared_ptr<SimlabSettings> setting)
{
    
    m_World = DevStudio::HlaWorld::Factory::create(false, m_SettingsPtr);

    if (m_World)
    {
        std::wcout << "crc host: " << m_SettingsPtr->getCrcHost() << std::endl;

        try {
            m_World->connect();
        }
        catch (const DevStudio::HlaConnectException& e) {
            std::cerr << "Failed to connect to the RTI: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaInvalidLicenseException& e) {
            std::cerr << "Invalid license: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaFomException& e) {
            std::cerr << "FOM error: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaInvalidLogicalTimeException& e) {
            std::cerr << "Invalid logical time: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaRtiException& e) {
            std::cerr << "RTI-related problem: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaInternalException& e) {
            std::cerr << "Internal error: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaNotConnectedException& e) {
            std::cerr << "Federate was disconnected during initialization: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaSaveInProgressException& e) {
            std::cerr << "Federation save in progress: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaRestoreInProgressException& e) {
            std::cerr << "Federation restore in progress: " << e.what() << std::endl;
            return false;
        }
        catch (const DevStudio::HlaNormalizerException& e) {
            std::cerr << "Normalizer not set: " << e.what() << std::endl;
            return false;
        }
        // catch (const DevStudio::HlaCredentialsProviderException& e) {
        //     std::cerr << "Failed to provide credentials: " << e.what() << std::endl;
        //     return false;
        // }
        // catch (const DevStudio::HlaInvalidCredentialsException& e) {
        //     std::cerr << "Invalid credentials: " << e.what() << std::endl;
        //     return false;
        // }
        // catch (const DevStudio::HlaUnauthorizedException& e) {
        //     std::cerr << "Credentials not accepted by RTI: " << e.what() << std::endl;
        //     return false;
        // }
        catch (const std::exception& e) {
            std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
            return false;
        }

        std::cout << "Created! " << std::endl;
    }
    return true;
}

void SDL_RTI::ClientPitch::startSimulation(void)
{
    m_World->evokeAll(0.1);
    //std::cout<<"Tama�o:" << m_World->getHlaAircraftManager()->getRemoteHlaAircrafts().size() << std::endl;
    getRemoteAircrafts();
    getRemoteEmitterSystems();
    getRemoteJammers();
    getRemoteRadars();
}

bool SDL_RTI::ClientPitch::getConnectionHlaStatus(void)
{
    return m_Status;
}

void ClientPitch::finishedFederation(void)
{
    disconnectCallbackInteraction();
    disconnectCallbackObject();

    m_World->disconnect();
    m_Status = false;
    m_FederationName = "";
}

void SDL_RTI::ClientPitch::connectCallbackInteraction(void)
{
    events::connect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::StartResumeInteractionEnum);
    events::connect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::StopFreezeInteractionEnum);
    events::connect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::AcknowledgeInteractionEnum);
    events::connect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::WeaponFireInteractionEnum);
    events::connect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::MunitionDetonationInteractionEnum);
    events::connect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::CollisionInteractionEnum);
}

void SDL_RTI::ClientPitch::disconnectCallbackInteraction(void)
{
    events::disconnect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::StartResumeInteractionEnum);
    events::disconnect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::StopFreezeInteractionEnum);
    events::disconnect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::AcknowledgeInteractionEnum);
    events::disconnect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::WeaponFireInteractionEnum);
    events::disconnect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::MunitionDetonationInteractionEnum);
    events::disconnect<EventOnDataAvailableInteraction>(events::EventBus::getInstance(), *this, enumsCommon_Client::InteractionEnum::CollisionInteractionEnum);
}

void SDL_RTI::ClientPitch::handleMagicAction(const unsigned int& objectClass, const std::vector<DevStudio::AttributeValuePairStruct >& attributeValueSet, const unsigned int& requestIdentifier)
{

    if (!m_World)
    {
        std::cerr << "World is not initialized" << std::endl;
        return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    interactionManager->sendCreateObjectRequest(objectClass, attributeValueSet, requestIdentifier);

    std::cout << "handleMagicAction create: " << std::endl;
}

void SDL_RTI::ClientPitch::handlePlayResume(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, 
			const DevStudio::ClockTimeStruct& realWorldTimeStruct, const DevStudio::ClockTimeStruct& simulationTimeStruct, const int& requestID)
{
    if (!m_World) {
    std::cerr << "World is not initialized" << std::endl;
    return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager) {
        std::cerr << "Interaction Manager is not initialized" << std::endl;
        return;
    }
    auto startResumeInteraction = interactionManager->getHlaStartResumeInteraction();
    if (!startResumeInteraction) {
        std::cerr << "Failed to get HlaStartResumeInteractionImpl instance." << std::endl;
        return;
    }

    // config the parameter in the interaction
    startResumeInteraction->setOriginatingEntity(originatingEntity);
    startResumeInteraction->setReceivingEntity(receivingEntity);
    startResumeInteraction->setRealWorldTime(realWorldTimeStruct);
    startResumeInteraction->setRequestIdentifier(requestID);
    startResumeInteraction->setSimulationTime(simulationTimeStruct);
    startResumeInteraction->sendInteraction();

    std::cout << "handlePlayResume Final Send" << std::endl;
}

void SDL_RTI::ClientPitch::handlePauseFreeze(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, 
			const DevStudio::ClockTimeStruct& realWorldTimeStruct, const DevStudio::ClockTimeStruct& simulationTimeStruct, const int& requestID, 
            const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& devStudioReason, bool reflectValues, bool runInternalSimulationClock, bool updateAttributes)
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

    stopFreezeInteraction->setOriginatingEntity(originatingEntity);
    stopFreezeInteraction->setReceivingEntity(receivingEntity);
    stopFreezeInteraction->setRealWorldTime(realWorldTimeStruct);
    stopFreezeInteraction->setRequestIdentifier(requestID);
    stopFreezeInteraction->setReason(devStudioReason);
    stopFreezeInteraction->setReflectValues(reflectValues);
    stopFreezeInteraction->setRunInternalSimulationClock(runInternalSimulationClock);
    stopFreezeInteraction->setUpdateAttributes(updateAttributes);
    stopFreezeInteraction->sendInteraction();
}

void SDL_RTI::ClientPitch::handleStop(const DevStudio::EntityIdentifierStruct& originatingEntity, const DevStudio::EntityIdentifierStruct& receivingEntity, 
			const DevStudio::ClockTimeStruct& realWorldTimeStruct, const DevStudio::ClockTimeStruct& simulationTimeStruct, const int& requestID, 
            const DevStudio::StopFreezeReasonEnum::StopFreezeReasonEnum& devStudioReason, bool reflectValues, bool runInternalSimulationClock, bool updateAttributes )
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
    

    stopFreezeInteraction->setOriginatingEntity(originatingEntity);
    stopFreezeInteraction->setReceivingEntity(receivingEntity);
    stopFreezeInteraction->setRealWorldTime(realWorldTimeStruct);
    stopFreezeInteraction->setRequestIdentifier(requestID);
    stopFreezeInteraction->setReason(devStudioReason);
    stopFreezeInteraction->setReflectValues(reflectValues);
    stopFreezeInteraction->setRunInternalSimulationClock(runInternalSimulationClock);
    stopFreezeInteraction->setUpdateAttributes(updateAttributes);
    stopFreezeInteraction->sendInteraction();
}

void SDL_RTI::ClientPitch::setFederationManagerWrapper(void* wrapperInstance)
{
#ifdef SEOS
    System::IntPtr ptrManagedInstance(wrapperInstance);

    System::Runtime::InteropServices::GCHandle handle = System::Runtime::InteropServices::GCHandle::FromIntPtr(ptrManagedInstance);

    Wrapper::FederationManagerWrapper^ wrapperInstanceCast = safe_cast<Wrapper::FederationManagerWrapper^>(handle.Target);

    if (wrapperInstanceCast)
    {
        //Cambiar la inicializacion si falla
        //m_FederationManagerWrapper = std::make_shared<gcroot<Wrapper::FederationManagerWrapper^>>(wrapperInstanceCast);
        m_FederationManagerWrapper = std::shared_ptr<gcroot<Wrapper::FederationManagerWrapper^>>(
            new gcroot<Wrapper::FederationManagerWrapper^>(wrapperInstanceCast));
    }

    //Necessary to be able to send messages in the notification manager
    LoggerManager::getInstance().setFederationManager(this);
#endif
}

void SDL_RTI::ClientPitch::getRemoteAircrafts(void)
{
    for (auto it : m_World->getHlaAircraftManager()->getRemoteHlaAircrafts()) {
        m_AircraftEntityListener->hlaAircraftInitialized(it, m_World->getCurrentTime(), m_World->getCurrentLogicalTime());
        }

    
}

void SDL_RTI::ClientPitch::getRemoteRadars(void)
{
    for (auto it : m_World->getHlaRadarBeamManager()->getRemoteHlaRadarBeams()) {
        m_RadarBeamListener->hlaRadarBeamInitialized(it, m_World->getCurrentTime(), m_World->getCurrentLogicalTime());
    }
}

void SDL_RTI::ClientPitch::getRemoteJammers(void)
{
    for (auto it : m_World->getHlaJammerBeamManager()->getRemoteHlaJammerBeams()) {
        m_JammerBeamListener->hlaJammerBeamInitialized(it, m_World->getCurrentTime(), m_World->getCurrentLogicalTime());
    }
}

void SDL_RTI::ClientPitch::getRemoteEmitterSystems(void)
{
    for (auto it : m_World->getHlaEmitterSystemManager()->getRemoteHlaEmitterSystems()) {
        m_EmitterSystemListener->hlaEmitterSystemInitialized(it, m_World->getCurrentTime(), m_World->getCurrentLogicalTime());
    }
}

void SDL_RTI::ClientPitch::connectCallbackObject(void)
{
    
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::FederateObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
    events::connect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
}

void SDL_RTI::ClientPitch::disconnectCallbackObject(void)
{
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::FederateObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
    events::disconnect<EventOnDataAvailableObject>(events::EventBus::getInstance(), *this, enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
}

void SDL_RTI::ClientPitch::sendStartResumeInteractionHLA(
    int originatingEntityId,
    int originatingApplicationId,
    int originatingSiteId,
    int receivingEntityId,
    int receivingApplicationId,
    int receivingSiteId,
    double realWorldTime,
    double simulationTime,
    unsigned int requestIdentifier) {
    if (!m_World) {
        std::cerr << "World is not  ialized" << std::endl;
        return;
    }

    auto interactionManager = std::dynamic_pointer_cast<DevStudio::HlaInteractionManagerImpl>(m_World->getHlaInteractionManager());
    if (!interactionManager) {
        std::cerr << "Interaction Manager is not initialized" << std::endl;
        return;
    }
    auto startResumeInteraction = interactionManager->getHlaStartResumeInteraction();
    if (!startResumeInteraction) {
        std::cerr << "Failed to get HlaStartResumeInteractionImpl instance." << std::endl;
        return;
    }

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

    DevStudio::ClockTimeStruct simulationTimeStruct = {
        static_cast<int32_t>(std::floor(simulationTime)),
        static_cast<uint32_t>((simulationTime - std::floor(simulationTime)) * 1e9)
    };

    // Configurar los parametros en la interaccion
    startResumeInteraction->setOriginatingEntity(originatingEntity);
    startResumeInteraction->setReceivingEntity(receivingEntity);
    startResumeInteraction->setRealWorldTime(realWorldTimeStruct);
    startResumeInteraction->setRequestIdentifier(requestIdentifier);
    startResumeInteraction->setSimulationTime(simulationTimeStruct);
    startResumeInteraction->sendInteraction();
}

#ifdef SEOS
void SDL_RTI::ClientPitch::sendStopFreezeInteractionHLA(int originatingEntityId, int originatingApplicationId, int originatingSiteId, int receivingEntityId, int receivingApplicationId, int receivingSiteId, double realWorldTime, unsigned int requestIdentifier, enumsCommon::ReasonEnum reason, bool reflectValues, bool runInternalSimulationClock, bool updateAttributes)
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

void ClientPitch::notifyFinishedFederation(const std::string& description)
{
//Investigar caidas o cierras del rti para notificar a la seos.
//#ifdef SEOS
//    if (m_FederationManagerWrapper)
//    {
//        (*m_FederationManagerWrapper)->notifyFinishedFederation(description);
//    }
//    else
//    {
//        std::cout << "The wrapper object is invalid" << std::endl;
//    }
//#endif
}

void ClientPitch::evokeAll(unsigned long long millis)
{
    m_World->evokeAll(millis);
}


