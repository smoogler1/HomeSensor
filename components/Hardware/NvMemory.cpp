#include "NvMemory.hpp"
#include "nvs_flash.h"
#include "esp_log.h"

static const char* TAG = "NVMEM";

NvMemory::NvMemory()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

bool NvMemory::Save(const char* key,  void* data, uint32_t size)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("nv_mem", NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open %s failed: %s",key, esp_err_to_name(err));
        return false;
    }

    err = nvs_set_blob(nvs, key, data, size);
    if (err == ESP_OK) {
        err = nvs_commit(nvs);
    }
    nvs_close(nvs);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save : key: %s err:%s",key, esp_err_to_name(err));
        return false;
    }

    return (err == ESP_OK);
}

bool NvMemory::Load(const char* key,  void* buffer, uint32_t size)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("nv_mem", NVS_READONLY, &nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open %s failed: %s",key, esp_err_to_name(err));
        return false;
    }

    err = nvs_get_blob(nvs, key, buffer, reinterpret_cast<size_t*>(&size));
    nvs_close(nvs);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "No data found for %s", key);
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_get_blob failed: %s", esp_err_to_name(err));
    }

    return (err == ESP_OK);
}