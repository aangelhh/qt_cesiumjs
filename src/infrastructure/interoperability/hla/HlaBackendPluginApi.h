#pragma once

#include <stddef.h>
#include <stdint.h>

#define QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION 9U

#ifdef _WIN32
#define QTTEST_HLA_PLUGIN_EXPORT __declspec(dllexport)
#else
#define QTTEST_HLA_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* QttestHlaBackendHandle;

typedef enum QttestHlaBackendStateV1 {
  QTTEST_HLA_STATE_UNAVAILABLE = 0,
  QTTEST_HLA_STATE_DISCONNECTED = 1,
  QTTEST_HLA_STATE_CONNECTED = 2,
  QTTEST_HLA_STATE_JOINED = 3,
  QTTEST_HLA_STATE_ERROR = 4
} QttestHlaBackendStateV1;

typedef struct QttestHlaStringArrayV1 {
  uint32_t structSize;
  const char* const* values;
  size_t count;
} QttestHlaStringArrayV1;

typedef struct QttestHlaByteSpanV2 {
  uint32_t structSize;
  const uint8_t* data;
  size_t size;
} QttestHlaByteSpanV2;

typedef struct QttestHlaNamedValueV2 {
  uint32_t structSize;
  const char* name;
  QttestHlaByteSpanV2 value;
} QttestHlaNamedValueV2;

typedef struct QttestHlaNamedValueArrayV2 {
  uint32_t structSize;
  const QttestHlaNamedValueV2* values;
  size_t count;
} QttestHlaNamedValueArrayV2;

typedef enum QttestHlaReceiveOrderV7 {
  QTTEST_HLA_RECEIVE_ORDER = 0,
  QTTEST_HLA_TIMESTAMP_ORDER = 1
} QttestHlaReceiveOrderV7;

typedef struct QttestHlaReceiveInfoV7 {
  uint32_t structSize;
  QttestHlaReceiveOrderV7 order;
  int hasLogicalTime;
  double logicalTimeSeconds;
} QttestHlaReceiveInfoV7;

typedef struct QttestHlaCallbacksV9 {
  uint32_t structSize;
  void* context;
  void (*objectDiscovered)(
      void* context,
      uint64_t instanceId,
      const char* objectClassName,
      const char* instanceName);
  void (*objectReflected)(
      void* context,
      uint64_t instanceId,
      const QttestHlaNamedValueArrayV2* attributes,
      const QttestHlaByteSpanV2* tag,
      const QttestHlaReceiveInfoV7* receiveInfo);
  void (*objectRemoved)(
      void* context,
      uint64_t instanceId,
      const QttestHlaByteSpanV2* tag,
      const QttestHlaReceiveInfoV7* receiveInfo);
  void (*interactionReceived)(
      void* context,
      const char* interactionClassName,
      const QttestHlaNamedValueArrayV2* parameters,
      const QttestHlaByteSpanV2* tag,
      const QttestHlaReceiveInfoV7* receiveInfo);
  void (*synchronizationPointRegistrationResult)(
      void* context,
      const char* label,
      int succeeded,
      const char* reason);
  void (*synchronizationPointAnnounced)(
      void* context,
      const char* label,
      const QttestHlaByteSpanV2* tag);
  void (*federationSynchronized)(void* context, const char* label);
  void (*timeRegulationEnabled)(void* context, double logicalTimeSeconds);
  void (*timeConstrainedEnabled)(void* context, double logicalTimeSeconds);
  void (*timeAdvanceGranted)(void* context, double logicalTimeSeconds);
  void (*attributeOwnershipAcquired)(
      void* context,
      uint64_t instanceId,
      const QttestHlaStringArrayV1* attributeNames,
      const QttestHlaByteSpanV2* tag);
  void (*attributeOwnershipUnavailable)(
      void* context,
      uint64_t instanceId,
      const QttestHlaStringArrayV1* attributeNames);
  void (*attributeOwnershipReleaseRequested)(
      void* context,
      uint64_t instanceId,
      const QttestHlaStringArrayV1* attributeNames,
      const QttestHlaByteSpanV2* tag);
} QttestHlaCallbacksV9;

