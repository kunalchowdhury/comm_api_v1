/*
 * serial_sender_receiver.h
 *
 * Created: 4/27/2016 7:06:49 AM
 *  Author: Ireena
 */ 


#ifndef SERIAL_SENDER_RECEIVER_H_
#define SERIAL_SENDER_RECEIVER_H_

#include <avr/io.h>

/************************************************************************/
/* API to get the defined buffer                                        */
/************************************************************************/
uint8_t * get_serial_buffer();

/************************************************************************/
/* API to send an array of characters over the serial port              */
/************************************************************************/
void tx_serial(const uint8_t *);

/************************************************************************/
/* API to receive an array of characters over the serial port           */
/************************************************************************/
uint8_t * rx_serial();


/************************************************************************/
/* API to send a single character over the serial port                  */
/************************************************************************/
void tx_serial_single(const uint8_t);


/************************************************************************/
/* API to receive a single character over the serial port               */
/************************************************************************/
uint8_t rx_serial_single();

#endif /* SERIAL_SENDER_RECEIVER_H_ */