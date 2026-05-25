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
