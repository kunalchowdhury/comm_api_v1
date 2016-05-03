/*
 * fsm.h
 *
 * Created: 5/1/2016 1:49:46 PM
 *  Author: Ireena
 */ 


#ifndef FSM_H_
#define FSM_H_
#include <avr/io.h>
#include "util.h"

/************************************************************************/
/* API to execute the coordinator state machine							*/
/************************************************************************/
void master_exec();


/************************************************************************/
/* API to execute the slave state machine								*/
/************************************************************************/
void slave_exec();

/************************************************************************/
/* API to write addresses to EEPROM if not loaded already               */
/************************************************************************/
void write_address_to_eeprom(uint8_t size, 
                             uint16_t *xbee_addr, 
							 uint8_t *cmds, 
							 bool first) ;

/************************************************************************/
/* API to decide whether there are any  16bit addresses in EEPROM       */
/************************************************************************/
bool is_magic_number_present();

/************************************************************************/
/* API to initialize EEPROM read			                            */
/************************************************************************/
void init_eeprom_read(uint16_t address);

#endif /* FSM_H_ */