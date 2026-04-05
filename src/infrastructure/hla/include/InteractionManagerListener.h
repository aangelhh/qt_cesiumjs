#pragma once
#include <DevStudio/HlaInteractionListener.h>
#include "../ClientPitch.h"

class InteractionManagerListener : public DevStudio::HlaInteractionListener::Adapter
{
public:
    //Interaction Start/Resume (Play/Pause)
    void startResume(bool local, DevStudio::HlaStartResumeParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    // Interaction Stop/Freeze
    void stopFreeze(bool local, DevStudio::HlaStopFreezeParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

    void collision(bool local, DevStudio::HlaCollisionParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

    void createEntity(bool local, DevStudio::HlaCreateEntityParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void removeEntity(bool local, DevStudio::HlaRemoveEntityParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

    void acknowledge(bool local, DevStudio::HlaAcknowledgeParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;

    void munitionDetonation(bool local, DevStudio::HlaMunitionDetonationParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void resupplyCancel(bool local, DevStudio::HlaResupplyCancelParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void resupplyOffer(bool local, DevStudio::HlaResupplyOfferParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void resupplyReceived(bool local, DevStudio::HlaResupplyReceivedParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;
    void weaponFire(bool local, DevStudio::HlaWeaponFireParametersPtr parameters, DevStudio::HlaTimeStampPtr timeStamp, DevStudio::HlaLogicalTimePtr logicalTime) override;


    

private:
    
};

