#pragma once

#include <stddef.h>
#include <stdint.h>

#define QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION 3U

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

typedef struct QttestHlaCallbacksV3 {
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
      const QttestHlaByteSpanV2* tag);
  void (*objectRemoved)(
      void* context,
      uint64_t instanceId,
      const QttestHlaByteSpanV2* tag);
  void (*interactionReceived)(
      void* context,
      const char* interactionClassName,
      const QttestHlaNamedValueArrayV2* parameters,
      const QttestHlaByteSpanV2* tag);
} QttestHlaCallbacksV3;

typedef struct QttestHlaBackendApiV3 {
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
  int (*deleteObjectInstance)(
      QttestHlaBackendHandle,
      uint64_t instanceId,
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
  int (*setCallbacks)(
      QttestHlaBackendHandle,
      const QttestHlaCallbacksV3* callbacks);
  int (*poll)(QttestHlaBackendHandle, double maximumSeconds);
  int (*resign)(QttestHlaBackendHandle);
  int (*disconnect)(QttestHlaBackendHandle);
  QttestHlaBackendStateV1 (*state)(QttestHlaBackendHandle);
  const char* (*lastError)(QttestHlaBackendHandle);
} QttestHlaBackendApiV3;

typedef const QttestHlaBackendApiV3* (*QttestHlaBackendApiFn)(void);

QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV3*
qttest_hla_backend_api_v3(void);

#ifdef __cplusplus
} // extern "C"
#endif
