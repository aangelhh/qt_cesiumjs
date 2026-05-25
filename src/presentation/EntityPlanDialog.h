#pragma once

#include <QDialog>
#include <QString>
#include <functional>

#include "presentation/PlanTypes.h"

class QListWidget;
class QLabel;
class QPushButton;
class ScenarioState;

namespace presentation {

class EntityPlanExecutor;

/// Modal-ish dialog to edit and run an entity's plan. Extracted from
/// MainWindow::openEntityPlanDialog to keep the God Class smaller.
///
/// UI side-effects on MainWindow (log, status bar, full UI resync) are
/// injected as callbacks.
class EntityPlanDialog : public QDialog {
  Q_OBJECT
public:
  using ConfigureStepFn = std::function<bool(const QString& entityName,
                                              PlanStepKind kind,
                                              PlanStep& step)>;
  using LogFn           = std::function<void(const QString&)>;
  using StatusFn        = std::function<void(const QString&)>;
  using SyncUiFn        = std::function<void()>;

  EntityPlanDialog(
      const QString&       entityName,
      ScenarioState*       scenarioState,
      EntityPlanExecutor*  planExecutor,
      ConfigureStepFn      configureStep,
      LogFn                log,
      StatusFn             status,
      SyncUiFn             syncUi,
      QWidget*             parent = nullptr);

private:
  void refreshList();
  void refreshButtons();
  void onAdd();
  void onRemove();
  void onMoveUp();
  void onMoveDown();
  void onRun();
  void onStop();

  bool blockIfRunning();

  QString             _entityName;
  ScenarioState*      _scenarioState;
  EntityPlanExecutor* _planExecutor;
  ConfigureStepFn     _configureStep;
  LogFn               _log;
  StatusFn            _status;
  SyncUiFn            _syncUi;

  QListWidget* _stepsList{nullptr};
  QLabel*      _planStatusLabel{nullptr};
  QPushButton* _addButton{nullptr};
  QPushButton* _removeButton{nullptr};
  QPushButton* _upButton{nullptr};
  QPushButton* _downButton{nullptr};
  QPushButton* _runButton{nullptr};
  QPushButton* _stopButton{nullptr};
};

} // namespace presentation
