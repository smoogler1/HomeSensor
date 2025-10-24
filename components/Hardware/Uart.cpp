#include <Uart.hpp>
#include "driver/uart.h"
#include "driver/gpio.h"

Uart::Uart(uint8_t uartId, int baudRate, int txPin, int rxPin):
m_uartId(uartId)
{
    uart_config_t uart_config = {
        .baud_rate = baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install((uart_port_t) m_uartId, DEFAULT_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config((uart_port_t) m_uartId, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin((uart_port_t) m_uartId, GPIO_NUM_21, GPIO_NUM_20, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

uint32_t Uart::WriteSync(std::vector<uint8_t> data)
{
    return uart_write_bytes((uart_port_t) m_uartId, (const char *) data.data(), data.size());
}


uint32_t Uart::ReadSync(std::vector<uint8_t>& buffer, uint32_t bytesToRead, uint32_t timeoutMs)
{
    return uart_read_bytes((uart_port_t) m_uartId, buffer.data(), bytesToRead, timeoutMs);;
}

uint32_t Uart::ReadSyncAll(std::vector<uint8_t>& buffer, uint32_t timeoutMs)
{
    size_t bufSize = 0;

    auto result = uart_get_buffered_data_len((uart_port_t) m_uartId,&bufSize);

    if(result != ESP_OK)
        return 0;

    return uart_read_bytes((uart_port_t) m_uartId, buffer.data(), bufSize, timeoutMs);
}

Uart::~Uart()
{
    
}
