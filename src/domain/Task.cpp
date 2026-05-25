#include "Task.h"

#include <algorithm>
#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {
    constexpr double kEarthRadiusMeters = 6371000.0;
    constexpr double kPatrolArrivalThresholdMeters = 200.0;
    constexpr int kGeneratedPatrolPointCount = 6;

    constexpr double toRadians(double degrees) { return degrees * M_PI / 180.0; }
    constexpr double toDegrees(double radians) { return radians * 180.0 / M_PI; }

    double bearingDegrees(double latitude1, double longitude1, double latitude2, double longitude2) {
        const double lat1 = toRadians(latitude1);
        const double lat2 = toRadians(latitude2);
        const double deltaLongitude = toRadians(longitude2 - longitude1);
        const double y = std::sin(deltaLongitude) * std::cos(lat2);
        const double x = std::cos(lat1) * std::sin(lat2) - std::sin(lat1) * std::cos(lat2) * std::cos(deltaLongitude);
        double degrees = toDegrees(std::atan2(y, x));
        while (degrees < 0.0) { degrees += 360.0; }
        return degrees;
    }

    double distanceMeters(double latitude1, double longitude1, double latitude2, double longitude2) {
        const double lat1 = toRadians(latitude1);
        const double lon1 = toRadians(longitude1);
        const double lat2 = toRadians(latitude2);
        const double lon2 = toRadians(longitude2);
        const double deltaLat = lat2 - lat1;
        const double deltaLon = lon2 - lon1;
        const double sinHalfLat = std::sin(deltaLat / 2.0);
        const double sinHalfLon = std::sin(deltaLon / 2.0);
        const double a = sinHalfLat * sinHalfLat + std::cos(lat1) * std::cos(lat2) * sinHalfLon * sinHalfLon;
        const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        return kEarthRadiusMeters * c;
    }

    RoutePoint offsetPointMeters(
        double centerLatitude,
        double centerLongitude,
        double northMeters,
        double eastMeters,
        double altitudeMeters) {
        const double latitudeRadians = toRadians(centerLatitude);
        const double deltaLatitudeRadians = northMeters / kEarthRadiusMeters;
        const double cosLatitude = std::max(0.000001, std::cos(latitudeRadians));
        const double deltaLongitudeRadians =
            eastMeters / (kEarthRadiusMeters * cosLatitude);

        RoutePoint point;
        point.latitude = centerLatitude + toDegrees(deltaLatitudeRadians);
        point.longitude = centerLongitude + toDegrees(deltaLongitudeRadians);
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
    (void)dt;

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

FollowEntityTask::FollowEntityTask(
    double targetAlt,
    double targetSpeedKnots,
    double followDistanceMeters,
    double arrivalToleranceMeters)
    : m_fallbackAlt(targetAlt),
      m_fallbackSpeed(targetSpeedKnots),
      m_followDistanceMeters(std::max(1.0, followDistanceMeters)),
      m_arrivalToleranceMeters(std::max(0.0, arrivalToleranceMeters))
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
    (void)dt;

    if (!m_hasTargetData) {
        m_state = State::Failed; // Target unavailable
        return {currentHeading, currentAlt, 0.0};
    }
    
    m_state = State::Running;
    
    const double targetHeading = bearingDegrees(currentLat, currentLon, m_targetLat, m_targetLon);
    const double dist = distanceMeters(currentLat, currentLon, m_targetLat, m_targetLon);
    const double assignedAlt = (m_fallbackAlt > 0) ? m_fallbackAlt : m_targetAlt;

    if (dist <= m_followDistanceMeters) {
        return {currentHeading, assignedAlt, 0.0};
    }

    const double targetSpeed = std::max(m_targetSpeed, m_fallbackSpeed);
    const double assignedSpeed =
        (dist > m_followDistanceMeters + m_arrivalToleranceMeters)
        ? targetSpeed
        : 0.0;
    
    return {targetHeading, assignedAlt, assignedSpeed};
}

// --- InterceptEntity2DTask ---

InterceptEntity2DTask::InterceptEntity2DTask(double speedKnots, double interceptDistanceMeters)
    : m_speedKnots(speedKnots),
      m_interceptDistanceMeters(std::max(1.0, interceptDistanceMeters))
{
}

void InterceptEntity2DTask::updateTargetLocation(double targetLat, double targetLon)
{
    m_targetLat = targetLat;
    m_targetLon = targetLon;
    m_hasTargetData = true;
}

ITask::State InterceptEntity2DTask::getState() const
{
    return m_state;
}

