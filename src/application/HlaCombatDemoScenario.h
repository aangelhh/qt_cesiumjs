#pragma once

#include "domain/Entity.h"

#include <QVector>

namespace application {

struct HlaCombatDemoScenario {
  Entity friendly;
  Entity opposing;
  EntityTask opposingMovementTask;
  QVector<EntityTask> friendlyPlanTasks;
};

HlaCombatDemoScenario makeHlaCombatDemoScenario();

} // namespace application
