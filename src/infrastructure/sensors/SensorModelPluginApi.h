#pragma once

#include <stdint.h>

// Stable C ABI used by optional in-process sensor model adapters. Keep this
// boundary free of Qt and C++ standard-library types.
#define QTTEST_SENSOR_MODEL_PLUGIN_ABI_VERSION 1U

#define QTTEST_SENSOR_DIAGNOSTIC_SNR              (1U << 0U)
#define QTTEST_SENSOR_DIAGNOSTIC_SNR_DB           (1U << 1U)
#define QTTEST_SENSOR_DIAGNOSTIC_RANGE_LOSS_DB    (1U << 2U)
#define QTTEST_SENSOR_DIAGNOSTIC_ECHO_RATIO       (1U << 3U)
#define QTTEST_SENSOR_DIAGNOSTIC_RECEIVED_POWER_W (1U << 4U)
#define QTTEST_SENSOR_DIAGNOSTIC_NOISE_POWER_W    (1U << 5U)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QttestSensorEvaluationInputV1 {
  uint32_t structSize;
  uint32_t scenarioSeed;
  int64_t evaluationIndex;
  double simulationTimeSeconds;
  double rangeMeters;
  double configuredProbability;
  double targetSignature;
  double observerLatitudeDegrees;
  double observerLongitudeDegrees;
  double observerAltitudeMeters;
  double observerHeadingDegrees;
  double observerSpeedKnots;
  double targetLatitudeDegrees;
  double targetLongitudeDegrees;
  double targetAltitudeMeters;
  double targetHeadingDegrees;
  double targetSpeedKnots;
  // Append-only ABI extension. Plugins compiled against the original V1
  // layout can ignore these fields by checking structSize.
  double sensorMinRangeMeters;
  double sensorMaxRangeMeters;
  double radarPeakPowerWatts;
  double radarDutyCycle;
  double radarBandwidthHertz;
  double radarReceiverNoiseDecibels;
  double radarFrequencyHertz;
  double radarAntennaGainDecibels;
  double radarBeamWidthDegrees;
  uint32_t radarNumberPulses;
  double radarSystemLossDecibels;
  double radarProbabilityFalseAlarm;
  double radarRcsScaleSquareMeters;
} QttestSensorEvaluationInputV1;

typedef struct QttestSensorEvaluationOutputV1 {
  uint32_t structSize;
  int32_t status;
  double probability;
  // Append-only diagnostics. A zero mask means the provider only supports
  // the original V1 probability output.
  uint32_t diagnosticsMask;
  double signalToNoiseRatio;
  double signalToNoiseRatioDecibels;
  double rangeLossDecibels;
  double echoRatio;
  double receivedPowerWatts;
  double noisePowerWatts;
} QttestSensorEvaluationOutputV1;

typedef uint32_t (*QttestSensorPluginAbiVersionFn)(void);
typedef const char* (*QttestSensorPluginModelIdFn)(void);
typedef const char* (*QttestSensorPluginVersionFn)(void);
typedef int (*QttestSensorPluginEvaluateFn)(
    const QttestSensorEvaluationInputV1*,
    QttestSensorEvaluationOutputV1*);

#ifdef __cplusplus
} // extern "C"
#endif
