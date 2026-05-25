#include "SimulationEngine.h"
#include "FlightDynamicsEngine.h"
#include "EventBus.h"
#include <chrono>
#include <thread>
#include <QVariantMap>

namespace application {

SimulationEngine::SimulationEngine(ScenarioState* scenarioState, QObject* parent)
    : QThread(parent),
      m_scenario(scenarioState)
{
    // Clear out any stale JSON state to start fresh
    if (m_scenario) {
        m_scenario->stopMission();
    }
}

SimulationEngine::~SimulationEngine()
{
    stop();
    wait(); // Wait for the thread to finish
}

void SimulationEngine::enqueueCommand(std::unique_ptr<ICommand> command)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_commandQueue.push(std::move(command));
}

void SimulationEngine::stop()
{
    requestInterruption();
}

void SimulationEngine::drainCommands()
{
    std::queue<std::unique_ptr<ICommand>> localQueue;

    {
        // Fast lock and swap to minimize contention with UI/Network threads
        std::lock_guard<std::mutex> lock(m_queueMutex);
        std::swap(m_commandQueue, localQueue);
    }

    // Process commands outside the lock via polymorphic dispatch.
    while (!localQueue.empty()) {
        auto cmd = std::move(localQueue.front());
        localQueue.pop();

        if (cmd && m_scenario) {
            cmd->apply(*m_scenario);
        }
    }
}

void SimulationEngine::run()
{
    using namespace std::chrono;

    // Target 60 FPS -> 16.666 ms per frame
    const nanoseconds targetDeltaTime(16666666); 
    
    auto lastTime = steady_clock::now();
    double timeSinceLastUiUpdate = 0.0;

    while (!isInterruptionRequested()) {
        auto currentTime = steady_clock::now();
        duration<double> actualDeltaTime = currentTime - lastTime;
        lastTime = currentTime;
        timeSinceLastUiUpdate += actualDeltaTime.count();

        // Phase 1: Drain input commands
        drainCommands();

        // Phase 2: AI / Sensor logic (TODO)
        
        // Phase 3: Task & Physics Evaluation
        if (m_scenario) {
            m_scenario->advanceSimulation(actualDeltaTime.count());
        }

        // Phase 4: Event emission
        emit tickComplete(actualDeltaTime.count());
        
        // Throttle UI updates to 10Hz (every ~0.1s)
        if (timeSinceLastUiUpdate >= 0.1) {
            // MVP: Broadcast state of all entities via EventBus instead of direct Qt signals
            if (m_scenario) {
                for (const auto& e : m_scenario->entities()) {
                    QString teamLabel;
                    switch(e.forceIdentifier) {
                        case 1: teamLabel = "Friendly"; break;
                        case 2: teamLabel = "Opposing"; break;
                        case 3: teamLabel = "Neutral"; break;
                        default: teamLabel = "Unknown"; break;
                    }
                    
                    EventBus::instance().publish(EventKinematicsUpdated(
                        // Use a default ID or hash since Entity doesn't store an ID currently
                        1, e.name, e.latitude, e.longitude, static_cast<double>(e.altitude), e.headingDegrees, e.speedKnots, teamLabel
                    ));
                }
            }
            timeSinceLastUiUpdate = 0.0;
        }

        // Sleep to maintain the target 60Hz tick rate
        auto workEndTime = steady_clock::now();
        auto workDuration = workEndTime - currentTime;
        
        auto sleepTime = targetDeltaTime - workDuration;
        
        if (sleepTime > nanoseconds::zero()) {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

} // namespace application
