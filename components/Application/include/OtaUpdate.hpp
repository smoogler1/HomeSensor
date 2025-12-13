#pragma once

#include <cstdint>
#include <esp_ota_ops.h>

class OtaUpdate
{
    public:

    bool StartUpdate();
    bool WriteFirmwarePart(uint8_t* buffer, uint32_t partSize);
    bool FinalizeUpdate();

    private:
    esp_ota_handle_t m_otaHandle;
    const esp_partition_t * m_partition;
    bool m_updateStarted;
};