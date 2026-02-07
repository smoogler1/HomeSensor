#include "http_server.h"
#include "HttpServicesInit.h"
#include "http_requests/GetStatusStream.hpp"
#include "http_requests/PostLightControllerState.hpp"

static ClimateSensor *climateSensor;
static Photoresistor *photoresistor;
static LightController *lightController;

void http_set_climate_sensor(ClimateSensor *pClimateSensor)
{
	climateSensor = pClimateSensor;
}

void http_set_photoresistor(Photoresistor *pPhotoresistor)
{
	photoresistor = pPhotoresistor;
}

void http_set_light_controller(LightController *pLightController)
{
	lightController = pLightController;
}

void HttpServicesInit()
{
    status_stream_init(climateSensor, photoresistor,lightController);
	light_controller_state_init(lightController);
}