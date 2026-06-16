#include "presentation/EntityPlanDialog.h"
#include "presentation/EntityPlanExecutor.h"
#include "application/ScenarioState.h"
#include "domain/CombatRules.h"
#include "domain/Entity.h"

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace presentation {

EntityPlanDialog::EntityPlanDialog(
    const QString&       entityName,
    ScenarioState*       scenarioState,
    EntityPlanExecutor*  planExecutor,
    ConfigureStepFn      configureStep,
    LogFn                log,
    StatusFn             status,
    SyncUiFn             syncUi,
    QWidget*             parent)
  : QDialog(parent)
  , _entityName(entityName)
  , _scenarioState(scenarioState)
  , _planExecutor(planExecutor)
  , _configureStep(std::move(configureStep))
  , _log(std::move(log))
  , _status(std::move(status))
  , _syncUi(std::move(syncUi)) {

  this->setWindowTitle(QStringLiteral("Plan for %1").arg(entityName));
  this->resize(520, 420);
  this->setWindowModality(Qt::WindowModal);

  _planExecutor->ensurePlan(entityName);

  auto* layout = new QVBoxLayout(this);
  _stepsList = new QListWidget(this);
  layout->addWidget(_stepsList);
  _planStatusLabel = new QLabel(this);
  layout->addWidget(_planStatusLabel);

  auto* editButtons = new QHBoxLayout();
  _addButton    = new QPushButton(QStringLiteral("Add"), this);
  _removeButton = new QPushButton(QStringLiteral("Remove"), this);
  _upButton     = new QPushButton(QStringLiteral("Up"), this);
  _downButton   = new QPushButton(QStringLiteral("Down"), this);
  editButtons->addWidget(_addButton);
  editButtons->addWidget(_removeButton);
  editButtons->addWidget(_upButton);
  editButtons->addWidget(_downButton);
  editButtons->addStretch(1);
  layout->addLayout(editButtons);

  auto* actionButtons = new QHBoxLayout();
  _runButton  = new QPushButton(QStringLiteral("Run Plan"), this);
  _stopButton = new QPushButton(QStringLiteral("Stop Plan"), this);
  auto* closeButton = new QPushButton(QStringLiteral("Close"), this);
  actionButtons->addWidget(_runButton);
  actionButtons->addWidget(_stopButton);
  actionButtons->addStretch(1);
  actionButtons->addWidget(closeButton);
  layout->addLayout(actionButtons);

  connect(_stepsList, &QListWidget::currentRowChanged, this, [this](int) { refreshButtons(); });
  connect(_addButton,    &QPushButton::clicked, this, &EntityPlanDialog::onAdd);
  connect(_removeButton, &QPushButton::clicked, this, &EntityPlanDialog::onRemove);
  connect(_upButton,     &QPushButton::clicked, this, &EntityPlanDialog::onMoveUp);
  connect(_downButton,   &QPushButton::clicked, this, &EntityPlanDialog::onMoveDown);
  connect(_runButton,    &QPushButton::clicked, this, &EntityPlanDialog::onRun);
  connect(_stopButton,   &QPushButton::clicked, this, &EntityPlanDialog::onStop);
  connect(closeButton,   &QPushButton::clicked, this, &QDialog::accept);

  auto* refreshTimer = new QTimer(this);
  refreshTimer->setInterval(250);
  connect(refreshTimer, &QTimer::timeout, this, [this]() {
    refreshList();
    refreshButtons();
  });
  refreshTimer->start();

  refreshList();
  refreshButtons();
}

bool EntityPlanDialog::blockIfRunning() {
  if (_planExecutor->ensurePlan(_entityName).running) {
    _status(QStringLiteral("No puedes editar steps mientras el plan esta en ejecucion."));
    refreshButtons();
    return true;
  }
  return false;
}

void EntityPlanDialog::refreshList() {
  const EntityPlan& plan = _planExecutor->ensurePlan(_entityName);
  const int previousRow = _stepsList->currentRow();
  _stepsList->clear();
  _planStatusLabel->setText(
      QStringLiteral("Plan Status: %1").arg(plan.status.trimmed().isEmpty()
          ? QStringLiteral("NotStarted")
          : domain::planStatusDisplayLabel(plan.status.trimmed())));
  for (int index = 0; index < plan.steps.size(); ++index) {
    const QString prefix =
        (plan.running && index == plan.currentStepIndex) ? QStringLiteral(">> ") : QString();
    _stepsList->addItem(
        QStringLiteral("%1%2. [%3] %4")
            .arg(prefix)
            .arg(index + 1)
            .arg(plan.steps.at(index).status.trimmed().isEmpty()
                     ? QStringLiteral("NotStarted")
                     : plan.steps.at(index).status.trimmed())
            .arg(EntityPlanExecutor::planStepDisplayLabel(plan.steps.at(index))));
  }
  if (!plan.steps.isEmpty()) {
    _stepsList->setCurrentRow(qBound(0, previousRow, plan.steps.size() - 1));
  }
}

void EntityPlanDialog::refreshButtons() {
  const EntityPlan& plan = _planExecutor->ensurePlan(_entityName);
  const int currentRow = _stepsList->currentRow();
  const bool hasSelection = currentRow >= 0 && currentRow < plan.steps.size();
  const bool editable = !plan.running;

  const Entity* entity = nullptr;
  for (const Entity& e : _scenarioState->entities()) {
    if (e.name == _entityName) { entity = &e; break; }
  }

  _addButton->setEnabled(editable);
  _removeButton->setEnabled(editable && hasSelection);
  _upButton->setEnabled(editable && hasSelection && currentRow > 0);
  _downButton->setEnabled(
      editable && hasSelection &&
      currentRow >= 0 && currentRow < plan.steps.size() - 1);
  _runButton->setEnabled(editable && entity && !entity->destroyed && !plan.steps.isEmpty());
  _stopButton->setEnabled(plan.running);
}

void EntityPlanDialog::onAdd() {
  if (blockIfRunning()) {
    return;
  }

  QMenu addMenu(this);
  struct Item { const char* label; PlanStepKind kind; bool separatorBefore; };
  static const Item items[] = {
      {"Move To Location",              PlanStepKind::MoveToLocation,           false},
      {"Wait on Location",              PlanStepKind::WaitOnLocation,           false},
      {"Move To Waypoint",              PlanStepKind::MoveToWaypoint,           false},
      {"Follow Route",                  PlanStepKind::FollowRoute,              false},
      {"Patrol Area",                   PlanStepKind::PatrolArea,               false},
      {"Fly Heading / Altitude / Speed",PlanStepKind::FlyHeadingAltitudeSpeed,  false},
      {"Orbit / Hold (Location)",       PlanStepKind::OrbitHoldLocation,        false},
      {"Hold Racetrack",                PlanStepKind::HoldRacetrack,            false},
      {"Follow Entity",                 PlanStepKind::FollowEntity,             false},
      {"Intercept Entity",              PlanStepKind::InterceptEntity,           false},
      {"Return To Base",                PlanStepKind::ReturnToBase,             false},
      {"Attack Once",                   PlanStepKind::AttackOnce,               true },
      {"Attack Until Destroyed",        PlanStepKind::AttackUntilDestroyed,     false},
      {"Fire on Position",              PlanStepKind::FireOnPosition,           false},
      {"Fire in Direction",             PlanStepKind::FireInDirection,          false},
      {"Stop Weapons Task",             PlanStepKind::StopWeaponsTask,          false},
      {"Attack Air",                    PlanStepKind::AttackAir,                false},
      {"Attack Surface",                PlanStepKind::AttackSurface,            false},
      {"Wait Until Target Detected",    PlanStepKind::WaitUntilTargetDetected,  true },
      {"Wait Until Target Destroyed",   PlanStepKind::WaitUntilTargetDestroyed, false},
      {"Wait Until Damaged",            PlanStepKind::WaitUntilDamaged,         false},
      {"Wait Until Time",               PlanStepKind::WaitUntilTime,            false},
      {"Wait Until In Range",           PlanStepKind::WaitUntilInRange,         false},
  };
  for (const Item& item : items) {
    if (item.separatorBefore) {
      addMenu.addSeparator();
    }
    QAction* action = addMenu.addAction(QString::fromLatin1(item.label));
    action->setData(static_cast<int>(item.kind));
  }

  QAction* selected = addMenu.exec(_addButton->mapToGlobal(QPoint(0, _addButton->height())));
  if (!selected) {
    return;
  }

  PlanStep step;
  this->hide();
  const bool configured = _configureStep(
      _entityName, static_cast<PlanStepKind>(selected->data().toInt()), step);
  this->show();
  this->raise();
  this->activateWindow();
  if (!configured) {
    return;
  }

  EntityPlan& plan = _planExecutor->ensurePlan(_entityName);
  plan.steps.push_back(step);
  refreshList();
  _stepsList->setCurrentRow(plan.steps.size() - 1);
  refreshButtons();
}

void EntityPlanDialog::onRemove() {
  if (blockIfRunning()) {
    return;
  }
  EntityPlan& plan = _planExecutor->ensurePlan(_entityName);
  const int row = _stepsList->currentRow();
  if (row < 0 || row >= plan.steps.size()) {
    return;
  }
  plan.steps.removeAt(row);
  refreshList();
  refreshButtons();
}

void EntityPlanDialog::onMoveUp() {
  if (blockIfRunning()) {
    return;
  }
  EntityPlan& plan = _planExecutor->ensurePlan(_entityName);
  const int row = _stepsList->currentRow();
  if (row <= 0 || row >= plan.steps.size()) {
    return;
  }
  plan.steps.swapItemsAt(row, row - 1);
  refreshList();
  _stepsList->setCurrentRow(row - 1);
  refreshButtons();
}

void EntityPlanDialog::onMoveDown() {
  if (blockIfRunning()) {
    return;
  }
  EntityPlan& plan = _planExecutor->ensurePlan(_entityName);
  const int row = _stepsList->currentRow();
  if (row < 0 || row >= plan.steps.size() - 1) {
    return;
  }
  plan.steps.swapItemsAt(row, row + 1);
  refreshList();
  _stepsList->setCurrentRow(row + 1);
  refreshButtons();
}

void EntityPlanDialog::onRun() {
  if (!_planExecutor->startPlan(_entityName)) {
    return;
  }
  _syncUi();
  refreshList();
  refreshButtons();
}

void EntityPlanDialog::onStop() {
  _planExecutor->stopPlan(_entityName, true);
  _log(QStringLiteral("Plan stopped for %1").arg(_entityName));
  _status(QStringLiteral("Plan detenido para %1.").arg(_entityName));
  refreshList();
  refreshButtons();
}

} // namespace presentation
