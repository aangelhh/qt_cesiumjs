# MBSE / SysML-Oriented PlantUML

These diagrams document the movement-task subsystem from a more MBSE-oriented
point of view: mission use cases, requirements, operational behavior, state
machines, SysML-like blocks and validation cases.

PlantUML does not implement full SysML notation, so these diagrams use UML with
SysML-style stereotypes such as `<<block>>`, `<<requirement>>`,
`<<constraint>>`, `<<valueType>>`, `<<testCase>>` and `<<interface>>`.

Render with:

```bash
plantuml Architecture/plantuml/mbse/*.puml
```

## Diagrams

- `01_use_cases_movement_tasks.puml` - operational use cases.
- `02_requirements_traceability.puml` - SysML-style requirements and trace links.
- `03_bdd_movement_task_system.puml` - block definition diagram.
- `04_ibd_runtime_context.puml` - internal block/runtime context diagram.
- `05_activity_plan_execution.puml` - plan execution activity.
- `06_state_machine_entity_task.puml` - task/plan status state machine.
- `07_sequence_follow_route_to_intercept.puml` - operational sequence for Follow Route -> Intercept -> next step.
- `08_parametric_completion_constraints.puml` - completion/failure constraints.
- `09_validation_test_cases.puml` - validation test cases traced to requirements.
