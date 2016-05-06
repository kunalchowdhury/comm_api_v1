/*
 * main_slave.c
 *
 * Created: 5/4/2016 8:14:56 AM
 *  Author: Ireena
 */ 
#include <avr/io.h>
#include "header/serial_sender_receiver.h"
#include "header/util.h"

volatile uint16_t address_16 =0;

int main1(void)
{
   while(1)
   {
	 uint8_t *work_request = rx_serial();
	 if(address_16)
         address_16 = get_self_xbee_16_id(); 	 
	 if(is_valid_zb_req(work_request, address) )
   }
	
}
