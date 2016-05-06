/*
 * slave_apis.c
 *
 * Created: 5/5/2016 11:17:15 AM
 *  Author: Ireena
 */ 
#include <avr/io.h>
#include "header/slave_api.h"

uint16_t *get_self_xbee_16_id()
{
	
}

bool is_valid_zb_req(uint8_t *req, uint16_t address_16)
{
	uint8_t offset = ZIGBEE_TRANSMIT_STATUS_REQUEST_SIZE -1;
	uint8_t checksum = 0;
	while (offset)
	{
		switch(offset)
		{
			case 0:
			if(req[offset] != 0x7E)
			{
				return false;
			}
			break;
			case 1:
			if(req[offset] != 0x00)
			{
				return false;
			}
			break;
			case 2:
			if(req[offset] != 0x16)
			{
				return false;
			}
			break;
			case 3:
			if (req[offset] != 0x10)
			{
				return false;
			}
			break;
			case 4:
			if (req[offset] != 0x01)
			{
				return false;
			}
			break;
			case 13:
			if (req[offset] != (uint8_t)(address_16 >> 8))
			{
				return false;
			}
			break;
			case 14:
			if (req[offset] != (uint8_t)(address_16))
			{
				return false;
			}
			break;
			case 25:
			if(req[offset] == (0xFF - (checksum & 0xFF)))
			{
				return false;
			}
			break;
			
		}
		if(offset >= 3)
		{
			checksum += req[offset];
		}
		--offset;
	}
	return true;
}

