#pragma once

#include <QString>
#include <QStringList>
#include <functional>

class QMenu;
class QModelIndex;

namespace presentation {

/// Builds the right-click context menu for an entity in the object tree.
/// Extracted from MainWindow to keep the God Class slim.
///
/// All slot wiring is received as a struct of std::function so the class
/// has no dependency on MainWindow's member functions.
struct EntityContextMenuSlots {
  std::function<void()> assignFlyHeadingAltitudeSpeedTask;
  std::function<void()> assignMoveToLocationTask;
  std::function<void()> assignWaitOnLocationTask;
  std::function<void()> assignMoveToWaypointTask;
  std::function<void()> assignMoveAlongRouteTask;
  std::function<void()> assignPatrolRouteTask;
  std::function<void()> assignOrbitHoldLocationTask;
  std::function<void()> assignHoldRacetrackTask;
  std::function<void()> assignReturnToBaseTask;
  std::function<void()> assignPatrolAreaTask;
  std::function<void()> assignOrbitAreaTask;
  std::function<void()> assignFollowEntityTask;
  std::function<void()> assignInterceptEntityTask;
  std::function<void()> assignAttackOnceTask;
  std::function<void()> assignAttackUntilDestroyedTask;
  std::function<void()> assignFireOnPositionTask;
  std::function<void()> assignFireInDirectionTask;
  std::function<void()> assignStopWeaponsTask;
  std::function<void()> assignAttackAirTask;
  std::function<void()> assignAttackSurfaceTask;
  std::function<void()> assignWaitUntilTargetDetectedTask;
  std::function<void()> assignWaitUntilTargetDestroyedTask;
  std::function<void()> assignWaitUntilDamagedTask;
  std::function<void()> assignWaitUntilTimeTask;
  std::function<void()> assignWaitUntilInRangeTask;
  std::function<void()> clearSelectedTask;
  std::function<void()> setSelectedEntityHeading;
  std::function<void()> setSelectedEntityAltitude;
  std::function<void()> setSelectedEntitySpeed;
  std::function<void()> setSelectedEntityFuel;
  std::function<void(const QString& mode)> setSelectedEntityBehaviorMode;
  std::function<void()> openEntityPlanDialog;
  std::function<void()> addMissileToSelectedEntity;
  std::function<void()> addBombToSelectedEntity;
  std::function<void()> launchMissileFromSelectedEntity;
  std::function<void()> launchMissileAtSelectedEntity;
  std::function<void()> releaseBombFromSelectedEntity;
  std::function<void()> releaseBombAtSurfaceEntity;
  std::function<void()> releaseBombAtCustomCoordinates;
  std::function<void()> addBombTargetToQueue;
  std::function<void()> addCustomBombTargetToQueue;
  std::function<void()> clearBombTargetQueue;
  std::function<void()> cancelPendingBombRelease;
  std::function<void()> openSelectedEntityDetails;
  std::function<void()> focusSelectedEntityInMap;
  std::function<void()> deleteSelectedEntity;
  std::function<void()> restoreSelectedEntity;
  std::function<void()> destroySelectedEntity;
  std::function<void(bool hidden)> setSelectedEntityHidden;
  std::function<void(bool visible)> setSelectedEntityRadarCoverageVisible;
  std::function<void(bool visible)> setSelectedEntityTrackHistoryVisible;
};

/// State needed to configure enable/disable/check of context menu items.
struct EntityContextMenuState {
  bool entityDestroyed{false};
  bool canConfigureFuel{false};
  bool canUseWeapons{false};
  int missileCount{0};
  int bombCount{0};
  int detectedMissileTargetCount{0};
  bool simulationRunning{false};
  bool bombReleasePendingForThisEntity{false};
  QString entityName;
  QString currentBehaviorMode;
  QStringList behaviorModeOptions;
  bool hidden{false};
  bool radarCoverageVisible{false};
  bool trackHistoryVisible{false};
};

/// Populates `menu` with the entity context menu structure.
void populateEntityContextMenu(
    QMenu& menu,
    const EntityContextMenuState& state,
    const EntityContextMenuSlots& actions);

} // namespace presentation
