#include "../include/ObjectManagerListeners.h"
#include "../include/events/EventOnDataAvailableObject.h"
#include "../include/enumsCommon/EnumsCommons.h"
#include "../include/events/EventBus.h"
#include "../include/SimlabSettings.h"
#include "DevStudio/HlaAircraftImpl.h"
#include "DevStudio/HlaPlatformImpl.h"
#include "DevStudio/HlaBaseEntityImpl.h"
#include "DevStudio/HlaPhysicalEntityImpl.h"
#include "DevStudio/HlaEmitterSystemImpl.h"
#include "DevStudio/HlaEmitterBeamImpl.h"
#include "DevStudio/HlaRadarBeamImpl.h"
#include "DevStudio/HlaJammerBeamImpl.h"
#include "DevStudio/HlaAmphibiousVehicleImpl.h"
#include "DevStudio/HlaGroundVehicleImpl.h"
#include "DevStudio/HlaMultiDomainPlatformImpl.h"
#include "DevStudio/HlaSpacecraftImpl.h"
#include "DevStudio/HlaSubmersibleVesselImpl.h"
#include "DevStudio/HlaSurfaceVesselImpl.h"
#include "DevStudio/HlaLifeformImpl.h"
#include "DevStudio/HlaHumanImpl.h"
#include "DevStudio/HlaNonHumanImpl.h"
#include "DevStudio/HlaCulturalFeatureImpl.h"
#include "DevStudio/HlaMunitionImpl.h"
#include "DevStudio/HlaExpendablesImpl.h"
#include "DevStudio/HlaRadioImpl.h"
#include "DevStudio/HlaSensorImpl.h"
#include "DevStudio/HlaSuppliesImpl.h"
#include "DevStudio/HlaEmbeddedSystemImpl.h"
#include "DevStudio/HlaGriddedDataImpl.h"
#include "DevStudio/HlaEnvironmentProcessImpl.h"
#include "DevStudio/HlaStructureObjectImpl.h"
#include "DevStudio/HlaRibbonBridgeObjectImpl.h"
#include "DevStudio/HlaLinearObjectImpl.h"
#include "DevStudio/HlaMinefieldObjectImpl.h"
#include "DevStudio/HlaArealObjectImpl.h"
#include "DevStudio/HlaPointObjectImpl.h"
#include "DevStudio/HlaEnvironmentObjectImpl.h"
#include "DevStudio/HlaMinefieldDataImpl.h"
#include "DevStudio/HlaMinefieldImpl.h"
#include "DevStudio/HlaRRBImpl.h"
#include "DevStudio/HlaIFFImpl.h"
#include "DevStudio/HlaDesignatorImpl.h"
#include "DevStudio/HlaRadioReceiverImpl.h"
#include "DevStudio/HlaRadioTransmitterImpl.h"
#include "../ClientPitch.h"
#include <future>
#include <cmath>
#include <memory>
#include <iostream>
#include <iostream>
#include "../include/utils/StringConverter.h"
#include "../include/DataTypes/Converters.h"
#include "DevStudio/HlaHLAfederateImpl.h"

using namespace SDL_RTI;

void SDLFederateManagerListener::hlaHLAfederateDiscovered(DevStudio::HlaHLAfederatePtr hLAfederate, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new federate: " << hLAfederate->getHlaInstanceName() << std::endl;
}

void SDLFederateManagerListener::hlaHLAfederateInitialized(DevStudio::HlaHLAfederatePtr hLAfederate, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    auto future = std::async(std::launch::async, [hLAfederate]() {
        while (!hLAfederate->hasHLAfederateName()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaHLAfederateImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaHLAfederateImpl>(hLAfederate);

    if (attributesimpl) {
        DevStudio::HlaHLAfederateAttributesPtr attributes = attributesimpl->getHlaHLAfederateAttributes();
        EventOnDataAvailableObject data;

        if (attributes)
        {

            auto attributesCopy = attributes;
            data.setObjectName(hLAfederate->getHlaInstanceName());
            std::wcout << L"Instance name: " << hLAfederate->getHlaInstanceName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::FederateObjectEnum);
            data.setFederateName(hLAfederate->getProducingFederate());

            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::FederateObjectEnum);
        }
    }


    hLAfederate->addHlaHLAfederateListener(shared_from_this());
}

void SDLFederateManagerListener::hlaHLAfederateDeleted(DevStudio::HlaHLAfederatePtr hLAfederate, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::wcout << L"hlaFederateApplicationDeleted federate: " << hLAfederate->getHlaInstanceName() << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaHLAfederateImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaHLAfederateImpl>(hLAfederate);
    DevStudio::HlaHLAfederateAttributesPtr attributes = attributesimpl->getHlaHLAfederateAttributes();

    if (attributesimpl && attributes) {
        auto updater = attributesimpl->getHlaHLAfederateUpdater();

        data.setObjectName(hLAfederate->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::FederateObjectEnum);
        data.setFederateName(hLAfederate->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::FederateObjectEnum);
    }
}

void SDLFederateManagerListener::attributesUpdated(DevStudio::HlaHLAfederatePtr hLAfederate, const DevStudio::HlaEnumSet<DevStudio::HlaHLAfederateAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::wcout << L"attributesUpdated federate: " << hLAfederate->getHlaInstanceName() << std::endl;
    DevStudio::HlaHLAfederateImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaHLAfederateImpl>(hLAfederate);
    if (attributesimpl)
    {
        DevStudio::HlaHLAfederateAttributesPtr attributesFederate = attributesimpl->getHlaHLAfederateAttributes();

        EventOnDataAvailableObject data;

        if (attributesFederate)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesFederate);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::FederateObjectEnum);
            data.setFederateName(hLAfederate->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::FederateObjectEnum);
        }
    }
}

void SDLPhysicalEntityManagerListener::hlaPhysicalEntityDiscovered(DevStudio::HlaPhysicalEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new physical entity: " << entity->getHlaInstanceName() << std::endl;
}
void SDLPhysicalEntityManagerListener::hlaPhysicalEntityInitialized(DevStudio::HlaPhysicalEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Espera hasta que EntityIdentifier esté disponible (ajusta si tu clase lo soporta)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaPhysicalEntityImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPhysicalEntityImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaPhysicalEntityAttributesPtr attributes = attributesimpl->getHlaPhysicalEntityAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
        }
    }

    entity->addHlaPhysicalEntityListener(shared_from_this());
}
void SDLPhysicalEntityManagerListener::hlaPhysicalEntityDeleted(DevStudio::HlaPhysicalEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a physical entity: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaPhysicalEntityImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPhysicalEntityImpl>(entity);
    DevStudio::HlaPhysicalEntityAttributesPtr attributes = attributesimpl->getHlaPhysicalEntityAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaPhysicalEntityUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
    }
}
void SDLPhysicalEntityManagerListener::attributesUpdated(DevStudio::HlaPhysicalEntityPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaPhysicalEntityAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaPhysicalEntityImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPhysicalEntityImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaPhysicalEntityAttributesPtr attributesPhysicalEntity = attributesimpl->getHlaPhysicalEntityAttributes();

        EventOnDataAvailableObject data;

        if (attributesPhysicalEntity)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesPhysicalEntity);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PhysicalEntityObjectEnum);
        }
    }
}

void SDLBaseEntityManagerListener::hlaBaseEntityDiscovered(DevStudio::HlaBaseEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new base entity: " << entity->getHlaInstanceName() << std::endl;
}
void SDLBaseEntityManagerListener::hlaBaseEntityDeleted(DevStudio::HlaBaseEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a base entity: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaBaseEntityImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaBaseEntityImpl>(entity);
    DevStudio::HlaBaseEntityAttributesPtr attributes = attributesimpl->getHlaBaseEntityAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaBaseEntityUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
    }
}
void SDLBaseEntityManagerListener::hlaBaseEntityInitialized(DevStudio::HlaBaseEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for BaseEntity, otherwise remove this block)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaBaseEntityImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaBaseEntityImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaBaseEntityAttributesPtr attributes = attributesimpl->getHlaBaseEntityAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
        }
    }

    entity->addHlaBaseEntityListener(shared_from_this());
}
void SDLBaseEntityManagerListener::attributesUpdated(DevStudio::HlaBaseEntityPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaBaseEntityAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaBaseEntityImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaBaseEntityImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaBaseEntityAttributesPtr attributesBaseEntity = attributesimpl->getHlaBaseEntityAttributes();

        EventOnDataAvailableObject data;

        if (attributesBaseEntity)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesBaseEntity);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::BaseEntityObjectEnum);
        }
    }
}

