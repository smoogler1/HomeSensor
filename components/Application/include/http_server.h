#ifndef HTTP_SERVER
#define HTTP_SERVER

#include "ClimateSensor.hpp"
#include "Photoresistor.hpp"
#include "LightController.hpp"

void http_set_climate_sensor(ClimateSensor *pClimateSensor);
void http_set_photoresistor(Photoresistor *pPhotoresistor);
void http_set_light_controller(LightController *pLightController);

#ifdef __cplusplus
extern "C" {
#endif

void http_server_init();

#ifdef __cplusplus
}
#endif

#endif