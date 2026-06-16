#include "presentation/PlanStepConfigurator.h"
#include "domain/Entity.h"
#include "domain/GeoMath.h"
#include "presentation/EntityHomePositionTracker.h"

#include <QtMath>

static constexpr double kOrbitHoldDefaultRadiusMeters = 1500.0;

namespace presentation {

PlanStepConfigurator::PlanStepConfigurator(
    CaptureFn   capture,
    FindAreaFn  findArea,
    HomePosFn   homePos,
    AskItemFn   askItem,
    AskDoubleFn askDouble)
  : _capture(std::move(capture))
  , _findArea(std::move(findArea))
  , _homePos(std::move(homePos))
  , _askItem(std::move(askItem))
  , _askDouble(std::move(askDouble)) {}

bool PlanStepConfigurator::configure(
    const Entity&  entity,
    double         defaultHeading,
    int            defaultAltitudeMeters,
    double         defaultSpeedKnots,
    PlanStepKind   kind,
    PlanStep&      step) {

  step = PlanStep{};
  step.kind          = kind;
  step.task.enabled  = true;
  step.task.status   = QStringLiteral("Queued");
  step.status        = QStringLiteral("NotStarted");

  bool ok = false;

  switch (kind) {
    case PlanStepKind::MoveToLocation: {
      EntityTask initial;
      initial.taskType           = QStringLiteral("MoveToLocation");
      initial.enabled            = true;
      initial.status             = QStringLiteral("Queued");
      initial.targetLatitude     = entity.latitude;
      initial.targetLongitude    = entity.longitude;
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots   = defaultSpeedKnots;
      if (!_capture(entity.name, initial, QStringLiteral("MoveToLocation"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Move To %1, %2")
          .arg(step.task.targetLatitude,  0, 'f', 4)
          .arg(step.task.targetLongitude, 0, 'f', 4);
      return true;
    }

    case PlanStepKind::WaitOnLocation: {
      EntityTask initial;
      initial.taskType             = QStringLiteral("WaitOnLocation");
      initial.enabled              = true;
      initial.status               = QStringLiteral("Queued");
      initial.targetLatitude       = entity.latitude;
      initial.targetLongitude      = entity.longitude;
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots     = defaultSpeedKnots;
      initial.arrivalToleranceMeters = 200.0;
      initial.durationSeconds      = 0.0;
      if (!_capture(entity.name, initial, QStringLiteral("WaitOnLocation"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("WaitOnLocation");
      step.label = QStringLiteral("Wait on Location at %1, %2")
          .arg(step.task.targetLatitude,  0, 'f', 4)
          .arg(step.task.targetLongitude, 0, 'f', 4);
      return true;
    }

    case PlanStepKind::MoveToWaypoint: {
      EntityTask initial;
      initial.taskType             = QStringLiteral("MoveToWaypoint");
      initial.enabled              = true;
      initial.status               = QStringLiteral("Queued");
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots     = defaultSpeedKnots;
      if (!_capture(entity.name, initial, QStringLiteral("MoveToWaypoint"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Move To Waypoint: %1").arg(step.task.targetWaypointName);
      return true;
    }

    case PlanStepKind::FollowRoute:
    case PlanStepKind::MoveAlongRoute: {
      EntityTask initial;
      initial.taskType             = QStringLiteral("FollowRoute");
      initial.enabled              = true;
      initial.status               = QStringLiteral("Queued");
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots     = defaultSpeedKnots;
      initial.arrivalToleranceMeters =
          entity.domain.compare(QStringLiteral("Ground"), Qt::CaseInsensitive) == 0
          ? 500.0
          : 1000.0;
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("FollowRoute"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("FollowRoute");
      step.label = QStringLiteral("Follow Route: %1").arg(step.task.targetRouteName);
      return true;
    }

    case PlanStepKind::PatrolArea: {
      EntityTask initial;
      initial.taskType             = QStringLiteral("PatrolArea");
      initial.enabled              = true;
      initial.status               = QStringLiteral("Queued");
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots     = defaultSpeedKnots;
      if (!_capture(entity.name, initial, QStringLiteral("PatrolArea"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      if (const AreaDefinition* area = _findArea(step.task.targetAreaName)) {
        step.task.targetLatitude  = area->centerLatitude;
        step.task.targetLongitude = area->centerLongitude;
        double radius = area->radiusMeters;
        if (radius <= 0.0) {
          if (area->areaType == QStringLiteral("Ellipse")) {
            radius = qMax(area->semiMinorAxisMeters, 100.0);
          } else if (!area->points.isEmpty()) {
            radius = 250.0;
          } else {
            radius = 500.0;
          }
        }
        step.task.targetAreaRadiusMeters = radius;
      }
      step.label = QStringLiteral("Patrol Area: %1").arg(step.task.targetAreaName);
      return true;
    }

    case PlanStepKind::FlyHeadingAltitudeSpeed: {
      EntityTask initial;
      initial.taskType             = QStringLiteral("FlyHeadingAltitudeSpeed");
      initial.enabled              = true;
      initial.status               = QStringLiteral("Queued");
      initial.targetHeadingDegrees = defaultHeading;
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots     = defaultSpeedKnots;
      if (!_capture(entity.name, initial, QStringLiteral("FlyHeadingAltitudeSpeed"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.label = QStringLiteral("Fly H%1 A%2 S%3")
          .arg(step.task.targetHeadingDegrees, 0, 'f', 0)
          .arg(step.task.targetAltitudeMeters)
          .arg(step.task.targetSpeedKnots, 0, 'f', 0);
      return true;
    }

    case PlanStepKind::OrbitHoldLocation: {
      double centerLat = entity.latitude;
      double centerLon = entity.longitude;

      const QString centerMode = _askItem(
          QStringLiteral("Plan Step: Orbit / Hold (Location)"),
          QStringLiteral("Center"),
          QStringList{
              QStringLiteral("Current Position"),
              QStringLiteral("Custom Coordinates"),
          },
          ok);
      if (!ok) {
        return false;
      }

      if (centerMode == QStringLiteral("Custom Coordinates")) {
        centerLat = _askDouble(
            QStringLiteral("Plan Step: Orbit / Hold (Location)"),
            QStringLiteral("Latitude"),
            centerLat, -90.0, 90.0, ok);
        if (!ok) {
          return false;
        }
        centerLon = _askDouble(
            QStringLiteral("Plan Step: Orbit / Hold (Location)"),
            QStringLiteral("Longitude"),
            centerLon, -180.0, 180.0, ok);
        if (!ok) {
          return false;
        }
      }

      step.task.taskType               = QStringLiteral("OrbitArea");
      step.task.targetLatitude         = centerLat;
      step.task.targetLongitude        = centerLon;
      step.task.targetAltitudeMeters   = defaultAltitudeMeters;
      step.task.targetSpeedKnots       = defaultSpeedKnots;
      step.task.targetAreaRadiusMeters = kOrbitHoldDefaultRadiusMeters;
      step.label = QStringLiteral("Orbit / Hold at %1, %2")
          .arg(centerLat, 0, 'f', 4)
          .arg(centerLon, 0, 'f', 4);
      return true;
    }

    case PlanStepKind::HoldRacetrack: {
      EntityTask initial;
      initial.taskType = QStringLiteral("HoldRacetrack");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.targetLatitude = entity.latitude;
      initial.targetLongitude = entity.longitude;
      initial.targetHeadingDegrees = defaultHeading;
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots = defaultSpeedKnots;
      initial.racetrackLegLengthMeters = 10000.0;
      initial.durationSeconds = 0.0;
      if (!_capture(entity.name, initial, QStringLiteral("HoldRacetrack"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      step.task.taskType = QStringLiteral("HoldRacetrack");
      step.label = QStringLiteral("Hold Racetrack at %1, %2")
          .arg(step.task.targetLatitude, 0, 'f', 4)
          .arg(step.task.targetLongitude, 0, 'f', 4);
      return true;
    }

    case PlanStepKind::FollowEntity: {
      EntityTask initial;
      initial.taskType = QStringLiteral("FollowEntity");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.targetAltitudeMeters = defaultAltitudeMeters;
      initial.targetSpeedKnots = defaultSpeedKnots;
      initial.followDistanceMeters = 1000.0;
      initial.arrivalToleranceMeters = 100.0;
      if (!_capture(entity.name, initial, QStringLiteral("FollowEntity"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      step.label = QStringLiteral("Follow Entity: %1")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::InterceptEntity:
    case PlanStepKind::InterceptEntity2D:
    case PlanStepKind::InterceptEntity3D: {
      EntityTask initial;
      initial.taskType = QStringLiteral("InterceptEntity");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.targetSpeedKnots = defaultSpeedKnots;
      initial.interceptDistanceMeters = 500.0;
      initial.altitudeToleranceMeters = 100.0;
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("InterceptEntity"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      step.task.taskType = QStringLiteral("InterceptEntity");
      step.label = QStringLiteral("Intercept Entity: %1")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::ReturnToBase: {
      const EntityHomePosition home = _homePos(entity.name);
      step.task.taskType             = QStringLiteral("MoveToLocation");
      step.task.targetLatitude       = home.valid ? home.latitude    : entity.latitude;
      step.task.targetLongitude      = home.valid ? home.longitude   : entity.longitude;
      step.task.targetAltitudeMeters =
          home.valid ? home.altitudeMeters : defaultAltitudeMeters;
      step.task.targetSpeedKnots     = defaultSpeedKnots;
      step.label = QStringLiteral("Return To Base");
      return true;
    }

    case PlanStepKind::AttackOnce: {
      EntityTask initial;
      initial.taskType = QStringLiteral("AttackOnce");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.weaponType = QStringLiteral("Auto");
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("AttackOnce"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      step.task.taskType = QStringLiteral("AttackOnce");
      step.label = QStringLiteral("Attack Once: %1")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::AttackUntilDestroyed: {
      EntityTask initial;
      initial.taskType = QStringLiteral("AttackUntilDestroyed");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.weaponType = QStringLiteral("Auto");
      initial.maxEngagementTimeSeconds = 120.0;
      initial.timeoutSeconds = 120.0;
      initial.shotCooldownSeconds = 8.0;
      if (!_capture(entity.name, initial, QStringLiteral("AttackUntilDestroyed"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status = QStringLiteral("Queued");
      step.task.taskType = QStringLiteral("AttackUntilDestroyed");
      step.label = QStringLiteral("Attack Until Destroyed: %1")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::AttackAir: {
      EntityTask initial;
      initial.taskType = QStringLiteral("AttackAir");
      initial.enabled  = true;
      initial.status   = QStringLiteral("Queued");
      if (!_capture(entity.name, initial, QStringLiteral("AttackAir"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status  = QStringLiteral("Queued");
      step.label = QStringLiteral("Attack Air: %1")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::AttackSurface: {
      EntityTask initial;
      initial.taskType             = QStringLiteral("AttackSurface");
      initial.enabled              = true;
      initial.status               = QStringLiteral("Queued");
      initial.targetLatitude       = entity.latitude;
      initial.targetLongitude      = entity.longitude;
      initial.targetAltitudeMeters = 0;
      if (!_capture(entity.name, initial, QStringLiteral("AttackSurface"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.status  = QStringLiteral("Queued");
      const QString targetName = step.task.targetEntityName.trimmed();
      step.label = targetName.isEmpty()
          ? QStringLiteral("Attack Surface: %1")
                .arg(domain::attackPointLabel(step.task.targetLatitude, step.task.targetLongitude))
          : QStringLiteral("Attack Surface: %1").arg(targetName);
      return true;
    }

    case PlanStepKind::WaitUntilTargetDetected: {
      EntityTask initial;
      initial.taskType = QStringLiteral("WaitUntilTargetDetected");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.targetDomain = QStringLiteral("Any");
      initial.enemyOnly = true;
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("WaitUntilTargetDetected"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("WaitUntilTargetDetected");
      step.label = QStringLiteral("Wait Until Target Detected");
      return true;
    }

    case PlanStepKind::WaitUntilTargetDestroyed: {
      EntityTask initial;
      initial.taskType = QStringLiteral("WaitUntilTargetDestroyed");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("WaitUntilTargetDestroyed"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("WaitUntilTargetDestroyed");
      step.label = QStringLiteral("Wait Until Target Destroyed: %1")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed());
      return true;
    }

    case PlanStepKind::WaitUntilDamaged: {
      EntityTask initial;
      initial.taskType = QStringLiteral("WaitUntilDamaged");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.damageThresholdPercent = 50.0;
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("WaitUntilDamaged"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("WaitUntilDamaged");
      step.label = QStringLiteral("Wait Until Damaged: %1 >= %2%")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed())
          .arg(step.task.damageThresholdPercent, 0, 'f', 0);
      return true;
    }

    case PlanStepKind::WaitUntilTime: {
      EntityTask initial;
      initial.taskType = QStringLiteral("WaitUntilTime");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.durationSeconds = 30.0;
      if (!_capture(entity.name, initial, QStringLiteral("WaitUntilTime"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("WaitUntilTime");
      step.label = QStringLiteral("Wait Until Time: %1 s")
          .arg(step.task.durationSeconds, 0, 'f', 0);
      return true;
    }

    case PlanStepKind::WaitUntilInRange: {
      EntityTask initial;
      initial.taskType = QStringLiteral("WaitUntilInRange");
      initial.enabled = true;
      initial.status = QStringLiteral("Queued");
      initial.rangeMeters = 1000.0;
      initial.timeoutSeconds = 120.0;
      if (!_capture(entity.name, initial, QStringLiteral("WaitUntilInRange"), step.task)) {
        return false;
      }
      step.task.enabled = true;
      step.task.taskType = QStringLiteral("WaitUntilInRange");
      step.label = QStringLiteral("Wait Until In Range: %1 <= %2 m")
          .arg(step.task.targetEntityName.trimmed().isEmpty()
               ? QStringLiteral("-")
               : step.task.targetEntityName.trimmed())
          .arg(step.task.rangeMeters, 0, 'f', 0);
      return true;
    }
  }

  return false;
}

} // namespace presentation
