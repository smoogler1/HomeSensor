#include "XiaoHumanPresenceSensor.hpp"
#include "freertos/FreeRTOS.h"
#include <vector>
#include <map>
#include <array>
#include <sstream>
#include "esp_log.h"

namespace Xiao
{
    namespace
    {
        constexpr uint8_t INIT_FRAME_DATA_LENGTH_FIELD_SIZE = 2;
        constexpr uint8_t COMMAND_WORD_SIZE = 2;
        constexpr uint8_t MIN_RESPONSE_FRAME_LENGTH = 12;
        constexpr uint8_t MIN_REPORT_FRAME_LENGTH = 23;

        constexpr std::array<uint8_t, 4> FrameHeader{0xFD, 0xFC, 0xFB, 0xFA};
        constexpr std::array<uint8_t, 4> EndOfFrame{0x04, 0x03, 0x02, 0x01};

        constexpr std::array<uint8_t, 4> ReportHeader{0xF4, 0xF3, 0xF2, 0xF1};
        constexpr std::array<uint8_t, 4> ReportEndOfFrame{0xF8, 0xF7, 0xF6, 0xF5};

        enum class Command : uint8_t
        {
            EnableConfigurationMode,
            DisableConfigurationMode
        };

        enum class ReportedDataType: uint8_t
        {
            EngineeringModeData = 0x01,
            BasicModeData = 0x02
        };

        const std::map<Command, CommandWordType> CommandWordMap =
            {
                {Command::EnableConfigurationMode, {0xFF, 0x00}},
                {Command::DisableConfigurationMode, {0xFE, 0x00}}};

        #pragma pack(1)
        struct ConfigFrameStructure
        {
            std::array<uint8_t, 4> frameHeader;
            uint16_t length;
            CommandWordType commandWord;
            uint8_t commandValue[];
        };

        struct ResponseStatus
        {
            uint16_t status;
        };

        struct ReportedDataFrameStructure
        {
            std::array<uint8_t, 4> frameHeader;
            uint16_t length;
            ReportedDataType dataType;
            uint8_t head;
            BasicTargetData targetData;
            uint8_t tail;
            uint8_t calibration;
            std::array<uint8_t, 4> endOfFrame;
        };
        #pragma pack(pop)

        inline bool IsCommandWordResponse(CommandWordType commandWord)
        {
            constexpr uint16_t RESPONSE_MASK = 0x01;

            return commandWord[1] == RESPONSE_MASK;
        }

        std::vector<uint8_t> BuildFrame(CommandWordType commandWord, std::vector<uint8_t> commandValue)
        {
            std::vector<uint8_t> frame;
            frame.reserve(FrameHeader.size() + INIT_FRAME_DATA_LENGTH_FIELD_SIZE + sizeof(CommandWordType) + commandValue.size() + EndOfFrame.size());
            uint16_t length = static_cast<uint16_t>(commandValue.size()) + static_cast<uint16_t>(commandWord.size());
            // Frame header
            frame.insert(frame.end(), FrameHeader.begin(), FrameHeader.end());
            // Intra-frame data length
            frame.push_back(static_cast<uint8_t>(length & 0xFF));
            frame.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
            // Intra-frame command word
            frame.insert(frame.end(), commandWord.begin(), commandWord.end());
            // Intra-frame command value
            if (commandValue.size() > 0)
                frame.insert(frame.end(), commandValue.begin(), commandValue.end());
            // End of frame
            frame.insert(frame.end(), EndOfFrame.begin(), EndOfFrame.end());
            return frame;
        }

        bool CheckResponseFrame(const std::vector<uint8_t> frameData)
        {
            if (frameData.size() < MIN_RESPONSE_FRAME_LENGTH)
                return false;

            const ConfigFrameStructure *responseFrame = reinterpret_cast<const ConfigFrameStructure *>(frameData.data());

            if (responseFrame->frameHeader != FrameHeader)
                return false;

            if (!IsCommandWordResponse(responseFrame->commandWord))
                return false;

            return true;
        }

        bool CheckReportFrame(const std::vector<uint8_t> frameData)
        {
            if (frameData.size() < MIN_REPORT_FRAME_LENGTH)
                return false;

            const ReportedDataFrameStructure *responseFrame = reinterpret_cast<const ReportedDataFrameStructure *>(frameData.data());

            if (responseFrame->frameHeader != ReportHeader)
                return false;

            if (responseFrame->head != 0xAA)
                return false;

            if (responseFrame->tail != 0x55)
                return false;

            if(responseFrame->endOfFrame != ReportEndOfFrame)
                return false;

            return true;
        }

    }

