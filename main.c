/*
* main_slave.c
*
* Created: 5/4/2016 8:14:56 AM
*  Author: Ireena
*/
#include <avr/io.h>
#include "serial_sender_receiver.h"
#include "util.h"
#include "api_ref.h"
#include "slave_api.h"
#include <stdio.h>
volatile uint16_t address_16 =0;

int main(void)
{
	//while(1){
		address_16 = get_self_xbee_16_id();
		//printf("address  = %d ", address_16);
		//for (int i = 0 ; i < 100000; i++)	{}
		
	//}
	
	
	//while(1)
	//{
	//uint8_t *work_request = rx_serial();
	//if(address_16)
	//{
	//address_16 = get_self_xbee_16_id();
	//}
	//uint8_t port_status[3] = {0x03, 0x02, 0};
	//if(is_valid_zb_req(work_request, address_16) == true)
	//{
	//exec_appliance(true, port_status);
	//}
	//}
	//
}