void SDLPlatformManagerListener::hlaPlatformDiscovered(DevStudio::HlaPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new platform: " << entity->getHlaInstanceName() << std::endl;
}
void SDLPlatformManagerListener::hlaPlatformDeleted(DevStudio::HlaPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a platform: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaPlatformImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPlatformImpl>(entity);
    DevStudio::HlaPlatformAttributesPtr attributes = attributesimpl->getHlaPlatformAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaPlatformUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
    }
}
void SDLPlatformManagerListener::hlaPlatformInitialized(DevStudio::HlaPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // wait untip EntityIdenfier is set
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    future.get();
    DevStudio::HlaPlatformImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPlatformImpl>(entity);
    if (attributesimpl) 
    {
        DevStudio::HlaPlatformAttributesPtr attributes = attributesimpl->getHlaPlatformAttributes();
        EventOnDataAvailableObject data;

        if (attributes)
        {
            auto attributesCopy = attributes;
            // auto updater = attributesimpl->getHlaAircraftUpdater();
             //updater->
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
        }
    }


    // entity->addHlaAircraftListener(shared_from_this());
}
void SDLPlatformManagerListener::attributesUpdated(DevStudio::HlaPlatformPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaPlatformAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaPlatformImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPlatformImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaPlatformAttributesPtr attributesPlatform = attributesimpl->getHlaPlatformAttributes();

        EventOnDataAvailableObject data;

        if (attributesPlatform)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesPlatform);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PlatformObjectEnum);
        }
    }
}

void SDLAircraftManagerListener::hlaAircraftDiscovered(DevStudio::HlaAircraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    if (entity) {
        std::wcout << L"Discovered a new aircraft: " << entity->getHlaInstanceName() << std::endl;
    }
}
void SDLAircraftManagerListener::hlaAircraftDeleted(DevStudio::HlaAircraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted an aircraft: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaAircraftImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaAircraftImpl>(entity);
    DevStudio::HlaAircraftAttributesPtr attributes = attributesimpl->getHlaAircraftAttributes();
    
    perf_.onDelete(entity->getHlaInstanceName());
    if (attributesimpl && attributes) 
    {
        auto updater = attributesimpl->getHlaAircraftUpdater();
        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
    }
}
void SDLAircraftManagerListener::hlaAircraftInitialized(DevStudio::HlaAircraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{   
    // wait untip EntityIdenfier is set
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    future.get();

    DevStudio::HlaAircraftImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaAircraftImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaAircraftAttributesPtr attributes = attributesimpl->getHlaAircraftAttributes();
        EventOnDataAvailableObject data;

        if (attributes)
        {

            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
        }
    }

    entity->addHlaAircraftListener(shared_from_this());
}
void SDLAircraftManagerListener::attributesUpdated(DevStudio::HlaAircraftPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaAircraftAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaAircraftImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaAircraftImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaAircraftAttributesPtr attributesAircraft = attributesimpl->getHlaAircraftAttributes();


        // PERFORMANCE TEST

        const std::wstring& inst = entity->getHlaInstanceName();               // :contentReference[oaicite:1]{index=1}
        const std::string producer = SDL_RTI::StringConverter::tostr(
                                       entity->getProducingFederate()->getFederateName());
        perf_.onUpdate(inst, attributes.size(), producer);

        std::cout << "updated aircraft" << std::endl;
        ///////////////////





        EventOnDataAvailableObject data;

        if (attributesAircraft)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesAircraft);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::AircraftObjectEnum);
        }
    }
}

void SDLAmphibiousVehicleManagerListener::hlaAmphibiousVehicleDiscovered(DevStudio::HlaAmphibiousVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new amphibious vehicle: " << entity->getHlaInstanceName() << std::endl;
}
void SDLAmphibiousVehicleManagerListener::hlaAmphibiousVehicleDeleted(DevStudio::HlaAmphibiousVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted an amphibious vehicle: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaAmphibiousVehicleImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaAmphibiousVehicleImpl>(entity);
    DevStudio::HlaAmphibiousVehicleAttributesPtr attributes = attributesimpl->getHlaAmphibiousVehicleAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaAmphibiousVehicleUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::AmphibiousVehicleObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::AmphibiousVehicleObjectEnum);
    }
}
void SDLAmphibiousVehicleManagerListener::hlaAmphibiousVehicleInitialized(DevStudio::HlaAmphibiousVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    future.get();

    DevStudio::HlaAmphibiousVehicleImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaAmphibiousVehicleImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaAmphibiousVehicleAttributesPtr attributes = attributesimpl->getHlaAmphibiousVehicleAttributes();
        EventOnDataAvailableObject data;

        if (attributes)
        {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::AmphibiousVehicleObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::AmphibiousVehicleObjectEnum);
        }
    }

    entity->addHlaAmphibiousVehicleListener(shared_from_this());
}
void SDLAmphibiousVehicleManagerListener::attributesUpdated(DevStudio::HlaAmphibiousVehiclePtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaAmphibiousVehicleAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaAmphibiousVehicleImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaAmphibiousVehicleImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaAmphibiousVehicleAttributesPtr attributesVehicle = attributesimpl->getHlaAmphibiousVehicleAttributes();

        EventOnDataAvailableObject data;

        if (attributesVehicle)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesVehicle);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::AmphibiousVehicleObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::AmphibiousVehicleObjectEnum);
        }
    }
}

void SDLGroundVehicleManagerListener::hlaGroundVehicleDiscovered(DevStudio::HlaGroundVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new ground vehicle: " << entity->getHlaInstanceName() << std::endl;
}
void SDLGroundVehicleManagerListener::hlaGroundVehicleDeleted(DevStudio::HlaGroundVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a ground vehicle: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaGroundVehicleImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleImpl>(entity);
    DevStudio::HlaGroundVehicleAttributesPtr attributes = attributesimpl->getHlaGroundVehicleAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaGroundVehicleUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::GroundVehicleObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::GroundVehicleObjectEnum);
    }
}
void SDLGroundVehicleManagerListener::hlaGroundVehicleInitialized(DevStudio::HlaGroundVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    future.get();

    DevStudio::HlaGroundVehicleImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaGroundVehicleAttributesPtr attributes = attributesimpl->getHlaGroundVehicleAttributes();
        EventOnDataAvailableObject data;

        if (attributes)
        {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::GroundVehicleObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::GroundVehicleObjectEnum);
        }
    }

    entity->addHlaGroundVehicleListener(shared_from_this());
}
void SDLGroundVehicleManagerListener::attributesUpdated(DevStudio::HlaGroundVehiclePtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaGroundVehicleAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaGroundVehicleImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaGroundVehicleImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaGroundVehicleAttributesPtr attributesGroundVehicle = attributesimpl->getHlaGroundVehicleAttributes();

        EventOnDataAvailableObject data;

        if (attributesGroundVehicle)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesGroundVehicle);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::GroundVehicleObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::GroundVehicleObjectEnum);
        }
    }
}

void SDLMultiDomainPlatformManagerListener::hlaMultiDomainPlatformDiscovered(DevStudio::HlaMultiDomainPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new multi-domain platform: " << entity->getHlaInstanceName() << std::endl;
}
void SDLMultiDomainPlatformManagerListener::hlaMultiDomainPlatformDeleted(DevStudio::HlaMultiDomainPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a multi-domain platform: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaMultiDomainPlatformImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMultiDomainPlatformImpl>(entity);
    DevStudio::HlaMultiDomainPlatformAttributesPtr attributes = attributesimpl->getHlaMultiDomainPlatformAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaMultiDomainPlatformUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::MultiDomainPlatformObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MultiDomainPlatformObjectEnum);
    }
}
void SDLMultiDomainPlatformManagerListener::hlaMultiDomainPlatformInitialized(DevStudio::HlaMultiDomainPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    future.get();

    DevStudio::HlaMultiDomainPlatformImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMultiDomainPlatformImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMultiDomainPlatformAttributesPtr attributes = attributesimpl->getHlaMultiDomainPlatformAttributes();
        EventOnDataAvailableObject data;

        if (attributes)
        {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MultiDomainPlatformObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MultiDomainPlatformObjectEnum);
        }
    }

    entity->addHlaMultiDomainPlatformListener(shared_from_this());
}
void SDLMultiDomainPlatformManagerListener::attributesUpdated(DevStudio::HlaMultiDomainPlatformPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMultiDomainPlatformAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaMultiDomainPlatformImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMultiDomainPlatformImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaMultiDomainPlatformAttributesPtr attributesPlatform = attributesimpl->getHlaMultiDomainPlatformAttributes();

        EventOnDataAvailableObject data;

        if (attributesPlatform)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesPlatform);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MultiDomainPlatformObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MultiDomainPlatformObjectEnum);
        }
    }
}

