/*
* spi_sender_receiver.c
*
* Created: 4/24/2016 8:42:22 PM
*  Author: Ireena
*/

#include "cmd.h"
#include <avr/io.h>

#define MOSI 3
#define SCK 5
#define SS 2

void init_spi(){
	DDRB = (1 << MOSI) | (1 << SCK) | (1 << SS) ;
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) ;
}

uint8_t get_cmd()
{
	init_spi();
	uint8_t ch;
	while(!(SPSR & (1 << SPIF)));
	ch = SPDR;
	return ch;
}

void send_cmd(uint8_t cmd)
{
	init_spi();
	SPDR = cmd;
	while(!(SPSR & (1 << SPIF)));
	
}