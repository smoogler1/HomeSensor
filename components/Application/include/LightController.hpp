#pragma once
#include "Photoresistor.hpp"
#include "PresenceSensor.hpp"
#include "ITask.hpp"
class LightController: public ITask
{
    public:
    LightController(PresenceSensor* presenceSensor, Photoresistor* photoresistor, uint32_t lightEnabledTimeS);

    void Update();
    private:

    void SetLightState(bool state);

    PresenceSensor* m_presenceSensor;
    Photoresistor* m_photoresistor;

    bool m_isDark;
    bool m_lightEnabled;
    const uint32_t m_lightEnabledTimeS;
    uint32_t m_enabledCounterS;

    bool m_movementLatch;
};