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

char* ssid;
char* password;

static rtw_result_t scan_result_handler(rtw_scan_handler_result_t* malloced_scan_result);


void scanNetworks(void)
{
	printf("\n\n[WLAN_SCENARIO_EXAMPLE] Wi-Fi example scenario case 3...\n");
		/*********************************************************************************
		*	1. Enable Wi-Fi with STA mode
		**********************************************************************************/
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] Enable Wi-Fi\n");
		if(wifi_on(RTW_MODE_STA) < 0){
			printf("\n\r[WLAN_SCENARIO_EXAMPLE] ERROR: wifi_on failed\n");
			return;
		}
		vTaskDelay(5000);//Seems to be needed, not sure why
		/**********************************************************************************
		*	3. Connect to AP use STA mode (If failed, re-connect one time.)
		**********************************************************************************/
		printf("\n\r[WLAN_SCENARIO_EXAMPLE] Connect to AP use STA mode\n");

		// Set the auto-reconnect mode with retry 1 time(limit is 2) and timeout 5 seconds.
		// This command need to be set before invoke wifi_connect() to make reconnection work.
		wifi_config_autoreconnect(1, 2, 5);

		// Connect to AP with PSK-WPA2-AES.
		ssid = "WLAN_Test";
		password = "testtest";
		if(wifi_connect(ssid, RTW_SECURITY_WPA2_AES_PSK, password, strlen(ssid), strlen(password), -1, NULL) == RTW_SUCCESS)
		{
			printf("moving on...");
			//ethernetif_init(&xnetif[0]);
			//xnetif[0].flags |= 0x20U;
			LwIP_DHCP(0, DHCP_START);
		}
		printf("moving on 2...");
		vTaskDelete(NULL);
		// Show Wi-Fi info.
		//rtw_wifi_setting_t setting;
		//wifi_get_setting(WLAN0_NAME,&setting);
		//wifi_show_setting(WLAN0_NAME,&setting);

}

void main(void)
{
	//Initialize console
	console_init();
	//Initialize LwIP
	LwIP_Init();
	//Initialize wifi manager
	wifi_manager_init();
	//example_httpc();

#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	//Create scan networks task
	if(xTaskCreate(scanNetworks, ((const char*)"init"), STACKSIZE, NULL, tskIDLE_PRIORITY + 3 + PRIORITIE_OFFSET, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
	//vTaskDelete(NULL);
	printf("Wifi Started");
	printf("starting tcp example");
	example_socket_tcp_trx_1();
#endif


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
