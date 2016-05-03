/*
* xbee_api_mode.h
*
* Created: 4/27/2016 7:21:41 AM
*  Author: Ireena
*/


#ifndef XBEE_API_MODE_H_
#define XBEE_API_MODE_H_

#include "util.h"

struct xbee_at_command_base{
	uint8_t start_delimiter ;
	uint8_t msb ;
	uint8_t lsb ;
	uint8_t frame_type;
	uint8_t frame_id;
	uint8_t at_command_char1 ;
	uint8_t at_command_char2;
} base;



struct xbee_at_command_request{
	struct xbee_at_command_base base;
	uint8_t checksum;
	
};
struct xbee_at_command_request _at_cmd_request ;
struct xbee_at_command_request *_p_xb_cmd_req = &_at_cmd_request;

void init_at_cmd_request(){
	_p_xb_cmd_req->base.frame_id = 0x7E;
	_p_xb_cmd_req->base.msb = 0x00;
	_p_xb_cmd_req->base.lsb = 0x04;
	_p_xb_cmd_req->base.frame_type = 0x08;
}


struct xbee_at_command_response{
	struct xbee_at_command_base base;
	uint8_t at_command_status;
	uint8_t checksum;
};
struct xbee_at_command_response _at_cmd_response ;
struct xbee_at_command_response *_p_xb_cmd_res = &_at_cmd_response;

struct destination_address_64{
	uint8_t address[8];
};

struct destination_address_16{
	uint8_t address[2];
};

struct rf_data{
	uint8_t data[8];
};

struct tx_req_data
{
	uint8_t tx_data[UART_SENDER_BUFFER_SZ];
};

struct zigbee_transmit_request{
	struct destination_address_64 address_64;
	struct rf_data data ;
	struct tx_req_data tx_data;
	struct  destination_address_16 address_16;
	uint8_t start_delimiter;
	uint8_t msb;
	uint8_t lsb;
	uint8_t frame_type;
	uint8_t frame_id;
	uint8_t broadcast_radius;
	uint8_t options;
	uint8_t checksum;
	
};

struct zigbee_transmit_request _zb_tx_req;
struct zigbee_transmit_request *_p_zb_tx_req = &_zb_tx_req;

struct zigbee_transmit_status{
	struct destination_address_16 address_16;
	uint8_t start_delimiter;
	uint8_t msb ;
	uint8_t lsb;
	uint8_t frame_type;
	uint8_t frame_id;
	uint8_t transmit_retry_count ;
	uint8_t delivery_status;
	uint8_t discovery_status;
	uint8_t checksum;
	
};

struct zigbee_transmit_status _zb_tx_status;
struct zigbee_transmit_status *_p_zb_tx_status = &_zb_tx_status;


void init_zb_tx_request(){
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

#endif /* XBEE_API_MODE_H_ */