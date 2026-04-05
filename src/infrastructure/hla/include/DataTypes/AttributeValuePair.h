#pragma once

#include <DevStudio/HlaObjectInstanceBase.h>
#include <DevStudio/datatypes/AttributeValuePairStruct.h>

using namespace DevStudio;

namespace SDL_RTI
{
	namespace DataTypes
	{
		class AttributeValuePair
		{
		private:
			HlaObjectInstanceBase::ObjectClassType objectType;
			int attribute;
			std::string attributeName;
			void* data;

			void decode(const AttributeValuePairStruct& pairStruct);
			std::vector<char> encode();

		public:
			AttributeValuePair(HlaObjectInstanceBase::ObjectClassType objectType_, int attribute_, void* data_);
			AttributeValuePair(HlaObjectInstanceBase::ObjectClassType objectType_, std::string attributeName, void* data_);
			AttributeValuePair(HlaObjectInstanceBase::ObjectClassType objectType_, const AttributeValuePairStruct& pairStruct);

			AttributeValuePairStruct getAttributeValuePairStruct();

			static std::vector<AttributeValuePair> getVector(const HlaObjectInstanceBase::ObjectClassType& classType, const std::vector<AttributeValuePairStruct>& attributeValueSet);

			HlaObjectInstanceBase::ObjectClassType& getType()
			{
				return objectType;
			}

			void* getData() const
			{
				return data;
			};
			int getAttribute()
			{
				return attribute;
			};
			std::string getAttributeName() const
			{
				return attributeName;
			};
		};
	}
}