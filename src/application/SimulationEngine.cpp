#include "SimulationEngine.h"
#include "FlightDynamicsEngine.h"
#include "EventBus.h"
#include <chrono>
#include <thread>
#include <QVariantMap>

namespace application {

SimulationEngine::SimulationEngine(QObject* parent)
    : QThread(parent)
{
    // Clear out any stale JSON state to start fresh
    m_scenario.stopMission();
    
    // For MVP demonstration, push an initial Create command instead of hardcoding
    enqueueCommand(std::make_unique<CmdCreateEntity>(
        1, "TestAircraft", 40.0, -3.0, 5000.0, 45.0, 250.0, "Fighter"
    ));
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
        
        if (auto* createCmd = dynamic_cast<CmdCreateEntity*>(cmd.get())) {
            domain::Entity newEntity;
            newEntity.id = createCmd->id;
            newEntity.name = createCmd->name;
            newEntity.latitude = createCmd->lat;
            newEntity.longitude = createCmd->lon;
            newEntity.altitude = static_cast<int>(createCmd->alt);
            newEntity.headingDegrees = createCmd->heading;
            newEntity.speedKnots = createCmd->speed;
            newEntity.type = createCmd->type;
            newEntity.flightDynamicsEnabled = true;
            newEntity.flightDynamicsMode = "jsbsim";
            
            m_scenario.addEntity(newEntity);
        }
        else if (auto* moveCmd = dynamic_cast<CmdAssignMoveTask*>(cmd.get())) {
            EntityTask task;
            task.enabled = true;
            task.taskType = "MoveToLocation";
            task.targetLatitude = moveCmd->targetLat;
            task.targetLongitude = moveCmd->targetLon;
            task.targetAltitudeMeters = static_cast<int>(moveCmd->targetAlt);
            task.targetSpeedKnots = moveCmd->targetSpeed;
            task.status = "Running";
            
            // For MVP, if targetEntityId is 1, just find TestAircraft
            QString targetName = (moveCmd->targetEntityId == 1) ? "TestAircraft" : "";
            if (!targetName.isEmpty()) {
                m_scenario.assignTask(targetName, task);
            }
        }
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
        // Run JSBSim step via ScenarioState wrapper
        m_scenario.advanceSimulation(actualDeltaTime.count());

        // Phase 4: Event emission
        emit tickComplete(actualDeltaTime.count());
        
        // MVP: Broadcast state of all entities via EventBus instead of direct Qt signals
        for (const auto& e : m_scenario.entities()) {
            EventBus::instance().publish(EventKinematicsUpdated(
                e.id, e.name, e.latitude, e.longitude, static_cast<double>(e.altitude), e.headingDegrees, e.speedKnots, e.team
            ));
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
