#ifndef EVENTS_EVENTLISTENER_HPP
#define EVENTS_EVENTLISTENER_HPP

namespace events {

template <typename Event>
class EventListener
{
public:
   virtual ~EventListener()
   {
   }

   /// This method is called every time the listener receives an event.
   virtual void onEventReceived(const Event& event) = 0;
};

} // namespace events

#endif // EVENTS_EVENTLISTENER_HPP
