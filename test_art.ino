#include <avr/io.h>

#define F_CPU_1 16000000
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU_1 / (USART_BAUDRATE * 16UL))) - 1)
#define UART_SENDER_BUFFER_SZ 0x1F

uint8_t buffer[UART_SENDER_BUFFER_SZ];

volatile uint8_t sz;

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


uint8_t arr[8] = {0x7E, 0x00, 0x04, 0x08, 0x52, 0x4D, 0x59, 0xFF};

uint8_t *rx_serial(uint8_t limit)
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


void setup() {
   pinMode(13, OUTPUT);
   delay(5000);              
   digitalWrite(13, LOW);
   delay(5000); 
   digitalWrite(13, HIGH);
   tx_serial(arr, 9);
   
   //sz = 0 ;
   rx_serial(8);
   Serial.begin(9600);
   // Serial.println("HELLO");
   //UCSR0B = ~(1 << TXEN0) & ~(1 << RXEN0);
   // UDR0 = 0x00;
   //uint8_t v =  (0x08 + 0x52 + 0x4D + 0x59) & 0xFF ;
   //uint8_t v1 = 0xFF - v ;
   //Serial.begin(9600);
   //Serial.flush();
   //for(int i = 0 ; i < UART_SENDER_BUFFER_SZ ; i++)
   //{
     // Serial.println(i, HEX);
    // Serial.println(buffer[i], HEX); 
   //}
    Serial.println(buffer[1], HEX);  
}

void loop() {
  // put your main code here, to run repeatedly:

}
