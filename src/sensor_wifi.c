#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include "sensor_wifi.h"

static void wifi_socket_thread(void *param)
{
	int loops = 0;
	while (1)
	{
		printf("WiFi loop: %d\n",loops);
		loops++;
		vTaskDelay(10000);
	}
}

void start_sensor_wifi()
{
	if(xTaskCreate(wifi_socket_thread, ((const char*)"WiFi Socket Thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate(wifi_socket_thread) failed", __FUNCTION__);
}
