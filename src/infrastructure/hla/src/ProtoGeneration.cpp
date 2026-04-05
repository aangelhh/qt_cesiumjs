#include "ProtoGeneration.h"

using namespace SDL_RTI;

ProtoGeneration::ProtoGeneration()
{

}

ProtoGeneration::~ProtoGeneration()
{

}

Envelope ProtoGeneration::createProtoPhysicalEntity(int action, int idEntity, double latitude, double longitude, double altitude, double heading, double pitch, double roll, int forceIdentifier,  int kind, int domain, int countryCode, int category, int subcategory,  int specific, int extra, const std::string& marking)
{
    Envelope envelope;

    // Rellenar el mensaje base_entity dentro del envelope
    BaseEntityProto* entity = envelope.mutable_base_entity();
    entity->set_data("BaseEntity");
    entity->set_action(std::to_string(action));
    entity->set_forceidentifier(forceIdentifier);
    entity->set_marking(marking);

    EntityTypeProto* type = entity->mutable_entitytype();
    type->set_entitykind(std::to_string(kind));
    type->set_domain(std::to_string(domain));
    type->set_countrycode(std::to_string(countryCode));
    type->set_category(std::to_string(category));
    type->set_subcategory(std::to_string(subcategory));
    type->set_specific(std::to_string(specific));
    type->set_extra(std::to_string(extra));

    EntityIdentifierProto* identifier = entity->mutable_entityidentifier();
    FederateIdentifierProto* federate = identifier->mutable_federateidentifier();
    federate->set_siteid("0");
    federate->set_applicationid("0");
    identifier->set_entitynumber(std::to_string(idEntity));

    SpatialProto* spatial = entity->mutable_spatial();
    WorldLocationProto* location = spatial->mutable_worldlocation();
    location->set_altitude(altitude);
    location->set_longitude(longitude);
    location->set_latitude(latitude);
    
    spatial->set_velocity(0.0);
    spatial->set_velocityx(0.0);
    spatial->set_velocityy(0.0);
    spatial->set_velocityz(0.0);

    spatial->set_isfrozen(false);

    OrientationProto* orientation = spatial->mutable_orientation();
    orientation->set_heading(heading);
    orientation->set_pitch(pitch);
    orientation->set_roll(roll);

    return envelope;
}

Envelope ProtoGeneration::createProtoEmitterBeam(int action, int EmitterSystemId, int EmitterBeamId, int EntityId, std::vector<int> TrackedObjectIdentifiers,
            float BeamAzimuthCenter, float BeamAzimuthSweep, float BeamElevationCenter, float BeamElevationSweep, float SweepSynch, float pulseRepetitionFrequency)
{
    Envelope envelope;

    EmitterBeamProto* emmiterBeam = envelope.mutable_emitter_beam();

    emmiterBeam->set_data("EmmiterBeam");
    emmiterBeam->set_action(std::to_string(action));
    emmiterBeam->set_emittersystemid(EmitterSystemId);
    emmiterBeam->set_emitterbeamid(EmitterBeamId);
    emmiterBeam->set_entityid(EntityId);
    for(auto& track : TrackedObjectIdentifiers)
        emmiterBeam->add_trackedobjectidentifiers(track);
    emmiterBeam->set_beamazimuthcenter(BeamAzimuthCenter);
    emmiterBeam->set_beamazimuthsweep(BeamAzimuthSweep);
    emmiterBeam->set_beamelevationcenter(BeamElevationCenter);
    emmiterBeam->set_beamelevationsweep(BeamElevationSweep);
    emmiterBeam->set_sweepsynch(SweepSynch);
    emmiterBeam->set_pulserepetitionfrequency(pulseRepetitionFrequency);

    return envelope;
    
}

Envelope ProtoGeneration::createProtoFederate(int action, const std::string& federateHandle
    , const std::string& federationName, const std::string& federateName
    , const std::string& federateType, const std::string& federateHost
    , int federateState)
{
    Envelope envelope;

    ReceivedFederateHlaProto* federateMsg = envelope.mutable_receivedfederate();

    federateMsg->set_action(std::to_string(action));
    federateMsg->set_federatehandle(federateHandle);
    federateMsg->set_federationname(federationName);
    federateMsg->set_federatename(federateName);
    federateMsg->set_federatetype(federateType);
    federateMsg->set_federatehost(federateHost);
    federateMsg->set_federatestate(federateState);

    return envelope;
}

Envelope ProtoGeneration::createProtoPlayResume(void)
{
    Envelope envelope;

    InteractionProto* interaction = envelope.mutable_interaction();

    interaction->set_type(InteractionProto::PLAY);

    return envelope;
}
    
