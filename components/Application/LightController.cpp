#include "LightController.hpp"
#include "esp_log.h"
namespace
{

}

LightController::LightController(PresenceSensor* presenceSensor, Photoresistor* photoresistor, Gpio* gpio, uint32_t lightEnabledTimeS):
m_presenceSensor(presenceSensor),
m_photoresistor(photoresistor),
m_gpio(gpio),
m_lightEnabledTimeS(lightEnabledTimeS),
m_movementLatch(false)
{
    m_presenceSensor->RegisterChangeCallback([this](PresenceSensor::SensorCurrentState currentPresenceState)
    {
        if(m_isDark == false)
            return;

        if((currentPresenceState.movementDetected == true) && (m_movementLatch == false))
        {
            SetLightState(true);
            m_movementLatch = true;
        }

    });

    m_photoresistor->RegisterBrightnessLevelChange([this](Photoresistor::BrightnessLevel brightness)
    {
        const uint32_t maxBrightessLevel = static_cast<uint32_t>(Photoresistor::BrightnessLevel::BRIGHTNESS_DIM);
        const uint32_t currentBrightessLevel = static_cast<uint32_t>(brightness);
        ESP_LOGI("LOG","Brightness change %d",currentBrightessLevel);
        if(currentBrightessLevel < maxBrightessLevel)
            m_isDark = true;
        else
            m_isDark = false;

    });
}

void LightController::SetLightState(bool state)
{
    if(state)
    {
        m_enabledCounterS = m_lightEnabledTimeS;
        m_gpio->Set(true);
    }
    else
    {
        m_enabledCounterS = 0;
        m_gpio->Set(false);
    }

    ESP_LOGI("LOG","Set Light %d", state);    
}

void LightController::Update()
{
    if(m_enabledCounterS)
    {
        m_enabledCounterS--;
        
        if(m_enabledCounterS == 0)
        {
            SetLightState(false);
            m_movementLatch = false;
        }
    }
}