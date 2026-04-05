#pragma once
#include <vector>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>
//#include <DevStudio/HlaAllHeaders.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstring>

namespace SDL_RTI {
	class uuidUtils
	{
	public:
		uuidUtils() {};
		~uuidUtils() {};
		enum uuidType : char
		{
			UUID_TYPE_ENTITY_IDENTIFIER,
			UUID_TYPE_TASK,
		};
		static std::string convert_uuid_to_string(std::vector<char> uuid)
		{
			std::ostringstream oss;
			oss << std::hex << std::setfill('0');

			for (char byte : uuid) {
				oss << std::setw(2) << static_cast<int>(byte);
			}

			return oss.str();
		}
		static std::vector<char> convert_string_to_uuid(const std::string& hexString)
		{
			std::vector<char> uuid;
			uuid.clear();
			if (hexString.length() % 2 != 0) {
				std::cerr << "convert_string_to_uuid: The string is incorrect." << std::endl;
				throw std::invalid_argument("The string is incorrect.");
			}
			for (size_t i = 0; i < hexString.length(); i += 2) {
				std::string byteString = hexString.substr(i, 2);
				uuid.push_back(static_cast<char>(std::stoul(byteString, nullptr, 16)));
			}
			for (int i = uuid.size(); i < 16; i++)
				uuid.push_back(0);

			return uuid;
		}
		static std::vector<char> convert_entityIdentifier_to_uuid(const DevStudio::EntityIdentifierStruct& entity)
		{
			std::string str =
			std::to_string(entity.federateIdentifier.siteID) + "." +
			std::to_string(entity.federateIdentifier.applicationID) + "." +
			std::to_string(entity.entityNumber);

			std::vector<char> ret(16, 0);

			for (size_t i = 0; i < str.size() && i < ret.size(); ++i)
			{
				ret[i] = static_cast<char>(static_cast<unsigned char>(str[i]));
			}

			return ret;
		}
		static DevStudio::EntityIdentifierStruct convert_uuid_to_entityIdentifier(const std::vector<char> uuid)
		{
			// Layout: [1 type][2 siteID][2 applicationID][2 entityNumber] = 7 bytes minimum
			if (uuid.size() < 7) {
				throw std::invalid_argument("convert_uuid_to_entityIdentifier: UUID buffer too small.");
			}
			if (static_cast<unsigned char>(uuid[0]) != uuidType::UUID_TYPE_ENTITY_IDENTIFIER)
			{
				throw std::invalid_argument("convert_uuid_to_entityIdentifier: Unexpected UUID type.");
			}
			DevStudio::EntityIdentifierStruct entity;
			int pace = 1;
			memcpy(&entity.federateIdentifier.siteID, uuid.data() + pace, sizeof(entity.federateIdentifier.siteID));
			pace += sizeof(entity.federateIdentifier.siteID);
			memcpy(&entity.federateIdentifier.applicationID, uuid.data() + pace, sizeof(entity.federateIdentifier.applicationID));
			pace += sizeof(entity.federateIdentifier.applicationID);
			memcpy(&entity.entityNumber, uuid.data() + pace, sizeof(entity.entityNumber));
			return entity;
		}
		static std::vector<char> convert_task_to_uuid(const short id, const short step)
		{
			std::vector<char> ret(16, 0);
			ret[0] = uuidType::UUID_TYPE_TASK;
			int pace = 1;
			memcpy(ret.data() + pace, &id, sizeof(id));
			pace += sizeof(id);
			memcpy(ret.data() + pace, &step, sizeof(step));
			return ret;
		}
		static void convert_uuid_to_task(const std::vector<char>& uuid, short& id, short& step)
		{
			if (uuid[0] != uuidType::UUID_TYPE_TASK)
			{
				throw std::invalid_argument("convert_uuid_to_task: The string is incorrect.");
			}
			int pace = 1;
			memcpy(&id, uuid.data() + pace, sizeof(id));
			pace += sizeof(id);
			memcpy(&step, uuid.data() + pace, sizeof(step));
		}
	};
}