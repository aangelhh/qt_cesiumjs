// Unit tests for the application::EventBus pub/sub utility.

#include "application/Event.h"
#include "application/EventBus.h"

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

namespace {

struct EventA : public application::IEvent {
    int payload = 0;
};

struct EventB : public application::IEvent {
    int payload = 0;
};

} // namespace

TEST(EventBus, DeliversToMatchingSubscriber) {
    auto& bus = application::EventBus::instance();
    int received = 0;
    const auto id = bus.subscribe<EventA>(
        [&received](const EventA& e) { received = e.payload; });

    EventA ev;
    ev.payload = 42;
    bus.publish(ev);
    EXPECT_EQ(received, 42);

    bus.unsubscribe<EventA>(id);
}

TEST(EventBus, IgnoresEventsOfOtherTypes) {
    auto& bus = application::EventBus::instance();
    int receivedA = 0;
    int receivedB = 0;
    const auto idA = bus.subscribe<EventA>(
        [&receivedA](const EventA& e) { receivedA = e.payload; });
    const auto idB = bus.subscribe<EventB>(
        [&receivedB](const EventB& e) { receivedB = e.payload; });

    EventA ea;
    ea.payload = 7;
    bus.publish(ea);
    EXPECT_EQ(receivedA, 7);
    EXPECT_EQ(receivedB, 0);

    EventB eb;
    eb.payload = 9;
    bus.publish(eb);
    EXPECT_EQ(receivedA, 7);
    EXPECT_EQ(receivedB, 9);

    bus.unsubscribe<EventA>(idA);
    bus.unsubscribe<EventB>(idB);
}

TEST(EventBus, UnsubscribeStopsDelivery) {
    auto& bus = application::EventBus::instance();
    int callCount = 0;
    const auto id = bus.subscribe<EventA>(
        [&callCount](const EventA&) { ++callCount; });

    EventA ev;
    bus.publish(ev);
    EXPECT_EQ(callCount, 1);

    bus.unsubscribe<EventA>(id);
    bus.publish(ev);
    EXPECT_EQ(callCount, 1);
}

TEST(EventBus, UnsubscribeWithUnknownIdIsHarmless) {
    auto& bus = application::EventBus::instance();
    // Should not crash or throw.
    bus.unsubscribe<EventA>(0);
    bus.unsubscribe<EventA>(999999);
    SUCCEED();
}

TEST(EventBus, ConcurrentPublishersDoNotDeadlock) {
    auto& bus = application::EventBus::instance();
    std::atomic<int> callCount{0};
    const auto id = bus.subscribe<EventA>(
        [&callCount](const EventA&) { callCount.fetch_add(1, std::memory_order_relaxed); });

    constexpr int kThreads = 4;
    constexpr int kPerThread = 250;
    std::vector<std::thread> workers;
    workers.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([&bus]() {
            for (int i = 0; i < kPerThread; ++i) {
                EventA ev;
                ev.payload = i;
                bus.publish(ev);
            }
        });
    }
    for (auto& w : workers) {
        w.join();
    }

    EXPECT_EQ(callCount.load(), kThreads * kPerThread);
    bus.unsubscribe<EventA>(id);
}
