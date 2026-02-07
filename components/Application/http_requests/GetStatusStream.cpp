#include "esp_http_server.h"
#include "esp_log.h"
#include "http_requests/GetStatusStream.hpp"
#include "jsmn.h"
static const char *TAG = "Status";

static ClimateSensor *climateSensor;
static Photoresistor *photoresistor;
static LightController* lightContoller;

static bool streamInitialized = false;
static httpd_handle_t httpHandle;
static int socket_id;
static httpd_req_t *httpAsyncHandle;
static bool active = false;

void status_stream_task( void * pvParameters );

void status_stream_init(ClimateSensor *pClimateSensor, Photoresistor *pPhotoresistor, LightController* pLightController)
{
    climateSensor = pClimateSensor;
    photoresistor = pPhotoresistor;
    lightContoller = pLightController;

    xTaskCreate(status_stream_task, "StatusStream", 2048, NULL, 3, NULL);
    streamInitialized = true;
}

void status_stream_once(void *arg)
{
    if(active == false)
        return;

    char buf[128];
    int len = snprintf(buf, sizeof(buf),
        "data: {\"temperature\":%f, \"humidity\":%f, \"brightness\":%d, \"light_state\":%d}\n\n",
        climateSensor->GetTemperature(),
        climateSensor->GetHumidity(),
        (int)photoresistor->CurrentBrightness(),
        lightContoller->GetLightState());

    // Send on the underlying socket (common pattern for async/push use-cases)
    // If send fails, mark inactive and let the server close it.
    esp_err_t err = httpd_resp_send_chunk(httpAsyncHandle, buf, len);

    if(err != ESP_OK)
    {
        httpd_req_async_handler_complete(httpAsyncHandle);
        active = false;
    }
}

void status_stream_task( void * pvParameters )
{
    ESP_LOGI(TAG,"status_stream_task 1");
    for( ;; )
    {
        if(active == true)
        {
            httpd_queue_work(httpHandle, status_stream_once, NULL);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static esp_err_t get_status_stream_handler(httpd_req_t *req)
{
    if(active)
    {
        httpd_resp_set_status(httpAsyncHandle, "200 OK");
        httpd_resp_sendstr(req, "SSE already active");
        return ESP_OK;
    }
        
    ESP_LOGI("LOG","get_status_stream_handler 1");
    if (httpd_req_async_handler_begin(req, &httpAsyncHandle) != ESP_OK) {
        return ESP_FAIL;
    } 

    httpd_resp_set_status(httpAsyncHandle, "200 OK");
    httpd_resp_set_type(httpAsyncHandle, "text/event-stream");
    httpd_resp_set_hdr(httpAsyncHandle, "Cache-Control", "no-cache");
    httpd_resp_set_hdr(httpAsyncHandle, "Connection", "keep-alive");

    const char *hello = ": connected\n\n";
    esp_err_t err = httpd_resp_send_chunk(httpAsyncHandle, hello, HTTPD_RESP_USE_STRLEN);

    if (err != ESP_OK) {
        httpd_req_async_handler_complete(httpAsyncHandle); // [web:33]
        return ESP_OK;
    }

    httpHandle = httpAsyncHandle->handle;
    socket_id = httpd_req_to_sockfd(httpAsyncHandle);

    active = true;
    ESP_LOGI("LOG","get_status_stream_handler 2");
    return ESP_OK;
}

httpd_uri_t http_request_get_status_stream = {
    .uri = "/statusStream",
    .method = HTTP_GET,
    .handler = get_status_stream_handler,
    .user_ctx = NULL};