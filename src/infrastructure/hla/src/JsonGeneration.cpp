#include "JsonGeneration.h"

using nlohmann::json;
using namespace SDL_RTI;

JsonGeneration::JsonGeneration()
{

}

JsonGeneration::~JsonGeneration()
{

}
nlohmann::json JsonGeneration::createJsonPhysicalEntity(int action, int idEntity, double latitude, double longitude, double altitude)
{
       
    json jBaseEntity;
    jBaseEntity["Data"] = "BaseEntity"; 
    jBaseEntity["Action"] = action; 
    jBaseEntity["ForceIdentifier"] = 1; 
    jBaseEntity["EntityType"]["EntityKind"] = "0"; 
    jBaseEntity["EntityType"]["Domain"] = "0"; 
    jBaseEntity["EntityType"]["CountryCode"] = "0"; 
    jBaseEntity["EntityType"]["Category"] = "0"; 
    jBaseEntity["EntityType"]["Subcategory"] = "0"; 
    jBaseEntity["EntityType"]["Specific"] = "0"; 
    jBaseEntity["EntityType"]["Extra"] = "0";
    jBaseEntity["EntityIdentifier"]["FederateIdentifier"]["SiteID"] = "0";
    jBaseEntity["EntityIdentifier"]["FederateIdentifier"]["ApplicationID"] = "0";
    jBaseEntity["EntityIdentifier"]["EntityNumber"] = std::to_string(idEntity);
    jBaseEntity["Spatial"]["WorldLocation"]["Altitude"] = altitude;
    jBaseEntity["Spatial"]["WorldLocation"]["Longitude"] = longitude;
    jBaseEntity["Spatial"]["WorldLocation"]["Latitude"] = latitude;
    jBaseEntity["Spatial"]["IsFrozen"] = false;
    jBaseEntity["Spatial"]["Orientation"]["Heading"] = 0;
    jBaseEntity["Spatial"]["Orientation"]["Pitch"] = 0;
    jBaseEntity["Spatial"]["Orientation"]["Roll"] = 0;

    return jBaseEntity;
}

nlohmann::json JsonGeneration::createJsonMessageEntity(int idEntity, int action, double latitude, double longitude)
{
    json jBaseEntity;
    jBaseEntity["Data"] = "BaseEntity"; 
    jBaseEntity["Action"] = action; 
    jBaseEntity["ForceIdentifier"] = 1; 
    jBaseEntity["EntityType"]["EntityKind"] = "0"; 
    jBaseEntity["EntityType"]["Domain"] = "0"; 
    jBaseEntity["EntityType"]["CountryCode"] = "0"; 
    jBaseEntity["EntityType"]["Category"] = "0"; 
    jBaseEntity["EntityType"]["Subcategory"] = "0"; 
    jBaseEntity["EntityType"]["Specific"] = "0"; 
    jBaseEntity["EntityType"]["Extra"] = "0";
    jBaseEntity["EntityIdentifier"]["FederateIdentifier"]["SiteID"] = "0";
    jBaseEntity["EntityIdentifier"]["FederateIdentifier"]["ApplicationID"] = "0";
    jBaseEntity["EntityIdentifier"]["EntityNumber"] = std::to_string(idEntity);
    jBaseEntity["Spatial"]["WorldLocation"]["Altitude"] = 10000;
    jBaseEntity["Spatial"]["WorldLocation"]["Longitude"] = longitude;
    jBaseEntity["Spatial"]["WorldLocation"]["Latitude"] = latitude;
    jBaseEntity["Spatial"]["IsFrozen"] = false;
    jBaseEntity["Spatial"]["Orientation"]["Heading"] = 0;
    jBaseEntity["Spatial"]["Orientation"]["Pitch"] = 0;
    jBaseEntity["Spatial"]["Orientation"]["Roll"] = 0;

    return jBaseEntity;
}

nlohmann::json JsonGeneration::createJsonMessageEmitter(int idRadar, int action) 
{
    json jEmitter;
    jEmitter["IdRadar"] = idRadar; 
    jEmitter["Data"] = "EmitterBeam"; 
    jEmitter["Action"] = action; 
    jEmitter["BeamIdentifier"] = 0; 
    jEmitter["BeamAzimuthCenter"] = 0; 
    jEmitter["BeamAzimuthSweep"] = 0; 
    jEmitter["BeamElevationCenter"] = 0; 
    jEmitter["EffectiveRadiatedPower"] = 0; 
    jEmitter["EmissionFrequency"] = 0; 
    jEmitter["FrequencyRange"] = 0; 
    jEmitter["PulseRepetitionFrequency"] = 0; 
    jEmitter["BeamFunctionCode"] = 0; 
    jEmitter["EmitterSystemIdentifier"] = 0; 
    jEmitter["BeamElevationSweep"] = 0;  
    jEmitter["BeamParameterIndex"] = 0;  
    jEmitter["IdEntity"] = "1"; 
    jEmitter["PulseWidth"] = 0;  
    jEmitter["SweepSynch"] = 0;  

    return jEmitter;
}

json JsonGeneration::createFederateJson(int federateHandle, const std::string& federationName, const std::string& federateName, const std::string& federateType, const std::string& federateHost, int federateState) {
    json jFederate;
    jFederate["Data"] = "Federate";
    jFederate["HLAfederateHandle"] = federateHandle;
    jFederate["HLAfederationName"] = federationName;
    jFederate["HLAfederateName"] = federateName;
    jFederate["HLAfederateType"] = federateType;
    jFederate["HLAfederateHost"] = federateHost;
    jFederate["HLAfederateState"] = federateState;

    return jFederate;
}

json JsonGeneration::createJsonMessageWithOrientation(int id, double lat, double lon, double heading, double pitch, double roll) {
    json message;
    message["id"] = id;
    message["latitude"] = lat;
    message["longitude"] = lon;
    message["heading"] = heading;
    message["pitch"] = pitch;
    message["roll"] = roll;
    return message;
}