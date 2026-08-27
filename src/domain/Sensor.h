#pragma once

#include <QString>
#include <QVector>

#include <limits>

enum class SensorType {
  Radar,
  Infrared,
  Visual,
  Unknown,
};

enum class SensorSubType {
  Generic,
  AirborneRadar,
  GroundRadar,
  NavalRadar,
  Unknown,
};

struct RadarProfile {
  QString profileId = QStringLiteral("generic");
  double peakPowerWatts = 25000.0;
  double dutyCycle = 0.1;
  double bandwidthHertz = 1.0e6;
  double receiverNoiseDecibels = 3.0;
  double frequencyHertz = 10.0e9;
  double antennaGainDecibels = 35.0;
  double beamWidthDegrees = 120.0;
  int numberPulses = 16;
  double systemLossDecibels = 6.0;
  double probabilityFalseAlarm = 1.0e-6;
  double rcsScaleSquareMeters = 1.0;
};

struct SensorDefinition {
  QString id;
  QString name;
  QString modelProviderId = QStringLiteral("native");
  QString sensorType = QStringLiteral("radar");
  QString sensorSubType = QStringLiteral("generic");
  bool enabled = true;
  bool emitting = true;
  double maxRangeMeters = 250000.0;
  double minRangeMeters = 0.0;
  double azimuthCenterDegrees = 0.0;
  double azimuthWidthDegrees = 360.0;
  double elevationCenterDegrees = 0.0;
  double elevationWidthDegrees = 90.0;
  double updatePeriodSeconds = 5.0;
  int maxTracks = 32;
  bool iffCapable = true;
  bool canDetectAir = true;
  bool canDetectGround = false;
  bool canDetectSurface = false;
  bool terrainMaskingEnabled = false;
  double probabilityOfDetection = 1.0;
  double trackHoldSeconds = 10.0;
  RadarProfile radarProfile;
};

struct SensorEvaluationDiagnostics {
  QString requestedModelProviderId = QStringLiteral("native");
  QString effectiveModelProviderId = QStringLiteral("native");
  QString providerVersion;
  bool fallbackUsed = false;
  QString fallbackReason;
  double detectionProbability = 0.0;
  double deterministicSample = 0.0;
  double targetSignature = 1.0;
  double signalToNoiseRatio = std::numeric_limits<double>::quiet_NaN();
  double signalToNoiseRatioDecibels =
      std::numeric_limits<double>::quiet_NaN();
  double rangeLossDecibels = std::numeric_limits<double>::quiet_NaN();
  double echoRatio = std::numeric_limits<double>::quiet_NaN();
  double receivedPowerWatts = std::numeric_limits<double>::quiet_NaN();
  double noisePowerWatts = std::numeric_limits<double>::quiet_NaN();
  double evaluationDurationMilliseconds = 0.0;
};

struct SensorContact {
  QString sensorId;
  QString sensorModelProviderId = QStringLiteral("native");
  QString sensorType;
  QString sensorSubType;
  QString targetEntityId;
  QString targetEntityName;
  double rangeMeters = 0.0;
  double bearingDegrees = 0.0;
  bool lineOfSight = true;
  bool detected = false;
  double confidence = 0.0;
  double lastSeenSimulationSeconds = 0.0;
  QString trackState = QStringLiteral("Detected");
  qint64 lastEvaluationIndex = -1;
  int missedDetectionCount = 0;
  SensorEvaluationDiagnostics evaluation;
};

struct SensorRuntimeStatus {
  QString sensorId;
  QString lastTargetEntityId;
  QString lastTargetEntityName;
  double lastEvaluationSimulationSeconds = 0.0;
  qint64 lastEvaluationIndex = -1;
  quint64 evaluationCount = 0;
  quint64 detectionCount = 0;
  SensorEvaluationDiagnostics evaluation;
};

using SensorDefinitions = QVector<SensorDefinition>;
using SensorContacts = QVector<SensorContact>;
using SensorRuntimeStatuses = QVector<SensorRuntimeStatus>;

namespace sensor {

SensorType typeFromString(const QString& value);
QString typeToString(SensorType type);

SensorSubType subTypeFromString(const QString& value);
QString subTypeToString(SensorSubType subType);

bool canDetectDomain(const SensorDefinition& definition, const QString& domain);
bool canOperateFromDomain(const SensorDefinition& definition, const QString& domain);
bool supportsTargetDomain(const SensorDefinition& definition, const QString& domain);

} // namespace sensor
