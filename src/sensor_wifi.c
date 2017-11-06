#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include "sensor_wifi.h"

static SemaphoreHandle_t wifiBufSem = 0;
static char wifiBuf[50];

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
	wifiBufSem = xSemaphoreCreateMutex(); // initialize semaphore
	if(xTaskCreate(wifi_socket_thread, ((const char*)"WiFi Socket Thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate(wifi_socket_thread) failed", __FUNCTION__);
}

BaseType_t write_wifi_buffer(const char * dataStart){
	BaseType_t retValue = pdFAIL;
	if(wifiBufSem){ // only run with initialized semaphore
		retValue = xSemaphoreTake(wifiBufSem, 0);
		if(retValue == pdPASS){ // copy data and return semaphore if semaphore was taken
			strcpy(wifiBuf, dataStart);
			xSemaphoreGive(wifiBufSem);
		}
	}
	return retValue;

}
