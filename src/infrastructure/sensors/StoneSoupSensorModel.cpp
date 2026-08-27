#include "infrastructure/sensors/StoneSoupSensorModel.h"

#include "application/SensorDetectionModel.h"
#include "application/sensors/NativeSensorModel.h"
#include "domain/EntityIdentity.h"

#include <QElapsedTimer>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

#include <algorithm>
#include <cmath>

namespace {

application::sensors::SensorEvaluationResult nativeFallback(
    const application::sensors::SensorEvaluationContext& context,
    const QString& reason) {
  application::sensors::NativeSensorModel fallback;
  application::sensors::SensorEvaluationResult result = fallback.evaluate(context);
  result.fallbackUsed = true;
  result.fallbackReason = reason;
  return result;
}

} // namespace

namespace infrastructure::sensors {

StoneSoupSensorModel::StoneSoupSensorModel(Configuration configuration)
    : _configuration(std::move(configuration)) {
}

StoneSoupSensorModel::~StoneSoupSensorModel() {
  QMutexLocker lock(&_mutex);
  this->stopLocked();
}

QString StoneSoupSensorModel::modelId() const {
  return QStringLiteral("stone-soup");
}

application::sensors::SensorEvaluationResult StoneSoupSensorModel::evaluate(
    const application::sensors::SensorEvaluationContext& context) const {
  QJsonObject request;
  request.insert(QStringLiteral("operation"), QStringLiteral("evaluate"));
  request.insert(
      QStringLiteral("simulationTimeSeconds"),
      context.simulationTimeSeconds);
  request.insert(QStringLiteral("rangeMeters"), context.rangeMeters);
  request.insert(
      QStringLiteral("configuredProbability"),
      context.sensor.probabilityOfDetection);
  request.insert(
      QStringLiteral("targetSignature"),
      application::SensorDetectionModel::targetSignature(
          context.sensor,
          context.target));
  request.insert(
      QStringLiteral("options"),
      QJsonObject::fromVariantMap(_configuration.options));

  QByteArray responseBytes;
  {
    QMutexLocker lock(&_mutex);
    if (!this->exchangeLocked(
            QJsonDocument(request).toJson(QJsonDocument::Compact),
            responseBytes)) {
      return nativeFallback(
          context,
          _errorString.trimmed().isEmpty()
              ? QStringLiteral("Stone Soup provider is unavailable")
              : _errorString);
    }
  }

  QJsonParseError parseError;
  const QJsonDocument responseDocument =
      QJsonDocument::fromJson(responseBytes, &parseError);
  const QJsonObject response = responseDocument.object();
  if (parseError.error != QJsonParseError::NoError ||
      !response.value(QStringLiteral("ok")).toBool(false)) {
    const QString reason = response.value(QStringLiteral("error")).toString();
    return nativeFallback(
        context,
        reason.trimmed().isEmpty()
            ? QStringLiteral("Stone Soup returned an invalid response")
            : reason);
  }

  application::sensors::SensorEvaluationResult result;
  result.probability = std::clamp(
      response.value(QStringLiteral("probability")).toDouble(),
      0.0,
      1.0);
  result.sample = application::SensorDetectionModel::deterministicSample(
      context.scenarioSeed,
      domain::entityKey(context.observer),
      context.sensor.id.trimmed().isEmpty()
          ? context.sensor.name
          : context.sensor.id,
      domain::entityKey(context.target),
      context.evaluationIndex);
  result.detected = result.sample < result.probability;
  result.effectiveModelId = this->modelId();
  result.providerVersion =
      response.value(QStringLiteral("providerVersion")).toString();
  result.targetSignature = application::SensorDetectionModel::targetSignature(
      context.sensor,
      context.target);
  const double signalToNoiseRatio =
      response.value(QStringLiteral("snr")).toDouble();
  if (std::isfinite(signalToNoiseRatio) && signalToNoiseRatio >= 0.0) {
    result.signalToNoiseRatio = signalToNoiseRatio;
    if (signalToNoiseRatio > 0.0) {
      result.signalToNoiseRatioDecibels =
          10.0 * std::log10(signalToNoiseRatio);
    }
  }
  return result;
}

bool StoneSoupSensorModel::isAvailable() const {
  QMutexLocker lock(&_mutex);
  QJsonObject request;
  request.insert(QStringLiteral("operation"), QStringLiteral("health"));
  QByteArray responseBytes;
  if (!this->exchangeLocked(
          QJsonDocument(request).toJson(QJsonDocument::Compact),
          responseBytes,
          3000)) {
    return false;
  }
  const QJsonDocument response = QJsonDocument::fromJson(responseBytes);
  return response.isObject() &&
      response.object().value(QStringLiteral("ok")).toBool(false) &&
      response.object().value(QStringLiteral("provider")).toString() ==
          this->modelId();
}

QString StoneSoupSensorModel::errorString() const {
  QMutexLocker lock(&_mutex);
  return _errorString;
}

bool StoneSoupSensorModel::ensureStartedLocked() const {
  if (_process.state() == QProcess::Running) {
    return true;
  }

  const QString executable = _configuration.pythonExecutable.trimmed();
  const QString script = _configuration.serviceScript.trimmed();
  if (executable.isEmpty() || script.isEmpty() || !QFileInfo::exists(script)) {
    _errorString = QStringLiteral("Python executable or Stone Soup service script is missing");
    return false;
  }

  _stdoutBuffer.clear();
  _process.setProcessChannelMode(QProcess::SeparateChannels);
  _process.start(executable, {script, QStringLiteral("--stdio")});
  if (!_process.waitForStarted(1500)) {
    _errorString = _process.errorString();
    return false;
  }
  return true;
}

bool StoneSoupSensorModel::exchangeLocked(
    const QByteArray& request,
    QByteArray& response,
    int timeoutMilliseconds) const {
  if (!this->ensureStartedLocked()) {
    return false;
  }

  const int timeout = timeoutMilliseconds > 0
      ? timeoutMilliseconds
      : _configuration.requestTimeoutMilliseconds;

  QByteArray message = request;
  message.append('\n');
  if (_process.write(message) != message.size() ||
      !_process.waitForBytesWritten(timeout)) {
    _errorString = QStringLiteral("Stone Soup request could not be written");
    this->stopLocked();
    return false;
  }

  QElapsedTimer timer;
  timer.start();
  while (timer.elapsed() < timeout) {
    _stdoutBuffer.append(_process.readAllStandardOutput());
    const qsizetype newlineIndex = _stdoutBuffer.indexOf('\n');
    if (newlineIndex >= 0) {
      response = _stdoutBuffer.left(newlineIndex);
      _stdoutBuffer.remove(0, newlineIndex + 1);
      _errorString.clear();
      return true;
    }
    const int remaining = std::max(
        1,
        timeout -
            static_cast<int>(timer.elapsed()));
    if (!_process.waitForReadyRead(remaining) &&
        _process.state() != QProcess::Running) {
      break;
    }
  }

  const QString stderrText = QString::fromUtf8(
      _process.readAllStandardError()).trimmed();
  _errorString = stderrText.isEmpty()
      ? QStringLiteral("Stone Soup request timed out")
      : stderrText;
  this->stopLocked();
  return false;
}

void StoneSoupSensorModel::stopLocked() const {
  if (_process.state() == QProcess::NotRunning) {
    return;
  }
  _process.terminate();
  if (!_process.waitForFinished(250)) {
    _process.kill();
    _process.waitForFinished(250);
  }
}

} // namespace infrastructure::sensors
