#include "presentation/EntityContextMenuBuilder.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>

namespace presentation {

void populateEntityContextMenu(
    QMenu& menu,
    const EntityContextMenuState& s,
    const EntityContextMenuSlots& actions) {

  // ── Task submenu ────────────────────────────────────────────────────────
  QMenu* taskMenu = menu.addMenu(QStringLiteral("Task"));
  QMenu* movementMenu = taskMenu->addMenu(QStringLiteral("Movement"));

  auto addSlotAction = [](QMenu* m, const QString& text,
                          const std::function<void()>& fn) {
    QAction* a = m->addAction(text);
    QObject::connect(a, &QAction::triggered, m, [fn]() { fn(); });
    return a;
  };

  addSlotAction(movementMenu, QStringLiteral("Fly Heading / Altitude / Speed..."),
      actions.assignFlyHeadingAltitudeSpeedTask);
  addSlotAction(movementMenu, QStringLiteral("Move To Location..."),
      actions.assignMoveToLocationTask);
  addSlotAction(movementMenu, QStringLiteral("Move To Waypoint..."),
      actions.assignMoveToWaypointTask);
  addSlotAction(movementMenu, QStringLiteral("Move Along Route..."),
      actions.assignMoveAlongRouteTask);
  addSlotAction(movementMenu, QStringLiteral("Patrol Route..."),
      actions.assignPatrolRouteTask);
  addSlotAction(movementMenu, QStringLiteral("Orbit / Hold (Location)..."),
      actions.assignOrbitHoldLocationTask);
  addSlotAction(movementMenu, QStringLiteral("Return To Base"),
      actions.assignReturnToBaseTask);
  addSlotAction(movementMenu, QStringLiteral("Patrol Area..."),
      actions.assignPatrolAreaTask);
  addSlotAction(movementMenu, QStringLiteral("Orbit Area..."),
      actions.assignOrbitAreaTask);
  addSlotAction(movementMenu, QStringLiteral("Follow Entity..."),
      actions.assignFollowEntityTask);

  QMenu* attackTaskMenu = taskMenu->addMenu(QStringLiteral("Attack"));
  addSlotAction(attackTaskMenu, QStringLiteral("Attack Air..."),
      actions.assignAttackAirTask);
  addSlotAction(attackTaskMenu, QStringLiteral("Attack Surface..."),
      actions.assignAttackSurfaceTask);

  taskMenu->addSeparator();
  addSlotAction(taskMenu, QStringLiteral("Clear Current Task"),
      actions.clearSelectedTask);
  taskMenu->setEnabled(!s.entityDestroyed);

  // ── Set submenu ─────────────────────────────────────────────────────────
  QMenu* setMenu = menu.addMenu(QStringLiteral("Set"));
  addSlotAction(setMenu, QStringLiteral("Heading..."),
      actions.setSelectedEntityHeading);
  addSlotAction(setMenu, QStringLiteral("Altitude..."),
      actions.setSelectedEntityAltitude);
  addSlotAction(setMenu, QStringLiteral("Speed..."),
      actions.setSelectedEntitySpeed);
  setMenu->setEnabled(!s.entityDestroyed);

  // ── Behavior submenu ────────────────────────────────────────────────────
  QMenu* behaviorMenu = menu.addMenu(QStringLiteral("Behavior"));
  QActionGroup* behaviorGroup = new QActionGroup(behaviorMenu);
  behaviorGroup->setExclusive(true);
  for (const QString& mode : s.behaviorModeOptions) {
    QAction* action = behaviorMenu->addAction(mode);
    action->setCheckable(true);
    action->setChecked(
        mode.compare(s.currentBehaviorMode, Qt::CaseInsensitive) == 0);
    behaviorGroup->addAction(action);
    QObject::connect(action, &QAction::triggered, behaviorMenu,
        [&actions, mode]() { actions.setSelectedEntityBehaviorMode(mode); });
  }
  behaviorMenu->setEnabled(!s.entityDestroyed && !s.entityName.isEmpty());

  // ── Plan ────────────────────────────────────────────────────────────────
  QAction* planAction = addSlotAction(&menu, QStringLiteral("Plan..."),
      actions.openEntityPlanDialog);
  planAction->setEnabled(!s.entityDestroyed);

  // ── Weapons submenu ─────────────────────────────────────────────────────
  QMenu* weaponsMenu = menu.addMenu(QStringLiteral("Weapons"));
  QAction* addMissile = addSlotAction(weaponsMenu, QStringLiteral("Add Missile"),
      actions.addMissileToSelectedEntity);
  QAction* addBomb = addSlotAction(weaponsMenu, QStringLiteral("Add Bomb"),
      actions.addBombToSelectedEntity);
  QAction* launchMissile = addSlotAction(weaponsMenu,
      QStringLiteral("Launch Missile (%1)").arg(s.missileCount),
      actions.launchMissileFromSelectedEntity);
  QAction* launchMissileAt = addSlotAction(weaponsMenu,
      QStringLiteral("Launch Missile At..."),
      actions.launchMissileAtSelectedEntity);
  QAction* releaseBomb = addSlotAction(weaponsMenu,
      QStringLiteral("Release Bomb (%1)").arg(s.bombCount),
      actions.releaseBombFromSelectedEntity);
  QMenu* releaseBombAtMenu = weaponsMenu->addMenu(QStringLiteral("Release Bomb At..."));
  QAction* releaseBombAtSurface = addSlotAction(releaseBombAtMenu,
      QStringLiteral("Surface Entity..."),
      actions.releaseBombAtSurfaceEntity);
  QAction* releaseBombAtCustom = addSlotAction(releaseBombAtMenu,
      QStringLiteral("Custom Coordinates..."),
      actions.releaseBombAtCustomCoordinates);
  QAction* cancelBomb = addSlotAction(weaponsMenu,
      QStringLiteral("Cancel Bomb Release"),
      actions.cancelPendingBombRelease);

  addMissile->setEnabled(s.canUseWeapons);
  addBomb->setEnabled(s.canUseWeapons);
  launchMissile->setEnabled(s.canUseWeapons && s.missileCount > 0 && s.simulationRunning);
  launchMissileAt->setEnabled(
      s.canUseWeapons && s.missileCount > 0 && s.simulationRunning &&
      s.detectedMissileTargetCount > 0);
  releaseBomb->setEnabled(s.canUseWeapons && s.bombCount > 0 && s.simulationRunning);
  releaseBombAtMenu->setEnabled(s.canUseWeapons && s.bombCount > 0 && s.simulationRunning);
  releaseBombAtSurface->setEnabled(s.canUseWeapons && s.bombCount > 0 && s.simulationRunning);
  releaseBombAtCustom->setEnabled(s.canUseWeapons && s.bombCount > 0 && s.simulationRunning);
  cancelBomb->setEnabled(s.bombReleasePendingForThisEntity);
  if (s.canUseWeapons && s.missileCount > 0 && s.simulationRunning &&
      s.detectedMissileTargetCount <= 0) {
    const QString tip =
        QStringLiteral("No detected air targets in missile range for %1.").arg(s.entityName);
    launchMissileAt->setToolTip(tip);
    launchMissileAt->setStatusTip(tip);
  }

  // ── Info / visibility / delete ──────────────────────────────────────────
  menu.addSeparator();
  addSlotAction(&menu, QStringLiteral("Information..."),
      actions.openSelectedEntityDetails);
  addSlotAction(&menu, QStringLiteral("Focus / Track Camera"),
      actions.focusSelectedEntityInMap);
  menu.addSeparator();

  menu.addAction(QStringLiteral("Edit..."));  // placeholder — no-op
  addSlotAction(&menu, QStringLiteral("Delete"),
      actions.deleteSelectedEntity);

  QAction* hideAction = menu.addAction(QStringLiteral("Hide"));
  hideAction->setCheckable(true);
  hideAction->setChecked(s.hidden);
  QObject::connect(hideAction, &QAction::toggled, &menu,
      [&actions](bool hidden) { actions.setSelectedEntityHidden(hidden); });

  if (s.entityDestroyed) {
    addSlotAction(&menu, QStringLiteral("Restore"),
        actions.restoreSelectedEntity);
  } else {
    addSlotAction(&menu, QStringLiteral("Destroyed"),
        actions.destroySelectedEntity);
  }

  menu.addSeparator();

  QAction* radarAction = menu.addAction(QStringLiteral("Show Radar Coverage"));
  radarAction->setCheckable(true);
  radarAction->setChecked(s.radarCoverageVisible);
  QObject::connect(radarAction, &QAction::toggled, &menu,
      [&actions](bool visible) { actions.setSelectedEntityRadarCoverageVisible(visible); });

  QAction* historyAction = menu.addAction(QStringLiteral("Show Track History"));
  historyAction->setCheckable(true);
  historyAction->setChecked(s.trackHistoryVisible);
  QObject::connect(historyAction, &QAction::toggled, &menu,
      [&actions](bool visible) { actions.setSelectedEntityTrackHistoryVisible(visible); });
}

} // namespace presentation
