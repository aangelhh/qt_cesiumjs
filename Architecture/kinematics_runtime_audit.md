# EPIC 1.3 - Kinematics Runtime Audit

Status: completed audit

Objective: identify every relevant runtime path that modifies entity kinematics, document ownership conflicts, and define safe follow-up fixes without a broad refactor.

## Scope

Audited fields:

- `latitude`
- `longitude`
- `altitude`
- `headingDegrees`
- `speedKnots`
- `currentTask.targetLatitude`
- `currentTask.targetLongitude`
- `currentTask.targetAltitudeMeters`
- `pitchDegrees`
- `rollDegrees`
- `verticalSpeedMetersPerSecond`

Audited systems:

- `FlightDynamicsEngine`
- `ScenarioState::advanceSimulation`
- `FlightDynamicsEngine::advanceEntities`
- `EntityTask`
- `TaskStack`
- `AttackAir`
- `AttackUntilDestroyed`
- `FollowEntity`
- `InterceptEntity`
- `ReturnToBase`
- `HoldRacetrack`
- `WaitOnLocation`
- Ground movement handling

## Runtime Ownership Map

| Area | Primary files | Writes | Runtime role |
| --- | --- | --- | --- |
| Physical kinematic integration | `src/application/FlightDynamicsEngine.cpp` | `latitude`, `longitude`, `altitude`, `headingDegrees`, `speedKnots`, `pitchDegrees`, `rollDegrees`, `verticalSpeedMetersPerSecond` | Main runtime owner of entity movement while simulation advances. |
| Task target resolution | `src/application/FlightDynamicsEngine.cpp` | `currentTask.targetLatitude`, `currentTask.targetLongitude`, `currentTask.targetAltitudeMeters`, `currentTask.targetHeadingDegrees`, `currentTask.targetSpeedKnots`, `currentTask.status`, `currentTask.elapsedSeconds`, route indices | Converts active `TaskStack` output and dynamic targets into task setpoints. |
| Domain task evaluation | `src/domain/Task.cpp` | Internal task state only; returns `DesiredState` | Produces desired heading, altitude, and speed from current state. It should not mutate `Entity` directly. |
| Task assignment and stack creation | `src/application/TaskApplicator.cpp`, `src/application/Command.cpp` | `entity.currentTask`, task target fields, task stack contents | Normalizes task parameters, resolves waypoint/route/area coordinates, and creates matching `TaskStack` entries. |
| Plan progression | `src/presentation/EntityPlanExecutor.cpp` | `PlanStep.status`, plan indices, starts tasks through applicator/controller paths | Decides when a step is complete/failed and advances plans. It should not integrate physical movement directly. |
| Attack pursuit setpoints | `src/application/AttackTaskProcessor.cpp` | `currentTask.targetLatitude`, `currentTask.targetLongitude`, `currentTask.targetAltitudeMeters`, `currentTask.targetHeadingDegrees`, `currentTask.targetSpeedKnots`, `currentTask.status` | AttackAir and AttackUntilDestroyed update pursuit targets while attack task is active. |
| Scenario lifecycle cleanup | `src/application/ScenarioState.cpp` | `currentTask`, `speedKnots`, `verticalSpeedMetersPerSecond`, `altitude`, `pitchDegrees`, `rollDegrees`, task stacks | Stops/destroys entities and clears state. |
| Serialization/load normalization | `src/application/ScenarioSerializer.cpp`, `src/application/ScenarioState.cpp` | Initial entity/task fields on load | Loads persisted kinematic state and normalizes Ground entities. |
| UI task defaults | `src/AssignTaskDialog.cpp`, `src/presentation/*Configurator*`, `src/presentation/*Controller*` | Task target fields before assignment | Captures operator intent; not part of per-tick physical integration. |

## Per-Tick Execution Order

`ScenarioState::advanceSimulation(deltaSeconds)` currently executes:

1. `FlightDynamicsEngine::advanceEntities(...)`
2. `advanceBehaviors(deltaSeconds)`
3. `advanceActiveMunitions(deltaSeconds)`
4. `advanceTransientEffects(deltaSeconds)`
5. `refreshSensors()`

Important implication:

- Movement is advanced before behavior and munitions for the tick.
- Attack task processing is not visible in `ScenarioState::advanceSimulation` itself; it is triggered by callers around the simulation loop. If attack processing writes movement setpoints after `FlightDynamicsEngine` has already advanced the entity, those setpoints affect the next tick.

