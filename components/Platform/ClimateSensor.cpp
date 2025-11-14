#include "ClimateSensor.hpp"
#include "dht.h"
#include "esp_log.h"
namespace
{
    dht_sensor_type_t sensorType = dht_sensor_type_t::DHT_TYPE_DHT11;
    constexpr uint32_t READ_SENSOR_PERIOD_S = 3;
}

ClimateSensor::ClimateSensor(int gpioPin):
m_gpioPin(gpioPin)
{

}

void ClimateSensor::Update()
{
    static int countSeconds = READ_SENSOR_PERIOD_S;

    if(countSeconds == 0)
    {
        dht_read_float_data(sensorType,static_cast<gpio_num_t>(m_gpioPin),&m_humidity,&m_temperature);
        countSeconds = READ_SENSOR_PERIOD_S;
    }

    ESP_LOGI("LOG","humidity %f, temperature: %f", m_humidity, m_temperature);
    
    countSeconds--;
}

float ClimateSensor::GetTemperature()
{
    return m_temperature;
}

float ClimateSensor::GetHumidity()
{
    return m_humidity;
}