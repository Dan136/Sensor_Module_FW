#ifndef SENSOR_WIFI_H
#define SENSOR_WIFI_H

#include <FreeRTOS.h>
#include <semphr.h>

void start_tcp_socket(void);
void start_sensor_wifi(void);
int connect_to_network(void);
BaseType_t write_wifi_buffer(const char * dataStart);


#endif
