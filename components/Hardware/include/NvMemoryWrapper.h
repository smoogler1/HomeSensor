#ifndef NV_MEMORY_WRAPPER
#define NV_MEMORY_WRAPPER

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

bool nv_memory_save(const char* key,  void* data, uint32_t size);
bool nv_memory_load(const char* key,  void* buffer, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif