#pragma once

#include "Event.h"
#include <functional>
#include <vector>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <memory>

namespace application {

// A thread-safe Pub/Sub Event Bus for decoupling domain logic from infrastructure adapters
class EventBus {
public:
    using EventHandler = std::function<void(const IEvent&)>;

    // Singleton access
    static EventBus& instance() {
        static EventBus bus;
        return bus;
    }

    // Subscribe to a specific event type
    template<typename T>
    void subscribe(std::function<void(const T&)> handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_handlers[typeid(T)].push_back([handler](const IEvent& e) {
            handler(static_cast<const T&>(e));
        });
    }

    // Publish an event to all subscribers
    template<typename T>
    void publish(const T& event) {
        std::vector<EventHandler> handlersCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_handlers.find(typeid(T));
            if (it != m_handlers.end()) {
                handlersCopy = it->second;
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
    std::unordered_map<std::type_index, std::vector<EventHandler>> m_handlers;
};

} // namespace application