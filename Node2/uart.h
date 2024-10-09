
// UART is connected to the ATmega16U2 on the Arduino, which acts as a USB serial device.
// This module hooks into the stdio functions, by spoofing several system calls
//
// Unfortunately, for some unknown reason `scanf` does not work as expected. Use 
// `uart_flush` and `sscanf` instead (see example below)
//
// Internally, receiving is handled with interrupts and a ring buffer, so no characters
// are lost, until the buffer is full. If necessary, you can change this buffer size in 
// uart.c
#include <stdint.h>
#define baudrate 9600
typedef struct {
	uint8_t buffer[1024];
	int capacity;
	
	int insertIdx;
	int removeIdx;
	int length;
}RingBuf;

extern RingBuf rb;
void ringbuf_init(RingBuf *rb);
int pop(RingBuf* rb, uint8_t* val);
int push(RingBuf* rb, uint8_t val);
// Initialize. Hooks stdio functions (like `printf`)
void uart_init(uint32_t cpufreq);

// Send a single character
// Prefer using `printf` instead
void uart_tx(uint8_t val);

// Read a single character
// Prefer using `uart_flush` and `sscanf` instead (see below)
uint8_t uart_rx(uint8_t* val, RingBuf *rb);

// Flush the internal ring buffer into your own buffer
// Example (`scanf` workaround):
//    int result;
//    char buf[80] = {0};
//
//    uart_flush(buf, sizeof(buf)/sizeof(buf[0]));
//    int numFilled = sscanf(buf, "%d", &result);
//    if(numFilled == 1){
//        printf("%d\n", result);
//    }
int uart_flush(char* buf, int len, RingBuf *rb);

void UART_Handler();

