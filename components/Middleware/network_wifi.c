#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include <netdb.h>
#include "nvs_flash.h"
#include "NvMemoryWrapper.h"
#include "network_wifi.h"

static const char *TAG = "wifi";

#define WIFI_SSID "Netia"
#define WIFI_PASS "$Threesmallpigs$"
#define WIFI_MAXIMUM_RETRY 10

#define STATIC_IP_ADDR "192.168.1.52"
#define STATIC_NETMASK_ADDR "255.255.255.0"
#define STATIC_GW_ADDR "192.168.1.1"

#define IP_CONFIG_KEY "ip_config"

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_network_event_group;
static esp_netif_t *s_netif;
#define CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static void print_ip_uint(esp_netif_ip_info_t *config)
{
    char ip_string[32];
    ip4addr_ntoa_r((ip4_addr_t*)&config->ip, ip_string, sizeof(ip_string));

    char netmask_string[32];
    ip4addr_ntoa_r((ip4_addr_t*)&config->netmask, netmask_string, sizeof(netmask_string));

    char gw_string[32];
    ip4addr_ntoa_r((ip4_addr_t*)&config->gw, gw_string, sizeof(gw_string));

    ESP_LOGI(TAG, "IP: %s, netmask: %s, gw: %s", ip_string, netmask_string, gw_string);
}

static bool validate_ip_config(esp_netif_ip_info_t *config)
{
#define EMPTY_IP_ADDR 0xFFFFFFFF

    if (config->ip.addr == EMPTY_IP_ADDR)
        return false;

    if (config->netmask.addr == EMPTY_IP_ADDR)
        return false;

    if (config->gw.addr == EMPTY_IP_ADDR)
        return false;

    return true;
};

static bool load_ip_config(esp_netif_ip_info_t *config)
{
    bool result = nv_memory_load(IP_CONFIG_KEY, config, sizeof(esp_netif_ip_info_t));

    if(result == true)
    {
        ESP_LOGI(TAG, "Loaded static ip");
        print_ip_uint(config);
    }
    else
    {
        ESP_LOGI(TAG, "Failed to load IP");
        return false;
    }

    return validate_ip_config(config) && result;
}

static bool save_ip_config(esp_netif_ip_info_t *config)
{
    return nv_memory_save(IP_CONFIG_KEY, config, sizeof(esp_netif_ip_info_t));
}

static bool set_static_ip_string(esp_netif_t *netif, const char *ip, const char *netmask, const char *gw)
{

    esp_netif_ip_info_t ip_config;
    memset(&ip_config, 0, sizeof(esp_netif_ip_info_t));
    ip_config.ip.addr = ipaddr_addr(ip);
    ip_config.netmask.addr = ipaddr_addr(netmask);
    ip_config.gw.addr = ipaddr_addr(gw);
    if (esp_netif_set_ip_info(netif, &ip_config) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set ip info");
        return false;
    }

    save_ip_config(&ip_config);

    ESP_LOGI(TAG, "Success to set static ip:");
    print_ip_uint(&ip_config);

    return true;
}

static bool set_static_ip_uint(esp_netif_t *netif, uint32_t ip, uint32_t netmask, uint32_t gw)
{
    esp_netif_ip_info_t ip_config;
    memset(&ip_config, 0, sizeof(esp_netif_ip_info_t));
    ip_config.ip.addr = ip;
    ip_config.netmask.addr = netmask;
    ip_config.gw.addr = gw;
    if (esp_netif_set_ip_info(netif, &ip_config) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set ip info");
        return false;
    }

    save_ip_config(&ip_config);

    ESP_LOGI(TAG, "Success to set static ip:");
    print_ip_uint(&ip_config);

    return true;
}

static void load_ip(esp_netif_t *netif)
{
    esp_netif_ip_info_t ip;
    if (load_ip_config(&ip) == true)
    {
        set_static_ip_uint(netif, ip.ip.addr, ip.netmask.addr, ip.gw.addr);
    }
    else
    {
        set_static_ip_string(netif, STATIC_IP_ADDR, STATIC_NETMASK_ADDR, STATIC_GW_ADDR);
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        //
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < WIFI_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_network_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_network_event_group, CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    s_netif = esp_netif_create_default_wifi_sta();
    assert(s_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        s_netif,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        s_netif,
                                                        &instance_got_ip));

    if (esp_netif_dhcpc_stop(s_netif) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to stop dhcp client");
    }

    load_ip(s_netif);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. In case your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by wifi_event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_network_event_group,
                                           CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 WIFI_SSID, WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 WIFI_SSID, WIFI_PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_network_event_group);
}

void network_wifi_init(void)
{
    s_network_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_sta();
}

bool network_wifi_set_ip(const char *ip, const char *netmask, const char *gw)
{
    return set_static_ip_string(s_netif, ip, netmask, gw);
}