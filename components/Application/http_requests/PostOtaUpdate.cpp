#include "esp_http_server.h"
#include "OtaUpdate.hpp"
#include "freertos/task.h"
#include "esp_system.h"

esp_err_t post_ota_update_handler(httpd_req_t *req)
{
	char buf[1000];
	int remaining = req->content_len;

    OtaUpdate otaUpdate;

    if(otaUpdate.StartUpdate() == false)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Start error");
		return ESP_FAIL;
    }

	while (remaining > 0) {
		int recv_len = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));

		// Timeout Error: Just retry
		if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
			continue;

		// Serious Error: Abort OTA
		} else if (recv_len <= 0) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Protocol Error");
			return ESP_FAIL;
		}

        // Successful Upload: Flash firmware chunk
		if (!otaUpdate.WriteFirmwarePart(reinterpret_cast<uint8_t*>(buf),recv_len)) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Flash Error");
			return ESP_FAIL;
		}

		remaining -= recv_len;
	}

	// Validate and switch to new OTA image and reboot
	if(otaUpdate.FinalizeUpdate() == false) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Validation / Activation Error");
			return ESP_FAIL;
	}

	httpd_resp_sendstr(req, "Firmware update complete, rebooting now!\n");

	vTaskDelay(500 / portTICK_PERIOD_MS);
	esp_restart();

	return ESP_OK;
}

httpd_uri_t http_request_post_ota_update = {
	.uri	  = "/update",
	.method   = HTTP_POST,
	.handler  = post_ota_update_handler,
	.user_ctx = NULL
};