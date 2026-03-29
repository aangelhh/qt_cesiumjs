Layered frontend layout

- ui
  - MainWindow
  - MainWindow.ui
  - AddEntityDialog
- presentation
  - EntityTextFormatter
- application
  - ScenarioState
  - SensorEngine
  - FlightDynamicsEngine
- domain
  - Entity
  - Sensor
- infrastructure
  - CesiumScenePage
  - MapBridge
  - ModelCatalog
  - DisEntityCatalog

Intent
- MainWindow orchestrates widgets and workflows.
- Dialogs collect user input.
- Domain holds tactical data structures.
- Application owns scenario state and use-case style coordination.
- Infrastructure adapts Qt WebEngine and CesiumJS integration.
