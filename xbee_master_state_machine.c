/*
* xbee_master_state_machine.c
*
* Created: 5/1/2016 1:49:13 PM
*  Author: Ireena
*/


#include "fsm.h"
#include "api_ref.h"
#include "serial_sender_receiver.h"
#include "xbee_message.h"
#include "util.h"

struct command_types _command_types = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
struct zigbee_transmit_request _zb_tx_req;
struct zigbee_transmit_request *_p_zb_tx_req = &_zb_tx_req;


volatile uint8_t eeprom_start_byte;

uint8_t eeprom_read_buffer[MAX_SLAVE_DEVICES+1];

void init_zb_tx_request(struct zigbee_transmit_request *_p_zb_tx_req){
	_p_zb_tx_req->start_delimiter = 0x70;
	_p_zb_tx_req->msb = 0x00;
	_p_zb_tx_req->lsb = 0x16;
	_p_zb_tx_req->frame_type= 0x10;
	_p_zb_tx_req->broadcast_radius = 0x00;
}


uint8_t * serial_payload(struct zigbee_transmit_request * ptr)
{
	ptr->tx_data.tx_data[0x00] = ptr->start_delimiter ;
	ptr->tx_data.tx_data[0x01] = ptr->msb ;
	ptr->tx_data.tx_data[0x02] = ptr->lsb ;
	ptr->tx_data.tx_data[0x03] = ptr->frame_type ;
	ptr->tx_data.tx_data[0x04] = ptr->frame_id ;
	for (uint8_t c = 0x05 ; c < 0x0D ; c++)
	{
		ptr->tx_data.tx_data[c] = ptr->address_64.address[0x07-(c-0x05)];
	}
	for (uint8_t c = 0x0D ; c < 0x0F ; c++)
	{
		ptr->tx_data.tx_data[c] = ptr->address_16.address[0x01-(c-0x0D)];
	}
	ptr->tx_data.tx_data[0x0F] = 0x00;
	ptr->tx_data.tx_data[0x10] = 0x00;
	for (uint8_t c = 0x11 ; c < 0x19 ; c++)
	{
		ptr->tx_data.tx_data[c] = ptr->data.data[0x07-(c-0x11)];
	}
	ptr->tx_data.tx_data[0x19] = ptr->checksum;
	
	return ptr->tx_data.tx_data ;
}


/************************************************************************/
/* Broadcast API											            */
/************************************************************************/

void send_slave_msg(uint8_t c, uint8_t cmd_type)
{
	init_zb_tx_request(_p_zb_tx_req);
	_p_zb_tx_req->frame_id = 0x01;
	_p_zb_tx_req->address_64.address[1] = 0xFF;
	_p_zb_tx_req->address_64.address[0] = 0xFF;
	_p_zb_tx_req->address_16.address[1] = eeprom_read_buffer[c];
	_p_zb_tx_req->address_16.address[0] = eeprom_read_buffer[c+1];
	_p_zb_tx_req->options = 0x00;
	_p_zb_tx_req->data.data[0] = cmd_type ;
	 uint8_t _msg_checksum = (0x70 + 0x16+ 0x10 + 0x01 +  0xFF + 0xFF + 0xFF + 0xFE + 0x09) & 0xFF;
	_p_zb_tx_req->checksum = 0xFF - _msg_checksum ;
	 tx_serial(serial_payload(_p_zb_tx_req));
}

