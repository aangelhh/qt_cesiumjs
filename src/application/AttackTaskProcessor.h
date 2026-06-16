#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <functional>

class ScenarioState;
struct Entity;

namespace presentation {
class BombReleaseController;
}

namespace application {

/// Processes per-tick advancement of AttackAir and AttackSurface tasks.
/// Holds cooldown / elapsed time state.
/// Side effects (logging, status, bomb queueing) are injected.
class AttackTaskProcessor : public QObject {
  Q_OBJECT
public:
  using LogFn    = std::function<void(const QString&)>;
  using StatusFn = std::function<void(const QString&)>;
  using QueueBombFn = std::function<void(
      const QString& launcher, double lat, double lon, double alt,
      const QString& label, const QString& source,
      const QString& targetEntity, bool log, bool focus)>;

  explicit AttackTaskProcessor(
      ScenarioState* state,
      presentation::BombReleaseController* bombCtrl,
      LogFn         log,
      StatusFn      setStatus,
      QueueBombFn   queueBomb,
      QObject*      parent = nullptr);

  /// Process all attack tasks for one simulation tick.
  void processAttackTasks(double deltaSeconds, bool simulationRunning);

  /// Process auto-bombing behaviors for aggressive entities.
  void processAutoBombing(double deltaSeconds, bool simulationRunning);

  /// Remove timing state when an entity is destroyed or removed.
  void removeEntity(const QString& entityName);

  QHash<QString, double>& attackAirElapsedSeconds();
  QHash<QString, double>& attackAirMissileCooldownSeconds();
  QHash<QString, double>& autoBombReleaseCooldownSeconds();
  QHash<QString, int>&    autoBehaviorDamageReactionLevel();

private:
  bool processAttackOnceTask(const QString& entityName, double deltaSeconds);
  bool processAttackUntilDestroyedTask(const QString& entityName, double deltaSeconds);
  bool processAttackAirTask(const QString& entityName, double deltaSeconds);
  bool processAttackSurfaceTask(const QString& entityName);
  bool setEntityTaskStatus(const QString& entityName, const QString& status);

  ScenarioState* _state;
  presentation::BombReleaseController* _bombCtrl;
  LogFn       _log;
  StatusFn    _setStatus;
  QueueBombFn _queueBomb;

  QHash<QString, double> _attackAirElapsedSeconds;
  QHash<QString, double> _attackAirMissileCooldownSeconds;
  QHash<QString, double> _autoBombReleaseCooldownSeconds;
  QHash<QString, int>    _autoBehaviorDamageReactionLevel;

  static constexpr double kAttackAirTimeoutSeconds = 120.0;
  static constexpr double kAttackAirMissileCooldownSeconds = 8.0;
  static constexpr double kAttackAirMinimumPursuitSpeedKnots = 320.0;
  static constexpr double kAttackAirTargetSpeedMarginKnots = 60.0;
  static constexpr double kAutoBombReleaseCooldownSeconds = 20.0;
};

} // namespace application
