#pragma once

#include "Event.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace application {

// Opaque handle returned by subscribe<T>() that allows callers to later
// unsubscribe. Wrapping the IDs in distinct per-event-type tags would be ideal
// but a simple monotonically-increasing integer is sufficient and matches the
// scale of subscriptions in this app.
using SubscriptionId = std::uint64_t;

// A thread-safe Pub/Sub Event Bus for decoupling domain logic from infrastructure adapters
class EventBus {
public:
    using EventHandler = std::function<void(const IEvent&)>;

    struct Subscription {
        SubscriptionId id = 0;
        EventHandler handler;
    };

    // Singleton access
    static EventBus& instance() {
        static EventBus bus;
        return bus;
    }

    // Subscribe to a specific event type. Returns an id usable with unsubscribe<T>().
    template<typename T>
    SubscriptionId subscribe(std::function<void(const T&)> handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        const SubscriptionId id = ++m_nextId;
        m_handlers[typeid(T)].push_back(Subscription{
            id,
            [h = std::move(handler)](const IEvent& e) {
                h(static_cast<const T&>(e));
            }
        });
        return id;
    }

    // Remove a previously installed handler. Safe to call with an unknown id.
    template<typename T>
    void unsubscribe(SubscriptionId id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.find(typeid(T));
        if (it == m_handlers.end()) {
            return;
        }
        auto& subs = it->second;
        subs.erase(std::remove_if(subs.begin(), subs.end(),
                                  [id](const Subscription& s) { return s.id == id; }),
                   subs.end());
    }

    // Publish an event to all subscribers
    template<typename T>
    void publish(const T& event) {
        std::vector<EventHandler> handlersCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_handlers.find(typeid(T));
            if (it != m_handlers.end()) {
                handlersCopy.reserve(it->second.size());
                for (const Subscription& s : it->second) {
                    handlersCopy.push_back(s.handler);
                }
            }
        }

        // Execute handlers outside the lock to prevent deadlocks
        for (const auto& handler : handlersCopy) {
            handler(event);
        }
    }

private:
    EventBus() = default;

    std::mutex m_mutex;
    std::unordered_map<std::type_index, std::vector<Subscription>> m_handlers;
    SubscriptionId m_nextId = 0;
};

} // namespace application