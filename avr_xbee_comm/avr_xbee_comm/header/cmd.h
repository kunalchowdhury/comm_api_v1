/*
* cmd.h
*
* Created: 4/24/2016 7:48:29 AM
*  Author: Ireena
*/


#ifndef CMD_H_
#define CMD_H_
#include <avr/io.h>

/************************************************************************/
/* API to interpret the SPI command
Every SPI communication should wrap a command of 7 bits. Following
constitutes the meaning of these commands

A. 0x01 = ACK
B. 0x02 = NACK
C  0x03 = CONNECT TRIVIAL APPLIANCE
D. 0x04 = CONNECT STEPPER MOTOR
E. 0x05 = PWM
F. 0x06 = THERMOSTAT
G. 0x07 = RESET AVR
H. 0x08 = HEARTBEAT MESSAGE OF SLAVE

*/
/************************************************************************/
uint8_t get_cmd();

/************************************************************************/
/*  API to send a command over SPI.

The command can be any one of those described above.Note - apart from the 
command the HIGHER BITS would also supply additional information about the 
additional values that the command needs to execute.

*/
/************************************************************************/

void send_cmd(uint8_t cmd);

#endif /* CMD_H_ */