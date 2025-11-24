#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "XiaoHumanPresenceSensor.hpp"
#include "Uart.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "PresenceSensor.hpp"
#include "Adc.hpp"
#include "DispatcherTask.hpp"
#include "Photoresistor.hpp"
#include "ClimateSensor.hpp"
#include "LightController.hpp"
#include "Gpio.hpp"
#include "http_serv.h"

/// to check: https://github.com/Jeija/esp32-softap-ota/blob/master/main/main.c
extern "C"
{
    void app_main(void);
}

DispatcherTask dispatcher;

void init()
{

    ESP_LOGI("LOG", "!!! Application Initialization !!!");

    Uart* uart = new Uart(1, 256000, GPIO_NUM_21, GPIO_NUM_20);
    PresenceSensor* presenceSensor = new PresenceSensor(new Xiao::XiaoHumanPresenceSensor(uart));

    Adc* adc = new Adc(ADC_UNIT_1, ADC_CHANNEL_3);

    Photoresistor* photoresistor = new Photoresistor(adc);

    ClimateSensor* climateSensor = new ClimateSensor(GPIO_NUM_2);

    Gpio* lightOutputGpio = new Gpio(GPIO_NUM_9,Gpio::DirectionMode::OUTPUT,Gpio::PullMode::NO_PULL);
    LightController* lightController = new LightController(presenceSensor, photoresistor,lightOutputGpio, 10);

    dispatcher.RegisterTask(presenceSensor,DispatcherTask::TaskFrequency::TASK_FREQ_100MS);
    dispatcher.RegisterTask(adc,DispatcherTask::TaskFrequency::TASK_FREQ_100MS);
    dispatcher.RegisterTask(photoresistor,DispatcherTask::TaskFrequency::TASK_FREQ_1S);
    dispatcher.RegisterTask(climateSensor,DispatcherTask::TaskFrequency::TASK_FREQ_1S);
    dispatcher.RegisterTask(lightController,DispatcherTask::TaskFrequency::TASK_FREQ_1S);
}


void app_main(void)
{
    http_server_init();
    init();
    while(1)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}