#pragma once
#include <string>

namespace enumsCommon_Client
{
    const std::string ActionAdd = "Add"; const std::string ActionUpdate = "Update"; const std::string ActionRemove = "Remove";
    const std::string AcknowledgeCreate = "Create"; const std::string AcknowledgeRemove = "Remove"; const std::string AcknowledgeStart = "Start";
    const std::string AcknowledgeStop = "Stop"; const std::string AcknowledgeTransfer_CONTROL = "Transfer Control";

    enum InteractionEnum {
        NullInteractionEnum = 0, StartResumeInteractionEnum, StartResumeAnswerInteractionEnum, StopFreezeInteractionEnum, StopFreezeAnswerInteractionEnum,
        AcknowledgeInteractionEnum, CreateEntityInteractionEnum, RemoveEntityInteractionEnum, CollisionInteractionEnum, WeaponFireInteractionEnum, MunitionDetonationInteractionEnum, 
        ResupplyCancelInteractionEnum, ResupplyOfferInteractionEnum, ResupplyReceivedInteractionEnum, AttributeChangeRequestEnum, CreateObjectRequestEnum, RemoveObjectRequestEnum
    };

    enum ObjectEnum { NullObjectEnum = 1000, FederateObjectEnum, BaseEntityObjectEnum, EmbeddedSystemObjectEnum, EmitterSystemObjectEnum, 
        EmitterBeamObjectEnum, PhysicalEntityObjectEnum, PlatformObjectEnum, RadarBeamObjectEnum, JammerBeamObjectEnum, 
        AircraftObjectEnum, GroundVehicleObjectEnum, MunitionObjectEnum, SuppliesObjectEnum, ExpendablesObjectEnum, AggregateObjectEnum, 
        SensorObjectEnum, DesignatorObjectEnum, AmphibiousVehicleObjectEnum, MultiDomainPlatformObjectEnum, SubmersibleVesselObjectEnum,
        SpacecraftObjectEnum, SurfaceVesselObjectEnum, LifeformObjectEnum, HumanObjectEnum, NonHumanObjectEnum, CulturalFeatureObjectEnum,
        RadioObjectEnum, RadioTransmitterObjectEnum, RadioReceiverObjectEnum, IFFObjectEnum, RRBObjectEnum, MinefieldDataObjectEnum,
        EnvironmentObjectObjectEnum, MinefieldObjectObjectEnum, ArealObjectObjectEnum, LinearObjectObjectEnum, PointObjectObjectEnum,
        RibbonBridgeObjectObjectEnum, StructureObjectObjectEnum, EnvironmentProcessObjectEnum, GriddedDataObjectEnum,
        StuctureObjectObjectEnum, MinefieldObjectEnum, TankerAircraftObjectEnum, OrganizationElementObjectEnum, EquipmentObjectEnum,
        AISEquipmentObjectEnum, VesselObjectEnum, AggregateEntityObjectEnum, SARaircraftObjectEnum, BasestationObjectEnum, AidToNavigationObjectEnum,
        UnitObjectEnum, InstallationObjectEnum, OrganizationObjectEnum, DIMHazardRegionObjectEnum, CBRNHazardObjectEnum, CBRNHazardPredictionObjectEnum,
        FloodingObjectEnum, WildfireObjectEnum, EarthquakeAreaObjectEnum, ConnectionObjectEnum, CommunicationNodeObjectEnum, CommunicationNetworkObjectEnum,
        LandslideObjectEnum, PhysicalNetworkObjectEnum, LinkStatesObjectEnum, DisruptionEffectObjectEnum, BreachableLinearObjectObjectEnum,
        LeveeObjectEnum, FirebreakObjectEnum, BreachObjectObjectEnum, ExhaustSmokeObjectObjectEnum, OtherLinearObjectObjectEnum, CheckpointObjectEnum,
        ObservationPostObjectEnum, BreachablePointObjectObjectEnum, BurstPointObjectObjectEnum, CraterObjectObjectEnum, OtherPointObjectObjectEnum,
        ETRDelegatedTaskObjectEnum, METOCEnvironmentConditionObjectEnum, WeatherObjectEnum, LandSurfaceObjectEnum, WaterSurfaceObjectEnum, 
        TroposphereLayerObjectEnum, SubsurfaceLayerObjectEnum, WindCorridorObjectEnum, SMCServiceObjectEnum, METOCServiceObjectEnum,
        EntitySimulationObjectEnum, NatoIFFObjectEnum, NatoIFFInterrogatorObjectEnum, NatoIFFTransponderObjectEnum,
        SovietIFFObjectEnum, SovietIFFInterrogatorObjectEnum, SovietIFFTransponderObjectEnum, UnderwaterAcousticsEmissionObjectEnum,
        ActiveSonarObjectEnum, PropulsionNoiseObjectEnum, AdditionalPassiveActivitiesObjectEnum, ActiveSonarBeamObjectEnum
    };

    enum ActionObjectEnum {
        AddObject = 1, UpdateObject = 2, RemoveObject = 3, Interaction = 4 
    };

    enum levelLoggerEnum {
        CriticalErrorLevel = 0, ErrorLevel, WarningLevel, InfoLevel,
        VoiceLevel, DebugLevel, MixedFormationLevel
    };
    
    enum ReasonEnum {
        ReasonOther, ReasonRecess, ReasonTermination,
        ReasonSystemFailure, ReasonSecurityViolation, ReasonEntityReconstitution,
        ForReset, ForRestart, AbortTraining
    };

    enum FederateStateEnum {
        FedStateActiveFederate = 1, FedStateFederateSaveInProgress = 3,
        FedStateFederateRestoreInProgress = 5
    };

     enum EntityKind {
        EntKindOther = 0, Platform, Munition, LifeForm, Environmental, CulturaFeature, Supply, Radio, Expendable, SensorEmitter
    };

    enum EntityDomain {
        EDOther = 0, Land, Air, Surface, Subsurface, Space
    };
}