bool read_slave_addresses()
{
	uint8_t sum = 0;
	uint8_t value= 0;
	init_eeprom_read(ROM_START_ADD_DEV_ID_W);
	value = EEDR;
	if(value != ROM_MAGIC_NUMBER)
	{
		return false;
	}
	sum += ROM_MAGIC_NUMBER ;
	init_eeprom_read(ROM_START_ADD_DEV_ID_W+1);
	value = EEDR;
	eeprom_read_buffer[0] = value;
	sum += value;
	for (int i = 1 ;  i <= value+1; i++)
	{
		init_eeprom_read(ROM_START_ADD_DEV_ID_W+1+i);
		if(i <= value){
			eeprom_read_buffer[i] = EEDR;
		}
		sum += value ;
	}
	if(sum == 0)
	{
		return true;
	}
	return false;
	
}
/************************************************************************/
/* This should be a zig bee transmit status response - MSB FIRST		*/
/************************************************************************/
bool slave_ok(uint8_t *reply, uint8_t slave_low_address, uint8_t slave_high_address )
{
	/************************************************************************/
	/* Not a valid ZB delimiter                                             */
	/************************************************************************/
	if(reply[0] != 0x7E)
	{
		return false;
	}
	
	uint8_t limit = 3 + (reply[2] - reply[1]) ; 
	uint8_t offalset = 3;
	uint8_t checksum = 0;
	while(offalset <= limit)
	{
		switch(offalset)
		{
			case 3:
			if(reply[offalset] != 0x8B)
			{
				return false;
			}
			break;
			case 4:
			if(reply[offalset] != 0x01)
			{
				return false;
			}
			break;
			case 5:
			if(reply[offalset] != slave_low_address) // check if this is msb or lsb
			{
				return false;
			}
			break;
			case 6:
			if(reply[offalset] != slave_high_address) // check if this is msb or lsb
			{
				return false;
			}
			break;
			case 8:
			if(reply[offalset] != 0x00) 
			{
				return false;
			}
			break;
			case 9:
			if(reply[offalset] != 0x01)
			{
				return false;
			}
			break;
			case 10:
			if(reply[offalset] == (0xFF - (checksum & 0xFF)))
			{
				return false;
			}
			break;
		}
		
		checksum += reply[offalset];
		++offalset;
	}
	return true;
}
void master_exec()
{
	if(read_slave_addresses() == true)
	{
		for(uint8_t c = 1; c < eeprom_read_buffer[1] ; c +=2)
		{
			/************************************************************************/
			/* SEND INITIAL REQUEST TO SLAVE										*/
			/************************************************************************/
            send_slave_msg(c, _command_types.send_slave_hb);
			uint8_t counter =0 ;
			bool slave_heartbeating = false;
			/************************************************************************/
			/* KEEP RETRYING TILL MAX_RETRIES_FIRST_HEARTBEAT                       */
			/************************************************************************/
			while(counter++ < NUM_RETRIES_FIRST_HEARTBEAT)
			{
				uint8_t* reply = rx_serial();
				
			/************************************************************************/
			/*  IF REPLY RECEIVED                                                   */
			/************************************************************************/	
				if(slave_ok(reply, eeprom_read_buffer[c], eeprom_read_buffer[c+1]) == true)
				{
					slave_heartbeating = true;
			/************************************************************************/
			/*  SLAVE HEARTBEATING FINE, SEND ACK TO SLAVE                          */
			/************************************************************************/
					send_slave_msg(c, _command_types.ack);
					break;
				}
				
			}
			
			/************************************************************************/
			/*  SEND MESSAGE TO GATEWAY ABOUT THE STATUS                            */
			/************************************************************************/
			if(slave_heartbeating == false)
			{
				// send message to gateway telling slave is down
			}
			else
			{
				
			}
			
		}
		
	}
}

bool is_magic_number_present()
{
	init_eeprom_read(ROM_START_ADD_DEV_ID_W);
	eeprom_start_byte = EEDR ;
	if(eeprom_start_byte == ROM_MAGIC_NUMBER)
	{
		return true ;
	}
	return false; 
	
}

void eeprom_write(uint16_t address, uint8_t value)
{
	while(EECR & (1 << EEPE));
	EEAR = address;
	EEDR = value ;
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
	return;
}

void init_eeprom_read(uint16_t address)
{
	while(EECR & (1 << EEPE));
	EEAR  = address ;
	EECR |= 1 << EERE;
}

void write_address_to_eeprom(uint8_t size, uint16_t *xbee_addr, uint8_t* cmd, bool first)
{
	uint8_t sum = 0;
	uint8_t chk_sum_byte = 0;
	eeprom_write(ROM_START_ADD_DEV_ID_W, ROM_MAGIC_NUMBER);
	eeprom_write(ROM_START_ADD_DEV_ID_W + 1, size);
	uint8_t c = 1;
	sum = ROM_MAGIC_NUMBER + size ;
	while(--size)
	{
		uint16_t address = *xbee_addr;
		uint8_t high = (uint8_t)(address >> 8);
		uint8_t low = (uint8_t)(address);
		eeprom_write(ROM_START_ADD_DEV_ID_W+1+c, high);
		eeprom_write(ROM_START_ADD_DEV_ID_W+1+c+1, low);
		sum += high + low ;
		++xbee_addr;
		++cmd;
		c += 2;
		
	}
	chk_sum_byte = ~sum + 1;
	eeprom_write(ROM_START_ADD_DEV_ID_W+2*size+2, chk_sum_byte);
}
