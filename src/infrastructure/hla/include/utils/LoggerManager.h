#pragma once
#include "Singleton.h"
#include <string>
#include "../../ClientPitch.h"


namespace enumsCommon_Client
{
    enum levelLoggerEnum;
}

class LoggerManager : public events::Singleton<LoggerManager>
{
    public:
        LoggerManager();
        ~LoggerManager();

        void setFederationManager( SDL_RTI::ClientPitch* federationManager);

        void notifyNewLogger(const std::string message, enumsCommon_Client::levelLoggerEnum level);
        void notifyNewLogger(const std::wstring message, enumsCommon_Client::levelLoggerEnum level);

    private:
        SDL_RTI::ClientPitch* m_FederationManager;
};