#pragma once

#include "Uart.hpp"
#include <memory>


/// Config : Moving 96, stationary 60  


namespace Xiao
{
    using CommandWordType = std::array<uint8_t,2>;

    enum class TargetStatusType: uint8_t
    {
        NoTarget = 0x00,
        CampaignTarget = 0x01,
        StationaryTarget = 0x02,
        CampaignStationaryTarget = 0x03
    };

    #pragma pack(1)
    struct BasicTargetData
    {
        TargetStatusType targetStatus;
        uint16_t movementTargetDistanceCm;
        uint8_t excerciseTargetEnergyValue;
        uint16_t distanceToStationaryTargetCm;
        uint8_t stationaryTargetEnergyValue;
        uint16_t detectedDistanceCm;
    };
    #pragma pack(pop)

    class XiaoHumanPresenceSensor
    {
        public:

        
        XiaoHumanPresenceSensor(Uart* uart);
        ~XiaoHumanPresenceSensor();

        void Configure();
        void Update();
        BasicTargetData GetTargetData();

        private:
        std::optional<std::vector<uint8_t>> SendCommandAndGetResponseValue(CommandWordType commandWord, std::vector<uint8_t> commandValue);

        bool SetConfigurationMode(bool enable);
        std::vector<uint8_t> ReadReportFrame();

        Uart* m_uart;
        BasicTargetData currentTargetData;
    };
}