#pragma once

#include "domain/Entity.h"  // EntityTask
#include <QString>
#include <QVector>

// Task / plan status string constants (shared by plan types and executor logic)
namespace plan_status {
inline constexpr QLatin1StringView NotStarted("NotStarted");
inline constexpr QLatin1StringView Running("Running");
inline constexpr QLatin1StringView Completed("Completed");
inline constexpr QLatin1StringView Failed("Failed");
inline constexpr QLatin1StringView CompletedWithFailures("CompletedWithFailures");
} // namespace plan_status

/// Represents a pending bomb release targeting a specific location.
struct PendingBombRelease {
  QString launcherEntityId;
  QString launcherEntityName;
  QString targetEntityId;
  QString targetEntityName;
  double targetLatitude = 0.0;
  double targetLongitude = 0.0;
  double targetAltitudeMeters = 0.0;
  QString targetLabel;
  QString sourceDescription;
  bool pending = false;
  bool releaseCommandIssued = false;

  QString launcherReference() const {
    return launcherEntityId.trimmed().isEmpty()
        ? launcherEntityName.trimmed()
        : launcherEntityId.trimmed();
  }

  QString targetReference() const {
    return targetEntityId.trimmed().isEmpty()
        ? targetEntityName.trimmed()
        : targetEntityId.trimmed();
  }

  void clear() {
    *this = PendingBombRelease{};
  }
};

struct BombTargetQueueItem {
  QString launcherEntityId;
  QString launcherEntityName;
  QString targetEntityId;
  QString targetEntityName;
  double targetLatitude = 0.0;
  double targetLongitude = 0.0;
  double targetAltitudeMeters = 0.0;
  QString targetLabel;
  QString sourceDescription;

  QString launcherReference() const {
    return launcherEntityId.trimmed().isEmpty()
        ? launcherEntityName.trimmed()
        : launcherEntityId.trimmed();
  }

  QString targetReference() const {
    return targetEntityId.trimmed().isEmpty()
        ? targetEntityName.trimmed()
        : targetEntityId.trimmed();
  }
};

enum class PlanStepKind {
  MoveToLocation,
  WaitOnLocation,
  MoveToWaypoint,
  FollowRoute,
  MoveAlongRoute,
  PatrolArea,
  FlyHeadingAltitudeSpeed,
  OrbitHoldLocation,
  HoldRacetrack,
  FollowEntity,
  InterceptEntity,
  InterceptEntity2D,
  InterceptEntity3D,
  ReturnToBase,
  AttackOnce,
  AttackUntilDestroyed,
  FireOnPosition,
  FireInDirection,
  StopWeaponsTask,
  AttackAir,
  AttackSurface,
  WaitUntilTargetDetected,
  WaitUntilTargetDestroyed,
  WaitUntilDamaged,
  WaitUntilTime,
  WaitUntilInRange,
};

struct PlanStep {
  PlanStepKind kind = PlanStepKind::MoveToLocation;
  EntityTask task;
  QString label;
  QString status = QStringLiteral("NotStarted");
};

struct EntityPlan {
  QVector<PlanStep> steps;
  int currentStepIndex = -1;
  bool running = false;
  int currentStableTicks = 0;
  QString status = QStringLiteral("NotStarted");

  bool hasCurrentStep() const {
    return running && currentStepIndex >= 0 &&
           currentStepIndex < steps.size();
  }

  const PlanStep* currentStep() const {
    return hasCurrentStep() ? &steps[currentStepIndex] : nullptr;
  }

  PlanStep* currentStep() {
    return hasCurrentStep() ? &steps[currentStepIndex] : nullptr;
  }
};
