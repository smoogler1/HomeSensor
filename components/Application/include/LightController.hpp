#pragma once
#include "Photoresistor.hpp"
#include "PresenceSensor.hpp"
#include "ITask.hpp"
#include "Gpio.hpp"
class LightController: public ITask
{
    public:
    LightController(PresenceSensor* presenceSensor, Photoresistor* photoresistor, Gpio* gpio, uint32_t lightEnabledTimeS);

    void Update() override;
    bool GetLightState();

    void DisableController();
    void EnableController();
    private:

    void SetLightState(bool state);

    PresenceSensor* m_presenceSensor;
    Photoresistor* m_photoresistor;
    Gpio* m_gpio;
    
    bool m_isDark = false;
    bool m_lightEnabled = false;
    bool m_overrideFlag = false;
    const uint32_t m_lightEnabledTimeS;
    uint32_t m_enabledCounterS;

    bool m_movementLatch;
};