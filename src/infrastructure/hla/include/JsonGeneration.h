
#ifndef JSONGENERATION_H
#define JSONGENERATION_H

#include <nlohmann/json.hpp>
#include <DevStudio/HlaPhysicalEntityAttributesImpl.h>
#include <DevStudio/HlaPhysicalEntityImpl.h>


namespace SDL_RTI {
  
    class JsonGeneration
    {
        public:

        JsonGeneration();
        ~JsonGeneration();
        
        nlohmann::json createJsonPhysicalEntity(int action, int idEntity, double latitude, double longitude, double altitude);

        nlohmann::json createJsonMessageEntity(int idEntity, int action, double latitude, double longitude);
        nlohmann::json createJsonMessageEmitter(int idRadar, int action);

        nlohmann::json createFederateJson(int federateHandle, const std::string& federationName, const std::string& federateName, const std::string& federateType, const std::string& federateHost, int federateState);
        nlohmann::json createJsonMessageWithOrientation(int id, double lat, double lon, double heading, double pitch, double roll);

        private:


    };

}

#endif // JSONGENERATION_H