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
#include "NvMemorySetter.hpp"
#include "network_wifi.h"
#include "http_server.h"
/// to check: https://github.com/Jeija/esp32-softap-ota/blob/master/main/main.c
extern "C"
{
    void app_main(void);
}

DispatcherTask dispatcher;

void app_init()
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

    http_set_climate_sensor(climateSensor);
    http_set_photoresistor(photoresistor);
    http_set_light_controller(lightController);
    
    NvMemory* memoryDriver = new NvMemory();
    nv_memory_set_driver(memoryDriver);

    network_wifi_init();
    http_server_init();
}


void app_main(void)
{
    app_init();
    
    while(1)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}