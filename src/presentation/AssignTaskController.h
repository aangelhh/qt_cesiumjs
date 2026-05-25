#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <functional>

#include "domain/Task.h"

struct EntityPlan;

namespace presentation {

/// Handles the "assign task" flow triggered from the UI:
///  1. Guards (entity selected, operable, plan not running)
///  2. Extracts the current EntityTask from the track-summary QVariantMap
///  3. Delegates to an injected captureConfig callback (shows the dialog)
///  4. Applies the configured task via an injected applyTask callback
///
/// All side-effects are injected so the guard + extraction logic is fully
/// unit-testable without a QApplication.
class AssignTaskController : public QObject {
  Q_OBJECT
public:
  using SelectedEntityFn  = std::function<QString()>;
  using IsOperableFn      = std::function<bool()>;
  using IsPlanRunningFn   = std::function<bool(const QString&)>;
  using CurrentSummaryFn  = std::function<QVariantMap()>;
  using CaptureConfigFn   = std::function<bool(
      const QString& entityName,
      const EntityTask& initial,
      const QString& taskType,
      EntityTask& outTask)>;
  using ApplyTaskFn       = std::function<bool(const QString&, const EntityTask&)>;
  using StatusFn          = std::function<void(const QString&)>;

  explicit AssignTaskController(
      SelectedEntityFn  selectedEntityName,
      IsOperableFn      isOperable,
      IsPlanRunningFn   isPlanRunning,
      CurrentSummaryFn  currentSummary,
      CaptureConfigFn   captureConfig,
      ApplyTaskFn       applyTask,
      StatusFn          setStatus,
      QObject*          parent = nullptr);

  void open(const QString& initialTaskType);

  /// Converts a track-summary QVariantMap to an EntityTask.
  /// Exposed as static so it can be called and tested independently.
  static EntityTask taskFromSummary(const QVariantMap& summary);

private:
  SelectedEntityFn  _selectedEntityName;
  IsOperableFn      _isOperable;
  IsPlanRunningFn   _isPlanRunning;
  CurrentSummaryFn  _currentSummary;
  CaptureConfigFn   _captureConfig;
  ApplyTaskFn       _applyTask;
  StatusFn          _setStatus;
};

} // namespace presentation
