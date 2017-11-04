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
	serial_t    sobj;
	serial_init(&sobj,UART_TX,UART_RX);
	serial_baud(&sobj,38400);
	serial_format(&sobj, 8, ParityNone, 1);
	int loops = 0;
	while (1)
	{
		printf("Serial loop: %d\n",loops);
		//rc = serial_getc(&sobj); //Not working, mot sure why
		printf("Serial Received: ");
		printf(rc);
		printf("\n");
		loops++;
		vTaskDelay(5000);
	}
}

void start_serial_thread()
{
	if(xTaskCreate(serial_read_thread, ((const char*)"serial read thread"), 2048, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate(serial_read_thread) failed", __FUNCTION__);
}
