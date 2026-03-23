Layered frontend layout

- ui
  - MainWindow
  - MainWindow.ui
  - AddFighterDialog
- presentation
  - FighterTextFormatter
- application
  - ScenarioState
- domain
  - FighterEntity
- infrastructure
  - CesiumScenePage
  - MapBridge

Intent
- MainWindow orchestrates widgets and workflows.
- Dialogs collect user input.
- Domain holds tactical data structures.
- Application owns scenario state and use-case style coordination.
- Infrastructure adapts Qt WebEngine and CesiumJS integration.
