#pragma once

#include "ITask.hpp"

class ClimateSensor: public ITask
{
    public:

    ClimateSensor(int gpioPin);
    void Update();

    float GetTemperature();
    float GetHumidity();

    private:

    float m_temperature;
    float m_humidity;

    int m_gpioPin;
};