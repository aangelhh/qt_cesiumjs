#include "Task.h"
#include <QtMath>

#include <limits>

namespace {
    constexpr double kEarthRadiusMeters = 6371000.0;
    constexpr double kPatrolArrivalThresholdMeters = 200.0;
    constexpr int kGeneratedPatrolPointCount = 6;

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

    RoutePoint offsetPointMeters(
        double centerLatitude,
        double centerLongitude,
        double northMeters,
        double eastMeters,
        double altitudeMeters) {
        const double latitudeRadians = qDegreesToRadians(centerLatitude);
        const double deltaLatitudeRadians = northMeters / kEarthRadiusMeters;
        const double cosLatitude = qMax(0.000001, qCos(latitudeRadians));
        const double deltaLongitudeRadians =
            eastMeters / (kEarthRadiusMeters * cosLatitude);

        RoutePoint point;
        point.latitude = centerLatitude + qRadiansToDegrees(deltaLatitudeRadians);
        point.longitude = centerLongitude + qRadiansToDegrees(deltaLongitudeRadians);
        point.altitudeMeters = altitudeMeters;
        return point;
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

// --- RouteTask ---

RouteTask::RouteTask(const QVector<RoutePoint>& points, double targetSpeedKnots)
    : m_points(points), m_targetSpeed(targetSpeedKnots)
{
}

ITask::State RouteTask::getState() const
{
    return m_state;
}

DesiredState RouteTask::evaluate(
    double currentLat,
    double currentLon,
    double currentAlt,
    double currentHeading,
    double dt)
{
    Q_UNUSED(dt);

    if (m_points.isEmpty()) {
        m_state = State::Failed;
        return {currentHeading, currentAlt, 0.0};
    }

    if (m_currentPointIndex < 0) {
        m_currentPointIndex = 0;
    }

    if (m_currentPointIndex >= m_points.size()) {
        m_state = State::Completed;
        const RoutePoint& finalPoint = m_points.last();
        return {currentHeading, finalPoint.altitudeMeters, 0.0};
    }

    m_state = State::Running;

    RoutePoint targetPoint = m_points.at(m_currentPointIndex);
    double distanceToPoint = distanceMeters(
        currentLat, currentLon, targetPoint.latitude, targetPoint.longitude);

    while (distanceToPoint < 200.0 && m_currentPointIndex < m_points.size()) {
        ++m_currentPointIndex;
        if (m_currentPointIndex >= m_points.size()) {
            m_state = State::Completed;
            return {currentHeading, targetPoint.altitudeMeters, 0.0};
        }
        targetPoint = m_points.at(m_currentPointIndex);
        distanceToPoint = distanceMeters(
            currentLat, currentLon, targetPoint.latitude, targetPoint.longitude);
    }

    const double targetHeading = bearingDegrees(
        currentLat,
        currentLon,
        targetPoint.latitude,
        targetPoint.longitude);
    return {targetHeading, targetPoint.altitudeMeters, m_targetSpeed};
}

// --- FlyHeadingAltitudeSpeedTask ---

FlyHeadingAltitudeSpeedTask::FlyHeadingAltitudeSpeedTask(double targetHeading, double targetAlt, double targetSpeedKnots)
    : m_targetHeading(targetHeading), m_targetAlt(targetAlt), m_targetSpeed(targetSpeedKnots)
{
}

ITask::State FlyHeadingAltitudeSpeedTask::getState() const
{
    return m_state;
}

DesiredState FlyHeadingAltitudeSpeedTask::evaluate(double currentLat, double currentLon, double currentAlt, double currentHeading, double dt)
{
    m_state = State::Running;
    return {m_targetHeading, m_targetAlt, m_targetSpeed};
}

// --- FollowEntityTask ---

FollowEntityTask::FollowEntityTask(double targetAlt, double targetSpeedKnots)
    : m_fallbackAlt(targetAlt), m_fallbackSpeed(targetSpeedKnots)
{
}

void FollowEntityTask::updateTargetLocation(double targetLat, double targetLon, double targetAlt, double targetSpeedKnots)
{
    m_targetLat = targetLat;
    m_targetLon = targetLon;
    m_targetAlt = targetAlt;
    m_targetSpeed = targetSpeedKnots;
    m_hasTargetData = true;
}

ITask::State FollowEntityTask::getState() const
{
    return m_state;
}

DesiredState FollowEntityTask::evaluate(double currentLat, double currentLon, double currentAlt, double currentHeading, double dt)
{
    if (!m_hasTargetData) {
        m_state = State::Failed; // Target unavailable
        return {currentHeading, currentAlt, 0.0};
    }
    
    m_state = State::Running;
    
    double targetHeading = bearingDegrees(currentLat, currentLon, m_targetLat, m_targetLon);
    double dist = distanceMeters(currentLat, currentLon, m_targetLat, m_targetLon);
    
    double targetSpeed = qMax(m_targetSpeed, m_fallbackSpeed);
    double assignedSpeed = (dist > 1500.0) ? targetSpeed + 40.0 : targetSpeed;
    double assignedAlt = (m_fallbackAlt > 0) ? m_fallbackAlt : m_targetAlt;
    
    return {targetHeading, assignedAlt, assignedSpeed};
}

// --- OrbitAreaTask ---

OrbitAreaTask::OrbitAreaTask(double centerLat, double centerLon, double radiusMeters, double targetAlt, double targetSpeedKnots, bool isPatrol)
    : m_centerLat(centerLat), m_centerLon(centerLon), m_radiusMeters(radiusMeters), m_targetAlt(targetAlt), m_targetSpeed(targetSpeedKnots), m_isPatrol(isPatrol)
{
}

ITask::State OrbitAreaTask::getState() const
{
    return m_state;
}

DesiredState OrbitAreaTask::evaluate(double currentLat, double currentLon, double currentAlt, double currentHeading, double dt)
{
    m_state = State::Running;
    
    double distToCenter = distanceMeters(currentLat, currentLon, m_centerLat, m_centerLon);
    double areaRadius = qMax(100.0, m_radiusMeters);
    double orbitRadius = m_isPatrol ? qMax(200.0, areaRadius * 0.55) : qMax(150.0, areaRadius * 0.75);
    double approachRadius = orbitRadius * 1.15;
    
    double assignedHeading = currentHeading;
    
    if (distToCenter > approachRadius) {
        // First move toward the area center; only start the pattern once near the orbit/patrol lane.
        assignedHeading = bearingDegrees(currentLat, currentLon, m_centerLat, m_centerLon);
    } else {
        // Orbiting / Patrolling
        double bearingFromCenter = bearingDegrees(m_centerLat, m_centerLon, currentLat, currentLon);
        double lookAhead = m_isPatrol ? 115.0 : 55.0; // Math matched from legacy fallback
        
        // Approximate the target coordinate on the circle
        double angularDistance = orbitRadius / kEarthRadiusMeters;
        double bearing = qDegreesToRadians(bearingFromCenter + lookAhead);
        double lat1 = qDegreesToRadians(m_centerLat);
        double lon1 = qDegreesToRadians(m_centerLon);
        
        double lat2 = qAsin(qSin(lat1) * qCos(angularDistance) + qCos(lat1) * qSin(angularDistance) * qCos(bearing));
        double lon2 = lon1 + qAtan2(qSin(bearing) * qSin(angularDistance) * qCos(lat1), qCos(angularDistance) - qSin(lat1) * qSin(lat2));
        
        assignedHeading = bearingDegrees(currentLat, currentLon, qRadiansToDegrees(lat2), qRadiansToDegrees(lon2));
    }
    
    return {assignedHeading, m_targetAlt, m_targetSpeed};
}

// --- PatrolAreaTask ---

PatrolAreaTask::PatrolAreaTask(
    const QVector<RoutePoint>& patrolPoints,
    double targetAlt,
    double targetSpeedKnots)
    : m_patrolPoints(patrolPoints),
      m_targetAlt(targetAlt),
      m_targetSpeed(targetSpeedKnots)
{
}

ITask::State PatrolAreaTask::getState() const
{
    return m_state;
}

DesiredState PatrolAreaTask::evaluate(
    double currentLat,
    double currentLon,
    double currentAlt,
    double currentHeading,
    double dt)
{
    Q_UNUSED(dt);

    if (m_patrolPoints.isEmpty()) {
        m_state = State::Failed;
        return {currentHeading, currentAlt, 0.0};
    }

    if (m_currentPointIndex < 0) {
        double bestDistance = std::numeric_limits<double>::max();
        int bestIndex = 0;
        for (int index = 0; index < m_patrolPoints.size(); ++index) {
            const RoutePoint& point = m_patrolPoints.at(index);
            const double distance = distanceMeters(
                currentLat, currentLon, point.latitude, point.longitude);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndex = index;
            }
        }
        m_currentPointIndex = bestIndex;
    }

    m_state = State::Running;

    RoutePoint targetPoint = m_patrolPoints.at(m_currentPointIndex);
    double distanceToPoint = distanceMeters(
        currentLat, currentLon, targetPoint.latitude, targetPoint.longitude);

    if (!m_patternActive) {
        if (distanceToPoint <= kPatrolArrivalThresholdMeters) {
            m_patternActive = true;
            m_currentPointIndex = (m_currentPointIndex + 1) % m_patrolPoints.size();
            targetPoint = m_patrolPoints.at(m_currentPointIndex);
            distanceToPoint = distanceMeters(
                currentLat, currentLon, targetPoint.latitude, targetPoint.longitude);
        }
    } else if (distanceToPoint <= kPatrolArrivalThresholdMeters) {
        m_currentPointIndex = (m_currentPointIndex + 1) % m_patrolPoints.size();
        targetPoint = m_patrolPoints.at(m_currentPointIndex);
    }

    const double targetHeading = bearingDegrees(
        currentLat, currentLon, targetPoint.latitude, targetPoint.longitude);
    const double assignedAlt =
        targetPoint.altitudeMeters != 0.0 ? targetPoint.altitudeMeters : m_targetAlt;
    return {targetHeading, assignedAlt, m_targetSpeed};
}

QVector<RoutePoint> buildPatrolRouteFromArea(const AreaDefinition& area)
{
    if (area.areaType == QStringLiteral("Polygon") && !area.points.isEmpty()) {
        return area.points;
    }

    QVector<RoutePoint> route;
    route.reserve(kGeneratedPatrolPointCount);

    const double altitudeMeters = area.centerAltitudeMeters;
    const double semiMajorMeters =
        qMax(area.areaType == QStringLiteral("Circle") ? area.radiusMeters : area.semiMajorAxisMeters, 100.0);
    const double semiMinorMeters =
        qMax(area.areaType == QStringLiteral("Circle") ? area.radiusMeters : area.semiMinorAxisMeters, 100.0);
    const double rotationRadians = qDegreesToRadians(area.rotationDegrees);

    for (int index = 0; index < kGeneratedPatrolPointCount; ++index) {
        const double angle = (2.0 * M_PI * static_cast<double>(index)) /
                             static_cast<double>(kGeneratedPatrolPointCount);
        const double localEast = qCos(angle) * semiMajorMeters;
        const double localNorth = qSin(angle) * semiMinorMeters;
        const double rotatedEast =
            localEast * qCos(rotationRadians) - localNorth * qSin(rotationRadians);
        const double rotatedNorth =
            localEast * qSin(rotationRadians) + localNorth * qCos(rotationRadians);
        route.push_back(offsetPointMeters(
            area.centerLatitude,
            area.centerLongitude,
            rotatedNorth,
            rotatedEast,
            altitudeMeters));
    }

    return route;
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
