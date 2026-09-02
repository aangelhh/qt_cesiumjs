#pragma once

#include <stddef.h>
#include <stdint.h>

#define QTTEST_HLA_BACKEND_PLUGIN_ABI_VERSION 2U

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

typedef struct QttestHlaBackendApiV2 {
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
  int (*sendInteraction)(
      QttestHlaBackendHandle,
      const char* interactionClassName,
      const QttestHlaNamedValueArrayV2* parameters,
      const QttestHlaByteSpanV2* tag);
  int (*poll)(QttestHlaBackendHandle, double maximumSeconds);
  int (*resign)(QttestHlaBackendHandle);
  int (*disconnect)(QttestHlaBackendHandle);
  QttestHlaBackendStateV1 (*state)(QttestHlaBackendHandle);
  const char* (*lastError)(QttestHlaBackendHandle);
} QttestHlaBackendApiV2;

typedef const QttestHlaBackendApiV2* (*QttestHlaBackendApiFn)(void);

QTTEST_HLA_PLUGIN_EXPORT const QttestHlaBackendApiV2*
qttest_hla_backend_api_v2(void);

#ifdef __cplusplus
} // extern "C"
#endif
