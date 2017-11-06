#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include "device.h"
#include "serial_api.h"
#include "sensor_serial.h"

#define UART_TX    PA_4
#define UART_RX    PA_0

static void serial_read_thread(void *param)
{
	char rc = 'q';
	char sData[300];
	char* dataToSend;

	serial_t    sobj;
	serial_init(&sobj,UART_TX,UART_RX);
	serial_baud(&sobj,9600);
	serial_format(&sobj, 8, ParityNone, 1);
	int loops = 0;
	unsigned int charsRecieved;
	while (1)
	{
		printf("Serial loop: %d\n",loops);
		charsRecieved = serial_recv_blocked(&sobj, sData, 2*SENSOR_PACKET_SIZE, 3000);
		serial_clear_rx(&sobj);
		dataToSend = get_data_loc(sData, charsRecieved);
		if(dataToSend == 0){
			printf("Serial Received: No Terminator Found\n"); //Error with data
		}
		else
		{
			printf("Serial Received: %d characters \n%s", charsRecieved,  dataToSend);
			BaseType_t wifiBufPass = pdFAIL;
			while(wifiBufPass != pdPASS){ // attempt write to wifi buf until success
				wifiBufPass = write_wifi_buffer(dataToSend);
			}
		}
		loops++;
		vTaskDelay(5000);
	}
}

char* get_data_loc(char* buffer_contents, int size)
{
	int i = size - 1;
	while (i > 0) // while there is at least one char before possible termination char
	{
		if (buffer_contents[i]==PACKET_TERMINATION_CHAR)
		{
			buffer_contents[i] = '\0'; //Change end char to null for string termination
			return buffer_contents + (i>SENSOR_PACKET_SIZE-1 ? i-SENSOR_PACKET_SIZE : 0);  //conditional guard against partial packet causing access to uninitialized mem
		}
		i--;
	}
	return 0;
}

void start_serial_thread()
{
	if(xTaskCreate(serial_read_thread, ((const char*)"serial read thread"), 2048, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate(serial_read_thread) failed", __FUNCTION__);
}
