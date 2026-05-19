#pragma once

#include <QObject>
#include <QThread>
#include <mutex>
#include <queue>
#include <memory>
#include <QVariantMap>
#include "Command.h"
#include "ScenarioState.h"

namespace application {

class SimulationEngine : public QObject {
    Q_OBJECT

public:
    explicit SimulationEngine(ScenarioState* scenarioState, QObject* parent = nullptr);
    ~SimulationEngine() override;

    // Enqueue a command from any thread
    void enqueueCommand(std::unique_ptr<ICommand> command);

    // Stop the simulation loop cleanly
    void start();
    void stop();

signals:
    // Emitted at the end of each tick (60Hz)
    void tickComplete(double deltaTimeSecs);

    // Emitted to the MapBridge for UI updates
    void telemetryUpdated(const QVariantMap& trackData);

private:
    void runLoop();
    std::mutex m_queueMutex;
    std::queue<std::unique_ptr<ICommand>> m_commandQueue;
    QThread m_workerThread;
    
    // The registry and state owner for all entities (borrowed from MainWindow)
    ScenarioState* m_scenario;

    // Process all pending commands
    void drainCommands();
};

} // namespace application
