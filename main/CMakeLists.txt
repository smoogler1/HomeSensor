
idf_component_register(
    SRCS 
"main.cpp" 
#"http_serv.c"
REQUIRES Hardware Platform Application Middleware esp_driver_gpio esp_eth  esp_wifi esp_http_server nvs_flash app_update
INCLUDE_DIRS "."
)
