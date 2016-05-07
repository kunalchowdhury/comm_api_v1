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
} ;

struct xbee_at_command_request{
	struct xbee_at_command_base base;
	uint8_t checksum;
	
};

void init_at_cmd_request(struct xbee_at_command_request *_p_xb_cmd_req, uint8_t command_char1, uint8_t command_char2);

struct xbee_at_command_response{
	struct xbee_at_command_base base;
	uint8_t at_command_status;
	uint8_t checksum;
};

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

void init_zb_tx_request(struct zigbee_transmit_request *_p_zb_tx_req);
uint8_t * serial_payload(struct zigbee_transmit_request * ptr);

#endif /* XBEE_API_MODE_H_ */