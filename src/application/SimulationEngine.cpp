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

    // Process commands outside the lock
    while (!localQueue.empty()) {
        auto cmd = std::move(localQueue.front());
        localQueue.pop();
        
        if (auto* createCmd = dynamic_cast<CmdCreateEntity*>(cmd.get())) {
            Entity newEntity;
            newEntity.name = createCmd->name;
            newEntity.latitude = createCmd->lat;
            newEntity.longitude = createCmd->lon;
            newEntity.altitude = static_cast<int>(createCmd->alt);
            newEntity.headingDegrees = createCmd->heading;
            newEntity.speedKnots = 0.0;
            newEntity.verticalSpeedMetersPerSecond = 0.0;
            newEntity.type = createCmd->type;
            newEntity.flightDynamicsEnabled = false;
            newEntity.flightDynamicsMode = "kinematic";
            // newEntity.id doesn't exist on Entity, we rely on name as key

            
            if (m_scenario) {
                m_scenario->addEntity(newEntity);
            }
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
            
            QString targetName = moveCmd->targetEntityName.trimmed();
            if (targetName.isEmpty() && moveCmd->targetEntityId == 1) {
                targetName = "TestAircraft";
            }
            
            if (!targetName.isEmpty() && m_scenario) {
                m_scenario->assignTask(targetName, task);
                
                // Handle specific UI Task types by pushing onto the TaskStack
                domain::TaskStack* stack = m_scenario->getTaskStack(targetName);
                if (stack) {
                    while (!stack->isEmpty()) {
                        stack->pop();
                    }
                    
                    stack->push(std::make_unique<domain::MoveToLocationTask>(
                        task.targetLatitude, task.targetLongitude,
                        task.targetAltitudeMeters, task.targetSpeedKnots
                    ));
                }
            }
        }
        else if (auto* flyCmd = dynamic_cast<CmdAssignFlyHeadingTask*>(cmd.get())) {
            QString targetName = flyCmd->targetEntityName.trimmed();
            if (!targetName.isEmpty() && m_scenario) {
                EntityTask task;
                task.enabled = true;
                task.taskType = "FlyHeadingAltitudeSpeed";
                task.targetHeadingDegrees = flyCmd->targetHeadingDegrees;
                task.targetAltitudeMeters = static_cast<int>(flyCmd->targetAltitudeMeters);
                task.targetSpeedKnots = flyCmd->targetSpeedKnots;
                task.status = "Running";
                m_scenario->assignTask(targetName, task);

                domain::TaskStack* stack = m_scenario->getTaskStack(targetName);
                if (stack) {
                    while (!stack->isEmpty()) {
                        stack->pop();
                    }
                    stack->push(std::make_unique<domain::FlyHeadingAltitudeSpeedTask>(
                        task.targetHeadingDegrees,
                        static_cast<double>(task.targetAltitudeMeters),
                        task.targetSpeedKnots
                    ));
                }
            }
        }
        else if (auto* followCmd = dynamic_cast<CmdAssignFollowTask*>(cmd.get())) {
            QString targetName = followCmd->targetEntityName.trimmed();
            if (!targetName.isEmpty() && m_scenario) {
                EntityTask task;
                task.enabled = true;
                task.taskType = "FollowEntity";
                task.targetEntityName = followCmd->followEntityName;
                task.targetAltitudeMeters = static_cast<int>(followCmd->targetAltitudeMeters);
                task.targetSpeedKnots = followCmd->targetSpeedKnots;
                task.status = "Running";
                m_scenario->assignTask(targetName, task);

                domain::TaskStack* stack = m_scenario->getTaskStack(targetName);
                if (stack) {
                    while (!stack->isEmpty()) {
                        stack->pop();
                    }
                    stack->push(std::make_unique<domain::FollowEntityTask>(
                        static_cast<double>(task.targetAltitudeMeters),
                        task.targetSpeedKnots
                    ));
                }
            }
        }
        else if (auto* orbitCmd = dynamic_cast<CmdAssignOrbitTask*>(cmd.get())) {
            QString targetName = orbitCmd->targetEntityName.trimmed();
            if (!targetName.isEmpty() && m_scenario) {
                EntityTask task;
                task.enabled = true;
                task.taskType = orbitCmd->isPatrol ? "PatrolArea" : "OrbitArea";
                task.targetLatitude = orbitCmd->targetLatitude;
                task.targetLongitude = orbitCmd->targetLongitude;
                task.targetAreaRadiusMeters = orbitCmd->targetAreaRadiusMeters;
                task.targetAltitudeMeters = static_cast<int>(orbitCmd->targetAltitudeMeters);
                task.targetSpeedKnots = orbitCmd->targetSpeedKnots;
                task.status = "Running";
                m_scenario->assignTask(targetName, task);

                domain::TaskStack* stack = m_scenario->getTaskStack(targetName);
                if (stack) {
                    while (!stack->isEmpty()) {
                        stack->pop();
                    }
                    stack->push(std::make_unique<domain::OrbitAreaTask>(
                        task.targetLatitude,
                        task.targetLongitude,
                        task.targetAreaRadiusMeters,
                        static_cast<double>(task.targetAltitudeMeters),
                        task.targetSpeedKnots,
                        orbitCmd->isPatrol
                    ));
                }
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
