#ifndef VS_HOST_RUNTIME_H
#define VS_HOST_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

typedef struct VsHostRuntime VsHostRuntime;

typedef struct VsHostRuntimeConfig {
    const char* asset_root;
    int argc;
    char** argv;
} VsHostRuntimeConfig;

typedef enum VsHostResult {
    VS_HOST_OK = 0,
    VS_HOST_ERROR = -1,
} VsHostResult;

VsHostResult vs_host_runtime_init(VsHostRuntime* runtime, const VsHostRuntimeConfig* config);
void vs_host_runtime_tick(VsHostRuntime* runtime);
void vs_host_runtime_shutdown(VsHostRuntime* runtime);

#endif
