#pragma once

#include "ClimateSensor.hpp"
#include "Photoresistor.hpp"
#include "LightController.hpp"

void status_stream_init(ClimateSensor *pClimateSensor, Photoresistor *pPhotoresistor, LightController* pLightController);