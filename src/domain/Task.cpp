#include "Task.h"
#include <QtMath>

namespace {
    constexpr double kEarthRadiusMeters = 6371000.0;

    double bearingDegrees(double latitude1, double longitude1, double latitude2, double longitude2) {
        const double lat1 = qDegreesToRadians(latitude1);
        const double lat2 = qDegreesToRadians(latitude2);
        const double deltaLongitude = qDegreesToRadians(longitude2 - longitude1);
        const double y = qSin(deltaLongitude) * qCos(lat2);
        const double x = qCos(lat1) * qSin(lat2) - qSin(lat1) * qCos(lat2) * qCos(deltaLongitude);
        double degrees = qRadiansToDegrees(qAtan2(y, x));
        while (degrees < 0.0) { degrees += 360.0; }
        return degrees;
    }

    double distanceMeters(double latitude1, double longitude1, double latitude2, double longitude2) {
        const double lat1 = qDegreesToRadians(latitude1);
        const double lon1 = qDegreesToRadians(longitude1);
        const double lat2 = qDegreesToRadians(latitude2);
        const double lon2 = qDegreesToRadians(longitude2);
        const double deltaLat = lat2 - lat1;
        const double deltaLon = lon2 - lon1;
        const double a = qPow(qSin(deltaLat / 2.0), 2.0) + qCos(lat1) * qCos(lat2) * qPow(qSin(deltaLon / 2.0), 2.0);
        const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
        return kEarthRadiusMeters * c;
    }
}

namespace domain {

// --- MoveToLocationTask ---

MoveToLocationTask::MoveToLocationTask(double targetLat, double targetLon, double targetAlt, double targetSpeedKnots)
    : m_targetLat(targetLat), m_targetLon(targetLon), m_targetAlt(targetAlt), m_targetSpeed(targetSpeedKnots)
{
}

ITask::State MoveToLocationTask::getState() const
{
    return m_state;
}

DesiredState MoveToLocationTask::evaluate(double currentLat, double currentLon, double currentAlt, double currentHeading, double dt)
{
    m_state = State::Running;
    
    double dist = distanceMeters(currentLat, currentLon, m_targetLat, m_targetLon);
    
    // Check completion
    if (dist < 200.0) {
        m_state = State::Completed;
        return {currentHeading, m_targetAlt, 0.0}; // Stop
    }
    
    // Calculate geometry
    double targetHeading = bearingDegrees(currentLat, currentLon, m_targetLat, m_targetLon);
    
    return {targetHeading, m_targetAlt, m_targetSpeed};
}


// --- TaskStack ---

void TaskStack::push(std::unique_ptr<ITask> task)
{
    m_stack.push_back(std::move(task));
}

void TaskStack::pop()
{
    if (!m_stack.empty()) {
        m_stack.pop_back();
    }
}

ITask* TaskStack::top() const
{
    if (m_stack.empty()) return nullptr;
    return m_stack.back().get();
}

bool TaskStack::isEmpty() const
{
    return m_stack.empty();
}

DesiredState TaskStack::evaluateTop(double currentLat, double currentLon, double currentAlt, double currentHeading, double dt)
{
    if (m_stack.empty()) {
        return {currentHeading, currentAlt, 0.0}; // Hold current state if no tasks
    }
    
    ITask* currentTask = m_stack.back().get();
    DesiredState desired = currentTask->evaluate(currentLat, currentLon, currentAlt, currentHeading, dt);
    
    // Auto-pop completed tasks
    if (currentTask->getState() == ITask::State::Completed || currentTask->getState() == ITask::State::Failed) {
        pop();
    }
    
    return desired;
}

} // namespace domain