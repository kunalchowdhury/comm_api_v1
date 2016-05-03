/*
 * gateway_message.h
 *
 * Created: 5/3/2016 11:02:11 AM
 *  Author: Ireena
 */ 


#ifndef GATEWAY_MESSAGE_H_
#define GATEWAY_MESSAGE_H_
#include <avr/io.h>
#include "header/util.h"

struct gateway_message
{
	uint8_t gateway_msg_identifier ;
	uint8_t device_id_high;
	uint8_t device_id_low;
	uint8_t command ;
	uint8_t session_id;
	
};



#endif /* GATEWAY_MESSAGE_H_ */
