
/*
 * spi_master_read_write.s
 *
 * Created: 4/25/2016 7:45:41 AM
 *  Author: Ireena
 */ 
#define _SFR_ASM_COMPAT 1 
#define __SFR_OFFSET 0          // Use 0 for the I/O register offset
#include <avr/io.h>             // Defines I/O port aliases
  
.global read_data				; Makes readData visible in other source files
.global write_data				; Makes writeData visible in other source files


.section .text                  ; Defines a code section

.equ MOSI, 12				;	// ATMEGA128p MOSI pin
.equ SCK, 11				;	// ATMEGA128p SCK pin
.equ SS, 10					;	// ATMEGA128P SS pin

read_data:
	RCALL prepare_for_spi_comm
	wait_to_receive:
			SBIS SPSR, SPIF
			RJMP wait_to_receive
			IN R24, SPDR				; PASS VALUE TO R24 FROM DATA REGISTER
			SBI PORTB, SS				; DISABLE SS 				
	RET
	
write data:
	RCALL prepare_for_spi_comm
	CBI PORTB, SS
	OUT SPDR,  R24
	RET
	
prepare_for_spi_comm:
	LDI R17, (1 << MOSI) | (1 << SCK) | (1 << SS)   ; only MOSI SCK and SS on PORTB set to 1
	OUT DDRB, R17

	LDI R17, (1 << SPE) | (1 << MSTR) | (1 << SPR0) ; enable SPI master mode, CLK = fck/16
	OUT SPCR, R17
	RET