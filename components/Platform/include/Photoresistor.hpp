#pragma once

#include "Adc.hpp"
#include "ITask.hpp"
#include <functional>

class Photoresistor: public ITask
{
    public:

    enum class BrightnessLevel {
        BRIGHTNESS_DARK = 0,
        BRIGHTNESS_VERY_DIM = 1,
        BRIGHTNESS_DIM = 2,
        BRIGHTNESS_MODERATE = 3,
        BRIGHTNESS_BRIGHT = 4,
        BRIGHTNESS_VERY_BRIGHT = 5,
        BRIGHTNESS_EXTREMELY_BRIGHT = 6
    };

    Photoresistor(Adc* adc);

    void Update();

    uint32_t CurrentResistance();
    BrightnessLevel CurrentBrightness();

    void RegisterBrightnessLevelChange(std::function<void(BrightnessLevel)>);
    private:
    uint32_t m_currentResistance;
    BrightnessLevel m_currentBrightness;
    std::function<void(BrightnessLevel)> m_callback;
    Adc* m_adc;
};