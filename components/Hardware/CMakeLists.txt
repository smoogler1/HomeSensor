idf_component_register(
    SRCS 
"Adc.cpp" 
"Uart.cpp"
"Gpio.cpp"
"NvMemory.cpp"
"NvMemoryWrapper.cpp"
REQUIRES esp_driver_uart esp_driver_gpio esp_adc nvs_flash Common
INCLUDE_DIRS "include")
