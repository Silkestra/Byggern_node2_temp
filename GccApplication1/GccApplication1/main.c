/*
 * GccApplication1.c
 *
 * Created: 09.10.2024 21:26:59
 * Author : augusal
 */ 

#include "sam.h"  // Include the SAM3X8E specific header

#define F_CPU 84000000     // 84 MHz for Arduino Due
#define BAUDRATE 9600      // Define the desired baud rate

void uart_init(uint32_t baudrate) {
	// Enable the UART clock
	PMC->PMC_PCER0 |= (1 << ID_UART);  // Power up UART peripheral clock

	// Configure UART pins (TX = PA9, RX = PA8)
	PIOA->PIO_PDR |= PIO_PA8 | PIO_PA9;   // Disable PIO control for PA8, PA9 (UART)
	PIOA->PIO_ABSR &= ~(PIO_PA8 | PIO_PA9); // Select Peripheral A function for PA8 and PA9

	// Configure UART baud rate
	UART->UART_BRGR = F_CPU / (16 * baudrate); // Set Baud Rate Generator

	// Configure UART mode: No parity, normal mode
	UART->UART_MR = UART_MR_PAR_NO;

	// Enable UART transmitter and receiver
	UART->UART_CR = UART_CR_TXEN | UART_CR_RXEN;
}

void uart_tx(uint8_t data) {
	// Wait until the transmitter is ready
	while (!(UART->UART_SR & UART_SR_TXEMPTY)) {
		// Wait for the TX buffer to be empty
	}
	// Load the data into the Transmit Holding Register
	UART->UART_THR = data;
}

void uart_send_string(const char* str) {
	// Transmit each character in the string
	while (*str) {
		uart_tx(*str++);
	}
}

int main(void) {
	// Initialize the system (set clocks, etc.)
	SystemInit();

	// Initialize UART
	uart_init(BAUDRATE);

	// Main loop
	while (1) {
		// Send a test string over UART
		uart_send_string("Hello, World!\n");

		// Delay between transmissions
		for (volatile int i = 0; i < 1000000; i++);  // Simple delay loop
	}
}
