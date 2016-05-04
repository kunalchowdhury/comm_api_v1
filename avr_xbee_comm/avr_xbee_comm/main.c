/*
 * avr_xbee_comm.c
 *
 * Created: 4/27/2016 10:41:36 AM
 * Author : Ireena
 */ 

#include <avr/io.h>
#include "header/serial_sender_receiver.h"
#include "header/util.h"
#include "header/fsm.h"


int main(void)
{
	uint16_t address[] = {0x2121, 0x4310, 0x5620, 0x8909};
	uint8_t  cmds[]	   = {0x05, 0x06, 0x08, 0x09};
		
	if(is_magic_number_present() == false)
	{
		write_address_to_eeprom(4, address, cmds, true);
	}
	master_exec();
    while (1)
    {
		/************************************************************************/
		/* This serial message maybe (1) from gateway or (2) from slaves             */
		/************************************************************************/
		//uint8_t * message = rx_serial();
		
		
    }
}

