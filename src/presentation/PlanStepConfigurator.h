#pragma once

#include <QString>
#include <functional>

#include "presentation/PlanTypes.h"

struct Entity;
struct AreaDefinition;

namespace presentation {

struct EntityHomePosition;

/// Configures a PlanStep for the given entity and step kind.
///
/// All UI interactions are injected as callbacks so the pure logic can
/// be exercised in tests without a QApplication.
class PlanStepConfigurator {
public:
  /// Show AssignTaskDialog; returns true iff user accepted.
  using CaptureFn   = std::function<bool(
      const QString& entityName,
      const EntityTask& initialTask,
      const QString& initialTaskType,
      EntityTask& outTask)>;

  /// Lookup an area by name or id; returns pointer into ScenarioState
  /// (caller must not store it past the call).
  using FindAreaFn  = std::function<const AreaDefinition*(const QString& nameOrId)>;

  /// Lookup the recorded home position for an entity.
  using HomePosFn   = std::function<EntityHomePosition(const QString& entityName)>;

  /// Show QInputDialog::getItem; returns chosen item and sets ok.
  using AskItemFn   = std::function<QString(
      const QString& title,
      const QString& label,
      const QStringList& items,
      bool& ok)>;

  /// Show QInputDialog::getDouble; returns value and sets ok.
  using AskDoubleFn = std::function<double(
      const QString& title,
      const QString& label,
      double defaultValue,
      double min,
      double max,
      bool& ok)>;

  PlanStepConfigurator(
      CaptureFn   capture,
      FindAreaFn  findArea,
      HomePosFn   homePos,
      AskItemFn   askItem,
      AskDoubleFn askDouble);

  /// Populate `step` interactively. Returns false if the user cancelled.
  bool configure(
      const Entity&     entity,
      double            defaultHeading,
      int               defaultAltitudeMeters,
      double            defaultSpeedKnots,
      PlanStepKind      kind,
      PlanStep&         step);

private:
  CaptureFn   _capture;
  FindAreaFn  _findArea;
  HomePosFn   _homePos;
  AskItemFn   _askItem;
  AskDoubleFn _askDouble;
};

} // namespace presentation
