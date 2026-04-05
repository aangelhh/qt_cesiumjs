#ifndef EVENTS_OBJECT_HPP
#define EVENTS_OBJECT_HPP


#include <string>

class EventObject {

    public:
        void setEventInteraction(void* userData)
        {
            m_UserData = userData;
        }
        
        void* getInteractionDataTypes() const
        {
            return m_UserData;
        }

    private:
        void* m_UserData;
};

#endif // EVENTS_OBJECT_HPP