#include "sam.h"
#include "uart.h"
#include <stdio.h>


#define F_CPU 84000000

void ringbuf_init(RingBuf *rb) {
	rb->capacity = sizeof(rb->buffer);
	rb->insertIdx = 0;
	rb->removeIdx = 0;
	rb->length = 0;
}


int push(RingBuf* rb, uint8_t val){
    if(rb->length >= (sizeof(rb->buffer)/sizeof(rb->buffer[0]))){
        return 0;
    }
    rb->buffer[rb->insertIdx] = val;
    rb->insertIdx = (rb->insertIdx + 1) % (sizeof(rb->buffer)/sizeof(rb->buffer[0]));
    rb->length++;
    return 1;
}

int pop(RingBuf* rb, uint8_t* val){    
    if(!rb->length){
        return 0;
    }
    *val = rb->buffer[rb->removeIdx];
    rb->removeIdx = (rb->removeIdx + 1) % (sizeof(rb->buffer)/sizeof(rb->buffer[0]));
    rb->length--;
    return 1;
}
/*
void uart_init(uint32_t cpufreq){
    PMC->PMC_PCER0 |= (1 << ID_UART);
    
    // Set UART pins (A8, A9) to use alternate function (this disables regular IO)
    PIOA->PIO_PDR   |=   PIO_PA8 | PIO_PA9;
    // Set alternate function A (see tables 9-2, 34-2)
    PIOA->PIO_ABSR  &= ~(PIO_PA8 | PIO_PA9);
    
    // Configure UART settings
    UART->UART_CR   |= UART_CR_TXEN | UART_CR_RXEN;
    UART->UART_MR   |= UART_MR_PAR_NO;
    UART->UART_BRGR = cpufreq / (16 * baudrate);
    
    
    // Configure interrupts on receive ready and errors
    UART->UART_IDR = 0xFFFFFFFF;
    UART->UART_IER = UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME | UART_IER_PARE;

    // Enable UART interrupt in the Nested Vectored Interrupt Controller (NVIC)
    NVIC_EnableIRQ((IRQn_Type) ID_UART);
	UART->UART_IDR = 0xFFFFFFFF;
    
}    
*/
void uart_init(uint32_t cpufreq) {
	PMC->PMC_PCER0 |= (1 << ID_UART); // Enable the clock for UART0

	// Set UART0 pins (PA8 as TX, PA9 as RX)
	PIOA->PIO_PDR |= PIO_PA8 | PIO_PA9; // Disable PIO control on PA8 and PA9
	PIOA->PIO_ABSR &= ~(PIO_PA8 | PIO_PA9); // Select Peripheral A for PA8 and PA9 (UART0)
	
	// Configure UART0 settings
	UART->UART_CR = UART_CR_TXEN | UART_CR_RXEN; // Enable TX and RX
	UART->UART_MR = UART_MR_PAR_NO;              // No parity
	UART->UART_BRGR = cpufreq / (16 * baudrate); // Set baud rate

	// Disable all UART interrupts (we're focusing on direct TX only)
	UART->UART_IDR = 0xFFFFFFFF;
}

/*
void uart_tx(uint8_t val){
    while(!(UART->UART_SR & UART_SR_TXEMPTY)){}
    UART->UART_THR = val;
}
*/
void uart_tx(uint8_t val) {
	UART->UART_THR = val; // Directly write to TX holding register
}

uint8_t uart_rx(uint8_t* val, RingBuf *rb){
    return pop(rb, val);
}    

int uart_flush(char* buf, int len, RingBuf *rb){
    int r = 0;
    for(; r < len; r++){
        int ret = uart_rx((uint8_t*)&buf[r],rb);
        if(!ret){
            break;
        }                
    }    
    return r;
}    


void UART_Handler(){
    
    uint32_t status = UART->UART_SR;
    
    // Errors: Reset UART
    if(status & (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE)){
        UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN | UART_CR_RSTSTA;
    }
    
    // Receive ready: push to ring buffer
    if(status & UART_SR_RXRDY){
        if(!push(&rb, UART->UART_RHR & 0xff)){
            printf("UART receive buffer full\n");
        }
    }
    
} 



// See https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib

extern int _end;
#include <sys/stat.h>

void *_sbrk(int incr){
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if(heap == NULL){
        heap = (unsigned char*)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return prev_heap;
}

int _close(int file){
    return -1;
}

int _fstat(int file, struct stat* st){
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file){
    return 1;
}

int _lseek(int file, int ptr, int dir){
    return 0;
}

__attribute__((naked)) void _exit(int status){
    //__asm("BKPT #0");
}

void _kill(int pid, int sig){
    return;
}

int _getpid(void){
    return -1;
}

int _write(int file, char* ptr, int len){
    if(file > 1){
        return -1;    
    }

    for(int idx = 0; idx < len; idx++){
        uart_tx((uint8_t)ptr[idx]);
    }
    return len;
}

int _read(int file, char* ptr, int len, RingBuf *rb){
    if(file > 1){
        return -1;
    }
    
    int nread = 0;
    for(int idx = 0; idx < len; idx++){
        int b = uart_rx((uint8_t*)&ptr[idx], rb);
        nread += b;     
        if(!b){
            return nread;
        }        
    }
    return nread;
}

  