#pragma once

#include "ClientPitch.h"
#include "include/Websocketserver.h"
#include "include/ProtoGeneration.h"

namespace SDL_RTI
{
	class ClientPitchWebSocket: public ClientPitch 
	{
	public:
		ClientPitchWebSocket();
		~ClientPitchWebSocket();

		void initConnect(const RequestClientHLAProto& requestClientHla);
		void finishConnect();
       
        void onEventReceived(const EventOnDataAvailableInteraction& event) override;
		void onEventReceived(const EventOnDataAvailableObject& event) override;


        void handleMagicAction(const CreateEntityMagicActionProto& action);
        void handleRemoveEntityMagicAction(const RemoveEntityMagicActionProto& action);
		void handleDestroyEntityMagicAction(const DestroyEntityProto& action);		
		void handleMagicMoveAction(const MagicMoveProto& action);


		void handlePlayResume();
		void handlePauseFreeze();
		void handleStop();

		void setRunningFlag(std::atomic<bool>* flag);
		void onStopEventReceived();

		void createMessageFederate(const std::shared_ptr<DevStudio::HlaHLAfederateAttributesImpl>& federate, enumsCommon_Client::ActionObjectEnum action);


    private:
        std::map<int, Envelope> m_MapProtoEntity;
		std::map<int, Envelope> m_MapProtoRadar;
		std::map<int, Envelope> m_MapProtoBeams;
		std::unordered_map<std::string, std::shared_ptr<DevStudio::HlaEmitterSystemAttributesImpl>> m_EmitterSystemMap;
        std::unique_ptr<SDL_RTI::Websocketserver> m_WebsocketServer;
        int m_RequestID;
        int m_IdAll;
		std::atomic<bool>* m_RunningFlag;

    };
}