void SDLSpacecraftManagerListener::hlaSpacecraftDiscovered(DevStudio::HlaSpacecraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new spacecraft: " << entity->getHlaInstanceName() << std::endl;
}
void SDLSpacecraftManagerListener::hlaSpacecraftDeleted(DevStudio::HlaSpacecraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a spacecraft: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaSpacecraftImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSpacecraftImpl>(entity);
    DevStudio::HlaSpacecraftAttributesPtr attributes = attributesimpl->getHlaSpacecraftAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaSpacecraftUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::SpacecraftObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SpacecraftObjectEnum);
    }
}
void SDLSpacecraftManagerListener::hlaSpacecraftInitialized(DevStudio::HlaSpacecraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for Spacecraft, otherwise adjust/remove)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaSpacecraftImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSpacecraftImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSpacecraftAttributesPtr attributes = attributesimpl->getHlaSpacecraftAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SpacecraftObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SpacecraftObjectEnum);
        }
    }

    entity->addHlaSpacecraftListener(shared_from_this());
}
void SDLSpacecraftManagerListener::attributesUpdated(DevStudio::HlaSpacecraftPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSpacecraftAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaSpacecraftImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSpacecraftImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaSpacecraftAttributesPtr attributesSpacecraft = attributesimpl->getHlaSpacecraftAttributes();

        EventOnDataAvailableObject data;

        if (attributesSpacecraft)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesSpacecraft);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SpacecraftObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SpacecraftObjectEnum);
        }
    }
}

void SDLSubmersibleVesselManagerListener::hlaSubmersibleVesselDiscovered(DevStudio::HlaSubmersibleVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new submersible vessel: " << entity->getHlaInstanceName() << std::endl;
}
void SDLSubmersibleVesselManagerListener::hlaSubmersibleVesselDeleted(DevStudio::HlaSubmersibleVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a submersible vessel: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaSubmersibleVesselImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSubmersibleVesselImpl>(entity);
    DevStudio::HlaSubmersibleVesselAttributesPtr attributes = attributesimpl->getHlaSubmersibleVesselAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaSubmersibleVesselUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::SubmersibleVesselObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SubmersibleVesselObjectEnum);
    }
}
void SDLSubmersibleVesselManagerListener::hlaSubmersibleVesselInitialized(DevStudio::HlaSubmersibleVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for SubmersibleVessel, otherwise adjust/remove)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaSubmersibleVesselImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSubmersibleVesselImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSubmersibleVesselAttributesPtr attributes = attributesimpl->getHlaSubmersibleVesselAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SubmersibleVesselObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SubmersibleVesselObjectEnum);
        }
    }

    entity->addHlaSubmersibleVesselListener(shared_from_this());
}
void SDLSubmersibleVesselManagerListener::attributesUpdated(DevStudio::HlaSubmersibleVesselPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSubmersibleVesselAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaSubmersibleVesselImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSubmersibleVesselImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaSubmersibleVesselAttributesPtr attributesSubmersibleVessel = attributesimpl->getHlaSubmersibleVesselAttributes();

        EventOnDataAvailableObject data;

        if (attributesSubmersibleVessel)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesSubmersibleVessel);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SubmersibleVesselObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SubmersibleVesselObjectEnum);
        }
    }
}

void SDLSurfaceVesselManagerListener::hlaSurfaceVesselDiscovered(DevStudio::HlaSurfaceVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new surface vessel: " << entity->getHlaInstanceName() << std::endl;
}
void SDLSurfaceVesselManagerListener::hlaSurfaceVesselDeleted(DevStudio::HlaSurfaceVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a surface vessel: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaSurfaceVesselImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselImpl>(entity);
    DevStudio::HlaSurfaceVesselAttributesPtr attributes = attributesimpl->getHlaSurfaceVesselAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaSurfaceVesselUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::SurfaceVesselObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SurfaceVesselObjectEnum);
    }
}
void SDLSurfaceVesselManagerListener::hlaSurfaceVesselInitialized(DevStudio::HlaSurfaceVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for SurfaceVessel, otherwise adjust/remove)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaSurfaceVesselImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSurfaceVesselAttributesPtr attributes = attributesimpl->getHlaSurfaceVesselAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SurfaceVesselObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SurfaceVesselObjectEnum);
        }
    }

    entity->addHlaSurfaceVesselListener(shared_from_this());
}
void SDLSurfaceVesselManagerListener::attributesUpdated(DevStudio::HlaSurfaceVesselPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSurfaceVesselAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaSurfaceVesselImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSurfaceVesselImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaSurfaceVesselAttributesPtr attributesSurfaceVessel = attributesimpl->getHlaSurfaceVesselAttributes();

        EventOnDataAvailableObject data;

        if (attributesSurfaceVessel)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesSurfaceVessel);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SurfaceVesselObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SurfaceVesselObjectEnum);
        }
    }
}

void SDLLifeformManagerListener::hlaLifeformDiscovered(DevStudio::HlaLifeformPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new lifeform: " << entity->getHlaInstanceName() << std::endl;
}
void SDLLifeformManagerListener::hlaLifeformDeleted(DevStudio::HlaLifeformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a lifeform: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaLifeformImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaLifeformImpl>(entity);
    DevStudio::HlaLifeformAttributesPtr attributes = attributesimpl->getHlaLifeformAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaLifeformUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::LifeformObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::LifeformObjectEnum);
    }
}
void SDLLifeformManagerListener::hlaLifeformInitialized(DevStudio::HlaLifeformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for Lifeform, otherwise adjust/remove)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaLifeformImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaLifeformImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaLifeformAttributesPtr attributes = attributesimpl->getHlaLifeformAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::LifeformObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::LifeformObjectEnum);
        }
    }

    entity->addHlaLifeformListener(shared_from_this());
}
void SDLLifeformManagerListener::attributesUpdated(DevStudio::HlaLifeformPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaLifeformAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaLifeformImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaLifeformImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaLifeformAttributesPtr attributesLifeform = attributesimpl->getHlaLifeformAttributes();

        EventOnDataAvailableObject data;

        if (attributesLifeform)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesLifeform);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::LifeformObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::LifeformObjectEnum);
        }
    }
}

void SDLHumanManagerListener::hlaHumanDiscovered(DevStudio::HlaHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new human: " << entity->getHlaInstanceName() << std::endl;
}
void SDLHumanManagerListener::hlaHumanDeleted(DevStudio::HlaHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a human: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaHumanImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaHumanImpl>(entity);
    DevStudio::HlaHumanAttributesPtr attributes = attributesimpl->getHlaHumanAttributes();

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaHumanUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::HumanObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::HumanObjectEnum);
    }
}
void SDLHumanManagerListener::hlaHumanInitialized(DevStudio::HlaHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for Human, otherwise adjust/remove)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaHumanImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaHumanImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaHumanAttributesPtr attributes = attributesimpl->getHlaHumanAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::HumanObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::HumanObjectEnum);
        }
    }

    entity->addHlaHumanListener(shared_from_this());
}
void SDLHumanManagerListener::attributesUpdated(DevStudio::HlaHumanPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaHumanAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaHumanImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaHumanImpl>(entity);
    if (attributesimpl)
    {
        DevStudio::HlaHumanAttributesPtr attributesHuman = attributesimpl->getHlaHumanAttributes();

        EventOnDataAvailableObject data;

        if (attributesHuman)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesHuman);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::HumanObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::HumanObjectEnum);
        }
    }
}

