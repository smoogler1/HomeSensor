#include "NvMemory.hpp"
#include "NvMemoryWrapper.h"

namespace
{
    NvMemory* nv_memory;
}

void nv_memory_set_driver(NvMemory* driver)
{
    nv_memory = driver;
}

bool nv_memory_save(const char* key,  void* data, uint32_t size)
{
    return nv_memory->Save(key,data,size);
}

bool nv_memory_load(const char* key,  void* buffer, uint32_t size)
{
    return nv_memory->Load(key,buffer,size);
}