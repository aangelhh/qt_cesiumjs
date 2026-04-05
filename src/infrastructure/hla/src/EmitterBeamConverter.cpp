#include "../include/DataTypes/EmitterBeamConverter.h"

SDL_RTI::EmitterBeamConverter::EmitterBeamConverter()
{
}

SDL_RTI::EmitterBeamConverter::~EmitterBeamConverter()
{
}

std::vector<std::string> SDL_RTI::EmitterBeamConverter::getVectorString(void)
{
	return m_VectorString;
}

std::vector<int> SDL_RTI::EmitterBeamConverter::getVectorInt(void)
{
	return m_VectorInt;
}

std::map<std::string, int> SDL_RTI::EmitterBeamConverter::getMap(void)
{
	return m_MapObjects;
}

void SDL_RTI::EmitterBeamConverter::setVectorString(std::vector<std::string> val)
{
	m_VectorString = val;
}

void SDL_RTI::EmitterBeamConverter::setVectorInt(std::vector<int> val)
{
	m_VectorInt = val;
}

void SDL_RTI::EmitterBeamConverter::setMap(std::map<std::string, int> val)
{
	m_MapObjects = val;
}

std::vector<int> SDL_RTI::EmitterBeamConverter::findEntityNumberJammer(DevStudio::HlaJammerBeamAttributesPtr* val)
{
	std::vector<int> vector;

	if (val->get()->hasJammedObjectIdentifiers()) {
		for (auto it : val->get()->getJammedObjectIdentifiers()) {
			if (m_MapObjects.size() > 0) {
				auto i = m_MapObjects.find(it);
				if (i != m_MapObjects.end()) {
					vector.push_back(i->second);
				}
			}

		}
	}
	return vector;
}

std::vector<int> SDL_RTI::EmitterBeamConverter::findEntityNumberRadar(DevStudio::HlaRadarBeamAttributesPtr* val)
{
	std::vector<int> vector;
	
	if (val->get()->hasTrackObjectIdentifiers()) {
		for (auto it : val->get()->getTrackObjectIdentifiers()) {
			if (m_MapObjects.size() > 0) {
				auto i = m_MapObjects.find(it);
				if (i != m_MapObjects.end()) {
					vector.push_back(i->second);
				}
			}

		}
	}

	return vector;
}

int SDL_RTI::EmitterBeamConverter::findEmitterSystemIdentifier(DevStudio::HlaRadarBeamAttributesPtr* val)
{
	int result = -1;

	//for (const auto& pair : m_MapObjects) {
	//	std::cout << "Clave: " << pair.first << ", Valor: " << pair.second << std::endl;
	//}
	//std::cout <<  val->get()->getEmitterSystemIdentifier() << std::endl;
	//std::cout <<  val->get()->getEventIdentifier().getIssuingObjectIdentifier() << std::endl;

	if (val->get()->hasEmitterSystemIdentifier()) {
		if (m_MapObjects.size() > 0) {
			auto i = m_MapObjects.find(val->get()->getEventIdentifier().getIssuingObjectIdentifier());
			if (i != m_MapObjects.end()) {
				result = i->second;
			}
		}
	}

	return result;
}

int SDL_RTI::EmitterBeamConverter::findEmitterSystemIdentifierJammer(DevStudio::HlaJammerBeamAttributesPtr* val)
{
	int result = -1;

	//for (const auto& pair : m_MapObjects) {
	//	std::cout << "Clave: " << pair.first << ", Valor: " << pair.second << std::endl;
	//}
	//std::cout <<  val->get()->getEmitterSystemIdentifier() << std::endl;
	//std::cout <<  val->get()->getEventIdentifier().getIssuingObjectIdentifier() << std::endl;

	if (val->get()->hasEventIdentifier()) {
		if (m_MapObjects.size() > 0) {
			auto i = m_MapObjects.find(val->get()->getEventIdentifier().getIssuingObjectIdentifier());
			if (i != m_MapObjects.end()) {
				result = i->second;
			}
		}
	}

	return result;
}