void SDLNonHumanManagerListener::hlaNonHumanDiscovered(DevStudio::HlaNonHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new non-human: " << entity->getHlaInstanceName() << std::endl;
}
void SDLNonHumanManagerListener::hlaNonHumanDeleted(DevStudio::HlaNonHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted non-human: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaNonHumanImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaNonHumanImpl>(entity);
    DevStudio::HlaNonHumanAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaNonHumanAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaNonHumanUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::NonHumanObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::NonHumanObjectEnum);
    }
}
void SDLNonHumanManagerListener::hlaNonHumanInitialized(DevStudio::HlaNonHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until EntityIdentifier is set (if available for NonHuman, otherwise adjust/remove)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaNonHumanImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaNonHumanImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaNonHumanAttributesPtr attributes = attributesimpl->getHlaNonHumanAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::NonHumanObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::NonHumanObjectEnum);
        }
    }

    entity->addHlaNonHumanListener(shared_from_this());
}
void SDLNonHumanManagerListener::attributesUpdated(DevStudio::HlaNonHumanPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaNonHumanAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaNonHumanImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaNonHumanImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaNonHumanAttributesPtr attributesNonHuman = attributesimpl->getHlaNonHumanAttributes();

        EventOnDataAvailableObject data;

        if (attributesNonHuman)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesNonHuman);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::NonHumanObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::NonHumanObjectEnum);
        }
    }
}

void SDLCulturalFeatureManagerListener::hlaCulturalFeatureDiscovered(DevStudio::HlaCulturalFeaturePtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new cultural feature: " << entity->getHlaInstanceName() << std::endl;
}
void SDLCulturalFeatureManagerListener::hlaCulturalFeatureDeleted(DevStudio::HlaCulturalFeaturePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted cultural feature: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaCulturalFeatureImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaCulturalFeatureImpl>(entity);
    DevStudio::HlaCulturalFeatureAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaCulturalFeatureAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaCulturalFeatureUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::CulturalFeatureObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::CulturalFeatureObjectEnum);
    }
}
void SDLCulturalFeatureManagerListener::hlaCulturalFeatureInitialized(DevStudio::HlaCulturalFeaturePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string culturalFeatureName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << culturalFeatureName << std::endl;

    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaCulturalFeatureImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaCulturalFeatureImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaCulturalFeatureAttributesPtr attributes = attributesimpl->getHlaCulturalFeatureAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::CulturalFeatureObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::CulturalFeatureObjectEnum);
        }
    }

    entity->addHlaCulturalFeatureListener(shared_from_this());
}
void SDLCulturalFeatureManagerListener::attributesUpdated(DevStudio::HlaCulturalFeaturePtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaCulturalFeatureAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaCulturalFeatureImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaCulturalFeatureImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaCulturalFeatureAttributesPtr attributesCultural = attributesimpl->getHlaCulturalFeatureAttributes();

        EventOnDataAvailableObject data;

        if (attributesCultural)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesCultural);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::CulturalFeatureObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::CulturalFeatureObjectEnum);
        }
    }
}

void SDLMunitionManagerListener::hlaMunitionDiscovered(DevStudio::HlaMunitionPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new munition: " << entity->getHlaInstanceName() << std::endl;
}
void SDLMunitionManagerListener::hlaMunitionDeleted(DevStudio::HlaMunitionPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string aircraftName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a munition: " << aircraftName << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaMunitionImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMunitionImpl>(entity);
    auto attributes = attributesimpl->getHlaMunitionAttributes();

    if (attributesimpl && attributes) {
        auto updater = attributesimpl->getHlaMunitionUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::MunitionObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MunitionObjectEnum);
    }
}
void SDLMunitionManagerListener::hlaMunitionInitialized(DevStudio::HlaMunitionPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string munitionName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << munitionName << std::endl;

    // Wait until EntityIdentifier is set (for consistency)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaMunitionImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMunitionImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMunitionAttributesPtr attributes = attributesimpl->getHlaMunitionAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MunitionObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MunitionObjectEnum);
        }
    }
    entity->addHlaMunitionListener(shared_from_this());
}
void SDLMunitionManagerListener::attributesUpdated(DevStudio::HlaMunitionPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMunitionAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaMunitionImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMunitionImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMunitionAttributesPtr attributesMunition = attributesimpl->getHlaMunitionAttributes();

        EventOnDataAvailableObject data;

        if (attributesMunition)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesMunition);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MunitionObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MunitionObjectEnum);
        }
    }
}

void SDLExpendablesManagerListener::hlaExpendablesDiscovered(DevStudio::HlaExpendablesPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new expendable: " << entity->getHlaInstanceName() << std::endl;
}
void SDLExpendablesManagerListener::hlaExpendablesDeleted(DevStudio::HlaExpendablesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted expendables: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaExpendablesImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaExpendablesImpl>(entity);
    DevStudio::HlaExpendablesAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaExpendablesAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaExpendablesUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::ExpendablesObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::ExpendablesObjectEnum);
    }
}
void SDLExpendablesManagerListener::hlaExpendablesInitialized(DevStudio::HlaExpendablesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string expendablesName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << expendablesName << std::endl;

    // Wait until EntityIdentifier is set (for consistency)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaExpendablesImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaExpendablesImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaExpendablesAttributesPtr attributes = attributesimpl->getHlaExpendablesAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::ExpendablesObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::ExpendablesObjectEnum);
        }
    }

    entity->addHlaExpendablesListener(shared_from_this());
}
void SDLExpendablesManagerListener::attributesUpdated(DevStudio::HlaExpendablesPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaExpendablesAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaExpendablesImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaExpendablesImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaExpendablesAttributesPtr attributesExpendables = attributesimpl->getHlaExpendablesAttributes();

        EventOnDataAvailableObject data;

        if (attributesExpendables)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesExpendables);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::ExpendablesObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::ExpendablesObjectEnum);
        }
    }
}

void SDLRadioManagerListener::hlaRadioDiscovered(DevStudio::HlaRadioPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new radio: " << entity->getHlaInstanceName() << std::endl;
}
void SDLRadioManagerListener::hlaRadioDeleted(DevStudio::HlaRadioPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted radio: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaRadioImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioImpl>(entity);
    DevStudio::HlaRadioAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaRadioAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaRadioUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioObjectEnum);
    }
}
void SDLRadioManagerListener::hlaRadioInitialized(DevStudio::HlaRadioPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string radioName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << radioName << std::endl;

    // Wait until EntityIdentifier is set (for consistency)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaRadioImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadioAttributesPtr attributes = attributesimpl->getHlaRadioAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioObjectEnum);
        }
    }

    entity->addHlaRadioListener(shared_from_this());
}
void SDLRadioManagerListener::attributesUpdated(DevStudio::HlaRadioPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadioAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaRadioImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadioAttributesPtr attributesRadio = attributesimpl->getHlaRadioAttributes();

        EventOnDataAvailableObject data;

        if (attributesRadio)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesRadio);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioObjectEnum);
        }
    }
}

void SDLSensorManagerListener::hlaSensorDiscovered(DevStudio::HlaSensorPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new sensor: " << entity->getHlaInstanceName() << std::endl;
}
void SDLSensorManagerListener::hlaSensorDeleted(DevStudio::HlaSensorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::wcout << L"Deleted a new sensor: " << entity->getHlaInstanceName() << std::endl;

    std::shared_ptr<DevStudio::HlaSensorImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSensorImpl>(entity);
    DevStudio::HlaSensorAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaSensorAttributes() : nullptr;

    EventOnDataAvailableObject data;
    if (attributesimpl && attributes) {
        auto updater = attributesimpl->getHlaSensorUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::SensorObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SensorObjectEnum);
    }
}
void SDLSensorManagerListener::hlaSensorInitialized(DevStudio::HlaSensorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string sensorName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << sensorName << std::endl;

    // Wait until EntityIdentifier is set (for consistency)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaSensorImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSensorImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSensorAttributesPtr attributes = attributesimpl->getHlaSensorAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SensorObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SensorObjectEnum);
        }
    }

    entity->addHlaSensorListener(shared_from_this());
}
void SDLSensorManagerListener::attributesUpdated(DevStudio::HlaSensorPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSensorAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaSensorImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSensorImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSensorAttributesPtr attributesSensor = attributesimpl->getHlaSensorAttributes();

        EventOnDataAvailableObject data;

        if (attributesSensor)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesSensor);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SensorObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SensorObjectEnum);
        }
    }
}

