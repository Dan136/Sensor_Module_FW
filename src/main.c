#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include "wifi_constants.h"
#include "wifi_structures.h"
#include "lwip_netconf.h"
#include "wifi_conf.h"
#include "netif.h"
#include "example_socket_tcp_trx.h"
#include <platform/platform_stdlib.h>
#include "sensor_serial.h"
#include "sensor_wifi.h"

#define STACKSIZE                   (512 + 768)
#if CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif

extern void wlan_netowrk(void);
extern void console_init(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */


static rtw_result_t scan_result_handler(rtw_scan_handler_result_t* malloced_scan_result);


void main(void)
{
	printf("\nInitializing Sensor Board Firmware...\n\n");
	//Initialize console
	console_init();
	//Initialize LwIP
	LwIP_Init();
	//Initialize wifi manager
	wifi_manager_init();
	if(xTaskCreate(start_sensor_wifi, ((const char*)"Sensor WiFI"), STACKSIZE, NULL, tskIDLE_PRIORITY + 3 + PRIORITIE_OFFSET, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(start_sensor_wifi) failed", __FUNCTION__);
	start_serial_thread();
	printf("starting tcp example");
	example_socket_tcp_trx_1();


    //3 3)Enable Schedule, Start Kernel
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	RtlConsolTaskRom(NULL);
#endif

}

static rtw_result_t scan_result_handler(rtw_scan_handler_result_t* malloced_scan_result){
	static int ApNum = 0;
	if (malloced_scan_result->scan_complete != RTW_TRUE) {
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

		printf("%d\t", ++ApNum);
		printf("%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra" );
    		printf("%02x:%02x:%02x:%02x:%02x:%02x", MAC_ARG(record->BSSID.octet));
    		printf(" %d\t ", record->signal_strength);
    		printf(" %d\t  ", record->channel);
    		printf(" %d\t  ", record->wps_type);
    		printf("%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
                                 ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
                                 ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
                                 ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_MIXED ) ? "WPA/WPA2 AES" :
                                 "Unknown");
    		printf( " %s ", record->SSID.val);
    		printf("\r\n");
	}
	return RTW_SUCCESS;
}
