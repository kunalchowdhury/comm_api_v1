/*
 * api_ref.h
 *
 * Created: 4/24/2016 9:11:46 AM
 *  Author: Ireena
 */ 

#ifndef API_REF_H_
#define API_REF_H_

#include "util.h"
#include <avr/io.h>


/************************************************************************/
/* API used by coordinator to discover slaves                           */
/************************************************************************/
void send_slave_msg(uint8_t c, uint8_t cmd_type);


/************************************************************************/
/* API used by coordinator to request device id                         */
/************************************************************************/
void request_slave_device_id();

/************************************************************************/
/* API to send heartbeat to the master coordinator
at a periodic interval
*/
/************************************************************************/

bool send_heartbeat();

/************************************************************************/
/* API to execute PWM at the host device                                */
/************************************************************************/

void exec_pwm();

/************************************************************************/
/* API to rotate stepper motor                                          */
/************************************************************************/

void exec_stepper_motor();

/************************************************************************/
/* API to run an appliance at given port(s), the user INPUT should wrap 
the PORT statuses i.e. which ones to enabled disable						*/
/************************************************************************/
bool exec_appliance(bool on, uint8_t *port_status);

/************************************************************************/
/* API to send a reply indicating whether the execution was successful  */
/************************************************************************/
bool send_reply();




#endif /* API_REF_H_ */