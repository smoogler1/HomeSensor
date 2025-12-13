#include "esp_http_server.h"

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

/* ---- Handlery HTTP ---- */
static esp_err_t get_index_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) index_html_start, index_html_end - index_html_start);
	return ESP_OK;
}

httpd_uri_t http_request_get_index = {
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = get_index_handler,
	.user_ctx = NULL
};