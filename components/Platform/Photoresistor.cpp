#include "Photoresistor.hpp"
#include <map>
#include "esp_log.h"
namespace
{
    using BrightnessLevel = Photoresistor::BrightnessLevel;

    constexpr uint32_t DIVIDER_RESISTOR = 210000;
    constexpr float DIVIDER_SUPPLY_VOLTAGE_MV = 3300;

    std::map<uint32_t, Photoresistor::BrightnessLevel> RESISTANCE_TO_BRIGTHNESS_MAP = 
    {
         {400000,BrightnessLevel::BRIGHTNESS_DARK},
         {300000,BrightnessLevel::BRIGHTNESS_VERY_DIM},
         {200000,BrightnessLevel::BRIGHTNESS_DIM},
         {90000,BrightnessLevel::BRIGHTNESS_MODERATE},
         {20000,BrightnessLevel::BRIGHTNESS_BRIGHT},
         {7000,BrightnessLevel::BRIGHTNESS_VERY_BRIGHT},
         {5000,BrightnessLevel::BRIGHTNESS_EXTREMELY_BRIGHT}
    };

    inline uint32_t CalculateResistance(const int voltageMv)
    {
        if(voltageMv >= DIVIDER_SUPPLY_VOLTAGE_MV)
            return 1000000;

        return static_cast<uint32_t>(voltageMv * DIVIDER_RESISTOR)/(DIVIDER_SUPPLY_VOLTAGE_MV - voltageMv);
    }
}

Photoresistor::Photoresistor(Adc* adc): m_adc(adc)
{
    
}

void Photoresistor::Update()
{
    auto adcVoltageMv = m_adc->GetCurrentVoltageMv();
    m_currentResistance = CalculateResistance(adcVoltageMv);

    uint32_t previousResistance = 0;
    auto previousBrightness = m_currentBrightness;

    for(const auto& b: RESISTANCE_TO_BRIGTHNESS_MAP)
    {
        if(m_currentResistance > previousResistance && m_currentResistance<=b.first)
            m_currentBrightness = b.second;

        previousResistance = b.first;
    }

    ESP_LOGI("LOG","brightness %d, resistance: %d adc %d", (int)m_currentBrightness,(int)m_currentResistance,adcVoltageMv);
}

uint32_t Photoresistor::CurrentResistance()
{
    return m_currentResistance;
}

Photoresistor::BrightnessLevel Photoresistor::CurrentBrightness()
{
    return m_currentBrightness;
}

void Photoresistor::RegisterBrightnessLevelChange(std::function<void(BrightnessLevel)> callback)
{
    m_callback = callback;
}