## Field-Level Writers

### `latitude` / `longitude`

Runtime writers:

- `FlightDynamicsEngine::applyKinematicStep(...)`
  - Advances entity position using current `headingDegrees` and `speedKnots`.
- `FlightDynamicsEngine::applyJsbsimStep(...)`
  - Replaces entity position from JSBSim state when JSBSim path is active.

Initialization/load writers:

- `AddEntityDialog`
- `Command` entity creation
- `ScenarioSerializer`

Indirect writers:

- `TaskApplicator` and UI configurators write target coordinates, not actual position.

Audit result:

- Physical position has one main runtime owner: `FlightDynamicsEngine`.
- This is good. Keep it that way.

### `altitude`

Runtime writers:

- `FlightDynamicsEngine::applyKinematicStep(...)`
  - Integrates using `verticalSpeedMetersPerSecond`.
- `FlightDynamicsEngine::applyJsbsimStep(...)`
  - Replaces altitude from JSBSim state.
- `FlightDynamicsEngine::normalizeGroundKinematics(...)`
  - Clears vertical speed, pitch, and roll for Ground entities.
  - Preserves the current Ground altitude/elevation because there is no terrain model yet.

Lifecycle/load writers:

- `ScenarioState::normalizeGroundEntity(...)`
- `ScenarioState::setEntityDestroyed(...)`
- `ScenarioSerializer`

Audit result:

- Air altitude is controlled by `targetAltitudeMeters -> verticalSpeedMetersPerSecond -> altitude`.
- Ground altitude is not flight-controlled. It represents current configured elevation and must not be overwritten by movement tasks.
- No terrain model exists, so new Ground entities may still default to `0`, but loaded/configured Ground altitude should be preserved.

### `headingDegrees`

Runtime writers:

- `FlightDynamicsEngine::resolveTaskTargets(...)`
  - Updates heading through interpolation toward `currentTask.targetHeadingDegrees`.
- `FlightDynamicsEngine::applyJsbsimStep(...)`
  - Replaces heading from JSBSim state.

Indirect writers:

- Domain tasks return desired heading through `DesiredState`.
- `AttackTaskProcessor` writes `currentTask.targetHeadingDegrees` for attack pursuit.

Audit result:

- Heading integration is centralized in `FlightDynamicsEngine`.
- Target heading ownership is split between `TaskStack` evaluation and `AttackTaskProcessor`.

### `speedKnots`

Runtime writers:

- `FlightDynamicsEngine::resolveTaskTargets(...)`
  - Interpolates toward `currentTask.targetSpeedKnots`.
  - Forces speed `0` when status becomes `On target` or `Completed`.
- `FlightDynamicsEngine::advanceEntity(...)`
  - Forces speed `0` when task is disabled or terminal.
- `FlightDynamicsEngine::applyJsbsimStep(...)`
  - Replaces speed from JSBSim state.
- `ScenarioState::stopMission(...)`
  - Forces speed `0`.
- `ScenarioState::setEntityDestroyed(...)`
  - Forces speed `0`.

Task assignment/default writers:

- `TaskApplicator::resolveTaskCoordinates(...)`
  - Defaults `targetSpeedKnots`.

Audit result:

- Speed has explicit stop behavior on terminal/disabled tasks.
- Movement residual risk is mostly around non-movement tasks that leave `speedKnots > 0` and around setpoints written after plan transitions.

### `currentTask.targetLatitude` / `targetLongitude`

Runtime writers:

- `FlightDynamicsEngine::resolveTaskTargets(...)`
  - `FollowEntity`: updates target lat/lon from target entity each tick.
  - `InterceptEntity`: updates target lat/lon from target entity each tick.
  - Route tasks: updates target lat/lon from current route point.
- `AttackTaskProcessor`
  - `AttackAir` and `AttackUntilDestroyed`: updates target lat/lon from attack target while task is active.

Assignment writers:

- `TaskApplicator`
- `Command`
- `AssignTaskDialog`
- `PlanStepConfigurator`
- `TaskAssignmentController`
- `AssignTaskController`

Audit result:

- Dynamic movement tasks and attack pursuit both write task target coordinates.
- This is expected today, but it is a conflict point for future movement intent extraction.

### `currentTask.targetAltitudeMeters`

Runtime writers:

- `FlightDynamicsEngine::resolveTaskTargets(...)`
  - From `DesiredState`.
  - Intercept updates from target altitude when altitude applies.
  - Route updates indirectly through current route point desired state.