Envelope ProtoGeneration::createProtoPause(void)
{
    Envelope envelope;

    InteractionProto* interaction = envelope.mutable_interaction();
    
    interaction->set_type(InteractionProto::PAUSE);

    return envelope;
}

Envelope ProtoGeneration::createProtoStop(void)
{
    Envelope envelope;

    InteractionProto* interaction = envelope.mutable_interaction();

    interaction->set_type(InteractionProto::STOP);

    return envelope;
}



// nlohmann::json ProtoGeneration::createJsonMessageEntity(int idEntity, int action, double latitude, double longitude)
// {
//     BaseEntityProto entity;
//     entity.set_data("BaseEntity");

//     return entity;
//     // json jBaseEntity;
//     // jBaseEntity["Data"] = "BaseEntity"; 
//     // jBaseEntity["Action"] = action; 
//     // jBaseEntity["ForceIdentifier"] = 1; 
//     // jBaseEntity["EntityType"]["EntityKind"] = "0"; 
//     // jBaseEntity["EntityType"]["Domain"] = "0"; 
//     // jBaseEntity["EntityType"]["CountryCode"] = "0"; 
//     // jBaseEntity["EntityType"]["Category"] = "0"; 
//     // jBaseEntity["EntityType"]["Subcategory"] = "0"; 
//     // jBaseEntity["EntityType"]["Specific"] = "0"; 
//     // jBaseEntity["EntityType"]["Extra"] = "0";
//     // jBaseEntity["EntityIdentifier"]["FederateIdentifier"]["SiteID"] = "0";
//     // jBaseEntity["EntityIdentifier"]["FederateIdentifier"]["ApplicationID"] = "0";
//     // jBaseEntity["EntityIdentifier"]["EntityNumber"] = std::to_string(idEntity);
//     // jBaseEntity["Spatial"]["WorldLocation"]["Altitude"] = 10000;
//     // jBaseEntity["Spatial"]["WorldLocation"]["Longitude"] = longitude;
//     // jBaseEntity["Spatial"]["WorldLocation"]["Latitude"] = latitude;
//     // jBaseEntity["Spatial"]["IsFrozen"] = false;
//     // jBaseEntity["Spatial"]["Orientation"]["Heading"] = 0;
//     // jBaseEntity["Spatial"]["Orientation"]["Pitch"] = 0;
//     // jBaseEntity["Spatial"]["Orientation"]["Roll"] = 0;

//     // return jBaseEntity;
// }

// nlohmann::json ProtoGeneration::createJsonMessageEmitter(int idRadar, int action) 
// {
//     // json jEmitter;
//     // jEmitter["IdRadar"] = idRadar; 
//     // jEmitter["Data"] = "EmitterBeam"; 
//     // jEmitter["Action"] = action; 
//     // jEmitter["BeamIdentifier"] = 0; 
//     // jEmitter["BeamAzimuthCenter"] = 0; 
//     // jEmitter["BeamAzimuthSweep"] = 0; 
//     // jEmitter["BeamElevationCenter"] = 0; 
//     // jEmitter["EffectiveRadiatedPower"] = 0; 
//     // jEmitter["EmissionFrequency"] = 0; 
//     // jEmitter["FrequencyRange"] = 0; 
//     // jEmitter["PulseRepetitionFrequency"] = 0; 
//     // jEmitter["BeamFunctionCode"] = 0; 
//     // jEmitter["EmitterSystemIdentifier"] = 0; 
//     // jEmitter["BeamElevationSweep"] = 0;  
//     // jEmitter["BeamParameterIndex"] = 0;  
//     // jEmitter["IdEntity"] = "1"; 
//     // jEmitter["PulseWidth"] = 0;  
//     // jEmitter["SweepSynch"] = 0;  

//     // return jEmitter;
// }

// json ProtoGeneration::createFederateJson(int federateHandle, const std::string& federationName, const std::string& federateName, const std::string& federateType, const std::string& federateHost, int federateState) {
//     // json jFederate;
//     // jFederate["Data"] = "Federate";
//     // jFederate["HLAfederateHandle"] = federateHandle;
//     // jFederate["HLAfederationName"] = federationName;
//     // jFederate["HLAfederateName"] = federateName;
//     // jFederate["HLAfederateType"] = federateType;
//     // jFederate["HLAfederateHost"] = federateHost;
//     // jFederate["HLAfederateState"] = federateState;

//     // return jFederate;
// }

// json JsonGeneration::createJsonMessageWithOrientation(int id, double lat, double lon, double heading, double pitch, double roll) {
//     // json message;
//     // message["id"] = id;
//     // message["latitude"] = lat;
//     // message["longitude"] = lon;
//     // message["heading"] = heading;
//     // message["pitch"] = pitch;
//     // message["roll"] = roll;
//     // return message;
// }