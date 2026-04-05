#pragma once
#include "Singleton.h"
#include <string>
#include <map>
#include <memory>
//#include "../common/Estructuras/EntityType.h"
//#include "../common/Attributes/EntityTypeStruct.h"

class SisoManager : public events::Singleton<SisoManager>
{
public:
	SisoManager();
	~SisoManager();

	void sisoParser();
	//std::string getSisoName(DataTypes::EntityTypeStruct entityTypeStruct);

	bool hasRead();
private:
	std::map<std::string, std::string> m_MapSisoEntities;
	
};

