/*
 * util.h
 *
 * Created: 4/24/2016 9:20:03 AM
 *  Author: Ireena
 */ 


#ifndef UTIL_H_
#define UTIL_H_

#include <avr/io.h>


#define UART_SENDER_BUFFER_SZ 0x1F

#define SLAVE_PWM_PORT PORTB 
#define SLAVE_STEPPER_MOTOR_PORT PORTB
#define SLAVE_APPLIANCE_PORT PORTB
#define ROM_START_ADD_DEV_ID_W 0x0000
#define ROM_MAGIC_NUMBER 0x44
#define MAX_SLAVE_DEVICES 0x0A 
#define NUM_RETRIES_FIRST_HEARTBEAT 0x1000  
#define ZIGBEE_TRANSMIT_STATUS_RESPONSE_SIZE 0x0A
#define ZIGBEE_TRANSMIT_STATUS_REQUEST_SIZE  0xF9



struct command_types
{
	uint8_t ack;
	uint8_t nack;
	uint8_t appliance;
	uint8_t stepper_motor;
	uint8_t pwm;
	uint8_t thermostat;
	uint8_t reset_avr;
	uint8_t heartbeat;
	uint8_t send_slave_hb;
} ;


#endif /* UTIL_H_ */