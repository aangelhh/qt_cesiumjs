#pragma once
#include <memory>

namespace SDL_RTI
{
    class Configuration
    {
    public:
        static Configuration& getInstance()
        {
            static std::unique_ptr<Configuration> theInstance{ new Configuration() };
            return *theInstance;
        }

        void setSiteId(unsigned short siteId) { m_siteId = siteId; }
        void setApplicationId(unsigned short applicationId) { m_applicationId = applicationId; }

        unsigned short getSiteId() { return m_siteId; }
        unsigned short getApplicationId() { return m_applicationId; }
        DevStudio::FederateIdentifierStruct getFederateIdentifier()
        {
            return DevStudio::FederateIdentifierStruct(m_siteId, m_applicationId);
        }

    private:
        unsigned short m_siteId;
        unsigned short m_applicationId;
    };
}