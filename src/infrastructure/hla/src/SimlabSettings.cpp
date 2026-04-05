#include "../include/SimlabSettings.h"
#include <codecvt>
#define NOMINMAX
// #include <Windows.h>

#include <string>
#include <string_view>
// #include <format>

using namespace SDL_RTI;


    std::wstring SDL_RTI::towstr(const std::string& s) {
        return  std::wstring(s.begin(), s.end());
    }

    std::string SDL_RTI::tostr(const std::wstring& s) {
        return std::string(s.begin(), s.end());
    }



    SimlabSettings::SimlabSettings()
        
    {
    }

    SDL_RTI::SimlabSettings::SimlabSettings(const std::wstring& crcHost, int crcPort) : HlaSettings(crcHost, crcPort)
    {
    }

    void SimlabSettings::setCrcHost(const std::string& crcHost)
    {
        
        _rtiDriverProfile = L"B";
        
        std::wstring crcHostWstr(towstr(crcHost));
        _crcHost = crcHostWstr;
    }

    void SimlabSettings::setCrcPort(int crcPort)
    {
        _crcPort = crcPort;
    }

    void SimlabSettings::setLocalSettingsDesignator(const std::string& lsd)
    {
        std::wstring lsdWstr = towstr(lsd);
        std::wstring garbageRelease;
        _localSettingsDesignator.swap(garbageRelease);
        _localSettingsDesignator = lsdWstr;
    }

    void SimlabSettings::setFederationName(const std::string& name)
    {
        std::wstring nameWstr = towstr(name);
        std::wstring garbageRelease;
        _federationName.swap(garbageRelease);
        _federationName = nameWstr;
    }

    void SimlabSettings::setFederateName(const std::string& name)
    {
        std::wstring nameWstr = towstr(name);
        std::wstring garbageRelease;
        _federateName.swap(garbageRelease);
        _federateName = nameWstr;
    }

    void SimlabSettings::setCreateFederation(bool createFederation)
    {
        _createFederation = createFederation;
    }

    void SimlabSettings::setDestroyFederation(bool destroyFederation)
    {
        _destroyFederation = destroyFederation;
    }