typedef struct QttestHlaBackendApiV9 {
  uint32_t structSize;
  uint32_t abiVersion;
  const char* backendId;
  const char* backendVersion;
  const char* capabilities;

  QttestHlaBackendHandle (*create)(void);
  void (*destroy)(QttestHlaBackendHandle);
  int (*connect)(QttestHlaBackendHandle, const char* localSettingsDesignator);
  int (*createFederation)(
      QttestHlaBackendHandle,
      const char* federationName,
      const QttestHlaStringArrayV1* fomModules);
  int (*joinFederation)(
      QttestHlaBackendHandle,
      const char* federateName,
      const char* federateType,
      const char* federationName,
      const QttestHlaStringArrayV1* additionalFomModules);
  int (*publishObjectClass)(
      QttestHlaBackendHandle,
      const char* objectClassName,
      const QttestHlaStringArrayV1* attributeNames);
  int (*subscribeObjectClass)(
      QttestHlaBackendHandle,
      const char* objectClassName,
      const QttestHlaStringArrayV1* attributeNames);
  int (*registerObjectInstance)(
      QttestHlaBackendHandle,
      const char* objectClassName,
      const char* instanceName,
      uint64_t* instanceId);
  int (*updateObjectAttributes)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
      const QttestHlaNamedValueArrayV2* attributes,
      const QttestHlaByteSpanV2* tag);
  int (*updateObjectAttributesAtTime)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
      const QttestHlaNamedValueArrayV2* attributes,
      double logicalTimeSeconds,
      const QttestHlaByteSpanV2* tag);
  int (*deleteObjectInstance)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
      const QttestHlaByteSpanV2* tag);
  int (*deleteObjectInstanceAtTime)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
      double logicalTimeSeconds,
      const QttestHlaByteSpanV2* tag);
  int (*publishInteractionClass)(
      QttestHlaBackendHandle,
      const char* interactionClassName);
  int (*subscribeInteractionClass)(
      QttestHlaBackendHandle,
      const char* interactionClassName,
      const QttestHlaStringArrayV1* parameterNames);
  int (*sendInteraction)(
      QttestHlaBackendHandle,
      const char* interactionClassName,
      const QttestHlaNamedValueArrayV2* parameters,
      const QttestHlaByteSpanV2* tag);
  int (*sendInteractionAtTime)(
      QttestHlaBackendHandle,
      const char* interactionClassName,
      const QttestHlaNamedValueArrayV2* parameters,
      double logicalTimeSeconds,
      const QttestHlaByteSpanV2* tag);
  int (*registerSynchronizationPoint)(
      QttestHlaBackendHandle,
      const char* label,
      const QttestHlaByteSpanV2* tag);
  int (*achieveSynchronizationPoint)(
      QttestHlaBackendHandle,
      const char* label);
  int (*enableTimeRegulation)(
      QttestHlaBackendHandle,
      double lookaheadSeconds);
  int (*enableTimeConstrained)(QttestHlaBackendHandle);
  int (*requestTimeAdvance)(
      QttestHlaBackendHandle,
      double logicalTimeSeconds);
  int (*requestAttributeOwnershipAcquisition)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
      const QttestHlaStringArrayV1* attributeNames,
      const QttestHlaByteSpanV2* tag);
  int (*unconditionalAttributeOwnershipDivestiture)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
      const QttestHlaStringArrayV1* attributeNames);
  int (*setCallbacks)(
      QttestHlaBackendHandle,
      const QttestHlaCallbacksV9* callbacks);
  int (*poll)(QttestHlaBackendHandle, double maximumSeconds);
  int (*resign)(QttestHlaBackendHandle);
  int (*disconnect)(QttestHlaBackendHandle);
  QttestHlaBackendStateV1 (*state)(QttestHlaBackendHandle);
  const char* (*lastError)(QttestHlaBackendHandle);
} QttestHlaBackendApiV9;

typedef const QttestHlaBackendApiV9* (*QttestHlaBackendApiFn)(void);

QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV9*
qttest_hla_backend_api_v9(void);

#ifdef __cplusplus
} // extern "C"
#endif
