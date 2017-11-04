#define SENSOR_PACKET_SIZE 18
#define PACKET_TERMINATION_CHAR 'z'

void start_serial_thread(void);
char* get_data_loc(char* buffer_contents, int size);
