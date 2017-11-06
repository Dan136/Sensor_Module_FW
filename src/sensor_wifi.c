#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <semphr.h>
#include "sensor_wifi.h"
#include "wifi_constants.h"
#include "wifi_structures.h"
#include "lwip_netconf.h"
#include "wifi_conf.h"
#include "netif.h"

static SemaphoreHandle_t wifiBufSem = 0;
static char wifiBuf[50];

char* ssid;
char* password;

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
	connect_to_network();
	wifiBufSem = xSemaphoreCreateMutex(); // initialize semaphore
	if(xTaskCreate(wifi_socket_thread, ((const char*)"WiFi Socket Thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate(wifi_socket_thread) failed", __FUNCTION__);
	vTaskDelete(NULL);
}

int connect_to_network()
{
	wifi_on(RTW_MODE_STA); //Resets wifi so we dont get errors
	wifi_off(); //Resets wifi to avoid errors due to warm start
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r[WIFI] ERROR: wifi_on failed\n");
		return 0;
	}
	vTaskDelay(5000);
	// Set the auto-reconnect mode with retry 1 time(limit is 2) and timeout 5 seconds.
	// This command need to be set before invoke wifi_connect() to make reconnection work.
	wifi_config_autoreconnect(1, 2, 5);
	// Connect to AP with PSK-WPA2-AES.
	ssid = "WLAN_Test";
	password = "testtest";
	if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
	{
		printf("WiFi Connected!");
		LwIP_DHCP(0, DHCP_START);
		vTaskDelay(2000);
	}
	else {
		printf("Error connecting to WIFI!");
		return 0;
	}
	return 1;
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
