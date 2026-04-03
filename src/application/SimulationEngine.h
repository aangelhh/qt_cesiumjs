#pragma once

#include <QThread>
#include <mutex>
#include <queue>
#include <memory>
#include <QVariantMap>
#include "Command.h"
#include "ScenarioState.h"

namespace application {

class SimulationEngine : public QThread {
    Q_OBJECT

public:
    explicit SimulationEngine(QObject* parent = nullptr);
    ~SimulationEngine() override;

    // Enqueue a command from any thread
    void enqueueCommand(std::unique_ptr<ICommand> command);

    // Stop the simulation loop cleanly
    void stop();

signals:
    // Emitted at the end of each tick (60Hz)
    void tickComplete(double deltaTimeSecs);

    // Emitted to the MapBridge for UI updates
    void telemetryUpdated(const QVariantMap& trackData);

protected:
    void run() override;

private:
    std::mutex m_queueMutex;
    std::queue<std::unique_ptr<ICommand>> m_commandQueue;
    
    // The registry and state owner for all entities
    ScenarioState m_scenario;

    // Process all pending commands
    void drainCommands();
};

} // namespace application