DesiredState InterceptEntity2DTask::evaluate(
    double currentLat,
    double currentLon,
    double currentAlt,
    double currentHeading,
    double dt)
{
    (void)dt;

    if (!m_hasTargetData) {
        m_state = State::Failed;
        return {currentHeading, currentAlt, 0.0};
    }

    const double dist = distanceMeters(currentLat, currentLon, m_targetLat, m_targetLon);
    if (dist <= m_interceptDistanceMeters) {
        m_state = State::Completed;
        return {currentHeading, currentAlt, 0.0};
    }

    m_state = State::Running;
    const double targetHeading = bearingDegrees(currentLat, currentLon, m_targetLat, m_targetLon);
    return {targetHeading, currentAlt, m_speedKnots};
}

// --- InterceptEntity3DTask ---

InterceptEntity3DTask::InterceptEntity3DTask(
    double speedKnots,
    double interceptDistanceMeters,
    double altitudeToleranceMeters)
    : m_speedKnots(speedKnots),
      m_interceptDistanceMeters(std::max(1.0, interceptDistanceMeters)),
      m_altitudeToleranceMeters(std::max(0.0, altitudeToleranceMeters))
{
}

void InterceptEntity3DTask::updateTargetLocation(double targetLat, double targetLon, double targetAlt)
{
    m_targetLat = targetLat;
    m_targetLon = targetLon;
    m_targetAlt = targetAlt;
    m_hasTargetData = true;
}

ITask::State InterceptEntity3DTask::getState() const
{
    return m_state;
}

DesiredState InterceptEntity3DTask::evaluate(
    double currentLat,
    double currentLon,
    double currentAlt,
    double currentHeading,
    double dt)
{
    (void)dt;

    if (!m_hasTargetData) {
        m_state = State::Failed;
        return {currentHeading, currentAlt, 0.0};
    }

    const double horizontalDistance = distanceMeters(currentLat, currentLon, m_targetLat, m_targetLon);
    const double altitudeError = std::abs(currentAlt - m_targetAlt);
    if (horizontalDistance <= m_interceptDistanceMeters &&
        altitudeError <= m_altitudeToleranceMeters) {
        m_state = State::Completed;
        return {currentHeading, m_targetAlt, 0.0};
    }

    m_state = State::Running;
    const double targetHeading = bearingDegrees(currentLat, currentLon, m_targetLat, m_targetLon);
    return {targetHeading, m_targetAlt, m_speedKnots};
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
    double areaRadius = std::max(100.0, m_radiusMeters);
    double orbitRadius = m_isPatrol ? std::max(200.0, areaRadius * 0.55) : std::max(150.0, areaRadius * 0.75);
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
        double bearing = toRadians(bearingFromCenter + lookAhead);
        double lat1 = toRadians(m_centerLat);
        double lon1 = toRadians(m_centerLon);
        
        double lat2 = std::asin(std::sin(lat1) * std::cos(angularDistance) + std::cos(lat1) * std::sin(angularDistance) * std::cos(bearing));
        double lon2 = lon1 + std::atan2(std::sin(bearing) * std::sin(angularDistance) * std::cos(lat1), std::cos(angularDistance) - std::sin(lat1) * std::sin(lat2));
        
        assignedHeading = bearingDegrees(currentLat, currentLon, toDegrees(lat2), toDegrees(lon2));
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
    (void)dt;

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
        std::max(area.areaType == QStringLiteral("Circle") ? area.radiusMeters : area.semiMajorAxisMeters, 100.0);
    const double semiMinorMeters =
        std::max(area.areaType == QStringLiteral("Circle") ? area.radiusMeters : area.semiMinorAxisMeters, 100.0);
    const double rotationRadians = toRadians(area.rotationDegrees);

    for (int index = 0; index < kGeneratedPatrolPointCount; ++index) {
        const double angle = (2.0 * M_PI * static_cast<double>(index)) /
                             static_cast<double>(kGeneratedPatrolPointCount);
        const double localEast = std::cos(angle) * semiMajorMeters;
        const double localNorth = std::sin(angle) * semiMinorMeters;
        const double rotatedEast =
            localEast * std::cos(rotationRadians) - localNorth * std::sin(rotationRadians);
        const double rotatedNorth =
            localEast * std::sin(rotationRadians) + localNorth * std::cos(rotationRadians);
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

DesiredState TaskStack::evaluateTop(
    double currentLat,
    double currentLon,
    double currentAlt,
    double currentHeading,
    double dt,
    ITask::State* evaluatedState)
{
    if (m_stack.empty()) {
        if (evaluatedState) {
            *evaluatedState = ITask::State::NotStarted;
        }
        return {currentHeading, currentAlt, 0.0}; // Hold current state if no tasks
    }
    
    ITask* currentTask = m_stack.back().get();
    DesiredState desired = currentTask->evaluate(currentLat, currentLon, currentAlt, currentHeading, dt);
    const ITask::State state = currentTask->getState();
    if (evaluatedState) {
        *evaluatedState = state;
    }
    
    // Auto-pop completed tasks
    if (state == ITask::State::Completed || state == ITask::State::Failed) {
        pop();
    }
    
    return desired;
}

} // namespace domain
