#pragma once

#include <cstdint>

class Gpio
{
public:
    using GpioPin = uint32_t;

    enum DirectionMode
    {
        INPUT,
        OUTPUT
    };

    enum PullMode
    {
        NO_PULL,
        PULL_DOWN,
        PULL_UP
    };

    Gpio(GpioPin pin, DirectionMode direction, PullMode pull);

    void Set(bool value);
    bool Get();

private:
    GpioPin m_pin;
};