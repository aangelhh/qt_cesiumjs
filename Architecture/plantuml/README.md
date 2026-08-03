# PlantUML Architecture

This folder documents the current task and plan architecture around:

- Follow Entity
- Intercept Entity
- Follow Route / Trajectory
- waypoint and route altitude support
- plan execution and task progression

Render all diagrams with PlantUML, for example:

```bash
plantuml Architecture/plantuml/*.puml
```

## Diagrams

- `01_context_and_layers.puml` - high-level module/layer view.
- `02_task_domain_model.puml` - task, plan, entity, waypoint and route model.
- `03_task_assignment_flow.puml` - UI to task stack assignment flow.
- `04_simulation_tick_flow.puml` - runtime tick and plan advancement flow.
- `05_follow_route_sequence.puml` - Follow Route waypoint progression, altitude and overshoot handling.
- `06_intercept_entity_sequence.puml` - unified Intercept Entity 3D progression.
- `07_serializer_compatibility.puml` - save/load and legacy compatibility.
- `08_current_system_architecture.puml` - current layered architecture including cockpit, telemetry, Cesium and optional JSBSim.
- `09_dynamics_cockpit_and_telemetry.puml` - runtime sequence from cockpit setpoints through movement intent and telemetry views.
- `10_execution_modes_and_analysis.puml` - target execution architecture shared by real-time, fast-time, replay and operational analysis.
- `11_extensible_model_runtime.puml` - target interfaces and adapters for native, FMI and externally hosted models.
- `12_runtime_tick_and_runner_evolution.puml` - current Qt-driven tick and target common SimulationRunner contract.
- `13_task_and_plan_lifecycle.puml` - task assignment, terminal-state handling and plan progression.
