#include "Adc.hpp"
#include "esp_log.h"
namespace
{
    constexpr uint32_t ADC_REFERENCE_VOLTAGE_MV = 2500;
    constexpr uint32_t ADC_RESOLUTION_BIT = (1<<12);

}
Adc::Adc(uint32_t unit, uint32_t channel, Adc::Dividers dividers):
m_multiplyFactor(dividers.groundDividerResitor / (dividers.groundDividerResitor + dividers.supplyDividerResistor))
{
    adc_unit_t unit_id = static_cast<adc_unit_t>(unit);

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = unit_id,
    };

    adc_oneshot_new_unit(&init_config1, &m_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    adc_channel_t channel_id = static_cast<adc_channel_t>(channel);

    adc_oneshot_config_channel(m_handle, channel_id, &config);
    
};

void Adc::Update()
{
    adc_oneshot_read(m_handle, ADC_CHANNEL_2, &m_currentRawValue);
}

int Adc::GetCurrentRawValue()
{
    return m_currentRawValue;
}

int Adc::GetCurrentVoltageMv()
{
    ESP_LOGI("LOG","adc raw %d", (int)m_currentRawValue);

    return (m_currentRawValue * m_multiplyFactor * ADC_REFERENCE_VOLTAGE_MV)/ADC_RESOLUTION_BIT;
}