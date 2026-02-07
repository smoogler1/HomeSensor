#include "esp_http_server.h"

extern const uint8_t status_html_start[] asm("_binary_status_html_start");
extern const uint8_t status_html_end[] asm("_binary_status_html_end");

/* ---- Handlery HTTP ---- */
static esp_err_t get_status_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) status_html_start, status_html_end - status_html_start);
	return ESP_OK;
}

httpd_uri_t http_request_get_status = {
	.uri	  = "/status",
	.method   = HTTP_GET,
	.handler  = get_status_handler,
	.user_ctx = NULL
};