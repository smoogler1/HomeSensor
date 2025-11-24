idf_component_register(
    SRCS 
"Adc.cpp" 
"Uart.cpp"
"Gpio.cpp"
REQUIRES esp_driver_uart esp_driver_gpio esp_adc Dispatcher
INCLUDE_DIRS "include")
