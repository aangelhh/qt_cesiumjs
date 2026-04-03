#include "SimulationEngine.h"
#include "FlightDynamicsEngine.h"
#include <chrono>
#include <thread>
#include <QVariantMap>

namespace application {

SimulationEngine::SimulationEngine(QObject* parent)
    : QThread(parent)
{
    // Initialize a test entity for the MVP
    domain::Entity testEntity;
    testEntity.id = 1;
    testEntity.name = "TestAircraft";
    testEntity.latitude = 40.0;
    testEntity.longitude = -3.0;
    testEntity.altitude = 5000.0;
    testEntity.headingDegrees = 45.0;
    testEntity.speedKnots = 250.0;
    testEntity.flightDynamicsEnabled = true;
    testEntity.flightDynamicsMode = "jsbsim"; // Trigger JSBSim fallback logic
    
    m_entities.append(testEntity);
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

    // Process commands outside the lock
    while (!localQueue.empty()) {
        auto cmd = std::move(localQueue.front());
        localQueue.pop();

        // TODO: Execute the command on the domain state
    }
}

void SimulationEngine::run()
{
    using namespace std::chrono;

    // Target 60 FPS -> 16.666 ms per frame
    const nanoseconds targetDeltaTime(16666666); 
    
    auto lastTime = steady_clock::now();

    while (!isInterruptionRequested()) {
        auto currentTime = steady_clock::now();
        duration<double> actualDeltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Phase 1: Drain input commands
        drainCommands();

        // Phase 2: AI / Sensor logic (TODO)
        
        // Phase 3: Task & Physics Evaluation
        // Run JSBSim step via FlightDynamicsEngine
        FlightDynamicsEngine::advanceEntities(m_entities, actualDeltaTime.count());

        // Phase 4: Event emission
        emit tickComplete(actualDeltaTime.count());
        
        // MVP: Broadcast state of our test entity
        if (!m_entities.isEmpty()) {
            const auto& e = m_entities.first();
            QVariantMap trackData;
            trackData["id"] = e.id;
            trackData["name"] = e.name;
            trackData["latitude"] = e.latitude;
            trackData["longitude"] = e.longitude;
            trackData["altitude"] = e.altitude;
            trackData["headingDegrees"] = e.headingDegrees;
            trackData["speedKnots"] = e.speedKnots;
            trackData["team"] = "blue"; // Friendly
            
            emit telemetryUpdated(trackData);
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