- `AttackTaskProcessor`
  - AttackAir / AttackUntilDestroyed set target altitude to target altitude.

Assignment/load writers:

- `TaskApplicator`, `Command`, UI configurators, `ScenarioSerializer`.

Audit result:

- Air movement uses this as the vertical setpoint.
- Ground entities are protected by `normalizeGroundKinematics`, so target altitude should not produce Ground climb.

### `pitchDegrees` / `rollDegrees`

Runtime writers:

- `FlightDynamicsEngine::updateDerivedKinematicAttitude(...)`
  - Derives visual attitude from vertical speed and heading changes.
- `FlightDynamicsEngine::relaxDerivedAttitude(...)`
  - Returns attitude toward zero when stopped/non-moving.
- `FlightDynamicsEngine::normalizeGroundKinematics(...)`
  - Forces Ground pitch/roll to `0`.
- `FlightDynamicsEngine::applyJsbsimStep(...)`
  - Replaces pitch/roll from JSBSim state.

Audit result:

- Pitch/roll are derived output, not task intent.
- Ground pitch/roll are forcibly disabled.

### `verticalSpeedMetersPerSecond`

Runtime writers:

- `FlightDynamicsEngine::resolveTaskTargets(...)`
  - Computes from altitude error and climb-rate cap.
- `FlightDynamicsEngine::advanceEntity(...)`
  - Clears it for terminal/disabled/non-movement states.
- `FlightDynamicsEngine::normalizeGroundKinematics(...)`
  - Forces `0`.
- `FlightDynamicsEngine::applyJsbsimStep(...)`
  - Replaces from JSBSim state.
- `ScenarioState::stopMission(...)` and `setEntityDestroyed(...)`
  - Force `0`.

Audit result:

- Vertical speed is derived from target altitude and cleared in most terminal paths.
- Good candidate to be owned only by future movement controller.

## Movement Task Behavior Map

| Task | Runtime path | Desired state source | Completion source | Notes |
| --- | --- | --- | --- | --- |
| MoveToLocation | `TaskStack` -> `MoveToLocationTask` or fallback block in `FlightDynamicsEngine` | Fixed target lat/lon/alt/speed | `On target` when close; plan treats as complete | Legacy fallback still exists when no stack is present. |
| MoveToWaypoint | `TaskApplicator` resolves waypoint; stack uses `MoveToLocationTask` | Resolved waypoint coordinates | Same as MoveToLocation | Air altitude respects waypoint altitude when set; Ground uses current configured altitude/elevation. |
| FollowRoute / MoveAlongRoute | `RouteTask` | Current route point | `Completed` at final point | Overshoot handling exists in domain task. |
| FollowEntity | `FlightDynamicsEngine` feeds target data into `FollowEntityTask` | Dynamic target location | `Completed` after stable distance unless duration overrides | Speed forced `0` inside follow distance. |
| InterceptEntity | `FlightDynamicsEngine` feeds target data into `InterceptEntity3DTask` | Dynamic target lat/lon/alt if applicable | `Completed` inside horizontal and vertical tolerances | Legacy 2D/3D names normalize to unified task. |
| HoldRacetrack | `HoldRacetrackTask` | Alternating generated endpoints | Runs until duration or cancellation/plan advance | No natural completion unless duration is set. |
| WaitOnLocation | `WaitOnLocationTask` | Fixed target lat/lon/alt/speed | Arrive, then duration if configured | If no duration, remains active. |
| ReturnToBase | Plan/applicator maps to movement target | Home position | `On target` | Depends on stored home altitude. |
| AttackAir | `AttackTaskProcessor` writes pursuit setpoints; `FlightDynamicsEngine` moves as movement task | Dynamic target entity | Completed on target destroyed, Failed on target unavailable/no ammo/timeout | Attack processor is the pursuit authority. |
| AttackUntilDestroyed | Same as AttackAir | Dynamic target entity | Completed on destroyed, Failed on timeout/no ammo/unavailable | Same kinematic risks as AttackAir. |
| AttackOnce / FireOnPosition / AttackSurface | Attack processor arms weapon release | Usually no sustained movement | Completed after launch/arming | Should not influence movement after completion. |
| StopWeaponsTask | Attack processor cancels active weapon action | None | Completed | Should not influence movement. |
| Conditional waits | `EntityPlanExecutor` checks conditions | None | Completed/Failed by condition | Should not move entity. |

## Identified Conflict Points

