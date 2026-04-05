#pragma once
#include <DevStudio/HlaPhysicalEntityManager.h>
#include <DevStudio/HlaBaseEntityManager.h>
#include <DevStudio/HlaPlatformManager.h>
#include <DevStudio/HlaAircraftManager.h>
#include <DevStudio/HlaAmphibiousVehicleManager.h>
#include <DevStudio/HlaGroundVehicleManager.h>
#include <DevStudio/HlaMultiDomainPlatformManager.h>
#include <DevStudio/HlaSpacecraftManager.h>
#include <DevStudio/HlaSubmersibleVesselManager.h>
#include <DevStudio/HlaSurfaceVesselManager.h>
#include <DevStudio/HlaLifeformManager.h>
#include <DevStudio/HlaHumanManager.h>
#include <DevStudio/HlaNonHumanManager.h>
#include <DevStudio/HlaCulturalFeatureManager.h>
#include <DevStudio/HlaMunitionManager.h>
#include <DevStudio/HlaExpendablesManager.h>
#include <DevStudio/HlaRadioManager.h>
#include <DevStudio/HlaSensorManager.h>
#include <DevStudio/HlaSuppliesManager.h>
#include <DevStudio/HlaEmbeddedSystemManager.h>
#include <DevStudio/HlaRadioTransmitterManager.h>
#include <DevStudio/HlaRadioReceiverManager.h>
#include <DevStudio/HlaDesignatorManager.h>
#include <DevStudio/HlaEmitterSystemManager.h>
#include <DevStudio/HlaIFFManager.h>
#include <DevStudio/HlaRRBManager.h>
#include <DevStudio/HlaMinefieldDataManager.h>
#include <DevStudio/HlaEnvironmentObjectManager.h>
#include <DevStudio/HlaArealObjectManager.h>
#include <DevStudio/HlaMinefieldObjectManager.h>
#include <DevStudio/HlaLinearObjectManager.h>
#include <DevStudio/HlaPointObjectManager.h>
#include <DevStudio/HlaRibbonBridgeObjectManager.h>
#include <DevStudio/HlaStructureObjectManager.h>
#include <DevStudio/HlaEnvironmentProcessManager.h>
#include <DevStudio/HlaGriddedDataManager.h>
#include <DevStudio/HlaEmitterBeamManager.h>
#include <DevStudio/HlaRadarBeamManager.h>
#include <DevStudio/HlaJammerBeamManager.h>
#include <DevStudio/HlaMinefieldManager.h>
#include "DataTypes/BaseEntityDt.h"
#include "PerformanceMeter.h"
#include <DevStudio/HlaHLAfederateManager.h>

