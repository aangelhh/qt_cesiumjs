#ifndef EVENTS_EVENTSOURCE_HPP
#define EVENTS_EVENTSOURCE_HPP

#include "EventListener.h"
#include <string>
#include <map>

namespace events {

template <class Event>
class EventSource
{
public:
   virtual ~EventSource()
   {
   }

   /// Register the eventListener
   void addEventListener(EventListener<Event>& eventListener, const int data) const
   {  
      mapEventListeners.insert(std::make_pair(data, &eventListener));
   }

   /// Unregister the eventListener
   void removeEventListener(EventListener<Event>& eventListener, int data) const
   {
        auto it = mapEventListeners.begin();
        
        while (it != mapEventListeners.end()) 
        {
            if (it->second == &eventListener && it->first == data)
            {
                it = mapEventListeners.erase(it);
            }
            else 
            {
                ++it;
            }
        }
   }

   //All listeners are notified that the event has been registered
   void dispatchEvent(const Event& event, int data) const
   {
        auto iter = mapEventListeners.find(data);

        if (iter != mapEventListeners.end())
        {
            
            while (iter != mapEventListeners.end() && iter->first == data)
            {
                iter->second->onEventReceived(event);
                ++iter;
            }
        }
    }

   EventSource& operator = (const EventSource &otherEventSource)
   {
      mapEventListeners = otherEventSource.mapEventListeners;
      return *this;
   }

protected:
   EventSource()
   {
   }

   EventSource(const EventSource& eventSource)
   {
      mapEventListeners = eventSource.mapEventListeners;
   }

private:
   typedef std::multimap<int, EventListener<Event>*> m_MapEventListenerList;
   mutable m_MapEventListenerList mapEventListeners;

};

//The connect function is used to register the listener event
template <class Event>
void connect(const EventSource<Event>& eventSource, EventListener<Event>& eventlistener, int data)
{
   eventSource.addEventListener(eventlistener, data);
}

//The disconnect function is used to unregister the listener event
template <class Event>
void disconnect(const EventSource<Event>& eventSource, EventListener<Event>& eventlistener, int data)
{
   eventSource.removeEventListener(eventlistener, data);
}

}

#endif // EVENTS_EVENTSOURCE_HPP