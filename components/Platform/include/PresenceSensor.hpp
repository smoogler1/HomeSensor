#pragma once

#include "XiaoHumanPresenceSensor.hpp"
#include <functional>
#include "ITask.hpp"

class PresenceSensor: public ITask
{
    public:

    struct SensorConfig
    {
        uint32_t minPresenceDistanceCm;
        uint32_t minMovementDistanceCm;
        uint32_t maxPresenceDistanceCm;
        uint32_t maxMovementDistanceCm;
    };

    struct SensorCurrentState
    {
        bool presenceDetected;
        bool movementDetected;

        bool operator==(const SensorCurrentState& s)
        {
            return (presenceDetected == s.presenceDetected) && (movementDetected == s.movementDetected);
        } 
    };

    PresenceSensor(Xiao::XiaoHumanPresenceSensor* sensorImpl);

    void RegisterChangeCallback(std::function<void(SensorCurrentState)> callback);   
    void Update() override;  

    private:
    
    Xiao::XiaoHumanPresenceSensor* m_sensorImplementation;

    SensorConfig m_config = {0, 0, 1000, 1000};

    std::function<void(SensorCurrentState)> m_detectionCallback;
    SensorCurrentState m_currentState;

};