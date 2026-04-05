#include "SisoManager.h"
//#include <tchar.h>
//#using <mscorlib.dll>
//#using <System.xml.dll>
//#include <msclr\marshal_cppstd.h>
#include <iostream>
//#include "../common/Attributes/EntityTypeStruct.h"

//using namespace System;
//using namespace System::Xml;
//using namespace msclr::interop;
//using namespace DataTypes;


SisoManager::SisoManager()
{
}

SisoManager::~SisoManager()
{
}

void SisoManager::sisoParser()
{
	//String^ filePath = "EntitySiso.xml";

	//try
	//{
	//	XmlTextReader^ reader = gcnew XmlTextReader(filePath);

	//	std::string currentEntityName;
	//	int idEntity = 1;
	//	int kind, domain, country, category, subcat, spec, extra;
	//	std::string sisoName = "";
	//	std::string sisoID = "";

	//	while (reader->Read())
	//	{
	//		if (reader->NodeType == XmlNodeType::Element && reader->Name == "entity")
	//		{
	//			while (reader->Read())
	//			{
	//				if (reader->NodeType == XmlNodeType::Element)
	//				{
	//					String^ elementName = reader->Name;
	//					reader->Read();

	//					if (elementName == "name")
	//					{
	//						sisoName = marshal_as<std::string>(reader->Value);
	//					}
	//					else if (elementName == "entityKind")
	//					{
	//						kind = Convert::ToInt32(reader->Value);
	//					}
	//					else if (elementName == "domain")
	//					{
	//						domain = (Convert::ToInt32(reader->Value));
	//					}
	//					else if (elementName == "countryCode")
	//					{
	//						country = Convert::ToInt32(reader->Value);
	//					}
	//					else if (elementName == "category")
	//					{
	//						category = Convert::ToInt32(reader->Value);
	//					}
	//					else if (elementName == "subcategory")
	//					{
	//						subcat = Convert::ToInt32(reader->Value);
	//					}
	//					else if (elementName == "specific")
	//					{
	//						spec = Convert::ToInt32(reader->Value);
	//					}
	//					else if (elementName == "extra")
	//					{
	//						extra = Convert::ToInt32(reader->Value);
	//					}
	//				}
	//				else if (reader->NodeType == XmlNodeType::EndElement && reader->Name == "entity")
	//				{
	//					sisoID = std::to_string(kind) + ":" + std::to_string(domain) + ":" + std::to_string(country)
	//						+ ":" + std::to_string(category) + ":" + std::to_string(subcat) + ":" + std::to_string(spec)
	//						+ ":" + std::to_string(extra);
	//					m_MapSisoEntities.insert(make_pair(sisoID, sisoName));
	//					//std::cout << "added entity " << sisoName << " with siso " << sisoID << std::endl;
	//					break;
	//				}
	//			}
	//		}
	//	}
	//	reader->Close();
	//}
	//catch (System::Exception^ e)
	//{
	//	Console::WriteLine("Error: " + e->Message);
	//}
}

//std::string SisoManager::getSisoName(DataTypes::EntityTypeStruct entityType)
//{
//	std::string nameEntity = "Not found";
//	int kind, domain, country, category, subcat, spec, extra;
//
//	//if (!entityType)
//	//	return "ERROR: NULL OBJECT!!!";
//
//	kind = (entityType.getEntityKind());
//	domain = (entityType.getDomain());
//	country = entityType.getCountryCode();
//	category = (entityType.getCategory());
//	subcat = (entityType.getSubcategory());
//	spec = (entityType.getSpecific());
//	extra = (entityType.getExtra());
//
//	std::string sisoID = std::to_string(kind) + ":" + std::to_string(domain) + ":" + std::to_string(country)
//		+ ":" + std::to_string(category) + ":" + std::to_string(subcat) + ":" + std::to_string(spec)
//		+ ":" + std::to_string(extra);
//
//	if (m_MapSisoEntities.find(sisoID) != m_MapSisoEntities.end()) {
//		nameEntity = m_MapSisoEntities[sisoID];
//	}
//
//	return nameEntity;
//}

bool SisoManager::hasRead()
{
	if (m_MapSisoEntities.size() > 0)
		return true;
	else
		return false;
}
