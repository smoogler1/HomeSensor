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

extern "C"
{
    void app_main(void);
}

DispatcherTask dispatcher;

void update(PresenceSensor::SensorCurrentState state)
{
    ESP_LOGI("LOG",
    "presence detected: %d, movement detected: %d", state.presenceDetected, state.movementDetected);
}

void init()
{
    Uart* uart = new Uart(1, 256000, GPIO_NUM_21, GPIO_NUM_20);
    PresenceSensor* sensor = new PresenceSensor(new Xiao::XiaoHumanPresenceSensor(uart));
    sensor->RegisterChangeCallback(update);

    Adc* adc = new Adc(ADC_UNIT_1, ADC_CHANNEL_2);

    Photoresistor* photoresistor = new Photoresistor(adc);

    dispatcher.RegisterTask(sensor,DispatcherTask::TaskFrequency::TASK_FREQ_100MS);
    dispatcher.RegisterTask(adc,DispatcherTask::TaskFrequency::TASK_FREQ_100MS);
    dispatcher.RegisterTask(photoresistor,DispatcherTask::TaskFrequency::TASK_FREQ_1S);
}

void app_main(void)
{
    init();
    while(1)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}