idf_component_register(
SRCS 
"LightController.cpp" 
"OtaUpdate.cpp" 
"http_server.c"
"HttpServicesInit.cpp"
# requests
"http_requests/GetIndex.cpp"
"http_requests/PostOtaUpdate.cpp"
"http_requests/PostSetIp.cpp"
"http_requests/PostLightControllerState.cpp"
"http_requests/GetStatusStream.cpp"
"http_requests/GetStatus.cpp"
INCLUDE_DIRS "include"
REQUIRES Common Platform Hardware Middleware esp_http_server esp_wifi app_update json_parser 
EMBED_TXTFILES web/index.html
EMBED_TXTFILES web/status.html)
