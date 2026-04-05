#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <DevStudio/HlaJammerBeamAttributesImpl.h>
#include <DevStudio/HlaRadarBeamAttributesImpl.h>


namespace SDL_RTI {

	class EmitterBeamConverter
	{
	public:
		EmitterBeamConverter();
		~EmitterBeamConverter();

		std::vector<std::string> getVectorString(void);
		std::vector<int> getVectorInt(void);
		std::map<std::string, int> getMap(void);

		void setVectorString(std::vector<std::string>val);
		void setVectorInt(std::vector<int>val);
		void setMap(std::map<std::string, int>val);
		std::vector<int> findEntityNumberJammer(DevStudio::HlaJammerBeamAttributesPtr* val);
		std::vector<int> findEntityNumberRadar(DevStudio::HlaRadarBeamAttributesPtr* val);

		int findEmitterSystemIdentifier(DevStudio::HlaRadarBeamAttributesPtr* val);
		int findEmitterSystemIdentifierJammer(DevStudio::HlaJammerBeamAttributesPtr* val);

	private:
		std::vector<std::string> m_VectorString;
		std::vector<int> m_VectorInt;
		std::map<std::string, int> m_MapObjects;

	};

}

