#ifndef EVENTS_INTERACTION_HPP
#define EVENTS_INTERACTION_HPP

#include <string>


namespace events {
    class EventInteraction {

        public:
            void setEventInteraction(void* interaction)
            {
                m_UserData = interaction;
            }
        
            void* getInteractionDataTypes() const
            {
                return m_UserData;
            }

        private:
            void* m_UserData;
    };
}

#endif // EVENTS_PARTICIPANTS_HPP