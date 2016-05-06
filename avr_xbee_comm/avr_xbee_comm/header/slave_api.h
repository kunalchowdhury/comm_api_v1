/*
 * slave_api.h
 *
 * Created: 5/5/2016 11:30:25 AM
 *  Author: Ireena
 */ 


#ifndef SLAVE_API_H_
#define SLAVE_API_H_
#include "util.h"



/*********************************************************/
/* API for the Slave to validate the incoming ZB request */
/*********************************************************/
bool is_valid_zb_req(uint8_t *req, uint16_t address_16) ;


/************************************************************************/
/* API to get the 16 bit address of the current XBEE slave              */
/************************************************************************/
uint16_t *get_self_xbee_16_id();


#endif /* SLAVE_API_H_ */