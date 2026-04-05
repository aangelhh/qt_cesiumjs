#ifndef EVENTBUS_HPP
#define EVENTBUS_HPP


#include "../utils/Singleton.h"
#include "EventListener.h"
#include "EventSource.h"
#include "EventOnDataAvailableInteraction.h"
#include "EventInteraction.h"
#include "EventOnDataAvailableObject.h"
#include "EventObject.h"

namespace events
{
    class EventBus
        : public events::EventSource<EventOnDataAvailableInteraction>
        , public events::EventSource<EventInteraction>
        , public events::EventSource<EventOnDataAvailableObject>
        , public events::EventSource<EventObject>
        , public events::Singleton<EventBus>
    {
        public:
            using events::EventSource<EventOnDataAvailableInteraction>::dispatchEvent;
            using events::EventSource<EventInteraction>::dispatchEvent;
            using events::EventSource<EventOnDataAvailableObject>::dispatchEvent;
            using events::EventSource<EventObject>::dispatchEvent;
    };
}
#endif // EVENTBUS_HPP