### 1. Split ownership of attack pursuit setpoints

`AttackAir` and `AttackUntilDestroyed` are classified as movement task types in the engine, but their target coordinates/headings/speeds are written by `AttackTaskProcessor`.

Risk:

- If plan progression starts a new movement step after an attack terminal state, stale attack setpoints may remain in `currentTask` until overwritten.
- Current engine terminal guards prevent continued movement when status is terminal, but stale target fields can still confuse summaries or override checks if not replaced by the next step.

Safe fix candidate for 1.4:

- Add a small helper to clear transient movement targets when a task becomes terminal or when a new plan step starts.
- Do not clear operator-configured static targets before they are needed.

### 2. Fallback movement block still handles movement without a `TaskStack`

`FlightDynamicsEngine::resolveTaskTargets(...)` has a `TaskStack` path and a fallback path for `MoveToLocation`, `WaitOnLocation`, route-like tasks, and attack tasks.

Risk:

- If a task is assigned but stack creation fails or is cleared unexpectedly, fallback movement may still move based on `currentTask` fields.
- This is useful for legacy compatibility but makes authority less explicit.

Safe fix candidate for 1.4:

- Add tests proving terminal tasks do not move even with stale speed and target fields.
- Add explicit logging or status for movement tasks missing an expected stack, instead of silent fallback for modern task types.

### 3. Ground normalization exists in multiple layers

Ground entities are normalized in serializer/load, scenario normalization, and every engine tick.

Risk:

- This is safe for current MVP if normalization only disables airborne dynamics.
- It becomes unsafe if normalization overwrites configured Ground altitude/elevation.
- Ground `currentTask.targetAltitudeMeters` should be aligned to the current Ground altitude so tasks do not command climb/descent.

Safe fix candidate for 1.5/1.6:

- Normalize Ground task target altitude to the entity current altitude at assignment time.
- Preserve Ground physical altitude at runtime while clearing vertical speed, pitch, and roll.
- Add tests for Ground movement tasks preserving current altitude, pitch `0`, roll `0`, and vertical speed `0`.

### 4. Completed movement tasks force speed zero, but non-movement terminal cleanup relies on engine guards

The engine stops entities when `currentTask.enabled` is false or status is terminal.

Risk:

- If a task completes but leaves status briefly as `Running`, the engine can still advance one tick.
- Attack processor and plan executor run outside the visible `ScenarioState::advanceSimulation` sequence, so exact ordering matters.

Safe fix candidate for 1.4:

- Ensure plan advancement checks `Completed` before override/failure, then immediately starts next step.
- Add regression tests around `AttackAir -> ReturnToBase`, `InterceptEntity -> ReturnToBase`, and `FollowRoute -> InterceptEntity`.

### 5. JSBSim path is mostly bypassed for current movement tasks

When `QTTEST_HAS_JSBSIM` is enabled, most current movement task types prefer kinematic guidance and bypass JSBSim.

Risk:

- JSBSim integration exists but is not the active path for most tactical movement tasks.
- Feature 1.10 should not assume current tasks are already JSBSim-driven.

Safe fix candidate for 1.10:

- Keep fallback kinematic path explicit.
- Introduce a task-to-control-mode matrix before enabling JSBSim broadly.

## Confirmed Safe Areas

- Domain tasks in `src/domain/Task.cpp` do not mutate `Entity` directly. They return `DesiredState` and internal task state only.
- Main physical position writes are centralized in `FlightDynamicsEngine`.
- Terminal task status is checked before movement for both Air and Ground in `FlightDynamicsEngine::advanceEntity`.
- Ground pitch, roll, and vertical speed are forced safe in the engine; altitude is preserved as configured/current elevation.
- Destroyed entities have task stacks cleared and speed/vertical speed set to zero in `ScenarioState::setEntityDestroyed`.

## Recommended Fix List for Feature 1.4

Priority 1:

- Add regression tests that a `Completed`, `Failed`, or `Target unavailable` task does not change `latitude`, `longitude`, `altitude`, `headingDegrees`, or `speedKnots` on the next engine tick.
- Add tests for plan step transition cleanup:
  - `FollowRoute -> InterceptEntity -> ReturnToBase`
  - `AttackAir -> ReturnToBase`
  - `WaitOnLocation -> ReturnToBase`

Priority 2:

- Introduce a small kinematics cleanup helper, for example `clearTransientMovementState(Entity&)`, that resets:
  - `speedKnots`
  - `verticalSpeedMetersPerSecond`
  - pitch/roll relaxation remains engine-owned
