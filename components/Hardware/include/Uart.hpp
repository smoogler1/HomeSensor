#pragma once    

#include <cstdint>
#include <vector>

class Uart
{
    public:

    constexpr static int DEFAULT_BUF_SIZE = 1024;
    constexpr static int DEFAULT_TIMEOUT_MS = 1000;

    Uart(uint8_t uartId, int baudRate, int txPin, int rxPin);

    uint32_t WriteSync(std::vector<uint8_t> data);
    uint32_t ReadSync(std::vector<uint8_t>& buffer, uint32_t bytesToRead = DEFAULT_BUF_SIZE, uint32_t timeoutMs = DEFAULT_TIMEOUT_MS);
    uint32_t ReadSyncAll(std::vector<uint8_t>& buffer, uint32_t timeoutMs = DEFAULT_TIMEOUT_MS);

    ~Uart();

    private:
    uint8_t m_uartId;
};