void SDLSuppliesManagerListener::hlaSuppliesDiscovered(DevStudio::HlaSuppliesPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered new supplies: " << entity->getHlaInstanceName() << std::endl;
}
void SDLSuppliesManagerListener::hlaSuppliesDeleted(DevStudio::HlaSuppliesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted supplies: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaSuppliesImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSuppliesImpl>(entity);
    auto attributes = attributesimpl ? attributesimpl->getHlaSuppliesAttributes() : nullptr;

    if (attributesimpl && attributes) {
        auto updater = attributesimpl->getHlaSuppliesUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::SuppliesObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SuppliesObjectEnum);
    }
}
void SDLSuppliesManagerListener::hlaSuppliesInitialized(DevStudio::HlaSuppliesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string suppliesName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << suppliesName << std::endl;

    // Wait until EntityIdentifier is set (for consistency)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaSuppliesImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSuppliesImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSuppliesAttributesPtr attributes = attributesimpl->getHlaSuppliesAttributes();
        EventOnDataAvailableObject data;
        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SuppliesObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SuppliesObjectEnum);
        }
    }

    entity->addHlaSuppliesListener(shared_from_this());
}
void SDLSuppliesManagerListener::attributesUpdated(DevStudio::HlaSuppliesPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSuppliesAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaSuppliesImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaSuppliesImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaSuppliesAttributesPtr attributesSupplies = attributesimpl->getHlaSuppliesAttributes();

        EventOnDataAvailableObject data;

        if (attributesSupplies)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesSupplies);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::SuppliesObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::SuppliesObjectEnum);
        }
    }
}

void SDLEmbeddedSystemManagerListener::hlaEmbeddedSystemDiscovered(DevStudio::HlaEmbeddedSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new embedded system: " << entity->getHlaInstanceName() << std::endl;
}
void SDLEmbeddedSystemManagerListener::hlaEmbeddedSystemDeleted(DevStudio::HlaEmbeddedSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted embedded system: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaEmbeddedSystemImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmbeddedSystemImpl>(entity);
    DevStudio::HlaEmbeddedSystemAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaEmbeddedSystemAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaEmbeddedSystemUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum);
    }
}
void SDLEmbeddedSystemManagerListener::hlaEmbeddedSystemInitialized(DevStudio::HlaEmbeddedSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string embeddedName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << embeddedName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaEmbeddedSystemImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmbeddedSystemImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEmbeddedSystemAttributesPtr attributes = attributesimpl->getHlaEmbeddedSystemAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum);
        }
    }

    entity->addHlaEmbeddedSystemListener(shared_from_this());
}
void SDLEmbeddedSystemManagerListener::attributesUpdated(DevStudio::HlaEmbeddedSystemPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEmbeddedSystemAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaEmbeddedSystemImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmbeddedSystemImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEmbeddedSystemAttributesPtr attributesEmbedded = attributesimpl->getHlaEmbeddedSystemAttributes();

        EventOnDataAvailableObject data;

        if (attributesEmbedded)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesEmbedded);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmbeddedSystemObjectEnum);
        }
    }
}

void SDLRadioTransmitterManagerListener::hlaRadioTransmitterDiscovered(DevStudio::HlaRadioTransmitterPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new radio transmitter: " << entity->getHlaInstanceName() << std::endl;
}
void SDLRadioTransmitterManagerListener::hlaRadioTransmitterDeleted(DevStudio::HlaRadioTransmitterPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted radio transmitter: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaRadioTransmitterImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioTransmitterImpl>(entity);
    DevStudio::HlaRadioTransmitterAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaRadioTransmitterAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaRadioTransmitterUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioTransmitterObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioTransmitterObjectEnum);
    }
}
void SDLRadioTransmitterManagerListener::hlaRadioTransmitterInitialized(DevStudio::HlaRadioTransmitterPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string radioName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << radioName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaRadioTransmitterImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioTransmitterImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadioTransmitterAttributesPtr attributes = attributesimpl->getHlaRadioTransmitterAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioTransmitterObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioTransmitterObjectEnum);
        }
    }

    entity->addHlaRadioTransmitterListener(shared_from_this());
}
void SDLRadioTransmitterManagerListener::attributesUpdated(DevStudio::HlaRadioTransmitterPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadioTransmitterAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaRadioTransmitterImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioTransmitterImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadioTransmitterAttributesPtr attributesTransmitter = attributesimpl->getHlaRadioTransmitterAttributes();

        EventOnDataAvailableObject data;

        if (attributesTransmitter)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesTransmitter);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioTransmitterObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioTransmitterObjectEnum);
        }
    }
}

void SDLRadioReceiverManagerListener::hlaRadioReceiverDiscovered(DevStudio::HlaRadioReceiverPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new radio receiver: " << entity->getHlaInstanceName() << std::endl;
}
void SDLRadioReceiverManagerListener::hlaRadioReceiverDeleted(DevStudio::HlaRadioReceiverPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted radio receiver: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaRadioReceiverImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioReceiverImpl>(entity);
    DevStudio::HlaRadioReceiverAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaRadioReceiverAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaRadioReceiverUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioReceiverObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioReceiverObjectEnum);
    }
}
void SDLRadioReceiverManagerListener::hlaRadioReceiverInitialized(DevStudio::HlaRadioReceiverPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string radioName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << radioName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaRadioReceiverImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioReceiverImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadioReceiverAttributesPtr attributes = attributesimpl->getHlaRadioReceiverAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioReceiverObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioReceiverObjectEnum);
        }
    }

    entity->addHlaRadioReceiverListener(shared_from_this());
}
void SDLRadioReceiverManagerListener::attributesUpdated(DevStudio::HlaRadioReceiverPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadioReceiverAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaRadioReceiverImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadioReceiverImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadioReceiverAttributesPtr attributesReceiver = attributesimpl->getHlaRadioReceiverAttributes();

        EventOnDataAvailableObject data;

        if (attributesReceiver)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesReceiver);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadioReceiverObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadioReceiverObjectEnum);
        }
    }
}

void SDLDesignatorManagerListener::hlaDesignatorDiscovered(DevStudio::HlaDesignatorPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new designator: " << entity->getHlaInstanceName() << std::endl;
}
void SDLDesignatorManagerListener::hlaDesignatorInitialized(DevStudio::HlaDesignatorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string designatorName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << designatorName << std::endl;



    DevStudio::HlaDesignatorPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaDesignatorImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaDesignatorAttributesPtr attributes = attributesimpl->getHlaDesignatorAttributes();
        //std::cout << attributes->getAfterburnerOn() << std::endl;
        EventOnDataAvailableObject data;

        if (attributes)
        {
            auto attributesCopy = attributes;

            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            //data.setObjectObject(enumsCommon_Client::ObjectEnum::DesignatorObjectEnum);
            //events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::DesignatorObjectEnum);
        }
    }


    // entity->addHlaAircraftListener(shared_from_this());
}
void SDLDesignatorManagerListener::hlaDesignatorDeleted(DevStudio::HlaDesignatorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted designator: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaDesignatorImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaDesignatorImpl>(entity);
    DevStudio::HlaDesignatorAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaDesignatorAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaDesignatorUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::DesignatorObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::DesignatorObjectEnum);
    }
}
void SDLDesignatorManagerListener::attributesUpdated(DevStudio::HlaDesignatorPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaDesignatorAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaDesignatorImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaDesignatorImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaDesignatorAttributesPtr attributesDesignator = attributesimpl->getHlaDesignatorAttributes();

        EventOnDataAvailableObject data;

        if (attributesDesignator)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesDesignator);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::DesignatorObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::DesignatorObjectEnum);
        }
    }
}