- Use it only on terminal/cancelled/destroyed/plan-step transition paths.

Priority 3:

- Add a helper for task classification shared by `FlightDynamicsEngine` and `TaskApplicator`.
- Today each file has local `isMovementTaskType` / `isInterceptEntityTaskType` helpers. They should not drift.

Priority 4:

- Decide whether modern movement tasks should be allowed to move without a `TaskStack`.
- If fallback remains, document it as legacy compatibility and test it.

## Recommended Fix List for Feature 1.5 / 1.6

Implemented baseline:

- Normalize Ground `targetAltitudeMeters` to the entity current altitude during task assignment and before runtime integration.
- Keep Ground physical altitude as configured/current elevation while clearing pitch, roll, and vertical speed.
- Prevent Ground entities from drifting on residual speed when the active task is not a Ground movement task.
- Reject explicitly air-only movement/attack pursuit tasks for Ground at task assignment time.
- Add regression tests:
  - Ground MoveToLocation moves horizontally only.
  - Ground non-movement task with residual speed does not move.
  - Ground air-only movement task does not move or climb.
  - Ground FollowRoute keeps current configured altitude.
  - Ground InterceptEntity keeps current configured altitude.
  - Air WaitOnLocation respects configured altitude.
  - ReturnToBase uses home altitude for Air.

## Recommended Fix List for Feature 1.8

Implemented baseline:

- Introduce `MovementIntent` as the small adapter between task setpoints and physical kinematics.
- Keep task/runtime behavior unchanged while centralizing heading, speed, and vertical-speed interpolation.
- Route both TaskStack-driven movement and legacy fallback movement through the same intent application helper.
- Add regression tests for:
  - shortest heading turn across 0/360 degrees;
  - climb/descent intent producing correct vertical speed sign;
  - Air turn + climb deriving positive pitch/roll;
  - Air turn + descent deriving negative pitch/roll.

Deferred to Feature 1.9:

- Advanced turn-rate limits by platform.
- Acceleration/deceleration profiles by aircraft type.
- More realistic pitch/roll coupling and energy/performance modeling.

Remaining future work:

- Replace the current Ground altitude fallback with a terrain/elevation service when EPIC 16 adds world elevation sampling.
- Move duplicated task classification helpers into one shared module before larger movement-controller refactors.

## Recommended Fix List for Feature 1.7

Implemented baseline:

- Sustained attack pursuit still writes task setpoints while the attack is active.
- `AttackAir` and `AttackUntilDestroyed` now clear pursuit setpoints when they enter a terminal status.
- Terminal pursuit cleanup resets task target latitude/longitude/altitude/heading to current ownship state and target speed to `0`.
- Existing plan progression tests cover `AttackAir -> ReturnToBase` replacing stale pursuit targets.
- Added regression tests:
  - `AttackAir` completed by destroyed target clears pursuit targets.
  - `AttackAir` failed against invalid/friendly target clears pursuit targets.
  - `AttackUntilDestroyed` completed by destroyed target clears pursuit targets.
  - `AttackUntilDestroyed` failed by no ammo clears pursuit targets.

Remaining future work:

- Treat sustained attack pursuit as a first-class movement intent in Feature 1.8 instead of writing setpoints directly from `AttackTaskProcessor`.

## Future Refactor Direction for Feature 1.8

Suggested conceptual model:

```cpp
struct MovementIntent {
  enum class Mode {
    None,
    HoldPosition,
    MoveToLocation,
    FollowEntity,
    InterceptEntity,
    Route,
    Racetrack,
    AttackPursuit
  };

  Mode mode = Mode::None;
  std::optional<double> desiredLatitude;
  std::optional<double> desiredLongitude;
  std::optional<double> desiredAltitudeMeters;
  std::optional<double> desiredHeadingDegrees;
  std::optional<double> desiredSpeedKnots;
  QString sourceTaskType;
};
```

Short-term rule:

- Tasks and attack processors decide intent.
- `FlightDynamicsEngine` executes intent.
- Physical fields should only be written by the movement engine or lifecycle cleanup.

## Audit Conclusion

The current system is stable enough to proceed to Feature 1.4 without a large refactor. The main physical kinematic authority is already `FlightDynamicsEngine`. The highest-value hardening is not architectural surgery; it is focused terminal-state and plan-transition cleanup, plus regression tests around stale movement setpoints.
