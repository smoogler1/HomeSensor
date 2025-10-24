#pragma once

#include <cstdint>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "ITask.hpp"

class Adc: public ITask
{
    public:

    struct Dividers
    {
        uint32_t supplyDividerResistor;
        uint32_t groundDividerResitor;
    };

    Adc(uint32_t unit, uint32_t channel, Dividers dividers = {0,1});

    void Update();

    int GetCurrentRawValue();
    int GetCurrentVoltageMv();

    private:

    int m_currentRawValue;
    const float m_multiplyFactor;
    adc_oneshot_unit_handle_t m_handle;
};