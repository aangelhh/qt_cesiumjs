#pragma once

#include "DevStudio/HlaObjectInstanceBase.h"
#include "DevStudio/datatypes/EntityIdentifierStruct.h"

using namespace DevStudio;

namespace SDL_RTI
{
	namespace DataTypes
	{
	    class ObjectIdentifier
		{
		public:
			ObjectIdentifier() : objectType(HlaObjectInstanceBase::ObjectClassType::HLAOBJECT_ROOT), entityIdentifier() {};
			ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, void* entityIdentifier_) : objectType(objectType_)
			{
				entityIdentifier = entityIdentifier_;
			};
			ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, EntityIdentifierStruct entityIdentifier_);
			ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, char index, EntityIdentifierStruct entityIdentifier_);
			ObjectIdentifier(HlaObjectInstanceBase::ObjectClassType objectType_, char beamIdentifier, char emitterIndex, EntityIdentifierStruct entityIdentifier_);
			ObjectIdentifier(DevStudio::HlaWorldPtr world, std::string identifier);

			std::string GetObjectIdentifierString(DevStudio::HlaWorldPtr world);
			HlaObjectInstanceBase::ObjectClassType GetType() const { return objectType; }
			void* GetObjectIdentifier() const { return entityIdentifier; }
			std::shared_ptr<void> GetHlaObject(DevStudio::HlaWorldPtr world) const;
			

		private:
			HlaObjectInstanceBase::ObjectClassType objectType;
			void* entityIdentifier;
		};

		inline std::ostream& operator<<(std::ostream& os, const ObjectIdentifier& obj) {
			os << "Type: " << static_cast<HlaObjectInstanceBase::ObjectClassType>(obj.GetType());
			switch (obj.GetType())
			{
			case HlaObjectInstanceBase::ObjectClassType::EMITTER_SYSTEM:
			{
				auto emitterId = static_cast<std::tuple<char, DevStudio::EntityIdentifierStruct>*>(obj.GetObjectIdentifier());
				os << ", Emitter Index: " << std::get<0>(*emitterId ) << ", Entity Identifier: " << std::get<1>(*emitterId) << std::endl;
				break;
			}
			case HlaObjectInstanceBase::ObjectClassType::RADAR_BEAM:
			case HlaObjectInstanceBase::ObjectClassType::JAMMER_BEAM:
			{
				auto beamId = static_cast<std::tuple<char, char, DevStudio::EntityIdentifierStruct>*>(obj.GetObjectIdentifier());
				os << ", Beam Id: " << std::get<0>(*beamId) << ", Emitter Index: " << std::get<1>(*beamId) << ", Entity Identifier: " << std::get<2>(*beamId) << std::endl;
				break;
			}
			case HlaObjectInstanceBase::ObjectClassType::SENSOR:
			{
				auto sensorId = static_cast<DevStudio::EntityIdentifierStruct*>(obj.GetObjectIdentifier());
				os << ", Entity Identifier: " << *sensorId << std::endl;
				break;
			}
			default:
				break;
			}
			return os;
		}
	}
}