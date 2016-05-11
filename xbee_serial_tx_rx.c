/*
 * xbee_serial_tx_rx.c
 *
 * Created: 4/30/2016 1:37:55 PM
 *  Author: Ireena
 */ 

#include "serial_sender_receiver.h"
#include <avr/io.h>
#include "util.h"
#include <stdbool.h>

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

uint8_t buffer[UART_SENDER_BUFFER_SZ];

uint8_t * get_serial_buffer()
{
	return buffer;
}

// baud rate = 9600, 8 bit data, 1 stop bit
void uart_init(bool send){
	if(send == true)
	{
		UCSR0B = (1 << TXEN0);
	}else
	{
		UCSR0B = (1 << RXEN0);
	}
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8) ;
	UBRR0L = (uint8_t) UBRR_VALUE; 
}

void send(uint8_t ch)
{
	while(! (UCSR0A & (1 << UDRE0))) ;
	UDR0 = ch;
}

void tx_serial(const uint8_t * ptr){
	uart_init(true);
	uint8_t sz = UART_SENDER_BUFFER_SZ -1;
	ptr = ptr +sz ;
	while(sz--)
	{
		send(*ptr);
		ptr--;
	}
}

void tx_serial_single(const uint8_t c)
{
	uart_init(true);
	send(c);
}

uint8_t *rx_serial()
{
	uart_init(false);
	uint8_t sz = UART_SENDER_BUFFER_SZ -1;
	while(sz)
	{
		while(!(UCSR0A & (1 << RXC0)));
		buffer[sz] = UDR0;
		sz--;
	}
	return (uint8_t *)buffer;
}

uint8_t rx_serial_single()
{
	uart_init(false);
	uint8_t ch ;
	while(!(UCSR0A & (1 << RXC0)));
	ch = UDR0;
	return ch;
}


