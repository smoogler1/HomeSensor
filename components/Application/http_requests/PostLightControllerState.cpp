#include "esp_http_server.h"
#include "json_parser.h"
#include "http_requests/PostLightControllerState.hpp"

LightController* lightController = nullptr;

void light_controller_state_init(LightController* pLightController)
{
    lightController = pLightController;
}

esp_err_t post_light_controller_state(httpd_req_t *req)
{
    if(lightController == nullptr)
        return ESP_FAIL;

    constexpr size_t BUFFER_SIZE = 256;
    constexpr size_t TOKENS_NUMBER = 64;

    char buf[BUFFER_SIZE];
	int recv_len = httpd_req_recv(req, buf, sizeof(buf));

    jparse_ctx_t jsonCtx;
    json_tok_t tokens[TOKENS_NUMBER];

    int parse_result = json_parse_start_static(&jsonCtx, buf, recv_len, tokens, TOKENS_NUMBER);

    if(parse_result != 0)
        return ESP_FAIL;
        
    bool state = true;

    if(json_obj_get_bool(&jsonCtx, "state", &state) == 0)
    {
        if(state == true)
            lightController->EnableController();
        else
            lightController->DisableController();

        char responseBuffer[64];
        snprintf(responseBuffer, sizeof(responseBuffer), "{state:%d}", state); 

        httpd_resp_sendstr(req, responseBuffer);

        return ESP_OK;
    }

    return ESP_FAIL;

}

httpd_uri_t http_request_post_light_controller_state = {
	.uri	  = "/lightControllerState",
	.method   = HTTP_POST,
	.handler  = post_light_controller_state,
	.user_ctx = NULL
};