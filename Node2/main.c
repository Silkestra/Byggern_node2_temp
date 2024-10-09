/*
 * Node2.c
 *
 * Created: 09.10.2024 17:34:00
 * Author : augusal
 */ 
#define F_CPU 84000000     // 84 MHz for Arduino Due
    // Define the desired baud rate
#include "uart.h"
#include "sam.h"

RingBuf rb;
int main(void) {
	uart_init(F_CPU); // Initialize UART
	 ringbuf_init(&rb);
	// Transmit "Hello, UART!" in a loop
	char *testString = "Hello, UART!\n";
	int i = 0;

	while (1) {
		if (testString[i] != '\0') {
			uart_tx(testString[i]);
			i++;
			for (volatile int j = 0; j < 100000; j++); 
			} 
		else {
			i = 0; // Reset the index to send the string again
		}
	}
	}

































/*
#include "sam.h"



#define F_CPU 84000000UL


void delay_ms(uint32_t ms) {
	volatile uint32_t count;
	while (ms--) {
		// Assuming 1 iteration takes ~1 microsecond at F_CPU = 84 MHz
		count = (F_CPU / 14000); // Adjust this constant for your clock speed
		while (count--);
	}
}


void set_PB13_high() {
	// Enable the peripheral clock for PIOB
	

	// Enable PIO control of PB13 (using the Peripheral Enable Register)
	REG_PIOB_PER = (1 << 13); // Enable control over PB13 (Peripheral Enable Register)
	
	// Set PB13 as an output
	REG_PIOB_OER = (1 << 13); // Output Enable Register

	// Set PB13 high
	REG_PIOB_SODR = (1 << 13); // Set Output Data Register
}

void set_PB13_low() {
	// Enable the peripheral clock for PIOB
	

	// Enable PIO control of PB13 (using the Peripheral Enable Register)
	REG_PIOB_PER = (1 << 13); // Enable control over PB13 (Peripheral Enable Register)
	
	// Set PB13 as an output
	REG_PIOB_OER = (1 << 13); // Output Enable Register

	// Set PB13 low
	REG_PIOB_CODR = (1 << 13); // Clear Output Data Register to set PB13 low
}

int main(void)
{
 
    while (1) 
    {
		set_PB13_low();
		delay_ms(100);
		set_PB13_high();
		delay_ms(100);
		
    }
}
*/