void SDLEmitterSystemManagerListener::hlaEmitterSystemDiscovered(DevStudio::HlaEmitterSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new emitter system: " << entity->getHlaInstanceName() << std::endl;
}
void SDLEmitterSystemManagerListener::hlaEmitterSystemDeleted(DevStudio::HlaEmitterSystemPtr emitterSystem, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(emitterSystem->getHlaInstanceName());
    std::cout << "Deleted emitter system: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaEmitterSystemImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmitterSystemImpl>(emitterSystem);
    DevStudio::HlaEmitterSystemAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaEmitterSystemAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaEmitterSystemUpdater();

        data.setObjectName(emitterSystem->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
        data.setFederateName(emitterSystem->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
    }
}
void SDLEmitterSystemManagerListener::hlaEmitterSystemInitialized(DevStudio::HlaEmitterSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string designatorName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << designatorName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaEmitterSystemImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmitterSystemImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEmitterSystemAttributesPtr attributes = attributesimpl->getHlaEmitterSystemAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
        }
    }

    entity->addHlaEmitterSystemListener(shared_from_this());
}
void SDLEmitterSystemManagerListener::attributesUpdated(DevStudio::HlaEmitterSystemPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEmitterSystemAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated " << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaEmitterSystemImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmitterSystemImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEmitterSystemAttributesPtr attributesSys = attributesimpl->getHlaEmitterSystemAttributes();

        EventOnDataAvailableObject data;

        if (attributesSys)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesSys);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmitterSystemObjectEnum);
        }
    }
}

void SDLIFFManagerListener::hlaIFFDiscovered(DevStudio::HlaIFFPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new IFF system: " << entity->getHlaInstanceName() << std::endl;
}
void SDLIFFManagerListener::hlaIFFDeleted(DevStudio::HlaIFFPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted IFF: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaIFFImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaIFFImpl>(entity);
    DevStudio::HlaIFFAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaIFFAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaIFFUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::IFFObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::IFFObjectEnum);
    }
}
void SDLIFFManagerListener::hlaIFFInitialized(DevStudio::HlaIFFPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string iffName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << iffName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaIFFImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaIFFImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaIFFAttributesPtr attributes = attributesimpl->getHlaIFFAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::IFFObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::IFFObjectEnum);
        }
    }

    entity->addHlaIFFListener(shared_from_this());
}
void SDLIFFManagerListener::attributesUpdated(DevStudio::HlaIFFPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaIFFAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaIFFImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaIFFImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaIFFAttributesPtr attributesIFF = attributesimpl->getHlaIFFAttributes();

        EventOnDataAvailableObject data;

        if (attributesIFF)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesIFF);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::IFFObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::IFFObjectEnum);
        }
    }
}

void SDLRRBManagerListener::hlaRRBDiscovered(DevStudio::HlaRRBPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new RRB: " << entity->getHlaInstanceName() << std::endl;
}
void SDLRRBManagerListener::hlaRRBDeleted(DevStudio::HlaRRBPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted RRB: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaRRBImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRRBImpl>(entity);
    DevStudio::HlaRRBAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaRRBAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaRRBUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::RRBObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RRBObjectEnum);
    }
}
void SDLRRBManagerListener::hlaRRBInitialized(DevStudio::HlaRRBPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string rrbName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << rrbName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaRRBImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRRBImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRRBAttributesPtr attributes = attributesimpl->getHlaRRBAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RRBObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RRBObjectEnum);
        }
    }

    entity->addHlaRRBListener(shared_from_this());
}
void SDLRRBManagerListener::attributesUpdated(DevStudio::HlaRRBPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRRBAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaRRBImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRRBImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRRBAttributesPtr attributesRRB = attributesimpl->getHlaRRBAttributes();

        EventOnDataAvailableObject data;

        if (attributesRRB)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesRRB);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RRBObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RRBObjectEnum);
        }
    }
}

void SDLMinefieldDataManagerListener::hlaMinefieldDataDiscovered(DevStudio::HlaMinefieldDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new minefield data: " << entity->getHlaInstanceName() << std::endl;
}
void SDLMinefieldDataManagerListener::hlaMinefieldDataDeleted(DevStudio::HlaMinefieldDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted minefield data: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaMinefieldDataImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldDataImpl>(entity);
    DevStudio::HlaMinefieldDataAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaMinefieldDataAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaMinefieldDataUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldDataObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldDataObjectEnum);
    }
}
void SDLMinefieldDataManagerListener::hlaMinefieldDataInitialized(DevStudio::HlaMinefieldDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string minefieldName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << minefieldName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasEntityIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaMinefieldDataImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldDataImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMinefieldDataAttributesPtr attributes = attributesimpl->getHlaMinefieldDataAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldDataObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldDataObjectEnum);
        }
    }

    entity->addHlaMinefieldDataListener(shared_from_this());
}
void SDLMinefieldDataManagerListener::attributesUpdated(DevStudio::HlaMinefieldDataPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMinefieldDataAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaMinefieldDataImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldDataImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMinefieldDataAttributesPtr attributesData = attributesimpl->getHlaMinefieldDataAttributes();

        EventOnDataAvailableObject data;

        if (attributesData)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesData);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldDataObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldDataObjectEnum);
        }
    }
}

void SDLEnvironmentObjectManagerListener::hlaEnvironmentObjectDiscovered(DevStudio::HlaEnvironmentObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new environment object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLEnvironmentObjectManagerListener::hlaEnvironmentObjectDeleted(DevStudio::HlaEnvironmentObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted environment object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaEnvironmentObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEnvironmentObjectImpl>(entity);
    DevStudio::HlaEnvironmentObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaEnvironmentObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaEnvironmentObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::EnvironmentObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EnvironmentObjectObjectEnum);
    }
}
void SDLEnvironmentObjectManagerListener::hlaEnvironmentObjectInitialized(DevStudio::HlaEnvironmentObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string environmentName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << environmentName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaEnvironmentObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEnvironmentObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEnvironmentObjectAttributesPtr attributes = attributesimpl->getHlaEnvironmentObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EnvironmentObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EnvironmentObjectObjectEnum);
        }
    }

    entity->addHlaEnvironmentObjectListener(shared_from_this());
}
void SDLEnvironmentObjectManagerListener::attributesUpdated(DevStudio::HlaEnvironmentObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEnvironmentObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaEnvironmentObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEnvironmentObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEnvironmentObjectAttributesPtr attributesEnv = attributesimpl->getHlaEnvironmentObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesEnv)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesEnv);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EnvironmentObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EnvironmentObjectObjectEnum);
        }
    }
}

void SDLArealObjectManagerListener::hlaArealObjectDiscovered(DevStudio::HlaArealObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new areal object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLArealObjectManagerListener::hlaArealObjectDeleted(DevStudio::HlaArealObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted areal object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaArealObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaArealObjectImpl>(entity);
    DevStudio::HlaArealObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaArealObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaArealObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::ArealObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::ArealObjectObjectEnum);
    }
}
void SDLArealObjectManagerListener::hlaArealObjectInitialized(DevStudio::HlaArealObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string arealName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << arealName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaArealObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaArealObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaArealObjectAttributesPtr attributes = attributesimpl->getHlaArealObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::ArealObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::ArealObjectObjectEnum);
        }
    }

    entity->addHlaArealObjectListener(shared_from_this());
}
void SDLArealObjectManagerListener::attributesUpdated(DevStudio::HlaArealObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaArealObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaArealObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaArealObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaArealObjectAttributesPtr attributesAreal = attributesimpl->getHlaArealObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesAreal)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesAreal);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::ArealObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::ArealObjectObjectEnum);
        }
    }
}

void SDLMinefieldObjectManagerListener::hlaMinefieldObjectDiscovered(DevStudio::HlaMinefieldObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new minefield object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLMinefieldObjectManagerListener::hlaMinefieldObjectDeleted(DevStudio::HlaMinefieldObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted minefield object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaMinefieldObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldObjectImpl>(entity);
    DevStudio::HlaMinefieldObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaMinefieldObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaMinefieldObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldObjectObjectEnum);
    }
}
void SDLMinefieldObjectManagerListener::hlaMinefieldObjectInitialized(DevStudio::HlaMinefieldObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string minefieldName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << minefieldName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaMinefieldObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMinefieldObjectAttributesPtr attributes = attributesimpl->getHlaMinefieldObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldObjectObjectEnum);
        }
    }

    entity->addHlaMinefieldObjectListener(shared_from_this());
}
void SDLMinefieldObjectManagerListener::attributesUpdated(DevStudio::HlaMinefieldObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMinefieldObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaMinefieldObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMinefieldObjectAttributesPtr attributesMinefield = attributesimpl->getHlaMinefieldObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesMinefield)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesMinefield);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldObjectObjectEnum);
        }
    }
}

