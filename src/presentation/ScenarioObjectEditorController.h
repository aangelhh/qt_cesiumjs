#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <functional>

class ScenarioState;

/// Handles object-lifecycle commands for the currently selected entity or
/// tactical graphic: deleteSelected and clearSelectedTask.
///
/// Abstracts the multi-type dispatch (Entity / Waypoint / Route / Area)
/// and all side-effects (log, status, syncUi, removeTrackFromMap, cleanup).
namespace presentation {

class ScenarioObjectEditorController : public QObject {
  Q_OBJECT
public:
  using SelectedNameFn    = std::function<QString()>;
  using SelectedObjectFn  = std::function<QString()>;
  using IsEntityFn        = std::function<bool()>;
  using IsDestroyedFn     = std::function<bool()>;
  using SummaryFn         = std::function<QVariantMap()>;
  using CleanupEntityFn   = std::function<QString(const QString&)>;
  using RemoveTrackFn     = std::function<void(const QString&)>;
  using LogFn             = std::function<void(const QString&)>;
  using StatusFn          = std::function<void(const QString&)>;
  using SyncUiFn          = std::function<void()>;

  explicit ScenarioObjectEditorController(
      ScenarioState*    state,
      SelectedNameFn    selectedEntityName,
      SelectedObjectFn  selectedObjectName,
      IsEntityFn        isEntity,
      IsDestroyedFn     isDestroyed,
      SummaryFn         summary,
      CleanupEntityFn   cleanupEntity,
      RemoveTrackFn     removeTrack,
      LogFn             log,
      StatusFn          setStatus,
      SyncUiFn          syncUi,
      QObject*          parent = nullptr);

  void deleteSelected();
  void clearSelectedTask();

private:
  ScenarioState*    _state;
  SelectedNameFn    _selectedEntityName;
  SelectedObjectFn  _selectedObjectName;
  IsEntityFn        _isEntity;
  IsDestroyedFn     _isDestroyed;
  SummaryFn         _summary;
  CleanupEntityFn   _cleanupEntity;
  RemoveTrackFn     _removeTrack;
  LogFn             _log;
  StatusFn          _setStatus;
  SyncUiFn          _syncUi;
};

} // namespace presentation
