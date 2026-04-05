#ifndef EVENTS_ON_DATA_AVAILABLE_OBJECT_HPP
#define EVENTS_ON_DATA_AVAILABLE_OBJECT_HPP


class EventOnDataAvailableObject {

    public:
        template <typename T>
        void setEventOnDataAvailableObject(std::shared_ptr<T> userData) {
            m_UserData = std::static_pointer_cast<void>(userData);
        }
       
        std::shared_ptr<void> getEventOnDataAvailableObject() const {
            return m_UserData;
        }

        void setActionObject(int actionObject) {
            m_ActionEnum = actionObject;
        }

        int getActionObject() const {
            return m_ActionEnum;
        }

        void setObjectObject(int objectEnum) {
            m_ObjectEnum = objectEnum;
        }

        int getObjectObject() const {
            return m_ObjectEnum;
        }

        void setObjectName(std::wstring objectName) {
            m_ObjectName = objectName;
        }

        std::wstring getObjectName() const {
            return m_ObjectName;
        }

        DevStudio::HlaFederateIdPtr* getFederateName() const {
            return m_FederateName;
        }

        void setFederateName(DevStudio::HlaFederateIdPtr objectName) {
            m_FederateName = &objectName;
        }

		void setModifiedAttributesList(std::vector<int> modifiedAttributesList) 
        {
			m_ModifiedAttributesList = modifiedAttributesList;
		}

		std::vector<int> getModifiedAttributesList() const
		{
			return m_ModifiedAttributesList;
		}

		void addModifiedAttribute(int modifiedAttribute)
		{
			m_ModifiedAttributesList.push_back(modifiedAttribute);
		}

private:
    std::shared_ptr<void> m_UserData; // Usa shared_ptr<void> en lugar de void*
    int m_ActionEnum;
    int m_ObjectEnum;
    std::vector<int> m_ModifiedAttributesList;
    std::wstring m_ObjectName;
    DevStudio::HlaFederateIdPtr* m_FederateName;
};

#endif // EVENTS_ON_DATA_AVAILABLE_OBJECT_HPP