void SDLLinearObjectManagerListener::hlaLinearObjectDiscovered(DevStudio::HlaLinearObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new linear object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLLinearObjectManagerListener::hlaLinearObjectDeleted(DevStudio::HlaLinearObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted linear object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaLinearObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaLinearObjectImpl>(entity);
    DevStudio::HlaLinearObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaLinearObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaLinearObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::LinearObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::LinearObjectObjectEnum);
    }
}
void SDLLinearObjectManagerListener::hlaLinearObjectInitialized(DevStudio::HlaLinearObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string linearName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << linearName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaLinearObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaLinearObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaLinearObjectAttributesPtr attributes = attributesimpl->getHlaLinearObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::LinearObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::LinearObjectObjectEnum);
        }
    }

    entity->addHlaLinearObjectListener(shared_from_this());
}
void SDLLinearObjectManagerListener::attributesUpdated(DevStudio::HlaLinearObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaLinearObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaLinearObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaLinearObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaLinearObjectAttributesPtr attributesLinear = attributesimpl->getHlaLinearObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesLinear)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesLinear);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::LinearObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::LinearObjectObjectEnum);
        }
    }
}

void SDLPointObjectManagerListener::hlaPointObjectDiscovered(DevStudio::HlaPointObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new point object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLPointObjectManagerListener::hlaPointObjectDeleted(DevStudio::HlaPointObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted point object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaPointObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPointObjectImpl>(entity);
    DevStudio::HlaPointObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaPointObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaPointObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::PointObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PointObjectObjectEnum);
    }
}
void SDLPointObjectManagerListener::hlaPointObjectInitialized(DevStudio::HlaPointObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string pointName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << pointName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaPointObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPointObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaPointObjectAttributesPtr attributes = attributesimpl->getHlaPointObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::PointObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PointObjectObjectEnum);
        }
    }

    entity->addHlaPointObjectListener(shared_from_this());
}
void SDLPointObjectManagerListener::attributesUpdated(DevStudio::HlaPointObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaPointObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaPointObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaPointObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaPointObjectAttributesPtr attributesPoint = attributesimpl->getHlaPointObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesPoint)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesPoint);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::PointObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::PointObjectObjectEnum);
        }
    }
}

void SDLRibbonBridgeObjectManagerListener::hlaRibbonBridgeObjectDiscovered(DevStudio::HlaRibbonBridgeObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new ribbon bridge object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLRibbonBridgeObjectManagerListener::hlaRibbonBridgeObjectDeleted(DevStudio::HlaRibbonBridgeObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted ribbon bridge object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaRibbonBridgeObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRibbonBridgeObjectImpl>(entity);
    DevStudio::HlaRibbonBridgeObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaRibbonBridgeObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaRibbonBridgeObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::RibbonBridgeObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RibbonBridgeObjectObjectEnum);
    }
}
void SDLRibbonBridgeObjectManagerListener::hlaRibbonBridgeObjectInitialized(DevStudio::HlaRibbonBridgeObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string ribbonName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << ribbonName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaRibbonBridgeObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRibbonBridgeObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRibbonBridgeObjectAttributesPtr attributes = attributesimpl->getHlaRibbonBridgeObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RibbonBridgeObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RibbonBridgeObjectObjectEnum);
        }
    }

    entity->addHlaRibbonBridgeObjectListener(shared_from_this());
}
void SDLRibbonBridgeObjectManagerListener::attributesUpdated(DevStudio::HlaRibbonBridgeObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRibbonBridgeObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaRibbonBridgeObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRibbonBridgeObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRibbonBridgeObjectAttributesPtr attributesRibbon = attributesimpl->getHlaRibbonBridgeObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesRibbon)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesRibbon);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RibbonBridgeObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RibbonBridgeObjectObjectEnum);
        }
    }
}

void SDLStructureObjectManagerListener::hlaStructureObjectDiscovered(DevStudio::HlaStructureObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new structure object: " << entity->getHlaInstanceName() << std::endl;
}
void SDLStructureObjectManagerListener::hlaStructureObjectDeleted(DevStudio::HlaStructureObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted structure object: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaStructureObjectImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaStructureObjectImpl>(entity);
    DevStudio::HlaStructureObjectAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaStructureObjectAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaStructureObjectUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::StructureObjectObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::StructureObjectObjectEnum);
    }
}
void SDLStructureObjectManagerListener::hlaStructureObjectInitialized(DevStudio::HlaStructureObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string structureName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << structureName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasObjectIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaStructureObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaStructureObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaStructureObjectAttributesPtr attributes = attributesimpl->getHlaStructureObjectAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::StuctureObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::StuctureObjectObjectEnum);
        }
    }

    entity->addHlaStructureObjectListener(shared_from_this());
}
void SDLStructureObjectManagerListener::attributesUpdated(DevStudio::HlaStructureObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaStructureObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaStructureObjectImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaStructureObjectImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaStructureObjectAttributesPtr attributesStruct = attributesimpl->getHlaStructureObjectAttributes();

        EventOnDataAvailableObject data;

        if (attributesStruct)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesStruct);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::StructureObjectObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::StructureObjectObjectEnum);
        }
    }
}

void SDLEnvironmentProcessManagerListener::hlaEnvironmentProcessDiscovered(DevStudio::HlaEnvironmentProcessPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new environment process: " << entity->getHlaInstanceName() << std::endl;
}
void SDLEnvironmentProcessManagerListener::hlaEnvironmentProcessDeleted(DevStudio::HlaEnvironmentProcessPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted environment process: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaEnvironmentProcessImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEnvironmentProcessImpl>(entity);
    DevStudio::HlaEnvironmentProcessAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaEnvironmentProcessAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaEnvironmentProcessUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::EnvironmentProcessObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EnvironmentProcessObjectEnum);
    }
}
void SDLEnvironmentProcessManagerListener::hlaEnvironmentProcessInitialized(DevStudio::HlaEnvironmentProcessPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string environmentName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << environmentName << std::endl;

    // Wait until EntityIdentifier is set (if available)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasProcessIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaEnvironmentProcessImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEnvironmentProcessImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEnvironmentProcessAttributesPtr attributes = attributesimpl->getHlaEnvironmentProcessAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EnvironmentProcessObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EnvironmentProcessObjectEnum);
        }
    }

    entity->addHlaEnvironmentProcessListener(shared_from_this());
}
void SDLEnvironmentProcessManagerListener::attributesUpdated(DevStudio::HlaEnvironmentProcessPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEnvironmentProcessAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaEnvironmentProcessImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEnvironmentProcessImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEnvironmentProcessAttributesPtr attributesProcess = attributesimpl->getHlaEnvironmentProcessAttributes();

        EventOnDataAvailableObject data;

        if (attributesProcess)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesProcess);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EnvironmentProcessObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EnvironmentProcessObjectEnum);
        }
    }
}

void SDLGriddedDataManagerListener::hlaGriddedDataDiscovered(DevStudio::HlaGriddedDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered new gridded data: " << entity->getHlaInstanceName() << std::endl;
}
void SDLGriddedDataManagerListener::hlaGriddedDataDeleted(DevStudio::HlaGriddedDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted gridded data: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaGriddedDataImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaGriddedDataImpl>(entity);
    DevStudio::HlaGriddedDataAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaGriddedDataAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaGriddedDataUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::GriddedDataObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::GriddedDataObjectEnum);
    }
}
void SDLGriddedDataManagerListener::hlaGriddedDataInitialized(DevStudio::HlaGriddedDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string griddedName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << griddedName << std::endl;

    // Wait until the identifier is available (if applicable)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasGridIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaGriddedDataImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaGriddedDataImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaGriddedDataAttributesPtr attributes = attributesimpl->getHlaGriddedDataAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::GriddedDataObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::GriddedDataObjectEnum);
        }
    }

    entity->addHlaGriddedDataListener(shared_from_this());
}
void SDLGriddedDataManagerListener::attributesUpdated(DevStudio::HlaGriddedDataPtr entity,const DevStudio::HlaEnumSet<DevStudio::HlaGriddedDataAttributes::Attribute>& attributes,DevStudio::HlaTimeStampPtr timeStamp,DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::cout << "Updated" << SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName()) << std::endl;

    DevStudio::HlaGriddedDataImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaGriddedDataImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaGriddedDataAttributesPtr attributesGridded = attributesimpl->getHlaGriddedDataAttributes();

        EventOnDataAvailableObject data;

        if (attributesGridded)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesGridded);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::GriddedDataObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::GriddedDataObjectEnum);
        }
    }
}

