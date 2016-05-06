/*
 * slave_apis.c
 *
 * Created: 5/5/2016 11:17:15 AM
 *  Author: Ireena
 */ 
#include <avr/io.h>
#include "header/slave_api.h"
#include "header/serial_sender_receiver.h"
#include "header/util.h"

struct xbee_at_command_request _at_command_req ;
struct xbee_at_command_request *_p_at_command_req = &_at_command_req;
volatile uint8_t my_address_16_L;
volatile uint8_t my_address_16_H;

static inline uint8_t checksum()
{
	uint8_t _checksum = _p_at_command_req->base.start_delimiter + 
	                    _p_at_command_req->base.msb+_p_at_command_req->base.lsb
	                    + _p_at_command_req->base.frame_type 
						+ _p_at_command_req->base.frame_id
						+_p_at_command_req->base.at_command_char1
						+_p_at_command_req->base.at_command_char2 ;
	return 0xFF - (_checksum & 0xFF);
}

static inline void init_at_cmd_request(){
	_p_at_command_req->base.start_delimiter = 0x7E;
	_p_at_command_req->base.msb = 0x00;
	_p_at_command_req->base.lsb = 0x04;
	_p_at_command_req->base.frame_type = 0x08;
	_p_at_command_req->base.frame_id= 0x01;
	_p_at_command_req->base.at_command_char1='M';
	_p_at_command_req->base.at_command_char2='Y';
	_p_at_command_req->checksum = checksum();
}

void init_for_transmit(uint8_t * buff)
{
	buff[0x00] = _p_at_command_req->base.start_delimiter ;
	buff[0x01] = _p_at_command_req->base.msb;
	buff[0x02] = _p_at_command_req->base.lsb;
	buff[0x03] = _p_at_command_req->base.frame_type;
	buff[0x04] = _p_at_command_req->base.frame_id;
	buff[0x05] = _p_at_command_req->base.at_command_char1;
	buff[0x06] = _p_at_command_req->base.at_command_char2;
	buff[0x07] = _p_at_command_req->checksum;
	
}

bool valid_at_resp(uint8_t * resp, uint8_t command_char_1, uint8_t command_char_2)
{
	if(resp[0] != 0x7E)
	{
		return false;
	}
	uint8_t limit = 3 + (resp[2] - resp[1]);
	uint8_t offset = 3;
	uint8_t checksum = 0;
	while(offset <= limit)
	{
		switch(offset)
		{
			case 3:
			if(resp[offset] != 0x88)
			{
				return false;
			}
			break;
			case 4:
			if(resp[offset] != 0x01)
			{
				return false;
			}
			break;
			case 5:
			if(resp[offset] != command_char_1)
			{
				return false;
			}
			break;
			case 6:
			if(resp[offset] != command_char_2)
			{
				return false;
			}
			break;
		    case 7:
			if(resp[offset] != 0x00)
			{
				return false;
			}
			break;

		}
		
		
	   checksum += resp[offset];	
	   ++offset;
	}
	return true;
}


uint16_t *get_self_xbee_16_id()
{
	init_at_cmd_request();
    uint8_t * buff = get_serial_buffer();
	/************************************************************************/
	/* Reset the buffer before transmitting                                 */
	/************************************************************************/
	for(uint8_t i= 0 ; i < UART_SENDER_BUFFER_SZ ; i++)
	{
		buff[i] =0;
	}
	tx_serial(buff);
	uint8_t retry = 0;
	while(++retry < NUM_RETRIES_FIRST_HEARTBEAT)
	{
		uint8_t *resp = rx_serial();
		/************************************************************************/
		/* Validate AT command response                                         */
		/************************************************************************/
		if(valid_at_resp(resp, 'M', 'Y') == true)
		{
			// ?? how to retrieve??
			break;
		}
	}
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

