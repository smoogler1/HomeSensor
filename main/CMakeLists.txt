
idf_component_register(
    SRCS 
"main.cpp" 
"http_serv.c"
REQUIRES  esp_driver_gpio esp_eth Hardware Platform Dispatcher Application esp_wifi esp_http_server nvs_flash app_update
INCLUDE_DIRS "."
EMBED_TXTFILES web/index.html)
