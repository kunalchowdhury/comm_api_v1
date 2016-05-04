/*
* appliance_slave.c
*
* Created: 5/4/2016 6:48:42 AM
*  Author: Ireena
*/

#include "header/api_ref.h"
#include <avr/io.h>

#define APPLIANCE_PORT_SIZE 3

static inline bool request_valid(uint8_t *req)
{
     return false ;	
}

static inline bool exec(uint8_t *port_status, int flag)
{
	for (int c = 0 ; c < APPLIANCE_PORT_SIZE ; c++)
	{
		switch(c)
		{
			case 0:
				DDRB = flag;
				PORTB = port_status[c];
			    return true;
			case 1:
				DDRC = flag;
				PORTC = port_status[c];
			    return true;
			case 2:
			    DDRD = flag ;
			    PORTD = port_status[c];
			    return true;
			default:
			    return false;
		}
	}
	return false;
}

bool exec_appliance(bool on, uint8_t *port_status)
{
	if(on == true)
	{
		return exec(port_status, 0xFF);
	}
	else
	{
		return exec(port_status, 0);
		
	}
	
}

int main(void)
{
     while(1)
     {
     	uint8_t *work_order = rx_serial();
     	if(request_valid(work_order) == true)
     	{
     		
     	}
     }
	
}