void SDLEmitterBeamManagerListener::hlaEmitterBeamDiscovered(DevStudio::HlaEmitterBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new emitter beam: " << entity->getHlaInstanceName() << std::endl;
}
void SDLEmitterBeamManagerListener::hlaEmitterBeamDeleted(DevStudio::HlaEmitterBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted an emitter beam: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaEmitterBeamImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmitterBeamImpl>(entity);
    DevStudio::HlaEmitterBeamAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaEmitterBeamAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaEmitterBeamUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
    }
}
void SDLEmitterBeamManagerListener::hlaEmitterBeamInitialized(DevStudio::HlaEmitterBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string emitterBeamName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << emitterBeamName << std::endl;

    // Wait until BeamIdentifier is available (if applicable)
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasBeamIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaEmitterBeamImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmitterBeamImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEmitterBeamAttributesPtr attributes = attributesimpl->getHlaEmitterBeamAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
        }
    }

    entity->addHlaEmitterBeamListener(shared_from_this());
}
void SDLEmitterBeamManagerListener::attributesUpdated(DevStudio::HlaEmitterBeamPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEmitterBeamAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaEmitterBeamImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaEmitterBeamImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaEmitterBeamAttributesPtr attributesBeam = attributesimpl->getHlaEmitterBeamAttributes();

        EventOnDataAvailableObject data;

        if (attributesBeam)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesBeam);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::EmitterBeamObjectEnum);
        }
    }
}

void SDLRadarBeamManagerListener::hlaRadarBeamDiscovered(DevStudio::HlaRadarBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    if (entity) {

        std::wcout << L"Discovered a new radar beam: " << entity->getHlaInstanceName() << std::endl;
    }
}
void SDLRadarBeamManagerListener::hlaRadarBeamDeleted(DevStudio::HlaRadarBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a radar beam: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaRadarBeamImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadarBeamImpl>(entity);
    DevStudio::HlaRadarBeamAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaRadarBeamAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaRadarBeamUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
    }
}
void SDLRadarBeamManagerListener::hlaRadarBeamInitialized(DevStudio::HlaRadarBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until BeamIdentifier is available
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasBeamIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        });
    future.get();

    DevStudio::HlaRadarBeamImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadarBeamImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadarBeamAttributesPtr attributes = attributesimpl->getHlaRadarBeamAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
        }
    }

    entity->addHlaRadarBeamListener(shared_from_this());
}
void SDLRadarBeamManagerListener::attributesUpdated(DevStudio::HlaRadarBeamPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadarBeamAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaRadarBeamImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaRadarBeamImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaRadarBeamAttributesPtr attributesRadarBeam = attributesimpl->getHlaRadarBeamAttributes();

        EventOnDataAvailableObject data;

        if (attributesRadarBeam)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesRadarBeam);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::RadarBeamObjectEnum);
        }
    }
}

void SDLJammerBeamManagerListener::hlaJammerBeamDiscovered(DevStudio::HlaJammerBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    if (entity) {
        std::wcout << L"Discovered a new jammer beam: " << entity->getHlaInstanceName() << std::endl;
    }
}
void SDLJammerBeamManagerListener::hlaJammerBeamDeleted(DevStudio::HlaJammerBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a jammer beam: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaJammerBeamImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaJammerBeamImpl>(entity);
    DevStudio::HlaJammerBeamAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaJammerBeamAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaJammerBeamUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
    }
}
void SDLJammerBeamManagerListener::hlaJammerBeamInitialized(DevStudio::HlaJammerBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until BeamIdentifier is available
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasBeamIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        });
    future.get();
    
    std::string jammerBeamName = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Initialized " << jammerBeamName << std::endl;
    
    DevStudio::HlaJammerBeamImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaJammerBeamImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaJammerBeamAttributesPtr attributes = attributesimpl->getHlaJammerBeamAttributes();
        EventOnDataAvailableObject data;
    
        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
        }
    }
    
    entity->addHlaJammerBeamListener(shared_from_this());
}
void SDLJammerBeamManagerListener::attributesUpdated(DevStudio::HlaJammerBeamPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaJammerBeamAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaJammerBeamImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaJammerBeamImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaJammerBeamAttributesPtr attributesJammerBeam = attributesimpl->getHlaJammerBeamAttributes();

        EventOnDataAvailableObject data;

        if (attributesJammerBeam)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesJammerBeam);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::JammerBeamObjectEnum);
        }
    }
}

void SDLMinefieldManagerListener::hlaMinefieldDiscovered(DevStudio::HlaMinefieldPtr entity, DevStudio::HlaTimeStampPtr timeStamp)
{
    std::wcout << L"Discovered a new minefield: " << entity->getHlaInstanceName() << std::endl;
}
void SDLMinefieldManagerListener::hlaMinefieldDeleted(DevStudio::HlaMinefieldPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    std::string name = SDL_RTI::StringConverter::tostr(entity->getHlaInstanceName());
    std::cout << "Deleted a minefield: " << name << std::endl;

    EventOnDataAvailableObject data;
    std::shared_ptr<DevStudio::HlaMinefieldImpl> attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldImpl>(entity);
    DevStudio::HlaMinefieldAttributesPtr attributes = attributesimpl ? attributesimpl->getHlaMinefieldAttributes() : nullptr;

    if (attributesimpl && attributes)
    {
        auto updater = attributesimpl->getHlaMinefieldUpdater();

        data.setObjectName(entity->getHlaInstanceName());
        data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::RemoveObject));
        data.setEventOnDataAvailableObject(attributes);
        data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldObjectEnum);
        data.setFederateName(entity->getProducingFederate());
        events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldObjectEnum);
    }
}
void SDLMinefieldManagerListener::hlaMinefieldInitialized(DevStudio::HlaMinefieldPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    // Wait until BeamIdentifier is available
    auto future = std::async(std::launch::async, [entity]() {
        while (!entity->hasMinefieldIdentifier()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });
    future.get();

    DevStudio::HlaMinefieldImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMinefieldAttributesPtr attributes = attributesimpl->getHlaMinefieldAttributes();
        EventOnDataAvailableObject data;

        if (attributes) {
            auto attributesCopy = attributes;
            data.setObjectName(entity->getHlaInstanceName());
            std::wcout << L"Instance name: " << data.getObjectName() << std::endl;
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::AddObject));
            data.setEventOnDataAvailableObject(attributes);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldObjectEnum);
        }
    }

    entity->addHlaMinefieldListener(shared_from_this());
}
void SDLMinefieldManagerListener::attributesUpdated(DevStudio::HlaMinefieldPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMinefieldAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime)
{
    DevStudio::HlaMinefieldImplPtr attributesimpl = std::dynamic_pointer_cast<DevStudio::HlaMinefieldImpl>(entity);
    if (attributesimpl) {
        DevStudio::HlaMinefieldAttributesPtr attributesMinefield = attributesimpl->getHlaMinefieldAttributes();

        EventOnDataAvailableObject data;

        if (attributesMinefield)
        {
            data.setActionObject(static_cast<int>(enumsCommon_Client::ActionObjectEnum::UpdateObject));
            data.setEventOnDataAvailableObject(attributesMinefield);
            data.setObjectObject(enumsCommon_Client::ObjectEnum::MinefieldObjectEnum);
            data.setFederateName(entity->getProducingFederate());
            for (auto attribute : attributes)
            {
                data.addModifiedAttribute(attribute);
            }
            events::EventBus::getInstance().dispatchEvent(data, enumsCommon_Client::ObjectEnum::MinefieldObjectEnum);
        }
    }
}

