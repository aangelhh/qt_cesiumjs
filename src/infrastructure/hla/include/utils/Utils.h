#pragma once
#include <vector>
#include <map>
#include "../enumsCommon/EnumsCommons.h"
#include <DevStudio/HlaObjectInstanceBase.h>

using namespace DevStudio;

namespace SDL_RTI {
    class Utils
    {
        std::map<HlaObjectInstanceBase::ObjectClassType, std::map<std::string, int>> objectClassTypeAttributesMap;
        std::map<HlaObjectInstanceBase::ObjectClassType, std::map<int, std::string>> objectClassTypeAttributesReverseMap;

        Utils();
    public:
        static Utils& GetInstance()
        {
            static std::unique_ptr<Utils> theInstance{ new Utils() };
            return *theInstance;
        }

        std::string getAttributeName(const HlaObjectInstanceBase::ObjectClassType& objectType, const int& attribute);
        int getAttributeFromName(const HlaObjectInstanceBase::ObjectClassType& objectType, const std::string& attributeName);


        static HlaObjectInstanceBase::ObjectClassType ObjectClassTypeFromSiso(unsigned char kind, unsigned char domain, bool isTanker);

        //static DevStudio::TaskDefinition CreateTaskDefinition(DevStudio::EntityIdentifierStruct entity, short taskId, short step, DevStudio::TaskStatusEnum::TaskStatusEnum status);

        static std::vector<unsigned long long> ConverteVectorCharToVectorLong(std::vector<char> charVector);
        static std::vector<char> ConvertVectorLongToVectorChar(std::vector<unsigned long long>& longVector);
    };
}