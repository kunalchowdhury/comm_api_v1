
#include xbee_message.h

#define F_CPU_1 16000000
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU_1 / (USART_BAUDRATE * 16UL))) - 1)
#define UART_SENDER_BUFFER_SZ 0x1F

uint8_t buffer[UART_SENDER_BUFFER_SZ];


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


void setup() 
{
  cli();
  pinMode(13, OUTPUT);
  delay(5000);              
  digitalWrite(13, LOW);
  delay(5000); 
  digitalWrite(13, HIGH);
  

  
}

void loop()
{
  
  
  
}
