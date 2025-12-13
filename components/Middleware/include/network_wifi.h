#ifndef NETWORK_WIFI
#define NETWORK_WIFI

#ifdef __cplusplus
extern "C" {
#endif

void network_wifi_init();
bool network_wifi_set_ip(const char* ip, const char* netmask, const char* gw);

#ifdef __cplusplus
}
#endif
#endif