#ifndef SENSOR_WIFI_H
#define SENSOR_WIFI_H

#include "FreeRTOS.h"
#include "semphr.h"
void start_sensor_wifi_thread(void);
BaseType_t write_wifi_buffer(const char * dataStart);


#endif
