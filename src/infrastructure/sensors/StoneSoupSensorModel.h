#pragma once

#include "application/sensors/ISensorModel.h"

#include <QByteArray>
#include <QMutex>
#include <QProcess>
#include <QVariantMap>

namespace infrastructure::sensors {

class StoneSoupSensorModel final
    : public application::sensors::ISensorModel {
public:
  struct Configuration {
    QString pythonExecutable;
    QString serviceScript;
    QVariantMap options;
    int requestTimeoutMilliseconds = 100;
  };

  explicit StoneSoupSensorModel(Configuration configuration);
  ~StoneSoupSensorModel() override;

  QString modelId() const override;
  application::sensors::SensorEvaluationResult evaluate(
      const application::sensors::SensorEvaluationContext& context) const override;

  bool isAvailable() const;
  QString errorString() const;

private:
  bool ensureStartedLocked() const;
  bool exchangeLocked(
      const QByteArray& request,
      QByteArray& response,
      int timeoutMilliseconds = -1) const;
  void stopLocked() const;

  Configuration _configuration;
  mutable QMutex _mutex;
  mutable QProcess _process;
  mutable QByteArray _stdoutBuffer;
  mutable QString _errorString;
};

} // namespace infrastructure::sensors