class SDLFederateManagerListener : public DevStudio::HlaHLAfederateManagerListener::Adapter, public DevStudio::HlaHLAfederateListener::Adapter, public std::enable_shared_from_this<SDLFederateManagerListener>
{
public:
    void hlaHLAfederateDiscovered(DevStudio::HlaHLAfederatePtr hLAfederate, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaHLAfederateInitialized(DevStudio::HlaHLAfederatePtr hLAfederate, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaHLAfederateDeleted(DevStudio::HlaHLAfederatePtr hLAfederate, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaHLAfederatePtr hLAfederate, const DevStudio::HlaEnumSet<DevStudio::HlaHLAfederateAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class  SDLPhysicalEntityManagerListener : public DevStudio::HlaPhysicalEntityManagerListener::Adapter, public DevStudio::HlaPhysicalEntityListener::Adapter, public std::enable_shared_from_this<SDLPhysicalEntityManagerListener> {
public:
    void hlaPhysicalEntityDiscovered(DevStudio::HlaPhysicalEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaPhysicalEntityDeleted(DevStudio::HlaPhysicalEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaPhysicalEntityInitialized(DevStudio::HlaPhysicalEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaPhysicalEntityPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaPhysicalEntityAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;


};
class  SDLBaseEntityManagerListener : public DevStudio::HlaBaseEntityManagerListener::Adapter, public DevStudio::HlaBaseEntityListener::Adapter, public std::enable_shared_from_this<SDLBaseEntityManagerListener> {
public:
    void hlaBaseEntityDiscovered(DevStudio::HlaBaseEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaBaseEntityDeleted(DevStudio::HlaBaseEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaBaseEntityInitialized(DevStudio::HlaBaseEntityPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaBaseEntityPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaBaseEntityAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;


};
class  SDLPlatformManagerListener : public DevStudio::HlaPlatformManagerListener::Adapter, public DevStudio::HlaPlatformListener::Adapter, public std::enable_shared_from_this<SDLPlatformManagerListener> {
public:
    void hlaPlatformDiscovered(DevStudio::HlaPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaPlatformDeleted(DevStudio::HlaPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaPlatformInitialized(DevStudio::HlaPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaPlatformPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaPlatformAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;



};
class  SDLAircraftManagerListener : public DevStudio::HlaAircraftManagerListener::Adapter, public DevStudio::HlaAircraftListener::Adapter, public std::enable_shared_from_this<SDLAircraftManagerListener> {
public:
    SDLAircraftManagerListener()
      : perf_("aircraft_updates.csv") {}

    void hlaAircraftDiscovered(DevStudio::HlaAircraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaAircraftDeleted(DevStudio::HlaAircraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

    void hlaAircraftInitialized(DevStudio::HlaAircraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaAircraftPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaAircraftAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
private:
    PerfMeter perf_;
};
class  SDLAmphibiousVehicleManagerListener : public DevStudio::HlaAmphibiousVehicleManagerListener::Adapter , public DevStudio::HlaAmphibiousVehicleListener::Adapter, public std::enable_shared_from_this<SDLAmphibiousVehicleManagerListener> {
public:
    void hlaAmphibiousVehicleDiscovered(DevStudio::HlaAmphibiousVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaAmphibiousVehicleDeleted(DevStudio::HlaAmphibiousVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

    void hlaAmphibiousVehicleInitialized(DevStudio::HlaAmphibiousVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaAmphibiousVehiclePtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaAmphibiousVehicleAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLGroundVehicleManagerListener : public DevStudio::HlaGroundVehicleManagerListener::Adapter, public DevStudio::HlaGroundVehicleListener::Adapter, public std::enable_shared_from_this<SDLGroundVehicleManagerListener> {
public:
    void hlaGroundVehicleDiscovered(DevStudio::HlaGroundVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaGroundVehicleDeleted(DevStudio::HlaGroundVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaGroundVehicleInitialized(DevStudio::HlaGroundVehiclePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaGroundVehiclePtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaGroundVehicleAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLMultiDomainPlatformManagerListener : public DevStudio::HlaMultiDomainPlatformManagerListener::Adapter, public DevStudio::HlaMultiDomainPlatformListener::Adapter, public std::enable_shared_from_this<SDLMultiDomainPlatformManagerListener> {
public:
    void hlaMultiDomainPlatformDiscovered(DevStudio::HlaMultiDomainPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaMultiDomainPlatformDeleted(DevStudio::HlaMultiDomainPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaMultiDomainPlatformInitialized(DevStudio::HlaMultiDomainPlatformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaMultiDomainPlatformPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMultiDomainPlatformAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLSpacecraftManagerListener : public DevStudio::HlaSpacecraftManagerListener::Adapter, public DevStudio::HlaSpacecraftListener::Adapter, public std::enable_shared_from_this<SDLSpacecraftManagerListener> {
public:
    void hlaSpacecraftDiscovered(DevStudio::HlaSpacecraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaSpacecraftInitialized(DevStudio::HlaSpacecraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaSpacecraftDeleted(DevStudio::HlaSpacecraftPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaSpacecraftPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSpacecraftAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLSubmersibleVesselManagerListener : public DevStudio::HlaSubmersibleVesselManagerListener::Adapter, public DevStudio::HlaSubmersibleVesselListener::Adapter, public std::enable_shared_from_this<SDLSubmersibleVesselManagerListener> {
public:
    void hlaSubmersibleVesselDiscovered(DevStudio::HlaSubmersibleVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaSubmersibleVesselInitialized(DevStudio::HlaSubmersibleVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaSubmersibleVesselDeleted(DevStudio::HlaSubmersibleVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaSubmersibleVesselPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSubmersibleVesselAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLSurfaceVesselManagerListener : public DevStudio::HlaSurfaceVesselManagerListener::Adapter, public DevStudio::HlaSurfaceVesselListener::Adapter, public std::enable_shared_from_this<SDLSurfaceVesselManagerListener> {
public:
    void hlaSurfaceVesselDiscovered(DevStudio::HlaSurfaceVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaSurfaceVesselInitialized(DevStudio::HlaSurfaceVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaSurfaceVesselDeleted(DevStudio::HlaSurfaceVesselPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaSurfaceVesselPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSurfaceVesselAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLLifeformManagerListener : public DevStudio::HlaLifeformManagerListener::Adapter, public DevStudio::HlaLifeformListener::Adapter, public std::enable_shared_from_this<SDLLifeformManagerListener> {
public:
    void hlaLifeformDiscovered(DevStudio::HlaLifeformPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaLifeformInitialized(DevStudio::HlaLifeformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaLifeformDeleted(DevStudio::HlaLifeformPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaLifeformPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaLifeformAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLHumanManagerListener : public DevStudio::HlaHumanManagerListener::Adapter, public DevStudio::HlaHumanListener::Adapter, public std::enable_shared_from_this<SDLHumanManagerListener> {
public:
    void hlaHumanDiscovered(DevStudio::HlaHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaHumanInitialized(DevStudio::HlaHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaHumanDeleted(DevStudio::HlaHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaHumanPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaHumanAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class  SDLNonHumanManagerListener : public DevStudio::HlaNonHumanManagerListener::Adapter, public DevStudio::HlaNonHumanListener::Adapter, public std::enable_shared_from_this<SDLNonHumanManagerListener> {
public:
    void hlaNonHumanDiscovered(DevStudio::HlaNonHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaNonHumanInitialized(DevStudio::HlaNonHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaNonHumanDeleted(DevStudio::HlaNonHumanPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaNonHumanPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaNonHumanAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

};
class SDLCulturalFeatureManagerListener : public DevStudio::HlaCulturalFeatureManagerListener::Adapter, public DevStudio::HlaCulturalFeatureListener::Adapter, public std::enable_shared_from_this<SDLCulturalFeatureManagerListener> {
public:
    void hlaCulturalFeatureDiscovered(DevStudio::HlaCulturalFeaturePtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaCulturalFeatureInitialized(DevStudio::HlaCulturalFeaturePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaCulturalFeatureDeleted(DevStudio::HlaCulturalFeaturePtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaCulturalFeaturePtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaCulturalFeatureAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLMunitionManagerListener : public DevStudio::HlaMunitionManagerListener::Adapter, public DevStudio::HlaMunitionListener::Adapter, public std::enable_shared_from_this<SDLMunitionManagerListener> {
public:
    void hlaMunitionDiscovered(DevStudio::HlaMunitionPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaMunitionInitialized(DevStudio::HlaMunitionPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaMunitionDeleted(DevStudio::HlaMunitionPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaMunitionPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMunitionAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLExpendablesManagerListener : public DevStudio::HlaExpendablesManagerListener::Adapter, public DevStudio::HlaExpendablesListener::Adapter, public std::enable_shared_from_this<SDLExpendablesManagerListener> {
public:
    void hlaExpendablesDiscovered(DevStudio::HlaExpendablesPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaExpendablesInitialized(DevStudio::HlaExpendablesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaExpendablesDeleted(DevStudio::HlaExpendablesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaExpendablesPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaExpendablesAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLRadioManagerListener : public DevStudio::HlaRadioManagerListener::Adapter, public DevStudio::HlaRadioListener::Adapter, public std::enable_shared_from_this<SDLRadioManagerListener> {
public:
    void hlaRadioDiscovered(DevStudio::HlaRadioPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaRadioInitialized(DevStudio::HlaRadioPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaRadioDeleted(DevStudio::HlaRadioPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaRadioPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadioAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLSensorManagerListener : public DevStudio::HlaSensorManagerListener::Adapter, public DevStudio::HlaSensorListener::Adapter, public std::enable_shared_from_this<SDLSensorManagerListener> {
public:
    void hlaSensorDiscovered(DevStudio::HlaSensorPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaSensorInitialized(DevStudio::HlaSensorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaSensorDeleted(DevStudio::HlaSensorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaSensorPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSensorAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLSuppliesManagerListener : public DevStudio::HlaSuppliesManagerListener::Adapter, public DevStudio::HlaSuppliesListener::Adapter, public std::enable_shared_from_this<SDLSuppliesManagerListener> {
public:
    void hlaSuppliesDiscovered(DevStudio::HlaSuppliesPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaSuppliesInitialized(DevStudio::HlaSuppliesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaSuppliesDeleted(DevStudio::HlaSuppliesPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaSuppliesPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaSuppliesAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLEmbeddedSystemManagerListener : public DevStudio::HlaEmbeddedSystemManagerListener::Adapter, public DevStudio::HlaEmbeddedSystemListener::Adapter, public std::enable_shared_from_this<SDLEmbeddedSystemManagerListener> {
public:
    void hlaEmbeddedSystemDiscovered(DevStudio::HlaEmbeddedSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaEmbeddedSystemInitialized(DevStudio::HlaEmbeddedSystemPtr embeddedSystem, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaEmbeddedSystemDeleted(DevStudio::HlaEmbeddedSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaEmbeddedSystemPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEmbeddedSystemAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLRadioTransmitterManagerListener : public DevStudio::HlaRadioTransmitterManagerListener::Adapter, public DevStudio::HlaRadioTransmitterListener::Adapter, public std::enable_shared_from_this<SDLRadioTransmitterManagerListener> {
public:
    void hlaRadioTransmitterDiscovered(DevStudio::HlaRadioTransmitterPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaRadioTransmitterInitialized(DevStudio::HlaRadioTransmitterPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaRadioTransmitterDeleted(DevStudio::HlaRadioTransmitterPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaRadioTransmitterPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadioTransmitterAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLRadioReceiverManagerListener : public DevStudio::HlaRadioReceiverManagerListener::Adapter, public DevStudio::HlaRadioReceiverListener::Adapter, public std::enable_shared_from_this<SDLRadioReceiverManagerListener> {
public:
    void hlaRadioReceiverDiscovered(DevStudio::HlaRadioReceiverPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaRadioReceiverInitialized(DevStudio::HlaRadioReceiverPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaRadioReceiverDeleted(DevStudio::HlaRadioReceiverPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaRadioReceiverPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadioReceiverAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLDesignatorManagerListener : public DevStudio::HlaDesignatorManagerListener::Adapter, public DevStudio::HlaDesignatorListener::Adapter, public std::enable_shared_from_this<SDLDesignatorManagerListener> {
public:
    void hlaDesignatorDiscovered(DevStudio::HlaDesignatorPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaDesignatorInitialized(DevStudio::HlaDesignatorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaDesignatorDeleted(DevStudio::HlaDesignatorPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaDesignatorPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaDesignatorAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLEmitterSystemManagerListener : public DevStudio::HlaEmitterSystemManagerListener::Adapter, public DevStudio::HlaEmitterSystemListener::Adapter, public std::enable_shared_from_this<SDLEmitterSystemManagerListener> {
public:
    void hlaEmitterSystemDiscovered(DevStudio::HlaEmitterSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaEmitterSystemDeleted(DevStudio::HlaEmitterSystemPtr emitterSystem, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaEmitterSystemInitialized(DevStudio::HlaEmitterSystemPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaEmitterSystemPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEmitterSystemAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLIFFManagerListener : public DevStudio::HlaIFFManagerListener::Adapter, public DevStudio::HlaIFFListener::Adapter, public std::enable_shared_from_this<SDLIFFManagerListener> {
public:
    void hlaIFFDiscovered(DevStudio::HlaIFFPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaIFFInitialized(DevStudio::HlaIFFPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaIFFDeleted(DevStudio::HlaIFFPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaIFFPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaIFFAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLRRBManagerListener : public DevStudio::HlaRRBManagerListener::Adapter, public DevStudio::HlaRRBListener::Adapter, public std::enable_shared_from_this<SDLRRBManagerListener> {
public:
    void hlaRRBDiscovered(DevStudio::HlaRRBPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaRRBInitialized(DevStudio::HlaRRBPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaRRBDeleted(DevStudio::HlaRRBPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaRRBPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRRBAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLMinefieldDataManagerListener : public DevStudio::HlaMinefieldDataManagerListener::Adapter, public DevStudio::HlaMinefieldDataListener::Adapter, public std::enable_shared_from_this<SDLMinefieldDataManagerListener> {
public:
    void hlaMinefieldDataDiscovered(DevStudio::HlaMinefieldDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaMinefieldDataInitialized(DevStudio::HlaMinefieldDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaMinefieldDataDeleted(DevStudio::HlaMinefieldDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaMinefieldDataPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMinefieldDataAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLEnvironmentObjectManagerListener : public DevStudio::HlaEnvironmentObjectManagerListener::Adapter, public DevStudio::HlaEnvironmentObjectListener::Adapter, public std::enable_shared_from_this<SDLEnvironmentObjectManagerListener> {
public:
    void hlaEnvironmentObjectDiscovered(DevStudio::HlaEnvironmentObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaEnvironmentObjectInitialized(DevStudio::HlaEnvironmentObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaEnvironmentObjectDeleted(DevStudio::HlaEnvironmentObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaEnvironmentObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEnvironmentObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLArealObjectManagerListener : public DevStudio::HlaArealObjectManagerListener::Adapter, public DevStudio::HlaArealObjectListener::Adapter, public std::enable_shared_from_this<SDLArealObjectManagerListener> {
public:
    void hlaArealObjectDiscovered(DevStudio::HlaArealObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaArealObjectInitialized(DevStudio::HlaArealObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaArealObjectDeleted(DevStudio::HlaArealObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaArealObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaArealObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLMinefieldObjectManagerListener : public DevStudio::HlaMinefieldObjectManagerListener::Adapter, public DevStudio::HlaMinefieldObjectListener::Adapter, public std::enable_shared_from_this<SDLMinefieldObjectManagerListener> {
public:
    void hlaMinefieldObjectDiscovered(DevStudio::HlaMinefieldObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaMinefieldObjectInitialized(DevStudio::HlaMinefieldObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaMinefieldObjectDeleted(DevStudio::HlaMinefieldObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaMinefieldObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMinefieldObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLLinearObjectManagerListener : public DevStudio::HlaLinearObjectManagerListener::Adapter, public DevStudio::HlaLinearObjectListener::Adapter, public std::enable_shared_from_this<SDLLinearObjectManagerListener> {
public:
    void hlaLinearObjectDiscovered(DevStudio::HlaLinearObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaLinearObjectInitialized(DevStudio::HlaLinearObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaLinearObjectDeleted(DevStudio::HlaLinearObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaLinearObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaLinearObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLPointObjectManagerListener : public DevStudio::HlaPointObjectManagerListener::Adapter, public DevStudio::HlaPointObjectListener::Adapter, public std::enable_shared_from_this<SDLPointObjectManagerListener> {
public:
    void hlaPointObjectDiscovered(DevStudio::HlaPointObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaPointObjectInitialized(DevStudio::HlaPointObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaPointObjectDeleted(DevStudio::HlaPointObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaPointObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaPointObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};



class SDLRibbonBridgeObjectManagerListener : public DevStudio::HlaRibbonBridgeObjectManagerListener::Adapter, public DevStudio::HlaRibbonBridgeObjectListener::Adapter, public std::enable_shared_from_this<SDLRibbonBridgeObjectManagerListener> {
public:
    void hlaRibbonBridgeObjectDiscovered(DevStudio::HlaRibbonBridgeObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaRibbonBridgeObjectInitialized(DevStudio::HlaRibbonBridgeObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaRibbonBridgeObjectDeleted(DevStudio::HlaRibbonBridgeObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaRibbonBridgeObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRibbonBridgeObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLStructureObjectManagerListener : public DevStudio::HlaStructureObjectManagerListener::Adapter, public DevStudio::HlaStructureObjectListener::Adapter, public std::enable_shared_from_this<SDLStructureObjectManagerListener> {
public:
    void hlaStructureObjectDiscovered(DevStudio::HlaStructureObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaStructureObjectInitialized(DevStudio::HlaStructureObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaStructureObjectDeleted(DevStudio::HlaStructureObjectPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaStructureObjectPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaStructureObjectAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLEnvironmentProcessManagerListener : public DevStudio::HlaEnvironmentProcessManagerListener::Adapter, public DevStudio::HlaEnvironmentProcessListener::Adapter, public std::enable_shared_from_this<SDLEnvironmentProcessManagerListener> {
public:
    void hlaEnvironmentProcessDiscovered(DevStudio::HlaEnvironmentProcessPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaEnvironmentProcessInitialized(DevStudio::HlaEnvironmentProcessPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaEnvironmentProcessDeleted(DevStudio::HlaEnvironmentProcessPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaEnvironmentProcessPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEnvironmentProcessAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLGriddedDataManagerListener : public DevStudio::HlaGriddedDataManagerListener::Adapter, public DevStudio::HlaGriddedDataListener::Adapter, public std::enable_shared_from_this<SDLGriddedDataManagerListener> {
public:
    void hlaGriddedDataDiscovered(DevStudio::HlaGriddedDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaGriddedDataInitialized(DevStudio::HlaGriddedDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaGriddedDataDeleted(DevStudio::HlaGriddedDataPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaGriddedDataPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaGriddedDataAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLEmitterBeamManagerListener : public DevStudio::HlaEmitterBeamManagerListener::Adapter, public DevStudio::HlaEmitterBeamListener::Adapter, public std::enable_shared_from_this<SDLEmitterBeamManagerListener> {
public:
    void hlaEmitterBeamDiscovered(DevStudio::HlaEmitterBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaEmitterBeamDeleted(DevStudio::HlaEmitterBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaEmitterBeamInitialized(DevStudio::HlaEmitterBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaEmitterBeamPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaEmitterBeamAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLRadarBeamManagerListener : public DevStudio::HlaRadarBeamManagerListener::Adapter, public DevStudio::HlaRadarBeamListener::Adapter, public std::enable_shared_from_this<SDLRadarBeamManagerListener> {
public:
    void hlaRadarBeamDiscovered(DevStudio::HlaRadarBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaRadarBeamDeleted(DevStudio::HlaRadarBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaRadarBeamInitialized(DevStudio::HlaRadarBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaRadarBeamPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaRadarBeamAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLJammerBeamManagerListener : public DevStudio::HlaJammerBeamManagerListener::Adapter, public DevStudio::HlaJammerBeamListener::Adapter, public std::enable_shared_from_this<SDLJammerBeamManagerListener> {
public:
    void hlaJammerBeamDiscovered(DevStudio::HlaJammerBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaJammerBeamDeleted(DevStudio::HlaJammerBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaJammerBeamInitialized(DevStudio::HlaJammerBeamPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaJammerBeamPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaJammerBeamAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};

class SDLMinefieldManagerListener : public DevStudio::HlaMinefieldManagerListener::Adapter, public DevStudio::HlaMinefieldListener::Adapter, public std::enable_shared_from_this<SDLMinefieldManagerListener> {
public:
    void hlaMinefieldDiscovered(DevStudio::HlaMinefieldPtr entity, DevStudio::HlaTimeStampPtr timeStamp) override;
    void hlaMinefieldInitialized(DevStudio::HlaMinefieldPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void hlaMinefieldDeleted(DevStudio::HlaMinefieldPtr entity, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void attributesUpdated(DevStudio::HlaMinefieldPtr entity, const DevStudio::HlaEnumSet<DevStudio::HlaMinefieldAttributes::Attribute>& attributes, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
};