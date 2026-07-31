#pragma once

#include "domain/Task.h"

#include <QHash>
#include <QString>

#include <functional>

class ScenarioState;

namespace application {

enum class EntityControlAuthority {
  None,
  Task,
  Plan,
  Behavior,
  CockpitAutopilot,
  CockpitDirect,
  ExternalModel,
};

struct FlightControlCommand {
  QString entityName;
  double headingDegrees = 0.0;
  int altitudeMeters = 0;
  double speedKnots = 0.0;
};

QString controlAuthorityLabel(EntityControlAuthority authority);

class CockpitControlService {
public:
  using ApplyTaskFn = std::function<bool(const QString&, const EntityTask&, bool)>;
  using StopPlanFn = std::function<void(const QString&, bool)>;
  using LogFn = std::function<void(const QString&)>;

  CockpitControlService(
      ScenarioState* state,
      ApplyTaskFn applyTask,
      StopPlanFn stopPlan,
      LogFn log = {});

  bool takeControl(const FlightControlCommand& command);
  bool updateSetpoints(const FlightControlCommand& command);
  bool releaseControl(const QString& entityName);

  bool hasControl(const QString& entityName) const;
  EntityControlAuthority authorityFor(const QString& entityName) const;
  void reconcile(const QString& entityName, const EntityTask& activeTask);

private:
  bool applyCockpitTask(const FlightControlCommand& command);
  bool commandIsValid(const FlightControlCommand& command) const;

  ScenarioState* _state;
  ApplyTaskFn _applyTask;
  StopPlanFn _stopPlan;
  LogFn _log;
  QHash<QString, EntityControlAuthority> _authorities;
};

} // namespace application
