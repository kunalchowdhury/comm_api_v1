#include <api_ref.h>
#include <cmd.h>
#include <fsm.h>
#include <gateway_message.h>
#include <serial_sender_receiver.h>
#include <slave_api.h>
#include <util.h>
#include <xbee_message.h>

#define F_CPU_1 16000000
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU_1 / (USART_BAUDRATE * 16UL))) - 1)
#define UART_SENDER_BUFFER_SZ 0x1F

uint8_t buffer[UART_SENDER_BUFFER_SZ];
volatile uint8_t sz = 0;

struct zigbee_transmit_request _zb_tx_req_arduino;
struct zigbee_transmit_request *_p_zb_tx_req_arduino = &_zb_tx_req_arduino;

void init_zb_tx_request(struct zigbee_transmit_request *_p_zb_tx_req){
  _p_zb_tx_req->start_delimiter = 0x7E;
  _p_zb_tx_req->msb = 0x00;
  _p_zb_tx_req->lsb = 0x16;
  _p_zb_tx_req->frame_type= 0x10;
  _p_zb_tx_req->broadcast_radius = 0x00;
}

uint8_t * serial_payload(struct zigbee_transmit_request * ptr)
{
  uint8_t checksum = 0;
  ptr->tx_data.tx_data[0x00] = ptr->start_delimiter ;
  ptr->tx_data.tx_data[0x01] = ptr->msb ;
  ptr->tx_data.tx_data[0x02] = ptr->lsb ;
  
  ptr->tx_data.tx_data[0x03] = ptr->frame_type ; 
  checksum += ptr->tx_data.tx_data[0x03] ;
  
  ptr->tx_data.tx_data[0x04] = ptr->frame_id ;
  ptr->tx_data.tx_data[0x04] ;
  
  for (uint8_t c = 0x05 ; c < 0x0D ; c++)
  {
    ptr->tx_data.tx_data[c] = ptr->address_64.address[0x07-(c-0x05)];
    checksum += ptr->tx_data.tx_data[c]  ;
  }
  for (uint8_t c = 0x0D ; c < 0x0F ; c++)
  {
    ptr->tx_data.tx_data[c] = ptr->address_16.address[0x01-(c-0x0D)];
    checksum += ptr->tx_data.tx_data[c]  ;
  
  }
  ptr->tx_data.tx_data[0x0F] = 0x00;
  checksum += ptr->tx_data.tx_data[0x0F]  ;
  
  ptr->tx_data.tx_data[0x10] = 0x00;
  checksum += ptr->tx_data.tx_data[0x10]  ;
  
  for (uint8_t c = 0x11 ; c < 0x19 ; c++)
  {
    ptr->tx_data.tx_data[c] = ptr->data.data[0x07-(c-0x11)];
    checksum += ptr->tx_data.tx_data[c]  ;
  }
  ptr->tx_data.tx_data[0x19] = checksum;
  
  return ptr->tx_data.tx_data ;
}

void uart_init(bool send){
  if(send)
  {
    UCSR0B = (1 << TXEN0);
  }else
  {
    UCSR0B = (1 << RXEN0);
  }
  
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  UBRR0H = (uint8_t)(UBRR_VALUE >> 8) ;
  UBRR0L = (uint8_t) UBRR_VALUE; 
}

void send(uint8_t ch)
{
  while(! (UCSR0A & (1 << UDRE0))) ;
  UDR0 = ch;
}

void tx_serial(const uint8_t * ptr, uint8_t sz){
  uart_init(true);
  while(sz--)
  {
    send(*ptr);
    ptr++;
  }
}

uint8_t * rx_serial(uint8_t limit)
{
  uart_init(false);
  while(sz <= limit)
  {
    while(!(UCSR0A & (1 << RXC0)));
    buffer[sz] = UDR0;
    sz++;
  }
  return (uint8_t *)buffer;
}

void send_slave_msg(uint8_t slave_hi, uint8_t slave_lo, uint8_t cmd_type)
{
  init_zb_tx_request(_p_zb_tx_req_arduino);
  _p_zb_tx_req_arduino->frame_id = 0x01;
  _p_zb_tx_req_arduino->address_64.address[1] = 0xFF;
  _p_zb_tx_req_arduino->address_64.address[0] = 0xFF;
  _p_zb_tx_req_arduino->address_16.address[1] = slave_hi;
  _p_zb_tx_req_arduino->address_16.address[0] = slave_lo;
  _p_zb_tx_req_arduino->options = 0x00;
  _p_zb_tx_req_arduino->data.data[0] = cmd_type ;
   Serial.begin(9600);
   uint8_t * payload = serial_payload(_p_zb_tx_req_arduino);
   uint8_t i = 0;
   while(i++ < 0x19)
   {
      Serial.println(*payload);
      payload++;
   }
   
   //tx_serial(serial_payload(_p_zb_tx_req));
}

void setup() 
{
  cli();
  pinMode(13, OUTPUT);
  delay(5000);              
  digitalWrite(13, LOW);
  delay(5000); 
  digitalWrite(13, HIGH);
  send_slave_msg(0xEA, 0x21, 0x09);

  
}

void loop()
{
  
  
  
}


