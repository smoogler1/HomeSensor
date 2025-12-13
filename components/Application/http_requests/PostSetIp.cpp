#include "esp_http_server.h"
#include "json_parser.h"
#include "esp_wifi.h"
#include <netdb.h>
#include "network_wifi.h"
#include "esp_log.h"
/*
 * Handle OTA file upload
 */
esp_err_t post_ip_set_handler(httpd_req_t *req)
{
    constexpr size_t TOKENS_NUMBER = 64;
    constexpr size_t BUFFER_SIZE = 256;
    constexpr size_t IP_BUFFER_SIZE = 32;

    char buf[BUFFER_SIZE];
	int recv_len = httpd_req_recv(req, buf, sizeof(buf));

    jparse_ctx_t jsonCtx;
    json_tok_t tokens[TOKENS_NUMBER];

    int parse_result = json_parse_start_static(&jsonCtx, buf, recv_len, tokens, TOKENS_NUMBER);

    if(parse_result != 0)
        goto ip_fail;

    esp_netif_ip_info_t ip;

    char ipString[IP_BUFFER_SIZE];
    char netmaskString[IP_BUFFER_SIZE];
    char gatewayString[IP_BUFFER_SIZE];

    if(json_obj_get_string(&jsonCtx, "ip", ipString, IP_BUFFER_SIZE) != 0)
        goto ip_fail;

    if(json_obj_get_string(&jsonCtx, "netmask", netmaskString, IP_BUFFER_SIZE) != 0)
        goto ip_fail;

    if(json_obj_get_string(&jsonCtx, "gateway", gatewayString, IP_BUFFER_SIZE) != 0)
        goto ip_fail;

    if(network_wifi_set_ip(ipString, netmaskString, gatewayString) == true)
    {
        ESP_LOGI("HTTP", "New IP set ip: %s, netmask: %s, gateway: %s", ipString, netmaskString, gatewayString);
        httpd_resp_sendstr(req, "New IP set!\n");
        return ESP_OK;
    }
    
    ip_fail:
    httpd_resp_sendstr(req, "IP set error!\n");
    return ESP_FAIL;
}

httpd_uri_t http_request_post_set_ip = {
	.uri	  = "/set_ip",
	.method   = HTTP_POST,
	.handler  = post_ip_set_handler,
	.user_ctx = NULL
};