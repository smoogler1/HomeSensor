#include "OtaUpdate.hpp"

bool OtaUpdate::StartUpdate()
{
    auto partition = esp_ota_get_next_update_partition(NULL);

    esp_err_t result = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &m_otaHandle);

    if (result != ESP_OK)
        return false;

    m_updateStarted = true;
    m_partition = partition;

    return true;
}

bool OtaUpdate::WriteFirmwarePart(uint8_t *buffer, uint32_t partSize)
{
    if (m_updateStarted == false)
        return false;

    return esp_ota_write(m_otaHandle, (const void *)buffer, partSize) == ESP_OK;
}

bool OtaUpdate::FinalizeUpdate()
{
    if (m_updateStarted == false)
        return false;

    if (esp_ota_end(m_otaHandle) != ESP_OK)
        return false;

    if (esp_ota_set_boot_partition(m_partition) != ESP_OK)
        return false;

    return true;
}