    XiaoHumanPresenceSensor::XiaoHumanPresenceSensor(Uart *uart) : m_uart(uart)
    {
        SetConfigurationMode(true);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        SetConfigurationMode(false);
    }

    XiaoHumanPresenceSensor::~XiaoHumanPresenceSensor()
    {
    }

    void XiaoHumanPresenceSensor::Configure()
    {
    }

    std::optional<std::vector<uint8_t>> XiaoHumanPresenceSensor::SendCommandAndGetResponseValue(CommandWordType commandWord, std::vector<uint8_t> commandValue)
    {
        const std::vector<uint8_t> frame = BuildFrame(commandWord, commandValue);

        std::ostringstream ss1;

        for (auto byte : frame)
        {
            ss1 << std::hex << (int)byte << " ";
        }

        ESP_LOGI("LOG", "Write str: %s", ss1.str().c_str());

        auto writtenBytes = m_uart->WriteSync(frame);

        if (writtenBytes != frame.size())
            return {};

        vTaskDelay(100 / portTICK_PERIOD_MS);

        std::vector<uint8_t> readBuffer(256);

        auto readBytes = m_uart->ReadSyncAll(readBuffer, 2000);

        std::ostringstream ss2;

        for (auto byte : readBuffer)
        {
            ss2 << std::hex << (int)byte << " ";
        }

        ESP_LOGI("LOG", "Recv str: %s", ss2.str().c_str());
        if (CheckResponseFrame(readBuffer) == false)
            return {};

        ESP_LOGI("LOG", "Response Frame");
        ConfigFrameStructure *responseFrame = reinterpret_cast<ConfigFrameStructure *>(readBuffer.data());

        ResponseStatus *responseData = reinterpret_cast<ResponseStatus *>(&responseFrame->commandValue);

        if (responseData->status != 0)
            return {};

        ESP_LOGI("LOG", "Status ok");
        
        return std::vector<uint8_t>(reinterpret_cast<uint8_t*>(responseData), reinterpret_cast<uint8_t*>(responseData + responseFrame->length - COMMAND_WORD_SIZE));
    }

    bool XiaoHumanPresenceSensor::SetConfigurationMode(bool enable)
    {
        std::vector<uint8_t> commandValue = {};
        CommandWordType command = enable ? CommandWordMap.at(Command::EnableConfigurationMode) : CommandWordMap.at(Command::DisableConfigurationMode);

        if (enable)
            commandValue = std::vector<uint8_t>{0x01, 0x00};

        auto result = SendCommandAndGetResponseValue(command, commandValue);

        if (result.has_value())
            return true;

        return false;
    }

    std::vector<uint8_t> XiaoHumanPresenceSensor::ReadReportFrame()
    {
        std::vector<uint8_t> retBuffer(1);
        auto readBytes = m_uart->ReadSync(retBuffer, 1, 10);

        if(readBytes == 0)
            return {};
        
        if(retBuffer[0] == ReportHeader[0])
        {
            std::vector<uint8_t> dataBuffer(64);

            std::size_t bytesToRead = MIN_REPORT_FRAME_LENGTH - 1;
            auto readBytes = m_uart->ReadSync(dataBuffer,bytesToRead, 1000);

            retBuffer.insert(retBuffer.begin() + 1, dataBuffer.begin(), dataBuffer.end());

            if(readBytes >= bytesToRead)
                return retBuffer;
        }
        else
        {
            ESP_LOGW("XIAO","WRONG FRAME");
        }

        return {};

    }

    void XiaoHumanPresenceSensor::Update()
    {
        std::vector<uint8_t> readBuffer(ReadReportFrame());

        if(readBuffer.empty())
            return;

        // std::ostringstream ss2;

        // for (auto byte : readBuffer)
        // {
        //     ss2 << std::hex << (int)byte << " ";
        // }

        //ESP_LOGI("LOG", "Recv str: %s", ss2.str().c_str());

        if (CheckReportFrame(readBuffer) == false)
            return;

        //ESP_LOGI("LOG", "Report ok");

        ReportedDataFrameStructure *responseFrame = reinterpret_cast<ReportedDataFrameStructure *>(readBuffer.data());
        BasicTargetData targetData = responseFrame->targetData;
        currentTargetData = targetData;
    }

    BasicTargetData XiaoHumanPresenceSensor::GetTargetData()
    {
        return currentTargetData;
    }

}