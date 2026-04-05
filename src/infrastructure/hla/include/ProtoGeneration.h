#ifndef PROTOGENERATION_H
#define PROTOGENERATION_H

#include "../../protobuf/c++/entity.pb.h"
#include <DevStudio/HlaPhysicalEntityAttributesImpl.h>
#include <DevStudio/HlaPhysicalEntityImpl.h>


namespace SDL_RTI {
  
    class ProtoGeneration
    {
        public:

        ProtoGeneration();
        ~ProtoGeneration();
        
        Envelope createProtoPhysicalEntity(int action, int idEntity, double latitude, double longitude
            , double altitude, double heading, double pitch, double roll, int forceIdentifier
            ,  int kind, int domain, int countryCode, int category, int subcategory
            ,  int specific, int extra, const std::string& marking);

        Envelope createProtoEmitterBeam(int action, int EmitterSystemId, int EmitterBeamId, int EntityId, std::vector<int> TrackedObjectIdentifiers,
            float BeamAzimuthCenter, float BeamAzimuthSweep, float BeamElevationCenter, float BeamElevationSweep, float SweepSynch, float pulseRepetitionFrequency);

        Envelope createProtoFederate(int action, const std::string& federateHandle
        , const std::string& federationName, const std::string& federateName
        , const std::string& federateType, const std::string& federateHost
        , int federateState);

        
        Envelope createProtoPlayResume(void);
        Envelope createProtoPause(void);
        Envelope createProtoStop(void);

        private:


    };

}

#endif // PROTOGENERATION_H
