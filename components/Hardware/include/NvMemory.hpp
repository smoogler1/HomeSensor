#pragma once
#include <cstdint>

class NvMemory
{
    public:
    NvMemory();
    bool Save(const char* key,  void* data, uint32_t size);
    bool Load(const char* key,  void* buffer, uint32_t size);
};