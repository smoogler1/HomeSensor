#include "esp_http_server.h"
#include "HttpServicesInit.h"

extern httpd_uri_t http_request_get_index;
extern httpd_uri_t http_request_post_ota_update;
extern httpd_uri_t http_request_post_set_ip;
extern httpd_uri_t http_request_get_status_stream;
extern httpd_uri_t http_request_get_status;
extern httpd_uri_t http_request_post_light_controller_state;
bool http_server_init(void) {
	static httpd_handle_t http_server = NULL;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	HttpServicesInit();

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &http_request_get_index);
		httpd_register_uri_handler(http_server, &http_request_post_ota_update);
        httpd_register_uri_handler(http_server, &http_request_post_set_ip);
		httpd_register_uri_handler(http_server, &http_request_get_status_stream);
		httpd_register_uri_handler(http_server, &http_request_get_status);
		httpd_register_uri_handler(http_server, &http_request_post_light_controller_state);
	}

	return http_server == NULL ? false : true;
}