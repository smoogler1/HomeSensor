#include "PresenceSensor.hpp"

#include "XiaoHumanPresenceSensor.hpp"
#include <functional>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

PresenceSensor::PresenceSensor(Xiao::XiaoHumanPresenceSensor *sensorImplemention) : m_sensorImplementation(sensorImplemention)
{

}

void PresenceSensor::RegisterChangeCallback(std::function<void(SensorCurrentState)> callback)
{
    m_detectionCallback = callback;
}

void PresenceSensor::Update()
{
    m_sensorImplementation->Update();

    auto targetData = m_sensorImplementation->GetTargetData();
    auto previousState = m_currentState;

    if ((targetData.targetStatus == Xiao::TargetStatusType::StationaryTarget) || (targetData.targetStatus == Xiao::TargetStatusType::CampaignStationaryTarget))
    {
        if ((targetData.distanceToStationaryTargetCm >= m_config.minPresenceDistanceCm) && (targetData.distanceToStationaryTargetCm <= m_config.maxPresenceDistanceCm))
        {
            m_currentState.presenceDetected = true;
        }
        else
        {
            m_currentState.presenceDetected = false;
        }
    }
    else
    {
        m_currentState.presenceDetected = false;
    }

    if ((targetData.targetStatus == Xiao::TargetStatusType::CampaignTarget) || (targetData.targetStatus == Xiao::TargetStatusType::CampaignStationaryTarget))
    {
        
        if ((targetData.movementTargetDistanceCm >= m_config.minMovementDistanceCm) && (targetData.movementTargetDistanceCm <= m_config.maxMovementDistanceCm))
        {
            m_currentState.movementDetected = true;
        }
        else
        {
            m_currentState.movementDetected = false;
        }
    }
    else
    {
        m_currentState.movementDetected = false;
    }

    if (previousState != m_currentState)
    {
        ESP_LOGI("LOG", "Report:  \r\n "
            "detected distance %d, \r\n "
            "stationary distance %d, \r\n "
            "excercise energy %d, \r\n "
            "movement target discance %d, \r\n "
            "stationary energy %d \r\n"
            "target status %d",
        targetData.detectedDistanceCm,
        targetData.distanceToStationaryTargetCm,
        targetData.excerciseTargetEnergyValue,
        targetData.movementTargetDistanceCm,
        targetData.stationaryTargetEnergyValue,
        targetData.targetStatus);

        m_detectionCallback(m_currentState);
    }
}
