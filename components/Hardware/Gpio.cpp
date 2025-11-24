#include "Gpio.hpp"
#include "driver/gpio.h"

Gpio::Gpio(GpioPin pin, DirectionMode direction,PullMode pull ):
m_pin(pin)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = (direction == DirectionMode::OUTPUT) ? GPIO_MODE_OUTPUT: GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1<<pin);

    if(pull == PullMode::PULL_DOWN)
    {
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else if(pull == PullMode::PULL_UP)
    {
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    }
    else
    {
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    }

    //configure GPIO with the given settings
    gpio_config(&io_conf);

    Set(0);
}

void Gpio::Set(bool value)
{
    gpio_set_level(static_cast<gpio_num_t>(m_pin), value);
}

bool Gpio::Get()
{
    return gpio_get_level(static_cast<gpio_num_t>(m_pin));
}